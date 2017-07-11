/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "date/date.h"
#include "tucucommon/timeofday.h"
#include "tucucommon/datetime.h"
#include "tucucommon/duration.h"

namespace Tucuxi {
namespace Common {

DateTime::DateTime()
    : m_date(std::chrono::system_clock::now())
{
}

DateTime::DateTime(const date::year_month_day& _date)
    : m_date(date::sys_days(_date))
{
}

DateTime::DateTime(const date::year_month_day& _date, const TimeOfDay& _time)
{
    m_date = date::sys_days(_date);
    m_date += _time.getDuration<std::chrono::seconds>();
}

date::year_month_day DateTime::getDate() const
{
    return date::year_month_day(date::floor<date::days>(m_date));
}

TimeOfDay DateTime::getTimeOfDay() const
{
    date::sys_days today = date::floor<date::days>(m_date);
    return std::chrono::duration_cast<std::chrono::seconds>(m_date - today);
}

void DateTime::setDate(const date::year_month_day& _newDate)
{
    TimeOfDay tod = DateTime::getTimeOfDay();
    m_date = date::sys_days(_newDate);
    m_date += tod.getDuration<std::chrono::seconds>();
}

void DateTime::setTimeOfDay(const TimeOfDay& _newTime)
{
    m_date = date::floor<date::days>(m_date);
    m_date += _newTime.getDuration<std::chrono::seconds>();
}

const DateTime DateTime::operator+(const Duration& _duration) const
{
    return DateTime(*this) += _duration.get<std::chrono::seconds>();
}

DateTime& DateTime::operator+=(const Duration& _duration)
{
    m_date += _duration.get<std::chrono::seconds>();
    return *this;
}

const DateTime DateTime::operator-(const Duration& _duration) const
{
    return DateTime(*this) -= _duration.get<std::chrono::seconds>();
}

DateTime& DateTime::operator-=(const Duration& _duration)
{
    m_date -= _duration.get<std::chrono::seconds>();
    return *this;
}

const Duration DateTime::operator-(const DateTime& _date) const
{
    return Duration(std::chrono::duration_cast<std::chrono::seconds>(m_date - _date.m_date));
}

}
}