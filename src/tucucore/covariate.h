/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_COVARIATE_H
#define TUCUXI_CORE_COVARIATE_H

#include <vector>

#include "tucucommon/duration.h"

#include "tucucore/definitions.h"
#include "tucucore/timedevent.h"
#include "tucucore/operation.h"
#include "tucucore/operablegraphmanager.h"
#include "tucucore/drugdefinitions.h"

namespace Tucuxi {
namespace Core {

/// \brief Define the covariate types.
/// - Standard: if no patient variate exist -> use operation in drug model to generate a new value each time one or more
///                                            inputs of the operation are modified
///             if cannot apply operation   -> use default value
///             if >= 1 variate exists      -> if only one value -> use for the entire period
///                                            else              -> interpolate with function defined in
///                                                                 CovariateDefinition, using first observed value for
///                                                                 the interval between start and the first observation
///   \warning Look also at values outside the given period! The period itself limits the range of measures we are
///            interested in, but does not affect the available variates.
/// - AgeInYears: automatic calculation based on birth date, use default if not available, unit = years.
/// - AgeInDays: automatic calculation based on birth date, use default if not available, unit = days.
/// - AgeInMonths: automatic calculation based on birth date, use default if not available, unit = months.
enum class CovariateType {
    Standard = 0,
    AgeInYears,
    AgeInDays,
    AgeInMonths
};

/// \brief Allowed data types.
enum class DataType {
    Int = 0,
    Double,
    Bool,
    Date
};

/// \brief Available interpolation functions.
/// - Direct: when value observed, set it as current value.
/// - Linear: between two occurrences of observed covariates, use linear interpolation.
/// - Sigmoid: between two occurrences of observed covariates, use sigmoidal interpolation.
/// - Tanh: between two occurrences of observed covariates, use hyperbolic tangent interpolation.
enum class InterpolationType
{
    Direct = 0,
    Linear,
    Sigmoid,
    Tanh
};

/// \brief Definition of a covariate for a given drug, using the information extracted from the drug's XML file.
class CovariateDefinition : public PopulationValue
{
public:
    CovariateDefinition(const std::string &_id, Value _value, Operation *_operation, CovariateType _type) :
        PopulationValue(_id, _value, _operation), m_type(_type) {}

    /// \brief Get the covariate's type.
    /// \return Covariate's type.
    CovariateType getType() const { return m_type; }

    /// \brief Get the data type.
    /// \return Data type.
    DataType getDataType() const { return m_dataType; }

    /// \brief Get the interpolation type.
    /// \return Selected interpolation type.
    InterpolationType getInterpolationType() const { return m_interpolationType; }

    /// \brief Get the data unit.
    /// \return Data unit.
    Unit getUnit() const { return m_unit; }

    /// \brief Get the refresh period.
    /// \return Refresh period.
    Tucuxi::Common::Duration getRefreshPeriod() const { return m_refreshPeriod; }

    /// \brief Set the refresh period.
    /// \param _refreshPeriod Refresh period to set.
    void setRefreshPeriod(const Tucuxi::Common::Duration &_refreshPeriod) { m_refreshPeriod = _refreshPeriod; }

protected:
    CovariateType m_type;
    DataType m_dataType;
    InterpolationType m_interpolationType;
    Unit m_unit;
    Tucuxi::Common::Duration m_refreshPeriod;   // Only in the case of CovariateType::Interpolated
};

typedef std::vector<std::unique_ptr<CovariateDefinition> > CovariateDefinitions;


class PatientCovariate : public TimedEvent
{
    // TODO : Make variables protected and write accessors

    std::string m_value;
    std::string m_id;
    DataType m_dataType;
    Unit m_unit;
};

typedef std::vector<std::unique_ptr<PatientCovariate> > PatientVariates;

/// \brief Models a change of a covariate.
class CovariateEvent :  public IndividualValue<CovariateDefinition>, TimedEvent, Operable
{
public:
    CovariateEvent() = delete;
    CovariateEvent(const CovariateDefinition& _covariateDef, const DateTime& _date, Value _value)
        : IndividualValue(_covariateDef), TimedEvent(_date), Operable(_value), m_value(_value)
    {}

    Value getValue() { return m_value; }

private:
    Value m_value;

};

/// \brief List of covariate series (that is, changes).
typedef std::vector<CovariateEvent> CovariateSeries;

}
}

#endif // TUCUXI_CORE_COVARIATEEVENT_H
