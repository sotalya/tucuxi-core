/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "date/date.h"

#include "basetypes.h"
#include "duration.h"

namespace Tucuxi {
namespace Common {

Duration::Duration()
    : m_duration(m_duration.zero())
{
}


Duration::Duration(const std::chrono::hours &_value)
    : m_duration(_value)
{
}


Duration::Duration(std::chrono::minutes _value)
    : m_duration(_value)
{
}


Duration::Duration(std::chrono::seconds _value)
    : m_duration(_value)
{
}


Duration::Duration(std::chrono::milliseconds _value)
    : m_duration(_value)
{
}


bool Duration::isEmpty() const
{
    return (m_duration == m_duration.zero());
}


bool Duration::isNegative() const
{
    return (m_duration < m_duration.zero());
}


void Duration::clear()
{
    m_duration = m_duration.zero();
}


const Duration Duration::operator+(const Duration &_other) const
{
    return Duration(*this) += _other;
}


Duration& Duration::operator+=(const Duration &_other)
{
    m_duration += _other.m_duration;
    return *this;
}


const Duration Duration::operator-(const Duration &_other) const
{
    return Duration(*this) -= _other;
}


Duration& Duration::operator-=(const Duration &_other)
{
    m_duration -= _other.m_duration;
    return *this;
}


const Duration Duration::operator*(float _factor) const
{
    return Duration(*this) *= _factor;
}


Duration& Duration::operator*=(float _factor)
{
    m_duration *= _factor;
    return *this;
}


const Duration Duration::operator/(float _divider) const
{
    return Duration(*this) /= _divider;
}


Duration& Duration::operator/=(float _divider)
{
    m_duration /= _divider;
    return *this;
}


float Duration::operator/(const Duration &_other) const
{
    return m_duration / _other.m_duration;
}


const Duration Duration::operator%(const Duration &_other) const
{
    int64 n1 = std::chrono::duration_cast<std::chrono::seconds>(m_duration).count();
    int64 n2 = std::chrono::duration_cast<std::chrono::seconds>(_other.m_duration).count();
    return Duration(std::chrono::seconds(n1 % n2));
}


const Duration Duration::operator%(int _divider) const
{
    int64 n1 = std::chrono::duration_cast<std::chrono::seconds>(m_duration).count();
    return Duration(std::chrono::seconds(n1 % _divider));
}


bool Duration::operator<(const Duration &_other) const
{
    return m_duration < _other.m_duration;
}


bool Duration::operator>(const Duration &_other) const
{
    return m_duration > _other.m_duration;
}


bool Duration::operator>=(const Duration &_other) const
{
    return m_duration >= _other.m_duration;
}


bool Duration::operator<=(const Duration &_other) const
{
    return m_duration <= _other.m_duration;
}


bool Duration::operator==(const Duration &_other) const
{
    return m_duration == _other.m_duration;
}


bool Duration::operator!=(const Duration &_other) const
{
    return m_duration != _other.m_duration;
}

int64 Duration::toDays() const
{
    return get<date::days>().count();
}


int64 Duration::toHours() const
{
    return get<std::chrono::milliseconds>().count();
}


int64 Duration::toMinutes() const
{
    return get<std::chrono::milliseconds>().count();
}


int64 Duration::toSeconds() const
{
    return get<std::chrono::seconds>().count();
}


int64 Duration::toMilliseconds() const
{
    return get<std::chrono::milliseconds>().count();
}

}
}
