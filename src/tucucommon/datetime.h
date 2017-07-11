/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_TUCUCOMMON_DATETIME_H
#define TUCUXI_TUCUCOMMON_DATETIME_H

#include <chrono>

namespace date {
class year_month_day;
}

namespace Tucuxi {
namespace Common {

class Duration;
class TimeOfDay;

class DateTime
{
public:
    /// Build a date and time object with the current time
    DateTime();

    DateTime(const date::year_month_day& _date);

    DateTime(const date::year_month_day& _date, const TimeOfDay& _time);

    /// \brief Returns the duration in one of the std::chrono duration class (hours, minutes, seconds, ...)    
    /// @return The duration in the specified T unit.
    template<class T> T get() const
    {
        return std::chrono::duration_cast<T>(m_duration);
    }

    date::year_month_day getDate() const;

    TimeOfDay getTimeOfDay() const;

    void setDate(const date::year_month_day& _newDate);

    void setTimeOfDay(const TimeOfDay& _newTime);

    const DateTime operator+(const Duration& _duration) const;

    DateTime& operator+=(const Duration& _duration);

    const DateTime operator-(const Duration& _duration) const;

    DateTime& operator-=(const Duration& _duration);

    const Duration operator-(const DateTime& _date) const;

private:
    std::chrono::time_point<std::chrono::system_clock> m_date;
};

}
}

#endif // TUCUXI_TUCUCOMMON_DATETIME_H