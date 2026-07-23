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

#include "tucucommon/datetime.h"

#include "tucucore/computingcomponent.h"
#include "tucucore/computingservice/computingrequest.h"
#include "tucucore/computingservice/computingresponse.h"
#include "tucucore/computingservice/computingtrait.h"
#include "tucucore/drugmodel/drugmodel.h"
#include "tucucore/drugtreatment/drugtreatment.h"
#include "tucucore/drugtreatment/sample.h"

#include "computingcomponentfactory.h"
#include "drugmodels/buildimatinib.h"
#include "gtest_core.h"

using namespace Tucuxi::Core;

using namespace std::chrono_literals;
using namespace date;

TEST(Core_TestComputingComponentEtoda, ImatinibEtoda)
{
    auto component = ComputingComponentFactory::createComputingService();
    ASSERT_TRUE(component != nullptr);

    BuildImatinib builder;
    auto drugModel = builder.buildDrugModel();
    ASSERT_TRUE(drugModel != nullptr);

    const FormulationAndRoute route(Formulation::OralSolution, AdministrationRoute::Oral);
    DateTime startSept2018(
            date::year_month_day(date::year(2018), date::month(9), date::day(1)),
            Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));
    auto drugTreatment = buildDrugTreatment(route, startSept2018);

    RequestResponseId requestResponseId = "1";
    Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
    Tucuxi::Common::DateTime end(2018_y / sep / 2, 8h + 0min);
    Tucuxi::Common::DateTime adjustmentEnd(2018_y / sep / 8, 8h + 0min);
    Tucuxi::Common::DateTime sampleDate(2018_y / sep / 5, 8h + 0min);
    double nbPointsPerHour = 20.0;
    TimeOffsets samplingHours({2.0, 4.0});
    int nbConcentrationPoints = 10;
    PercentileRanks ranks({1.0, 99.0});
    ComputingOption computingOption(PredictionParameterType::Population, CompartmentsOption::MainCompartment);

    std::unique_ptr<ComputingTraitEtoda> traits = std::make_unique<ComputingTraitEtoda>(
            requestResponseId,
            start,
            end,
            nbPointsPerHour,
            computingOption,
            adjustmentEnd,
            sampleDate,
            samplingHours,
            nbConcentrationPoints,
            ranks);

    ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(traits));
    std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

    ComputingStatus result = component->compute(request, response);

    ASSERT_EQ(result, ComputingStatus::Ok);

    const ComputedData* responseData = response->getData();
    ASSERT_TRUE(dynamic_cast<const EtodaData*>(responseData) != nullptr);
    const auto* etodaData = dynamic_cast<const EtodaData*>(responseData);

    const auto& hourResults = etodaData->getEtodaResults();
    ASSERT_EQ(hourResults.size(), static_cast<size_t>(2));

    // Default ETODA options use two sampling hours and a 10x10 concentration grid.
    ASSERT_DOUBLE_EQ(hourResults[0].m_samplingHour, 2.0);
    ASSERT_DOUBLE_EQ(hourResults[1].m_samplingHour, 4.0);
    ASSERT_EQ(hourResults[0].m_points.size(), static_cast<size_t>(100));
    ASSERT_EQ(hourResults[1].m_points.size(), static_cast<size_t>(100));

    // Spot-check that generated points correspond to the current sampling hour.
    ASSERT_DOUBLE_EQ(hourResults[0].m_points[0].m_samplingHour, hourResults[0].m_samplingHour);
    ASSERT_DOUBLE_EQ(hourResults[1].m_points[0].m_samplingHour, hourResults[1].m_samplingHour);
}

TEST(Core_TestComputingComponentEtoda, ImatinibEtodaFromAdjustmentRequest)
{
    auto component = ComputingComponentFactory::createComputingService();
    ASSERT_TRUE(component != nullptr);

    BuildImatinib builder;
    auto drugModel = builder.buildDrugModel();
    ASSERT_TRUE(drugModel != nullptr);

    const FormulationAndRoute route(Formulation::OralSolution, AdministrationRoute::Oral);
    DateTime startSept2018(
            date::year_month_day(date::year(2018), date::month(9), date::day(1)),
            Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));
    auto drugTreatment = buildDrugTreatment(route, startSept2018);

    // ETODA over adjustments requires at least one measured sample.
    drugTreatment->addSample(std::make_unique<Sample>(
            Tucuxi::Common::DateTime(2018_y / sep / 4, 8h + 0min),
            AnalyteId(drugModel->getDrugId()),
            1000.0,
            TucuUnit("ug/l")));

    RequestResponseId requestResponseId = "2";
    Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
    Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
    double nbPointsPerHour = 10.0;
    ComputingOption computingOption(PredictionParameterType::Aposteriori, CompartmentsOption::MainCompartment);
    Tucuxi::Common::DateTime adjustmentTime(2018_y / sep / 4, 8h + 0min);

    auto traits = std::make_unique<ComputingTraitAdjustment>(
            requestResponseId,
            start,
            end,
            nbPointsPerHour,
            computingOption,
            adjustmentTime,
            BestCandidatesOption::BestDosage,
            LoadingOption::NoLoadingDose,
            RestPeriodOption::NoRestPeriod,
            SteadyStateTargetOption::WithinTreatmentTimeRange,
            TargetExtractionOption::PopulationValues,
            FormulationAndRouteSelectionOption::LastFormulationAndRoute,
            AdjustmentWithCurrentDosageOption::AlwaysAdjust,
            AdjustmentWithEtodaOption::WithEtoda);

    ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(traits));
    auto response = std::make_unique<ComputingResponse>(requestResponseId);

    ComputingStatus result = component->compute(request, response);
    ASSERT_EQ(result, ComputingStatus::Ok);

    const auto* responseData = response->getData();
    ASSERT_TRUE(dynamic_cast<const AdjustmentData*>(responseData) != nullptr);
    const auto* adjustmentData = dynamic_cast<const AdjustmentData*>(responseData);

    ASSERT_FALSE(adjustmentData->getAdjustments().empty());

    const auto& bestAdjustment = adjustmentData->getAdjustments().front();
    ASSERT_TRUE(bestAdjustment.getEtodaData().has_value());

    const auto& etodaResults = bestAdjustment.getEtodaData()->getEtodaResults();
    ASSERT_EQ(etodaResults.size(), static_cast<size_t>(1));
    ASSERT_DOUBLE_EQ(etodaResults[0].m_samplingHour, 0.0);
    ASSERT_EQ(etodaResults[0].m_points.size(), static_cast<size_t>(10));
}
