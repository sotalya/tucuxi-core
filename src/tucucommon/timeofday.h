/*
 * Tucuxi - Tucuxi-core library and command line tool.
 * This code allows to perform prediction of drug concentration in blood
 * and to propose dosage adaptations.
 * It has been developed by HEIG-VD, in close collaboration with CHUV.
 * Copyright (C) 2023 HEIG-VD, maintained by Yann Thoma  <yann.thoma@heig-vd.ch>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */


#ifndef TUCUXI_TUCUCOMMON_TIME_H
#define TUCUXI_TUCUCOMMON_TIME_H

#include <chrono>

#undef min // Prevent problems with date.h
#undef max // Prevent problems with date.h
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

    /// \brief builds an unnormalized time of day (hours can be more than 23)
    /// \param _time A duration
    /// \return A TimeOfDay object where hours can be more than 23
    static TimeOfDay buildUnnormalized(const Duration& _time);

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

    std::chrono::duration<ChronoBaseType> m_time{}; /// The encapsulated time (a duration from 0h00)
};

} // namespace Common
} // namespace Tucuxi

#endif // TUCUXI_TUCUCOMMON_TIME_H
