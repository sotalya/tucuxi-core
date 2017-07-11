
#include "fructose/fructose.h"

#include "date/date.h"

#include "tucucommon/datetime.h"
#include "tucucommon/timeofday.h"
#include "tucucommon/duration.h"

using namespace std::chrono;

struct TestDateTime : public fructose::test_base<TestDateTime>
{
    std::chrono::duration<float> t(std::chrono::duration<float> d) {
        d = d * 2;
    }

    void basic(const std::string& _testName)
    {
        std::cout << _testName << std::endl;

        Tucuxi::Common::DateTime now;
        Tucuxi::Common::DateTime later = now + 2h;
        fructose_assert(later - now == 2h);

        Tucuxi::Common::Duration diff = later - now;
        fructose_assert(diff == 2h);

        float n = diff / 15min;
        fructose_assert(n == 8);

        Tucuxi::Common::TimeOfDay tod1 = 8h + 0min + 0s;
        Tucuxi::Common::TimeOfDay tod2 = 8h + 42min + 37s;
        Tucuxi::Common::Duration d = tod2 - tod1;
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
