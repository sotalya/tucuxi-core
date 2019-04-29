/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_COVARIATEEXTRACTOR_H
#define TUCUXI_CORE_COVARIATEEXTRACTOR_H

#include <algorithm>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "tucucommon/datetime.h"
#include "tucucommon/utils.h"

#include "tucucore/definitions.h"
#include "tucucore/covariateevent.h"
#include "tucucore/computingservice/computingresult.h"

struct TestCovariateExtractor;

namespace Tucuxi {
namespace Core {

using Tucuxi::Common::Duration;

/// \brief Interface for the covariate extractor.
class ICovariateExtractor
{
public:

    /// \brief Create a Covariate Extractor for the specified interval and covariate set.
    /// \param _defaults Default covariate events.
    /// \param _patientCovariates Patient-specific covariates.
    /// \param _start Start time of the considered interval.
    /// \param _end End time of the considered interval.
    ICovariateExtractor(const CovariateDefinitions &_defaults,
                        const PatientVariates &_patientCovariates,
                        const DateTime &_start,
                        const DateTime &_end)
        : m_defaults{_defaults}, m_patientCovariates{_patientCovariates},
          m_start{_start}, m_end{_end}
    { }

    /// \brief Default destructor for the Covariate Extractor.
    virtual ~ICovariateExtractor() = default;

    /// \brief Extract covariate events.
    /// \param _series Set of extracted covariate events.
    /// \return 0 on success, an error code in case an issue arised.
    virtual ComputingResult extract(CovariateSeries &_series) = 0;


protected:
    /// \brief Set of default values for the covariates.
    const CovariateDefinitions &m_defaults;

    /// \brief Patient-specific values for the covariates.
    const PatientVariates &m_patientCovariates;

    /// \brief Start time for the desired interval.
    const DateTime m_start;

    /// \brief End time for the desired interval.
    const DateTime m_end;
};

/// \brief Extractor for covariate events from a list of covariate definitions and patient's covariates.
class CovariateExtractor : public ICovariateExtractor
{
public:

    /// \brief Create a Covariate Extractor for the specified interval and covariate set.
    /// \param _defaults Default covariate events.
    /// \param _patientCovariates Patient-specific covariates.
    /// \param _start Start time of the considered interval.
    /// \param _end End time of the considered interval.
    /// \pre FORALL(d : _defaults) { d != nullptr }
    /// \pre FORALL(p : _patientCovariates) { p != nullptr }
    /// \pre _start <= _end
    /// \pre No duplicates in _defaults
    CovariateExtractor(const CovariateDefinitions &_defaults,
                       const PatientVariates &_patientCovariates,
                       const DateTime &_start,
                       const DateTime &_end);

    /// \brief Default destructor for the Covariate Extractor.
    ~CovariateExtractor() override = default;

    /// \brief Extract covariate events.
    /// \param _series Set of extracted covariate events.
    /// \return ComputingResult::Ok on success, Result::ExtractionError in case an issue arised.
    ComputingResult extract(CovariateSeries &_series) override;

    // Make the test class friend, as this will allow us to test the helper methods (which are private).
    friend TestCovariateExtractor;

    /// \brief Standard name for a covariate that carries the birth date.
    static const std::string BIRTHDATE_CNAME; // NOLINT(readability-identifier-naming)

private:
    /// \brief Collect the time instants when the computed covariates have to be re-evaluated.
    /// \param _computedValuesMap Map that holds the pointers to the events due to computed covariates, as well as their
    ///                           latest value.
    /// \param _refreshMap Map containing, for each selected time instant, the list of covariates to update.
    void collectRefreshIntervals(std::map<DateTime, std::vector<std::string>> &_refreshMap);

    /// \brief Generate the events due to the covariates and the patient variates.
    /// \param _refreshMap Map containing, for each selected time instant, the list of covariates to update.
    /// \param _series Set of extracted covariate events.
    /// \return True if the events have been successfully extracted, false in case of errors.
    bool computeEvents(const std::map<DateTime, std::vector<std::string>> &_refreshMap, CovariateSeries &_series);

    /// \brief Perform the chosen interpolation between the given values.
    /// \note The desired time can be *before* the lower date or *after* the higher date. This results in extrapolation,
    ///       and can be used to extend the initial or final value. However, care must be taken to avoid invalid values
    ///       (for instance, negative weights). Those checks are left to the user of the function.
    /// \param _val1 First value.
    /// \param _date1 Time of the first measurement.
    /// \param _val2 Second value.
    /// \param _date2 Time of the second measurement.
    /// \param _dateRes Time of the desired interpolated value.
    /// \param _interpolationType Interpolation type.
    /// \param _valRes Interpolated value at the desired time.
    /// \return True if the interpolation was successful, false otherwise.
    /// \pre (_date1 < _date2) || (_date1 == _date2 && _val1 == _val2)
    /// \post if (_date1 < _date2) { _valRes = INTERPOLATED_VALUE && [RETURN] == true }
    ///       else  if (_date1 == _date2 && _val1 == _val2) { _valRes = _val1 && [RETURN] == true }
    ///       else { [RETURN] == false }
    bool interpolateValues(Value _val1, const DateTime &_date1,
                           Value _val2, const DateTime &_date2,
                           const DateTime &_dateRes,
                           InterpolationType _interpolationType,
                           Value &_valRes) const;

    /// \brief Sort available Patient Variates, discarding those not of interest.
    /// If the covariate is not interpolated, then its first observation is replaced by an observation at the beginning
    /// of the interval.
    void sortPatientVariates();

    /// \brief Flag that marks that a birth date has been found.
    bool m_hasBirthDate;

    /// \brief Birth date (if found).
    DateTime m_birthDate;

    /// \brief Default value for an AgeInDays variable (if present).
    double m_initAgeInDays;

    /// \brief Default value for an AgeInWeeks variable (if present).
    double m_initAgeInWeeks;

    /// \brief Default value for an AgeInMonths variable (if present).
    double m_initAgeInMonths;

    /// \brief Default value for an AgeInYears variable (if present).
    double m_initAgeInYears;

    /// \brief Operable Graph Manager, in charge of performing all the computations needed to derive covariate values.
    OperableGraphManager m_ogm;

    /// \brief Covariate Definitions that have their own independent value (that is, not computed).
    std::map<std::string, cdIterator_t> m_cdValued;

    /// \brief Covariate Definitions that are computed.
    std::map<std::string, cdIterator_t> m_cdComputed;

    /// \brief Patient Variates (measured values, therefore cannot be computed!).
    /// We normally have multiple values for each patient variate (corresponding to multiple measurements of the same
    /// physical quantity).
    std::map<std::string, std::vector<pvIterator_t>> m_pvValued;
};


} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_COVARIATEEXTRACTOR_H
