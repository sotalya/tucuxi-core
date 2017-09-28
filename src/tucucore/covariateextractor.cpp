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
do {                                                        \
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
            pvValued.at((*it)->getId()).push_back(it);
        }
    }
    // For each patient variate, sort by date the list of values, then discard those not of interest (e.g., those before
    // the beginning of the interval, except the very last one before the start of the interval).
    for (auto &pvV : pvValued) {
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
    }

    // ** Push the covariates at the initial time in the covariate serie and in the OGM ***
    OperableGraphManager ogm;

    // Standard values (no computations involved).
    for (const auto &cdv : cdValued) {
        std::map<std::string, std::vector<pvIterator_t>>::const_iterator it;
        it = pvValued.find(cdv.first);
        std::shared_ptr<CovariateEvent> event;
        if (it == pvValued.end()) {
            // If no patient variates associated, then take default value (it won't be touched afterwards).
            event = std::make_shared<CovariateEvent>(**(cdv.second), _start, (*(cdv.second))->getValue());

        } else {
            double newVal = 0.0;
            if (it->second.size() == 1 || (*(it->second.at(0)))->getEventTime() >= _start) {
                // If single patient variate value or measurement start after _start, then take the first value.
                newVal = std::stod((*(it->second.at(0)))->getValue());
            } else {
                // If multiple values with the first before _start, then use the value interpolated using the first two
                // elements of the vector.
                double val1 = std::stod((*(it->second.at(0)))->getValue());
                double val2 = std::stod((*(it->second.at(1)))->getValue());
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
    }

    // Map holding the pointers to the events linked with computed covariates and their latest value.
    std::map<std::string, std::pair<std::shared_ptr<CovariateEvent>, Value>> computedValuesMap;

    // Computed values.
    if (cdComputed.size() > 0) {
        // Push an Operable for each computed Covariate.
        for (const auto &cdc : cdComputed) {
            std::shared_ptr<CovariateEvent> event = std::make_shared<CovariateEvent>(**(cdc.second), _start, 0.0);
            computedValuesMap[cdc.first] = std::make_pair(event, 0.0);
            ogm.registerOperable(event, cdc.first);
        }
        // Call the evaluation once to generate the first set of values for the
        // computed events.
        bool rc = ogm.evaluate();
        if (!rc) {
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
    // (consider that patient variates are already sorted by date (for each patient variate type)).
    if (cdComputed.size() > 0) {
        // Unfortunately discovering all the relations among covariates is too difficult -- it would mean redoing the
        // job of the OperableGraphManager. We will therefore iterate over the PatientVariates and call an evaluate()
        // each time the update is needed.



    } else {
        // Shortcut: no need to recompute the graph each time a patient variate is inserted, so we can just push the
        // patient variates in the series.
        for (const auto &pvMap : pvValued) {
            Duration refreshPeriod = (*cdValued[pvMap.first])->getRefreshPeriod();
            if (refreshPeriod.isEmpty()) {
                // If no refresh period set, then just push the values as they are.
                for (const auto &pv : pvMap.second) {
                    if (pvMap.second.size() > 1) {
                        // If we have a single value, this value is set as initial default and used for the entire
                        // period, so we can ignore it here.
                        PUSH_EVENT(**cdValued[pvMap.first],             // Covariate Definition
                                   (*pv)->getEventTime(),               // Time
                                   stringToValue((*pv)->getValue(),
                                                 (*pv)->getDataType()), // Value
                                   _series);                            // Series
                    }
                }
            } else {
                // We need to compute the interpolated values and push them at the appropriate time.
                // The "appropriate time" is represented by the initial time plus the refresh period, therefore we have
                // to interpolate the correct values for each refresh period.
                std::vector<pvIterator_t>::const_iterator pvIt = pvMap.second.begin();

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
                        PUSH_EVENT(**cdValued[pvMap.first],                                  // Covariate Definition
                                   timeIt,                                                   // Time
                                   stringToValue((**(pvMap.second.begin()))->getValue(),
                                                 (**(pvMap.second.begin()))->getDataType()), // Value
                                   _series);                                                 // Series
                    } else {
                        // Advance until we pass the measurement just before the chosen time, or until we get to the
                        // last available measurement.
                        while (timeIt >= (**pvIt)->getEventTime() && std::next(pvIt) != pvMap.second.end()) {
                            ++pvIt;
                        }
                        if (std::next(pvIt) == pvMap.second.end()) {
                            // We are past in time after the last measurement we have, so we have to keep the value.
                            PUSH_EVENT(**cdValued[pvMap.first],                  // Covariate Definition
                                       timeIt,                                   // Time
                                       stringToValue((**(pvIt))->getValue(),
                                                     (**(pvIt))->getDataType()), // Value
                                       _series);                                 // Series
                        } else {
                            // We have two measurements to interpolate from (we have dealt with the case of the first
                            // measurement being past the desired time, so we can safely assume we have a previous
                            // measurement.
                            double newVal = 0.0;
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
                            PUSH_EVENT(**cdValued[pvMap.first], // Covariate Definition
                                       timeIt,                  // Time
                                       newVal,                  // Value
                                       _series);                // Series
                        }
                    }
                    // Do not forget to increment the vector iterator here if needed!
                    ++pvIt;
                }
            }
        }
    }

    return 0;
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

