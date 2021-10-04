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

namespace Tucuxi {
namespace Common {

#ifdef EASY_DEBUG
#define UPDATESTRING updateString()
#else
#define UPDATESTRING
#endif // EASY_DEBUG

#ifdef CHECK_DATETIME
static bool sm_enableChecks = true;

void DateTime::enableChecks()
{
    sm_enableChecks = true;
}

void DateTime::disableChecks()
{
    sm_enableChecks = false;
}

/// This function is just useful to place a breakpoint during debugging
void errorUndefinedDateTime(const DateTime &/*_date*/)
{
    std::cerr << "Error : Using an undefined DateTime" << std::endl;
}

#define SETDEFINED(value) {m_isDefined = value;}
#define CHECKDEFINED {if (sm_enableChecks && this->isUndefined()){errorUndefinedDateTime(*this);throw std::runtime_error("Date Time used but undefined");}}
#define CHECKOTHERDEFINED {if (sm_enableChecks && _other.isUndefined()) {errorUndefinedDateTime(_other);throw std::runtime_error("Date Time used but undefined");}}
#else // CHECK_DATETIME
#define SETDEFINED(value) {m_isDefined = value;}
#define CHECKDEFINED
#define CHECKOTHERDEFINED
void DateTime::enableChecks() {}
void DateTime::disableChecks() {}
#endif // CHECK_DATETIME



#ifdef EASY_DEBUG
void DateTime::updateString()
{
    if (!m_isDefined) {
        m_dateString = "Undefined date time. Should not be used before proper initialization";
        return;
    }
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
    : m_date(std::chrono::time_point<std::chrono::system_clock>())
{
    m_isDefined = false;
    UPDATESTRING;
}


DateTime::DateTime(std::chrono::time_point<std::chrono::system_clock> _clockTime) : m_date(_clockTime)
{
    m_isDefined = true;
    UPDATESTRING;
}

DateTime DateTime::undefinedDateTime()
{
    auto date = DateTime(std::chrono::time_point<std::chrono::system_clock>());
    date.m_isDefined = false;
    return date;
}


DateTime DateTime::now()
{
    auto date = DateTime(std::chrono::system_clock::now());
    date.m_isDefined = true;
    return date;
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
    date::year_month_day day = date::year_month_day(date::year(1900 + tm.tm_year),
                                                    date::month(static_cast<unsigned int>(tm.tm_mon + 1)),
                                                    date::day(static_cast<unsigned int>(tm.tm_mday)));

    m_date = date::sys_days(day);
    m_date += std::chrono::milliseconds(tm.tm_hour * 3600 * 1000 + tm.tm_min * 60000 + tm.tm_sec * 1000);
    SETDEFINED(true);
    UPDATESTRING;
}


DateTime::DateTime(const date::year_month_day& _date)
    : m_date(date::sys_days(_date))
{
    SETDEFINED(true);
    UPDATESTRING;
}


DateTime::DateTime(const date::year_month_day& _date, const TimeOfDay& _time)
{
    m_date = date::sys_days(_date);
    m_date += std::chrono::milliseconds(_time.getDuration());
    SETDEFINED(true);
    UPDATESTRING;
}


DateTime::DateTime(const date::year_month_day& _date, const std::chrono::seconds& _time)
{
    m_date = date::sys_days(_date);
    m_date += _time;
    SETDEFINED(true);
    UPDATESTRING;
}


DateTime::DateTime(const Duration &_durationSinceEpoch)
{
    int64 ms = _durationSinceEpoch.toMilliseconds();
    const std::chrono::system_clock::duration d = std::chrono::milliseconds(ms);
    m_date = std::chrono::time_point<std::chrono::system_clock>(d);
    SETDEFINED(true);
    UPDATESTRING;
}


date::year_month_day DateTime::getDate() const
{
    CHECKDEFINED;
    return date::year_month_day(date::floor<date::days>(m_date));
}


TimeOfDay DateTime::getTimeOfDay() const
{
    // Not sure we should keep it without a check
//    CHECKDEFINED;
    date::sys_days today = date::floor<date::days>(m_date);
    return TimeOfDay(Duration(std::chrono::duration_cast<std::chrono::milliseconds>(m_date - today)));
}


void DateTime::setDate(const date::year_month_day& _newDate)
{
    // Do not check here, as we could use this function on an undefined datetime
    // CHECKDEFINED;
    TimeOfDay tod = getTimeOfDay();
    m_date = date::sys_days(_newDate);
    m_date += std::chrono::milliseconds(tod.getDuration());
    // The next line is only partially true, but we consider it enough
    SETDEFINED(true);
    UPDATESTRING;
}


void DateTime::setTimeOfDay(const TimeOfDay& _newTime)
{
    // Do not check here, as we could use this function on an undefined datetime
    // CHECKDEFINED;
    m_date = date::floor<date::days>(m_date);
    m_date += std::chrono::milliseconds(_newTime.getDuration());
    // The next line is only partially true, but we consider it enough
    SETDEFINED(true);
    UPDATESTRING;
}


void DateTime::addYears(int _nYears)
{    
    CHECKDEFINED;
    date::year_month_day curDate = DateTime::getDate();
    TimeOfDay t = getTimeOfDay();
    int years = year() + _nYears;
    m_date = date::sys_days(date::year_month_day(date::year(years), curDate.month(), curDate.day()));
    m_date += std::chrono::milliseconds(t.getDuration());
    UPDATESTRING;
}


void DateTime::addMonths(int _nMonths)
{
    CHECKDEFINED;
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
    CHECKDEFINED;
    m_date += std::chrono::hours(_nDays*24);
    UPDATESTRING;
}


DateTime DateTime::operator+(const Duration& _duration) const
{
    CHECKDEFINED;
    DateTime tmp(*this);
    tmp += _duration;
    return tmp;
}


DateTime DateTime::operator+(const DateTime& _dateTime) const
{
    CHECKDEFINED;
    DateTime tmp(*this);
    tmp += _dateTime.get<std::chrono::milliseconds>();
    return DateTime(*this);
}


DateTime& DateTime::operator+=(const Duration& _duration)
{
    CHECKDEFINED;
    m_date += std::chrono::milliseconds(_duration.toMilliseconds());
    UPDATESTRING;
    return *this;
}


DateTime& DateTime::operator+=(const DateTime& _dateTime)
{
    CHECKDEFINED;
    m_date += _dateTime.get<std::chrono::milliseconds>();
    UPDATESTRING;
    return *this;
}


DateTime DateTime::operator-(const Duration& _duration) const
{
    CHECKDEFINED;
    DateTime tmp(*this);
    tmp -= _duration;
    return tmp;
}


DateTime& DateTime::operator-=(const Duration& _duration)
{
    CHECKDEFINED;
    m_date -= std::chrono::milliseconds(_duration.toMilliseconds());
    UPDATESTRING;
    return *this;
}


Duration DateTime::operator-(const DateTime& _date) const
{
    CHECKDEFINED;
    return Duration(std::chrono::duration_cast<std::chrono::milliseconds>(m_date - _date.m_date));
}


bool DateTime::operator<(const DateTime& _other) const
{
    CHECKDEFINED;
    CHECKOTHERDEFINED;
    return m_date < _other.m_date;
}


bool DateTime::operator>(const DateTime& _other) const
{
    CHECKDEFINED;
    CHECKOTHERDEFINED;
    return m_date > _other.m_date;
}


bool DateTime::operator>=(const DateTime& _other) const
{
    CHECKDEFINED;
    CHECKOTHERDEFINED;
    return m_date >= _other.m_date;
}


bool DateTime::operator<=(const DateTime& _other) const
{
    CHECKDEFINED;
    CHECKOTHERDEFINED;
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
    CHECKDEFINED;
    date::sys_days days = date::floor<date::days>(m_date);
    return static_cast<int>(date::year_month_day(days).year());
}


int DateTime::month() const
{
    CHECKDEFINED;
    date::sys_days days = date::floor<date::days>(m_date);
    return static_cast<int>(static_cast<unsigned>(date::year_month_day(days).month()));
}


int DateTime::day() const
{
    CHECKDEFINED;
    date::sys_days days = date::floor<date::days>(m_date);
    return static_cast<int>(static_cast<unsigned>(date::year_month_day(days).day()));
}


int DateTime::hour() const
{
    CHECKDEFINED;
    date::sys_days days = date::floor<date::days>(m_date);
    return static_cast<int>(std::chrono::duration_cast<std::chrono::hours>(m_date - days).count());
}


int DateTime::minute() const
{
    CHECKDEFINED;
    date::sys_days days = date::floor<date::days>(m_date);
    return static_cast<int>(std::chrono::duration_cast<std::chrono::minutes>(m_date - days).count() % 60);
}


int DateTime::second() const
{
    CHECKDEFINED;
    date::sys_days days = date::floor<date::days>(m_date);
    return static_cast<int>(std::chrono::duration_cast<std::chrono::seconds>(m_date - days).count() % 60);
}


int DateTime::millisecond() const
{
    CHECKDEFINED;
    date::sys_days days = date::floor<date::days>(m_date);
    return static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(m_date - days).count() % 1000);
}


void DateTime::reset()
{
    m_date = std::chrono::time_point<std::chrono::system_clock>();
    SETDEFINED(false);
}


bool DateTime::isUndefined() const
{
    return !m_isDefined;
}


double DateTime::toSeconds() const
{
    CHECKDEFINED;
    return static_cast<double>(get<std::chrono::seconds>().count());
}

double DateTime::toDays() const
{
    CHECKDEFINED;
    return std::floor(static_cast<double>(get<std::chrono::hours>().count()) / 24);
}

} // namespace Common
} // namespace Tucuxi
