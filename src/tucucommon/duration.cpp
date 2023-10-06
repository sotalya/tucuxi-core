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


#include "duration.h"

#include "date/date.h"

#include "basetypes.h"

namespace Tucuxi {
namespace Common {


#ifdef EASY_DEBUG
void Duration::updateDurationString()
{
    std::stringstream str;
    int hours = static_cast<int>(toHours());
    int minutes = static_cast<int>(toMinutes()) - hours * 60;
    int seconds = static_cast<int>(toSeconds()) - hours * 3600 - minutes * 60;
    str << hours << ":" << minutes << ":" << seconds;
    m_durationString = str.str();
}

#define UPDATESTRING updateDurationString()

#else
#define UPDATESTRING
#endif // EASY_DEBUG


Duration::Duration() : m_duration(std::chrono::duration<ChronoBaseType>::zero())
{
    UPDATESTRING;
}


Duration::Duration(const date::years& _value) : m_duration(_value)
{
    UPDATESTRING;
}


Duration::Duration(const date::months& _value) : m_duration(_value)
{
    UPDATESTRING;
}


Duration::Duration(const date::days& _value) : m_duration(_value)
{
    UPDATESTRING;
}


Duration::Duration(const std::chrono::hours& _value) : m_duration(_value)
{
    UPDATESTRING;
}


Duration::Duration(const std::chrono::minutes& _value) : m_duration(_value)
{
    UPDATESTRING;
}


Duration::Duration(const std::chrono::seconds& _value) : m_duration(_value)
{
    UPDATESTRING;
}


Duration::Duration(const std::chrono::milliseconds& _value) : m_duration(_value)
{
    UPDATESTRING;
}

Duration::Duration(
        const std::chrono::hours& _hours, const std::chrono::minutes& _minutes, const std::chrono::seconds& _seconds)
    : m_duration(std::chrono::seconds(_hours) + std::chrono::seconds(_minutes) + _seconds)
{
    UPDATESTRING;
}

bool Duration::isEmpty() const
{
    return (m_duration == std::chrono::duration<ChronoBaseType>::zero());
}


bool Duration::isNegative() const
{
    return (m_duration < std::chrono::duration<ChronoBaseType>::zero());
}


void Duration::clear()
{
    m_duration = std::chrono::duration<ChronoBaseType>::zero();
    UPDATESTRING;
}


Duration Duration::operator+(const Duration& _other) const
{
    return Duration(*this) += _other;
}


Duration& Duration::operator+=(const Duration& _other)
{
    m_duration += _other.m_duration;
    UPDATESTRING;
    return *this;
}


Duration Duration::operator-(const Duration& _other) const
{
    return Duration(*this) -= _other;
}


Duration& Duration::operator-=(const Duration& _other)
{
    m_duration -= _other.m_duration;
    UPDATESTRING;
    return *this;
}


Duration Duration::operator*(double _factor) const
{
    return Duration(*this) *= _factor;
}


Duration& Duration::operator*=(double _factor)
{
    m_duration *= _factor;
    UPDATESTRING;
    return *this;
}


Duration Duration::operator/(double _divider) const
{
    return Duration(*this) /= _divider;
}


Duration& Duration::operator/=(double _divider)
{
    m_duration /= _divider;
    UPDATESTRING;
    return *this;
}


double Duration::operator/(const Duration& _other) const
{
    return m_duration / _other.m_duration;
}


Duration Duration::operator%(const Duration& _other) const
{
    int64 n1 = std::chrono::duration_cast<std::chrono::seconds>(m_duration).count();
    int64 n2 = std::chrono::duration_cast<std::chrono::seconds>(_other.m_duration).count();
    return Duration(std::chrono::seconds(n1 % n2));
}


Duration Duration::operator%(int _divider) const
{
    int64 n1 = std::chrono::duration_cast<std::chrono::seconds>(m_duration).count();
    return Duration(std::chrono::seconds(n1 % _divider));
}


bool Duration::operator<(const Duration& _other) const
{
    return m_duration < _other.m_duration;
}


bool Duration::operator>(const Duration& _other) const
{
    return m_duration > _other.m_duration;
}


bool Duration::operator>=(const Duration& _other) const
{
    return m_duration >= _other.m_duration;
}


bool Duration::operator<=(const Duration& _other) const
{
    return m_duration <= _other.m_duration;
}


bool Duration::operator==(const Duration& _other) const
{
    return m_duration == _other.m_duration;
}


bool Duration::operator!=(const Duration& _other) const
{
    return m_duration != _other.m_duration;
}


int64 Duration::toYears() const
{
    return get<date::years>().count();
}


int64 Duration::toMonths() const
{
    return get<date::months>().count();
}


double Duration::toDays() const
{
    return static_cast<double>(get<std::chrono::milliseconds>().count()) / (24 * 60 * 60 * 1000);
}


double Duration::toHours() const
{
    return static_cast<double>(get<std::chrono::milliseconds>().count()) / (60 * 60 * 1000);
}


double Duration::toMinutes() const
{
    return static_cast<double>(get<std::chrono::milliseconds>().count()) / (60 * 1000);
}


double Duration::toSeconds() const
{
    return static_cast<double>(get<std::chrono::milliseconds>().count()) / 1000;
}


int64 Duration::toMilliseconds() const
{
    return get<std::chrono::milliseconds>().count();
}

} // namespace Common
} // namespace Tucuxi
