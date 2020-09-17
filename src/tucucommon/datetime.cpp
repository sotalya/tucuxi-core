/*
* Copyright (C) 2017 Tucuxi SA
*/

#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>

#undef min  // Prevent problems with date.h
#undef max  // Prevent problems with date.h
#include "date/date.h"

#include "tucucommon/timeofday.h"
#include "tucucommon/datetime.h"
#include "tucucommon/duration.h"

#ifdef _WIN32
#define timegm _mkgmtime
#endif

#ifdef EASY_DEBUG
#define UPDATESTRING updateString()
#else
#define UPDATESTRING
#endif // EASY_DEBUG

namespace Tucuxi {
namespace Common {

#ifdef EASY_DEBUG
void DateTime::updateString()
{
    std::stringstream str;
    str << day() << "/"
        << month() << "/"
        << year() << " "
        << hour() << ":"
        << minute() << ":"
        << second();
    m_dateString = str.str();
}

#endif

DateTime::DateTime()
    : m_date(std::chrono::system_clock::now())
{
    UPDATESTRING;
}


DateTime::DateTime(const std::string &_date, const std::string& _format)
{
    std::tm tm = {};
    std::stringstream ss(_date);
    // std::istringstream ss(_date);
    ss >> std::get_time(&tm, _format.c_str());
    if(ss.fail()){

        throw std::runtime_error("Date Time parsing failed");
    }
    else{
        date::year_month_day day = date::year_month_day(date::year(1900 + tm.tm_year),
                                                        date::month(static_cast<unsigned int>(tm.tm_mon + 1)),
                                                        date::day(static_cast<unsigned int>(tm.tm_mday)));

        m_date = date::sys_days(day);
        m_date += std::chrono::milliseconds(tm.tm_hour * 3600 * 1000 + tm.tm_min * 60000 + tm.tm_sec * 1000);
    }
    UPDATESTRING;
}


DateTime::DateTime(const date::year_month_day& _date)
    : m_date(date::sys_days(_date))
{
    UPDATESTRING;
}


DateTime::DateTime(const date::year_month_day& _date, const TimeOfDay& _time)
{
    m_date = date::sys_days(_date);
    m_date += std::chrono::milliseconds(_time.getDuration());
    UPDATESTRING;
}


DateTime::DateTime(const date::year_month_day& _date, const std::chrono::seconds& _time)
{
    m_date = date::sys_days(_date);
    m_date += _time;
    UPDATESTRING;
}


DateTime::DateTime(const Duration &_durationSinceEpoch)
{
    int64 ms = _durationSinceEpoch.toMilliseconds();
    const std::chrono::system_clock::duration d = std::chrono::milliseconds(ms);
    m_date = std::chrono::time_point<std::chrono::system_clock>(d);
    UPDATESTRING;
}

DateTime DateTime::getUndefined()
{
    return DateTime(Duration(std::chrono::milliseconds(0)));
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
    UPDATESTRING;
}


void DateTime::setTimeOfDay(const TimeOfDay& _newTime)
{
    m_date = date::floor<date::days>(m_date);
    m_date += std::chrono::milliseconds(_newTime.getDuration());
    UPDATESTRING;
}


void DateTime::addYears(int _nYears)
{    
    date::year_month_day curDate = DateTime::getDate();
    TimeOfDay t = getTimeOfDay();
    int years = year() + _nYears;
    m_date = date::sys_days(date::year_month_day(date::year(years), curDate.month(), curDate.day()));
    m_date += std::chrono::milliseconds(t.getDuration());
    UPDATESTRING;
}


void DateTime::addMonths(int _nMonths)
{
    date::year_month_day curDate = DateTime::getDate();
    TimeOfDay t = getTimeOfDay();
    int nMonths = year() *12 + month() + _nMonths;
    int nYears = nMonths / 12;
    nMonths = nMonths % 12;
    m_date = date::sys_days(date::year_month_day(date::year(nYears),
                                                 date::month(static_cast<unsigned int>(nMonths)),
                                                 curDate.day()));
    m_date += std::chrono::milliseconds(t.getDuration());
    UPDATESTRING;
}


void DateTime::addDays(int _nDays)
{
    m_date += std::chrono::hours(_nDays*24);
    UPDATESTRING;
}


DateTime DateTime::operator+(const Duration& _duration) const
{
    DateTime tmp(*this);
    tmp += _duration;
    return tmp;
}


DateTime DateTime::operator+(const DateTime& _dateTime) const
{
    DateTime tmp(*this);
    tmp += _dateTime.get<std::chrono::milliseconds>();
    return DateTime(*this);
}


DateTime& DateTime::operator+=(const Duration& _duration)
{
    m_date += std::chrono::milliseconds(_duration.toMilliseconds());
    UPDATESTRING;
    return *this;
}


DateTime& DateTime::operator+=(const DateTime& _dateTime)
{
    m_date += _dateTime.get<std::chrono::milliseconds>();
    UPDATESTRING;
    return *this;
}


DateTime DateTime::operator-(const Duration& _duration) const
{
    DateTime tmp(*this);
    tmp -= _duration;
    return tmp;
}


DateTime& DateTime::operator-=(const Duration& _duration)
{
    m_date -= std::chrono::milliseconds(_duration.toMilliseconds());
    UPDATESTRING;
    return *this;
}


Duration DateTime::operator-(const DateTime& _date) const
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
    return static_cast<int>(date::year_month_day(days).year());
}


int DateTime::month() const
{
    date::sys_days days = date::floor<date::days>(m_date);
    return static_cast<int>(static_cast<unsigned>(date::year_month_day(days).month()));
}


int DateTime::day() const
{
    date::sys_days days = date::floor<date::days>(m_date);
    return static_cast<int>(static_cast<unsigned>(date::year_month_day(days).day()));
}


int DateTime::hour() const
{
    date::sys_days days = date::floor<date::days>(m_date);
    return static_cast<int>(std::chrono::duration_cast<std::chrono::hours>(m_date - days).count());
}


int DateTime::minute() const
{
    date::sys_days days = date::floor<date::days>(m_date);
    return static_cast<int>(std::chrono::duration_cast<std::chrono::minutes>(m_date - days).count() % 60);
}


int DateTime::second() const
{
    date::sys_days days = date::floor<date::days>(m_date);
    return static_cast<int>(std::chrono::duration_cast<std::chrono::seconds>(m_date - days).count() % 60);
}


int DateTime::millisecond() const
{
    date::sys_days days = date::floor<date::days>(m_date);
    return static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(m_date - days).count() % 1000);
}


void DateTime::reset()
{
    m_date = std::chrono::time_point<std::chrono::system_clock>();
}


bool DateTime::isUndefined() const
{
    return (m_date.time_since_epoch().count() == 0);
}


double DateTime::toSeconds() const
{
    return static_cast<double>(get<std::chrono::seconds>().count());
}

double DateTime::toDays() const
{
    return std::floor(static_cast<double>(get<std::chrono::hours>().count()) / 24);
}

} // namespace Common
} // namespace Tucuxi
