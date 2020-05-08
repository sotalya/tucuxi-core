/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_TUCUCOMMON_TIME_H
#define TUCUXI_TUCUCOMMON_TIME_H

#include <chrono>

#undef min  // Prevent problems with date.h
#undef max  // Prevent problems with date.h
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

    /// \brief Returns the duration since 0h00 in milliseconds 
    /// \return The duration in milliseconds.
    int64 getDuration() const;

    /// \brief Returns the duration since 0h00 as a Duration object
    /// \return The duration object
    Duration getRealDuration() const;

    /// \brief Computes the duration between two times
    /// \param _time A time to compare to.
    /// \return The millisecond.
    Duration operator-(const TimeOfDay& _time) const;

    /// \brief Return the hour of the contained time
    /// \return The hour.
    int hour() const;

    /// \brief Return the minute of the contained time
    /// \return The minute.
    int minute() const;

    /// \brief Return the second of the contained time
    /// \return The second.
    int second() const;

    /// \brief Return the millisecond of the contained time
    /// \return The millisecond.
    int millisecond() const;

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
    std::chrono::duration<ChronoBaseType> m_time{};  /// The encapsulated time (a duration from 0h00)
};

} // namespace Common
} // namespace Tucuxi

#endif // TUCUXI_TUCUCOMMON_TIME_H
