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

class TestCovariateExtractor;

namespace Tucuxi {
namespace Core {

using Tucuxi::Common::Duration;
using namespace Tucuxi::Common::Utils;

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
    virtual int extract(CovariateSeries &_series) = 0;


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
    CovariateExtractor(const CovariateDefinitions &_defaults,
                       const PatientVariates &_patientCovariates,
                       const DateTime &_start,
                       const DateTime &_end);

    /// \brief Default destructor for the Covariate Extractor.
    virtual ~CovariateExtractor() = default;

    /// \brief Extract covariate events.
    /// \param _series Set of extracted covariate events.
    /// \return 0 on success, an error code in case an issue arised.
    int extract(CovariateSeries &_series) override;

    // Make the test class friend, as this will allow us to test the helper methods (which are private).
    friend TestCovariateExtractor;

private:

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

    Value getPatientVariateValue(const std::vector<pvIterator_t>& _PV,
                                 const DateTime &_t,
                                 const InterpolationType _interpolationType);




    int createInitialEvents(std::map<std::string, std::pair<std::shared_ptr<CovariateEvent>, Value>> &_computedValuesMap,
                            std::map<std::string, std::shared_ptr<CovariateEvent>> &_nccValuesMap,
                            CovariateSeries &_series);

    void collectRefreshIntervals(const std::map<std::string, std::pair<std::shared_ptr<CovariateEvent>, Value>> &_computedValuesMap,
                                 std::map<DateTime, std::vector<std::string>> &_refreshMap);

    int generatePeriodicComputedCovariates(const std::map<DateTime, std::vector<std::string>> _refreshMap,
                                           std::map<std::string, std::pair<std::shared_ptr<CovariateEvent>, Value>> &_computedValuesMap,
                                           std::map<std::string, std::shared_ptr<CovariateEvent>> &_nccValuesMap,
                                           CovariateSeries &_series);

    int addPatientVariateNoRefresh(const std::string &_pvName,
                                   const std::vector<pvIterator_t> &_pvValues,
                                   std::map<std::string, std::pair<std::shared_ptr<CovariateEvent>, Value>> &_computedValuesMap,
                                   CovariateSeries &_series);

    int addPatientVariateWithRefresh(const Duration &_refreshPeriod,
                                     const std::string &_pvName,
                                     const std::vector<pvIterator_t> &_pvValues,
                                     std::map<std::string, std::pair<std::shared_ptr<CovariateEvent>, Value>> &_computedValuesMap,
                                     std::map<std::string, std::shared_ptr<CovariateEvent>> &_nccValuesMap,
                                     CovariateSeries &_series);

    /*******************************************************************************************************************
     *                                           OK VERIFIED AND TESTED                                                *
     ******************************************************************************************************************/

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
    bool interpolateValues(const Value _val1, const DateTime &_date1,
                           const Value _val2, const DateTime &_date2,
                           const DateTime &_dateRes,
                           const InterpolationType _interpolationType,
                           Value &_valRes);

    /// \brief Sort available Patient Variates, discarding those not of interest.
    int sortPatientVariates();
};


}
}

#endif // TUCUXI_CORE_COVARIATEEXTRACTOR_H
