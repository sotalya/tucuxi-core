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
        // Apply etas if available
        if (_etas.size() > 0) {
            // Make a copy to hold values with applied etas
            ParameterSetEvent *pParameters = new ParameterSetEvent(*it);
            // Apply etas
            pParameters->applyEtas(_etas);
            return ParameterSetEventPtr(pParameters);
        }

        // YTA: I do a copy of the parameters, else there is a segmentation fault later on.
        // I guess this is related to the use of unique_ptr<>()
        ParameterSetEvent *pParameters = new ParameterSetEvent(*it);
        return ParameterSetEventPtr(pParameters);
      //  return ParameterSetEventPtr(&(*it));
    }

    return nullptr;
}

void ParameterSetSeries::addParameterSetEvent(ParameterSetEvent parameterSetEvent)
{
    m_parameterSets.push_back(parameterSetEvent);
}

void ParameterSetEvent::applyEtas(const Etas& _etas)
{
    unsigned int k = 0;
    Parameters::iterator it;
    for (it = m_parameters.begin(); it != m_parameters.end(); it++) {
        if (it->isVariable()) {
            it->applyEta(_etas.at(k));
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
        switch (m_definition.getVariability().m_variabilityType) {
            case ParameterVariabilityType::Additive:
                m_value = m_value + _eta;
                break;
            case ParameterVariabilityType::Exponential:
                m_value = m_value * exp(_eta);
                break;
            case ParameterVariabilityType::Proportional:
                m_value = m_value * (1 + _eta);
                break;
            default: {
                Tucuxi::Common::LoggerHelper logger;
                logger.warn("Parameter {} has an unknown error model", m_definition.getName());
                break;
            }
        }
        if (m_value <= 0.0) {
            m_value = 0.00000001;
            Tucuxi::Common::LoggerHelper logger;
            logger.warn("Parameter {} is negative", m_definition.getName());
        }
        else if (std::isinf(m_value)) {
            m_value = std::numeric_limits<double>::max();
            Tucuxi::Common::LoggerHelper logger;
            logger.warn("Parameter {} is infinite", m_definition.getName());
        }
        else if (std::isnan(m_value)) {
            Tucuxi::Common::LoggerHelper logger;
            logger.warn("Parameter {} is not a number", m_definition.getName());
        }
    }
}


}
}
