/* 
 * Tucuxi - Tucuxi-core library and command line tool. 
 * This code allows to perform prediction of drug concentration in blood 
 * and to propose dosage adaptations.
 * It has been developed by HEIG-VD, in close collaboration with CHUV. 
 * Copyright (C) 2023 HEIG-VD, maintained by Yann Thoma  <yann.thoma@heig-vd.ch>
 * 
 * This program is free software: you can redistribute it and/or modify 
 * it under the terms of the GNU Affero General Public License as 
 * published by the Free Software Foundation, either version 3 of the 
 * License, or any later version. 
 * 
 * This program is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU Affero General Public License for more details. 
 * 
 * You should have received a copy of the GNU Affero General Public License 
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */


#include <gtest/gtest.h>

#include "date/date.h"

#include "tucucommon/datetime.h"
#include "tucucommon/duration.h"
#include "tucucommon/timeofday.h"
#include "tucucommon/utils.h"

using namespace std::chrono_literals;
using namespace date;


static void checkDateTime(
        const Tucuxi::Common::DateTime& _date, int _year, int _month, int _day, int _hour, int _minute, int _second)
{
    EXPECT_EQ(_date.year(), _year);
    EXPECT_EQ(_date.month(), _month);
    EXPECT_EQ(_date.day(), _day);
    EXPECT_EQ(_date.hour(), _hour);
    EXPECT_EQ(_date.minute(), _minute);
    EXPECT_EQ(_date.second(), _second);
}

static void checkTimeOfDay(const Tucuxi::Common::TimeOfDay& _time, int _hour, int _minute, int _second)
{
    EXPECT_EQ(_time.hour(), _hour);
    EXPECT_EQ(_time.minute(), _minute);
    EXPECT_EQ(_time.second(), _second);
}
static void checkDuration(const Tucuxi::Common::Duration& _duration, double _nbSeconds)
{
    bool isEmpty = _nbSeconds == 0.0;
    bool isNegative = _nbSeconds < 0.0;
    double nSeconds = _nbSeconds;
    EXPECT_TRUE(_duration.isEmpty() == isEmpty);
    EXPECT_TRUE(_duration.isNegative() == isNegative);
    EXPECT_DOUBLE_EQ(_duration.toSeconds(), nSeconds);
    EXPECT_DOUBLE_EQ(_duration.toMinutes(), nSeconds / 60);
    EXPECT_DOUBLE_EQ(_duration.toHours(), nSeconds / 60 / 60);
    EXPECT_DOUBLE_EQ(_duration.toDays(), nSeconds / 60 / 60 / 24);
    double d = static_cast<double>(_duration.toMilliseconds()) / 1000.0;
    EXPECT_DOUBLE_EQ(d, _nbSeconds);
}

TEST(Common_TestDateTime, DateTime)
{


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
    Tucuxi::Common::DateTime d10("2017-12-17 17:34:20", "%Y-%m-%d %H:%M:%S");
    Tucuxi::Common::DateTime d11(1951_y / may / 25, 24s);
    Tucuxi::Common::DateTime d12("1951-12-17 17:34:20", "%Y-%m-%d %H:%M:%S");

    checkDateTime(d1, 2017, 1, 1, 0, 0, 0);
    checkDateTime(d2, 2018, 1, 1, 0, 0, 0);
    checkDateTime(d3, 2012, 2, 22, 22, 0, 0);
    checkDateTime(d4, 2013, 3, 23, 0, 23, 0);
    checkDateTime(d5, 2014, 4, 24, 22, 23, 0);
    checkDateTime(d6, 2015, 5, 25, 0, 0, 24);
    checkDateTime(d7, 2016, 6, 26, 0, 23, 24);
    checkDateTime(d8, 2017, 7, 27, 22, 23, 24);
    checkDateTime(d9, 2018, 8, 28, 22, 23, 24);
    checkDateTime(d10, 2017, 12, 17, 17, 34, 20);
    checkDateTime(d11, 1951, 5, 25, 0, 0, 24);
    checkDateTime(d12, 1951, 12, 17, 17, 34, 20);

    // Test thrown exception
    ASSERT_THROW(Tucuxi::Common::DateTime d13("mauvais format", "%Y-%m-%dT%H:%M:%S"), std::runtime_error);
    ASSERT_THROW(Tucuxi::Common::DateTime d14("1911-12-17 s", "%Y-%m-%dT%H:%M:%S"), std::runtime_error);
    ASSERT_THROW(Tucuxi::Common::DateTime d15("", "%Y-%m-%dT%H:%M:%S"), std::runtime_error);
    ASSERT_THROW(Tucuxi::Common::DateTime d16("17:34:20", "%Y-%m-%dT%H:%M:%S"), std::runtime_error);

    // Test differences
    Tucuxi::Common::Duration diff = d2 - d1;
    ASSERT_DOUBLE_EQ(diff.toDays(), 365.0);
    ASSERT_NE(diff.toMonths(), 12.0); // Since a year is 365.2425 days
    ASSERT_NE(diff.toYears(), 1.0);   // Since a month is 365.2425/12
    diff = diff + Tucuxi::Common::Duration(24h) * 0.25;
    ASSERT_EQ(diff.toMonths(), 12);
    ASSERT_EQ(diff.toYears(), 1);

    // Test getDate
    Tucuxi::Common::DateTime d20(2017_y / jan / 1, 10h + 22min);
    checkDateTime(d20, 2017, 1, 1, 10, 22, 0);
    ASSERT_EQ(d1.getDate(), d20.getDate());

    // Test getTimeOfDay
    checkTimeOfDay(d20.getTimeOfDay(), 10, 22, 0);

    // Test setDate and setTimeOfDay
    Tucuxi::Common::DateTime d21;
    d21.setDate(d1.getDate());
    d21.setTimeOfDay(Tucuxi::Common::TimeOfDay(11h + 22min + 30s));
    checkDateTime(d21, 2017, 1, 1, 11, 22, 30);

    // Test reset and isUndefined
    ASSERT_FALSE(d1.isUndefined());
    d1.reset();
    ASSERT_TRUE(d1.isUndefined());
    d1.setDate(d2.getDate());
    ASSERT_FALSE(d1.isUndefined());

    // Test to and from seconds
    double nSeconds = d10.toSeconds();
    Tucuxi::Common::DateTime d30 = Tucuxi::Common::DateTime::fromDurationSinceEpoch(
            Tucuxi::Common::Duration(std::chrono::seconds(static_cast<int64>(nSeconds))));
    checkDateTime(d30, 2017, 12, 17, 17, 34, 20);

    // Test addDays, addMonth and addYears
    Tucuxi::Common::DateTime d31 = d10;
    d31.addDays(3);
    checkDateTime(d31, 2017, 12, 20, 17, 34, 20);
    d31.addDays(31);
    checkDateTime(d31, 2018, 1, 20, 17, 34, 20);
    d31.addDays(365);
    checkDateTime(d31, 2019, 1, 20, 17, 34, 20);
    d31.addDays(-399);
    checkDateTime(d31, 2017, 12, 17, 17, 34, 20);

    Tucuxi::Common::DateTime d32 = d10;
    d32.addMonths(3);
    checkDateTime(d32, 2018, 3, 17, 17, 34, 20);
    d32.addMonths(25);
    checkDateTime(d32, 2020, 4, 17, 17, 34, 20);
    d32.addMonths(-12);
    checkDateTime(d32, 2019, 4, 17, 17, 34, 20);

    Tucuxi::Common::DateTime d33 = d10;
    d33.addYears(3);
    checkDateTime(d33, 2020, 12, 17, 17, 34, 20);
    d33.addYears(-5);
    checkDateTime(d33, 2015, 12, 17, 17, 34, 20);

    {

        // dateDiffInDays and related functions are in utils.cpp, but it seems
        // reasonable to test them here
        Tucuxi::Common::DateTime d100(2016_y / jun / 26, 23min + 24s);
        Tucuxi::Common::DateTime d101(2017_y / jun / 26, 23min + 24s);

        ASSERT_EQ(Tucuxi::Common::Utils::dateDiffInYears(d100, d101), 1);
        ASSERT_EQ(Tucuxi::Common::Utils::dateDiffInYears(d101, d100), 1);
        ASSERT_EQ(Tucuxi::Common::Utils::dateDiffInDays(d100, d101), 365);
        ASSERT_EQ(Tucuxi::Common::Utils::dateDiffInDays(d101, d100), 365);
        ASSERT_EQ(Tucuxi::Common::Utils::dateDiffInWeeks(d100, d101), 52);
        ASSERT_EQ(Tucuxi::Common::Utils::dateDiffInWeeks(d101, d100), 52);
    }
}

TEST(Common_TestDateTime, TimeOfDay)
{


    // Test constructors
    Tucuxi::Common::TimeOfDay t1(22h);
    Tucuxi::Common::TimeOfDay t2(22h + 23min);
    Tucuxi::Common::TimeOfDay t3(22h + 23min + 24s);
    Tucuxi::Common::TimeOfDay t4(333s);
    Tucuxi::Common::TimeOfDay t5(48h + 10min + 3s);
    Tucuxi::Common::TimeOfDay t6(26h);
    checkTimeOfDay(t1, 22, 0, 0);
    checkTimeOfDay(t2, 22, 23, 0);
    checkTimeOfDay(t3, 22, 23, 24);
    checkTimeOfDay(t4, 0, 5, 33);
    checkTimeOfDay(t5, 0, 10, 3);
    checkTimeOfDay(t6, 2, 0, 0);
}

TEST(Common_TestDateTime, Duration)
{


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

    double n = d2 / 15min;
    ASSERT_EQ(n, 8);

    ASSERT_GT(d2, d3);
    ASSERT_GT(d2, d4);
    ASSERT_GT(d4, d1);
    ASSERT_GT(d4, d3);

    // Test other functions
    d2.clear();
    checkDuration(d2, 0);

    Tucuxi::Common::DateTime now = Tucuxi::Common::DateTime::now();
    Tucuxi::Common::DateTime later = now + 2h;
    ASSERT_EQ(later - now, 2h);
    ASSERT_EQ(now - later, -2h);

    // TODO : Rethink this test, as if done before midnight it fails
    Tucuxi::Common::TimeOfDay tod1 = now.getTimeOfDay().getRealDuration();
    Tucuxi::Common::TimeOfDay tod2 = later.getTimeOfDay().getRealDuration();
    ASSERT_EQ(tod2 - tod1, 2h);
}
