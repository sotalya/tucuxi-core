/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_COVARIATEEXTRACTOR_H
#define TUCUXI_CORE_COVARIATEEXTRACTOR_H

#include <algorithm>
#include <iostream>
#include <map>
#include <memory>
#include <vector>

#include "tucucommon/datetime.h"

#include "tucucore/definitions.h"
#include "tucucore/covariate.h"

namespace Tucuxi {
namespace Core {

/// \brief Interface for the covariate extractor.
class ICovariateExtractor
{
public:
    /// \brief Extract covariate events in the specified interval.
    /// \param _defaults Default covariate events.
    /// \param _patientCovariates Patient-specific covariates.
    /// \param _start Start time of the considered interval.
    /// \param _end End time of the considered interval.
    /// \param _series Set of extracted covariate events.
    /// \return 0 on success, an error code in case an issue arised.
    virtual int extract(
            const CovariateDefinitions &_defaults,
            const PatientVariates &_patientCovariates,
            const DateTime &_start,
            const DateTime &_end,
            CovariateSeries &_series) = 0;
};

/// \brief Extractor for covariate events from a list of covariate definitions and patient's covariates.
class CovariateExtractor : public ICovariateExtractor
{
public:
    /// \brief Extract covariate events in the specified interval.
    /// \param _defaults Default covariate events.
    /// \param _patientCovariates Patient-specific covariates.
    /// \param _start Start time of the considered interval.
    /// \param _end End time of the considered interval.
    /// \param _series Set of extracted covariate events.
    /// \return 0 on success, an error code in case an issue arised.
    int extract(
            const CovariateDefinitions &_defaults,
            const PatientVariates &_patientCovariates,
            const DateTime &_start,
            const DateTime &_end,
            CovariateSeries &_series) override;

    /// \brief Perform the chosen interpolation between the given values.
    /// \param _val1 First value.
    /// \param _date1 Time of the first measurement.
    /// \param _val2 Second value.
    /// \param _date2 Time of the second measurement.
    /// \param _dateRes Time of the desired interpolated value.
    /// \param _interpolationType Interpolation type.
    /// \param _valRes Interpolated value at the desired time.
    /// \return True if the interpolation was successful, false otherwise.
    /// \pre _date1 < _date2 || (_date1 == _date2 && _val1 == _val2)
    bool interpolateValues(const double _val1, const DateTime &_date1,
                             const double _val2, const DateTime &_date2,
                             const DateTime &_dateRes,
                             const InterpolationType _interpolationType,
                             double &_valRes);
};


}
}

#endif // TUCUXI_CORE_COVARIATEEXTRACTOR_H
