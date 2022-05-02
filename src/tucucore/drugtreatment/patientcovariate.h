//@@license@@

#ifndef PATIENTCOVARIATE_H
#define PATIENTCOVARIATE_H

#include <vector>

#include "tucucommon/duration.h"
#include "tucucommon/unit.h"
#include "tucucommon/utils.h"

#include "tucucore/definitions.h"
#include "tucucore/drugdefinitions.h"
#include "tucucore/operablegraphmanager.h"
#include "tucucore/operation.h"
#include "tucucore/timedevent.h"

namespace Tucuxi {
namespace Core {


///
/// \brief Change of a covariate value for a patient.
/// The value is saved as a string, as it can be any type (int, double, bool, Date).
/// Specifically the Date type requires such a special way of storing it.
/// It is then the responsibility of the user to cast and transform the covariate into
/// something meaningfull.
///
class PatientCovariate : public TimedEvent
{
public:
    /// \brief Create a change of a covariate value for a patient.
    /// \param _id Identifier of the original covariate for which the change applies.
    /// \param _value Recorded value expressed in string form.
    /// \param _dataType Type of the data stored in the _value variable.
    /// \param _unit Unit of measure of the value.
    /// \param _date Time when the change happened.
    PatientCovariate(std::string _id, std::string _value, const DataType _dataType, TucuUnit _unit, DateTime _date)
        : TimedEvent(_date), m_id{std::move(_id)}, m_value{std::move(_value)}, m_dataType{_dataType}, m_unit{std::move(
                                                                                                              _unit)}
    {
    }

    /// \brief Comparison (equality) operator.
    /// param _rhs PatientCovariate to compare to.
    bool operator==(const PatientCovariate& _rhs) const
    {
        // clang-format off
        return  this->getId() == _rhs.getId() &&
                this->getValue() == _rhs.getValue() &&
                this->getDataType() == _rhs.getDataType() &&
                this->getEventTime() == _rhs.getEventTime();
        // clang-format on
    }

    /// \brief Comparison (difference) operator.
    /// param _rhs PatientCovariate to compare to.
    bool operator!=(const PatientCovariate& _rhs) const
    {
        return !(*this == _rhs);
    }

    /// \brief Class' output operator.
    /// \param _output Output stream.
    /// \param _pv Self reference to the PatientCovariate to print.
    /// \return Output stream given as input (for output chaining).
    friend std::ostream& operator<<(std::ostream& _output, const PatientCovariate& _pv)
    {
        // clang-format off
        _output << "Patient Variate:\n\tID = " << _pv.getId()
                << "\n\tValue = " << _pv.getValue()
                << "\n\tDataType = " << static_cast<int>(_pv.getDataType())
                << "\n\tEventTime = " << _pv.getEventTime() << "\n";
        // clang-format on
        return _output;
    }

    /// \brief Return the identifier of the value.
    /// \return Value's identifier.
    std::string getId() const
    {
        return m_id;
    }

    /// \brief Get the value (as string).
    /// \return Returns the value.
    std::string getValue() const
    {
        return m_value;
    }

    /// \brief Get the data type.
    /// \return Data type.
    DataType getDataType() const
    {
        return m_dataType;
    }

    /// \brief Set the time the event happened.
    /// \return Time of the event.
    void setEventTime(const DateTime& _time)
    {
        m_time = _time;
    }

    /// \brief Get the data's unit of measure.
    /// \return Data's unit of measure.
    TucuUnit getUnit() const
    {
        return m_unit;
    }

    ///
    /// \brief Set the value as a DateTime. Useful for birthdate
    /// \param _date The date to be stored
    ///
    void setValueAsDate(DateTime _date)
    {
        m_value = Tucuxi::Common::Utils::varToString(_date);
    }

    ///
    /// \brief Get the value as a date. Useful for birthdate
    /// \return The date stored
    ///
    DateTime getValueAsDate()
    {
        return Tucuxi::Common::Utils::ValueToDate(Tucuxi::Common::Utils::stringToValue(m_value, DataType::Date));
    }

protected:
    /// \brief Identifier of the original covariate for which the change applies.
    std::string m_id;
    /// \brief Recorded value of the change.
    std::string m_value;
    /// \brief Type of the data stored.
    DataType m_dataType;
    /// \brief Unit of measure of the value.
    TucuUnit m_unit;
};

/// \brief List of patient variates.
typedef std::vector<std::unique_ptr<PatientCovariate>> PatientVariates;

/// \brief Iterator in the list of patient variates.
typedef std::vector<std::unique_ptr<PatientCovariate>>::const_iterator pvIterator_t;

} // namespace Core
} // namespace Tucuxi

#endif // PATIENTCOVARIATE_H
