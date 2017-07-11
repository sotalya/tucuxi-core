
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

        Tucuxi::Common::DateTime d1(2017_y / jan / 1);
        Tucuxi::Common::DateTime d2(2018_y / jan / 1);
        Tucuxi::Common::DateTime d3(2017_y / feb / 22, 22h);
        Tucuxi::Common::DateTime d4(2017_y / mar / 22, 23min);
        Tucuxi::Common::DateTime d5(2017_y / mar / 22, 22h + 23min);
        Tucuxi::Common::DateTime d6(2017_y / apr / 22, 24s);
        Tucuxi::Common::DateTime d7(2017_y / jun / 25, 22h + 23min + 24s);

        fructose_assert(d3.hour() == 22);
        fructose_assert(d3.minute() == 0);
        fructose_assert(d3.second() == 0);
        fructose_assert(d4.hour() == 0);
        fructose_assert(d4.minute() == 23);
        fructose_assert(d4.second() == 0);
        fructose_assert(d5.hour() == 22);
        fructose_assert(d5.minute() == 23);
        fructose_assert(d5.second() == 0);
        fructose_assert(d6.hour() == 0);
        fructose_assert(d6.minute() == 0);
        fructose_assert(d6.second() == 24);
        fructose_assert(d7.hour() == 22);
        fructose_assert(d7.minute() == 23);
        fructose_assert(d7.second() == 24);


        Tucuxi::Common::Duration diff = d2 - d1;
        fructose_assert(diff.get<days>().count() == 365);
        
        fructose_assert(diff.get<months>().count() != 12); // Since a year is 365.2425 days
        fructose_assert(diff.get<years>().count() != 1);   // Since a month is 365.2425/12 

        diff = diff + std::chrono::duration_cast<std::chrono::seconds>(0.25*24h);
        fructose_assert(diff.get<months>().count() == 12);
        fructose_assert(diff.get<years>().count() == 1);

        fructose_assert(d2.year() - d1.year() == 1);
        fructose_assert(d2.month() - d1.month() == 0);
    }

    void timeofday(const std::string& _testName)
    {
        std::cout << _testName << std::endl;
    }

    void duration(const std::string& _testName)
    {
        std::cout << _testName << std::endl;

        Tucuxi::Common::DateTime now;
        Tucuxi::Common::DateTime later = now + 2h;
        fructose_assert(later - now == 2h);
        fructose_assert(now - later == -2h);

        Tucuxi::Common::TimeOfDay tod1 = now.getTimeOfDay();
        Tucuxi::Common::TimeOfDay tod2 = later.getTimeOfDay();
        fructose_assert(tod2 - tod1 == 2h);
        
        Tucuxi::Common::Duration diff = later - now;
        fructose_assert(diff.get<std::chrono::seconds>().count() == 2*60*60);
        float n = diff / 15min;
        fructose_assert(n == 8);

        diff += 4min;
        fructose_assert(diff == (2h + 4min));        

        tod1 = Tucuxi::Common::TimeOfDay(8h + 0min + 0s);
        tod2 = Tucuxi::Common::TimeOfDay(8h + 42min + 37s);
        diff = tod2 - tod1;
        fructose_assert(diff.get<std::chrono::seconds>().count() == 42*60+37);
/*
        Tucuxi::Common::Duration a;
        Tucuxi::Common::Duration d(12min);
        d += 12h;
        a = d;
        a = d * 2;
        a = a + d;
        a = d - 2h;

        hours h = duration_cast<hours>(12s);

        date::days d = a.get<date::days>();
        date::years y = a.get<date::years>();

        hours adfa = a.get<hours>();
        float secs = a.get();
*/
    }
};
