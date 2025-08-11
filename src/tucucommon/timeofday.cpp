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


#include "tucucommon/timeofday.h"

#include "date/date.h"

#include "tucucommon/duration.h"

namespace Tucuxi {
namespace Common {

TimeOfDay::TimeOfDay()
{
    std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
    date::sys_days today = date::floor<date::days>(now);
    m_time = now - today;
}


TimeOfDay::TimeOfDay(std::chrono::seconds& _time) : m_time(_time)
{
    normalize();
}


TimeOfDay::TimeOfDay(const Duration& _time)
    : m_time(
            static_cast<double>(_time.toMilliseconds())
            / 1000.0) // Initialization must be done in seconds but we use milliseconds for better precision
{
    normalize();
}


TimeOfDay TimeOfDay::buildUnnormalized(const Duration& _time)
{
    TimeOfDay result;
    result.m_time = std::chrono::duration<ChronoBaseType>(static_cast<double>(_time.toMilliseconds()) / 1000.0);
    return result;
}

int64 TimeOfDay::getDuration() const
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(m_time).count();
}

Duration TimeOfDay::getRealDuration() const
{
    return Duration(std::chrono::duration_cast<std::chrono::milliseconds>(m_time));
}


Duration TimeOfDay::operator-(const TimeOfDay& _time) const
{
    return Duration(std::chrono::duration_cast<std::chrono::milliseconds>(m_time - _time.m_time));
}


int TimeOfDay::hour() const
{
    return static_cast<int>(std::chrono::duration_cast<std::chrono::hours>(m_time).count());
}


int TimeOfDay::minute() const
{
    return static_cast<int>(std::chrono::duration_cast<std::chrono::minutes>(m_time).count() % 60);
}


int TimeOfDay::second() const
{
    return static_cast<int>(std::chrono::duration_cast<std::chrono::seconds>(m_time).count() % 60);
}


int TimeOfDay::millisecond() const
{
    return static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(m_time).count() % 1000);
}

bool TimeOfDay::operator<(const TimeOfDay& _other) const
{
    return m_time < _other.m_time;
}


bool TimeOfDay::operator>(const TimeOfDay& _other) const
{
    return m_time > _other.m_time;
}


bool TimeOfDay::operator>=(const TimeOfDay& _other) const
{
    return m_time >= _other.m_time;
}


bool TimeOfDay::operator<=(const TimeOfDay& _other) const
{
    return m_time <= _other.m_time;
}


bool TimeOfDay::operator==(const TimeOfDay& _other) const
{
    return m_time == _other.m_time;
}


bool TimeOfDay::operator!=(const TimeOfDay& _other) const
{
    return m_time != _other.m_time;
}


void TimeOfDay::normalize()
{
    int oneDay = 24 * 60 * 60 * 1000;
    int64 value = std::chrono::duration_cast<std::chrono::milliseconds>(m_time).count();
    if (value > oneDay) {
        value %= oneDay;
        m_time = std::chrono::duration<ChronoBaseType>(value / 1000);
    }
}

} // namespace Common
} // namespace Tucuxi
