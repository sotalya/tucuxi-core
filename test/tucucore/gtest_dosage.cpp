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


#include <memory>

#include <gtest/gtest.h>

#include "tucucore/dosage.h"

using namespace Tucuxi::Core;

/// \brief Test the instantiation of a SingleDose-derived class, focusing on Single-Dose peculiarities.
/// The SingleDose class cannot be instantiated since it is abstract -- we will rely on the test below for a
/// LastingDose and focus on the capabilities of a pure SingleDose (i.e., the Dose, the Route, ...)
TEST(Core_TestDosage, SingleDose)
{
    const DoseValue validDose = 100.0;
    const DoseValue invalidDose = -100.0;
    const FormulationAndRoute routePerfusion(
            Formulation::Test, AdministrationRoute::IntravenousDrip, AbsorptionModel::Infusion);
    const Duration emptyInfusionTime;
    const Duration invalidInfusionTime(-std::chrono::minutes(20));
    const Duration validInfusionTime(std::chrono::minutes(20));
    const Duration validInterval(std::chrono::hours(10));

    ASSERT_THROW(
            Tucuxi::Core::LastingDose(invalidDose, TucuUnit("mg"), routePerfusion, validInfusionTime, validInterval),
            std::invalid_argument);

    // An infusion absorption now supports 0 as infusion time
    /*
        ASSERT_THROW(Tucuxi::Core::LastingDose(validDose,
                                                            routePerfusion,
                                                            emptyInfusionTime,
                                                            validInterval),
                                  std::invalid_argument);
        */
    ASSERT_THROW(
            Tucuxi::Core::LastingDose(validDose, TucuUnit("mg"), routePerfusion, invalidInfusionTime, validInterval),
            std::invalid_argument);
}

/// \brief Test the instantiation of a LastingDose.
TEST(Core_TestDosage, LastingDose)
{
    const DoseValue validDose = 100.0;
    const FormulationAndRoute routePerfusion(
            Formulation::Test, AdministrationRoute::IntravenousDrip, AbsorptionModel::Infusion);
    const Duration validInfusionTime(std::chrono::minutes(20));
    const Duration emptyInterval;
    const Duration invalidInterval(-std::chrono::hours(10));
    const Duration validInterval(std::chrono::hours(10));

    ASSERT_THROW(
            Tucuxi::Core::LastingDose(validDose, TucuUnit("mg"), routePerfusion, validInfusionTime, emptyInterval),
            std::invalid_argument);
    ASSERT_THROW(
            Tucuxi::Core::LastingDose(validDose, TucuUnit("mg"), routePerfusion, validInfusionTime, invalidInterval),
            std::invalid_argument);
    ASSERT_NO_THROW(
            Tucuxi::Core::LastingDose(validDose, TucuUnit("mg"), routePerfusion, validInfusionTime, validInterval));

    auto ptr = std::make_unique<Tucuxi::Core::LastingDose>(
            validDose, TucuUnit("mg"), routePerfusion, validInfusionTime, validInterval);

    ASSERT_TRUE(ptr->getTimeStep() == validInterval);
    DateTime dt(date::year_month_day(date::year(2017), date::month(7), date::day(20)), std::chrono::seconds(12345));
    ASSERT_TRUE(ptr->getFirstIntakeInterval(dt) == dt);
}

/// \brief Test the instantiation of a DailyDose.
TEST(Core_TestDosage, DailyDose)
{
    const DoseValue validDose = 100.0;
    const FormulationAndRoute routePerfusion(
            Formulation::Test, AdministrationRoute::IntravenousDrip, AbsorptionModel::Infusion);
    const Duration validInfusionTime(std::chrono::minutes(20));
    // Cannot have an invalid or invalid time of day (at worst, it simply takes the current time)
    const TimeOfDay validTimeOfDay(Duration(std::chrono::seconds(12345)));

    ASSERT_NO_THROW(
            Tucuxi::Core::DailyDose(validDose, TucuUnit("mg"), routePerfusion, validInfusionTime, validTimeOfDay));

    auto ptr = std::make_unique<Tucuxi::Core::DailyDose>(
            validDose, TucuUnit("mg"), routePerfusion, validInfusionTime, validTimeOfDay);

    // Check that the step is exactly 24 hours
    ASSERT_DOUBLE_EQ(ptr->getTimeStep().toHours(), 24.0);

    // Check that the first intake is correcly set according to the interval's starting date
    // (12345 seconds = 03:25:45)

    // Specified time of day falls before the intake time -> expect to get the intake time
    DateTime dtBefore(
            date::year_month_day(date::year(2017), date::month(7), date::day(20)), std::chrono::seconds(12340));
    DateTime dtBefore_Correct(
            date::year_month_day(date::year(2017), date::month(7), date::day(20)), std::chrono::seconds(12345));
    ASSERT_EQ(ptr->getFirstIntakeInterval(dtBefore), dtBefore_Correct);

    // Specified time of day falls exactly on the intake time -> expect to get the intake time
    DateTime dtEqual(
            date::year_month_day(date::year(2017), date::month(7), date::day(20)), std::chrono::seconds(12345));
    DateTime dtEqual_Correct(
            date::year_month_day(date::year(2017), date::month(7), date::day(20)), std::chrono::seconds(12345));
    ASSERT_EQ(ptr->getFirstIntakeInterval(dtEqual), dtEqual_Correct);

    // Specified time of day falls past the intake time -> expect to get the intake time, but the day after
    DateTime dtAfter(
            date::year_month_day(date::year(2017), date::month(7), date::day(20)), std::chrono::seconds(12350));
    DateTime dtAfter_Correct(
            date::year_month_day(date::year(2017), date::month(7), date::day(21)), std::chrono::seconds(12345));
    ASSERT_EQ(ptr->getFirstIntakeInterval(dtAfter), dtAfter_Correct);
}

/// \brief Test the instantiation of a WeeklyDose.
TEST(Core_TestDosage, WeeklyDose)
{
    const DoseValue validDose = 100.0;
    const FormulationAndRoute routePerfusion(
            Formulation::Test, AdministrationRoute::IntravenousDrip, AbsorptionModel::Infusion);
    const Duration validInfusionTime(std::chrono::minutes(20));
    // Cannot have an invalid or invalid time of day (at worst, it simply takes the current time)
    const TimeOfDay validTimeOfDay(Duration(std::chrono::seconds(12345)));
    const DayOfWeek invalidDayOfWeek(unsigned{11});
    // Days of week range from 0 to 6 (Sunday to Saturday)
    const DayOfWeek validDayOfWeek1(unsigned{SUNDAY});
    const DayOfWeek validDayOfWeek2(unsigned{MONDAY});

    {
        const DayOfWeek validDayOfWeek0(unsigned{MONDAY});
        const DayOfWeek validDayOfWeek1(unsigned{TUESDAY});
        const DayOfWeek validDayOfWeek2(unsigned{WEDNESDAY});
        const DayOfWeek validDayOfWeek3(unsigned{THURSDAY});
        const DayOfWeek validDayOfWeek4(unsigned{FRIDAY});
        const DayOfWeek validDayOfWeek5(unsigned{SATURDAY});
        const DayOfWeek validDayOfWeek6(unsigned{SUNDAY});

        std::vector<unsigned> c_encoding = {
                validDayOfWeek0.c_encoding(),
                validDayOfWeek1.c_encoding(),
                validDayOfWeek2.c_encoding(),
                validDayOfWeek3.c_encoding(),
                validDayOfWeek4.c_encoding(),
                validDayOfWeek5.c_encoding(),
                validDayOfWeek6.c_encoding()};

        std::vector<unsigned> iso_encoding = {
                validDayOfWeek0.iso_encoding(),
                validDayOfWeek1.iso_encoding(),
                validDayOfWeek2.iso_encoding(),
                validDayOfWeek3.iso_encoding(),
                validDayOfWeek4.iso_encoding(),
                validDayOfWeek5.iso_encoding(),
                validDayOfWeek6.iso_encoding()};

        ASSERT_EQ(validDayOfWeek0.iso_encoding(), 1);
        ASSERT_EQ(validDayOfWeek1.iso_encoding(), 2);
        ASSERT_EQ(validDayOfWeek2.iso_encoding(), 3);
        ASSERT_EQ(validDayOfWeek3.iso_encoding(), 4);
        ASSERT_EQ(validDayOfWeek4.iso_encoding(), 5);
        ASSERT_EQ(validDayOfWeek5.iso_encoding(), 6);
        ASSERT_EQ(validDayOfWeek6.iso_encoding(), 7);


        ASSERT_EQ(validDayOfWeek0.c_encoding(), 1);
        ASSERT_EQ(validDayOfWeek1.c_encoding(), 2);
        ASSERT_EQ(validDayOfWeek2.c_encoding(), 3);
        ASSERT_EQ(validDayOfWeek3.c_encoding(), 4);
        ASSERT_EQ(validDayOfWeek4.c_encoding(), 5);
        ASSERT_EQ(validDayOfWeek5.c_encoding(), 6);
        ASSERT_EQ(validDayOfWeek6.c_encoding(), 0);
    }

    ASSERT_THROW(
            Tucuxi::Core::WeeklyDose(
                    validDose, TucuUnit("mg"), routePerfusion, validInfusionTime, validTimeOfDay, invalidDayOfWeek),
            std::invalid_argument);

    ASSERT_NO_THROW(Tucuxi::Core::WeeklyDose(
            validDose, TucuUnit("mg"), routePerfusion, validInfusionTime, validTimeOfDay, validDayOfWeek1));

    ASSERT_NO_THROW(Tucuxi::Core::WeeklyDose(
            validDose, TucuUnit("mg"), routePerfusion, validInfusionTime, validTimeOfDay, validDayOfWeek2));

    auto ptr = std::make_unique<Tucuxi::Core::WeeklyDose>(
            validDose, TucuUnit("mg"), routePerfusion, validInfusionTime, validTimeOfDay, validDayOfWeek1);

    // Check that the step is exactly 7 * 24 hours
    ASSERT_DOUBLE_EQ(ptr->getTimeStep().toHours(), 7.0 * 24.0);

    // Check that the first intake is correcly set according to the interval's starting date
    // (12345 seconds = 03:25:45)
    // 16.07.2017 = Sunday

    // Good day of the week, before the time of day
    DateTime goodDay_beforeTOD(
            date::year_month_day(date::year(2017), date::month(7), date::day(16)), std::chrono::seconds(1234));
    DateTime goodDay_beforeTOD_correct(
            date::year_month_day(date::year(2017), date::month(7), date::day(16)), std::chrono::seconds(12345));
    ASSERT_EQ(ptr->getFirstIntakeInterval(goodDay_beforeTOD), goodDay_beforeTOD_correct);

    // Good day of the week, good time of day
    DateTime goodDay_goodTOD(
            date::year_month_day(date::year(2017), date::month(7), date::day(16)), std::chrono::seconds(12345));
    DateTime goodDay_goodTOD_correct(
            date::year_month_day(date::year(2017), date::month(7), date::day(16)), std::chrono::seconds(12345));
    ASSERT_EQ(ptr->getFirstIntakeInterval(goodDay_goodTOD), goodDay_goodTOD_correct);

    // Good day of the week, after the time of day
    DateTime goodDay_afterTOD(
            date::year_month_day(date::year(2017), date::month(7), date::day(16)), std::chrono::seconds(54321));
    DateTime goodDay_afterTOD_correct(
            date::year_month_day(date::year(2017), date::month(7), date::day(23)), std::chrono::seconds(12345));
    ASSERT_EQ(ptr->getFirstIntakeInterval(goodDay_afterTOD), goodDay_afterTOD_correct);

    // Day before the good day of the week, before the time of day
    DateTime beforeDay_beforeTOD(
            date::year_month_day(date::year(2017), date::month(7), date::day(15)), std::chrono::seconds(1234));
    DateTime beforeDay_beforeTOD_correct(
            date::year_month_day(date::year(2017), date::month(7), date::day(16)), std::chrono::seconds(12345));
    ASSERT_EQ(ptr->getFirstIntakeInterval(beforeDay_beforeTOD), beforeDay_beforeTOD_correct);

    // Day before the good day of the week, good time of day
    DateTime beforeDay_goodTOD(
            date::year_month_day(date::year(2017), date::month(7), date::day(15)), std::chrono::seconds(12345));
    DateTime beforeDay_goodTOD_correct(
            date::year_month_day(date::year(2017), date::month(7), date::day(16)), std::chrono::seconds(12345));
    ASSERT_EQ(ptr->getFirstIntakeInterval(beforeDay_goodTOD), beforeDay_goodTOD_correct);

    // Day before the good day of the week, after the time of day
    DateTime beforeDay_afterTOD(
            date::year_month_day(date::year(2017), date::month(7), date::day(15)), std::chrono::seconds(54321));
    DateTime beforeDay_afterTOD_correct(
            date::year_month_day(date::year(2017), date::month(7), date::day(16)), std::chrono::seconds(12345));
    ASSERT_EQ(ptr->getFirstIntakeInterval(beforeDay_afterTOD), beforeDay_afterTOD_correct);

    // Day after the good day of the week, before the time of day
    DateTime afterDay_beforeTOD(
            date::year_month_day(date::year(2017), date::month(7), date::day(17)), std::chrono::seconds(1234));
    DateTime afterDay_beforeTOD_correct(
            date::year_month_day(date::year(2017), date::month(7), date::day(23)), std::chrono::seconds(12345));
    ASSERT_EQ(ptr->getFirstIntakeInterval(afterDay_beforeTOD), afterDay_beforeTOD_correct);

    // Day after the good day of the week, good time of day
    DateTime afterDay_goodTOD(
            date::year_month_day(date::year(2017), date::month(7), date::day(17)), std::chrono::seconds(12345));
    DateTime afterDay_goodTOD_correct(
            date::year_month_day(date::year(2017), date::month(7), date::day(23)), std::chrono::seconds(12345));
    ASSERT_EQ(ptr->getFirstIntakeInterval(afterDay_goodTOD), afterDay_goodTOD_correct);

    // Day after the good day of the week, after the time of day
    DateTime afterDay_afterTOD(
            date::year_month_day(date::year(2017), date::month(7), date::day(17)), std::chrono::seconds(54321));
    DateTime afterDay_afterTOD_correct(
            date::year_month_day(date::year(2017), date::month(7), date::day(23)), std::chrono::seconds(12345));
    ASSERT_EQ(ptr->getFirstIntakeInterval(afterDay_afterTOD), afterDay_afterTOD_correct);
}

/// \brief Test a time range, checking that overlaps are properly detected.
TEST(Core_TestDosage, DosageTimeRange)
{
    // Give an undefined start date, expect an exception
    DateTime emptyDate;
    emptyDate.reset();
    const FormulationAndRoute routePerfusion(
            Formulation::Test, AdministrationRoute::IntravenousDrip, AbsorptionModel::Infusion);
    LastingDose fakeDose(
            DoseValue(200.0),
            TucuUnit("mg"),
            routePerfusion,
            Duration(std::chrono::minutes(20)),
            Duration(std::chrono::hours(240)));
    ASSERT_THROW(new Tucuxi::Core::DosageTimeRange(emptyDate, fakeDose), std::invalid_argument);

    DateTime startDate(
            date::year_month_day(date::year(2017), date::month(7), date::day(17)), std::chrono::seconds(12345));
    DateTime endDate(
            date::year_month_day(date::year(2017), date::month(7), date::day(23)), std::chrono::seconds(12345));

    // Give a valid start date but no end date, expect no exception but the end date must be undefined
    ASSERT_NO_THROW(Tucuxi::Core::DosageTimeRange* d = new Tucuxi::Core::DosageTimeRange(startDate, fakeDose);
                    delete d;);
    auto ptr1 = std::make_unique<Tucuxi::Core::DosageTimeRange>(startDate, fakeDose);
    ASSERT_EQ(ptr1->getStartDate(), startDate);
    ASSERT_TRUE(ptr1->getEndDate().isUndefined());

    // Same as above, but with start and end dates. Check also that the two dates are meaningful (start <= end).
    ASSERT_THROW(
            {
                Tucuxi::Core::DosageTimeRange* d = new Tucuxi::Core::DosageTimeRange(emptyDate, emptyDate, fakeDose);
                delete d;
            },
            std::invalid_argument);
    ASSERT_THROW(
            {
                Tucuxi::Core::DosageTimeRange* d = new Tucuxi::Core::DosageTimeRange(emptyDate, endDate, fakeDose);
                delete d;
            },
            std::invalid_argument);
    ASSERT_THROW(
            {
                Tucuxi::Core::DosageTimeRange* d = new Tucuxi::Core::DosageTimeRange(endDate, startDate, fakeDose);
                delete d;
            },
            std::invalid_argument);
    ASSERT_NO_THROW({
        Tucuxi::Core::DosageTimeRange* d = new Tucuxi::Core::DosageTimeRange(startDate, emptyDate, fakeDose);
        delete d;
    });
    ASSERT_NO_THROW({
        Tucuxi::Core::DosageTimeRange* d = new Tucuxi::Core::DosageTimeRange(startDate, endDate, fakeDose);
        delete d;
    });
    auto ptr2 = std::make_unique<Tucuxi::Core::DosageTimeRange>(startDate, endDate, fakeDose);
    ASSERT_EQ(ptr2->getStartDate(), startDate);
    ASSERT_EQ(ptr2->getEndDate(), endDate);

    // Check that overlaps are properly detected
    DateTime beforeStartDate(
            date::year_month_day(date::year(2017), date::month(6), date::day(17)), std::chrono::seconds(12345));
    DateTime afterStartBeforeEndDate(
            date::year_month_day(date::year(2017), date::month(7), date::day(19)), std::chrono::seconds(12345));
    // Just after the previous one, but still before the end
    DateTime afterMiddleBeforeEndDate(
            date::year_month_day(date::year(2017), date::month(7), date::day(20)), std::chrono::seconds(12345));
    DateTime afterEndDate(
            date::year_month_day(date::year(2017), date::month(8), date::day(19)), std::chrono::seconds(12345));

    auto beforeToMiddle =
            std::make_unique<Tucuxi::Core::DosageTimeRange>(beforeStartDate, afterStartBeforeEndDate, fakeDose);
    ASSERT_TRUE(timeRangesOverlap(*ptr2, *beforeToMiddle));

    auto middleToAfter =
            std::make_unique<Tucuxi::Core::DosageTimeRange>(afterStartBeforeEndDate, afterEndDate, fakeDose);
    ASSERT_TRUE(timeRangesOverlap(*ptr2, *middleToAfter));

    auto beforeToAfter = std::make_unique<Tucuxi::Core::DosageTimeRange>(beforeStartDate, afterEndDate, fakeDose);
    ASSERT_TRUE(timeRangesOverlap(*ptr2, *beforeToAfter));

    auto insideInterval = std::make_unique<Tucuxi::Core::DosageTimeRange>(
            afterStartBeforeEndDate, afterMiddleBeforeEndDate, fakeDose);
    ASSERT_TRUE(timeRangesOverlap(*ptr2, *insideInterval));

    auto beforeToOpenEnd = std::make_unique<Tucuxi::Core::DosageTimeRange>(beforeStartDate, fakeDose);
    ASSERT_TRUE(timeRangesOverlap(*ptr2, *beforeToOpenEnd));

    auto middleToOpenEnd = std::make_unique<Tucuxi::Core::DosageTimeRange>(afterStartBeforeEndDate, fakeDose);
    ASSERT_TRUE(timeRangesOverlap(*ptr2, *middleToOpenEnd));

    auto afterToOpenEnd = std::make_unique<Tucuxi::Core::DosageTimeRange>(afterEndDate, fakeDose);
    ASSERT_TRUE(!timeRangesOverlap(*ptr2, *afterToOpenEnd));

    // Check that two neighboring time ranges are not considered as overlapping
    DateTime neigh1Start(
            date::year_month_day(date::year(2017), date::month(6), date::day(17)), std::chrono::seconds(12345));
    DateTime neigh1End(
            date::year_month_day(date::year(2017), date::month(7), date::day(19)), std::chrono::seconds(12345));
    DateTime neigh2Start(
            date::year_month_day(date::year(2017), date::month(7), date::day(19)), std::chrono::seconds(12345));
    auto neigh1 = std::make_unique<Tucuxi::Core::DosageTimeRange>(neigh1Start, neigh1End, fakeDose);
    auto neigh2 = std::make_unique<Tucuxi::Core::DosageTimeRange>(neigh2Start, fakeDose);
    ASSERT_TRUE(!timeRangesOverlap(*neigh1, *neigh2));

    // Check the overlap detection for a time range against a list of time ranges
    DateTime before1(
            date::year_month_day(date::year(2017), date::month(1), date::day(17)), std::chrono::seconds(12345));
    DateTime before2(
            date::year_month_day(date::year(2017), date::month(2), date::day(17)), std::chrono::seconds(12345));
    DateTime before3(
            date::year_month_day(date::year(2017), date::month(3), date::day(17)), std::chrono::seconds(12345));
    DateTime before4(
            date::year_month_day(date::year(2017), date::month(4), date::day(17)), std::chrono::seconds(12345));
    DateTime before5(
            date::year_month_day(date::year(2017), date::month(5), date::day(17)), std::chrono::seconds(12345));
    DateTime before6(date::year_month_day(date::year(2017), date::month(6), date::day(1)), std::chrono::seconds(12345));

    DosageTimeRangeList trList;
    trList.emplace_back(new Tucuxi::Core::DosageTimeRange(before1, before2, fakeDose));
    trList.emplace_back(new Tucuxi::Core::DosageTimeRange(before2, before3, fakeDose));
    trList.emplace_back(new Tucuxi::Core::DosageTimeRange(before3, before4, fakeDose));
    trList.emplace_back(new Tucuxi::Core::DosageTimeRange(before4, before5, fakeDose));
    trList.emplace_back(new Tucuxi::Core::DosageTimeRange(before5, before6, fakeDose));

    ASSERT_TRUE(!timeRangesOverlap(*ptr2, trList));

    trList.emplace_back(new Tucuxi::Core::DosageTimeRange(before5, fakeDose));
    ASSERT_TRUE(timeRangesOverlap(*ptr2, trList));
}
