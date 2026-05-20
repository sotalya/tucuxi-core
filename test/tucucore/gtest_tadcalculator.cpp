#include <memory>

#include <date/date.h>
#include <gtest/gtest.h>

#include "tucucore/dosage.h"
#include "tucucore/intakeextractor.h"
#include "tucucore/timeafterdosecalculator.h"

#include "gtest_core.h"
#include "testutils.h"


using namespace Tucuxi::Core;
using namespace Tucuxi::Common;


/// Fixture that provides common dosage-building helpers.
class TADCalculatorTest : public testing::Test
{
protected:
    /// Create a simple DosageHistory with a single LastingDose time range.
    /// Doses are spaced by the given interval from startDate to endDate.
    static DosageHistory createLastingDoseHistory(
            const DateTime& _startDate, const DateTime& _endDate, const Duration& _interval)
    {
        DosageHistory history;
        Unit unit{"mg"};
        FormulationAndRoute formulationAndRoute;
        Duration infusionTime; // 0 by default

        LastingDose lastingDose{100, unit, formulationAndRoute, infusionTime, _interval};
        DosageTimeRange timeRange{_startDate, _endDate, lastingDose};
        history.addTimeRange(timeRange);
        return history;
    }

    /// Create a Samples vector from a list of DateTimes. All samples share the same analyte/value/unit.
    static Samples createSamples(const std::vector<DateTime>& _dates)
    {
        Samples samples;
        for (const auto& date : _dates) {
            samples.emplace_back(std::make_unique<Sample>(date, AnalyteId("drug"), 10.0, TucuUnit("ng/mL")));
        }
        return samples;
    }

    std::unique_ptr<DosageHistory> createDosageHistory()
    {
        DateTime startJune2017(
                date::year_month_day(date::year(2017), date::month(6), date::day(1)), std::chrono::seconds(0));
        DateTime endJune2017(
                date::year_month_day(date::year(2017), date::month(7), date::day(1)), std::chrono::seconds(0));
        DateTime startJuly2017(
                date::year_month_day(date::year(2017), date::month(7), date::day(1)), std::chrono::seconds(0));
        DateTime endJuly2017(
                date::year_month_day(date::year(2017), date::month(7), date::day(16)), std::chrono::seconds(0));

        // Traitement hebdomadaire en juin (mardi 8h30, 200 mg, 20 min perfusion)
        WeeklyDose juneWeeklyDose(
                DoseValue{200.0},
                TucuUnit("mg"),
                getInfusionFormulationAndRoute(),
                Duration(std::chrono::minutes(20)),
                TimeOfDay(Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
                DayOfWeek(TUESDAY));
        DosageLoop juneDose(juneWeeklyDose);
        auto june2017 = std::make_unique<Tucuxi::Core::DosageTimeRange>(startJune2017, endJune2017, juneDose);


        WeeklyDose julyWeeklyDose(
                DoseValue{400.0},
                TucuUnit("mg"),
                getInfusionFormulationAndRoute(),
                Duration(std::chrono::minutes(10)),
                TimeOfDay(Duration(std::chrono::hours(11), std::chrono::minutes(30), std::chrono::seconds(0))),
                DayOfWeek(WEDNESDAY));
        DosageLoop julyDose(julyWeeklyDose);
        auto july2017 = std::make_unique<Tucuxi::Core::DosageTimeRange>(startJuly2017, endJuly2017, julyDose);
        assert(!timeRangesOverlap(*june2017, *july2017));


        auto dosageHistory = std::make_unique<DosageHistory>();
        dosageHistory->addTimeRange(*june2017);
        dosageHistory->addTimeRange(*july2017);

        return dosageHistory;
    }

    Samples create5Samples()
    {
        Samples samples;
        AnalyteId analyteId("theAnalyte");

        samples.push_back(
                std::make_unique<Sample>(DATE_TIME_NO_VAR(2017, 6, 6, 3, 00, 00), analyteId, 12.0, TucuUnit("ug/l")));
        samples.push_back(
                std::make_unique<Sample>(DATE_TIME_NO_VAR(2017, 6, 8, 8, 30, 00), analyteId, 12.0, TucuUnit("ug/l")));
        samples.push_back(
                std::make_unique<Sample>(DATE_TIME_NO_VAR(2017, 6, 25, 10, 00, 00), analyteId, 12.0, TucuUnit("ug/l")));
        samples.push_back(
                std::make_unique<Sample>(DATE_TIME_NO_VAR(2017, 7, 5, 11, 30, 00), analyteId, 12.0, TucuUnit("ug/l")));
        samples.push_back(
                std::make_unique<Sample>(DATE_TIME_NO_VAR(2017, 7, 12, 12, 00, 00), analyteId, 12.0, TucuUnit("ug/l")));
        return samples;
    }

    /// Helper to build a DateTime from components.
    static DateTime makeDateTime(int _year, int _month, int _day, int _hour, int _minute, int _second)
    {
        return DateTime(
                date::year_month_day(date::year(_year), date::month(_month), date::day(_day)),
                TimeOfDay(Duration(
                        std::chrono::hours(_hour), std::chrono::minutes(_minute), std::chrono::seconds(_second))));
    }
};


TEST_F(TADCalculatorTest, CalculateDurations)
{
    IntakeExtractor extractor;
    TimeAfterDoseCalculator tadCalc;

    std::unique_ptr<DosageHistory> dh = createDosageHistory();
    Samples samples = create5Samples();

    std::vector<Duration> durations = tadCalc.calculateDurations(samples, *dh);

    ASSERT_EQ(durations.size(), samples.size());

    EXPECT_EQ(durations[0].toHours(), -5.5);
    EXPECT_EQ(durations[1].toHours(), 48.0);
    EXPECT_EQ(durations[2].toHours(), 121.5);
    EXPECT_EQ(durations[3].toHours(), 195.0);
    EXPECT_EQ(durations[4].toHours(), 0.5);
}

/// A single sample taken 30 minutes after a dose should yield a 30-minute duration.
TEST_F(TADCalculatorTest, SingleSampleAfterSingleDose)
{
    // Doses every 12 hours starting at 08:00 on Jan 1
    DateTime start = makeDateTime(2024, 1, 1, 8, 0, 0);
    DateTime end = makeDateTime(2024, 1, 2, 8, 0, 0);
    Duration interval{std::chrono::hours(12)};

    DosageHistory history = createLastingDoseHistory(start, end, interval);

    // Sample at 08:30 -> 30 min after the 08:00 dose
    Samples samples = createSamples({makeDateTime(2024, 1, 1, 8, 30, 0)});

    TimeAfterDoseCalculator calculator;
    std::vector<Duration> durations = calculator.calculateDurations(samples, history);

    ASSERT_EQ(durations.size(), 1u);
    EXPECT_EQ(durations[0], Duration(std::chrono::minutes(30)));
}


/// A sample taken exactly at a dose time should yield a zero duration.
TEST_F(TADCalculatorTest, SampleExactlyAtDoseTime)
{
    DateTime start = makeDateTime(2024, 1, 1, 8, 0, 0);
    DateTime end = makeDateTime(2024, 1, 2, 8, 0, 0);
    Duration interval{std::chrono::hours(12)};

    DosageHistory history = createLastingDoseHistory(start, end, interval);

    // Sample exactly at start (08:00)
    Samples samples = createSamples({makeDateTime(2024, 1, 1, 8, 0, 0)});

    TimeAfterDoseCalculator calculator;
    std::vector<Duration> durations = calculator.calculateDurations(samples, history);

    ASSERT_EQ(durations.size(), 1u);
    EXPECT_EQ(durations[0], Duration(std::chrono::seconds(0)));
}


/// A sample between two doses should be relative to the most recent preceding dose.
TEST_F(TADCalculatorTest, SampleBetweenTwoDoses)
{
    DateTime start = makeDateTime(2024, 1, 1, 8, 0, 0);
    DateTime end = makeDateTime(2024, 1, 2, 8, 0, 0);
    Duration interval{std::chrono::hours(12)};

    DosageHistory history = createLastingDoseHistory(start, end, interval);

    // Sample at 14:00 -> 6 hours after the 08:00 dose, 6 hours before the 20:00 dose
    Samples samples = createSamples({makeDateTime(2024, 1, 1, 14, 0, 0)});

    TimeAfterDoseCalculator calculator;
    std::vector<Duration> durations = calculator.calculateDurations(samples, history);

    ASSERT_EQ(durations.size(), 1u);
    EXPECT_EQ(durations[0], Duration(std::chrono::hours(6)));
}


/// Multiple samples should each yield the correct duration from their closest preceding intake.
TEST_F(TADCalculatorTest, MultipleSamples)
{
    // Three time ranges to produce intakes at 08:00, 16:00, and 00:00
    DosageHistory history;
    Unit unit{"mg"};
    FormulationAndRoute formulationAndRoute;
    Duration infusionTime;

    LastingDose dose{100, unit, formulationAndRoute, infusionTime, Duration{std::chrono::hours(8)}};

    DosageTimeRange range1{makeDateTime(2024, 1, 1, 8, 0, 0), makeDateTime(2024, 1, 1, 16, 0, 0), dose};
    DosageTimeRange range2{makeDateTime(2024, 1, 1, 16, 0, 0), makeDateTime(2024, 1, 2, 0, 0, 0), dose};
    DosageTimeRange range3{makeDateTime(2024, 1, 2, 0, 0, 0), makeDateTime(2024, 1, 2, 8, 0, 0), dose};

    history.addTimeRange(range1);
    history.addTimeRange(range2);
    history.addTimeRange(range3);

    // Three samples:
    //  - 10:00 -> 2h after 08:00 dose
    //  - 20:00 -> 4h after 16:00 dose
    //  - 01:00 -> 1h after 00:00 dose
    Samples samples = createSamples(
            {makeDateTime(2024, 1, 1, 10, 0, 0),
             makeDateTime(2024, 1, 1, 20, 0, 0),
             makeDateTime(2024, 1, 2, 1, 0, 0)});

    TimeAfterDoseCalculator calculator;
    std::vector<Duration> durations = calculator.calculateDurations(samples, history);

    ASSERT_EQ(durations.size(), 3u);
    EXPECT_EQ(durations[0], Duration(std::chrono::hours(2)));
    EXPECT_EQ(durations[1], Duration(std::chrono::hours(4)));
    EXPECT_EQ(durations[2], Duration(std::chrono::hours(1)));
}


/// Two dosage time ranges: sample in the second range should use the correct intake.
TEST_F(TADCalculatorTest, MultipleTimeRanges)
{
    DosageHistory history;
    Unit unit{"mg"};
    FormulationAndRoute formulationAndRoute;
    Duration infusionTime;

    // First range: Jan 1 08:00 to Jan 2 08:00, every 12h
    LastingDose dose1{100, unit, formulationAndRoute, infusionTime, Duration{std::chrono::hours(12)}};
    DosageTimeRange range1{makeDateTime(2024, 1, 1, 8, 0, 0), makeDateTime(2024, 1, 2, 8, 0, 0), dose1};
    history.addTimeRange(range1);

    // Second range: Jan 2 08:00 to Jan 3 08:00, every 8h
    LastingDose dose2{200, unit, formulationAndRoute, infusionTime, Duration{std::chrono::hours(8)}};
    DosageTimeRange range2{makeDateTime(2024, 1, 2, 8, 0, 0), makeDateTime(2024, 1, 3, 8, 0, 0), dose2};
    history.addTimeRange(range2);

    // Sample at Jan 2 12:00 -> 4h after the Jan 2 08:00 intake (start of second range)
    Samples samples = createSamples({makeDateTime(2024, 1, 2, 12, 0, 0)});

    TimeAfterDoseCalculator calculator;
    std::vector<Duration> durations = calculator.calculateDurations(samples, history);

    ASSERT_EQ(durations.size(), 1u);
    EXPECT_EQ(durations[0], Duration(std::chrono::hours(4)));
}


/// A sample at the exact boundary of the second dose should yield the interval.
TEST_F(TADCalculatorTest, SampleAtSecondDoseTime2Ranges)
{
    // Two time ranges to produce intakes at 08:00 and 20:00
    DosageHistory history;
    Unit unit{"mg"};
    FormulationAndRoute formulationAndRoute;
    Duration infusionTime;
    Duration interval{std::chrono::hours(12)};

    LastingDose dose{100, unit, formulationAndRoute, infusionTime, interval};

    DosageTimeRange range1{makeDateTime(2024, 1, 1, 8, 0, 0), makeDateTime(2024, 1, 1, 20, 0, 0), dose};
    DosageTimeRange range2{makeDateTime(2024, 1, 1, 20, 0, 0), makeDateTime(2024, 1, 2, 8, 0, 0), dose};

    history.addTimeRange(range1);
    history.addTimeRange(range2);

    // Sample at 20:00 (the second dose time)
    Samples samples = createSamples({makeDateTime(2024, 1, 1, 20, 0, 0)});

    TimeAfterDoseCalculator calculator;
    std::vector<Duration> durations = calculator.calculateDurations(samples, history);

    ASSERT_EQ(durations.size(), 1u);
    EXPECT_EQ(durations[0], Duration(std::chrono::hours(12)));
}

/// A sample at the exact boundary of the second dose should yield the interval.
TEST_F(TADCalculatorTest, SampleAtSecondDoseTime1Range)
{
    // Two time ranges to produce intakes at 08:00 and 20:00
    DosageHistory history;
    Unit unit{"mg"};
    FormulationAndRoute formulationAndRoute;
    Duration infusionTime;
    Duration interval{std::chrono::hours(12)};

    LastingDose dose{100, unit, formulationAndRoute, infusionTime, interval};

    DosageTimeRange range1{makeDateTime(2024, 1, 1, 8, 0, 0), makeDateTime(2024, 1, 2, 8, 0, 0), dose};

    history.addTimeRange(range1);

    // Sample at 20:00 (the second dose time)
    Samples samples = createSamples({makeDateTime(2024, 1, 1, 20, 0, 0)});

    TimeAfterDoseCalculator calculator;
    std::vector<Duration> durations = calculator.calculateDurations(samples, history);

    ASSERT_EQ(durations.size(), 1u);
    EXPECT_EQ(durations[0], Duration(std::chrono::hours(12)));
}


/// A sample at the exact boundary of the second dose should yield the interval.
TEST_F(TADCalculatorTest, SampleAtFirstDose)
{
    // Two time ranges to produce intakes at 08:00 and 20:00
    DosageHistory history;
    Unit unit{"mg"};
    FormulationAndRoute formulationAndRoute;
    Duration infusionTime;
    Duration interval{std::chrono::hours(12)};

    LastingDose dose{100, unit, formulationAndRoute, infusionTime, interval};

    DosageTimeRange range1{makeDateTime(2024, 1, 1, 8, 0, 0), makeDateTime(2024, 1, 2, 8, 0, 0), dose};

    history.addTimeRange(range1);

    // Sample at 20:00 (the second dose time)
    Samples samples = createSamples({makeDateTime(2024, 1, 1, 8, 0, 0)});

    TimeAfterDoseCalculator calculator;
    std::vector<Duration> durations = calculator.calculateDurations(samples, history);

    ASSERT_EQ(durations.size(), 1u);
    EXPECT_EQ(durations[0], Duration(std::chrono::hours(0)));
}


/// A sample taken before all intakes should produce a negative-ish duration (sampleTime - firstIntake).
TEST_F(TADCalculatorTest, SampleBeforeAllIntakes)
{
    // Doses start at 10:00
    DateTime start = makeDateTime(2024, 1, 1, 10, 0, 0);
    DateTime end = makeDateTime(2024, 1, 2, 10, 0, 0);
    Duration interval{std::chrono::hours(12)};

    DosageHistory history = createLastingDoseHistory(start, end, interval);

    // Sample at 09:00 -> 1h before the first 10:00 intake
    Samples samples = createSamples({makeDateTime(2024, 1, 1, 9, 0, 0)});

    TimeAfterDoseCalculator calculator;
    std::vector<Duration> durations = calculator.calculateDurations(samples, history);

    ASSERT_EQ(durations.size(), 1u);
    // The calculator uses sampleTime - firstIntakeTime, giving a negative duration
    Duration expected = makeDateTime(2024, 1, 1, 9, 0, 0) - makeDateTime(2024, 1, 1, 10, 0, 0);
    EXPECT_EQ(durations[0], expected);
}


/// A sample just before the second dose verifies correct "closest preceding" logic.
TEST_F(TADCalculatorTest, SampleJustBeforeSecondDose)
{
    DateTime start = makeDateTime(2024, 1, 1, 8, 0, 0);
    DateTime end = makeDateTime(2024, 1, 2, 8, 0, 0);
    Duration interval{std::chrono::hours(12)};

    DosageHistory history = createLastingDoseHistory(start, end, interval);

    // Sample at 19:59 -> 11h59m after the 08:00 dose (just before the 20:00 dose)
    Samples samples = createSamples({makeDateTime(2024, 1, 1, 19, 59, 0)});

    TimeAfterDoseCalculator calculator;
    std::vector<Duration> durations = calculator.calculateDurations(samples, history);

    ASSERT_EQ(durations.size(), 1u);
    Duration expected{std::chrono::hours(11), std::chrono::minutes(59), std::chrono::seconds(0)};
    EXPECT_EQ(durations[0], expected);
}
