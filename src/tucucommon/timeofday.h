/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_TUCUCOMMON_TIME_H
#define TUCUXI_TUCUCOMMON_TIME_H

#include <chrono>
#include <date/date.h>

namespace Tucuxi {
namespace Common {

class Duration;

class TimeOfDay
{
public:
    TimeOfDay();

    TimeOfDay(const Duration& _time);

    template<class T> 
    TimeOfDay(const date::time_of_day<T>& _time) : m_time(_time.to_duration())
    {
    }

    /// \brief Returns the duration in one of the std::chrono duration class (hours, minutes, seconds, ...)    
    /// @return The duration in the specified T unit.
    template<class T> T getDuration() const
    {
        return std::chrono::duration_cast<T>(m_time);
    }

    template<class T> date::time_of_day<T> get() const
    {
        return date::time_of_day<T>(m_time);
    }

    const Duration operator-(const TimeOfDay& _time) const;

private:
    std::chrono::duration<float> m_time;
};

}
}

#endif // TUCUXI_TUCUCOMMON_TIME_H