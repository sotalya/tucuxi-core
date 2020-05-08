/*
* Copyright (C) 2017 Tucuxi SA
*/

#include <memory>

#include "tucucommon/general.h"
#include "tucucommon/loggerhelper.h"
#include "tucucore/parameter.h"

namespace Tucuxi {
namespace Core {


ParameterSetEventPtr ParameterSetSeries::getAtTime(const DateTime &_date, const Etas &_etas) const
{
    // Find the lasted change that occured before the given date
    std::vector<ParameterSetEvent>::const_iterator it = m_parameterSets.begin();
    if (it != m_parameterSets.end()) {
        std::vector<ParameterSetEvent>::const_iterator itNext = it;
        while (++itNext != m_parameterSets.end() && _date > itNext->getEventTime()) {
            it++;
        }
    }

    // Did we find something?
    if (it != m_parameterSets.end())
    {
        // Make a copy to hold values with applied etas
        ParameterSetEvent *pParameters = new ParameterSetEvent(*it);

        // Apply etas if available
        if (!_etas.empty()) {
            pParameters->applyEtas(_etas);
        }

        return ParameterSetEventPtr(pParameters);
    }

    return nullptr;
}

void ParameterSetSeries::addParameterSetEvent(const ParameterSetEvent &_parameterSetEvent)
{
    m_parameterSets.push_back(_parameterSetEvent);
}

void ParameterSetEvent::addParameterEvent(const ParameterDefinition &_definition, Value _value)
{
    // This method ensures the parameters are always sorted:
    // variable parameters first, then fixed parameters.
    // Within a category, alphabetical order is applied.
    size_t insertIndex = 0;

    // Will be true if we update a parameter instead of inserting it
    bool updatingParameter = false;

    auto it = m_parameters.begin();

    if (m_parameters.empty()) {
        m_parameters.push_back(Parameter(_definition, _value));
    }
    else {

        for (it = m_parameters.begin(); it != m_parameters.end(); it++) {
            if (_definition.getId() == it->getParameterId()) {
                // We update the existing one with the new value
                updatingParameter = true;
                break;
            }
            if (_definition.isVariable() && !it->isVariable()) {
                break;
            }
            if (!_definition.isVariable() && it->isVariable()) {
                insertIndex ++;
                continue;
            }
            if (_definition.getId() < it->getParameterId()) {
                break;
            }

            insertIndex ++;
        }

        Parameters oldParams;
        for (it = m_parameters.begin(); it != m_parameters.end(); it++) {
            oldParams.push_back(Parameter(*it));
        }

        m_parameters.clear();

        size_t pIndex = 0;
        for (it = oldParams.begin(); it != oldParams.end(); it++) {
            if (insertIndex == pIndex) {
                m_parameters.push_back(Parameter(_definition, _value));
                if (!updatingParameter) {
                    m_parameters.push_back(Parameter(*it));
                }
            }
            else {
                m_parameters.push_back(Parameter(*it));
            }
            pIndex ++;
        }
        if (insertIndex == oldParams.size()) {
            m_parameters.push_back(Parameter(_definition, _value));
        }
    }

    // Update index

    int index = 0;
    for (it = m_parameters.begin(); it != m_parameters.end(); it++) {

        // Update our mapping between id (string) to index
        ParameterId::Enum id = ParameterId::fromString(it->getParameterId());
        m_IdToIndex[id] = index;

        index++;
    }

}

void ParameterSetEvent::applyEtas(const Etas& _etas)
{
    unsigned int k = 0;
    Parameters::iterator it;
    for (it = m_parameters.begin(); it != m_parameters.end(); it++) {
        if (it->isVariable()) {
            it->applyEta(_etas[k]);
            k++;
        }
    }
    if (_etas.size() != k) {
        Tucuxi::Common::LoggerHelper logger;
        logger.error("The eta vector does not fit the variable parameters size.");
    }
}

void Parameter::applyEta(Deviation _eta)
{
    if (m_definition.isVariable()) {
        switch (m_definition.getVariability().getType()) {
        case ParameterVariabilityType::Additive:
            m_value = m_value + _eta;
            break;
        case ParameterVariabilityType::Normal:
            m_value = m_value + _eta;
            break;
        case ParameterVariabilityType::Exponential:
            m_value = m_value * exp(_eta);
            break;
        case ParameterVariabilityType::LogNormal:
            m_value = m_value * exp(_eta);
            break;
        case ParameterVariabilityType::Proportional:
            // Actually if _eta is less than -1, then there is an issue
            m_value = m_value * (1 + _eta);
            break;
        default: {
            Tucuxi::Common::LoggerHelper logger;
            logger.warn("Parameter {} has an unknown error model", m_definition.getId());
            break;
        }
        }
        if (m_value <= 0.0) {
            m_value = 0.00000001;
            Tucuxi::Common::LoggerHelper logger;
            //logger.warn("Applying Eta to Parameter {} makes it negative", m_definition.getId());
        }
        else if (std::isinf(m_value)) {
            m_value = std::numeric_limits<double>::max();
            Tucuxi::Common::LoggerHelper logger;
            logger.warn("Applying Eta to Parameter {} makes it infinite", m_definition.getId());
        }
        else if (std::isnan(m_value)) {
            Tucuxi::Common::LoggerHelper logger;
            logger.warn("Applying Eta to Parameter {} makes it not a number", m_definition.getId());
        }
    }
}


} // namespace Core
} // namespace Tucuxi
