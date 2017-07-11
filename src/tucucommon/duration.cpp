/*
* Copyright (C) 2017 Tucuxi SA
*/

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

float Duration::get() const
{
    return m_duration.count();
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
    //return std::chrono::duration_cast<std::chrono::seconds>(m_duration % _other.m_duration);
    return Duration();
}

const Duration Duration::operator%(float _divider) const
{
    //return std::chrono::duration_cast<std::chrono::seconds>(m_duration % _divider);
    return Duration();
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

}
}
