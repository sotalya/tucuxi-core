/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_TUCUCOMMON_DATETIME_H
#define TUCUXI_TUCUCOMMON_DATETIME_H

#include <chrono>

#include "tucucommon/basetypes.h"

namespace date {
class year_month_day;
}

namespace Tucuxi {
namespace Common {

class Duration;
class TimeOfDay;

/// \ingroup TucuCommon
/// \brief A class to manage a date and a time
/// The class takes advantages of the "date" library written by Howard Hinnant (https://github.com/HowardHinnant/date)
/// This allows intializing a DateTime object this way:
/// \code
/// DateTime date(2016_y/mar/17, 20h+3min+2s);
/// \endcode
/// It is possible to add/substract a Duration form a DateTime as well as substract two DateTime to obtain the Duration
/// between the two.
/// Methods year, month, day, hour, minute, second can be used to extract the different components of a date.
/// \sa TimeOfDay, Duration
class DateTime
{
public:
    /// \brief Build a date with the current date and time.
    DateTime();

    /// \brief Build a date with the specified data at 0h00.
    /// @param _date The date to be used. 
    DateTime(const date::year_month_day& _date);

    /// \brief Build a date and time with the specified data and time.
    /// @param _date The date to be used. 
    /// @param _time The time to be used. 
    DateTime(const date::year_month_day& _date, const TimeOfDay& _time);

    /// \brief Build a date and time with the specified data and time.
    /// @param _date The date to be used. 
    /// @param _time The time to be used (as a number of seconds since 0h00.
    DateTime(const date::year_month_day& _date, const std::chrono::seconds& _time);

    /// \brief Returns the duration in one of the std::chrono duration class (years, months, days, ...)    
    /// @return The duration in the specified T unit.
    template<class T> T get() const
    {
        return std::chrono::duration_cast<T>(m_date.time_since_epoch());
    }

    /// \brief Returns the date part of the object.
    date::year_month_day getDate() const;

    /// \brief Returns the time of the day part of the object.
    TimeOfDay getTimeOfDay() const;

    /// \brief Set the date part of the object.
    /// @param _newDate The date to be used. 
    void setDate(const date::year_month_day& _newDate);

    /// \brief Set the time of the day part of the object.
    /// @param _newTime The time to be used. 
    void setTimeOfDay(const TimeOfDay& _newTime);

    /// \brief Return a new DateTime object with the addition of the date and the specified duration.
    /// @param _duration The duration to be added. 
    /// @return The result of the addition.
    const DateTime operator+(const Duration& _duration) const;

    /// \brief Add a duration to the date.
    /// @param _duration The duration to be added. 
    /// @return A reference to the modified DateTime.
    DateTime& operator+=(const Duration& _duration);

    /// \brief Return a new DateTime object with the substraction of the date and the specified duration.
    /// @param _duration The duration to be substracted.
    /// @return The result of the substraction.
    const DateTime operator-(const Duration& _duration) const;

    /// \brief Substract a duration from the date.
    /// @param _duration The duration to be substracted. 
    /// @return A reference to the modified DateTime.
    DateTime& operator-=(const Duration& _duration);

    /// \brief Compute the duration between two dates.
    /// @param _date The other date to compute the difference with. 
    /// @return The computed duration.
    const Duration operator-(const DateTime& _date) const;

    /// \brief Is the date smaller?
    bool operator<(const DateTime& _other) const;

    /// \brief Is the date bigger?
    bool operator>(const DateTime& _other) const;

    /// \brief Is the date bigger or equal?
    bool operator>=(const DateTime& _other) const;

    /// \brief Is the date smaller or equal?
    bool operator<=(const DateTime& _other) const;

    /// \brief Are these dates equals?
    bool operator==(const DateTime& _other) const;

    /// \brief Are these dates not equals?
    bool operator!=(const DateTime& _other) const;

    /// \brief Return the year of the contained date
    /// @return The year.
    int year() const;

    /// \brief Return the month of the contained date
    /// @return The month.
    int month() const;

    /// \brief Return the day of the contained date
    /// @return The day.
    int day() const;

    /// \brief Return the hour of the contained date
    /// @return The hour.
    int hour() const;

    /// \brief Return the minute of the contained date
    /// @return The minute.
    int minute() const;

    /// \brief Return the second of the contained date
    /// @return The second.
    int64 second() const;

    /// \brief Return the millisecond of the contained date
    /// @return The millisecond.
    int64 millisecond() const;

    /// \brief Clear the date to internal value 0
    void reset();

    /// \brief Check if the value is empty (internal value is 0). See method Reset().
    /// @return Returns true if the internal value is 0.
    bool isUndefined() const;

private:
    std::chrono::time_point<std::chrono::system_clock> m_date;  /// The date managed by the class
};

}
}

#endif // TUCUXI_TUCUCOMMON_DATETIME_H