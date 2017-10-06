/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "tucucommon/general.h"
#include "tucucore/covariateextractor.h"

namespace Tucuxi {
namespace Core {

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

int CovariateExtractor::addPatientVariateWithRefresh(std::map<std::string, std::pair<std::shared_ptr<CovariateEvent>, Value>> &_computedValuesMap,
                                                     std::map<std::string, std::shared_ptr<CovariateEvent>> &_nccValuesMap,
                                                     CovariateSeries &_series)
{
    for (const auto &pvMap : m_pvValued) {
        const std::string &pvName = pvMap.first;
        const std::vector<pvIterator_t> &pvValues = pvMap.second;
        Duration refreshPeriod = (*m_cdValued.at(pvName))->getRefreshPeriod();
        if (!refreshPeriod.isEmpty()) {

            // We need to compute the interpolated values and push them at the appropriate time.
            // The "appropriate time" is represented by the initial time plus the refresh period, therefore we have
            // to interpolate the correct values for each refresh period.
            std::vector<pvIterator_t>::const_iterator pvIt = pvValues.begin();
            double newVal = 0.0;
            // Use the first observed value for the whole period up to it, and the last observed value for the whole
            // period after it.
            // Example:
            //
            // |------1----2---3--------|
            // |----a----b----c----d----|
            //
            // where a, b, c, and d are time intervals given by the refresh period, and 1, 2, and 3 are
            // measurements, gives:
            // * a == 1
            // * b == interp(1, 2)
            // * c == interp(2, 3)
            // * d == 3
            // If we had a measurement before the start of the interval, we would have kept it in the vector and
            // thus we would interpolate and not set a fixed value. The same reasoning applies if we had a
            // measurement after the end of the interval.
            for (auto timeIt = m_start + refreshPeriod; timeIt <= m_end; timeIt += refreshPeriod) {

                std::cerr << "** TIME: " << timeIt << " **\n";

                if (timeIt <= (**(pvValues.begin()))->getEventTime()) {

                    std::cerr << "\tWe are before the time of the first measurement ("
                              << ((**(pvValues.begin()))->getEventTime())
                              << "), we SKIP the measurement to avoid duplicata.";
                    //                    std::cerr << "), we we use the value of the first measurement = "
                    //                              << (**(pvMap.second.begin()))->getValue() << "\n";

                    //                    // Before first measurement use the value of the first measurement.
                    //                    newVal = stringToValue((**(pvMap.second.begin()))->getValue(),
                    //                                           (**(pvMap.second.begin()))->getDataType());
                } else {
                    // Advance until we pass the measurement just before the chosen time, or until we get to the
                    // last available measurement.
                    while (pvIt != pvValues.end() && (**pvIt)->getEventTime() <= timeIt) {

                        std::cerr << "\tCurrent event time: "<< ((**pvIt)->getEventTime()) << ", advancing...\n";

                        ++pvIt;
                    }

                    std::cerr << "DONE!\n";

                    if (pvIt == pvValues.end()) {

                        std::cerr << "We are past in time after the last measurement we have, so we have to keep the value = "
                                  << (**(std::prev(pvIt)))->getValue() << "\n";

                        // We are past in time after the last measurement we have, so we have to keep the value.
                        newVal = stringToValue((**(std::prev(pvIt)))->getValue(),
                                               (**(std::prev(pvIt)))->getDataType());
                    } else {
                        // We have two measurements to interpolate from (we have dealt with the case of the first
                        // measurement being past the desired time, so we can safely assume we have a previous
                        // measurement.
                        if (!interpolateValues(stringToValue((**(std::prev(pvIt)))->getValue(),
                                                             (**(std::prev(pvIt)))->getDataType()), // val1
                                               (**(std::prev(pvIt)))->getEventTime(),               // date1
                                               stringToValue((**(pvIt))->getValue(),
                                                             (**(pvIt))->getDataType()),            // val2
                                               (**(pvIt))->getEventTime(),                          // date2
                                               timeIt,                                              // dateRes
                                               (*m_cdValued.at(pvName))->getInterpolationType(),    // interpolationType
                                               newVal)) {                                           // valRes
                            return -5;
                        }

                        std::cerr << "We have two measurements to interpolate from\n"
                                  << "\t" << (**(std::prev(pvIt)))->getValue() << " @ "
                                  << ((**(std::prev(pvIt)))->getEventTime())
                                  << "\n"
                                  << "\t" << (**(pvIt))->getValue() << " @ "<< ((**(pvIt))->getEventTime())
                                  << "\n"
                                  << "=>" << newVal << " @ " << (timeIt) << "\n";

                    }
                    PUSH_EVENT(**m_cdValued.at(pvName), // Covariate Definition
                               timeIt,                     // Time
                               newVal,                     // Value
                               _series);                   // Series


                    if (m_cdComputed.size() > 0) {

                        std::cerr << "We have computed values!\n";

                        // We have a change in a value and some of the values are computed -> we need to do an update of
                        // the operable graph with this new value, then trigger a recomputation, and update the values
                        // that changed. This is done by updating the value of the event pushed in the ogm.
                        (_nccValuesMap.at(pvName))->setValue(newVal);
                        m_ogm.evaluate();

                        for (auto &cvm : _computedValuesMap) {
                            // We do the update here *only* for the Computed Variates that have no refresh period.
                            // We will deal with those with a refresh period set afterwards.
                            if ((*(m_cdComputed.at(cvm.first)))->getRefreshPeriod().isEmpty()) {
                                double cvVal;
                                if (!m_ogm.getValue(cvm.second.first->getId(), cvVal)) {
                                    return -7;
                                }

                                // Check if the new value is different than the old one. If this is the case, create a
                                // new event.
                                if (cvVal != cvm.second.second) {
                                    // Update the value in the map.
                                    cvm.second.second = cvVal;
                                    // Update the date in the event.
                                    cvm.second.first->setEventTime(timeIt);
                                    // Push the covariate in the event series.
                                    _series.push_back(*cvm.second.first);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}


int CovariateExtractor::generatePeriodicComputedCovariates(const std::map<DateTime, std::vector<std::string>> _refreshMap,
                                                           std::map<std::string, std::pair<std::shared_ptr<CovariateEvent>, Value>> &_computedValuesMap,
                                                           std::map<std::string, std::shared_ptr<CovariateEvent>> &_nccValuesMap,
                                                           CovariateSeries &_series)
{
    // Iterate on the map, setting all patient covariates to the appropriate values and relaunching the computation
    // via the Operable Graph Manager.
    for (const auto &refresh_t : _refreshMap) {
        for (const auto &pvMap : m_pvValued) {
            // Get the new value for the patient variate.
            Value newVal = getPatientVariateValue(m_pvValued.at(pvMap.first), refresh_t.first,
                                                  (*m_cdValued.at(pvMap.first))->getInterpolationType());
            // Set the value in the non-computed covariate event whose pointer is stored in nccValuesMap.
            (_nccValuesMap.at(pvMap.first))->setValue(newVal);
        }
        // Run the evaluation for the desired time instant.
        m_ogm.evaluate();

        for (const auto &cvName : refresh_t.second) {
            double cvVal;
            if (!m_ogm.getValue(cvName, cvVal)) {
                return -7;
            }

            // Check if the new value is different than the old one. If this is the case, create a
            // new event.
            if (cvVal != _computedValuesMap.at(cvName).second) {
                // Update the value in the map.
                _computedValuesMap.at(cvName).second = cvVal;
                // Update the date in the event.
                _computedValuesMap.at(cvName).first->setEventTime(refresh_t.first);
                // Push the covariate in the event series.
                _series.push_back(*_computedValuesMap.at(cvName).first);
            }
        }

    }
    return 0;
}






// Error codes:
// -1 : null pointer in covariate definitions.
// -2 : null pointer in patient variates.
// -3 : invalid date (start > end).
// -4 : duplicate ID in covariate definitions.
// -5 : interpolation failure.
// -6 : initial evaluation of computed values failed.
// -7 : missing value in map.

int CovariateExtractor::extract(CovariateSeries &_series)
{
    sortPatientVariates();

    // Map holding the pointers to the events linked with non-computed covariates.
    std::map<std::string, std::shared_ptr<CovariateEvent>> nccValuesMap;

    // Map holding the pointers to the events linked with computed covariates and their latest value.
    std::map<std::string, std::pair<std::shared_ptr<CovariateEvent>, Value>> computedValuesMap;

    createInitialEvents(nccValuesMap, computedValuesMap, _series);

    // *** Generate events past the default ones ***
    // Unfortunately discovering all the relations among covariates is too difficult -- it would mean redoing the
    // job of the OperableGraphManager. We will therefore iterate over the PatientVariates and call an evaluate()
    // each time the update is needed.

    addPatientVariateNoRefresh(computedValuesMap, _series);
    addPatientVariateWithRefresh(computedValuesMap, nccValuesMap, _series);

    //    for (const auto &pvMap : m_pvValued) {

    //        std::cerr << "## VARIABLE: " << pvMap.first << " ##\n";

    //        // Consider that patient variates are already sorted by date (for each patient variate type).
    //        Duration refreshPeriod = (*m_cdValued.at(pvMap.first))->getRefreshPeriod();
    //        if (refreshPeriod.isEmpty()) {
    //            addPatientVariateNoRefresh(pvMap.first, pvMap.second, computedValuesMap, _series);
    //        } else {
    //            addPatientVariateWithRefresh(refreshPeriod, pvMap.first, pvMap.second,
    //                                         computedValuesMap, nccValuesMap, _series);
    //        }
    //    }

    // Now deal with the Computed Covariates with a fixed refresh period.
    // The least expensive approach is to get a list of the refresh times and perform the update (indeed, several
    // different computed covariates might want to refresh at the same time, and we do want to do their update once).

    // Collect refresh intervals. For each date, we can have multiple covariates that want to be refreshed.
    std::map<DateTime, std::vector<std::string>> refreshMap;
    collectRefreshIntervals(computedValuesMap, refreshMap);


    // If we have any computed covariate with a fixed refresh period...
    if (refreshMap.size() > 0) {
        generatePeriodicComputedCovariates(refreshMap, computedValuesMap, nccValuesMap, _series);
    }

    return 0;
}

Value CovariateExtractor::getPatientVariateValue(const std::vector<pvIterator_t>& _PV,
                                                 const DateTime &_t,
                                                 const InterpolationType _interpolationType)
{
    Value newVal = 0.0;
    if (_PV.size() == 1 || (*(_PV.at(0)))->getEventTime() >= _t) {
        // If single patient variate value or first variate measured after the refresh interval, then take the value of
        // the first variate.
        newVal = std::stod((*(_PV.at(0)))->getValue());
    } else {
        std::vector<pvIterator_t>::const_iterator pvIt = _PV.begin();
        // Advance until we pass the measurement just before the chosen time, or until we get to the
        // last available measurement.
        while (pvIt != _PV.end() && _t < (**pvIt)->getEventTime()) {
            ++pvIt;
        }
        if (pvIt == _PV.end()) {
            // We are past in time after the last measurement we have, so we have to keep the last value.
            newVal = stringToValue((**(std::prev(pvIt)))->getValue(),
                                   (**(std::prev(pvIt)))->getDataType());
        } else {
            interpolateValues(stringToValue((**(std::prev(pvIt)))->getValue(),
                                            (**(std::prev(pvIt)))->getDataType()), // val1
                              (**(std::prev(pvIt)))->getEventTime(),               // date1
                              stringToValue((**(pvIt))->getValue(),
                                            (**(pvIt))->getDataType()),            // val2
                              (**(pvIt))->getEventTime(),                          // date2
                              _t,                                                  // dateRes
                              _interpolationType,                                  // interpolationType
                              newVal);                                             // valRes
        }
    }
    return newVal;
}

/***********************************************************************************************************************
 *                                           OK VERIFIED AND TESTED                                                    *
 **********************************************************************************************************************/

CovariateExtractor::CovariateExtractor(const CovariateDefinitions &_defaults,
                                       const PatientVariates &_patientCovariates,
                                       const DateTime &_start,
                                       const DateTime &_end)
    : ICovariateExtractor(_defaults, _patientCovariates, _start, _end)
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
        if (rc.second == false) {
            // Duplicate ID!
            throw std::runtime_error("[CovariateExtractor] Duplicate covariate variable (" + (*it)->getId() + ")");
        }
    }

    // Push patient variates -- not computed by definition.
    for (pvIterator_t it = m_patientCovariates.begin(); it != m_patientCovariates.end(); ++it) {
        // If we cannot find a corresponding covariate definition, we can safely drop a patient variate.
        if (m_cdValued.find((*it)->getId()) != m_cdValued.end()) {
            if (m_pvValued.find((*it)->getId()) == m_pvValued.end()) {
                m_pvValued.insert(std::pair<std::string, std::vector<pvIterator_t>>((*it)->getId(),
                                                                                    std::vector<pvIterator_t>()));
            }
            m_pvValued.at((*it)->getId()).push_back(it);
        }
    }
}


int CovariateExtractor::addPatientVariateNoRefresh(std::map<std::string, std::pair<std::shared_ptr<CovariateEvent>, Value>> &_computedValuesMap,
                                                   CovariateSeries &_series)
{
    for (const auto &pvMap : m_pvValued) {
        const std::string &pvName = pvMap.first;
        const std::vector<pvIterator_t> &pvValues = pvMap.second;
        Duration refreshPeriod = (*m_cdValued.at(pvName))->getRefreshPeriod();
        if (refreshPeriod.isEmpty()) {
            // Ignore the patient variates with a refresh period set. For those without a refresh period, just push the
            // values as they are.

            for (const auto &pv : pvValues) {
                // If we have a single value, this value is set as initial default and used for the entire
                // period, so we can ignore it here.
                // The same applies if the first value happens at m_start.
                if (pvValues.size() > 1 && (*pv)->getEventTime() != m_start) {
                    PUSH_EVENT(**m_cdValued.at(pvName),          // Covariate Definition
                               (*pv)->getEventTime(),               // Time
                               stringToValue((*pv)->getValue(),
                                             (*pv)->getDataType()), // Value
                               _series);                            // Series
                    if (m_cdComputed.size() > 0) {
                        // We have a change in a value and some of the values are computed -> we need to do an update of
                        // the operable graph with this new value, then trigger a recomputation, and update the values
                        // that changed.
                        (*(m_cdValued.at(pvName)))->setValue(stringToValue((*pv)->getValue(), (*pv)->getDataType()));
                        m_ogm.evaluate();
                        for (auto &cvm : _computedValuesMap) {
                            // We do the update here *only* for the Computed Variates that have no refresh period.
                            // We will deal with those with a refresh period set afterwards.
                            if ((*(m_cdComputed.at(cvm.first)))->getRefreshPeriod().isEmpty()) {
                                double cvVal;
                                if (!m_ogm.getValue(cvm.second.first->getId(), cvVal)) {
                                    return -7;
                                }
                                // Check if the new value is different than the old one. If this is the case, create a
                                // new event.
                                if (cvVal != cvm.second.second) {
                                    // Update the value in the map.
                                    cvm.second.second = cvVal;
                                    // Push the covariate in the event series.
                                    _series.push_back(*cvm.second.first);
                                }
                            }
                        }
                    }
                }
            }

        }
    }

    return 0;
}


void CovariateExtractor::collectRefreshIntervals(const std::map<std::string, std::pair<std::shared_ptr<CovariateEvent>, Value>> &_computedValuesMap,
                                                 std::map<DateTime, std::vector<std::string>> &_refreshMap)
{
    for (const auto &cvm : _computedValuesMap) {
        Duration refreshInterval;
        refreshInterval = (*(m_cdComputed.at(cvm.first)))->getRefreshPeriod();
        if (!refreshInterval.isEmpty()) {
            for (DateTime t = m_start; t <= m_end; t += refreshInterval) {
                if (_refreshMap.find(t) == _refreshMap.end()) {
                    _refreshMap.insert(std::pair<DateTime, std::vector<std::string>>(t, std::vector<std::string>()));
                }
                _refreshMap.at(t).push_back(cvm.first);
            }
        }
    }
}


bool CovariateExtractor::createInitialEvents(std::map<std::string, std::shared_ptr<CovariateEvent>> &_nccValuesMap,
                                             std::map<std::string, std::pair<std::shared_ptr<CovariateEvent>, Value>> &_computedValuesMap,
                                             CovariateSeries &_series)
{
    // The order here is important -- computed covariates need the default value of non-computed ones to get their
    // value!
    if (!createNonComputedCEvents(_nccValuesMap, _series)) {
        return false;
    }
    if (!createComputedCEvents(_computedValuesMap, _series)) {
        return false;
    }
    return true;
}


bool CovariateExtractor::createNonComputedCEvents(std::map<std::string, std::shared_ptr<CovariateEvent>> &_nccValuesMap,
                                                  CovariateSeries &_series)
{
    // Standard values (no computations involved).
    for (const auto &cdv : m_cdValued) {
        std::map<std::string, std::vector<pvIterator_t>>::const_iterator it;
        it = m_pvValued.find(cdv.first);
        std::shared_ptr<CovariateEvent> event;
        if (it == m_pvValued.end()) {
            // If no patient variates associated, then take default value (it won't be touched afterwards).
            event = std::make_shared<CovariateEvent>(**(cdv.second), m_start, (*(cdv.second))->getValue());
        } else {
            Value newVal = 0.0;
            if (it->second.size() == 1 || (*(it->second.at(0)))->getEventTime() >= m_start) {
                // If single patient variate value or measurement start after m_start, then take the first value.
                newVal = stringToValue((*(it->second.at(0)))->getValue(), (*(it->second.at(0)))->getDataType());
            } else {
                // If multiple values with the first before m_start, then use the value interpolated using the first two
                // elements of the vector.
                Value val1 = stringToValue((*(it->second.at(0)))->getValue(), (*(it->second.at(0)))->getDataType());
                Value val2 = stringToValue((*(it->second.at(1)))->getValue(), (*(it->second.at(1)))->getDataType());
                bool rc = interpolateValues(val1, (*(it->second.at(0)))->getEventTime(),
                                            val2, (*(it->second.at(1)))->getEventTime(),
                                            m_start,
                                            (*(cdv.second))->getInterpolationType(),
                                            newVal);
                if (rc == false) {
                    return false;
                }
            }
            event = std::make_shared<CovariateEvent>(**(cdv.second), m_start, newVal);
        }

        _series.push_back(*event);
        m_ogm.registerInput(event, cdv.first);
        _nccValuesMap.insert(std::pair<std::string, std::shared_ptr<CovariateEvent>>(cdv.first, event));
    }
    return true;
}


bool CovariateExtractor::createComputedCEvents(std::map<std::string, std::pair<std::shared_ptr<CovariateEvent>, Value>> &_computedValuesMap,
                                               CovariateSeries &_series)
{
    // Computed values.
    if (m_cdComputed.size() > 0) {
        // Push an Operable for each computed Covariate.
        for (const auto &cdc : m_cdComputed) {
            std::shared_ptr<CovariateEvent> event = std::make_shared<CovariateEvent>(**(cdc.second), m_start, 0.0);
            _computedValuesMap.insert(std::pair<std::string, std::pair<std::shared_ptr<CovariateEvent>, Value>>(cdc.first,
                                                                                                                std::make_pair(event, 0.0)));
            m_ogm.registerOperable(event, cdc.first);
        }

        // Call the evaluation once to generate the first set of values for the computed events, using the default
        // values for the covariates.
        bool rc = m_ogm.evaluate();
        if (rc == false) {
            return false;
        }

        for (auto &cvm : _computedValuesMap) {
            // Update the value in the map.
            cvm.second.second = cvm.second.first->getValue();
            // Push each computed covariate in the event series.
            _series.push_back(*cvm.second.first);
        }
    }
    return true;
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
            (*(pvV.second.at(0)))->setEventTime(m_start);
        }
    }
}


}
}

