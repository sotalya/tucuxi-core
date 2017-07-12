
#include "fructose/fructose.h"

#include "date/date.h"

#include "tucucommon/datetime.h"
#include "tucucommon/timeofday.h"
#include "tucucommon/duration.h"

using namespace std::chrono_literals;
using namespace date;

struct TestDateTime : public fructose::test_base<TestDateTime>
{
    void datetime(const std::string& _testName)
    {
        std::cout << _testName << std::endl;

        // Test constructors and getter methods
        Tucuxi::Common::DateTime d1(2017_y / jan / 1);
        Tucuxi::Common::DateTime d2(2018_y / jan / 1);
        Tucuxi::Common::DateTime d3(2012_y / feb / 22, 22h);
        Tucuxi::Common::DateTime d4(2013_y / mar / 23, 23min);
        Tucuxi::Common::DateTime d5(2014_y / apr / 24, 22h + 23min);
        Tucuxi::Common::DateTime d6(2015_y / may / 25, 24s);
        Tucuxi::Common::DateTime d7(2016_y / jun / 26, 23min + 24s);
        Tucuxi::Common::DateTime d8(2017_y / jul / 27, 22h + 23min + 24s);
        Tucuxi::Common::DateTime d9(2018_y / aug / 28, Tucuxi::Common::TimeOfDay(22h + 23min + 24s));
        checkDateTime(d1, 2017, 1, 1, 0, 0, 0);
        checkDateTime(d2, 2018, 1, 1, 0, 0, 0);
        checkDateTime(d3, 2012, 2, 22, 22, 0, 0);
        checkDateTime(d4, 2013, 3, 23, 0, 23, 0);
        checkDateTime(d5, 2014, 4, 24, 22, 23, 0);
        checkDateTime(d6, 2015, 5, 25, 0, 0, 24);
        checkDateTime(d7, 2016, 6, 26, 0, 23, 24);
        checkDateTime(d8, 2017, 7, 27, 22, 23, 24);
        checkDateTime(d9, 2018, 8, 28, 22, 23, 24);

        // Test differences
        Tucuxi::Common::Duration diff = d2 - d1;
        fructose_assert(diff.get<days>().count() == 365);
        fructose_assert(diff.get<months>().count() != 12); // Since a year is 365.2425 days
        fructose_assert(diff.get<years>().count() != 1);   // Since a month is 365.2425/12 
        diff = diff + std::chrono::duration_cast<std::chrono::seconds>(0.25*24h);
        fructose_assert(diff.get<months>().count() == 12);
        fructose_assert(diff.get<years>().count() == 1);

        // Test get<T>()
        fructose_assert(d2.get<days>().count() - d1.get<days>().count() == 365);

        // Test getDate
        Tucuxi::Common::DateTime d20(2017_y / jan / 1, 10h+22min);
        checkDateTime(d20, 2017, 1, 1, 10, 22, 0);
        fructose_assert(d1.getDate() == d20.getDate());

        // Test getTimeOfDay
        checkTimeOfDay(d20.getTimeOfDay(), 10, 22, 0);

        // Test setDate and setTimeOfDay
        Tucuxi::Common::DateTime d21;
        d21.setDate(d1.getDate());
        d21.setTimeOfDay(11h + 22min + 30s);
        checkDateTime(d21, 2017, 1, 1, 11, 22, 30);
    }

    void timeofday(const std::string& _testName)
    {
        std::cout << _testName << std::endl;

        // Test constructors
        Tucuxi::Common::TimeOfDay t1(22h);
        Tucuxi::Common::TimeOfDay t2(22h + 23min);
        Tucuxi::Common::TimeOfDay t3(22h + 23min + 24s);
        Tucuxi::Common::TimeOfDay t4(Tucuxi::Common::Duration(333s));
        checkTimeOfDay(t1, 22, 0, 0);
        checkTimeOfDay(t2, 22, 23, 0);
        checkTimeOfDay(t3, 22, 23, 24);
        checkTimeOfDay(t4, 0, 5, 33);

        time_of_day<std::chrono::seconds> tod = t3.get<std::chrono::seconds>();
        fructose_assert(tod.hours().count() == 22);
        fructose_assert(tod.minutes().count() == 23);
        fructose_assert(tod.seconds().count() == 24);
    }

    void duration(const std::string& _testName)
    {
        std::cout << _testName << std::endl;

        // Test constructors
        Tucuxi::Common::Duration d1;
        checkDuration(d1, 0);
        Tucuxi::Common::Duration d2(2h);
        checkDuration(d2, 2 * 60 * 60);
        Tucuxi::Common::Duration d3(-3h);
        checkDuration(d3, -3 * 60 * 60);
        Tucuxi::Common::Duration d4(437ms);
        checkDuration(d4, 0.437);

        // Test operators
        Tucuxi::Common::Duration d5 = d2 * 2;
        checkDuration(d5, 4 * 60 * 60);
        d5 *= 2;
        checkDuration(d5, 8 * 60 * 60);
        Tucuxi::Common::Duration d6 = d2 / 2;
        checkDuration(d6, 1 * 60 * 60);
        d6 /= 2;
        checkDuration(d6, 0.5 * 60 * 60);
        Tucuxi::Common::Duration d7 = d2 + 20s;
        checkDuration(d7, 2 * 60 * 60 + 20);
        d7 += 20s;
        checkDuration(d7, 2 * 60 * 60 + 40);
        Tucuxi::Common::Duration d8 = d2 - 20s;
        checkDuration(d8, 2 * 60 * 60 - 20);
        d8 -= 20s;
        checkDuration(d8, 2 * 60 * 60 - 40);
        Tucuxi::Common::Duration d9 = d2 % 25min;
        checkDuration(d9, 20 * 60);
        Tucuxi::Common::Duration d10 = d2 % 7;
        checkDuration(d10, 4);

        float n = d2/15min;
        fructose_assert(n == 8);

        fructose_assert(d2 > d3);
        fructose_assert(d2 > d4);
        fructose_assert(d4 > d1);
        fructose_assert(d4 > d3);

        // Test other functions
        d2.clear();
        checkDuration(d2, 0);

        Tucuxi::Common::DateTime now;
        Tucuxi::Common::DateTime later = now + 2h;
        fructose_assert(later - now == 2h);
        fructose_assert(now - later == -2h);

        Tucuxi::Common::TimeOfDay tod1 = now.getTimeOfDay();
        Tucuxi::Common::TimeOfDay tod2 = later.getTimeOfDay();
        fructose_assert(tod2 - tod1 == 2h);
    }

private:
    void checkDateTime(const Tucuxi::Common::DateTime& _date, int _year, int _month, int _day, int _hour, int _minute, int _second)
    {
        fructose_assert(_date.year() == _year);
        fructose_assert(_date.month() == _month);
        fructose_assert(_date.day() == _day);
        fructose_assert(_date.hour() == _hour);
        fructose_assert(_date.minute() == _minute);
        fructose_assert(_date.second() == _second);
    }

    void checkTimeOfDay(const Tucuxi::Common::TimeOfDay& _time, int _hour, int _minute, int _second)
    {
        fructose_assert(_time.hour() == _hour);
        fructose_assert(_time.minute() == _minute);
        fructose_assert(_time.second() == _second);
    }
    void checkDuration(const Tucuxi::Common::Duration& _duration, float _nbSeconds)
    {
        bool isEmpty = _nbSeconds == 0.0;
        bool isNegative = _nbSeconds < 0.0;
        int nSeconds = (int)_nbSeconds;
        fructose_assert(_duration.isEmpty() == isEmpty);
        fructose_assert(_duration.isNegative() == isNegative);
        fructose_assert(_duration.get<std::chrono::seconds>().count() == nSeconds);
        fructose_assert(_duration.get<std::chrono::minutes>().count() == nSeconds/60);
        fructose_assert(_duration.get<std::chrono::hours>().count() == nSeconds/60/60);
        fructose_assert(_duration.get() == _nbSeconds);
    }
};
