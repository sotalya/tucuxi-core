/*
* Copyright (C) 2017 Tucuxi SA
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
