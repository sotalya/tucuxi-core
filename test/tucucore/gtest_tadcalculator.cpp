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

std::unique_ptr<DosageHistory> createDosageHistory()
{
    DateTime startJune2017(
            date::year_month_day(date::year(2017), date::month(6), date::day(1)), std::chrono::seconds(0));
    DateTime endJune2017(date::year_month_day(date::year(2017), date::month(7), date::day(1)), std::chrono::seconds(0));
    DateTime startJuly2017(
            date::year_month_day(date::year(2017), date::month(7), date::day(1)), std::chrono::seconds(0));
    DateTime endJuly2017(
            date::year_month_day(date::year(2017), date::month(7), date::day(16)), std::chrono::seconds(0));

    // Traitement hebdomadaire en juin (mardi 8h30, 200 mg, 20 min perfusion)
    WeeklyDose juneWeeklyDose(
            DoseValue(200.0),
            TucuUnit("mg"),
            getInfusionFormulationAndRoute(),
            Duration(std::chrono::minutes(20)),
            TimeOfDay(Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
            DayOfWeek(TUESDAY));
    DosageLoop juneDose(juneWeeklyDose);
    auto june2017 = std::make_unique<Tucuxi::Core::DosageTimeRange>(startJune2017, endJune2017, juneDose);


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


    auto dosageHistory = std::make_unique<DosageHistory>();
    dosageHistory->addTimeRange(*june2017);
    dosageHistory->addTimeRange(*july2017);

    return dosageHistory;
}

Samples createSamples()
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


TEST(Core_TestTADExtractor, CalculateDurations)
{
    IntakeExtractor extractor;
    TimeAfterDoseCalculator tadCalc;

    std::unique_ptr<DosageHistory> dh = createDosageHistory();
    Samples samples = createSamples();

    std::vector<Duration> durations = tadCalc.calculateDurations(samples, *dh);

    ASSERT_EQ(durations.size(), samples.size());

    EXPECT_EQ(durations[0].toHours(), -5.5);
    EXPECT_EQ(durations[1].toHours(), 48.0);
    EXPECT_EQ(durations[2].toHours(), 121.5);
    EXPECT_EQ(durations[3].toHours(), 0.0);
    EXPECT_EQ(durations[4].toHours(), 0.5);
}
