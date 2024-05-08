//@@license@@

#include <utility>

#include "tucucore/parametersextractor.h"

#include "tucucommon/loggerhelper.h"

#include "tucucore/covariateevent.h"

using namespace std::chrono_literals;

namespace Tucuxi {
namespace Core {

ParametersExtractor::ParametersExtractor(
        const CovariateSeries& _covariates,
        Tucuxi::Common::Iterator<const ParameterDefinition*>& _paramsIterator,
        DateTime _start,
        DateTime _end)
    : m_paramsIterator{_paramsIterator}, m_start{_start}, m_end{_end}
{
    // Check that start time is past end time.
    if (m_start > m_end) {
        throw std::runtime_error("[ParametersExtractor] Invalid time interval specified");
    }

    // Create a list of time instants at which to compute the parameters, together with the covariate values that have
    // to be set at those instants.
    //std::cout << "New" << std::endl;
    for (const auto& cv : _covariates) {
        const DateTime dt = cv.getEventTime();


        //std::cout << dt.month() << " " << dt.day() << " " << dt.hour() << " " << dt.minute() << std::endl;

        //if (dt <= m_end) {
        //    std::cout << "In" << std::endl;
        //}
        //else {
        //    std::cout << "Out" << std::endl;
        //}

        // Covariate events past _end are discarded.
        if (dt <= m_end) {
            // Allocate the vector if not yet present at the given time instant.
            if (m_timedCValues.find(dt) == m_timedCValues.end()) {
                m_timedCValues.insert(std::make_pair(dt, std::vector<std::pair<std::string, Value>>()));
            }
            m_timedCValues.at(dt).push_back(std::make_pair(cv.getId(), cv.getValue()));
        }
    }

    // Allocate the vector at m_start if no event present. Indeed, even if no covariate plays an influence on the
    // parameters, the parameters have to be determined at m_start.
    if (m_timedCValues.empty()) {
        m_timedCValues.insert(std::make_pair(m_start, std::vector<std::pair<std::string, Value>>()));
    }

    // std::maps are sorted on the key value, so we can safely assume that the first value is the first time interval
    // considered.
    std::vector<DateTime> toErase;
    DateTime firstEvTime = m_timedCValues.begin()->first;
    if (firstEvTime < m_start) {
        // If we have values before m_start, we want m_start to be present to catch their first initializations.
        m_timedCValues.insert(std::make_pair(m_start, std::vector<std::pair<std::string, Value>>()));

        // This is the vector we seek to fill with the first values of the covariate events.
        std::vector<std::pair<std::string, Value>>& startCVs = m_timedCValues.at(m_start);
        // Propagate forward the covariates whose events happen before _start.
        for (auto& tcVec : m_timedCValues) {
            if (tcVec.first >= m_start) {
                // We can stop as soon as we reach m_start.
                break;
            }
            for (auto& tcVal : tcVec.second) {
                // If the covariate is not present, then simply add it. If it is present, then change its value to a
                // more up-to-date one.
                auto it = std::find_if(
                        startCVs.begin(), startCVs.end(), [tcVal](const std::pair<std::string, Value>& _event) {
                            return _event.first == tcVal.first;
                        });
                if (it == startCVs.end()) {
                    startCVs.push_back(tcVal);
                }
                else {
                    it->second = tcVal.second;
                }
            }
            // Add the time instant to the list of time instants to remove from the map.
            toErase.push_back(tcVec.first);
        }
    }
    // Clean up the set of time intervals.
    for (const auto& rm : toErase) {
        m_timedCValues.erase(rm);
    }

    if (!m_timedCValues.empty()) {
        // Check for covariates that appear out of nowhere past the start time -- if any other covariate was present
        // before, then that was the initial time of parameters computation and all the covariates must be defined at
        // that moment.
        std::vector<std::string> knownCovariates;
        for (const auto& startCV : (m_timedCValues.begin())->second) {
            knownCovariates.push_back(startCV.first);
        }

        // First vector checked twice, but the algorithm looks more clear in this way.
        for (auto& tcVec : m_timedCValues) {
            for (auto& tcVal : tcVec.second) {
                if (std::find(knownCovariates.begin(), knownCovariates.end(), tcVal.first) == knownCovariates.end()) {
                    throw std::runtime_error(
                            "[ParametersExtractor] Covariate event for " + tcVal.first + " comes out blue sky!");
                }
            }
            // Since we are already passing on individual dates, check that the covariates have no duplicate definition
            // (that is, for a given time instant, two possible values).
            std::sort(
                    tcVec.second.begin(),
                    tcVec.second.end(),
                    [](const std::pair<std::string, Value>& _a, const std::pair<std::string, Value>& _b) {
                        return _a.first < _b.first;
                    });
            // We consider wrong to have two covariate events referring to the same covariate at the same time instant
            // -- even if the value is the same, as this is possibly a mistake of the algorithm that extracted covariate
            // events.
            if (std::adjacent_find(
                        tcVec.second.begin(),
                        tcVec.second.end(),
                        [](const std::pair<std::string, Value>& _a, const std::pair<std::string, Value>& _b) {
                            return _a.first == _b.first;
                        })
                != tcVec.second.end()) {
                throw std::runtime_error("[ParametersExtractor] Duplicate values for the same covariate event.");
            }
        }
    }

    // Add default values of all parameters
    m_paramsIterator.reset();
    while (!m_paramsIterator.isDone()) {
        std::shared_ptr<ParameterEvent> event =
                std::make_shared<ParameterEvent>(ParameterEvent(**m_paramsIterator, (*m_paramsIterator)->getValue()));
        m_ogm.registerInput(event, (*m_paramsIterator)->getId() + "_population");
        m_paramsIterator.next();
    }
}


ComputingStatus ParametersExtractor::extract(ParameterSetSeries& _series)
{
    std::vector<std::string> covariateIds;
    bool firstIteration = true;

    // Map containing the computed parameters along with their latest value.
    std::map<std::string, std::pair<const ParameterDefinition*, Value>> cParamMap;

    // Map linking covariate events with their representative inside the OGM.
    std::map<std::string, std::shared_ptr<CovariateEvent>> cEvMap;

    // Iterate on the time instants and generate the parameter events. Particular handling is required by the events at
    // the beginning of the considered interval, but it is easier to do this inside the loop.
    for (const auto& tcv : m_timedCValues) {
        ParameterSetEvent pSetEvent(tcv.first);

        if (tcv.first == m_timedCValues.begin()->first) {
            // Generate all events related to non-computed parameters, while adding computed ones as operables in the
            // OGM.
            m_paramsIterator.reset();
            while (!m_paramsIterator.isDone()) {
                if (!(*m_paramsIterator)->isComputed()) {
                    // Add the parameter to the event set without change.
                    pSetEvent.addParameterEvent(**m_paramsIterator, (*m_paramsIterator)->getValue());
                }
                else {
                    // Add the parameter to the OGM for later computation of its value. Also, add its name to a map that
                    // keeps track of the computed parameters and their values.
                    std::shared_ptr<ParameterEvent> event =
                            std::make_shared<ParameterEvent>(ParameterEvent(**m_paramsIterator, 0.0));
                    m_ogm.registerOperable(event, (*m_paramsIterator)->getId());

                    cParamMap.insert(
                            std::make_pair((*m_paramsIterator)->getId(), std::make_pair(*m_paramsIterator, 0.0)));
                }
                m_paramsIterator.next();
            }
            // Add all covariates at the first time instant as inputs of the OGM.
            for (const auto& cv : tcv.second) {

                if (firstIteration) {
                    covariateIds.push_back(cv.first);
                }

                // Create ex-novo a fake covariate.
                std::shared_ptr<CovariateEvent> event = std::make_shared<CovariateEvent>(CovariateEvent(
                        CovariateDefinition(cv.first, Tucuxi::Common::Utils::varToString(cv.second), nullptr),
                        tcv.first,
                        cv.second));

                // std::cout << "Covariate " << cv.first << " : " << cv.second << std::endl;
                m_ogm.registerInput(event, cv.first);
                cEvMap.insert(std::make_pair(cv.first, event));
            }

            firstIteration = false;
        }
        else {
            // Just set the covariate values that are scheduled at this time instant.
            for (const auto& cv : tcv.second) {
                // std::cout << "Covariate " << cv.first << " : " << cv.second << std::endl;
                cEvMap.at(cv.first)->setValue(cv.second);
            }
        }

        // Update computed values in the OGM.
        if (!m_ogm.evaluate()) {
            // Something went wrong
            // TODO : Log something somewhere
            Tucuxi::Common::LoggerHelper logger;
            logger.error("Error with the a priori computation of parameters");



            return ComputingStatus::ParameterExtractionError;
        }

        // Retrieve updated values.
        Value newVal = 0.0;
        for (auto& cp : cParamMap) {
            bool rc = m_ogm.getValue(cp.first, newVal);
            if (!rc) {
                return ComputingStatus::ParameterExtractionError;
            }

            if (newVal != cp.second.second || tcv.first == m_timedCValues.begin()->first) {
                // The value has changed or we are at the first time instant, therefore update the stored values and
                // generate the parameter event.
                pSetEvent.addParameterEvent(*cp.second.first, newVal);
                cp.second.second = newVal;
            }
        }

        std::vector<SimpleCovariate> covariateValues;

        // Get the covariate values used here:
        for (const auto& cov : covariateIds) {
            double value = 0.0;
            m_ogm.getValue(cov, value);
            covariateValues.push_back({cov, value});
        }
        pSetEvent.m_covariates = covariateValues;


        // Add the parameter set event to the series of events.
        _series.addParameterSetEvent(pSetEvent);
    }

    return ComputingStatus::Ok;
}


ComputingStatus ParametersExtractor::buildFullSet(
        const ParameterSetSeries& _inputSeries, ParameterSetSeries& _outputSeries) const
{
    // Start with the first set of parameters (it should contain the full set)
    ParameterSetEvent current(_inputSeries.m_parameterSets[0]);
    _outputSeries.addParameterSetEvent(current);

    // Then iterate over the modifications
    for (size_t i = 1; i < _inputSeries.m_parameterSets.size(); i++) {

        current.setEventTime(_inputSeries.m_parameterSets[i].getEventTime());

        // And for each event, update the parameters that change at that time
        auto it = _inputSeries.m_parameterSets[i].begin();
        while (it != _inputSeries.m_parameterSets[i].end()) {
            current.addParameterEvent((*it).getDefinition(), (*it).getValue());
            it++;
        }

        current.m_covariates = _inputSeries.m_parameterSets[i].m_covariates;

        // Add the new event to the output series
        _outputSeries.addParameterSetEvent(current);
    }
    return ComputingStatus::Ok;
}


ComputingStatus ParametersExtractor::extractPopulation(ParameterSetSeries& _series)
{
    // Parameters valid since the beginning of time
    ParameterSetEvent pSetEvent(DateTime::minimumDateTime()); // (Tucuxi::Common::Duration(0h)));
    // Add default values of all parameters
    m_paramsIterator.reset();
    while (!m_paramsIterator.isDone()) {
        pSetEvent.addParameterEvent(**m_paramsIterator, (*m_paramsIterator)->getValue());
        m_paramsIterator.next();
    }


    // Add the parameter set event to the series of events.
    _series.addParameterSetEvent(pSetEvent);
    return ComputingStatus::Ok;
}

} // namespace Core
} // namespace Tucuxi
