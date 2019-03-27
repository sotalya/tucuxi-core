/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TEST_INTAKEEXTRACTOR_H
#define TEST_INTAKEEXTRACTOR_H

#include <iostream>
#include <memory>

#include "fructose/fructose.h"

#include "tucucore/dosage.h"
#include "tucucore/intakeextractor.h"
#include "tucucore/drugmodel/formulationandroute.h"

using namespace Tucuxi::Core;
using namespace Tucuxi::Common;

struct TestIntakeExtractor : public fructose::test_base<TestIntakeExtractor>
{
    static const int CYCLE_SIZE = 251;

    static constexpr double NB_POINTS_PER_HOUR = CYCLE_SIZE / 24.0;

    TestIntakeExtractor() { }


    Tucuxi::Core::FormulationAndRoute getInfusionFormulationAndRoute()
    {
        return Tucuxi::Core::FormulationAndRoute(
                    Formulation::Test,
                    AdministrationRoute::IntravenousDrip,
                    AbsorptionModel::Infusion);
    }

    FormulationAndRoute getBolusFormulationAndRoute()
    {
        return FormulationAndRoute(
                    Formulation::Test,
                    AdministrationRoute::IntravenousBolus,
                    AbsorptionModel::Intravascular);
    }

    FormulationAndRoute getExtraFormulationAndRoute() {
        return FormulationAndRoute(
                    Formulation::Test,
                    AdministrationRoute::Intramuscular,
                    AbsorptionModel::Extravascular);
    }



    /// \brief Scenario where a treatment is given once per week.
    /// \param _testName Test name.
    void testOncePerWeek(const std::string& /* _testName */)
    {
        // List of time ranges that will be pushed into the history
        DosageTimeRangeList timeRangeList;

        // Create a time range for the month of June 2017 and one for the first three weeks of July 2017 (counting the
        // first Saturday as a week)
        DateTime startJune2017(date::year_month_day(date::year(2017), date::month(6), date::day(1)),
                               std::chrono::seconds(0));
        DateTime endJune2017(date::year_month_day(date::year(2017), date::month(7), date::day(1)),
                             std::chrono::seconds(0));
        DateTime startJuly2017(date::year_month_day(date::year(2017), date::month(7), date::day(1)),
                               std::chrono::seconds(0));
        DateTime endJuly2017(date::year_month_day(date::year(2017), date::month(7), date::day(16)),
                             std::chrono::seconds(0));

        // Add a weekly treatment in June
        // 200mg via a 20-minutes perfusion at 08h30 on Tuesday
        WeeklyDose juneWeeklyDose(DoseValue(200.0),
                                  getInfusionFormulationAndRoute(),
                                  Duration(std::chrono::minutes(20)),
                                  TimeOfDay(Duration(std::chrono::hours(8),
                                                     std::chrono::minutes(30),
                                                     std::chrono::seconds(0))),
                                  DayOfWeek(TUESDAY));
        DosageLoop juneDose(juneWeeklyDose);
        std::unique_ptr<Tucuxi::Core::DosageTimeRange> june2017(new Tucuxi::Core::DosageTimeRange(startJune2017, endJune2017, juneDose));

        // Add a weekly treatment in July
        // 400mg via a 10-minutes perfusion at 11h30 on Wednesday
        WeeklyDose julyWeeklyDose(DoseValue(400.0),
                                  getInfusionFormulationAndRoute(),
                                  Duration(std::chrono::minutes(10)),
                                  TimeOfDay(Duration(std::chrono::hours(11),
                                                     std::chrono::minutes(30),
                                                     std::chrono::seconds(0))),
                                  DayOfWeek(WEDNESDAY));
        DosageLoop julyDose(julyWeeklyDose);
        std::unique_ptr<Tucuxi::Core::DosageTimeRange> july2017(new Tucuxi::Core::DosageTimeRange(startJuly2017, endJuly2017, julyDose));
        assert(!timeRangesOverlap(*june2017, *july2017));

        // Create the dosage history
        std::unique_ptr<DosageHistory> dh = std::make_unique<DosageHistory>();
        dh->addTimeRange(*june2017);
        dh->addTimeRange(*july2017);

        // Expected intake series
        IntakeSeries expectedIntakes;
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(6)),
                                                       Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(200.0),
                                              Duration(std::chrono::hours(7 * 24)),
                                              getInfusionFormulationAndRoute(),
                                              getInfusionFormulationAndRoute().getAbsorptionModel(),
                                              Duration(std::chrono::minutes(20)),
                                              7 * 24 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(13)),
                                                       Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(200.0),
                                              Duration(std::chrono::hours(7 * 24)),
                                              getInfusionFormulationAndRoute(),
                                              getInfusionFormulationAndRoute().getAbsorptionModel(),
                                              Duration(std::chrono::minutes(20)),
                                              7 * 24 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(20)),
                                                       Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(200.0),
                                              Duration(std::chrono::hours(7 * 24)),
                                              getInfusionFormulationAndRoute(),
                                              getInfusionFormulationAndRoute().getAbsorptionModel(),
                                              Duration(std::chrono::minutes(20)),
                                              7 * 24 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(27)),
                                                       Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(200.0),
                                              Duration(std::chrono::hours(7 * 24)),
                                              getInfusionFormulationAndRoute(),
                                              getInfusionFormulationAndRoute().getAbsorptionModel(),
                                              Duration(std::chrono::minutes(20)),
                                              7 * 24 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(7), date::day(5)),
                                                       Duration(std::chrono::hours(11), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(400.0),
                                              Duration(std::chrono::hours(7 * 24)),
                                              getInfusionFormulationAndRoute(),
                                              getInfusionFormulationAndRoute().getAbsorptionModel(),
                                              Duration(std::chrono::minutes(10)),
                                              7 * 24 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(7), date::day(12)),
                                                       Duration(std::chrono::hours(11), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(400.0),
                                              Duration(std::chrono::hours(7 * 24)),
                                              getInfusionFormulationAndRoute(),
                                              getInfusionFormulationAndRoute().getAbsorptionModel(),
                                              Duration(std::chrono::minutes(10)),
                                              7 * 24 * NB_POINTS_PER_HOUR + 1));

        // Extract the intake series
        DateTime fullPeriodStart(date::year_month_day(date::year(2017), date::month(6), date::day(1)),
                                 std::chrono::seconds(0));
        DateTime fullPeriodEnd(date::year_month_day(date::year(2017), date::month(7), date::day(16)),
                               std::chrono::seconds(0));
        IntakeSeries iSeries;
        IntakeExtractor extractor;
        IntakeExtractor::Result result = extractor.extract(*dh, fullPeriodStart, fullPeriodEnd, NB_POINTS_PER_HOUR, iSeries);
        fructose_assert(result == IntakeExtractor::Result::Ok);

        fructose_assert(iSeries.size() == expectedIntakes.size());

        for (size_t i = 0; i < iSeries.size(); ++i) {
            fructose_assert(iSeries.at(i) == expectedIntakes.at(i));
        }

        {
            // Try with inverted end/start dates
            IntakeExtractor extractor;
            IntakeExtractor::Result result = extractor.extract(*dh, fullPeriodEnd, fullPeriodStart, NB_POINTS_PER_HOUR, iSeries);
            fructose_assert(result == IntakeExtractor::Result::ExtractionError);
        }
    }


    /// \brief Scenario where a treatment is given once every ten days in the first time range, once every 4 days in
    /// the second.
    /// \param _testName Test name.
    void testOnceEveryTenDays(const std::string& /* _testName */)
    {
        // List of time ranges that will be pushed into the history
        DosageTimeRangeList timeRangeList;

        // Create a time range for the month of June 2017 and one for the first three weeks of July 2017 (counting the
        // first Saturday as a week)
        DateTime startJune2017(date::year_month_day(date::year(2017), date::month(6), date::day(1)),
                               Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0)));
        DateTime endJune2017(date::year_month_day(date::year(2017), date::month(7), date::day(1)),
                             Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0)));
        // Lasting doses start at the beginning of their time range, we have thus to start on the good day
        DateTime startJuly2017(date::year_month_day(date::year(2017), date::month(7), date::day(4)),
                               Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0)));
        DateTime endJuly2017(date::year_month_day(date::year(2017), date::month(7), date::day(16)),
                             Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0)));

        // Add a treatment intake every ten days in June
        // 200mg via a 20-minutes perfusion at 08h30, starting the 01.06
        LastingDose junePeriodicDose(DoseValue(200.0),
                                     getInfusionFormulationAndRoute(),
                                     Duration(std::chrono::minutes(20)),
                                     Duration(std::chrono::hours(10 * 24)));
        DosageLoop juneDose(junePeriodicDose);
        std::unique_ptr<Tucuxi::Core::DosageTimeRange> june2017(new Tucuxi::Core::DosageTimeRange(startJune2017, endJune2017, juneDose));

        // Add a treatment intake every four days in June
        // 400mg via a 10-minutes perfusion at 11h30, starting the 01.07
        LastingDose julyPeriodicDose(DoseValue(400.0),
                                     getInfusionFormulationAndRoute(),
                                     Duration(std::chrono::minutes(10)),
                                     Duration(std::chrono::hours(4 * 24)));
        DosageLoop julyDose(julyPeriodicDose);
        std::unique_ptr<Tucuxi::Core::DosageTimeRange> july2017(new Tucuxi::Core::DosageTimeRange(startJuly2017, endJuly2017, julyDose));
        assert(!timeRangesOverlap(*june2017, *july2017));

        // Create the dosage history
        DosageHistory dh;
        dh.addTimeRange(*june2017);
        dh.addTimeRange(*july2017);

        // Expected intake series
        IntakeSeries expectedIntakes;
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(1)),
                                                       Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(200.0),
                                              Duration(std::chrono::hours(10 * 24)),
                                              getInfusionFormulationAndRoute(),
                                              getInfusionFormulationAndRoute().getAbsorptionModel(),
                                              Duration(std::chrono::minutes(20)),
                                              10 * 24 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(11)),
                                                       Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(200.0),
                                              Duration(std::chrono::hours(10 * 24)),
                                              getInfusionFormulationAndRoute(),
                                              getInfusionFormulationAndRoute().getAbsorptionModel(),
                                              Duration(std::chrono::minutes(20)),
                                              10 * 24 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(21)),
                                                       Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(200.0),
                                              Duration(std::chrono::hours(10 * 24)),
                                              getInfusionFormulationAndRoute(),
                                              getInfusionFormulationAndRoute().getAbsorptionModel(),
                                              Duration(std::chrono::minutes(20)),
                                              10 * 24 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(7), date::day(4)),
                                                       Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(400.0),
                                              Duration(std::chrono::hours(4 * 24)),
                                              getInfusionFormulationAndRoute(),
                                              getInfusionFormulationAndRoute().getAbsorptionModel(),
                                              Duration(std::chrono::minutes(10)),
                                              4 * 24 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(7), date::day(8)),
                                                       Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(400.0),
                                              Duration(std::chrono::hours(4 * 24)),
                                              getInfusionFormulationAndRoute(),
                                              getInfusionFormulationAndRoute().getAbsorptionModel(),
                                              Duration(std::chrono::minutes(10)),
                                              4 * 24 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(7), date::day(12)),
                                                       Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(400.0),
                                              Duration(std::chrono::hours(4 * 24)),
                                              getInfusionFormulationAndRoute(),
                                              getInfusionFormulationAndRoute().getAbsorptionModel(),
                                              Duration(std::chrono::minutes(10)),
                                              4 * 24 * NB_POINTS_PER_HOUR + 1));
        // The 16.07.2017 must be EXCLUDED (the intervals are closed on the left, but opened on the right side!)

        DateTime fullPeriodStart(date::year_month_day(date::year(2017), date::month(6), date::day(1)),
                                 Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0)));
        DateTime fullPeriodEnd(date::year_month_day(date::year(2017), date::month(7), date::day(16)),
                               Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0)));
        IntakeSeries iSeries;
        IntakeExtractor extractor;
        IntakeExtractor::Result result = extractor.extract(dh, fullPeriodStart, fullPeriodEnd, NB_POINTS_PER_HOUR, iSeries);
        fructose_assert(result == IntakeExtractor::Result::Ok);

        fructose_assert(iSeries.size() == expectedIntakes.size());

        for (size_t i = 0; i < iSeries.size(); ++i) {
            fructose_assert(iSeries.at(i) == expectedIntakes.at(i));
        }
    }


    /// \brief Scenario where a treatment is given once every 36 hours.
    /// \param _testName Test name.
    void testOnceEvery36Hours(const std::string& /* _testName */)
    {
        // List of time ranges that will be pushed into the history
        DosageTimeRangeList timeRangeList;

        // Create a time range for the first week of June 2017
        DateTime startJune2017(date::year_month_day(date::year(2017), date::month(6), date::day(1)),
                               Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0)));
        DateTime endJune2017(date::year_month_day(date::year(2017), date::month(7), date::day(8)),
                             Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0)));

        // Add a treatment intake every ten days in June
        // 200mg via a intravascular at 08h30, starting the 01.06
        LastingDose junePeriodicDose(DoseValue(200.0),
                                     getBolusFormulationAndRoute(),
                                     Duration(),
                                     Duration(std::chrono::hours(36)));
        DosageLoop juneDose(junePeriodicDose);
        std::unique_ptr<Tucuxi::Core::DosageTimeRange> june2017(new Tucuxi::Core::DosageTimeRange(startJune2017, endJune2017, juneDose));

        // Create the dosage history
        DosageHistory dh;
        dh.addTimeRange(*june2017);

        // Expected intake series
        IntakeSeries expectedIntakes;
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(1)),
                                                       Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(200.0),
                                              Duration(std::chrono::hours(36)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              36 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                                                       Duration(std::chrono::hours(20), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(200.0),
                                              Duration(std::chrono::hours(36)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              36 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(4)),
                                                       Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(200.0),
                                              Duration(std::chrono::hours(36)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              36 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(5)),
                                                       Duration(std::chrono::hours(20), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(200.0),
                                              Duration(std::chrono::hours(36)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              36 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(7)),
                                                       Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(200.0),
                                              Duration(std::chrono::hours(36)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              36 * NB_POINTS_PER_HOUR + 1));

        // The 16.07.2017 must be EXCLUDED (the intervals are closed on the left, but opened on the right side!)

        DateTime fullPeriodStart(date::year_month_day(date::year(2017), date::month(6), date::day(1)),
                                 Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0)));
        DateTime fullPeriodEnd(date::year_month_day(date::year(2017), date::month(6), date::day(8)),
                               Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0)));
        IntakeSeries iSeries;
        IntakeExtractor extractor;
        IntakeExtractor::Result result = extractor.extract(dh, fullPeriodStart, fullPeriodEnd, NB_POINTS_PER_HOUR, iSeries);
        fructose_assert(result == IntakeExtractor::Result::Ok);

        fructose_assert(iSeries.size() == expectedIntakes.size());

        for (size_t i = 0; i < iSeries.size(); ++i) {
            fructose_assert(iSeries.at(i) == expectedIntakes.at(i));
        }
    }


    /// \brief Scenario where a treatment is given 5 times at a 12 hours interval.
    /// \param _testName Test name.
    void testFiveTimesEvery12Hours(const std::string& /* _testName */)
    {
        // List of time ranges that will be pushed into the history
        DosageTimeRangeList timeRangeList;

        // Create a time range starting at the beginning of june 2017, with no upper end (to test that the repetitions
        // are handled correctly)
        DateTime startJune2017(date::year_month_day(date::year(2017), date::month(6), date::day(1)),
                               Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0)));

        // Add a treatment intake every ten days in June
        // 200mg via a intravascular at 08h30, starting the 01.06
        LastingDose periodicDose(DoseValue(200.0),
                                 getBolusFormulationAndRoute(),
                                 Duration(),
                                 Duration(std::chrono::hours(12)));
        DosageRepeat repeatedDose(periodicDose, 5);
        std::unique_ptr<Tucuxi::Core::DosageTimeRange> june2017(new Tucuxi::Core::DosageTimeRange(startJune2017, repeatedDose));

        // Create the dosage history
        DosageHistory dh;
        dh.addTimeRange(*june2017);

        // Expected intake series
        IntakeSeries expectedIntakes;
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(1)),
                                                       Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(200.0),
                                              Duration(std::chrono::hours(12)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              12 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(1)),
                                                       Duration(std::chrono::hours(20), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(200.0),
                                              Duration(std::chrono::hours(12)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              12 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                                                       Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(200.0),
                                              Duration(std::chrono::hours(12)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              12 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                                                       Duration(std::chrono::hours(20), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(200.0),
                                              Duration(std::chrono::hours(12)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              12 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(3)),
                                                       Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(200.0),
                                              Duration(std::chrono::hours(12)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              12 * NB_POINTS_PER_HOUR + 1));

        DateTime fullPeriodStart(date::year_month_day(date::year(2017), date::month(6), date::day(1)),
                                 Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0)));
        IntakeSeries iSeries;
        IntakeExtractor extractor;
        IntakeExtractor::Result result = extractor.extract(dh, fullPeriodStart, DateTime(), NB_POINTS_PER_HOUR, iSeries);
        fructose_assert(result == IntakeExtractor::Result::Ok);

        fructose_assert(iSeries.size() == expectedIntakes.size());

        for (size_t i = 0; i < iSeries.size(); ++i) {
            fructose_assert(iSeries.at(i) == expectedIntakes.at(i));
        }
    }


    /// \brief Scenario where a treatment is given 5 times at a 12 hours interval, and we stop the extraction after 3.
    /// \param _testName Test name.
    void testFiveTimesEvery12HoursEarlyStop(const std::string& /* _testName */)
    {
        // List of time ranges that will be pushed into the history
        DosageTimeRangeList timeRangeList;

        // Create a time range starting at the beginning of june 2017, with no upper end (to test that the repetitions
        // are handled correctly)
        DateTime startJune2017(date::year_month_day(date::year(2017), date::month(6), date::day(1)),
                               Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0)));

        // Add a treatment intake every ten days in June
        // 200mg via a intravascular at 08h30, starting the 01.06
        LastingDose periodicDose(DoseValue(200.0),
                                 getBolusFormulationAndRoute(),
                                 Duration(),
                                 Duration(std::chrono::hours(12)));
        DosageRepeat repeatedDose(periodicDose, 5);
        std::unique_ptr<Tucuxi::Core::DosageTimeRange> june2017(new Tucuxi::Core::DosageTimeRange(startJune2017, repeatedDose));

        // Create the dosage history
        DosageHistory dh;
        dh.addTimeRange(*june2017);

        // Expected intake series
        IntakeSeries expectedIntakes;
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(1)),
                                                       Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(200.0),
                                              Duration(std::chrono::hours(12)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              12 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(1)),
                                                       Duration(std::chrono::hours(20), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(200.0),
                                              Duration(std::chrono::hours(12)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              12 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                                                       Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(200.0),
                                              Duration(std::chrono::hours(12)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              12 * NB_POINTS_PER_HOUR + 1));


        DateTime fullPeriodStart(date::year_month_day(date::year(2017), date::month(6), date::day(1)),
                                 Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0)));
        DateTime fullPeriodEnd(date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                               Duration(std::chrono::hours(20), std::chrono::minutes(30), std::chrono::seconds(0)));
        IntakeSeries iSeries;
        IntakeExtractor extractor;
        IntakeExtractor::Result result = extractor.extract(dh, fullPeriodStart, fullPeriodEnd, NB_POINTS_PER_HOUR, iSeries);
        fructose_assert(result == IntakeExtractor::Result::Ok);

        fructose_assert(iSeries.size() == expectedIntakes.size());

        for (size_t i = 0; i < iSeries.size(); ++i) {
            fructose_assert(iSeries.at(i) == expectedIntakes.at(i));
        }
    }


    /// \brief Scenario where a treatment is given twice in the same day, once with 200mg and the other time with
    /// 300mg, with a ten hours delay, but only every ten days, and this 3 times in a row.
    /// \param _testName Test name.
    void testTwiceEveryTenDays(const std::string& /* _testName */)
    {
        // List of time ranges that will be pushed into the history
        DosageTimeRangeList timeRangeList;

        // Create a time range starting at the beginning of june 2017, with no upper end (to test that the repetitions
        // are handled correctly)
        DateTime startJune2017(date::year_month_day(date::year(2017), date::month(6), date::day(1)),
                               Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0)));

        // Add a treatment intake every ten days in June
        LastingDose periodicDose200(DoseValue(200.0),
                                    getBolusFormulationAndRoute(),
                                    Duration(),
                                    Duration(std::chrono::hours(10)));
        LastingDose periodicDose300(DoseValue(300.0),
                                    getBolusFormulationAndRoute(),
                                    Duration(),
                                    Duration(std::chrono::hours(10 * 23)));
        DosageSequence ds(periodicDose200);
        ds.addDosage(periodicDose300);
        DosageRepeat repeatedDose(ds, 3);
        std::unique_ptr<Tucuxi::Core::DosageTimeRange> june2017(new Tucuxi::Core::DosageTimeRange(startJune2017, repeatedDose));

        // Create the dosage history
        DosageHistory dh;
        dh.addTimeRange(*june2017);

        // Expected intake series
        IntakeSeries expectedIntakes;
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(1)),
                                                       Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(200.0),
                                              Duration(std::chrono::hours(10)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              10 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(1)),
                                                       Duration(std::chrono::hours(18), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(300.0),
                                              Duration(std::chrono::hours(10 * 23)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              10 * 23 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(11)),
                                                       Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(200.0),
                                              Duration(std::chrono::hours(10)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              10 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(11)),
                                                       Duration(std::chrono::hours(18), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(300.0),
                                              Duration(std::chrono::hours(10 * 23)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              10 * 23 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(21)),
                                                       Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(200.0),
                                              Duration(std::chrono::hours(10)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              10 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(21)),
                                                       Duration(std::chrono::hours(18), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(300.0),
                                              Duration(std::chrono::hours(10 * 23)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              10 * 23 * NB_POINTS_PER_HOUR + 1));

        DateTime fullPeriodStart(date::year_month_day(date::year(2017), date::month(6), date::day(1)),
                                 Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0)));
        IntakeSeries iSeries;
        IntakeExtractor extractor;
        IntakeExtractor::Result result = extractor.extract(dh, fullPeriodStart, DateTime(), NB_POINTS_PER_HOUR, iSeries);
        fructose_assert(result == IntakeExtractor::Result::Ok);

        fructose_assert(iSeries.size() == expectedIntakes.size());

        for (size_t i = 0; i < iSeries.size(); ++i) {
            fructose_assert(iSeries.at(i) == expectedIntakes.at(i));
        }
    }


    /// \brief Scenario where a treatment is given once every day for the first full week of june 2017.
    /// \param _testName Test name.
    void testOnceEveryDay(const std::string& /* _testName */)
    {
        // List of time ranges that will be pushed into the history
        DosageTimeRangeList timeRangeList;

        // Create a time range for the first week of June 2017
        DateTime startJune2017(date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                               Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0)));
        DateTime endJune2017(date::year_month_day(date::year(2017), date::month(7), date::day(9)),
                             Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0)));

        // Add a treatment intake every day in the period
        // 200mg via a intravascular at 12h30, starting the 02.06
        DailyDose junePeriodicDose(DoseValue(200.0),
                                   getBolusFormulationAndRoute(),
                                   Duration(),
                                   TimeOfDay(Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))));
        DosageLoop juneDose(junePeriodicDose);
        std::unique_ptr<Tucuxi::Core::DosageTimeRange> june2017(new Tucuxi::Core::DosageTimeRange(startJune2017, endJune2017, juneDose));

        // Create the dosage history
        DosageHistory dh;
        dh.addTimeRange(*june2017);

        // Expected intake series
        IntakeSeries expectedIntakes;
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                                                       Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(200.0),
                                              Duration(std::chrono::hours(24)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              24 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(3)),
                                                       Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(200.0),
                                              Duration(std::chrono::hours(24)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              24 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(4)),
                                                       Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(200.0),
                                              Duration(std::chrono::hours(24)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              24 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(5)),
                                                       Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(200.0),
                                              Duration(std::chrono::hours(24)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              24 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(6)),
                                                       Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(200.0),
                                              Duration(std::chrono::hours(24)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              24 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(7)),
                                                       Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(200.0),
                                              Duration(std::chrono::hours(24)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              24 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(8)),
                                                       Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(200.0),
                                              Duration(std::chrono::hours(24)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              24 * NB_POINTS_PER_HOUR + 1));

        DateTime fullPeriodStart(date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                                 Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0)));
        DateTime fullPeriodEnd(date::year_month_day(date::year(2017), date::month(6), date::day(9)),
                               Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0)));
        IntakeSeries iSeries;
        IntakeExtractor extractor;
        IntakeExtractor::Result result = extractor.extract(dh, fullPeriodStart, fullPeriodEnd, NB_POINTS_PER_HOUR, iSeries);
        fructose_assert(result == IntakeExtractor::Result::Ok);

        fructose_assert(iSeries.size() == expectedIntakes.size());

        for (size_t i = 0; i < iSeries.size(); ++i) {
            fructose_assert(iSeries.at(i) == expectedIntakes.at(i));
        }
    }


    /// \brief Scenario where a treatment is given three times a day, every day of the first full week of June 2017.
    /// \param _testName Test name.
    void testThreeTimesEveryDay(const std::string& /* _testName */)
    {
        // List of time ranges that will be pushed into the history
        DosageTimeRangeList timeRangeList;

        // Create a time range for the first week of June 2017
        DateTime startJune2017(date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                               Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0)));
        DateTime endJune2017(date::year_month_day(date::year(2017), date::month(7), date::day(9)),
                             Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0)));

        // Add the treatments intake every day in the period
        // 200mg via a intravascular at 08h30, starting the 02.06
        DailyDose dose1(DoseValue(200.0),
                        getBolusFormulationAndRoute(),
                        Duration(),
                        TimeOfDay(Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))));
        // 300mg via a intravascular at 12h30, starting the 02.06
        DailyDose dose2(DoseValue(300.0),
                        getBolusFormulationAndRoute(),
                        Duration(),
                        TimeOfDay(Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))));
        // 400mg via a intravascular at 16h30, starting the 02.06
        DailyDose dose3(DoseValue(400.0),
                        getBolusFormulationAndRoute(),
                        Duration(),
                        TimeOfDay(Duration(std::chrono::hours(16), std::chrono::minutes(30), std::chrono::seconds(0))));
        ParallelDosageSequence ds(dose1, Duration());
        ds.addDosage(dose2, Duration());
        ds.addDosage(dose3, Duration());
        DosageLoop juneDose(ds);
        std::unique_ptr<Tucuxi::Core::DosageTimeRange> june2017(new Tucuxi::Core::DosageTimeRange(startJune2017, endJune2017, juneDose));

        // Create the dosage history
        DosageHistory dh;
        dh.addTimeRange(*june2017);

        // Expected intake series
        IntakeSeries expectedIntakes;
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                                                       Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(200.0),
                                              Duration(std::chrono::hours(24)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              24 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                                                       Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(300.0),
                                              Duration(std::chrono::hours(24)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              24 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                                                       Duration(std::chrono::hours(16), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(400.0),
                                              Duration(std::chrono::hours(24)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              24 * NB_POINTS_PER_HOUR + 1));


        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(3)),
                                                       Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(200.0),
                                              Duration(std::chrono::hours(24)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              24 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(3)),
                                                       Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(300.0),
                                              Duration(std::chrono::hours(24)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              24 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(3)),
                                                       Duration(std::chrono::hours(16), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(400.0),
                                              Duration(std::chrono::hours(24)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              24 * NB_POINTS_PER_HOUR + 1));

        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(4)),
                                                       Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(200.0),
                                              Duration(std::chrono::hours(24)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              24 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(4)),
                                                       Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(300.0),
                                              Duration(std::chrono::hours(24)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              24 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(4)),
                                                       Duration(std::chrono::hours(16), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(400.0),
                                              Duration(std::chrono::hours(24)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              24 * NB_POINTS_PER_HOUR + 1));


        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(5)),
                                                       Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(200.0),
                                              Duration(std::chrono::hours(24)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              24 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(5)),
                                                       Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(300.0),
                                              Duration(std::chrono::hours(24)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              24 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(5)),
                                                       Duration(std::chrono::hours(16), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(400.0),
                                              Duration(std::chrono::hours(24)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              24 * NB_POINTS_PER_HOUR + 1));

        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(6)),
                                                       Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(200.0),
                                              Duration(std::chrono::hours(24)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              24 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(6)),
                                                       Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(300.0),
                                              Duration(std::chrono::hours(24)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              24 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(6)),
                                                       Duration(std::chrono::hours(16), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(400.0),
                                              Duration(std::chrono::hours(24)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              24 * NB_POINTS_PER_HOUR + 1));


        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(7)),
                                                       Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(200.0),
                                              Duration(std::chrono::hours(24)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              24 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(7)),
                                                       Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(300.0),
                                              Duration(std::chrono::hours(24)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              24 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(7)),
                                                       Duration(std::chrono::hours(16), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(400.0),
                                              Duration(std::chrono::hours(24)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              24 * NB_POINTS_PER_HOUR + 1));

        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(8)),
                                                       Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(200.0),
                                              Duration(std::chrono::hours(24)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              24 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(8)),
                                                       Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(300.0),
                                              Duration(std::chrono::hours(24)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              24 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(8)),
                                                       Duration(std::chrono::hours(16), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(400.0),
                                              Duration(std::chrono::hours(24)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              24 * NB_POINTS_PER_HOUR + 1));

        DateTime fullPeriodStart(date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                                 Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0)));
        DateTime fullPeriodEnd(date::year_month_day(date::year(2017), date::month(6), date::day(9)),
                               Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0)));
        IntakeSeries iSeries;
        IntakeExtractor extractor;
        IntakeExtractor::Result result = extractor.extract(dh, fullPeriodStart, fullPeriodEnd, NB_POINTS_PER_HOUR, iSeries);
        fructose_assert(result == IntakeExtractor::Result::Ok);

        fructose_assert(iSeries.size() == expectedIntakes.size());

        for (size_t i = 0; i < expectedIntakes.size(); ++i) {
            fructose_assert(iSeries.at(i) == expectedIntakes.at(i));
        }
    }

    /// \brief Complex scenario to test ParallelSequence capabilities.
    /// The treatment is given:
    /// - twice a day (at 12h00 and 22h00)
    /// - 10x every 3 hours, with an initial offset of 2h30
    /// - 4x every 6 hours, with an initial offset of 6h30
    /// - the whole processus is repeated twice
    /// \param _testName Test name.
    void testComplexParallelSequence1(const std::string& /* _testName */)
    {
        // List of time ranges that will be pushed into the history
        DosageTimeRangeList timeRangeList;

        // Create a time range for the first week of June 2017, no end to test DosageRepeat
        DateTime startJune2017(date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                               Duration(std::chrono::hours(0), std::chrono::minutes(0), std::chrono::seconds(0)));

        // Add the treatments
        DailyDose firstDailyDose(DoseValue(100.0),
                                 getInfusionFormulationAndRoute(),
                                 Duration(std::chrono::minutes(20)),
                                 TimeOfDay(Duration(std::chrono::hours(12), std::chrono::minutes(0), std::chrono::seconds(0))));
        DailyDose secondDailyDose(DoseValue(200.0),
                                  getInfusionFormulationAndRoute(),
                                  Duration(std::chrono::minutes(10)),
                                  TimeOfDay(Duration(std::chrono::hours(22), std::chrono::minutes(0), std::chrono::seconds(0))));

        LastingDose doseEvery3Hours(DoseValue(300.0),
                                    getBolusFormulationAndRoute(),
                                    Duration(),
                                    Duration(std::chrono::hours(3)));

        LastingDose doseEvery6Hours(DoseValue(400.0),
                                    getExtraFormulationAndRoute(),
                                    Duration(),
                                    Duration(std::chrono::hours(6)));

        ParallelDosageSequence ds(firstDailyDose, Duration());
        ds.addDosage(secondDailyDose, Duration());
        DosageRepeat repeatEvery3Hours(doseEvery3Hours, 10);
        ds.addDosage(repeatEvery3Hours, Duration(std::chrono::hours(2), std::chrono::minutes(30), std::chrono::seconds(0)));
        DosageRepeat repeatEvery6Hours(doseEvery6Hours, 4);
        ds.addDosage(repeatEvery6Hours, Duration(std::chrono::hours(6), std::chrono::minutes(30), std::chrono::seconds(0)));
        DosageRepeat juneDoses(ds, 2);
        std::unique_ptr<Tucuxi::Core::DosageTimeRange> june2017(new Tucuxi::Core::DosageTimeRange(startJune2017, juneDoses));

        Duration expectedTimeStepDS(std::chrono::hours(32), std::chrono::minutes(30), std::chrono::seconds(0));
        fructose_assert(ds.getTimeStep() == expectedTimeStepDS);
        fructose_assert(juneDoses.getTimeStep() == expectedTimeStepDS + expectedTimeStepDS);

        // Create the dosage history
        DosageHistory dh;
        dh.addTimeRange(*june2017);

        // Expected intake series
        IntakeSeries expectedIntakes;
        // Intakes for doseEvery3Hours
        // First repeat
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                                                       Duration(std::chrono::hours(2), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(300.0),
                                              Duration(std::chrono::hours(3)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              3 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                                                       Duration(std::chrono::hours(5), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(300.0),
                                              Duration(std::chrono::hours(3)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              3 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                                                       Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(300.0),
                                              Duration(std::chrono::hours(3)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              3 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                                                       Duration(std::chrono::hours(11), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(300.0),
                                              Duration(std::chrono::hours(3)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              3 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                                                       Duration(std::chrono::hours(14), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(300.0),
                                              Duration(std::chrono::hours(3)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              3 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                                                       Duration(std::chrono::hours(17), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(300.0),
                                              Duration(std::chrono::hours(3)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              3 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                                                       Duration(std::chrono::hours(20), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(300.0),
                                              Duration(std::chrono::hours(3)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              3 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                                                       Duration(std::chrono::hours(23), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(300.0),
                                              Duration(std::chrono::hours(3)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              3 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(3)),
                                                       Duration(std::chrono::hours(2), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(300.0),
                                              Duration(std::chrono::hours(3)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              3 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(3)),
                                                       Duration(std::chrono::hours(5), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(300.0),
                                              Duration(std::chrono::hours(3)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              3 * NB_POINTS_PER_HOUR + 1));
        // Second repeat
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(3)),
                                                       Duration(std::chrono::hours(11), std::chrono::minutes(0), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(300.0),
                                              Duration(std::chrono::hours(3)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              3 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(3)),
                                                       Duration(std::chrono::hours(14), std::chrono::minutes(0), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(300.0),
                                              Duration(std::chrono::hours(3)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              3 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(3)),
                                                       Duration(std::chrono::hours(17), std::chrono::minutes(0), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(300.0),
                                              Duration(std::chrono::hours(3)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              3 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(3)),
                                                       Duration(std::chrono::hours(20), std::chrono::minutes(0), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(300.0),
                                              Duration(std::chrono::hours(3)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              3 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(3)),
                                                       Duration(std::chrono::hours(23), std::chrono::minutes(0), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(300.0),
                                              Duration(std::chrono::hours(3)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              3 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(4)),
                                                       Duration(std::chrono::hours(2), std::chrono::minutes(0), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(300.0),
                                              Duration(std::chrono::hours(3)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              3 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(4)),
                                                       Duration(std::chrono::hours(5), std::chrono::minutes(0), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(300.0),
                                              Duration(std::chrono::hours(3)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              3 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(4)),
                                                       Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(300.0),
                                              Duration(std::chrono::hours(3)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              3 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(4)),
                                                       Duration(std::chrono::hours(11), std::chrono::minutes(0), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(300.0),
                                              Duration(std::chrono::hours(3)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              3 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(4)),
                                                       Duration(std::chrono::hours(14), std::chrono::minutes(0), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(300.0),
                                              Duration(std::chrono::hours(3)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              3 * NB_POINTS_PER_HOUR + 1));

        // Intakes for doseEvery6Hours
        // First repeat
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                                                       Duration(std::chrono::hours(6), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(400.0),
                                              Duration(std::chrono::hours(6)),
                                              getExtraFormulationAndRoute(),
                                              getExtraFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              6 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                                                       Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(400.0),
                                              Duration(std::chrono::hours(6)),
                                              getExtraFormulationAndRoute(),
                                              getExtraFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              6 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                                                       Duration(std::chrono::hours(18), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(400.0),
                                              Duration(std::chrono::hours(6)),
                                              getExtraFormulationAndRoute(),
                                              getExtraFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              6 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(3)),
                                                       Duration(std::chrono::hours(0), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(400.0),
                                              Duration(std::chrono::hours(6)),
                                              getExtraFormulationAndRoute(),
                                              getExtraFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              6 * NB_POINTS_PER_HOUR + 1));
        // Second repeat
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(3)),
                                                       Duration(std::chrono::hours(15), std::chrono::minutes(0), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(400.0),
                                              Duration(std::chrono::hours(6)),
                                              getExtraFormulationAndRoute(),
                                              getExtraFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              6 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(3)),
                                                       Duration(std::chrono::hours(21), std::chrono::minutes(0), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(400.0),
                                              Duration(std::chrono::hours(6)),
                                              getExtraFormulationAndRoute(),
                                              getExtraFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              6 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(4)),
                                                       Duration(std::chrono::hours(3), std::chrono::minutes(0), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(400.0),
                                              Duration(std::chrono::hours(6)),
                                              getExtraFormulationAndRoute(),
                                              getExtraFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              6 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(4)),
                                                       Duration(std::chrono::hours(9), std::chrono::minutes(0), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(400.0),
                                              Duration(std::chrono::hours(6)),
                                              getExtraFormulationAndRoute(),
                                              getExtraFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              6 * NB_POINTS_PER_HOUR + 1));

        // Intakes for firstDailyDose and secondDailyDose
        // First repeat
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                                                       Duration(std::chrono::hours(12), std::chrono::minutes(0), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(100.0),
                                              Duration(std::chrono::hours(24)),
                                              getInfusionFormulationAndRoute(),
                                              getInfusionFormulationAndRoute().getAbsorptionModel(),
                                              Duration(std::chrono::minutes(20)),
                                              24 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                                                       Duration(std::chrono::hours(22), std::chrono::minutes(0), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(200.0),
                                              Duration(std::chrono::hours(24)),
                                              getInfusionFormulationAndRoute(),
                                              getInfusionFormulationAndRoute().getAbsorptionModel(),
                                              Duration(std::chrono::minutes(10)),
                                              24 * NB_POINTS_PER_HOUR + 1));
        // Second repeat
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(3)),
                                                       Duration(std::chrono::hours(12), std::chrono::minutes(0), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(100.0),
                                              Duration(std::chrono::hours(24)),
                                              getInfusionFormulationAndRoute(),
                                              getInfusionFormulationAndRoute().getAbsorptionModel(),
                                              Duration(std::chrono::minutes(20)),
                                              24 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(3)),
                                                       Duration(std::chrono::hours(22), std::chrono::minutes(0), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(200.0),
                                              Duration(std::chrono::hours(24)),
                                              getInfusionFormulationAndRoute(),
                                              getInfusionFormulationAndRoute().getAbsorptionModel(),
                                              Duration(std::chrono::minutes(10)),
                                              24 * NB_POINTS_PER_HOUR + 1));

        std::sort(expectedIntakes.begin(), expectedIntakes.end());

        DateTime fullPeriodStart(date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                                 Duration(std::chrono::hours(0), std::chrono::minutes(0), std::chrono::seconds(0)));
        IntakeSeries iSeries;
        IntakeExtractor extractor;
        IntakeExtractor::Result result = extractor.extract(dh, fullPeriodStart, DateTime(), NB_POINTS_PER_HOUR, iSeries);
        fructose_assert(result == IntakeExtractor::Result::Ok);

        fructose_assert(iSeries.size() == expectedIntakes.size());

        for (size_t i = 0; i < expectedIntakes.size(); ++i) {
            fructose_assert(iSeries.at(i) == expectedIntakes.at(i));
        }
    }


    /// \brief Complex scenario to test ParallelSequence capabilities with changes to the planned schedule.
    /// The treatment should be given:
    /// - twice a day (at 12h00 and 22h00)
    /// - 10x every 3 hours, with an initial offset of 2h30
    /// - 4x every 6 hours, with an initial offset of 6h30
    /// - the whole processus should be repeated twice.
    /// However, the intake at 2h30 of the 03.06.2017 (300mg INTRAVASCULAR) has been replaced by an EXTRAVASCULAR
    /// intake of 350mg at 2h49 (the same day). Also, the INFUSION scheduled for 12h00 the 03.06 is cancelled.
    /// \param _testName Test name.
    void testComplexParallelSequence2(const std::string& /* _testName */)
    {
        // List of time ranges that will be pushed into the history
        DosageTimeRangeList timeRangeList;

        // Create a time range for the first week of June 2017, no end to test DosageRepeat
        DateTime startJune2017(date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                               Duration(std::chrono::hours(0), std::chrono::minutes(0), std::chrono::seconds(0)));

        // Add the treatments
        DailyDose firstDailyDose(DoseValue(100.0),
                                 getInfusionFormulationAndRoute(),
                                 Duration(std::chrono::minutes(20)),
                                 TimeOfDay(Duration(std::chrono::hours(12), std::chrono::minutes(0), std::chrono::seconds(0))));
        DailyDose secondDailyDose(DoseValue(200.0),
                                  getInfusionFormulationAndRoute(),
                                  Duration(std::chrono::minutes(10)),
                                  TimeOfDay(Duration(std::chrono::hours(22), std::chrono::minutes(0), std::chrono::seconds(0))));

        LastingDose doseEvery3Hours(DoseValue(300.0),
                                    getBolusFormulationAndRoute(),
                                    Duration(),
                                    Duration(std::chrono::hours(3)));

        LastingDose doseEvery6Hours(DoseValue(400.0),
                                    getExtraFormulationAndRoute(),
                                    Duration(),
                                    Duration(std::chrono::hours(6)));

        ParallelDosageSequence ds(firstDailyDose, Duration());
        ds.addDosage(secondDailyDose, Duration());
        DosageRepeat repeatEvery3Hours(doseEvery3Hours, 10);
        ds.addDosage(repeatEvery3Hours, Duration(std::chrono::hours(2), std::chrono::minutes(30), std::chrono::seconds(0)));
        DosageRepeat repeatEvery6Hours(doseEvery6Hours, 4);
        ds.addDosage(repeatEvery6Hours, Duration(std::chrono::hours(6), std::chrono::minutes(30), std::chrono::seconds(0)));
        DosageRepeat juneDoses(ds, 2);
        std::unique_ptr<Tucuxi::Core::DosageTimeRange> june2017(new Tucuxi::Core::DosageTimeRange(startJune2017, juneDoses));

        Duration expectedTimeStepDS(std::chrono::hours(32), std::chrono::minutes(30), std::chrono::seconds(0));
        fructose_assert(ds.getTimeStep() == expectedTimeStepDS);
        fructose_assert(juneDoses.getTimeStep() == expectedTimeStepDS + expectedTimeStepDS);

        // Add changes to the scheduled treatments
        june2017->addIntakeChange(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(3)),
                                                       Duration(std::chrono::hours(2), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(300.0),
                                              Duration(std::chrono::hours(3)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              3 * NB_POINTS_PER_HOUR + 1), ScheduledIntakeOp::Skip);
        june2017->addIntakeChange(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(3)),
                                                       Duration(std::chrono::hours(2), std::chrono::minutes(49), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(350.0),
                                              Duration(std::chrono::hours(3)),
                                              getExtraFormulationAndRoute(),
                                              getExtraFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              3 * NB_POINTS_PER_HOUR + 1), ScheduledIntakeOp::Add);
        june2017->addIntakeChange(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(3)),
                                                       Duration(std::chrono::hours(12), std::chrono::minutes(0), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(100.0),
                                              Duration(std::chrono::hours(24)),
                                              getInfusionFormulationAndRoute(),
                                              getInfusionFormulationAndRoute().getAbsorptionModel(),
                                              Duration(std::chrono::minutes(20)),
                                              24 * NB_POINTS_PER_HOUR + 1), ScheduledIntakeOp::Skip);

        // Create the dosage history
        DosageHistory dh;
        dh.addTimeRange(*june2017);

        // Expected intake series
        IntakeSeries expectedIntakes;
        // Intakes for doseEvery3Hours
        // First repeat
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                                                       Duration(std::chrono::hours(2), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(300.0),
                                              Duration(std::chrono::hours(3)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              3 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                                                       Duration(std::chrono::hours(5), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(300.0),
                                              Duration(std::chrono::hours(3)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              3 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                                                       Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(300.0),
                                              Duration(std::chrono::hours(3)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              3 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                                                       Duration(std::chrono::hours(11), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(300.0),
                                              Duration(std::chrono::hours(3)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              3 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                                                       Duration(std::chrono::hours(14), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(300.0),
                                              Duration(std::chrono::hours(3)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              3 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                                                       Duration(std::chrono::hours(17), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(300.0),
                                              Duration(std::chrono::hours(3)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              3 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                                                       Duration(std::chrono::hours(20), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(300.0),
                                              Duration(std::chrono::hours(3)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              3 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                                                       Duration(std::chrono::hours(23), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(300.0),
                                              Duration(std::chrono::hours(3)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              3 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(3)),
                                                       Duration(std::chrono::hours(2), std::chrono::minutes(49), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(350.0),
                                              Duration(std::chrono::hours(3)),
                                              getExtraFormulationAndRoute(),
                                              getExtraFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              3 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(3)),
                                                       Duration(std::chrono::hours(5), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(300.0),
                                              Duration(std::chrono::hours(3)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              3 * NB_POINTS_PER_HOUR + 1));
        // Second repeat
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(3)),
                                                       Duration(std::chrono::hours(11), std::chrono::minutes(0), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(300.0),
                                              Duration(std::chrono::hours(3)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              3 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(3)),
                                                       Duration(std::chrono::hours(14), std::chrono::minutes(0), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(300.0),
                                              Duration(std::chrono::hours(3)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              3 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(3)),
                                                       Duration(std::chrono::hours(17), std::chrono::minutes(0), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(300.0),
                                              Duration(std::chrono::hours(3)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              3 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(3)),
                                                       Duration(std::chrono::hours(20), std::chrono::minutes(0), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(300.0),
                                              Duration(std::chrono::hours(3)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              3 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(3)),
                                                       Duration(std::chrono::hours(23), std::chrono::minutes(0), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(300.0),
                                              Duration(std::chrono::hours(3)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              3 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(4)),
                                                       Duration(std::chrono::hours(2), std::chrono::minutes(0), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(300.0),
                                              Duration(std::chrono::hours(3)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              3 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(4)),
                                                       Duration(std::chrono::hours(5), std::chrono::minutes(0), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(300.0),
                                              Duration(std::chrono::hours(3)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              3 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(4)),
                                                       Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(300.0),
                                              Duration(std::chrono::hours(3)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              3 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(4)),
                                                       Duration(std::chrono::hours(11), std::chrono::minutes(0), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(300.0),
                                              Duration(std::chrono::hours(3)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              3 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(4)),
                                                       Duration(std::chrono::hours(14), std::chrono::minutes(0), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(300.0),
                                              Duration(std::chrono::hours(3)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              3 * NB_POINTS_PER_HOUR + 1));

        // Intakes for doseEvery6Hours
        // First repeat
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                                                       Duration(std::chrono::hours(6), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(400.0),
                                              Duration(std::chrono::hours(6)),
                                              getExtraFormulationAndRoute(),
                                              getExtraFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              6 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                                                       Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(400.0),
                                              Duration(std::chrono::hours(6)),
                                              getExtraFormulationAndRoute(),
                                              getExtraFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              6 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                                                       Duration(std::chrono::hours(18), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(400.0),
                                              Duration(std::chrono::hours(6)),
                                              getExtraFormulationAndRoute(),
                                              getExtraFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              6 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(3)),
                                                       Duration(std::chrono::hours(0), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(400.0),
                                              Duration(std::chrono::hours(6)),
                                              getExtraFormulationAndRoute(),
                                              getExtraFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              6 * NB_POINTS_PER_HOUR + 1));
        // Second repeat
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(3)),
                                                       Duration(std::chrono::hours(15), std::chrono::minutes(0), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(400.0),
                                              Duration(std::chrono::hours(6)),
                                              getExtraFormulationAndRoute(),
                                              getExtraFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              6 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(3)),
                                                       Duration(std::chrono::hours(21), std::chrono::minutes(0), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(400.0),
                                              Duration(std::chrono::hours(6)),
                                              getExtraFormulationAndRoute(),
                                              getExtraFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              6 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(4)),
                                                       Duration(std::chrono::hours(3), std::chrono::minutes(0), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(400.0),
                                              Duration(std::chrono::hours(6)),
                                              getExtraFormulationAndRoute(),
                                              getExtraFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              6 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(4)),
                                                       Duration(std::chrono::hours(9), std::chrono::minutes(0), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(400.0),
                                              Duration(std::chrono::hours(6)),
                                              getExtraFormulationAndRoute(),
                                              getExtraFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              6 * NB_POINTS_PER_HOUR + 1));

        // Intakes for firstDailyDose and secondDailyDose
        // First repeat
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                                                       Duration(std::chrono::hours(12), std::chrono::minutes(0), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(100.0),
                                              Duration(std::chrono::hours(24)),
                                              getInfusionFormulationAndRoute(),
                                              getInfusionFormulationAndRoute().getAbsorptionModel(),
                                              Duration(std::chrono::minutes(20)),
                                              24 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                                                       Duration(std::chrono::hours(22), std::chrono::minutes(0), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(200.0),
                                              Duration(std::chrono::hours(24)),
                                              getInfusionFormulationAndRoute(),
                                              getInfusionFormulationAndRoute().getAbsorptionModel(),
                                              Duration(std::chrono::minutes(10)),
                                              24 * NB_POINTS_PER_HOUR + 1));
        // Second repeat
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(3)),
                                                       Duration(std::chrono::hours(22), std::chrono::minutes(0), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(200.0),
                                              Duration(std::chrono::hours(24)),
                                              getInfusionFormulationAndRoute(),
                                              getInfusionFormulationAndRoute().getAbsorptionModel(),
                                              Duration(std::chrono::minutes(10)),
                                              24 * NB_POINTS_PER_HOUR + 1));

        std::sort(expectedIntakes.begin(), expectedIntakes.end());

        DateTime fullPeriodStart(date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                                 Duration(std::chrono::hours(0), std::chrono::minutes(0), std::chrono::seconds(0)));
        IntakeSeries iSeries;
        IntakeExtractor extractor;
        IntakeExtractor::Result result = extractor.extract(dh, fullPeriodStart, DateTime(), NB_POINTS_PER_HOUR, iSeries);
        fructose_assert(result == IntakeExtractor::Result::Ok);

        fructose_assert(iSeries.size() == expectedIntakes.size());

        for (size_t i = 0; i < expectedIntakes.size(); ++i) {
            if (iSeries.at(i) == expectedIntakes.at(i)) {
            }
            else {
                int i = 0;
                i++;
            }
            fructose_assert(iSeries.at(i) == expectedIntakes.at(i));
        }
    }


    /// \brief Scenario where a treatment is given every day but Monday. Validate on the first two full weeks of June
    /// 2017.
    /// \param _testName Test name.
    void testFullWeekExceptMonday(const std::string& /* _testName */)
    {
        // List of time ranges that will be pushed into the history
        DosageTimeRangeList timeRangeList;

        // Create a time range starting the first full week of July 2017
        DateTime startJuly2017(date::year_month_day(date::year(2017), date::month(7), date::day(2)),
                               Duration(std::chrono::hours(0), std::chrono::minutes(0), std::chrono::seconds(0)));

        // Add a treatment intake every day in the period, except on mondays
        WeeklyDose sundayDose(DoseValue(100.0),
                              getBolusFormulationAndRoute(),
                              Duration(),
                              TimeOfDay(Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
                              DayOfWeek(SUNDAY));
        WeeklyDose tuesdayDose(DoseValue(200.0),
                               getBolusFormulationAndRoute(),
                               Duration(),
                               TimeOfDay(Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
                               DayOfWeek(TUESDAY));
        WeeklyDose wednesdayDose(DoseValue(300.0),
                                 getBolusFormulationAndRoute(),
                                 Duration(),
                                 TimeOfDay(Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
                                 DayOfWeek(WEDNESDAY));
        WeeklyDose thursdayDose(DoseValue(400.0),
                                getBolusFormulationAndRoute(),
                                Duration(),
                                TimeOfDay(Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
                                DayOfWeek(THURSDAY));
        WeeklyDose fridayDose(DoseValue(500.0),
                              getBolusFormulationAndRoute(),
                              Duration(),
                              TimeOfDay(Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
                              DayOfWeek(FRIDAY));
        WeeklyDose saturdayDose(DoseValue(600.0),
                                getBolusFormulationAndRoute(),
                                Duration(),
                                TimeOfDay(Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
                                DayOfWeek(SATURDAY));
        ParallelDosageSequence ds(sundayDose, Duration());
        ds.addDosage(tuesdayDose, Duration());
        ds.addDosage(wednesdayDose, Duration());
        ds.addDosage(thursdayDose, Duration());
        ds.addDosage(fridayDose, Duration());
        ds.addDosage(saturdayDose, Duration());
        DosageLoop treatment(ds);
        std::unique_ptr<Tucuxi::Core::DosageTimeRange> trFromJuly2017(new Tucuxi::Core::DosageTimeRange(startJuly2017, treatment));

        // Create the dosage history
        DosageHistory dh;
        dh.addTimeRange(*trFromJuly2017);

        // Expected intake series
        IntakeSeries expectedIntakes;
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(7), date::day(2)),
                                                       Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(100.0),
                                              Duration(std::chrono::hours(7 * 24)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              7 * 24 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(7), date::day(4)),
                                                       Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(200.0),
                                              Duration(std::chrono::hours(7 * 24)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              7 * 24 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(7), date::day(5)),
                                                       Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(300.0),
                                              Duration(std::chrono::hours(7 * 24)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              7 * 24 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(7), date::day(6)),
                                                       Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(400.0),
                                              Duration(std::chrono::hours(7 * 24)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              7 * 24 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(7), date::day(7)),
                                                       Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(500.0),
                                              Duration(std::chrono::hours(7 * 24)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              7 * 24 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(7), date::day(8)),
                                                       Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(600.0),
                                              Duration(std::chrono::hours(7 * 24)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              7 * 24 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(7), date::day(9)),
                                                       Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(100.0),
                                              Duration(std::chrono::hours(7 * 24)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              7 * 24 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(7), date::day(11)),
                                                       Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(200.0),
                                              Duration(std::chrono::hours(7 * 24)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              7 * 24 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(7), date::day(12)),
                                                       Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(300.0),
                                              Duration(std::chrono::hours(7 * 24)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              7 * 24 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(7), date::day(13)),
                                                       Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(400.0),
                                              Duration(std::chrono::hours(7 * 24)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              7 * 24 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(7), date::day(14)),
                                                       Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(500.0),
                                              Duration(std::chrono::hours(7 * 24)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              7 * 24 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(7), date::day(15)),
                                                       Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(600.0),
                                              Duration(std::chrono::hours(7 * 24)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              7 * 24 * NB_POINTS_PER_HOUR + 1));


        DateTime fullPeriodStart(date::year_month_day(date::year(2017), date::month(7), date::day(2)),
                                 Duration(std::chrono::hours(0), std::chrono::minutes(0), std::chrono::seconds(0)));
        DateTime fullPeriodEnd(date::year_month_day(date::year(2017), date::month(7), date::day(16)),
                               Duration(std::chrono::hours(0), std::chrono::minutes(0), std::chrono::seconds(0)));
        IntakeSeries iSeries;
        IntakeExtractor extractor;
        IntakeExtractor::Result result = extractor.extract(dh, fullPeriodStart, fullPeriodEnd, NB_POINTS_PER_HOUR, iSeries);
        fructose_assert(result == IntakeExtractor::Result::Ok);

        fructose_assert(iSeries.size() == expectedIntakes.size());

        for (size_t i = 0; i < iSeries.size(); ++i) {
            fructose_assert(iSeries.at(i) == expectedIntakes.at(i));
        }
    }


    /// \brief Scenario where a treatment is given once every 36 hours.
    /// \param _testName Test name.
    void testOnceEvery36HoursAtSteadyState(const std::string& /* _testName */)
    {
        // List of time ranges that will be pushed into the history
        DosageTimeRangeList timeRangeList;

        // Create a time range for the first week of June 2017
        DateTime emptyStart;
        DateTime emptyEnd;

        // Add a treatment intake every ten days in June
        // 200mg via a intravascular at 08h30, starting the 01.06
        LastingDose junePeriodicDose(DoseValue(200.0),
                                     getBolusFormulationAndRoute(),
                                     Duration(),
                                     Duration(std::chrono::hours(36)));

        DateTime lastDose(date::year_month_day(date::year(2017), date::month(6), date::day(4)),
                          Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0)));
        DosageSteadyState juneDose(junePeriodicDose, lastDose);
        std::unique_ptr<Tucuxi::Core::DosageTimeRange> june2017(new Tucuxi::Core::DosageTimeRange(emptyStart, emptyEnd, juneDose));

        // Create the dosage history
        DosageHistory dh;
        dh.addTimeRange(*june2017);

        // Expected intake series
        IntakeSeries expectedIntakes;
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(1)),
                                                       Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(200.0),
                                              Duration(std::chrono::hours(36)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              36 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                                                       Duration(std::chrono::hours(20), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(200.0),
                                              Duration(std::chrono::hours(36)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              36 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(4)),
                                                       Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(200.0),
                                              Duration(std::chrono::hours(36)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              36 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(5)),
                                                       Duration(std::chrono::hours(20), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(200.0),
                                              Duration(std::chrono::hours(36)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              36 * NB_POINTS_PER_HOUR + 1));
        expectedIntakes.push_back(IntakeEvent(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(7)),
                                                       Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
                                              Duration(),
                                              DoseValue(200.0),
                                              Duration(std::chrono::hours(36)),
                                              getBolusFormulationAndRoute(),
                                              getBolusFormulationAndRoute().getAbsorptionModel(),
                                              Duration(),
                                              36 * NB_POINTS_PER_HOUR + 1));

        // The 16.07.2017 must be EXCLUDED (the intervals are closed on the left, but opened on the right side!)
        {
            DateTime fullPeriodStart(date::year_month_day(date::year(2017), date::month(6), date::day(1)),
                                     Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0)));
            DateTime fullPeriodEnd(date::year_month_day(date::year(2017), date::month(6), date::day(8)),
                                   Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0)));
            IntakeSeries iSeries;
            IntakeExtractor extractor;
            IntakeExtractor::Result result = extractor.extract(dh, fullPeriodStart, fullPeriodEnd, NB_POINTS_PER_HOUR, iSeries);
            fructose_assert(result == IntakeExtractor::Result::Ok);

            fructose_assert(iSeries.size() == expectedIntakes.size());

            for (size_t i = 0; i < iSeries.size(); ++i) {
                fructose_assert(iSeries.at(i) == expectedIntakes.at(i));
            }
        }
        {
            // Same test, but the start is 30 minutes before. The output should be the same
            DateTime fullPeriodStart(date::year_month_day(date::year(2017), date::month(6), date::day(1)),
                                     Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));
            DateTime fullPeriodEnd(date::year_month_day(date::year(2017), date::month(6), date::day(8)),
                                   Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0)));
            IntakeSeries iSeries;
            IntakeExtractor extractor;
            IntakeExtractor::Result result = extractor.extract(dh, fullPeriodStart, fullPeriodEnd, NB_POINTS_PER_HOUR, iSeries);
            fructose_assert(result == IntakeExtractor::Result::Ok);

            fructose_assert(iSeries.size() == expectedIntakes.size());

            for (size_t i = 0; i < iSeries.size(); ++i) {
                fructose_assert(iSeries.at(i) == expectedIntakes.at(i));
            }
        }
    }
};

#endif // TEST_INTAKEEXTRACTOR_H
