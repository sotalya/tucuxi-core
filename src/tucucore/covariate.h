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

#include "drugmodel/covariatedefinition.h"

namespace Tucuxi {
namespace Core {

/// \brief Change of a covariate value for a patient.
class PatientCovariate : public TimedEvent
{
public:
    /// \brief Create a change of a covariate value for a patient.
    /// \param _id Identifier of the original covariate for which the change applies.
    /// \param _value Recorded value expressed in string form.
    /// \param _dataType Type of the data stored in the _value variable.
    /// \param _unit Unit of measure of the value.
    /// \param _date Time when the change happened.
    PatientCovariate(const std::string &_id, const std::string &_value, const DataType _dataType,
                     const Unit _unit, DateTime _date)
        : TimedEvent(_date), m_id{_id}, m_value{_value}, m_dataType{_dataType}, m_unit{_unit} {}

    /// \brief Return the identifier of the value.
    /// \return Value's identifier.
    std::string getId() const { return m_id;}

    /// \brief Get the value (as string).
    /// \return Returns the value.
    std::string getValue() const { return m_value; }

    /// \brief Get the data type.
    /// \return Data type.
    DataType getDataType() const { return m_dataType; }

    /// \brief Get the data's unit of measure.
    /// \return Data's unit of measure.
    Unit getUnit() const { return m_unit; }

protected:
    /// \brief Identifier of the original covariate for which the change applies.
    std::string m_id;
    /// \brief Recorded value of the change.
    std::string m_value;
    /// \brief Type of the data stored.
    DataType m_dataType;
    /// \brief Unit of measure of the value.
    Unit m_unit;
};

/// \brief List of patient variates.
typedef std::vector<std::unique_ptr<PatientCovariate>> PatientVariates;

/// \brief Iterator in the list of patient variates.
typedef std::vector<std::unique_ptr<PatientCovariate>>::const_iterator pvIterator_t;

/// \brief Model the change of a covariate.
class CovariateEvent :  public IndividualValue<CovariateDefinition>, public TimedEvent, public Operable
{
public:
    /// \brief Remove the default constructor.
    CovariateEvent() = delete;
    /// \brief Create a change in a covariate given a reference to the desired covariate, a time, and the new value.
    /// \param _covariateDef Covariate definition that is changed
    /// \param _date Time of change.
    /// \param _value New value of the covariate.
    CovariateEvent(const CovariateDefinition &_covariateDef, const DateTime &_date, Value _value)
        : IndividualValue(_covariateDef), TimedEvent(_date), Operable(_value),
          m_id(_covariateDef.getId()), m_value(_value)
    {}

    /// \brief Get the modified value of the covariate.
    /// \return Modified value of the covariate.
    Value getValue() const { return m_value; }

    /// \brief Return the identifier of the covariate involved in the change.
    /// \return Identifier of covariate involved in the change.
    std::string getId() const { return m_id;}

private:
    /// \brief Identifier of the covariate involved in the change
    std::string m_id;

    /// \brief New value of the covariate.
    Value m_value;
};

/// \brief List of covariate series (that is, changes).
typedef std::vector<CovariateEvent> CovariateSeries;

}
}

#endif // TUCUXI_CORE_COVARIATEEVENT_H
