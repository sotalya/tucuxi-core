/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "tucucommon/general.h"
#include "tucucore/covariateextractor.h"

namespace Tucuxi {
namespace Core {

// Error codes:
// -1 : null pointer in covariate definitions.
// -2 : null pointer in patient variates.
// -3 : invalid date (start > end).
// -4 : duplicate ID in covariate definitions.
// -5 : interpolation failure.

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

    // Computed values.
    if (cdComputed.size() > 0) {
        // Push an Operable for each computed Covariate.
        for (const auto &cdc : cdComputed) {
            std::shared_ptr<CovariateEvent> event = std::make_shared<CovariateEvent>(**(cdc.second), _start, 0);
            ogm.registerOperable(event, cdc.first);
        }
    }


    // *** ***
    if (cdComputed.size() > 0) {
        // Unfortunately discovering all the relations among covariates is too difficult -- it would mean redoing the
        // job of the OperableGraphManager. We will therefore need to throw everything inside the OGM, and then call an
        // evaluate() each time the update is needed.



    }
    // 1 - collect measure times (will be used to get interpolation times)

    // 2 - initialize values at _start

    // 3 - at every measure time, compute interpolated values for non-computed values

    // 4 - at every measure time, set values and perform evaluate on OGM

    // 5 - compute observation times for each variable with refresh time

    // 6 - fill covariate series

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


}
}

