/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_TUCUCOMMON_DATETIME_H
#define TUCUXI_TUCUCOMMON_DATETIME_H

#include <chrono>
#include <iostream>

#include "tucucommon/basetypes.h"

namespace date {
    class year_month_day; // NOLINT(readability-identifier-naming)
} // namespace date

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
/// It is possible to add/substract a Duration from a DateTime as well as substract two DateTime to obtain the Duration
/// between the two.
/// Methods year, month, day, hour, minute, second can be used to extract the different components of a date.
/// \sa TimeOfDay, Duration
class DateTime
{
public:
    /// \brief Build a date with the current date and time.
    DateTime();

    /// \brief Build a date from the specified string
    /// This constructor uses std::get_time internally. See get_time documentation for information about the _format  parameter.
    /// \param _date The string to be parsed
    /// \param _date The parsing format (see std::get_time documentation). 
    DateTime(const std::string &_date, const std::string& _format);

    /// \brief Build a date with the specified data at 0h00.
    /// \param _date The date to be used. 
    DateTime(const date::year_month_day& _date);

    /// \brief Build a date and time with the specified data and time.
    /// \param _date The date to be used. 
    /// \param _time The time to be used. 
    DateTime(const date::year_month_day& _date, const TimeOfDay& _time);

    /// \brief Build a date and time with the specified data and time.
    /// \param _date The date to be used. 
    /// \param _time The time to be used (as a number of seconds since 0h00.
    DateTime(const date::year_month_day& _date, const std::chrono::seconds& _time);

    /// \brief Build a date and time with the specified duration 
    /// \param _date The duration since epoch (see documentation of std::chrono::time_point)
    DateTime(const Duration &_durationSinceEpoch);

    static DateTime getUndefined();

    /// \brief Returns the date part of the object.
    date::year_month_day getDate() const;

    /// \brief Returns the time of the day part of the object.
    TimeOfDay getTimeOfDay() const;

    /// \brief Set the date part of the object.
    /// \param _newDate The date to be used. 
    void setDate(const date::year_month_day& _newDate);

    /// \brief Set the time of the day part of the object.
    /// \param _newTime The time to be used. 
    void setTimeOfDay(const TimeOfDay& _newTime);

    /// \brief Add the specified number of years to the date.
    /// \param _nYears The number of years to be added 
    void addYears(int _nYears);

    /// \brief Add the specified number of months to the date.
    /// \param _nMonths The number of months to be added 
    void addMonths(int _nMonths);

    /// \brief Add the specified number of days to the date.
    /// \param _nDays The number of days to be added 
    void addDays(int _nDays);

    /// \brief Return a new DateTime object with the addition of the date and the specified duration.
    /// \param _duration The duration to be added. 
    /// \return The result of the addition.
    DateTime operator+(const Duration& _duration) const;

    /// \brief Return a new DateTime object with the addition of the date and the specified date.
    /// \param _dateTime The date to be added.
    /// \return The result of the addition.
    DateTime operator+(const DateTime& _dateTime) const;

    /// \brief Add a duration to the date.
    /// \param _duration The duration to be added. 
    /// \return A reference to the modified DateTime.
    DateTime& operator+=(const Duration& _duration);

    /// \brief Add another date to the date.
    /// \param _dateTime The date to be added.
    /// \return A reference to the modified DateTime.
    DateTime& operator+=(const DateTime& _dateTime);

    /// \brief Return a new DateTime object with the substraction of the date and the specified duration.
    /// \param _duration The duration to be substracted.
    /// \return The result of the substraction.
    DateTime operator-(const Duration& _duration) const;

    /// \brief Substract a duration from the date.
    /// \param _duration The duration to be substracted. 
    /// \return A reference to the modified DateTime.
    DateTime& operator-=(const Duration& _duration);

    /// \brief Compute the duration between two dates.
    /// \param _date The other date to compute the difference with. 
    /// \return The computed duration.
    Duration operator-(const DateTime& _date) const;

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
    /// \return The year.
    int year() const;

    /// \brief Return the month of the contained date
    /// \return The month.
    int month() const;

    /// \brief Return the day of the contained date
    /// \return The day.
    int day() const;

    /// \brief Return the hour of the contained date
    /// \return The hour.
    int hour() const;

    /// \brief Return the minute of the contained date
    /// \return The minute.
    int minute() const;

    /// \brief Return the second of the contained date
    /// \return The second.
    int second() const;

    /// \brief Return the millisecond of the contained date
    /// \return The millisecond.
    int millisecond() const;

    /// \brief Clear the date to internal value 0
    void reset();

    /// \brief Check if the value is empty (internal value is 0). See method Reset().
    /// \return Returns true if the internal value is 0.
    bool isUndefined() const;

    /// \brief Retrieve the duration as a number of seconds.
    /// \return The number of seconds representing the duration.
    double toSeconds() const;

    /// \brief Convert a date in days.
    /// \return Number of days representing the duration.
    double toDays() const;

    /// \brief Class' output operator.
    /// \param _output Output stream.
    /// \param _dt Self reference to the DateTime to print.
    /// \return Output stream given as input (for output chaining).
    friend std::ostream& operator<<(std::ostream &_output, const DateTime &_dt) {
        _output << _dt.day() << "."
                << _dt.month() << "."
                << _dt.year() << " "
                << _dt.hour() << "h"
                << _dt.minute() << "m"
                << _dt.second() << "s";
        return _output;
    }

private:

    /// \brief Returns the duration in one of the std::chrono duration class (years, months, days, ...)    
    /// \return The duration in the specified T unit.
    template<class T> T get() const
    {
        return std::chrono::duration_cast<T>(m_date.time_since_epoch());
    }

private:
    std::chrono::time_point<std::chrono::system_clock> m_date;  /// The date managed by the class

#ifdef EASY_DEBUG
    std::string m_dateString;

    void updateString();
#endif // EASY_DEBUG

};

} // namespace Common
} // namespace Tucuxi

#endif // TUCUXI_TUCUCOMMON_DATETIME_H
