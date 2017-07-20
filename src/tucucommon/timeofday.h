/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_TUCUCOMMON_TIME_H
#define TUCUXI_TUCUCOMMON_TIME_H

#include <chrono>
#include <date/date.h>

#include "tucucommon/basetypes.h"

namespace Tucuxi {
namespace Common {

class Duration;

/// \ingroup TucuCommon
/// \brief A class to manage the time within a day
/// Methods hour, minute, second can be used to extract the different components of a date.
/// \sa DateTime, Duration
class TimeOfDay
{
public:
    /// \brief Default constructor. Use the current time for initialization.
    TimeOfDay();

    /// \brief Constructor from a Duration object
    /// In case the specifed duration is bigger than one day, we only use the remainder.
    TimeOfDay(const Duration& _time);

    /// \brief Constructor from a duration in seconds.
    /// In case the specifed duration is bigger than one day, we only use the remainder.
    TimeOfDay(std::chrono::seconds& _time);

    /// \brief Constructor from a time_of_day object.
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

    /// \brief Return the time as a time_of_day object.
    /// @return The time.
    template<class T> date::time_of_day<T> get() const
    {
        return date::time_of_day<T>(std::chrono::duration_cast<T>(m_time));
    }

    /// \brief Computes the duration between two times
    /// @param _time A time to compare to.
    /// @return The millisecond.
    const Duration operator-(const TimeOfDay& _time) const;

    /// \brief Return the hour of the contained time
    /// @return The hour.
    int hour() const;

    /// \brief Return the minute of the contained time
    /// @return The minute.
    int minute() const;

    /// \brief Return the second of the contained time
    /// @return The second.
    int64 second() const;

    /// \brief Return the millisecond of the contained time
    /// @return The millisecond.
    int64 millisecond() const;

    /// \brief Is the date smaller?
    bool operator<(const TimeOfDay& _other) const;

    /// \brief Is the date bigger?
    bool operator>(const TimeOfDay& _other) const;

    /// \brief Is the date bigger or equal?
    bool operator>=(const TimeOfDay& _other) const;

    /// \brief Is the date smaller or equal?
    bool operator<=(const TimeOfDay& _other) const;

    /// \brief Are these dates equal?
    bool operator==(const TimeOfDay& _other) const;

    /// \brief Are these dates not equal?
    bool operator!=(const TimeOfDay& _other) const;

private:
    /// \brief Make sure the time is not longer than one day.
    void normalize();

private:
    std::chrono::duration<float> m_time;  /// The encapsulated time (a duration from 0h00)
};

}
}

#endif // TUCUXI_TUCUCOMMON_TIME_H
