/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TEST_DOSAGE_H
#define TEST_DOSAGE_H

#include <iostream>
#include <memory>

#include "fructose/fructose.h"

#include "tucucore/definitions.h"
#include "tucucore/dosage.h"

using namespace Tucuxi::Core;

struct TestDosage : public fructose::test_base<TestDosage>
{

    TestDosage()
    {
    }

    void testSingleDose(const std::string& _testName)
    {
        std::cout << _testName << std::endl;

        // The SingleDose class cannot be instantiated since it is abstract -- we will rely on the test below for a
        // LastingDose and focus on the capabilities of a pure SingleDose (i.e., the Dose, the Route, ...)

        const Dose validDose = 100.0;
        const Dose invalidDose = -100.0;
        const RouteOfAdministration routePerfusion = RouteOfAdministration::PERFUSION;
        const Duration emptyInfusionTime;
        const Duration invalidInfusionTime(-std::chrono::minutes(20));
        const Duration validInfusionTime(std::chrono::minutes(20));
        const Duration validInterval(std::chrono::hours(10));

        fructose_assert_exception(Tucuxi::Core::LastingDose(invalidDose,
                                                            routePerfusion,
                                                            validInfusionTime,
                                                            validInterval),
                                  std::invalid_argument);

        fructose_assert_exception(Tucuxi::Core::LastingDose(validDose,
                                                            routePerfusion,
                                                            emptyInfusionTime,
                                                            validInterval),
                                  std::invalid_argument);

        fructose_assert_exception(Tucuxi::Core::LastingDose(validDose,
                                                            routePerfusion,
                                                            invalidInfusionTime,
                                                            validInterval),
                                  std::invalid_argument);

        std::cout << "=> " << _testName << " succeeds!" << std::endl;
    }

    void testLastingDose(const std::string& _testName)
    {
        std::cout << _testName << std::endl;

        const Dose validDose = 100.0;
        const RouteOfAdministration routePerfusion = RouteOfAdministration::PERFUSION;
        const Duration validInfusionTime(std::chrono::minutes(20));
        const Duration emptyInterval;
        const Duration invalidInterval(-std::chrono::hours(10));
        const Duration validInterval(std::chrono::hours(10));

        fructose_assert_exception(Tucuxi::Core::LastingDose(validDose,
                                                            routePerfusion,
                                                            validInfusionTime,
                                                            emptyInterval),
                                  std::invalid_argument);
        fructose_assert_exception(Tucuxi::Core::LastingDose(validDose,
                                                            routePerfusion,
                                                            validInfusionTime,
                                                            invalidInterval),
                                  std::invalid_argument);
        fructose_assert_no_exception(Tucuxi::Core::LastingDose(validDose,
                                                               routePerfusion,
                                                               validInfusionTime,
                                                               validInterval));

        std::unique_ptr<Tucuxi::Core::LastingDose> ptr(new Tucuxi::Core::LastingDose(validDose,
                                                                                     routePerfusion,
                                                                                     validInfusionTime,
                                                                                     validInterval));

        fructose_assert(ptr->getTimeStep() == validInterval);
        DateTime dt(date::year_month_day(date::year(2017), date::month(7), date::day(20)),
                    std::chrono::seconds(12345));
        fructose_assert(ptr->getFirstIntakeInterval(dt) == dt)

        std::cout << "=> " << _testName << " succeeds!" << std::endl;
    }

    void testDailyDose(const std::string& _testName)
    {
        std::cout << _testName << std::endl;

        const Dose validDose = 100.0;
        const RouteOfAdministration routePerfusion = RouteOfAdministration::PERFUSION;
        const Duration validInfusionTime(std::chrono::minutes(20));
        // Cannot have an invalid or invalid time of day (at worst, it simply takes the current time)
        const TimeOfDay validTimeOfDay(Duration(std::chrono::seconds(12345)));

        fructose_assert_no_exception(Tucuxi::Core::DailyDose(validDose,
                                                             routePerfusion,
                                                             validInfusionTime,
                                                             validTimeOfDay));

        std::unique_ptr<Tucuxi::Core::DailyDose> ptr(new Tucuxi::Core::DailyDose(validDose,
                                                                                 routePerfusion,
                                                                                 validInfusionTime,
                                                                                 validTimeOfDay));

        // Check that the step is exactly 24 hours
        fructose_assert(ptr->getTimeStep().get<std::chrono::hours>() == std::chrono::hours(24));

        // Check that the first intake is correcly set according to the interval's starting date
        // (12345 seconds = 03:25:45)

        // Specified time of day falls before the intake time -> expect to get the intake time
        DateTime dtBefore(date::year_month_day(date::year(2017), date::month(7), date::day(20)),
                          std::chrono::seconds(12340));
        DateTime dtBefore_Correct(date::year_month_day(date::year(2017), date::month(7), date::day(20)),
                                 std::chrono::seconds(12345));
        fructose_assert(ptr->getFirstIntakeInterval(dtBefore) == dtBefore_Correct);

        // Specified time of day falls exactly on the intake time -> expect to get the intake time
        DateTime dtEqual(date::year_month_day(date::year(2017), date::month(7), date::day(20)),
                         std::chrono::seconds(12345));
        DateTime dtEqual_Correct(date::year_month_day(date::year(2017), date::month(7), date::day(20)),
                                 std::chrono::seconds(12345));
        fructose_assert(ptr->getFirstIntakeInterval(dtEqual) == dtEqual_Correct);

        // Specified time of day falls past the intake time -> expect to get the intake time, but the day after
        DateTime dtAfter(date::year_month_day(date::year(2017), date::month(7), date::day(20)),
                         std::chrono::seconds(12350));
        DateTime dtAfter_Correct(date::year_month_day(date::year(2017), date::month(7), date::day(21)),
                                 std::chrono::seconds(12345));
        fructose_assert(ptr->getFirstIntakeInterval(dtAfter) == dtAfter_Correct);

        std::cout << "=> " << _testName << " succeeds!" << std::endl;
    }

    void testWeeklyDose(const std::string& _testName)
    {
        std::cout << _testName << std::endl;

        const Dose validDose = 100.0;
        const RouteOfAdministration routePerfusion = RouteOfAdministration::PERFUSION;
        const Duration validInfusionTime(std::chrono::minutes(20));
        // Cannot have an invalid or invalid time of day (at worst, it simply takes the current time)
        const TimeOfDay validTimeOfDay(Duration(std::chrono::seconds(12345)));
        const DayOfWeek invalidDayOfWeek((unsigned)11);
        // Days of week range from 0 to 6 (Sunday to Saturday)
        const DayOfWeek validDayOfWeek1((unsigned)0);
        const DayOfWeek validDayOfWeek2((unsigned)1);

        fructose_assert_exception(Tucuxi::Core::WeeklyDose(validDose,
                                                           routePerfusion,
                                                           validInfusionTime,
                                                           validTimeOfDay,
                                                           invalidDayOfWeek),
                                  std::invalid_argument);

        fructose_assert_no_exception(Tucuxi::Core::WeeklyDose(validDose,
                                                              routePerfusion,
                                                              validInfusionTime,
                                                              validTimeOfDay,
                                                              validDayOfWeek1));

        fructose_assert_no_exception(Tucuxi::Core::WeeklyDose(validDose,
                                                              routePerfusion,
                                                              validInfusionTime,
                                                              validTimeOfDay,
                                                              validDayOfWeek2));

        std::unique_ptr<Tucuxi::Core::WeeklyDose> ptr(new Tucuxi::Core::WeeklyDose(validDose,
                                                                                   routePerfusion,
                                                                                   validInfusionTime,
                                                                                   validTimeOfDay,
                                                                                   validDayOfWeek1));

        // Check that the step is exactly 7 * 24 hours
        fructose_assert(ptr->getTimeStep().get<std::chrono::hours>() == std::chrono::hours(7 * 24));

        // Check that the first intake is correcly set according to the interval's starting date
        // (12345 seconds = 03:25:45)
        // 16.07.2017 = Sunday

        // Good day of the week, before the time of day
        DateTime goodDay_beforeTOD(date::year_month_day(date::year(2017), date::month(7), date::day(16)),
                                   std::chrono::seconds(1234));
        DateTime goodDay_beforeTOD_correct(date::year_month_day(date::year(2017), date::month(7), date::day(16)),
                                           std::chrono::seconds(12345));
        fructose_assert(ptr->getFirstIntakeInterval(goodDay_beforeTOD) == goodDay_beforeTOD_correct);

        // Good day of the week, good time of day
        DateTime goodDay_goodTOD(date::year_month_day(date::year(2017), date::month(7), date::day(16)),
                                 std::chrono::seconds(12345));
        DateTime goodDay_goodTOD_correct(date::year_month_day(date::year(2017), date::month(7), date::day(16)),
                                         std::chrono::seconds(12345));
        fructose_assert(ptr->getFirstIntakeInterval(goodDay_goodTOD) == goodDay_goodTOD_correct);

        // Good day of the week, after the time of day
        DateTime goodDay_afterTOD(date::year_month_day(date::year(2017), date::month(7), date::day(16)),
                                   std::chrono::seconds(54321));
        DateTime goodDay_afterTOD_correct(date::year_month_day(date::year(2017), date::month(7), date::day(23)),
                                           std::chrono::seconds(12345));
        fructose_assert(ptr->getFirstIntakeInterval(goodDay_afterTOD) == goodDay_afterTOD_correct);

        // Day before the good day of the week, before the time of day
        DateTime beforeDay_beforeTOD(date::year_month_day(date::year(2017), date::month(7), date::day(15)),
                                     std::chrono::seconds(1234));
        DateTime beforeDay_beforeTOD_correct(date::year_month_day(date::year(2017), date::month(7), date::day(16)),
                                             std::chrono::seconds(12345));
        fructose_assert(ptr->getFirstIntakeInterval(beforeDay_beforeTOD) == beforeDay_beforeTOD_correct);

        // Day before the good day of the week, good time of day
        DateTime beforeDay_goodTOD(date::year_month_day(date::year(2017), date::month(7), date::day(15)),
                                   std::chrono::seconds(12345));
        DateTime beforeDay_goodTOD_correct(date::year_month_day(date::year(2017), date::month(7), date::day(16)),
                                           std::chrono::seconds(12345));
        fructose_assert(ptr->getFirstIntakeInterval(beforeDay_goodTOD) == beforeDay_goodTOD_correct);

        // Day before the good day of the week, after the time of day
        DateTime beforeDay_afterTOD(date::year_month_day(date::year(2017), date::month(7), date::day(15)),
                                    std::chrono::seconds(54321));
        DateTime beforeDay_afterTOD_correct(date::year_month_day(date::year(2017), date::month(7), date::day(16)),
                                            std::chrono::seconds(12345));
        fructose_assert(ptr->getFirstIntakeInterval(beforeDay_afterTOD) == beforeDay_afterTOD_correct);

        // Day after the good day of the week, before the time of day
        DateTime afterDay_beforeTOD(date::year_month_day(date::year(2017), date::month(7), date::day(17)),
                                    std::chrono::seconds(1234));
        DateTime afterDay_beforeTOD_correct(date::year_month_day(date::year(2017), date::month(7), date::day(23)),
                                             std::chrono::seconds(12345));
        fructose_assert(ptr->getFirstIntakeInterval(afterDay_beforeTOD) == afterDay_beforeTOD_correct);

        // Day after the good day of the week, good time of day
        DateTime afterDay_goodTOD(date::year_month_day(date::year(2017), date::month(7), date::day(17)),
                                   std::chrono::seconds(12345));
        DateTime afterDay_goodTOD_correct(date::year_month_day(date::year(2017), date::month(7), date::day(23)),
                                           std::chrono::seconds(12345));
        fructose_assert(ptr->getFirstIntakeInterval(afterDay_goodTOD) == afterDay_goodTOD_correct);

        // Day after the good day of the week, after the time of day
        DateTime afterDay_afterTOD(date::year_month_day(date::year(2017), date::month(7), date::day(17)),
                                    std::chrono::seconds(54321));
        DateTime afterDay_afterTOD_correct(date::year_month_day(date::year(2017), date::month(7), date::day(23)),
                                            std::chrono::seconds(12345));
        fructose_assert(ptr->getFirstIntakeInterval(afterDay_afterTOD) == afterDay_afterTOD_correct);

        std::cout << "=> " << _testName << " succeeds!" << std::endl;
    }

    void testDosageTimeRange(const std::string& _testName)
    {
        std::cout << _testName << std::endl;

        // Give an undefined start date, expect an exception
        DateTime emptyDate;
        emptyDate.reset();
        fructose_assert_exception(new Tucuxi::Core::DosageTimeRange(emptyDate), std::invalid_argument);

        DateTime startDate(date::year_month_day(date::year(2017), date::month(7), date::day(17)),
                           std::chrono::seconds(12345));
        DateTime endDate(date::year_month_day(date::year(2017), date::month(7), date::day(23)),
                           std::chrono::seconds(12345));

        // Give a valid start date but no end date, expect no exception but the end date must be undefined
        fructose_assert_no_exception(new Tucuxi::Core::DosageTimeRange(startDate));
        std::unique_ptr<Tucuxi::Core::DosageTimeRange> ptr1(new Tucuxi::Core::DosageTimeRange(startDate));
        fructose_assert(ptr1->getStartDate() == startDate);
        fructose_assert(ptr1->getEndDate().isUndefined());

        // Same as above, but with start and end dates. Check also that the two dates are meaningful (start <= end).
        fructose_assert_exception(new Tucuxi::Core::DosageTimeRange(emptyDate, emptyDate), std::invalid_argument);
        fructose_assert_exception(new Tucuxi::Core::DosageTimeRange(emptyDate, endDate), std::invalid_argument);
        fructose_assert_exception(new Tucuxi::Core::DosageTimeRange(endDate, startDate), std::invalid_argument);
        fructose_assert_no_exception(new Tucuxi::Core::DosageTimeRange(startDate, emptyDate));
        fructose_assert_no_exception(new Tucuxi::Core::DosageTimeRange(startDate, endDate));
        std::unique_ptr<Tucuxi::Core::DosageTimeRange> ptr2(new Tucuxi::Core::DosageTimeRange(startDate, endDate));
        fructose_assert(ptr2->getStartDate() == startDate);
        fructose_assert(ptr2->getEndDate() == endDate);

        // Check that overlaps are properly detected
        DateTime beforeStartDate(date::year_month_day(date::year(2017), date::month(6), date::day(17)),
                                 std::chrono::seconds(12345));
        DateTime afterStartBeforeEndDate(date::year_month_day(date::year(2017), date::month(7), date::day(19)),
                                         std::chrono::seconds(12345));
        // Just after the previous one, but still before the end
        DateTime afterMiddleBeforeEndDate(date::year_month_day(date::year(2017), date::month(7), date::day(20)),
                                         std::chrono::seconds(12345));
        DateTime afterEndDate(date::year_month_day(date::year(2017), date::month(8), date::day(19)),
                              std::chrono::seconds(12345));

        std::unique_ptr<Tucuxi::Core::DosageTimeRange> beforeToMiddle(new Tucuxi::Core::DosageTimeRange(beforeStartDate,
                                                                                                        afterStartBeforeEndDate));
        assert(timeRangesOverlap(*ptr2, *beforeToMiddle));

        std::unique_ptr<Tucuxi::Core::DosageTimeRange> middleToAfter(new Tucuxi::Core::DosageTimeRange(afterStartBeforeEndDate,
                                                                                                       afterEndDate));
        assert(timeRangesOverlap(*ptr2, *middleToAfter));

        std::unique_ptr<Tucuxi::Core::DosageTimeRange> beforeToAfter(new Tucuxi::Core::DosageTimeRange(beforeStartDate,
                                                                                                       afterEndDate));
        assert(timeRangesOverlap(*ptr2, *beforeToAfter));

        std::unique_ptr<Tucuxi::Core::DosageTimeRange> insideInterval(new Tucuxi::Core::DosageTimeRange(afterStartBeforeEndDate,
                                                                                                        afterMiddleBeforeEndDate));
        assert(timeRangesOverlap(*ptr2, *insideInterval));

        std::unique_ptr<Tucuxi::Core::DosageTimeRange> beforeToOpenEnd(new Tucuxi::Core::DosageTimeRange(beforeStartDate));
        assert(timeRangesOverlap(*ptr2, *beforeToOpenEnd));

        std::unique_ptr<Tucuxi::Core::DosageTimeRange> middleToOpenEnd(new Tucuxi::Core::DosageTimeRange(afterStartBeforeEndDate));
        assert(timeRangesOverlap(*ptr2, *middleToOpenEnd));

        std::unique_ptr<Tucuxi::Core::DosageTimeRange> afterToOpenEnd(new Tucuxi::Core::DosageTimeRange(afterEndDate));
        assert(!timeRangesOverlap(*ptr2, *afterToOpenEnd));

        // Check the overlap detection for a time range against a list of time ranges
        DateTime before1(date::year_month_day(date::year(2017), date::month(1), date::day(17)),
                         std::chrono::seconds(12345));
        DateTime before2(date::year_month_day(date::year(2017), date::month(2), date::day(17)),
                         std::chrono::seconds(12345));
        DateTime before3(date::year_month_day(date::year(2017), date::month(3), date::day(17)),
                         std::chrono::seconds(12345));
        DateTime before4(date::year_month_day(date::year(2017), date::month(4), date::day(17)),
                         std::chrono::seconds(12345));
        DateTime before5(date::year_month_day(date::year(2017), date::month(5), date::day(17)),
                         std::chrono::seconds(12345));
        DateTime before6(date::year_month_day(date::year(2017), date::month(6), date::day(1)),
                         std::chrono::seconds(12345));

        DosageTimeRangeList trList;
        trList.emplace_back(new Tucuxi::Core::DosageTimeRange(before1, before2));
        trList.emplace_back(new Tucuxi::Core::DosageTimeRange(before2, before3));
        trList.emplace_back(new Tucuxi::Core::DosageTimeRange(before3, before4));
        trList.emplace_back(new Tucuxi::Core::DosageTimeRange(before4, before5));
        trList.emplace_back(new Tucuxi::Core::DosageTimeRange(before5, before6));

        assert(!timeRangesOverlap(*ptr2, trList));

        trList.emplace_back(new Tucuxi::Core::DosageTimeRange(before5));
        assert(timeRangesOverlap(*ptr2, trList));

        std::cout << "=> " << _testName << " succeeds!" << std::endl;
    }
};

#endif // TEST_DOSAGE_H
