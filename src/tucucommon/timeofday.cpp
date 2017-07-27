/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "date/date.h"
#include "tucucommon/duration.h"
#include "tucucommon/timeofday.h"

namespace Tucuxi {
namespace Common {

TimeOfDay::TimeOfDay()
{
    std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
    date::sys_days today = date::floor<date::days>(now);    
    m_time = now - today;
}


TimeOfDay::TimeOfDay(std::chrono::seconds& _time)
    : m_time(_time)
{
    normalize();
}


TimeOfDay::TimeOfDay(const Duration& _time)
    : m_time(_time.toMilliseconds())
{
    normalize();
}


int64 TimeOfDay::getDuration() const
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(m_time).count();
}


const Duration TimeOfDay::operator-(const TimeOfDay& _time) const
{
    return Duration(std::chrono::duration_cast<std::chrono::milliseconds>(m_time - _time.m_time));
}


int TimeOfDay::hour() const
{
    return std::chrono::duration_cast<std::chrono::hours>(m_time).count();
}


int TimeOfDay::minute() const
{
    return std::chrono::duration_cast<std::chrono::minutes>(m_time).count() % 60;
}


int64 TimeOfDay::second() const
{
    return std::chrono::duration_cast<std::chrono::seconds>(m_time).count() % 60;
}


int64 TimeOfDay::millisecond() const
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(m_time).count() % 1000;
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
    if (value > oneDay)
    {
        value %= oneDay;
        m_time = std::chrono::duration<ChronoBaseType>(value/1000);
    }
}

}
}
