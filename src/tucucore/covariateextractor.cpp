/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "tucucommon/general.h"
#include "tucucore/covariateextractor.h"

namespace Tucuxi {
namespace Core {

void printDate(const Tucuxi::Common::DateTime &_dt)
{
    std::cerr << _dt.day() << "."
              << _dt.month() << "."
              << _dt.year() << " "
              << _dt.hour() << "h"
              << _dt.minute() << "m"
              << _dt.second() << "s";
}

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

// Error codes:
// -1 : null pointer in covariate definitions.
// -2 : null pointer in patient variates.
// -3 : invalid date (start > end).
// -4 : duplicate ID in covariate definitions.
// -5 : interpolation failure.
// -6 : initial evaluation of computed values failed.
// -7 : missing value in map.

int CovariateExtractor::extract(
        const CovariateDefinitions &_defaults,
        const PatientVariates &_patientCovariates,
        const DateTime &_start,
        const DateTime &_end,
        CovariateSeries &_series)
{
    // *** Verify preconditions ***
    // Invalid ptrs in covariate definitions
    for (const auto &cd : _defaults) {
        if (cd == nullptr) {
            return -1;
        }
    }
    // Invalid ptrs in patient variates
    for (const auto &pv : _patientCovariates) {
        if (pv == nullptr) {
            return -2;
        }
    }
    // Start time past end time
    if (_start > _end) {
        return -3;
    }

    // ** Fill internal structures with ID and position in vector, splitting between computed and not **
    // Covariate Definitions which are independent (that is, have their own value).
    std::map<std::string, cdIterator_t> cdValued;
    // Covariate Definitions which are computed.
    std::map<std::string, cdIterator_t> cdComputed;
    // Patient Variates (measured values, therefore cannot be computed!). We normally have multiple patient variates.
    std::map<std::string, std::vector<pvIterator_t>> pvValued;

    // Push covariate definitions, splitting between computed and not.
    for (cdIterator_t it = _defaults.begin(); it != _defaults.end(); ++it) {
        std::pair<std::map<std::string, cdIterator_t>::iterator, bool> rc;
        if ((*it)->isComputed()) {
            rc = cdComputed.insert(std::pair<std::string, cdIterator_t>((*it)->getId(), it));
        } else {
            rc = cdValued.insert(std::pair<std::string, cdIterator_t>((*it)->getId(), it));
        }
        if (rc.second == false) {
            // Duplicate ID!
            return -4;
        }
    }

    // Push patient variates -- not computed by definition.
    for (pvIterator_t it = _patientCovariates.begin(); it != _patientCovariates.end(); ++it) {
        // If we cannot find a corresponding covariate definition, we can safely drop a patient variate.
        if (cdValued.find((*it)->getId()) != cdValued.end()) {
            if (pvValued.find((*it)->getId()) == pvValued.end()) {
                pvValued.insert(std::pair<std::string, std::vector<pvIterator_t>>((*it)->getId(),
                                                                                  std::vector<pvIterator_t>()));
            }
            pvValued.at((*it)->getId()).push_back(it);
        }
    }

    // For each patient variate, sort by date the list of values, then discard those not of interest.
    // If the variable is not interpolated, then its first observation is replaced by an observation at the beginning
    // of the interval.
    for (auto &pvV : pvValued) {

//        std::cerr << "\n" << pvV.first << " before sorting:\n";
//        int aa = 0;
//        for (const auto &pval : pvV.second) {
//            std::cerr << aa++ << " ";
//            printDate((*pval)->getEventTime());
//            std::cerr << "\n";
//        }

        // Sort by increasing date.
        std::sort(pvV.second.begin(), pvV.second.end(),
                  [](pvIterator_t &_a, pvIterator_t &_b) { return (*_a)->getEventTime() < (*_b)->getEventTime(); });
        // If just one value, keep it, else do a cleanup.
        if (pvV.second.size() > 1) {
            // Drop all values before _start and past _end, except the one on the border (we will use them for the
            // interpolation).
            std::vector<pvIterator_t>::iterator it = pvV.second.begin();
            // Remove before _start.
            while (std::next(it) != pvV.second.end() && (**it)->getEventTime() < _start) {
                if ((**(std::next(it)))->getEventTime() >= _start && it != pvV.second.begin()) {
                    pvV.second.erase(pvV.second.begin(), std::prev(it));
                    break;
                }
                ++it;
            }
            // Remove past _end.
            it = pvV.second.begin();
            while (std::next(it) != pvV.second.end() && (**it)->getEventTime() < _end) {
                if ((**(std::next(it)))->getEventTime() >= _end) {
                    if (std::next(++it) != pvV.second.end()) {
                        pvV.second.erase(std::next(it), pvV.second.end());
                        break;
                    }
                }
                ++it;
            }
        }

        // If InterpolationType == InterpolationType::Direct or a single value is present, then no interpolation is
        // performed and we can change the time of the first measurement with the initial interval time.
        if ((*(cdValued.at(pvV.first)))->getInterpolationType() == InterpolationType::Direct || pvV.second.size() == 1) {
            (*(pvV.second.at(0)))->setEventTime(_start);
        }

//        std::cerr << "\n" << pvV.first << " after sorting:\n";
//        aa = 0;
//        for (const auto &pval : pvV.second) {
//            std::cerr << aa++ << " ";
//            printDate((*pval)->getEventTime());
//            std::cerr << "\n";
//        }

    }

    // ** Push the covariates at the initial time in the covariate serie and in the OGM ***
    OperableGraphManager ogm;

    // Map holding the pointers to the events linked with non-computed covariates.
    std::map<std::string, std::shared_ptr<CovariateEvent>> nccValuesMap;

    // Standard values (no computations involved).
    for (const auto &cdv : cdValued) {
        std::map<std::string, std::vector<pvIterator_t>>::iterator it;
        it = pvValued.find(cdv.first);
        std::shared_ptr<CovariateEvent> event;
        if (it == pvValued.end()) {
            // If no patient variates associated, then take default value (it won't be touched afterwards).
            event = std::make_shared<CovariateEvent>(**(cdv.second), _start, (*(cdv.second))->getValue());

        } else {
            Value newVal = 0.0;
            if (it->second.size() == 1 || (*(it->second.at(0)))->getEventTime() >= _start) {
                // If single patient variate value or measurement start after _start, then take the first value.
                newVal = std::stod((*(it->second.at(0)))->getValue());
            } else {
                // If multiple values with the first before _start, then use the value interpolated using the first two
                // elements of the vector.
                Value val1 = std::stod((*(it->second.at(0)))->getValue());
                Value val2 = std::stod((*(it->second.at(1)))->getValue());
                bool rc = interpolateValues(val1, (*(it->second.at(0)))->getEventTime(),
                                            val2, (*(it->second.at(1)))->getEventTime(),
                                            _start,
                                            (*(cdv.second))->getInterpolationType(),
                                            newVal);
                if (rc == false) {
                    return -5;
                }
            }
            event = std::make_shared<CovariateEvent>(**(cdv.second), _start, newVal);
        }

        _series.push_back(*event);
        ogm.registerInput(event, cdv.first);
        nccValuesMap.insert(std::pair<std::string, std::shared_ptr<CovariateEvent>>(cdv.first, event));
    }

    // Map holding the pointers to the events linked with covariates and their latest value.
    std::map<std::string, std::pair<std::shared_ptr<CovariateEvent>, Value>> computedValuesMap;

    // Computed values.
    if (cdComputed.size() > 0) {
        // Push an Operable for each computed Covariate.
        for (const auto &cdc : cdComputed) {
            std::shared_ptr<CovariateEvent> event = std::make_shared<CovariateEvent>(**(cdc.second), _start, 0.0);
            computedValuesMap.insert(std::pair<std::string, std::pair<std::shared_ptr<CovariateEvent>, Value>>(cdc.first,
                                                                                                               std::make_pair(event, 0.0)));
            ogm.registerOperable(event, cdc.first);
        }

        // Call the evaluation once to generate the first set of values for the
        // computed events.
        bool rc = ogm.evaluate();
        if (rc == false) {
            return -6;
        }

        for (auto &cvm : computedValuesMap) {
            // Update the value in the map.
            cvm.second.second = cvm.second.first->getValue();
            // Push each computed covariate in the event series.
            _series.push_back(*cvm.second.first);
        }
    }

    // *** Generate events past the default ones ***
    // Unfortunately discovering all the relations among covariates is too difficult -- it would mean redoing the
    // job of the OperableGraphManager. We will therefore iterate over the PatientVariates and call an evaluate()
    // each time the update is needed.

    for (const auto &pvMap : pvValued) {
        // Consider that patient variates are already sorted by date (for each patient variate type).
        Duration refreshPeriod = (*cdValued[pvMap.first])->getRefreshPeriod();
        if (refreshPeriod.isEmpty()) {
            // If no refresh period set, then just push the values as they are.
            for (const auto &pv : pvMap.second) {
                // If we have a single value, this value is set as initial default and used for the entire
                // period, so we can ignore it here.
                // The same applies if the first value happens at _start.
                if (pvMap.second.size() > 1 && (*pv)->getEventTime() != _start) {
                    PUSH_EVENT(**cdValued[pvMap.first],          // Covariate Definition
                            (*pv)->getEventTime(),               // Time
                            stringToValue((*pv)->getValue(),
                                          (*pv)->getDataType()), // Value
                            _series);                            // Series
                    if (cdComputed.size() > 0) {
                        // We have a change in a value and some of the values are computed -> we need to do an update of
                        // the operable graph with this new value, then trigger a recomputation, and update the values
                        // that changed.
                        (*(cdValued.at(pvMap.first)))->setValue(stringToValue((*pv)->getValue(), (*pv)->getDataType()));
                        ogm.evaluate();
                        for (auto &cvm : computedValuesMap) {
                            // We do the update here *only* for the Computed Variates that have no refresh period.
                            // We will deal with those with a refresh period set afterwards.
                            if ((*(cdComputed.at(cvm.first)))->getRefreshPeriod().isEmpty()) {
                                double cvVal;
                                if (!ogm.getValue(cvm.second.first->getId(), cvVal)) {
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
        } else {
            // We need to compute the interpolated values and push them at the appropriate time.
            // The "appropriate time" is represented by the initial time plus the refresh period, therefore we have
            // to interpolate the correct values for each refresh period.
            std::vector<pvIterator_t>::const_iterator pvIt = pvMap.second.begin();
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
            for (auto timeIt = _start + refreshPeriod; timeIt < _end; timeIt += refreshPeriod) {
                if (timeIt <= (**(pvMap.second.begin()))->getEventTime()) {
                    // Before first measurement use the value of the first measurement.
                    newVal = stringToValue((**(pvMap.second.begin()))->getValue(),
                                           (**(pvMap.second.begin()))->getDataType());
                } else {
                    // Advance until we pass the measurement just before the chosen time, or until we get to the
                    // last available measurement.
                    while (timeIt >= (**pvIt)->getEventTime() && std::next(pvIt) != pvMap.second.end()) {
                        ++pvIt;
                    }
                    if (std::next(pvIt) == pvMap.second.end()) {
                        // We are past in time after the last measurement we have, so we have to keep the value.
                        newVal = stringToValue((**(pvIt))->getValue(),
                                               (**(pvIt))->getDataType());
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
                                               (*cdValued[pvMap.first])->getInterpolationType(),    // interpolationType
                                               newVal)) {                                           // valRes
                            return -5;
                        }
                    }
                }
                PUSH_EVENT(**cdValued[pvMap.first], // Covariate Definition
                        timeIt,                     // Time
                        newVal,                     // Value
                        _series);                   // Series

                if (cdComputed.size() > 0) {

                    std::cerr << "We have computed values!\n";

                    // We have a change in a value and some of the values are computed -> we need to do an update of
                    // the operable graph with this new value, then trigger a recomputation, and update the values
                    // that changed. This is done by updating the value of the event pushed in the ogm.
                    (nccValuesMap.at(pvMap.first))->setValue(newVal);
                    ogm.evaluate();

                    for (auto &cvm : computedValuesMap) {
                        // We do the update here *only* for the Computed Variates that have no refresh period.
                        // We will deal with those with a refresh period set afterwards.
                        if ((*(cdComputed[cvm.first]))->getRefreshPeriod().isEmpty()) {
                            double cvVal;
                            if (!ogm.getValue(cvm.second.first->getId(), cvVal)) {
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

    // Now deal with the Computed Covariates with a fixed refresh period.
    // The least expensive approach is to get a list of the refresh times and perform the update (indeed, several
    // different computed covariates might want to refresh at the same time, and we do want to do their update once).

    // Collect refresh intervals. For each date, we can have multiple covariates that want to be refreshed.
    std::map<DateTime, std::vector<std::string>> refreshMap;
    for (const auto &cvm : computedValuesMap) {
        Duration refreshInterval;
        refreshInterval = (*(cdComputed[cvm.first]))->getRefreshPeriod();
        if (!refreshInterval.isEmpty()) {
            for (DateTime t = _start; t < _end; t += refreshInterval) {
                if (refreshMap.find(t) == refreshMap.end()) {
                    refreshMap.insert(std::pair<DateTime, std::vector<std::string>>(t, std::vector<std::string>()));
                }
                refreshMap.at(t).push_back(cvm.first);
            }
        }
    }
    // If we have any computed covariate with a fixed refresh period...
    if (refreshMap.size() > 0) {
        // Iterate on the map, setting all patient covariates to the appropriate values and relaunching the computation
        // via the Operable Graph Manager.
        for (const auto &refresh_t : refreshMap) {
            for (const auto &pvMap : pvValued) {
                // Get the new value for the patient variate.
                Value newVal = getPatientVariateValue(pvValued[pvMap.first], refresh_t.first,
                        (*cdValued[pvMap.first])->getInterpolationType());
                // Set the value in the non-computed covariate event whose pointer is stored in nccValuesMap.
                (nccValuesMap.at(pvMap.first))->setValue(newVal);
            }
            // Run the evaluation for the desired time instant.
            ogm.evaluate();

            for (const auto &cvName : refresh_t.second) {
                double cvVal;
                if (!ogm.getValue(cvName, cvVal)) {
                    return -7;
                }

                // Check if the new value is different than the old one. If this is the case, create a
                // new event.
                if (cvVal != computedValuesMap[cvName].second) {
                    // Update the value in the map.
                    computedValuesMap[cvName].second = cvVal;
                    // Update the date in the event.
                    computedValuesMap[cvName].first->setEventTime(refresh_t.first);
                    // Push the covariate in the event series.
                    _series.push_back(*computedValuesMap[cvName].first);
                }
            }

        }

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
        while (_t < (**pvIt)->getEventTime() && std::next(pvIt) != _PV.end()) {
            ++pvIt;
        }
        if (std::next(pvIt) == _PV.end()) {
            // We are past in time after the last measurement we have, so we have to keep the value.
            newVal = stringToValue((**(pvIt))->getValue(),
                                   (**(pvIt))->getDataType());
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

bool CovariateExtractor::interpolateValues(const double _val1, const DateTime &_date1,
                                           const double _val2, const DateTime &_date2,
                                           const DateTime &_dateRes,
                                           const InterpolationType _interpolationType,
                                           double &_valRes)
{
    // Precondition: dates are sorted.
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
            _valRes = _val2;
        }
        break;
    case InterpolationType::Linear:
        // Angular coefficient.
        double m;
        m = (_val2 - _val1) / (_date2.toSeconds() - _date1.toSeconds());
        // Intercept.
        double b;
        b = _val2 - m * _date2.toSeconds();
        _valRes = _dateRes.toSeconds() * m + b;
        break;
    default:
        return false;
    }
    return true;
}

Value CovariateExtractor::stringToValue(std::string _str, const DataType &_dataType)
{
    Value v;
    std::transform(_str.begin(), _str.end(), _str.begin(), ::tolower);
    switch (_dataType)
    {
    case DataType::Int:
    {
        int tmp = std::stoi(_str);
        v = tmp;
    }
        break;

    case DataType::Double:
        v = std::stod(_str);
        break;

    case DataType::Bool:
        if (_str == "0" || _str == "false") {
            v = 0.0;
        } else {
            v = 1.0;
        }
        break;

    case DataType::Date:
    {
        DateTime dt(_str, "%Y-%b-%d %H:%M:%S");
        v = dt.toSeconds();
    }
        break;

    default:
        // Invalid type, set the value to 0.0.
        v = 0.0;
        break;
    }
    return v;
}


}
}

