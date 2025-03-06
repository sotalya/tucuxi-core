#include <gtest/gtest.h>
#include <date/date.h>
#include <memory>

#include "testutils.h"
#include "tucucore/tdacalculator.h"
#include "tucucore/dosage.h"
#include "gtest_core.h"
#include "tucucore/intakeextractor.h"


using namespace Tucuxi::Core;
using namespace Tucuxi::Common;

std::unique_ptr<DosageHistory> createDosageHistory() {
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

Samples create_samples(){
    Samples samples;
    AnalyteId analyteId("theAnalyte");

    samples.push_back(
            std::make_unique<Sample>(DATE_TIME_NO_VAR(2017, 01, 02, 8, 00, 00), analyteId, 12.0, TucuUnit("ug/l")));
    samples.push_back(
            std::make_unique<Sample>(DATE_TIME_NO_VAR(2018, 01, 02, 8, 00, 00), analyteId, 10.0, TucuUnit("ug/l")));
    samples.push_back(
            std::make_unique<Sample>(DATE_TIME_NO_VAR(2018, 01, 03, 8, 00, 00), analyteId, 14.0, TucuUnit("mg/l")));
    samples.push_back(
            std::make_unique<Sample>(DATE_TIME_NO_VAR(2018, 01, 12, 8, 00, 00), analyteId, 12.0, TucuUnit("ug/l")));

    return samples;
}


TEST(Core_TestTDAExtractor, CalculateDurations) {
    IntakeExtractor extractor;
    TDACalculator tda_calc;

    std::unique_ptr<DosageHistory> dh = createDosageHistory();
    Samples samples = create_samples();

    tda_calc.calculateDurations(samples, *dh);
    // std::vector<Duration> durations = calculator->calculateDurations(samples, *dosageHistory);

    // ASSERT_EQ(durations.size(), samples.size());

    // // Vérification des valeurs attendues (à ajuster selon la logique)
    // EXPECT_EQ(durations[0].toHours(), -1.0);  // Échantillon avant la première prise connue
    // EXPECT_GT(durations[1].toHours(), 0);     // Devrait être une durée positive
    // EXPECT_GT(durations[2].toHours(), 0);
}
