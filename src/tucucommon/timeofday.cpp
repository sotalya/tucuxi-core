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
}

TimeOfDay::TimeOfDay(const Duration& _time)
    : m_time(_time.get<std::chrono::seconds>())
{
}

const Duration TimeOfDay::operator-(const TimeOfDay& _time) const
{
    return std::chrono::duration_cast<std::chrono::seconds>(m_time - _time.m_time);
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


}
}
