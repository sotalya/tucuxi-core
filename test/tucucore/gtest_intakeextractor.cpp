//@@license@@

#include <memory>

#include <gtest/gtest.h>

#include "tucucore/dosage.h"
#include "tucucore/drugmodel/formulationandroute.h"
#include "tucucore/intakeextractor.h"

#include "gtest_core.h"


using namespace Tucuxi::Core;
using namespace Tucuxi::Common;

static constexpr double NB_POINTS_PER_HOUR = CYCLE_SIZE / 24.0;

/// \brief Scenario where a treatment is given once per week.
TEST(Core_TestIntakeExtractor, OncePerWeek)
{
    // List of time ranges that will be pushed into the history
    DosageTimeRangeList timeRangeList;

    // Create a time range for the month of June 2017 and one for the first three weeks of July 2017 (counting the
    // first Saturday as a week)
    DateTime startJune2017(
            date::year_month_day(date::year(2017), date::month(6), date::day(1)), std::chrono::seconds(0));
    DateTime endJune2017(date::year_month_day(date::year(2017), date::month(7), date::day(1)), std::chrono::seconds(0));
    DateTime startJuly2017(
            date::year_month_day(date::year(2017), date::month(7), date::day(1)), std::chrono::seconds(0));
    DateTime endJuly2017(
            date::year_month_day(date::year(2017), date::month(7), date::day(16)), std::chrono::seconds(0));

    // Add a weekly treatment in June
    // 200mg via a 20-minutes perfusion at 08h30 on Tuesday
    WeeklyDose juneWeeklyDose(
            DoseValue(200.0),
            TucuUnit("mg"),
            getInfusionFormulationAndRoute(),
            Duration(std::chrono::minutes(20)),
            TimeOfDay(Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
            DayOfWeek(TUESDAY));
    DosageLoop juneDose(juneWeeklyDose);
    auto june2017 = std::make_unique<Tucuxi::Core::DosageTimeRange>(startJune2017, endJune2017, juneDose);

    // Add a weekly treatment in July
    // 400mg via a 10-minutes perfusion at 11h30 on Wednesday
    WeeklyDose julyWeeklyDose(
            DoseValue(400.0),
            TucuUnit("mg"),
            getInfusionFormulationAndRoute(),
            Duration(std::chrono::minutes(10)),
            TimeOfDay(Duration(std::chrono::hours(11), std::chrono::minutes(30), std::chrono::seconds(0))),
            DayOfWeek(WEDNESDAY));
    DosageLoop julyDose(julyWeeklyDose);
    auto july2017 = std::make_unique<Tucuxi::Core::DosageTimeRange>(startJuly2017, endJuly2017, julyDose);
    assert(!timeRangesOverlap(*june2017, *july2017));

    // Create the dosage history
    std::unique_ptr<DosageHistory> dh = std::make_unique<DosageHistory>();
    dh->addTimeRange(*june2017);
    dh->addTimeRange(*july2017);

    // Expected intake series
    IntakeSeries expectedIntakes;
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(6)),
                    Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(200.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(7 * 24)),
            getInfusionFormulationAndRoute(),
            getInfusionFormulationAndRoute().getAbsorptionModel(),
            Duration(std::chrono::minutes(20)),
            static_cast<int>(7 * 24 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(13)),
                    Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(200.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(7 * 24)),
            getInfusionFormulationAndRoute(),
            getInfusionFormulationAndRoute().getAbsorptionModel(),
            Duration(std::chrono::minutes(20)),
            static_cast<int>(7 * 24 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(20)),
                    Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(200.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(7 * 24)),
            getInfusionFormulationAndRoute(),
            getInfusionFormulationAndRoute().getAbsorptionModel(),
            Duration(std::chrono::minutes(20)),
            static_cast<int>(7 * 24 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(27)),
                    Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(200.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(7 * 24)),
            getInfusionFormulationAndRoute(),
            getInfusionFormulationAndRoute().getAbsorptionModel(),
            Duration(std::chrono::minutes(20)),
            static_cast<int>(7 * 24 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(7), date::day(5)),
                    Duration(std::chrono::hours(11), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(400.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(7 * 24)),
            getInfusionFormulationAndRoute(),
            getInfusionFormulationAndRoute().getAbsorptionModel(),
            Duration(std::chrono::minutes(10)),
            static_cast<int>(7 * 24 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(7), date::day(12)),
                    Duration(std::chrono::hours(11), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(400.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(7 * 24)),
            getInfusionFormulationAndRoute(),
            getInfusionFormulationAndRoute().getAbsorptionModel(),
            Duration(std::chrono::minutes(10)),
            static_cast<int>(7 * 24 * NB_POINTS_PER_HOUR + 1)));

    // Extract the intake series
    DateTime fullPeriodStart(
            date::year_month_day(date::year(2017), date::month(6), date::day(1)), std::chrono::seconds(0));
    DateTime fullPeriodEnd(
            date::year_month_day(date::year(2017), date::month(7), date::day(16)), std::chrono::seconds(0));
    IntakeSeries iSeries;
    IntakeExtractor extractor;
    ComputingStatus result = extractor.extract(
            *dh,
            fullPeriodStart,
            fullPeriodEnd,
            NB_POINTS_PER_HOUR,
            TucuUnit("mg"),
            iSeries,
            ExtractionOption::ForceCycle);
    ASSERT_EQ(result, ComputingStatus::Ok);

    ASSERT_EQ(iSeries.size(), expectedIntakes.size());

    for (size_t i = 0; i < iSeries.size(); ++i) {
        ASSERT_EQ(iSeries[i], expectedIntakes[i]);
    }

    {
        // Try with other final unit
        IntakeSeries iSeriesug;
        ComputingStatus result = extractor.extract(
                *dh,
                fullPeriodStart,
                fullPeriodEnd,
                NB_POINTS_PER_HOUR,
                TucuUnit("ug"),
                iSeriesug,
                ExtractionOption::ForceCycle);
        ASSERT_EQ(result, ComputingStatus::Ok);

        ASSERT_EQ(iSeriesug.size(), expectedIntakes.size());

        for (size_t i = 0; i < iSeriesug.size(); ++i) {
            ASSERT_DOUBLE_EQ(iSeries[i].getDose() * 1000.0, iSeriesug[i].getDose());
        }
    }

    {
        // Try with inverted end/start dates
        IntakeExtractor extractor;
        ComputingStatus result =
                extractor.extract(*dh, fullPeriodEnd, fullPeriodStart, NB_POINTS_PER_HOUR, TucuUnit("mg"), iSeries);
        ASSERT_EQ(result, ComputingStatus::IntakeExtractionError);
    }
}

/// \brief Scenario where a treatment is given once every ten days in the first time range, once every 4 days in
/// the second.
TEST(Core_TestIntakeExtractor, OnceEveryTenDays)
{
    // List of time ranges that will be pushed into the history
    DosageTimeRangeList timeRangeList;

    // Create a time range for the month of June 2017 and one for the first three weeks of July 2017 (counting the
    // first Saturday as a week)
    DateTime startJune2017(
            date::year_month_day(date::year(2017), date::month(6), date::day(1)),
            Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0)));
    DateTime endJune2017(
            date::year_month_day(date::year(2017), date::month(7), date::day(1)),
            Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0)));
    // Lasting doses start at the beginning of their time range, we have thus to start on the good day
    DateTime startJuly2017(
            date::year_month_day(date::year(2017), date::month(7), date::day(4)),
            Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0)));
    DateTime endJuly2017(
            date::year_month_day(date::year(2017), date::month(7), date::day(16)),
            Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0)));

    // Add a treatment intake every ten days in June
    // 200mg via a 20-minutes perfusion at 08h30, starting the 01.06
    LastingDose junePeriodicDose(
            DoseValue(200.0),
            TucuUnit("mg"),
            getInfusionFormulationAndRoute(),
            Duration(std::chrono::minutes(20)),
            Duration(std::chrono::hours(10 * 24)));
    DosageLoop juneDose(junePeriodicDose);
    auto june2017 = std::make_unique<Tucuxi::Core::DosageTimeRange>(startJune2017, endJune2017, juneDose);

    // Add a treatment intake every four days in June
    // 400mg via a 10-minutes perfusion at 11h30, starting the 01.07
    LastingDose julyPeriodicDose(
            DoseValue(400.0),
            TucuUnit("mg"),
            getInfusionFormulationAndRoute(),
            Duration(std::chrono::minutes(10)),
            Duration(std::chrono::hours(4 * 24)));
    DosageLoop julyDose(julyPeriodicDose);
    auto july2017 = std::make_unique<Tucuxi::Core::DosageTimeRange>(startJuly2017, endJuly2017, julyDose);
    assert(!timeRangesOverlap(*june2017, *july2017));

    // Create the dosage history
    DosageHistory dh;
    dh.addTimeRange(*june2017);
    dh.addTimeRange(*july2017);

    // Expected intake series
    IntakeSeries expectedIntakes;
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(1)),
                    Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(200.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(10 * 24)),
            getInfusionFormulationAndRoute(),
            getInfusionFormulationAndRoute().getAbsorptionModel(),
            Duration(std::chrono::minutes(20)),
            static_cast<int>(10 * 24 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(11)),
                    Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(200.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(10 * 24)),
            getInfusionFormulationAndRoute(),
            getInfusionFormulationAndRoute().getAbsorptionModel(),
            Duration(std::chrono::minutes(20)),
            static_cast<int>(10 * 24 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(21)),
                    Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(200.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(10 * 24)),
            getInfusionFormulationAndRoute(),
            getInfusionFormulationAndRoute().getAbsorptionModel(),
            Duration(std::chrono::minutes(20)),
            static_cast<int>(10 * 24 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(7), date::day(4)),
                    Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(400.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(4 * 24)),
            getInfusionFormulationAndRoute(),
            getInfusionFormulationAndRoute().getAbsorptionModel(),
            Duration(std::chrono::minutes(10)),
            static_cast<int>(4 * 24 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(7), date::day(8)),
                    Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(400.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(4 * 24)),
            getInfusionFormulationAndRoute(),
            getInfusionFormulationAndRoute().getAbsorptionModel(),
            Duration(std::chrono::minutes(10)),
            static_cast<int>(4 * 24 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(7), date::day(12)),
                    Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(400.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(4 * 24)),
            getInfusionFormulationAndRoute(),
            getInfusionFormulationAndRoute().getAbsorptionModel(),
            Duration(std::chrono::minutes(10)),
            static_cast<int>(4 * 24 * NB_POINTS_PER_HOUR + 1)));
    // The 16.07.2017 must be EXCLUDED (the intervals are closed on the left, but opened on the right side!)

    DateTime fullPeriodStart(
            date::year_month_day(date::year(2017), date::month(6), date::day(1)),
            Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0)));
    DateTime fullPeriodEnd(
            date::year_month_day(date::year(2017), date::month(7), date::day(16)),
            Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0)));
    IntakeSeries iSeries;
    IntakeExtractor extractor;
    ComputingStatus result =
            extractor.extract(dh, fullPeriodStart, fullPeriodEnd, NB_POINTS_PER_HOUR, TucuUnit("mg"), iSeries);
    ASSERT_EQ(result, ComputingStatus::Ok);

    ASSERT_EQ(iSeries.size(), expectedIntakes.size());

    for (size_t i = 0; i < iSeries.size(); ++i) {
        ASSERT_EQ(iSeries[i], expectedIntakes[i]);
    }
}

/// \brief Scenario where a treatment is given once every 36 hours.
TEST(Core_TestIntakeExtractor, OnceEvery36Hours)
{
    // List of time ranges that will be pushed into the history
    DosageTimeRangeList timeRangeList;

    // Create a time range for the first week of June 2017
    DateTime startJune2017(
            date::year_month_day(date::year(2017), date::month(6), date::day(1)),
            Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0)));
    DateTime endJune2017(
            date::year_month_day(date::year(2017), date::month(7), date::day(8)),
            Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0)));

    // Add a treatment intake every ten days in June
    // 200mg via a intravascular at 08h30, starting the 01.06
    LastingDose junePeriodicDose(
            DoseValue(200.0),
            TucuUnit("mg"),
            getBolusFormulationAndRoute(),
            Duration(),
            Duration(std::chrono::hours(36)));
    DosageLoop juneDose(junePeriodicDose);
    auto june2017 = std::make_unique<Tucuxi::Core::DosageTimeRange>(startJune2017, endJune2017, juneDose);

    // Create the dosage history
    DosageHistory dh;
    dh.addTimeRange(*june2017);

    // Expected intake series
    IntakeSeries expectedIntakes;
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(1)),
                    Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(200.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(36)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(36 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                    Duration(std::chrono::hours(20), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(200.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(36)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(36 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(4)),
                    Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(200.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(36)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(36 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(5)),
                    Duration(std::chrono::hours(20), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(200.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(36)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(36 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(7)),
                    Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(200.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(24)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(24 * NB_POINTS_PER_HOUR + 1)));

    // The 16.07.2017 must be EXCLUDED (the intervals are closed on the left, but opened on the right side!)

    DateTime fullPeriodStart(
            date::year_month_day(date::year(2017), date::month(6), date::day(1)),
            Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0)));
    DateTime fullPeriodEnd(
            date::year_month_day(date::year(2017), date::month(6), date::day(8)),
            Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0)));
    IntakeSeries iSeries;
    IntakeExtractor extractor;
    ComputingStatus result =
            extractor.extract(dh, fullPeriodStart, fullPeriodEnd, NB_POINTS_PER_HOUR, TucuUnit("mg"), iSeries);
    ASSERT_EQ(result, ComputingStatus::Ok);

    ASSERT_EQ(iSeries.size(), expectedIntakes.size());

    for (size_t i = 0; i < iSeries.size(); ++i) {
        ASSERT_EQ(iSeries[i], expectedIntakes[i]);
    }
}

/// \brief Scenario where a treatment is given 5 times at a 12 hours interval.
TEST(Core_TestIntakeExtractor, FiveTimesEvery12Hours)
{
    // List of time ranges that will be pushed into the history
    DosageTimeRangeList timeRangeList;

    // Create a time range starting at the beginning of june 2017, with no upper end (to test that the repetitions
    // are handled correctly)
    DateTime startJune2017(
            date::year_month_day(date::year(2017), date::month(6), date::day(1)),
            Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0)));

    // Add a treatment intake every ten days in June
    // 200mg via a intravascular at 08h30, starting the 01.06
    LastingDose periodicDose(
            DoseValue(200.0),
            TucuUnit("mg"),
            getBolusFormulationAndRoute(),
            Duration(),
            Duration(std::chrono::hours(12)));
    DosageRepeat repeatedDose(periodicDose, 5);
    auto june2017 = std::make_unique<Tucuxi::Core::DosageTimeRange>(startJune2017, repeatedDose);

    // Create the dosage history
    DosageHistory dh;
    dh.addTimeRange(*june2017);

    // Expected intake series
    IntakeSeries expectedIntakes;
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(1)),
                    Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(200.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(12)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(12 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(1)),
                    Duration(std::chrono::hours(20), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(200.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(12)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(12 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                    Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(200.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(12)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(12 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                    Duration(std::chrono::hours(20), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(200.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(12)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(12 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(3)),
                    Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(200.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(12)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(12 * NB_POINTS_PER_HOUR + 1)));

    DateTime fullPeriodStart(
            date::year_month_day(date::year(2017), date::month(6), date::day(1)),
            Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0)));
    IntakeSeries iSeries;
    IntakeExtractor extractor;
    ComputingStatus result =
            extractor.extract(dh, fullPeriodStart, DateTime::now(), NB_POINTS_PER_HOUR, TucuUnit("mg"), iSeries);
    ASSERT_EQ(result, ComputingStatus::Ok);

    ASSERT_EQ(iSeries.size(), expectedIntakes.size());

    for (size_t i = 0; i < iSeries.size(); ++i) {
        ASSERT_EQ(iSeries[i], expectedIntakes[i]);
    }
}

/// \brief Scenario where a treatment is given 5 times at a 12 hours interval, and we stop the extraction after 3.
TEST(Core_TestIntakeExtractor, FiveTimesEvery12HoursEarlyStop)
{
    // List of time ranges that will be pushed into the history
    DosageTimeRangeList timeRangeList;

    // Create a time range starting at the beginning of june 2017, with no upper end (to test that the repetitions
    // are handled correctly)
    DateTime startJune2017(
            date::year_month_day(date::year(2017), date::month(6), date::day(1)),
            Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0)));

    // Add a treatment intake every ten days in June
    // 200mg via a intravascular at 08h30, starting the 01.06
    LastingDose periodicDose(
            DoseValue(200.0),
            TucuUnit("mg"),
            getBolusFormulationAndRoute(),
            Duration(),
            Duration(std::chrono::hours(12)));
    DosageRepeat repeatedDose(periodicDose, 5);
    auto june2017 = std::make_unique<Tucuxi::Core::DosageTimeRange>(startJune2017, repeatedDose);

    // Create the dosage history
    DosageHistory dh;
    dh.addTimeRange(*june2017);

    // Expected intake series
    IntakeSeries expectedIntakes;
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(1)),
                    Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(200.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(12)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(12 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(1)),
                    Duration(std::chrono::hours(20), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(200.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(12)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(12 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                    Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(200.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(12)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(12 * NB_POINTS_PER_HOUR + 1)));


    DateTime fullPeriodStart(
            date::year_month_day(date::year(2017), date::month(6), date::day(1)),
            Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0)));
    DateTime fullPeriodEnd(
            date::year_month_day(date::year(2017), date::month(6), date::day(2)),
            Duration(std::chrono::hours(20), std::chrono::minutes(30), std::chrono::seconds(0)));
    IntakeSeries iSeries;
    IntakeExtractor extractor;
    ComputingStatus result =
            extractor.extract(dh, fullPeriodStart, fullPeriodEnd, NB_POINTS_PER_HOUR, TucuUnit("mg"), iSeries);
    ASSERT_EQ(result, ComputingStatus::Ok);

    ASSERT_EQ(iSeries.size(), expectedIntakes.size());

    for (size_t i = 0; i < iSeries.size(); ++i) {
        ASSERT_EQ(iSeries[i], expectedIntakes[i]);
    }
}

/// \brief Scenario where a treatment is given twice in the same day, once with 200mg and the other time with
/// 300mg, with a ten hours delay, but only every ten days, and this 3 times in a row.
TEST(Core_TestIntakeExtractor, TwiceEveryTenDays)
{
    // List of time ranges that will be pushed into the history
    DosageTimeRangeList timeRangeList;

    // Create a time range starting at the beginning of june 2017, with no upper end (to test that the repetitions
    // are handled correctly)
    DateTime startJune2017(
            date::year_month_day(date::year(2017), date::month(6), date::day(1)),
            Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0)));

    // Add a treatment intake every ten days in June
    LastingDose periodicDose200(
            DoseValue(200.0),
            TucuUnit("mg"),
            getBolusFormulationAndRoute(),
            Duration(),
            Duration(std::chrono::hours(10)));
    LastingDose periodicDose300(
            DoseValue(300.0),
            TucuUnit("mg"),
            getBolusFormulationAndRoute(),
            Duration(),
            Duration(std::chrono::hours(10 * 23)));
    DosageSequence ds(periodicDose200);
    ds.addDosage(periodicDose300);
    DosageRepeat repeatedDose(ds, 3);
    auto june2017 = std::make_unique<Tucuxi::Core::DosageTimeRange>(startJune2017, repeatedDose);

    // Create the dosage history
    DosageHistory dh;
    dh.addTimeRange(*june2017);

    // Expected intake series
    IntakeSeries expectedIntakes;
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(1)),
                    Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(200.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(10)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(10 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(1)),
                    Duration(std::chrono::hours(18), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(300.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(10 * 23)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(10 * 23 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(11)),
                    Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(200.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(10)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(10 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(11)),
                    Duration(std::chrono::hours(18), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(300.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(10 * 23)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(10 * 23 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(21)),
                    Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(200.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(10)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(10 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(21)),
                    Duration(std::chrono::hours(18), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(300.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(10 * 23)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(10 * 23 * NB_POINTS_PER_HOUR + 1)));

    DateTime fullPeriodStart(
            date::year_month_day(date::year(2017), date::month(6), date::day(1)),
            Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0)));
    IntakeSeries iSeries;
    IntakeExtractor extractor;
    ComputingStatus result =
            extractor.extract(dh, fullPeriodStart, DateTime::now(), NB_POINTS_PER_HOUR, TucuUnit("mg"), iSeries);
    ASSERT_EQ(result, ComputingStatus::Ok);

    ASSERT_EQ(iSeries.size(), expectedIntakes.size());

    for (size_t i = 0; i < iSeries.size(); ++i) {
        ASSERT_EQ(iSeries[i], expectedIntakes[i]);
    }
}

/// \brief Scenario where a treatment is given once every day for the first full week of june 2017.
TEST(Core_TestIntakeExtractor, OnceEveryDay)
{
    // List of time ranges that will be pushed into the history
    DosageTimeRangeList timeRangeList;

    // Create a time range for the first week of June 2017
    DateTime startJune2017(
            date::year_month_day(date::year(2017), date::month(6), date::day(2)),
            Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0)));
    DateTime endJune2017(
            date::year_month_day(date::year(2017), date::month(7), date::day(9)),
            Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0)));

    // Add a treatment intake every day in the period
    // 200mg via a intravascular at 12h30, starting the 02.06
    DailyDose junePeriodicDose(
            DoseValue(200.0),
            TucuUnit("mg"),
            getBolusFormulationAndRoute(),
            Duration(),
            TimeOfDay(Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))));
    DosageLoop juneDose(junePeriodicDose);
    auto june2017 = std::make_unique<Tucuxi::Core::DosageTimeRange>(startJune2017, endJune2017, juneDose);

    // Create the dosage history
    DosageHistory dh;
    dh.addTimeRange(*june2017);

    // Expected intake series
    IntakeSeries expectedIntakes;
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                    Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(200.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(24)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(24 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(3)),
                    Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(200.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(24)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(24 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(4)),
                    Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(200.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(24)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(24 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(5)),
                    Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(200.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(24)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(24 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(6)),
                    Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(200.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(24)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(24 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(7)),
                    Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(200.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(24)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(24 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(8)),
                    Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(200.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(24)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(24 * NB_POINTS_PER_HOUR + 1)));

    DateTime fullPeriodStart(
            date::year_month_day(date::year(2017), date::month(6), date::day(2)),
            Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0)));
    DateTime fullPeriodEnd(
            date::year_month_day(date::year(2017), date::month(6), date::day(9)),
            Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0)));
    IntakeSeries iSeries;
    IntakeExtractor extractor;
    ComputingStatus result = extractor.extract(
            dh,
            fullPeriodStart,
            fullPeriodEnd,
            NB_POINTS_PER_HOUR,
            TucuUnit("mg"),
            iSeries,
            ExtractionOption::ForceCycle);
    ASSERT_EQ(result, ComputingStatus::Ok);

    ASSERT_EQ(iSeries.size(), expectedIntakes.size());

    for (size_t i = 0; i < iSeries.size(); ++i) {
        ASSERT_EQ(iSeries[i], expectedIntakes[i]);
    }
}

/// \brief Scenario where a treatment is given three times a day, every day of the first full week of June 2017.
TEST(Core_TestIntakeExtractor, ThreeTimesEveryDay)
{
    // List of time ranges that will be pushed into the history
    DosageTimeRangeList timeRangeList;

    // Create a time range for the first week of June 2017
    DateTime startJune2017(
            date::year_month_day(date::year(2017), date::month(6), date::day(2)),
            Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0)));
    DateTime endJune2017(
            date::year_month_day(date::year(2017), date::month(7), date::day(9)),
            Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0)));

    // Add the treatments intake every day in the period
    // 200mg via a intravascular at 08h30, starting the 02.06
    DailyDose dose1(
            DoseValue(200.0),
            TucuUnit("mg"),
            getBolusFormulationAndRoute(),
            Duration(),
            TimeOfDay(Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))));
    // 300mg via a intravascular at 12h30, starting the 02.06
    DailyDose dose2(
            DoseValue(300.0),
            TucuUnit("mg"),
            getBolusFormulationAndRoute(),
            Duration(),
            TimeOfDay(Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))));
    // 400mg via a intravascular at 16h30, starting the 02.06
    DailyDose dose3(
            DoseValue(400.0),
            TucuUnit("mg"),
            getBolusFormulationAndRoute(),
            Duration(),
            TimeOfDay(Duration(std::chrono::hours(16), std::chrono::minutes(30), std::chrono::seconds(0))));
    ParallelDosageSequence ds(dose1, Duration());
    ds.addDosage(dose2, Duration());
    ds.addDosage(dose3, Duration());
    DosageLoop juneDose(ds);
    auto june2017 = std::make_unique<Tucuxi::Core::DosageTimeRange>(startJune2017, endJune2017, juneDose);

    // Create the dosage history
    DosageHistory dh;
    dh.addTimeRange(*june2017);

    // Expected intake series
    IntakeSeries expectedIntakes;
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                    Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(200.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(24)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(24 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                    Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(300.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(24)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(24 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                    Duration(std::chrono::hours(16), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(400.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(24)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(24 * NB_POINTS_PER_HOUR + 1)));


    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(3)),
                    Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(200.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(24)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(24 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(3)),
                    Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(300.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(24)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(24 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(3)),
                    Duration(std::chrono::hours(16), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(400.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(24)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(24 * NB_POINTS_PER_HOUR + 1)));

    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(4)),
                    Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(200.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(24)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(24 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(4)),
                    Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(300.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(24)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(24 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(4)),
                    Duration(std::chrono::hours(16), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(400.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(24)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(24 * NB_POINTS_PER_HOUR + 1)));


    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(5)),
                    Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(200.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(24)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(24 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(5)),
                    Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(300.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(24)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(24 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(5)),
                    Duration(std::chrono::hours(16), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(400.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(24)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(24 * NB_POINTS_PER_HOUR + 1)));

    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(6)),
                    Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(200.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(24)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(24 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(6)),
                    Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(300.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(24)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(24 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(6)),
                    Duration(std::chrono::hours(16), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(400.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(24)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(24 * NB_POINTS_PER_HOUR + 1)));


    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(7)),
                    Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(200.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(24)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(24 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(7)),
                    Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(300.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(24)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(24 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(7)),
                    Duration(std::chrono::hours(16), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(400.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(24)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(24 * NB_POINTS_PER_HOUR + 1)));

    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(8)),
                    Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(200.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(24)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(24 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(8)),
                    Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(300.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(24)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(24 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(8)),
                    Duration(std::chrono::hours(16), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(400.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(24)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(24 * NB_POINTS_PER_HOUR + 1)));

    DateTime fullPeriodStart(
            date::year_month_day(date::year(2017), date::month(6), date::day(2)),
            Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0)));
    DateTime fullPeriodEnd(
            date::year_month_day(date::year(2017), date::month(6), date::day(9)),
            Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0)));
    IntakeSeries iSeries;
    IntakeExtractor extractor;
    ComputingStatus result = extractor.extract(
            dh,
            fullPeriodStart,
            fullPeriodEnd,
            NB_POINTS_PER_HOUR,
            TucuUnit("mg"),
            iSeries,
            ExtractionOption::EndOfCycle);
    ASSERT_EQ(result, ComputingStatus::Ok);

    ASSERT_EQ(iSeries.size(), expectedIntakes.size());

    for (size_t i = 0; i < expectedIntakes.size(); ++i) {
        ASSERT_EQ(iSeries[i], expectedIntakes[i]);
    }
}

/// \brief Complex scenario to test ParallelSequence capabilities.
/// The treatment is given:
/// - twice a day (at 12h00 and 22h00)
/// - 10x every 3 hours, with an initial offset of 2h30
/// - 4x every 6 hours, with an initial offset of 6h30
/// - the whole processus is repeated twice
TEST(Core_TestIntakeExtractor, ComplexParallelSequence1)
{
    // List of time ranges that will be pushed into the history
    DosageTimeRangeList timeRangeList;

    // Create a time range for the first week of June 2017, no end to test DosageRepeat
    DateTime startJune2017(
            date::year_month_day(date::year(2017), date::month(6), date::day(2)),
            Duration(std::chrono::hours(0), std::chrono::minutes(0), std::chrono::seconds(0)));

    // Add the treatments
    DailyDose firstDailyDose(
            DoseValue(100.0),
            TucuUnit("mg"),
            getInfusionFormulationAndRoute(),
            Duration(std::chrono::minutes(20)),
            TimeOfDay(Duration(std::chrono::hours(12), std::chrono::minutes(0), std::chrono::seconds(0))));
    DailyDose secondDailyDose(
            DoseValue(200.0),
            TucuUnit("mg"),
            getInfusionFormulationAndRoute(),
            Duration(std::chrono::minutes(10)),
            TimeOfDay(Duration(std::chrono::hours(22), std::chrono::minutes(0), std::chrono::seconds(0))));

    LastingDose doseEvery3Hours(
            DoseValue(300.0),
            TucuUnit("mg"),
            getBolusFormulationAndRoute(),
            Duration(),
            Duration(std::chrono::hours(3)));

    LastingDose doseEvery6Hours(
            DoseValue(400.0),
            TucuUnit("mg"),
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
    auto june2017 = std::make_unique<Tucuxi::Core::DosageTimeRange>(startJune2017, juneDoses);

    Duration expectedTimeStepDS(std::chrono::hours(32), std::chrono::minutes(30), std::chrono::seconds(0));
    ASSERT_EQ(ds.getTimeStep(), expectedTimeStepDS);
    ASSERT_EQ(juneDoses.getTimeStep(), expectedTimeStepDS + expectedTimeStepDS);

    // Create the dosage history
    DosageHistory dh;
    dh.addTimeRange(*june2017);

    // Expected intake series
    IntakeSeries expectedIntakes;
    // Intakes for doseEvery3Hours
    // First repeat
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                    Duration(std::chrono::hours(2), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(300.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(3)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(3 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                    Duration(std::chrono::hours(5), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(300.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(3)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(3 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                    Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(300.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(3)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(3 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                    Duration(std::chrono::hours(11), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(300.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(3)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(3 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                    Duration(std::chrono::hours(14), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(300.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(3)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(3 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                    Duration(std::chrono::hours(17), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(300.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(3)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(3 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                    Duration(std::chrono::hours(20), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(300.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(3)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(3 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                    Duration(std::chrono::hours(23), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(300.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(3)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(3 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(3)),
                    Duration(std::chrono::hours(2), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(300.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(3)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(3 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(3)),
                    Duration(std::chrono::hours(5), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(300.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(3)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(3 * NB_POINTS_PER_HOUR + 1)));
    // Second repeat
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(3)),
                    Duration(std::chrono::hours(11), std::chrono::minutes(0), std::chrono::seconds(0))),
            Duration(),
            DoseValue(300.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(3)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(3 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(3)),
                    Duration(std::chrono::hours(14), std::chrono::minutes(0), std::chrono::seconds(0))),
            Duration(),
            DoseValue(300.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(3)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(3 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(3)),
                    Duration(std::chrono::hours(17), std::chrono::minutes(0), std::chrono::seconds(0))),
            Duration(),
            DoseValue(300.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(3)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(3 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(3)),
                    Duration(std::chrono::hours(20), std::chrono::minutes(0), std::chrono::seconds(0))),
            Duration(),
            DoseValue(300.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(3)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(3 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(3)),
                    Duration(std::chrono::hours(23), std::chrono::minutes(0), std::chrono::seconds(0))),
            Duration(),
            DoseValue(300.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(3)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(3 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(4)),
                    Duration(std::chrono::hours(2), std::chrono::minutes(0), std::chrono::seconds(0))),
            Duration(),
            DoseValue(300.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(3)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(3 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(4)),
                    Duration(std::chrono::hours(5), std::chrono::minutes(0), std::chrono::seconds(0))),
            Duration(),
            DoseValue(300.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(3)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(3 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(4)),
                    Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0))),
            Duration(),
            DoseValue(300.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(3)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(3 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(4)),
                    Duration(std::chrono::hours(11), std::chrono::minutes(0), std::chrono::seconds(0))),
            Duration(),
            DoseValue(300.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(3)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(3 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(4)),
                    Duration(std::chrono::hours(14), std::chrono::minutes(0), std::chrono::seconds(0))),
            Duration(),
            DoseValue(300.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(3)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(3 * NB_POINTS_PER_HOUR + 1)));

    // Intakes for doseEvery6Hours
    // First repeat
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                    Duration(std::chrono::hours(6), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(400.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(6)),
            getExtraFormulationAndRoute(),
            getExtraFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(6 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                    Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(400.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(6)),
            getExtraFormulationAndRoute(),
            getExtraFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(6 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                    Duration(std::chrono::hours(18), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(400.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(6)),
            getExtraFormulationAndRoute(),
            getExtraFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(6 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(3)),
                    Duration(std::chrono::hours(0), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(400.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(6)),
            getExtraFormulationAndRoute(),
            getExtraFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(6 * NB_POINTS_PER_HOUR + 1)));
    // Second repeat
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(3)),
                    Duration(std::chrono::hours(15), std::chrono::minutes(0), std::chrono::seconds(0))),
            Duration(),
            DoseValue(400.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(6)),
            getExtraFormulationAndRoute(),
            getExtraFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(6 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(3)),
                    Duration(std::chrono::hours(21), std::chrono::minutes(0), std::chrono::seconds(0))),
            Duration(),
            DoseValue(400.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(6)),
            getExtraFormulationAndRoute(),
            getExtraFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(6 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(4)),
                    Duration(std::chrono::hours(3), std::chrono::minutes(0), std::chrono::seconds(0))),
            Duration(),
            DoseValue(400.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(6)),
            getExtraFormulationAndRoute(),
            getExtraFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(6 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(4)),
                    Duration(std::chrono::hours(9), std::chrono::minutes(0), std::chrono::seconds(0))),
            Duration(),
            DoseValue(400.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(6)),
            getExtraFormulationAndRoute(),
            getExtraFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(6 * NB_POINTS_PER_HOUR + 1)));

    // Intakes for firstDailyDose and secondDailyDose
    // First repeat
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                    Duration(std::chrono::hours(12), std::chrono::minutes(0), std::chrono::seconds(0))),
            Duration(),
            DoseValue(100.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(24)),
            getInfusionFormulationAndRoute(),
            getInfusionFormulationAndRoute().getAbsorptionModel(),
            Duration(std::chrono::minutes(20)),
            static_cast<int>(24 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                    Duration(std::chrono::hours(22), std::chrono::minutes(0), std::chrono::seconds(0))),
            Duration(),
            DoseValue(200.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(24)),
            getInfusionFormulationAndRoute(),
            getInfusionFormulationAndRoute().getAbsorptionModel(),
            Duration(std::chrono::minutes(10)),
            static_cast<int>(24 * NB_POINTS_PER_HOUR + 1)));
    // Second repeat
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(3)),
                    Duration(std::chrono::hours(12), std::chrono::minutes(0), std::chrono::seconds(0))),
            Duration(),
            DoseValue(100.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(24)),
            getInfusionFormulationAndRoute(),
            getInfusionFormulationAndRoute().getAbsorptionModel(),
            Duration(std::chrono::minutes(20)),
            static_cast<int>(24 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(3)),
                    Duration(std::chrono::hours(22), std::chrono::minutes(0), std::chrono::seconds(0))),
            Duration(),
            DoseValue(200.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(24)),
            getInfusionFormulationAndRoute(),
            getInfusionFormulationAndRoute().getAbsorptionModel(),
            Duration(std::chrono::minutes(10)),
            static_cast<int>(24 * NB_POINTS_PER_HOUR + 1)));

    std::sort(expectedIntakes.begin(), expectedIntakes.end());

    DateTime fullPeriodStart(
            date::year_month_day(date::year(2017), date::month(6), date::day(2)),
            Duration(std::chrono::hours(0), std::chrono::minutes(0), std::chrono::seconds(0)));
    IntakeSeries iSeries;
    IntakeExtractor extractor;
    ComputingStatus result =
            extractor.extract(dh, fullPeriodStart, DateTime::now(), NB_POINTS_PER_HOUR, TucuUnit("mg"), iSeries);
    ASSERT_EQ(result, ComputingStatus::Ok);

    ASSERT_EQ(iSeries.size(), expectedIntakes.size());

    for (size_t i = 0; i < expectedIntakes.size(); ++i) {
        ASSERT_EQ(iSeries[i], expectedIntakes[i]);
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
TEST(Core_TestIntakeExtractor, ComplexParallelSequence2)
{
    // List of time ranges that will be pushed into the history
    DosageTimeRangeList timeRangeList;

    // Create a time range for the first week of June 2017, no end to test DosageRepeat
    DateTime startJune2017(
            date::year_month_day(date::year(2017), date::month(6), date::day(2)),
            Duration(std::chrono::hours(0), std::chrono::minutes(0), std::chrono::seconds(0)));

    // Add the treatments
    DailyDose firstDailyDose(
            DoseValue(100.0),
            TucuUnit("mg"),
            getInfusionFormulationAndRoute(),
            Duration(std::chrono::minutes(20)),
            TimeOfDay(Duration(std::chrono::hours(12), std::chrono::minutes(0), std::chrono::seconds(0))));
    DailyDose secondDailyDose(
            DoseValue(200.0),
            TucuUnit("mg"),
            getInfusionFormulationAndRoute(),
            Duration(std::chrono::minutes(10)),
            TimeOfDay(Duration(std::chrono::hours(22), std::chrono::minutes(0), std::chrono::seconds(0))));

    LastingDose doseEvery3Hours(
            DoseValue(300.0),
            TucuUnit("mg"),
            getBolusFormulationAndRoute(),
            Duration(),
            Duration(std::chrono::hours(3)));

    LastingDose doseEvery6Hours(
            DoseValue(400.0),
            TucuUnit("mg"),
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
    auto june2017 = std::make_unique<Tucuxi::Core::DosageTimeRange>(startJune2017, juneDoses);

    Duration expectedTimeStepDS(std::chrono::hours(32), std::chrono::minutes(30), std::chrono::seconds(0));
    ASSERT_EQ(ds.getTimeStep(), expectedTimeStepDS);
    ASSERT_EQ(juneDoses.getTimeStep(), expectedTimeStepDS + expectedTimeStepDS);

    // Add changes to the scheduled treatments
    june2017->addIntakeChange(
            IntakeEvent(
                    DateTime(
                            date::year_month_day(date::year(2017), date::month(6), date::day(3)),
                            Duration(std::chrono::hours(2), std::chrono::minutes(30), std::chrono::seconds(0))),
                    Duration(),
                    DoseValue(300.0),
                    TucuUnit("mg"),
                    Duration(std::chrono::hours(3)),
                    getBolusFormulationAndRoute(),
                    getBolusFormulationAndRoute().getAbsorptionModel(),
                    Duration(),
                    static_cast<int>(3 * NB_POINTS_PER_HOUR + 1)),
            ScheduledIntakeOp::Skip);
    june2017->addIntakeChange(
            IntakeEvent(
                    DateTime(
                            date::year_month_day(date::year(2017), date::month(6), date::day(3)),
                            Duration(std::chrono::hours(2), std::chrono::minutes(49), std::chrono::seconds(0))),
                    Duration(),
                    DoseValue(350.0),
                    TucuUnit("mg"),
                    Duration(std::chrono::hours(3)),
                    getExtraFormulationAndRoute(),
                    getExtraFormulationAndRoute().getAbsorptionModel(),
                    Duration(),
                    static_cast<int>(3 * NB_POINTS_PER_HOUR + 1)),
            ScheduledIntakeOp::Add);
    june2017->addIntakeChange(
            IntakeEvent(
                    DateTime(
                            date::year_month_day(date::year(2017), date::month(6), date::day(3)),
                            Duration(std::chrono::hours(12), std::chrono::minutes(0), std::chrono::seconds(0))),
                    Duration(),
                    DoseValue(100.0),
                    TucuUnit("mg"),
                    Duration(std::chrono::hours(24)),
                    getInfusionFormulationAndRoute(),
                    getInfusionFormulationAndRoute().getAbsorptionModel(),
                    Duration(std::chrono::minutes(20)),
                    static_cast<int>(24 * NB_POINTS_PER_HOUR + 1)),
            ScheduledIntakeOp::Skip);

    // Create the dosage history
    DosageHistory dh;
    dh.addTimeRange(*june2017);

    // Expected intake series
    IntakeSeries expectedIntakes;
    // Intakes for doseEvery3Hours
    // First repeat
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                    Duration(std::chrono::hours(2), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(300.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(3)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(3 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                    Duration(std::chrono::hours(5), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(300.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(3)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(3 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                    Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(300.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(3)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(3 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                    Duration(std::chrono::hours(11), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(300.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(3)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(3 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                    Duration(std::chrono::hours(14), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(300.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(3)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(3 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                    Duration(std::chrono::hours(17), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(300.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(3)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(3 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                    Duration(std::chrono::hours(20), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(300.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(3)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(3 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                    Duration(std::chrono::hours(23), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(300.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(3)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(3 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(3)),
                    Duration(std::chrono::hours(2), std::chrono::minutes(49), std::chrono::seconds(0))),
            Duration(),
            DoseValue(350.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(3)),
            getExtraFormulationAndRoute(),
            getExtraFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(3 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(3)),
                    Duration(std::chrono::hours(5), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(300.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(3)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(3 * NB_POINTS_PER_HOUR + 1)));
    // Second repeat
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(3)),
                    Duration(std::chrono::hours(11), std::chrono::minutes(0), std::chrono::seconds(0))),
            Duration(),
            DoseValue(300.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(3)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(3 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(3)),
                    Duration(std::chrono::hours(14), std::chrono::minutes(0), std::chrono::seconds(0))),
            Duration(),
            DoseValue(300.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(3)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(3 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(3)),
                    Duration(std::chrono::hours(17), std::chrono::minutes(0), std::chrono::seconds(0))),
            Duration(),
            DoseValue(300.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(3)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(3 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(3)),
                    Duration(std::chrono::hours(20), std::chrono::minutes(0), std::chrono::seconds(0))),
            Duration(),
            DoseValue(300.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(3)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(3 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(3)),
                    Duration(std::chrono::hours(23), std::chrono::minutes(0), std::chrono::seconds(0))),
            Duration(),
            DoseValue(300.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(3)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(3 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(4)),
                    Duration(std::chrono::hours(2), std::chrono::minutes(0), std::chrono::seconds(0))),
            Duration(),
            DoseValue(300.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(3)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(3 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(4)),
                    Duration(std::chrono::hours(5), std::chrono::minutes(0), std::chrono::seconds(0))),
            Duration(),
            DoseValue(300.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(3)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(3 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(4)),
                    Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0))),
            Duration(),
            DoseValue(300.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(3)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(3 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(4)),
                    Duration(std::chrono::hours(11), std::chrono::minutes(0), std::chrono::seconds(0))),
            Duration(),
            DoseValue(300.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(3)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(3 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(4)),
                    Duration(std::chrono::hours(14), std::chrono::minutes(0), std::chrono::seconds(0))),
            Duration(),
            DoseValue(300.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(3)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(3 * NB_POINTS_PER_HOUR + 1)));

    // Intakes for doseEvery6Hours
    // First repeat
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                    Duration(std::chrono::hours(6), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(400.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(6)),
            getExtraFormulationAndRoute(),
            getExtraFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(6 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                    Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(400.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(6)),
            getExtraFormulationAndRoute(),
            getExtraFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(6 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                    Duration(std::chrono::hours(18), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(400.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(6)),
            getExtraFormulationAndRoute(),
            getExtraFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(6 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(3)),
                    Duration(std::chrono::hours(0), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(400.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(6)),
            getExtraFormulationAndRoute(),
            getExtraFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(6 * NB_POINTS_PER_HOUR + 1)));
    // Second repeat
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(3)),
                    Duration(std::chrono::hours(15), std::chrono::minutes(0), std::chrono::seconds(0))),
            Duration(),
            DoseValue(400.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(6)),
            getExtraFormulationAndRoute(),
            getExtraFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(6 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(3)),
                    Duration(std::chrono::hours(21), std::chrono::minutes(0), std::chrono::seconds(0))),
            Duration(),
            DoseValue(400.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(6)),
            getExtraFormulationAndRoute(),
            getExtraFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(6 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(4)),
                    Duration(std::chrono::hours(3), std::chrono::minutes(0), std::chrono::seconds(0))),
            Duration(),
            DoseValue(400.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(6)),
            getExtraFormulationAndRoute(),
            getExtraFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(6 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(4)),
                    Duration(std::chrono::hours(9), std::chrono::minutes(0), std::chrono::seconds(0))),
            Duration(),
            DoseValue(400.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(6)),
            getExtraFormulationAndRoute(),
            getExtraFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(6 * NB_POINTS_PER_HOUR + 1)));

    // Intakes for firstDailyDose and secondDailyDose
    // First repeat
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                    Duration(std::chrono::hours(12), std::chrono::minutes(0), std::chrono::seconds(0))),
            Duration(),
            DoseValue(100.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(24)),
            getInfusionFormulationAndRoute(),
            getInfusionFormulationAndRoute().getAbsorptionModel(),
            Duration(std::chrono::minutes(20)),
            static_cast<int>(24 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                    Duration(std::chrono::hours(22), std::chrono::minutes(0), std::chrono::seconds(0))),
            Duration(),
            DoseValue(200.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(24)),
            getInfusionFormulationAndRoute(),
            getInfusionFormulationAndRoute().getAbsorptionModel(),
            Duration(std::chrono::minutes(10)),
            static_cast<int>(24 * NB_POINTS_PER_HOUR + 1)));
    // Second repeat
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(3)),
                    Duration(std::chrono::hours(22), std::chrono::minutes(0), std::chrono::seconds(0))),
            Duration(),
            DoseValue(200.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(24)),
            getInfusionFormulationAndRoute(),
            getInfusionFormulationAndRoute().getAbsorptionModel(),
            Duration(std::chrono::minutes(10)),
            static_cast<int>(24 * NB_POINTS_PER_HOUR + 1)));

    std::sort(expectedIntakes.begin(), expectedIntakes.end());

    DateTime fullPeriodStart(
            date::year_month_day(date::year(2017), date::month(6), date::day(2)),
            Duration(std::chrono::hours(0), std::chrono::minutes(0), std::chrono::seconds(0)));
    IntakeSeries iSeries;
    IntakeExtractor extractor;
    ComputingStatus result =
            extractor.extract(dh, fullPeriodStart, DateTime::now(), NB_POINTS_PER_HOUR, TucuUnit("mg"), iSeries);
    ASSERT_EQ(result, ComputingStatus::Ok);

    ASSERT_EQ(iSeries.size(), expectedIntakes.size());

    for (size_t i = 0; i < expectedIntakes.size(); ++i) {
        ASSERT_EQ(iSeries[i], expectedIntakes[i]);
    }
}

/// \brief Scenario where a treatment is given every day but Monday. Validate on the first two full weeks of June
/// 2017.
TEST(Core_TestIntakeExtractor, FullWeekExceptMonday)
{
    // List of time ranges that will be pushed into the history
    DosageTimeRangeList timeRangeList;

    // Create a time range starting the first full week of July 2017
    DateTime startJuly2017(
            date::year_month_day(date::year(2017), date::month(7), date::day(2)),
            Duration(std::chrono::hours(0), std::chrono::minutes(0), std::chrono::seconds(0)));

    // Add a treatment intake every day in the period, except on mondays
    WeeklyDose sundayDose(
            DoseValue(100.0),
            TucuUnit("mg"),
            getBolusFormulationAndRoute(),
            Duration(),
            TimeOfDay(Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
            DayOfWeek(SUNDAY));
    WeeklyDose tuesdayDose(
            DoseValue(200.0),
            TucuUnit("mg"),
            getBolusFormulationAndRoute(),
            Duration(),
            TimeOfDay(Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
            DayOfWeek(TUESDAY));
    WeeklyDose wednesdayDose(
            DoseValue(300.0),
            TucuUnit("mg"),
            getBolusFormulationAndRoute(),
            Duration(),
            TimeOfDay(Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
            DayOfWeek(WEDNESDAY));
    WeeklyDose thursdayDose(
            DoseValue(400.0),
            TucuUnit("mg"),
            getBolusFormulationAndRoute(),
            Duration(),
            TimeOfDay(Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
            DayOfWeek(THURSDAY));
    WeeklyDose fridayDose(
            DoseValue(500.0),
            TucuUnit("mg"),
            getBolusFormulationAndRoute(),
            Duration(),
            TimeOfDay(Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
            DayOfWeek(FRIDAY));
    WeeklyDose saturdayDose(
            DoseValue(600.0),
            TucuUnit("mg"),
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
    auto trFromJuly2017 = std::make_unique<Tucuxi::Core::DosageTimeRange>(startJuly2017, treatment);

    // Create the dosage history
    DosageHistory dh;
    dh.addTimeRange(*trFromJuly2017);

    // Expected intake series
    IntakeSeries expectedIntakes;
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(7), date::day(2)),
                    Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(100.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(7 * 24)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(7 * 24 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(7), date::day(4)),
                    Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(200.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(7 * 24)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(7 * 24 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(7), date::day(5)),
                    Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(300.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(7 * 24)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(7 * 24 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(7), date::day(6)),
                    Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(400.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(7 * 24)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(7 * 24 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(7), date::day(7)),
                    Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(500.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(7 * 24)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(7 * 24 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(7), date::day(8)),
                    Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(600.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(7 * 24)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(7 * 24 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(7), date::day(9)),
                    Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(100.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(7 * 24)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(7 * 24 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(7), date::day(11)),
                    Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(200.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(7 * 24)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(7 * 24 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(7), date::day(12)),
                    Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(300.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(7 * 24)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(7 * 24 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(7), date::day(13)),
                    Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(400.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(7 * 24)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(7 * 24 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(7), date::day(14)),
                    Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(500.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(7 * 24)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(7 * 24 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(7), date::day(15)),
                    Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(600.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(7 * 24)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(7 * 24 * NB_POINTS_PER_HOUR + 1)));


    DateTime fullPeriodStart(
            date::year_month_day(date::year(2017), date::month(7), date::day(2)),
            Duration(std::chrono::hours(0), std::chrono::minutes(0), std::chrono::seconds(0)));
    DateTime fullPeriodEnd(
            date::year_month_day(date::year(2017), date::month(7), date::day(16)),
            Duration(std::chrono::hours(0), std::chrono::minutes(0), std::chrono::seconds(0)));
    IntakeSeries iSeries;
    IntakeExtractor extractor;
    ComputingStatus result = extractor.extract(
            dh,
            fullPeriodStart,
            fullPeriodEnd,
            NB_POINTS_PER_HOUR,
            TucuUnit("mg"),
            iSeries,
            ExtractionOption::EndOfCycle);
    ASSERT_EQ(result, ComputingStatus::Ok);

    ASSERT_EQ(iSeries.size(), expectedIntakes.size());

    for (size_t i = 0; i < iSeries.size(); ++i) {
        ASSERT_EQ(iSeries[i], expectedIntakes[i]);
    }
}

/// \brief Scenario where a treatment is given once every 36 hours.
TEST(Core_TestIntakeExtractor, OnceEvery36HoursAtSteadyState)
{
    // List of time ranges that will be pushed into the history
    DosageTimeRangeList timeRangeList;

    // Create a time range for the first week of June 2017
    DateTime emptyStart = DateTime::now();
    DateTime emptyEnd = DateTime::now();

    // Add a treatment intake every ten days in June
    // 200mg via a intravascular at 08h30, starting the 01.06
    LastingDose junePeriodicDose(
            DoseValue(200.0),
            TucuUnit("mg"),
            getBolusFormulationAndRoute(),
            Duration(),
            Duration(std::chrono::hours(36)));

    DateTime lastDose(
            date::year_month_day(date::year(2017), date::month(6), date::day(4)),
            Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0)));
    DosageSteadyState juneDose(junePeriodicDose, lastDose);
    auto june2017 = std::make_unique<Tucuxi::Core::DosageTimeRange>(emptyStart, emptyEnd, juneDose);

    // Create the dosage history
    DosageHistory dh;
    dh.addTimeRange(*june2017);

    // Expected intake series
    IntakeSeries expectedIntakes;
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(1)),
                    Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(200.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(36)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(36 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(2)),
                    Duration(std::chrono::hours(20), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(200.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(36)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(36 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(4)),
                    Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(200.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(36)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(36 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(5)),
                    Duration(std::chrono::hours(20), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(200.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(36)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(36 * NB_POINTS_PER_HOUR + 1)));
    expectedIntakes.push_back(IntakeEvent(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(7)),
                    Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(200.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(24)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(),
            static_cast<int>(24 * NB_POINTS_PER_HOUR + 1)));

    // The 16.07.2017 must be EXCLUDED (the intervals are closed on the left, but opened on the right side!)
    {
        DateTime fullPeriodStart(
                date::year_month_day(date::year(2017), date::month(6), date::day(1)),
                Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0)));
        DateTime fullPeriodEnd(
                date::year_month_day(date::year(2017), date::month(6), date::day(8)),
                Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0)));
        IntakeSeries iSeries;
        IntakeExtractor extractor;
        ComputingStatus result =
                extractor.extract(dh, fullPeriodStart, fullPeriodEnd, NB_POINTS_PER_HOUR, TucuUnit("mg"), iSeries);
        ASSERT_EQ(result, ComputingStatus::Ok);

        ASSERT_EQ(iSeries.size(), expectedIntakes.size());

        for (size_t i = 0; i < iSeries.size(); ++i) {
            ASSERT_EQ(iSeries[i], expectedIntakes[i]);
        }
    }
    {
        // Same test, but the start is 30 minutes before. The output should be the same
        DateTime fullPeriodStart(
                date::year_month_day(date::year(2017), date::month(6), date::day(1)),
                Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));
        DateTime fullPeriodEnd(
                date::year_month_day(date::year(2017), date::month(6), date::day(8)),
                Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0)));
        IntakeSeries iSeries;
        IntakeExtractor extractor;
        ComputingStatus result =
                extractor.extract(dh, fullPeriodStart, fullPeriodEnd, NB_POINTS_PER_HOUR, TucuUnit("mg"), iSeries);
        ASSERT_EQ(result, ComputingStatus::Ok);

        ASSERT_EQ(iSeries.size(), expectedIntakes.size());

        for (size_t i = 0; i < iSeries.size(); ++i) {
            ASSERT_EQ(iSeries[i], expectedIntakes[i]);
        }
    }
}
