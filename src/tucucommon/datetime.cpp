/*
* Copyright (C) 2017 Tucuxi SA
*/

#include <string>
#include <sstream>
#include <iomanip>

#include "date/date.h"

#include "tucucommon/timeofday.h"
#include "tucucommon/datetime.h"
#include "tucucommon/duration.h"

#ifdef _WIN32
#define timegm _mkgmtime
#endif

namespace Tucuxi {
namespace Common {

DateTime::DateTime()
    : m_date(std::chrono::system_clock::now())
{
}


DateTime::DateTime(const std::string &_date, const std::string& _format)
{
    std::tm tm = {};
    std::stringstream ss(_date);
    ss >> std::get_time(&tm, _format.c_str());
    time_t t = timegm(&tm);
    m_date = std::chrono::system_clock::from_time_t(t);
}


DateTime::DateTime(const date::year_month_day& _date)
    : m_date(date::sys_days(_date))
{
}


DateTime::DateTime(const date::year_month_day& _date, const TimeOfDay& _time)
{
    m_date = date::sys_days(_date);
    m_date += std::chrono::milliseconds(_time.getDuration());
}


DateTime::DateTime(const date::year_month_day& _date, const std::chrono::seconds& _time)
{
    m_date = date::sys_days(_date);
    m_date += _time;
}


date::year_month_day DateTime::getDate() const
{
    return date::year_month_day(date::floor<date::days>(m_date));
}


TimeOfDay DateTime::getTimeOfDay() const
{
    date::sys_days today = date::floor<date::days>(m_date);
    return TimeOfDay(Duration(std::chrono::duration_cast<std::chrono::milliseconds>(m_date - today)));
}


void DateTime::setDate(const date::year_month_day& _newDate)
{
    TimeOfDay tod = DateTime::getTimeOfDay();
    m_date = date::sys_days(_newDate);
    m_date += std::chrono::milliseconds(tod.getDuration());
}


void DateTime::setTimeOfDay(const TimeOfDay& _newTime)
{
    m_date = date::floor<date::days>(m_date);
    m_date += std::chrono::milliseconds(_newTime.getDuration());
}


const DateTime DateTime::operator+(const Duration& _duration) const
{
    DateTime tmp(*this);
    tmp += _duration;
    return tmp;
}


const DateTime DateTime::operator+(const DateTime& _dateTime) const
{
    DateTime tmp(*this);
    tmp += _dateTime.get<std::chrono::milliseconds>();
    return DateTime(*this);
}


DateTime& DateTime::operator+=(const Duration& _duration)
{
    m_date += std::chrono::milliseconds(_duration.toMilliseconds());
    return *this;
}


DateTime& DateTime::operator+=(const DateTime& _dateTime)
{
    m_date += _dateTime.get<std::chrono::milliseconds>();
    return *this;
}


const DateTime DateTime::operator-(const Duration& _duration) const
{
    DateTime tmp(*this);
    tmp -= _duration;
    return tmp;
}


DateTime& DateTime::operator-=(const Duration& _duration)
{
    m_date -= std::chrono::milliseconds(_duration.toMilliseconds());
    return *this;
}


const Duration DateTime::operator-(const DateTime& _date) const
{
    return Duration(std::chrono::duration_cast<std::chrono::milliseconds>(m_date - _date.m_date));
}


bool DateTime::operator<(const DateTime& _other) const
{
    return m_date < _other.m_date;
}


bool DateTime::operator>(const DateTime& _other) const
{
    return m_date > _other.m_date;
}


bool DateTime::operator>=(const DateTime& _other) const
{
    return m_date >= _other.m_date;
}


bool DateTime::operator<=(const DateTime& _other) const
{
    return m_date <= _other.m_date;
}


bool DateTime::operator==(const DateTime& _other) const
{
    return m_date == _other.m_date;
}


bool DateTime::operator!=(const DateTime& _other) const
{
    return m_date != _other.m_date;
}


int DateTime::year() const
{
    date::sys_days days = date::floor<date::days>(m_date);
    return (int)date::year_month_day(days).year();
}


int DateTime::month() const
{
    date::sys_days days = date::floor<date::days>(m_date);
    return (unsigned)date::year_month_day(days).month();
}


int DateTime::day() const
{
    date::sys_days days = date::floor<date::days>(m_date);
    return (unsigned)date::year_month_day(days).day();
}


int DateTime::hour() const
{
    date::sys_days days = date::floor<date::days>(m_date);
    return std::chrono::duration_cast<std::chrono::hours>(m_date - days).count();
}


int DateTime::minute() const
{
    date::sys_days days = date::floor<date::days>(m_date);
    return std::chrono::duration_cast<std::chrono::minutes>(m_date - days).count() % 60;
}


int64 DateTime::second() const
{
    date::sys_days days = date::floor<date::days>(m_date);
    return std::chrono::duration_cast<std::chrono::seconds>(m_date - days).count() % 60;
}


int64 DateTime::millisecond() const
{
    date::sys_days days = date::floor<date::days>(m_date);
    return std::chrono::duration_cast<std::chrono::milliseconds>(m_date - days).count() % 1000;
}


void DateTime::reset()
{
    m_date = std::chrono::time_point<std::chrono::system_clock>();
}


bool DateTime::isUndefined() const
{
    return (m_date.time_since_epoch().count() == 0);
}


}
}
