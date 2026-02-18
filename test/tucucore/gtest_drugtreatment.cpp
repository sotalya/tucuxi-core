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
#include <string>

#include <gtest/gtest.h>

#include "tucucommon/utils.h"

#include "tucucore/definitions.h"
#include "tucucore/dosage.h"
#include "tucucore/drugmodel/formulationandroute.h"
#include "tucucore/drugtreatment/drugtreatment.h"

#include "testutils.h"

using namespace Tucuxi::Core;


// ============================================================
// Helper
// ============================================================

/// Build a simple DosageTimeRange: 4 bolus doses of 100 mg every 6 hours,
/// starting at _start with no explicit end date.
static std::unique_ptr<DosageTimeRange> buildSimpleTimeRange(const Tucuxi::Common::DateTime& _start)
{
    FormulationAndRoute route(Formulation::Test, AdministrationRoute::IntravenousBolus);
    LastingDose dose(DoseValue(100), TucuUnit("mg"), route, Duration(), Duration(std::chrono::hours(6)));
    DosageRepeat repeated(dose, 4);
    return std::make_unique<DosageTimeRange>(_start, repeated);
}


// ============================================================
// Construction
// ============================================================

TEST(Core_TestDrugTreatment, DefaultConstruction)
{
    // All collections must be empty after default construction.
    DrugTreatment treatment;
    EXPECT_TRUE(treatment.getSamples().empty());
    EXPECT_TRUE(treatment.getCovariates().empty());
    EXPECT_TRUE(treatment.getTargets().empty());
    EXPECT_TRUE(treatment.getDosageHistory().isEmpty());
    EXPECT_EQ(treatment.getDosageHistory().getNumberOfTimeRanges(), 0u);
}


// ============================================================
// Samples
// ============================================================

TEST(Core_TestDrugTreatment, AddSingleSample)
{
    // The sample must be retrievable with the exact properties it was created with.
    DrugTreatment treatment;
    Tucuxi::Common::DateTime sampleDate = DATE_TIME_NO_VAR(2021, 6, 15, 10, 30, 0);
    AnalyteId analyteId("ibuprofen");
    Value value = 12.5;
    TucuUnit unit("mg/l");

    treatment.addSample(std::make_unique<Sample>(sampleDate, analyteId, value, unit));

    const Samples& samples = treatment.getSamples();
    ASSERT_EQ(samples.size(), 1u);
    EXPECT_EQ(samples[0]->getDate(), sampleDate);
    EXPECT_EQ(samples[0]->getAnalyteID(), analyteId);
    EXPECT_DOUBLE_EQ(samples[0]->getValue(), value);
    EXPECT_EQ(samples[0]->getUnit().toString(), unit.toString());
}

TEST(Core_TestDrugTreatment, SampleDefaultWeight)
{
    // When no weight is given, the sample weight must default to 1.0.
    DrugTreatment treatment;
    treatment.addSample(std::make_unique<Sample>(
            DATE_TIME_NO_VAR(2021, 6, 15, 10, 0, 0), AnalyteId("drug"), 5.0, TucuUnit("mg/l")));

    EXPECT_DOUBLE_EQ(treatment.getSamples()[0]->getWeight(), 1.0);
}

TEST(Core_TestDrugTreatment, SampleCustomWeight)
{
    // A custom weight must be stored as provided.
    DrugTreatment treatment;
    treatment.addSample(std::make_unique<Sample>(
            DATE_TIME_NO_VAR(2021, 6, 15, 10, 0, 0), AnalyteId("drug"), 5.0, TucuUnit("mg/l"), 0.5));

    EXPECT_DOUBLE_EQ(treatment.getSamples()[0]->getWeight(), 0.5);
}

TEST(Core_TestDrugTreatment, AddMultipleSamples)
{
    // All samples must be stored and appear in insertion order.
    DrugTreatment treatment;
    treatment.addSample(
            std::make_unique<Sample>(DATE_TIME_NO_VAR(2021, 6, 15, 8, 0, 0), AnalyteId("drug"), 1.0, TucuUnit("mg/l")));
    treatment.addSample(std::make_unique<Sample>(
            DATE_TIME_NO_VAR(2021, 6, 15, 12, 0, 0), AnalyteId("drug"), 2.0, TucuUnit("mg/l")));
    treatment.addSample(std::make_unique<Sample>(
            DATE_TIME_NO_VAR(2021, 6, 15, 16, 0, 0), AnalyteId("drug"), 3.0, TucuUnit("mg/l")));

    const Samples& samples = treatment.getSamples();
    ASSERT_EQ(samples.size(), 3u);
    EXPECT_DOUBLE_EQ(samples[0]->getValue(), 1.0);
    EXPECT_DOUBLE_EQ(samples[1]->getValue(), 2.0);
    EXPECT_DOUBLE_EQ(samples[2]->getValue(), 3.0);
}


// ============================================================
// Covariates
// ============================================================

TEST(Core_TestDrugTreatment, AddSingleCovariate)
{
    // The covariate must be retrievable with the exact properties it was created with.
    DrugTreatment treatment;
    Tucuxi::Common::DateTime covDate = DATE_TIME_NO_VAR(2021, 1, 1, 0, 0, 0);
    std::string id = "weight";
    std::string value = Tucuxi::Common::Utils::varToString(70.0);
    TucuUnit unit("kg");

    treatment.addCovariate(std::make_unique<PatientCovariate>(id, value, DataType::Double, unit, covDate));

    const PatientVariates& covariates = treatment.getCovariates();
    ASSERT_EQ(covariates.size(), 1u);
    EXPECT_EQ(covariates[0]->getId(), id);
    EXPECT_EQ(covariates[0]->getValue(), value);
    EXPECT_EQ(covariates[0]->getDataType(), DataType::Double);
    EXPECT_EQ(covariates[0]->getUnit().toString(), unit.toString());
    EXPECT_EQ(covariates[0]->getEventTime(), covDate);
}

TEST(Core_TestDrugTreatment, AddMultipleCovariates)
{
    // All covariates must be stored and appear in insertion order.
    DrugTreatment treatment;
    Tucuxi::Common::DateTime t0 = DATE_TIME_NO_VAR(2021, 1, 1, 0, 0, 0);
    Tucuxi::Common::DateTime t1 = DATE_TIME_NO_VAR(2021, 2, 1, 0, 0, 0);

    treatment.addCovariate(std::make_unique<PatientCovariate>(
            "weight", Tucuxi::Common::Utils::varToString(70.0), DataType::Double, TucuUnit("kg"), t0));
    treatment.addCovariate(std::make_unique<PatientCovariate>(
            "creatinine", Tucuxi::Common::Utils::varToString(1.2), DataType::Double, TucuUnit("mg/dl"), t1));

    const PatientVariates& covariates = treatment.getCovariates();
    ASSERT_EQ(covariates.size(), 2u);
    EXPECT_EQ(covariates[0]->getId(), "weight");
    EXPECT_EQ(covariates[1]->getId(), "creatinine");
}


// ============================================================
// Targets
// ============================================================

TEST(Core_TestDrugTreatment, AddSingleTarget)
{
    // The target must be retrievable with the exact properties it was created with.
    DrugTreatment treatment;
    ActiveMoietyId moietyId("ibuprofen");
    TargetType type = TargetType::Residual;
    Value vmin = 2.0;
    Value vbest = 5.0;
    Value vmax = 10.0;

    treatment.addTarget(std::make_unique<Target>(moietyId, type, vmin, vbest, vmax));

    const Targets& targets = treatment.getTargets();
    ASSERT_EQ(targets.size(), 1u);
    EXPECT_EQ(targets[0]->getActiveMoietyId(), moietyId);
    EXPECT_EQ(targets[0]->getTargetType(), type);
    EXPECT_DOUBLE_EQ(targets[0]->getValueMin(), vmin);
    EXPECT_DOUBLE_EQ(targets[0]->getValueBest(), vbest);
    EXPECT_DOUBLE_EQ(targets[0]->getValueMax(), vmax);
}

TEST(Core_TestDrugTreatment, AddMultipleTargets)
{
    // All targets must be stored and appear in insertion order.
    DrugTreatment treatment;
    treatment.addTarget(std::make_unique<Target>(ActiveMoietyId("drugA"), TargetType::Residual, 1.0, 3.0, 5.0));
    treatment.addTarget(std::make_unique<Target>(ActiveMoietyId("drugB"), TargetType::Peak, 10.0, 15.0, 20.0));

    const Targets& targets = treatment.getTargets();
    ASSERT_EQ(targets.size(), 2u);
    EXPECT_EQ(targets[0]->getActiveMoietyId(), ActiveMoietyId("drugA"));
    EXPECT_EQ(targets[1]->getActiveMoietyId(), ActiveMoietyId("drugB"));
}

TEST(Core_TestDrugTreatment, TargetWithAlarmValues)
{
    // Inefficacy and toxicity alarms must be stored correctly.
    DrugTreatment treatment;
    treatment.addTarget(std::make_unique<Target>(
            ActiveMoietyId("drug"), TargetType::Residual, TucuUnit("mg/l"), 1.0, 4.0, 8.0, 0.5, 12.0));

    const Targets& targets = treatment.getTargets();
    ASSERT_EQ(targets.size(), 1u);
    EXPECT_DOUBLE_EQ(targets[0]->getInefficacyAlarm(), 0.5);
    EXPECT_DOUBLE_EQ(targets[0]->getToxicityAlarm(), 12.0);
}


// ============================================================
// Dosage history
// ============================================================

TEST(Core_TestDrugTreatment, DosageHistoryIsEmptyInitially)
{
    // isEmpty() must return true and the range count must be 0 on a fresh object.
    DrugTreatment treatment;
    EXPECT_TRUE(treatment.getDosageHistory().isEmpty());
    EXPECT_EQ(treatment.getDosageHistory().getNumberOfTimeRanges(), 0u);
}

TEST(Core_TestDrugTreatment, DosageHistoryStartOfTreatmentWhenEmpty)
{
    // getStartOfTreatment must return false when there are no dosage time ranges.
    DrugTreatment treatment;
    Tucuxi::Common::DateTime startDate;
    EXPECT_FALSE(treatment.getDosageHistory().getStartOfTreatment(startDate));
}

TEST(Core_TestDrugTreatment, AddDosageTimeRange)
{
    // After adding a time range the history must be non-empty with 1 time range.
    DrugTreatment treatment;
    Tucuxi::Common::DateTime start = DATE_TIME_NO_VAR(2021, 3, 1, 8, 0, 0);
    treatment.addDosageTimeRange(buildSimpleTimeRange(start));

    EXPECT_FALSE(treatment.getDosageHistory().isEmpty());
    EXPECT_EQ(treatment.getDosageHistory().getNumberOfTimeRanges(), 1u);
}

TEST(Core_TestDrugTreatment, DosageHistoryStartOfTreatment)
{
    // getStartOfTreatment must return the start date of the first added time range.
    DrugTreatment treatment;
    Tucuxi::Common::DateTime start = DATE_TIME_NO_VAR(2021, 3, 1, 8, 0, 0);
    treatment.addDosageTimeRange(buildSimpleTimeRange(start));

    Tucuxi::Common::DateTime treatmentStart;
    bool valid = treatment.getDosageHistory().getStartOfTreatment(treatmentStart);
    EXPECT_TRUE(valid);
    EXPECT_EQ(treatmentStart, start);
}

TEST(Core_TestDrugTreatment, AddTwoDosageTimeRanges)
{
    // Adding two non-overlapping sequential time ranges must result in 2 ranges.
    DrugTreatment treatment;
    Tucuxi::Common::DateTime start1 = DATE_TIME_NO_VAR(2021, 3, 1, 8, 0, 0);
    Tucuxi::Common::DateTime start2 = DATE_TIME_NO_VAR(2021, 3, 2, 8, 0, 0);

    treatment.addDosageTimeRange(buildSimpleTimeRange(start1));
    treatment.addDosageTimeRange(buildSimpleTimeRange(start2));

    EXPECT_EQ(treatment.getDosageHistory().getNumberOfTimeRanges(), 2u);
}

TEST(Core_TestDrugTreatment, DosageHistoryLastFormulationAndRoute)
{
    // getLastFormulationAndRoute must return the route of the most-recently added range.
    DrugTreatment treatment;
    Tucuxi::Common::DateTime start = DATE_TIME_NO_VAR(2021, 3, 1, 8, 0, 0);
    treatment.addDosageTimeRange(buildSimpleTimeRange(start));

    FormulationAndRoute expected(Formulation::Test, AdministrationRoute::IntravenousBolus);
    EXPECT_EQ(treatment.getDosageHistory().getLastFormulationAndRoute(), expected);
}

TEST(Core_TestDrugTreatment, GetModifiableDosageHistory)
{
    // Changes applied through getModifiableDosageHistory must be visible via
    // getDosageHistory, since both return a reference to the same object.
    DrugTreatment treatment;
    Tucuxi::Common::DateTime start = DATE_TIME_NO_VAR(2021, 4, 1, 8, 0, 0);

    auto timeRange = buildSimpleTimeRange(start);
    treatment.getModifiableDosageHistory().addTimeRange(*timeRange);

    EXPECT_FALSE(treatment.getDosageHistory().isEmpty());
    EXPECT_EQ(treatment.getDosageHistory().getNumberOfTimeRanges(), 1u);
}

TEST(Core_TestDrugTreatment, MergeDosageTimeRange)
{
    // Merging a later, non-overlapping time range must:
    //  1. trim the first range's end date to the second range's start,
    //  2. add the second range, resulting in exactly 2 time ranges.
    DrugTreatment treatment;
    Tucuxi::Common::DateTime start1 = DATE_TIME_NO_VAR(2021, 5, 1, 8, 0, 0);
    Tucuxi::Common::DateTime start2 = DATE_TIME_NO_VAR(2021, 5, 2, 8, 0, 0);

    treatment.addDosageTimeRange(buildSimpleTimeRange(start1));
    treatment.mergeDosageTimeRange(buildSimpleTimeRange(start2));

    EXPECT_EQ(treatment.getDosageHistory().getNumberOfTimeRanges(), 2u);
}

TEST(Core_TestDrugTreatment, MergeDosageTimeRangeReplacesOverlap)
{
    // When the new range starts before an existing range, the existing range
    // is removed and the new one takes its place (net result: 1 time range).
    DrugTreatment treatment;
    Tucuxi::Common::DateTime start1 = DATE_TIME_NO_VAR(2021, 5, 2, 8, 0, 0);
    Tucuxi::Common::DateTime start2 = DATE_TIME_NO_VAR(2021, 5, 1, 8, 0, 0); // earlier

    treatment.addDosageTimeRange(buildSimpleTimeRange(start1));
    treatment.mergeDosageTimeRange(buildSimpleTimeRange(start2));

    // start2 < start1, so the first range (start1) is removed and replaced by start2.
    EXPECT_EQ(treatment.getDosageHistory().getNumberOfTimeRanges(), 1u);
}
