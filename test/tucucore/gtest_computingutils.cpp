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

#include <date/date.h>
#include <gtest/gtest.h>

#include "tucucore/computingutils.h"
#include "tucucore/drugmodel/covariatedefinition.h"
#include "tucucore/generalextractor.h"

#include "testutils.h"

using namespace Tucuxi::Core;

TEST(Core_ComputingUtils, GetCovariateAtTimeSingleEventReturnsOnlyEvent)
{
    ComputingUtils utils;
    CovariateDefinition weightDefinition("Weight", "70", nullptr, CovariateType::Standard, DataType::Double);

    CovariateSeries series;
    series.emplace_back(weightDefinition, DATE_TIME_NO_VAR(2025, 1, 10, 8, 0, 0), 70.0);

    CovariateEvent resultBefore = utils.getCovariateAtTime(DATE_TIME_NO_VAR(2025, 1, 9, 8, 0, 0), series);
    CovariateEvent resultAfter = utils.getCovariateAtTime(DATE_TIME_NO_VAR(2025, 1, 11, 8, 0, 0), series);

    EXPECT_EQ(resultBefore.getId(), "Weight");
    EXPECT_DOUBLE_EQ(resultBefore.getValue(), 70.0);
    EXPECT_EQ(resultBefore.getEventTime(), DATE_TIME_NO_VAR(2025, 1, 10, 8, 0, 0));

    EXPECT_EQ(resultAfter.getId(), "Weight");
    EXPECT_DOUBLE_EQ(resultAfter.getValue(), 70.0);
    EXPECT_EQ(resultAfter.getEventTime(), DATE_TIME_NO_VAR(2025, 1, 10, 8, 0, 0));
}

TEST(Core_ComputingUtils, GetCovariateAtTimeKeepsFirstEventWhenDateEqualsSecondEvent)
{
    ComputingUtils utils;
    CovariateDefinition weightDefinition("Weight", "70", nullptr, CovariateType::Standard, DataType::Double);

    CovariateSeries series;
    series.emplace_back(weightDefinition, DATE_TIME_NO_VAR(2025, 1, 10, 8, 0, 0), 70.0);
    series.emplace_back(weightDefinition, DATE_TIME_NO_VAR(2025, 1, 11, 8, 0, 0), 71.0);

    CovariateEvent result = utils.getCovariateAtTime(DATE_TIME_NO_VAR(2025, 1, 11, 8, 0, 0), series);

    EXPECT_EQ(result.getId(), "Weight");
    EXPECT_DOUBLE_EQ(result.getValue(), 70.0);
    EXPECT_EQ(result.getEventTime(), DATE_TIME_NO_VAR(2025, 1, 10, 8, 0, 0));
}

TEST(Core_ComputingUtils, GetCovariateAtTimeReturnsLatestStrictlyBeforeGivenDate)
{
    ComputingUtils utils;
    CovariateDefinition weightDefinition("Weight", "70", nullptr, CovariateType::Standard, DataType::Double);

    CovariateSeries series;
    series.emplace_back(weightDefinition, DATE_TIME_NO_VAR(2025, 1, 10, 8, 0, 0), 70.0);
    series.emplace_back(weightDefinition, DATE_TIME_NO_VAR(2025, 1, 11, 8, 0, 0), 71.0);
    series.emplace_back(weightDefinition, DATE_TIME_NO_VAR(2025, 1, 12, 8, 0, 0), 72.0);

    CovariateEvent betweenResult = utils.getCovariateAtTime(DATE_TIME_NO_VAR(2025, 1, 11, 12, 0, 0), series);
    CovariateEvent afterLastResult = utils.getCovariateAtTime(DATE_TIME_NO_VAR(2025, 1, 13, 8, 0, 0), series);

    EXPECT_EQ(betweenResult.getId(), "Weight");
    EXPECT_DOUBLE_EQ(betweenResult.getValue(), 71.0);
    EXPECT_EQ(betweenResult.getEventTime(), DATE_TIME_NO_VAR(2025, 1, 11, 8, 0, 0));

    EXPECT_EQ(afterLastResult.getId(), "Weight");
    EXPECT_DOUBLE_EQ(afterLastResult.getValue(), 72.0);
    EXPECT_EQ(afterLastResult.getEventTime(), DATE_TIME_NO_VAR(2025, 1, 12, 8, 0, 0));
}
