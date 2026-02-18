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

#include <cmath>
#include <optional>
#include <vector>

#include <gtest/gtest.h>

#include "tucucommon/datetime.h"

#include "tucucore/computinggof.h"
#include "tucucore/computingservice/computingresponse.h"
#include "tucucore/definitions.h"

using namespace Tucuxi::Core;
using namespace date;


/// Exposes all protected static methods of ComputingGof for unit testing.
class ComputingGofExposer : public ComputingGof
{
public:
    using ComputingGof::computeGofStatistics;
    using ComputingGof::computeMae;
    using ComputingGof::computeMape;
    using ComputingGof::computeMeasurePredErrors;
    using ComputingGof::computeMse;
    using ComputingGof::computeRmse;
    using ComputingGof::computeRmsle;
    using ComputingGof::computeRSquared;
    using ComputingGof::findValueAt;
};


// ============================================================
// Helper
// ============================================================

/// Build a simple single-analyte cycle.
///   start  = 2018-09-01 08:00:00
///   end    = 2018-09-01 18:00:00
///   times  = {0, 2, 4, 6} hours from start
///   concs  = {10, 8, 6, 4}
static CycleData buildSimpleCycle()
{
    Tucuxi::Common::DateTime start(
            year_month_day(year(2018), month(9), day(1)), Tucuxi::Common::Duration(std::chrono::hours(8)));
    Tucuxi::Common::DateTime end(
            year_month_day(year(2018), month(9), day(1)), Tucuxi::Common::Duration(std::chrono::hours(18)));
    CycleData cycle(start, end, TucuUnit("mg/l"));
    TimeOffsets times = {0.0, 2.0, 4.0, 6.0};
    Concentrations concs = {10.0, 8.0, 6.0, 4.0};
    cycle.addData(times, concs);
    return cycle;
}


// ============================================================
// computeMae
// ============================================================

TEST(Core_TestComputingGof, MaeIdenticalValues)
{
    // When predicted == measured, MAE must be 0.
    std::vector<Value> computed = {1.0, 2.0, 3.0};
    std::vector<Value> measured = {1.0, 2.0, 3.0};
    EXPECT_DOUBLE_EQ(ComputingGofExposer::computeMae(computed, measured), 0.0);
}

TEST(Core_TestComputingGof, MaeKnownValues)
{
    // MAE = (|3-1| + |1-5|) / 2 = (2 + 4) / 2 = 3.0
    std::vector<Value> computed = {1.0, 5.0};
    std::vector<Value> measured = {3.0, 1.0};
    EXPECT_DOUBLE_EQ(ComputingGofExposer::computeMae(computed, measured), 3.0);
}

TEST(Core_TestComputingGof, MaeEmpty)
{
    std::vector<Value> computed;
    std::vector<Value> measured;
    EXPECT_DOUBLE_EQ(ComputingGofExposer::computeMae(computed, measured), 0.0);
}


// ============================================================
// computeMape
// ============================================================

TEST(Core_TestComputingGof, MapeIdenticalValues)
{
    std::vector<Value> computed = {1.0, 2.0, 3.0};
    std::vector<Value> measured = {1.0, 2.0, 3.0};
    EXPECT_DOUBLE_EQ(ComputingGofExposer::computeMape(computed, measured), 0.0);
}

TEST(Core_TestComputingGof, MapeKnownValues)
{
    // MAPE = (|2-1|/2 + |4-5|/4) / 2 = (0.5 + 0.25) / 2 = 0.375
    std::vector<Value> computed = {1.0, 5.0};
    std::vector<Value> measured = {2.0, 4.0};
    EXPECT_DOUBLE_EQ(ComputingGofExposer::computeMape(computed, measured), 0.375);
}

TEST(Core_TestComputingGof, MapeTinyMeasuredValue)
{
    // A near-zero measured value makes the denominator ~0: must return +infinity.
    std::vector<Value> computed = {1.0};
    std::vector<Value> measured = {0.0};
    Value const mape = ComputingGofExposer::computeMape(computed, measured);
    EXPECT_TRUE(std::isinf(mape) && mape > 0.0);
}

TEST(Core_TestComputingGof, MapeEmpty)
{
    std::vector<Value> computed;
    std::vector<Value> measured;
    EXPECT_DOUBLE_EQ(ComputingGofExposer::computeMape(computed, measured), 0.0);
}


// ============================================================
// computeMse
// ============================================================

TEST(Core_TestComputingGof, MseIdenticalValues)
{
    std::vector<Value> computed = {1.0, 2.0, 3.0};
    std::vector<Value> measured = {1.0, 2.0, 3.0};
    EXPECT_DOUBLE_EQ(ComputingGofExposer::computeMse(computed, measured), 0.0);
}

TEST(Core_TestComputingGof, MseKnownValues)
{
    // MSE = ((3-1)^2 + (1-5)^2) / 2 = (4 + 16) / 2 = 10.0
    std::vector<Value> computed = {1.0, 5.0};
    std::vector<Value> measured = {3.0, 1.0};
    EXPECT_DOUBLE_EQ(ComputingGofExposer::computeMse(computed, measured), 10.0);
}

TEST(Core_TestComputingGof, MseEmpty)
{
    std::vector<Value> computed;
    std::vector<Value> measured;
    EXPECT_DOUBLE_EQ(ComputingGofExposer::computeMse(computed, measured), 0.0);
}


// ============================================================
// computeRmse
// ============================================================

TEST(Core_TestComputingGof, RmseIdenticalValues)
{
    std::vector<Value> computed = {1.0, 2.0, 3.0};
    std::vector<Value> measured = {1.0, 2.0, 3.0};
    EXPECT_DOUBLE_EQ(ComputingGofExposer::computeRmse(computed, measured), 0.0);
}

TEST(Core_TestComputingGof, RmseKnownValues)
{
    // RMSE = sqrt(MSE) = sqrt(10)
    std::vector<Value> computed = {1.0, 5.0};
    std::vector<Value> measured = {3.0, 1.0};
    EXPECT_NEAR(ComputingGofExposer::computeRmse(computed, measured), std::sqrt(10.0), 1e-12);
}

TEST(Core_TestComputingGof, RmseEmpty)
{
    std::vector<Value> computed;
    std::vector<Value> measured;
    EXPECT_DOUBLE_EQ(ComputingGofExposer::computeRmse(computed, measured), 0.0);
}


// ============================================================
// computeRmsle
// ============================================================

TEST(Core_TestComputingGof, RmsleIdenticalValues)
{
    std::vector<Value> computed = {1.0, 2.0, 3.0};
    std::vector<Value> measured = {1.0, 2.0, 3.0};
    EXPECT_DOUBLE_EQ(ComputingGofExposer::computeRmsle(computed, measured), 0.0);
}

TEST(Core_TestComputingGof, RmsleZeroValues)
{
    // log(0 + 1) = 0, so two zero vectors give RMSLE = 0.
    std::vector<Value> computed = {0.0, 0.0};
    std::vector<Value> measured = {0.0, 0.0};
    EXPECT_DOUBLE_EQ(ComputingGofExposer::computeRmsle(computed, measured), 0.0);
}

TEST(Core_TestComputingGof, RmsleEmpty)
{
    std::vector<Value> computed;
    std::vector<Value> measured;
    EXPECT_DOUBLE_EQ(ComputingGofExposer::computeRmsle(computed, measured), 0.0);
}

TEST(Core_TestComputingGof, RmsleSingleKnownPair)
{
    // logDiff = log(3+1) - log(1+1) = log(4) - log(2) = log(2); RMSLE = log(2)
    std::vector<Value> computed = {3.0};
    std::vector<Value> measured = {1.0};
    Value const expected = std::log(4.0) - std::log(2.0);
    EXPECT_NEAR(ComputingGofExposer::computeRmsle(computed, measured), expected, 1e-12);
}

TEST(Core_TestComputingGof, RmsleMultipleKnownPairs)
{
    // computed = {2, 4}, measured = {1, 3}
    // d1 = log(3) - log(2), d2 = log(5) - log(4)
    // RMSLE = sqrt((d1^2 + d2^2) / 2)
    std::vector<Value> computed = {2.0, 4.0};
    std::vector<Value> measured = {1.0, 3.0};
    double const d1 = std::log(3.0) - std::log(2.0);
    double const d2 = std::log(5.0) - std::log(4.0);
    Value const expected = std::sqrt((d1 * d1 + d2 * d2) / 2.0);
    EXPECT_NEAR(ComputingGofExposer::computeRmsle(computed, measured), expected, 1e-12);
}

TEST(Core_TestComputingGof, RmsleNegativeComputedValue)
{
    // A negative predicted value makes log undefined: must return +infinity.
    std::vector<Value> computed = {-1.0};
    std::vector<Value> measured = {1.0};
    Value const rmsle = ComputingGofExposer::computeRmsle(computed, measured);
    EXPECT_TRUE(std::isinf(rmsle) && rmsle > 0.0);
}

TEST(Core_TestComputingGof, RmsleNegativeMeasuredValue)
{
    // A negative measured value makes log undefined: must return +infinity.
    std::vector<Value> computed = {1.0};
    std::vector<Value> measured = {-1.0};
    Value const rmsle = ComputingGofExposer::computeRmsle(computed, measured);
    EXPECT_TRUE(std::isinf(rmsle) && rmsle > 0.0);
}


// ============================================================
// computeRSquared
// ============================================================

TEST(Core_TestComputingGof, RSquaredPerfectFit)
{
    // Perfect prediction: R² = 1.
    std::vector<Value> computed = {1.0, 2.0, 3.0};
    std::vector<Value> measured = {1.0, 2.0, 3.0};
    EXPECT_DOUBLE_EQ(ComputingGofExposer::computeRSquared(computed, measured), 1.0);
}

TEST(Core_TestComputingGof, RSquaredKnownValues)
{
    // computed = {1.5, 2.5}, measured = {1, 3}
    // mean = 2; SS_res = 0.5; SS_tot = 2; R² = 1 - 0.5/2 = 0.75
    std::vector<Value> computed = {1.5, 2.5};
    std::vector<Value> measured = {1.0, 3.0};
    EXPECT_NEAR(ComputingGofExposer::computeRSquared(computed, measured), 0.75, 1e-12);
}

TEST(Core_TestComputingGof, RSquaredZeroVariancePerfectFit)
{
    // Constant measured values, perfect prediction: both SS ~0 → R² = 1.
    std::vector<Value> computed = {2.0, 2.0};
    std::vector<Value> measured = {2.0, 2.0};
    EXPECT_DOUBLE_EQ(ComputingGofExposer::computeRSquared(computed, measured), 1.0);
}

TEST(Core_TestComputingGof, RSquaredZeroVarianceBadFit)
{
    // Constant measured values but computed differ: SS_tot ~0, SS_res > 0 → R² = -inf.
    std::vector<Value> computed = {1.0, 3.0};
    std::vector<Value> measured = {2.0, 2.0};
    Value const rSquared = ComputingGofExposer::computeRSquared(computed, measured);
    EXPECT_TRUE(std::isinf(rSquared) && rSquared < 0.0);
}


// ============================================================
// computeMeasurePredErrors
// ============================================================

TEST(Core_TestComputingGof, MeasurePredErrorsSinglePair)
{
    // computed = {3}, measured = {1}
    // predError = 1 - 3 = -2
    // absPredErrorPct = |-2 / 3| * 100 = 200/3
    std::vector<Value> computed = {3.0};
    std::vector<Value> measured = {1.0};
    Value meanPredError = 0.0;
    Value meanAbsPredError = 0.0;
    auto errors = ComputingGofExposer::computeMeasurePredErrors(computed, measured, meanPredError, meanAbsPredError);

    ASSERT_EQ(errors.size(), 1u);
    EXPECT_DOUBLE_EQ(errors[0].getMeasure(), 1.0);
    EXPECT_DOUBLE_EQ(errors[0].getPrediction(), 3.0);
    EXPECT_NEAR(errors[0].getPredictionError(), -2.0, 1e-12);
    EXPECT_NEAR(errors[0].getAbsPredErrorPct(), 200.0 / 3.0, 1e-9);
    EXPECT_NEAR(meanPredError, -2.0, 1e-12);
    EXPECT_NEAR(meanAbsPredError, 200.0 / 3.0, 1e-9);
}

TEST(Core_TestComputingGof, MeasurePredErrorsMultiplePairs)
{
    // computed = {1, 5}, measured = {3, 1}
    // pe1: predError = 2,  absPct = |2/1| * 100 = 200
    // pe2: predError = -4, absPct = |-4/5| * 100 = 80
    // mean = (2-4)/2 = -1,  meanAbs = (200+80)/2 = 140
    std::vector<Value> computed = {1.0, 5.0};
    std::vector<Value> measured = {3.0, 1.0};
    Value meanPredError = 0.0;
    Value meanAbsPredError = 0.0;
    auto errors = ComputingGofExposer::computeMeasurePredErrors(computed, measured, meanPredError, meanAbsPredError);

    ASSERT_EQ(errors.size(), 2u);
    EXPECT_NEAR(errors[0].getPredictionError(), 2.0, 1e-12);
    EXPECT_NEAR(errors[0].getAbsPredErrorPct(), 200.0, 1e-9);
    EXPECT_NEAR(errors[1].getPredictionError(), -4.0, 1e-12);
    EXPECT_NEAR(errors[1].getAbsPredErrorPct(), 80.0, 1e-9);
    EXPECT_NEAR(meanPredError, -1.0, 1e-12);
    EXPECT_NEAR(meanAbsPredError, 140.0, 1e-9);
}

TEST(Core_TestComputingGof, MeasurePredErrorsTinyComputed)
{
    // Near-zero computed value: absPredErrorPct must be +infinity.
    std::vector<Value> computed = {0.0};
    std::vector<Value> measured = {1.0};
    Value meanPredError = 0.0;
    Value meanAbsPredError = 0.0;
    auto errors = ComputingGofExposer::computeMeasurePredErrors(computed, measured, meanPredError, meanAbsPredError);

    ASSERT_EQ(errors.size(), 1u);
    EXPECT_TRUE(std::isinf(errors[0].getAbsPredErrorPct()) && errors[0].getAbsPredErrorPct() > 0.0);
    EXPECT_TRUE(std::isinf(meanAbsPredError) && meanAbsPredError > 0.0);
}


// ============================================================
// computeGofStatistics
// ============================================================

TEST(Core_TestComputingGof, GofStatisticsPopulatesOptional)
{
    // Verify the output optional is populated after a successful call.
    std::vector<Value> computed = {1.0, 3.0};
    std::vector<Value> measured = {2.0, 4.0};
    std::optional<GofData> gofData;
    ComputingGofExposer::computeGofStatistics(computed, measured, gofData);
    EXPECT_TRUE(gofData.has_value());
}

TEST(Core_TestComputingGof, GofStatisticsKnownValues)
{
    // computed = {1, 3}, measured = {2, 4}
    // MAE  = 1.0,   MAPE = 0.375,  MSE = 1.0,  RMSE = 1.0
    // R²   = 0.0  (SS_res = SS_tot = 2)
    // meanPredictionError = 1.0
    std::vector<Value> computed = {1.0, 3.0};
    std::vector<Value> measured = {2.0, 4.0};
    std::optional<GofData> gofData;
    ComputingGofExposer::computeGofStatistics(computed, measured, gofData);

    ASSERT_TRUE(gofData.has_value());
    EXPECT_DOUBLE_EQ(gofData->getMae(), 1.0);
    EXPECT_DOUBLE_EQ(gofData->getMape(), 0.375);
    EXPECT_DOUBLE_EQ(gofData->getMse(), 1.0);
    EXPECT_DOUBLE_EQ(gofData->getRmse(), 1.0);
    EXPECT_GE(gofData->getRmsle(), 0.0);
    EXPECT_NEAR(gofData->getRSquared(), 0.0, 1e-12);
    ASSERT_EQ(gofData->getPredErrors().size(), 2u);
    EXPECT_NEAR(gofData->getMeanPredictionError(), 1.0, 1e-12);
}

TEST(Core_TestComputingGof, GofStatisticsPerfectFit)
{
    // Perfect fit: all error metrics are 0, R² = 1.
    std::vector<Value> computed = {1.0, 2.0, 3.0};
    std::vector<Value> measured = {1.0, 2.0, 3.0};
    std::optional<GofData> gofData;
    ComputingGofExposer::computeGofStatistics(computed, measured, gofData);

    ASSERT_TRUE(gofData.has_value());
    EXPECT_DOUBLE_EQ(gofData->getMae(), 0.0);
    EXPECT_DOUBLE_EQ(gofData->getMse(), 0.0);
    EXPECT_DOUBLE_EQ(gofData->getRmse(), 0.0);
    EXPECT_DOUBLE_EQ(gofData->getRmsle(), 0.0);
    EXPECT_DOUBLE_EQ(gofData->getRSquared(), 1.0);
    EXPECT_NEAR(gofData->getMeanPredictionError(), 0.0, 1e-12);
}


// ============================================================
// findValueAt
// ============================================================

TEST(Core_TestComputingGof, FindValueAtEmptyCycles)
{
    // No cycles at all: must return nullopt.
    std::vector<CycleData> cycles;
    Tucuxi::Common::TucuUnit unit;
    Tucuxi::Common::DateTime queryTime(
            year_month_day(year(2018), month(9), day(1)), Tucuxi::Common::Duration(std::chrono::hours(10)));
    EXPECT_FALSE(ComputingGofExposer::findValueAt(cycles, queryTime, unit).has_value());
}

TEST(Core_TestComputingGof, FindValueAtBeforeStart)
{
    // 07:00:00 is before cycle start (08:00:00): must return nullopt.
    auto cycle = buildSimpleCycle();
    Tucuxi::Common::TucuUnit unit;
    Tucuxi::Common::DateTime queryTime(
            year_month_day(year(2018), month(9), day(1)), Tucuxi::Common::Duration(std::chrono::hours(7)));
    EXPECT_FALSE(ComputingGofExposer::findValueAt({cycle}, queryTime, unit).has_value());
}

TEST(Core_TestComputingGof, FindValueAtAfterEnd)
{
    // 18:30:00 is after cycle end (18:00:00): must return nullopt.
    auto cycle = buildSimpleCycle();
    Tucuxi::Common::TucuUnit unit;
    Tucuxi::Common::DateTime queryTime(
            year_month_day(year(2018), month(9), day(1)),
            Tucuxi::Common::Duration(std::chrono::hours(18), std::chrono::minutes(30), std::chrono::seconds(0)));
    EXPECT_FALSE(ComputingGofExposer::findValueAt({cycle}, queryTime, unit).has_value());
}

TEST(Core_TestComputingGof, FindValueAtFirstPoint)
{
    // 08:00:00 = offset 0.0 h: lower_bound lands on begin → Case A → front = 10.0
    auto cycle = buildSimpleCycle();
    Tucuxi::Common::TucuUnit unit;
    Tucuxi::Common::DateTime queryTime(
            year_month_day(year(2018), month(9), day(1)), Tucuxi::Common::Duration(std::chrono::hours(8)));
    auto result = ComputingGofExposer::findValueAt({cycle}, queryTime, unit);
    ASSERT_TRUE(result.has_value());
    EXPECT_DOUBLE_EQ(result.value(), 10.0);
}

TEST(Core_TestComputingGof, FindValueAtExactInteriorMatch)
{
    // 10:00:00 = offset 2.0 h: exact match → Case C → concs[1] = 8.0
    auto cycle = buildSimpleCycle();
    Tucuxi::Common::TucuUnit unit;
    Tucuxi::Common::DateTime queryTime(
            year_month_day(year(2018), month(9), day(1)), Tucuxi::Common::Duration(std::chrono::hours(10)));
    auto result = ComputingGofExposer::findValueAt({cycle}, queryTime, unit);
    ASSERT_TRUE(result.has_value());
    EXPECT_DOUBLE_EQ(result.value(), 8.0);
}

TEST(Core_TestComputingGof, FindValueAtLinearInterpolation)
{
    // 08:30:00 = offset 0.5 h, between times[0]=0 and times[1]=2
    // alpha = 0.5/2 = 0.25; result = 10 + 0.25*(8-10) = 9.5
    auto cycle = buildSimpleCycle();
    Tucuxi::Common::TucuUnit unit;
    Tucuxi::Common::DateTime queryTime(
            year_month_day(year(2018), month(9), day(1)),
            Tucuxi::Common::Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0)));
    auto result = ComputingGofExposer::findValueAt({cycle}, queryTime, unit);
    ASSERT_TRUE(result.has_value());
    EXPECT_NEAR(result.value(), 9.5, 1e-9);
}

TEST(Core_TestComputingGof, FindValueAtBeyondLastPoint)
{
    // 15:00:00 = offset 7.0 h, within cycle but past last time (6.0) → Case B → back = 4.0
    auto cycle = buildSimpleCycle();
    Tucuxi::Common::TucuUnit unit;
    Tucuxi::Common::DateTime queryTime(
            year_month_day(year(2018), month(9), day(1)), Tucuxi::Common::Duration(std::chrono::hours(15)));
    auto result = ComputingGofExposer::findValueAt({cycle}, queryTime, unit);
    ASSERT_TRUE(result.has_value());
    EXPECT_DOUBLE_EQ(result.value(), 4.0);
}

TEST(Core_TestComputingGof, FindValueAtEmptyCycleData)
{
    // Cycle has no addData() call: m_times.empty() → nullopt.
    Tucuxi::Common::DateTime start(
            year_month_day(year(2018), month(9), day(1)), Tucuxi::Common::Duration(std::chrono::hours(8)));
    Tucuxi::Common::DateTime end(
            year_month_day(year(2018), month(9), day(1)), Tucuxi::Common::Duration(std::chrono::hours(18)));
    CycleData cycle(start, end, TucuUnit("mg/l"));
    Tucuxi::Common::TucuUnit unit;
    Tucuxi::Common::DateTime queryTime(
            year_month_day(year(2018), month(9), day(1)), Tucuxi::Common::Duration(std::chrono::hours(10)));
    EXPECT_FALSE(ComputingGofExposer::findValueAt({cycle}, queryTime, unit).has_value());
}

TEST(Core_TestComputingGof, FindValueAtEmptyTimeOffsets)
{
    // Cycle has addData({}, {}): inner TimeOffsets is empty → times.empty() → nullopt.
    Tucuxi::Common::DateTime start(
            year_month_day(year(2018), month(9), day(1)), Tucuxi::Common::Duration(std::chrono::hours(8)));
    Tucuxi::Common::DateTime end(
            year_month_day(year(2018), month(9), day(1)), Tucuxi::Common::Duration(std::chrono::hours(18)));
    CycleData cycle(start, end, TucuUnit("mg/l"));
    cycle.addData(TimeOffsets{}, Concentrations{});
    Tucuxi::Common::TucuUnit unit;
    Tucuxi::Common::DateTime queryTime(
            year_month_day(year(2018), month(9), day(1)), Tucuxi::Common::Duration(std::chrono::hours(10)));
    EXPECT_FALSE(ComputingGofExposer::findValueAt({cycle}, queryTime, unit).has_value());
}

TEST(Core_TestComputingGof, FindValueAtWrongAnalyteIndex)
{
    // Cycle has only analyte index 0; querying index 1 → nullopt.
    auto cycle = buildSimpleCycle();
    Tucuxi::Common::TucuUnit unit;
    Tucuxi::Common::DateTime queryTime(
            year_month_day(year(2018), month(9), day(1)), Tucuxi::Common::Duration(std::chrono::hours(10)));
    EXPECT_FALSE(ComputingGofExposer::findValueAt({cycle}, queryTime, unit, 1).has_value());
}
