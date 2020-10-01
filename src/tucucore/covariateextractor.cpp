/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "tucucommon/general.h"
#include "tucucommon/loggerhelper.h"

#include "tucucore/covariateextractor.h"

namespace Tucuxi {
namespace Core {

using namespace Tucuxi::Common::Utils;

const std::string CovariateExtractor::BIRTHDATE_CNAME = "birthdate"; // NOLINT(readability-identifier-naming)

/// \brief Create an event and push it in the event series.
/// \param COV_DEF Corresponding covariate definition.
/// \param TIME Time of the event.
/// \param VALUE Observed value.
/// \param SERIES Series to push to.
#define PUSH_EVENT(COV_DEF, TIME, VALUE, SERIES)            \
    do {                                                    \
    std::shared_ptr<CovariateEvent> event =                 \
    std::make_shared<CovariateEvent>(COV_DEF, TIME, VALUE); \
    SERIES.push_back(*event);                               \
} while(0);

CovariateExtractor::CovariateExtractor(const CovariateDefinitions &_defaults,
                                       const PatientVariates &_patientCovariates,
                                       const DateTime &_start,
                                       const DateTime &_end)
    : ICovariateExtractor(_defaults, _patientCovariates, _start, _end),
      m_hasBirthDate{false}, m_initAgeInDays{-1.0}, m_initAgeInWeeks{-1.0}, m_initAgeInMonths{-1.0}, m_initAgeInYears{-1.0}
{
    // *** Verify preconditions ***
    // Invalid ptrs in covariate definitions.
    for (const auto &cd : m_defaults) {
        if (cd == nullptr) {
            throw std::runtime_error("[CovariateExtractor] Invalid pointer in covariate definitions");
        }
    }
    // Invalid ptrs in patient variates.
    for (const auto &pv : m_patientCovariates) {
        if (pv == nullptr) {
            throw std::runtime_error("[CovariateExtractor] Invalid pointer in patient covariates");
        }
    }
    // Start time past end time.
    if (m_start > m_end) {
        throw std::runtime_error("[CovariateExtractor] Invalid time interval specified");
    }

    // ** Fill internal structures with ID and position in vector, splitting between computed and not **
    // Push covariate definitions, splitting between computed and not.
    for (cdIterator_t it = m_defaults.begin(); it != m_defaults.end(); ++it) {
        std::pair<std::map<std::string, cdIterator_t>::iterator, bool> rc;
        if ((*it)->isComputed()) {
            rc = m_cdComputed.insert(std::pair<std::string, cdIterator_t>((*it)->getId(), it));
        } else {
            rc = m_cdValued.insert(std::pair<std::string, cdIterator_t>((*it)->getId(), it));
        }
        // In case on non-standard covariates set the initial values.
        if (((*it)->getType() != CovariateType::Standard) && ((*it)->getType() != CovariateType::Sex)) {
            switch ((*it)->getType()) {
            case CovariateType::AgeInDays:
                if (m_initAgeInDays >= 0) {
                    throw std::runtime_error("[CovariateExtractor] Too many AgeInDays-type covariates");
                }
                if ((*it)->getValue() < 0) {
                    throw std::runtime_error("[CovariateExtractor] Invalid default value ("
                                             + std::to_string((*it)->getValue())
                                             + ") for an age expressed in days.");
                }
                m_initAgeInDays = (*it)->getValue();
                break;
            case CovariateType::AgeInWeeks:
                if (m_initAgeInWeeks >= 0) {
                    throw std::runtime_error("[CovariateExtractor] Too many AgeInWeeks-type covariates");
                }
                if ((*it)->getValue() < 0) {
                    throw std::runtime_error("[CovariateExtractor] Invalid default value ("
                                             + std::to_string((*it)->getValue())
                                             + ") for an age expressed in weeks.");
                }
                m_initAgeInWeeks = (*it)->getValue();
                break;
            case CovariateType::AgeInMonths:
                if (m_initAgeInMonths >= 0) {
                    throw std::runtime_error("[CovariateExtractor] Too many AgeInMonths-type covariates");
                }
                if ((*it)->getValue() < 0) {
                    throw std::runtime_error("[CovariateExtractor] Invalid default value ("
                                             + std::to_string((*it)->getValue())
                                             + ") for an age expressed in months.");
                }
                m_initAgeInMonths = (*it)->getValue();
                break;
            case CovariateType::AgeInYears:
                if (m_initAgeInYears >= 0) {
                    throw std::runtime_error("[CovariateExtractor] Too many AgeInYears-type covariates");
                }
                if ((*it)->getValue() < 0) {
                    throw std::runtime_error("[CovariateExtractor] Invalid default value ("
                                             + std::to_string((*it)->getValue())
                                             + ") for an age expressed in years.");
                }
                m_initAgeInYears = (*it)->getValue();
                break;
            default:
                throw std::runtime_error("[CovariateExtractor] Invalid covariate type (" + std::to_string(static_cast<int>((*it)->getType())) + ")");
                break;
            }
        }

        if (rc.second == false) {
            // Duplicate ID!
            throw std::runtime_error("[CovariateExtractor] Duplicate covariate variable (" + (*it)->getId() + ")");
        }
    }

    // Push patient variates -- not computed by definition.
    for (pvIterator_t it = m_patientCovariates.begin(); it != m_patientCovariates.end(); ++it) {
        // If we cannot find a corresponding covariate definition or if it is a non-standard type, we can safely drop a
        // patient variate.
        if (m_cdValued.find((*it)->getId()) != m_cdValued.end() &&
                ((*m_cdValued.at((*it)->getId()))->getType() == CovariateType::Standard ||
                 (*m_cdValued.at((*it)->getId()))->getType() == CovariateType::Sex)) {
            if (m_pvValued.find((*it)->getId()) == m_pvValued.end()) {
                m_pvValued.insert(std::pair<std::string, std::vector<pvIterator_t>>((*it)->getId(),
                                                                                    std::vector<pvIterator_t>()));
            }
            m_pvValued.at((*it)->getId()).push_back(it);
        }

        if ((*it)->getId() == BIRTHDATE_CNAME) {
            if ((*it)->getDataType() == DataType::Date) {
                // We found a birthdate
                m_hasBirthDate = true;

                m_birthDate = (*it)->getValueAsDate();
            }
        }
    }
/*
    // If a birth date is present, then set it.
    if (m_cdValued.find(sm_birthDateCName) != m_cdValued.end()) {
        if ((*m_cdValued.at(sm_birthDateCName))->getDataType() != DataType::Date) {
            throw std::runtime_error("[CovariateExtractor] Invalid data type for birth date variable " + sm_birthDateCName);
        }
        m_birthDate = Tucuxi::Common::Utils::ValueToDate((*m_cdValued.at(sm_birthDateCName))->getValue());

        if (m_birthDate > m_start) {
            throw std::runtime_error("[CovariateExtractor] The birth date covariate (" + sm_birthDateCName
                                     + ") is past the starting date.");
        }

        m_hasBirthDate = true;
        // Remove the birth date from the map.
        m_cdValued.erase(sm_birthDateCName);
    }
    */
}


bool CovariateExtractor::computeEvents(const std::map<DateTime, std::vector<std::string>> &_refreshMap,
                                       CovariateSeries &_series)
{
    // Map holding the pointers to the events linked with covariates and their latest values.
    std::map<std::string, std::pair<std::shared_ptr<CovariateEvent>, Value>> cEventsMap;

    // For each date in the refresh map, set all the values appropriately and then (if needed) launch the computation
    // via the Operable Graph Manager.
    for (const auto &refreshEvent : _refreshMap) {
        const DateTime refreshTime = refreshEvent.first;
        std::vector<std::string> updatedCVs;
        // Look at every non-computed covariate we are supposed to refresh.
        for (const auto &cvName : refreshEvent.second) {
            Value newVal = 0.0;
            Common::UnitManager unitManager;
            if (((*m_cdValued.at(cvName))->getType() == CovariateType::Standard) || ((*m_cdValued.at(cvName))->getType() == CovariateType::Sex)) {
                // Standard covariate -- get its value from the Patient Variates or from default values.
                std::map<std::string, std::vector<pvIterator_t>>::iterator pvCurrentC = m_pvValued.find(cvName);
                if (pvCurrentC == m_pvValued.end()) {
                    // If no patient variates associated, take the default value.
                    newVal = (*m_cdValued.at(cvName))->getValue();
                } else if (pvCurrentC->second.size() == 1) {
                    // If single patient variate value, then take the first value.
                    newVal = stringToValue((*(pvCurrentC->second[0]))->getValue(),
                                           (*(pvCurrentC->second[0]))->getDataType());

                    newVal = unitManager.convertToUnit(newVal, (*(pvCurrentC->second[0]))->getUnit(), getFinalUnit(cvName));
                } else {
                    // We have more Patient Variates, therefore we have to compute the good value.
                    std::vector<pvIterator_t>::const_iterator pvIt = pvCurrentC->second.begin();

                    while (pvIt != pvCurrentC->second.end() && (**pvIt)->getEventTime() <= refreshTime) {
                        ++pvIt;
                    }

                    if (pvIt == pvCurrentC->second.end()) {
                        // We are past in time after the last measurement we have, so we have to keep the value.
                        newVal = stringToValue((**(std::prev(pvIt)))->getValue(),
                                               (**(std::prev(pvIt)))->getDataType());

                        newVal = unitManager.convertToUnit(newVal, (**(std::prev(pvIt)))->getUnit(), getFinalUnit(cvName));
                    } else if (pvIt == pvCurrentC->second.begin()) {
                        // The first measurement is already past the desired time -- we simply have to keep the level.
                        newVal = stringToValue((**(pvIt))->getValue(),
                                               (**(pvIt))->getDataType());

                        newVal = unitManager.convertToUnit(newVal, (*(pvCurrentC->second[0]))->getUnit(), getFinalUnit(cvName));

                    } else {
                        // We have two measurements to interpolate from (we have dealt with the case of the first
                        // measurement being past the desired time, so we can safely assume we have a previous
                        // measurement.
                        Value firstValue = stringToValue((**(std::prev(pvIt)))->getValue(),
                                                         (**(std::prev(pvIt)))->getDataType());

                        Value secondValue = stringToValue((**(pvIt))->getValue(),
                                                          (**(pvIt))->getDataType());

                        if (!interpolateValues(unitManager.convertToUnit(
                                                   firstValue,
                                                   (**(std::prev(pvIt)))->getUnit(),
                                                   getFinalUnit(cvName)),                           // val1
                                               (**(std::prev(pvIt)))->getEventTime(),               // date1
                                               unitManager.convertToUnit(
                                                   secondValue,
                                                   (**(pvIt))->getUnit(),
                                                   getFinalUnit(cvName)),                           // val2
                                               (**(pvIt))->getEventTime(),                          // date2
                                               refreshTime,                                         // dateRes
                                               (*m_cdValued.at(cvName))->getInterpolationType(),    // interpolationType
                                               newVal)) {                                           // valRes
                            return false;
                        }
                    }
                }
            } else {
                // Age covariate -- compute its value from either the birth date (if present) or the default values.

                // If we have a birth date, then everything is referred to it. Otherwise, do computations relatives to
                // the start of the period, and then add the initial default values.
                DateTime tmpBirthDate = m_hasBirthDate ? m_birthDate : m_start;

                switch ((*m_cdValued.at(cvName))->getType()) {
                case CovariateType::AgeInDays:
                    newVal = static_cast<double>(dateDiffInDays(refreshTime, tmpBirthDate));
                    newVal += m_hasBirthDate ? 0 : m_initAgeInDays;
                    break;
                case CovariateType::AgeInWeeks:
                    newVal = static_cast<double>(dateDiffInWeeks(refreshTime, tmpBirthDate));
                    newVal += m_hasBirthDate ? 0 : m_initAgeInWeeks;
                    break;
                case CovariateType::AgeInMonths:
                    newVal = static_cast<double>(dateDiffInMonths(refreshTime, tmpBirthDate));
                    newVal += m_hasBirthDate ? 0 : m_initAgeInMonths;
                    break;
                case CovariateType::AgeInYears:
                    newVal = static_cast<double>(dateDiffInYears(refreshTime, tmpBirthDate));
                    newVal += m_hasBirthDate ? 0 : m_initAgeInYears;
                    break;
                default:
                    // Cannot get here.
                    break;
                }
            }

            // Now check if this update changed the covariate's value, and if it is the case then update the OGM.
            if (cEventsMap.count(cvName) == 0 || cEventsMap.at(cvName).second != newVal) {
                // Add the covariate to the list of changed values.
                updatedCVs.push_back(cvName);
                // If no event related to this covariate is present in the covariate graph, then add it, otherwise
                // just update the stored value.
                if (!m_ogm.isInputPresent(cvName)) {
                    // Create the event.
                    std::shared_ptr<CovariateEvent> event =
                            std::make_shared<CovariateEvent>(**m_cdValued.at(cvName), refreshTime, newVal);
                    cEventsMap.insert(std::make_pair(cvName, std::make_pair(event, newVal)));
                    m_ogm.registerInput(event, cvName);
                } else {
                    (cEventsMap.at(cvName).first)->setValue(newVal);
                }

                // Update the event date.
                (cEventsMap.at(cvName).first)->setEventTime(refreshTime);

                // Add copy of an event to the list of events to return.
                _series.push_back(*(cEventsMap.at(cvName).first));

                // Update the event map.
                cEventsMap.at(cvName).second = newVal;
            }
        }

        if ((!m_cdComputed.empty()) && (!updatedCVs.empty())) {
            // If we are at start, we still have to create the events associated with computed CVs and push them in the
            // OGM!
            if (refreshTime == m_start) {
                for (const auto &cvm : m_cdComputed) {
                    std::shared_ptr<CovariateEvent> event = std::make_shared<CovariateEvent>(**(cvm.second),
                                                                                             m_start,
                                                                                             0.0);
                    cEventsMap.insert(std::make_pair(cvm.first, std::make_pair(event, 0.0)));
                    m_ogm.registerOperable(event, cvm.first);
                }
            }

            // We have a change in at least a value and some of the values are computed:
            // -> we need to trigger a recomputation, and update the values that changed.
            if (!m_ogm.evaluate()) {
                // Something went wrong with the evaluation
                return false;
            }

            // If we are at the first iteration, then simply push all events, otherwise check those whose value has been
            // updated.
            for (auto &cvm : m_cdComputed) {
                double cvVal;
                if (!m_ogm.getValue(cvm.first, cvVal)) {
                    return false;
                }
                // We either have no events associated or the newly-computed value differ from the previous one,
                // therefore we update the stored value and create an associated event.
                if (refreshTime == m_start || cEventsMap.at(cvm.first).second != cvVal) {
                    (cEventsMap.at(cvm.first).first)->setValue(cvVal);
                    (cEventsMap.at(cvm.first).first)->setEventTime(refreshTime);
                    _series.push_back(*(cEventsMap.at(cvm.first).first));
                    cEventsMap.at(cvm.first).second = cvVal;
                }
            }
        }
    }
    return true;
}


ComputingStatus CovariateExtractor::extract(CovariateSeries &_series)
{
    bool rc;

    // Patient Variates have to be sorted by date (dropping all non-interesting ones).
    sortPatientVariates();

    // Collect refresh intervals. For each date, we can have multiple covariates that want to be refreshed.
    std::map<DateTime, std::vector<std::string>> refreshMap;
    collectRefreshIntervals(refreshMap);

    TUCU_TRY {
         rc = computeEvents(refreshMap, _series);
    } TUCU_CATCH (std::invalid_argument e) TUCU_ONEXCEPTION( {

        Tucuxi::Common::LoggerHelper logHelper;
        logHelper.error("Error with covariate extraction : {}", e.what());
        logHelper.error("If error due to conversion, please check Query and DrugModel units");
        return ComputingStatus::CovariateExtractionError;
    });

    if (!rc) {
        Tucuxi::Common::LoggerHelper logHelper;
        logHelper.error("Error with covariate extraction. It be caused by missing covariates used in a priori computations");
        return ComputingStatus::CovariateExtractionError;
    }

    return ComputingStatus::Ok;
}


void CovariateExtractor::collectRefreshIntervals(std::map<DateTime, std::vector<std::string>> &_refreshMap)
{
    for (const auto &cdv : m_cdValued) {
        Duration refreshInterval;
        refreshInterval = (*(cdv.second))->getRefreshPeriod();
        if (((*(cdv.second))->getInterpolationType() != InterpolationType::Direct) && (!refreshInterval.isEmpty())) {
            // When we have a refresh interval set, use it and ignore the patient variates.
            for (DateTime t = m_start; t <= m_end; t += refreshInterval) {
                if (_refreshMap.find(t) == _refreshMap.end()) {
                    _refreshMap.insert(std::pair<DateTime, std::vector<std::string>>(t, std::vector<std::string>()));
                }
                _refreshMap.at(t).push_back(cdv.first);
            }
        } else {
            // No refresh interval set, so we get the time instants given by patient variates:
            // # standard covariates #
            // - if no patient variates, then just add start (the default value won't change afterwards).
            // - if only a single value, then move it to the start.
            // - if more values, then add start (if not already there) and those in the (start, end] interval.
            // # ages #
            // - if a covariate named m_birthDateCName is present, then use it to set the refresh dates.
            // - else use the start interval and the type to set them.
            if (((*(cdv.second))->getType() == CovariateType::Standard) || ((*(cdv.second))->getType() == CovariateType::Sex)) {
                DateTime t = m_start;
                if (_refreshMap.find(t) == _refreshMap.end()) {
                    _refreshMap.insert(std::pair<DateTime, std::vector<std::string>>(t, std::vector<std::string>()));
                }
                _refreshMap.at(t).push_back(cdv.first);
                if (m_pvValued.count(cdv.first) > 0) {
                    const std::vector<pvIterator_t> &pvValues = m_pvValued.at(cdv.first);
                    if (pvValues.size() > 1) {
                        for (const auto &pv : pvValues) {
                            DateTime t = (*pv)->getEventTime();
                            if (t > m_start && t <= m_end) {
                                if (_refreshMap.find(t) == _refreshMap.end()) {
                                    _refreshMap.insert(std::pair<DateTime, std::vector<std::string>>(t, std::vector<std::string>()));
                                }
                                _refreshMap.at(t).push_back(cdv.first);
                            }
                        }
                    }
                }
            } else {
                DateTime startDate = m_start;
                if (m_hasBirthDate) {
                    startDate = m_birthDate;
                }

                DateTime t = startDate;
                while (t <= m_end) {
                    if (t >= m_start) {
                        if (_refreshMap.find(t) == _refreshMap.end()) {
                            _refreshMap.insert(std::pair<DateTime, std::vector<std::string>>(t, std::vector<std::string>()));
                        }
                        _refreshMap.at(t).push_back(cdv.first);
                    }
                    switch ((*(cdv.second))->getType()) {
                    case CovariateType::AgeInDays:
                        t.addDays(1);
                        break;
                    case CovariateType::AgeInWeeks:
                        t.addDays(7);
                        break;
                    case CovariateType::AgeInMonths:
                        t.addMonths(1);
                        break;
                    case CovariateType::AgeInYears:
                        t.addYears(1);
                        break;
                    default:
                        break;
                    }
                }

                // The age has still to be computed at the start time, so we add it there if this is not the case.
                if (_refreshMap.find(m_start) == _refreshMap.end()) {
                    _refreshMap.insert(std::pair<DateTime, std::vector<std::string>>(m_start, std::vector<std::string>()));
                }
                if (std::find(_refreshMap.at(m_start).begin(), _refreshMap.at(m_start).end(), cdv.first) == _refreshMap.at(m_start).end()) {
                    _refreshMap.at(m_start).push_back(cdv.first);
                }
            }
        }
    }

    // Now iterate on the produced refresh intervals, and add non-directly interpolated values to each interval if
    // absent. We do this because we must have the best guess for each value at the time of the computation of new
    // values, and if the values is interpolated, so be it. This should have no impact on the computations performed,
    // since in any case we would have to redo the computations for the given time instant.
    for (const auto &cv : m_cdValued) {
        if ((*(cv.second))->getInterpolationType() != InterpolationType::Direct) {
            for (auto &t : _refreshMap) {
                if (std::find(t.second.begin(), t.second.end(), cv.first) == t.second.end()) {
                    t.second.push_back(cv.first);
                }
            }
        }
    }
}


bool CovariateExtractor::interpolateValues(const Value _val1, const DateTime &_date1,
                                           const Value _val2, const DateTime &_date2,
                                           const DateTime &_dateRes,
                                           const InterpolationType _interpolationType,
                                           Value &_valRes) const
{
    // Check precondition: dates are sorted.
    if (_date2 < _date1) {
        return false;
    }

    // If the two values are at the same time, they must coincide. If this is the case, then the value will be the
    // interpolated value, otherwise signal failure.
    if (fabs(_date1.toSeconds() - _date2.toSeconds()) < 1e-6) {
        if (fabs(_val1 - _val2) < 1e-6) {
            _valRes = _val1;
            return true;
        } else {
            return false;
        }
    }

    // If the result's date corresponds to either date1 or date2, then return the corresponding value immediately.
    if (_dateRes == _date1) {
        _valRes = _val1;
        return true;
    }
    if (_dateRes == _date2) {
        _valRes = _val2;
        return true;
    }

    switch (_interpolationType) {
    case InterpolationType::Direct:
        if (_date2 > _dateRes) {
            // Extend _date1 to _dateRes.
            _valRes = _val1;
        } else {
            // Extend _val2 past _date2.
            _valRes = _val2;
        }
        break;
    case InterpolationType::Linear:
        // Angular coefficient.
        Value m;
        m = (_val2 - _val1) / (_date2.toSeconds() - _date1.toSeconds());
        // Intercept.
        Value b;
        b = _val2 - m * _date2.toSeconds();
        _valRes = _dateRes.toSeconds() * m + b;
        break;
    default:
        // Other interpolation types not yet implemented.
        return false;
    }
    return true;
}


void CovariateExtractor::sortPatientVariates()
{
    // For each patient variate, sort by date the list of values, then discard those not of interest.
    for (auto &pvV : m_pvValued) {
        // Sort by increasing date.
        std::sort(pvV.second.begin(), pvV.second.end(),
                  [](pvIterator_t &_a, pvIterator_t &_b) { return (*_a)->getEventTime() < (*_b)->getEventTime(); });

        // If just one value, keep it, else do a cleanup.
        if (pvV.second.size() > 1) {
            // Drop all values before m_start and past m_end, except the ones on the border (we will use them for the
            // interpolation).
            std::vector<pvIterator_t>::iterator it = pvV.second.begin();
            // Remove before m_start.
            while (std::next(it) != pvV.second.end() && (**it)->getEventTime() < m_start) {
                if ((**(std::next(it)))->getEventTime() >= m_start && it != pvV.second.begin()) {
                    pvV.second.erase(pvV.second.begin(), it);
                    break;
                }
                ++it;
            }
            // Remove past m_end.
            it = pvV.second.begin();
            while (std::next(it) != pvV.second.end() && (**it)->getEventTime() < m_end) {
                if ((**(std::next(it)))->getEventTime() >= m_end) {
                    ++it;
                    if (std::next(it) != pvV.second.end()) {
                        pvV.second.erase(std::next(it), pvV.second.end());
                    }
                    break;
                }
                ++it;
            }
        }

        // If InterpolationType == InterpolationType::Direct or a single value is present, then no interpolation is
        // performed and we can change the time of the first measurement with the initial interval time.
        if ((*(m_cdValued.at(pvV.first)))->getInterpolationType() == InterpolationType::Direct ||
                pvV.second.size() == 1) {
            (*(pvV.second[0]))->setEventTime(m_start);
        }
    }
}

const Unit CovariateExtractor::getFinalUnit(const std::string &_cvName) const{
    for(const auto &covInDM : m_defaults){
        if(_cvName == covInDM->getId()){
            return covInDM->getUnit();
        }
    }
    //CAN'T GO THERE
    return Unit("");
}

} // namespace Core
} // namespace Tucuxi
