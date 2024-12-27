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

#include "drugmodels/buildgentamicinfuchs2014.h"
#include "drugmodels/buildimatinib.h"
#include "gtest_core.h"

using namespace Tucuxi::Core;

using namespace std::chrono_literals;
using namespace date;

TEST(Core_TestComputingComponentAdjusements, ImatinibLastFormulationAndRouteAllDosages)
{
    IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

    ASSERT_TRUE(component != nullptr);

    BuildImatinib builder;
    auto drugModel = builder.buildDrugModel();
    ASSERT_TRUE(drugModel != nullptr);

    const FormulationAndRoute route(
            Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);

    DateTime startSept2018(
            date::year_month_day(date::year(2018), date::month(9), date::day(1)),
            Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));

    auto drugTreatment = buildDrugTreatment(route, startSept2018);

    // Construct the adjustment traits object
    RequestResponseId requestResponseId = "1";
    Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
    Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
    double nbPointsPerHour = 10.0;
    ComputingOption computingOption(PredictionParameterType::Population, CompartmentsOption::MainCompartment);
    Tucuxi::Common::DateTime adjustmentTime(2018_y / sep / 4, 8h + 0min);
    BestCandidatesOption adjustmentOption = BestCandidatesOption::AllDosages;
    std::unique_ptr<ComputingTraitAdjustment> adjustmentsTraits = std::make_unique<ComputingTraitAdjustment>(
            requestResponseId,
            start,
            end,
            nbPointsPerHour,
            computingOption,
            adjustmentTime,
            adjustmentOption,
            LoadingOption::NoLoadingDose,
            RestPeriodOption::NoRestPeriod,
            SteadyStateTargetOption::WithinTreatmentTimeRange,
            TargetExtractionOption::PopulationValues,
            FormulationAndRouteSelectionOption::LastFormulationAndRoute);

    ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(adjustmentsTraits));

    std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

    ComputingStatus result;
    result = component->compute(request, response);

    ASSERT_EQ(result, ComputingStatus::Ok);

    const ComputedData* responseData = response->getData();
    ASSERT_TRUE(dynamic_cast<const AdjustmentData*>(responseData) != nullptr);
    const AdjustmentData* resp = dynamic_cast<const AdjustmentData*>(responseData);

    // We expect 7 valid adjustment candidates
    ASSERT_EQ(resp->getAdjustments().size(), static_cast<size_t>(7));

    for (auto const& adj : resp->getAdjustments()) {
        ASSERT_EQ(adj.m_history.getDosageTimeRanges()[0]->getStartDate(), adjustmentTime);
        ASSERT_TRUE(adj.m_history.getLastFormulationAndRoute().isCompatible(route));
    }

    ASSERT_TRUE(resp->isCurrentInRange());
    ASSERT_LT(resp->getCurrentDosageWithScore().getGlobalScore(), 1.0);
    ASSERT_GT(resp->getCurrentDosageWithScore().getGlobalScore(), 0.0);
    ASSERT_EQ(resp->getCurrentDosageWithScore().m_targetsEvaluation.size(), 1);
    ASSERT_LT(resp->getCurrentDosageWithScore().m_targetsEvaluation[0].getValue(), 1000);

    // Delete all dynamically allocated objects
    delete component;
}

TEST(Core_TestComputingComponentAdjusements, ImatinibDefaultFormulationAndRouteAllDosages)
{
    IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

    ASSERT_TRUE(component != nullptr);

    BuildImatinib builder;
    auto drugModel = builder.buildDrugModel();
    ASSERT_TRUE(drugModel != nullptr);

    const FormulationAndRoute route(
            Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);

    DateTime startSept2018(
            date::year_month_day(date::year(2018), date::month(9), date::day(1)),
            Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));

    auto drugTreatment = buildDrugTreatment(route, startSept2018);

    // Construct the adjustment traits object
    RequestResponseId requestResponseId = "1";
    Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
    Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
    double nbPointsPerHour = 10.0;
    ComputingOption computingOption(PredictionParameterType::Population, CompartmentsOption::MainCompartment);
    Tucuxi::Common::DateTime adjustmentTime(2018_y / sep / 4, 8h + 0min);
    BestCandidatesOption adjustmentOption = BestCandidatesOption::AllDosages;
    std::unique_ptr<ComputingTraitAdjustment> adjustmentsTraits = std::make_unique<ComputingTraitAdjustment>(
            requestResponseId,
            start,
            end,
            nbPointsPerHour,
            computingOption,
            adjustmentTime,
            adjustmentOption,
            LoadingOption::NoLoadingDose,
            RestPeriodOption::NoRestPeriod,
            SteadyStateTargetOption::WithinTreatmentTimeRange,
            TargetExtractionOption::PopulationValues,
            FormulationAndRouteSelectionOption::DefaultFormulationAndRoute);

    ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(adjustmentsTraits));

    std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

    ComputingStatus result;
    result = component->compute(request, response);

    ASSERT_EQ(result, ComputingStatus::Ok);


    const ComputedData* responseData = response->getData();
    ASSERT_TRUE(dynamic_cast<const AdjustmentData*>(responseData) != nullptr);
    const AdjustmentData* resp = dynamic_cast<const AdjustmentData*>(responseData);

    // We expect 7 valid adjustment candidates
    ASSERT_EQ(resp->getAdjustments().size(), static_cast<size_t>(7));

    for (auto const& adj : resp->getAdjustments()) {
        ASSERT_EQ(
                adj.m_history.getLastFormulationAndRoute(),
                drugModel->getFormulationAndRoutes().getDefault()->getFormulationAndRoute());
    }

    // Delete all dynamically allocated objects
    delete component;
}

TEST(Core_TestComputingComponentAdjusements, DISABLED_ImatinibAllFormulationAndRouteAllDosages)
{
    // TODO : Check this test. It does not work anymore because of multiple routes not supported by the PK model

    IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

    ASSERT_TRUE(component != nullptr);

    BuildImatinib builder;
    auto drugModel = builder.buildDrugModel();
    ASSERT_TRUE(drugModel != nullptr);

    const FormulationAndRoute route(
            Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);

    DateTime startSept2018(
            date::year_month_day(date::year(2018), date::month(9), date::day(1)),
            Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));

    auto drugTreatment = buildDrugTreatment(route, startSept2018);


    // Construct the adjustment traits object
    RequestResponseId requestResponseId = "1";
    Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
    Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
    double nbPointsPerHour = 10.0;
    ComputingOption computingOption(PredictionParameterType::Population, CompartmentsOption::MainCompartment);
    Tucuxi::Common::DateTime adjustmentTime(2018_y / sep / 4, 8h + 0min);
    BestCandidatesOption adjustmentOption = BestCandidatesOption::AllDosages;
    std::unique_ptr<ComputingTraitAdjustment> adjustmentsTraits = std::make_unique<ComputingTraitAdjustment>(
            requestResponseId,
            start,
            end,
            nbPointsPerHour,
            computingOption,
            adjustmentTime,
            adjustmentOption,
            LoadingOption::NoLoadingDose,
            RestPeriodOption::NoRestPeriod,
            SteadyStateTargetOption::WithinTreatmentTimeRange,
            TargetExtractionOption::PopulationValues,
            FormulationAndRouteSelectionOption::AllFormulationAndRoutes);

    ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(adjustmentsTraits));

    std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

    ComputingStatus result;
    result = component->compute(request, response);

    ASSERT_EQ(result, ComputingStatus::Ok);


    const ComputedData* responseData = response->getData();
    ASSERT_TRUE(dynamic_cast<const AdjustmentData*>(responseData) != nullptr);
    const AdjustmentData* resp = dynamic_cast<const AdjustmentData*>(responseData);

    // We expect 7 valid adjustment candidates
    ASSERT_EQ(resp->getAdjustments().size(), static_cast<size_t>(7));

    // Delete all dynamically allocated objects
    delete component;
}

TEST(Core_TestComputingComponentAdjusements, ImatinibLastFormulationAndRouteBestDosage)
{
    IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

    ASSERT_TRUE(component != nullptr);

    BuildImatinib builder;
    auto drugModel = builder.buildDrugModel();
    ASSERT_TRUE(drugModel != nullptr);

    const FormulationAndRoute route(
            Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);

    DateTime startSept2018(
            date::year_month_day(date::year(2018), date::month(9), date::day(1)),
            Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));

    auto drugTreatment = buildDrugTreatment(route, startSept2018);

    // Construct the adjustment traits object
    RequestResponseId requestResponseId = "1";
    Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
    Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
    double nbPointsPerHour = 10.0;
    ComputingOption computingOption(PredictionParameterType::Population, CompartmentsOption::MainCompartment);
    Tucuxi::Common::DateTime adjustmentTime(2018_y / sep / 4, 8h + 0min);
    BestCandidatesOption adjustmentOption = BestCandidatesOption::BestDosage;
    std::unique_ptr<ComputingTraitAdjustment> adjustmentsTraits = std::make_unique<ComputingTraitAdjustment>(
            requestResponseId,
            start,
            end,
            nbPointsPerHour,
            computingOption,
            adjustmentTime,
            adjustmentOption,
            LoadingOption::NoLoadingDose,
            RestPeriodOption::NoRestPeriod,
            SteadyStateTargetOption::WithinTreatmentTimeRange,
            TargetExtractionOption::PopulationValues,
            FormulationAndRouteSelectionOption::LastFormulationAndRoute);

    ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(adjustmentsTraits));

    std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

    ComputingStatus result;
    result = component->compute(request, response);

    ASSERT_EQ(result, ComputingStatus::Ok);


    const ComputedData* responseData = response->getData();
    ASSERT_TRUE(dynamic_cast<const AdjustmentData*>(responseData) != nullptr);
    const AdjustmentData* resp = dynamic_cast<const AdjustmentData*>(responseData);

    // We expect 7 valid adjustment candidates
    ASSERT_EQ(resp->getAdjustments().size(), static_cast<size_t>(1));

    for (auto const& adj : resp->getAdjustments()) {
        ASSERT_TRUE(adj.m_history.getLastFormulationAndRoute().isCompatible(route));
    }

    // Delete all dynamically allocated objects
    delete component;
}

TEST(Core_TestComputingComponentAdjusements, ImatinibDefaultFormulationAndRouteBestDosage)
{
    IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

    ASSERT_TRUE(component != nullptr);

    BuildImatinib builder;
    auto drugModel = builder.buildDrugModel();
    ASSERT_TRUE(drugModel != nullptr);

    const FormulationAndRoute route(
            Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);

    DateTime startSept2018(
            date::year_month_day(date::year(2018), date::month(9), date::day(1)),
            Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));

    auto drugTreatment = buildDrugTreatment(route, startSept2018);

    // Construct the adjustment traits object
    RequestResponseId requestResponseId = "1";
    Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
    Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
    double nbPointsPerHour = 10.0;
    ComputingOption computingOption(PredictionParameterType::Population, CompartmentsOption::MainCompartment);
    Tucuxi::Common::DateTime adjustmentTime(2018_y / sep / 4, 8h + 0min);
    BestCandidatesOption adjustmentOption = BestCandidatesOption::BestDosage;
    std::unique_ptr<ComputingTraitAdjustment> adjustmentsTraits = std::make_unique<ComputingTraitAdjustment>(
            requestResponseId,
            start,
            end,
            nbPointsPerHour,
            computingOption,
            adjustmentTime,
            adjustmentOption,
            LoadingOption::NoLoadingDose,
            RestPeriodOption::NoRestPeriod,
            SteadyStateTargetOption::WithinTreatmentTimeRange,
            TargetExtractionOption::PopulationValues,
            FormulationAndRouteSelectionOption::DefaultFormulationAndRoute);

    ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(adjustmentsTraits));

    std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

    ComputingStatus result;
    result = component->compute(request, response);

    ASSERT_EQ(result, ComputingStatus::Ok);


    const ComputedData* responseData = response->getData();
    ASSERT_TRUE(dynamic_cast<const AdjustmentData*>(responseData) != nullptr);
    const AdjustmentData* resp = dynamic_cast<const AdjustmentData*>(responseData);

    // We expect 7 valid adjustment candidates
    ASSERT_EQ(resp->getAdjustments().size(), static_cast<size_t>(1));

    for (auto const& adj : resp->getAdjustments()) {
        ASSERT_EQ(
                adj.m_history.getLastFormulationAndRoute(),
                drugModel->getFormulationAndRoutes().getDefault()->getFormulationAndRoute());
    }

    // Delete all dynamically allocated objects
    delete component;
}

TEST(Core_TestComputingComponentAdjusements, DISABLED_ImatinibAllFormulationAndRouteBestDosage)
{
    // TODO : Check this test. It does not work anymore because of multiple routes not supported by the PK model

    IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

    ASSERT_TRUE(component != nullptr);

    BuildImatinib builder;
    auto drugModel = builder.buildDrugModel();
    ASSERT_TRUE(drugModel != nullptr);

    const FormulationAndRoute route(
            Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);

    DateTime startSept2018(
            date::year_month_day(date::year(2018), date::month(9), date::day(1)),
            Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));

    auto drugTreatment = buildDrugTreatment(route, startSept2018);

    // Construct the adjustment traits object
    RequestResponseId requestResponseId = "1";
    Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
    Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
    double nbPointsPerHour = 10.0;
    ComputingOption computingOption(PredictionParameterType::Population, CompartmentsOption::MainCompartment);
    Tucuxi::Common::DateTime adjustmentTime(2018_y / sep / 4, 8h + 0min);
    BestCandidatesOption adjustmentOption = BestCandidatesOption::BestDosage;
    std::unique_ptr<ComputingTraitAdjustment> adjustmentsTraits = std::make_unique<ComputingTraitAdjustment>(
            requestResponseId,
            start,
            end,
            nbPointsPerHour,
            computingOption,
            adjustmentTime,
            adjustmentOption,
            LoadingOption::NoLoadingDose,
            RestPeriodOption::NoRestPeriod,
            SteadyStateTargetOption::WithinTreatmentTimeRange,
            TargetExtractionOption::PopulationValues,
            FormulationAndRouteSelectionOption::AllFormulationAndRoutes);

    ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(adjustmentsTraits));

    std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

    ComputingStatus result;
    result = component->compute(request, response);

    ASSERT_EQ(result, ComputingStatus::Ok);


    const ComputedData* responseData = response->getData();
    ASSERT_TRUE(dynamic_cast<const AdjustmentData*>(responseData) != nullptr);
    const AdjustmentData* resp = dynamic_cast<const AdjustmentData*>(responseData);

    // We expect 1 valid adjustment candidate
    ASSERT_EQ(resp->getAdjustments().size(), static_cast<size_t>(1));

    // Delete all dynamically allocated objects
    delete component;
}

TEST(Core_TestComputingComponentAdjusements, ImatinibEmptyTreatmentDefaultFormulationAndRouteAllDosages)
{
    IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

    ASSERT_TRUE(component != nullptr);

    BuildImatinib builder;
    auto drugModel = builder.buildDrugModel();
    ASSERT_TRUE(drugModel != nullptr);

    // We start with an empty treatment
    auto drugTreatment = std::make_unique<DrugTreatment>();

    // Construct the adjustment traits object
    RequestResponseId requestResponseId = "1";
    Tucuxi::Common::DateTime start(2018_y / sep / 4, 8h + 0min);
    Tucuxi::Common::DateTime end(2018_y / sep / 10, 8h + 0min);
    double nbPointsPerHour = 10.0;
    ComputingOption computingOption(PredictionParameterType::Population, CompartmentsOption::MainCompartment);
    Tucuxi::Common::DateTime adjustmentTime(2018_y / sep / 4, 8h + 0min);
    BestCandidatesOption adjustmentOption = BestCandidatesOption::AllDosages;
    std::unique_ptr<ComputingTraitAdjustment> adjustmentsTraits = std::make_unique<ComputingTraitAdjustment>(
            requestResponseId,
            start,
            end,
            nbPointsPerHour,
            computingOption,
            adjustmentTime,
            adjustmentOption,
            LoadingOption::NoLoadingDose,
            RestPeriodOption::NoRestPeriod,
            SteadyStateTargetOption::WithinTreatmentTimeRange,
            TargetExtractionOption::PopulationValues,
            FormulationAndRouteSelectionOption::DefaultFormulationAndRoute);

    ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(adjustmentsTraits));

    std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

    ComputingStatus result;
    result = component->compute(request, response);

    ASSERT_EQ(result, ComputingStatus::Ok);

    const ComputedData* responseData = response->getData();
    ASSERT_TRUE(dynamic_cast<const AdjustmentData*>(responseData) != nullptr);
    const AdjustmentData* resp = dynamic_cast<const AdjustmentData*>(responseData);
    TMP_UNUSED_PARAMETER(resp);
    // We expect 7 valid adjustment candidates
    //ASSERT_EQ(resp->getAdjustments().size(), static_cast<size_t>(7));

    //            for (auto const & adj : resp->getAdjustments()) {
    //              ASSERT_EQ(adj.m_history.getLastFormulationAndRoute(), drugModel->getFormulationAndRoutes().getDefault()->getFormulationAndRoute());
    //        }

    // Delete all dynamically allocated objects
    delete component;
}

TEST(Core_TestComputingComponentAdjusements, ImatinibSteadyStateLastFormulationAndRouteAllDosages)
{
    IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

    ASSERT_TRUE(component != nullptr);

    BuildImatinib builder;
    auto drugModel = builder.buildDrugModel();
    ASSERT_TRUE(drugModel != nullptr);

    const FormulationAndRoute route(
            Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);

    DateTime startSept2018(
            date::year_month_day(date::year(2018), date::month(9), date::day(1)),
            Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));

    auto drugTreatment = buildDrugTreatment(route, startSept2018);


    // Construct the adjustment traits object
    RequestResponseId requestResponseId = "1";
    Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
    Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
    double nbPointsPerHour = 10.0;
    ComputingOption computingOption(PredictionParameterType::Population, CompartmentsOption::MainCompartment);
    Tucuxi::Common::DateTime adjustmentTime(2018_y / sep / 4, 8h + 0min);
    BestCandidatesOption adjustmentOption = BestCandidatesOption::AllDosages;
    std::unique_ptr<ComputingTraitAdjustment> adjustmentsTraits = std::make_unique<ComputingTraitAdjustment>(
            requestResponseId,
            start,
            end,
            nbPointsPerHour,
            computingOption,
            adjustmentTime,
            adjustmentOption,
            LoadingOption::NoLoadingDose,
            RestPeriodOption::NoRestPeriod,
            SteadyStateTargetOption::AtSteadyState,
            TargetExtractionOption::PopulationValues,
            FormulationAndRouteSelectionOption::LastFormulationAndRoute);

    ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(adjustmentsTraits));

    std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

    ComputingStatus result;
    result = component->compute(request, response);

    ASSERT_EQ(result, ComputingStatus::Ok);


    const ComputedData* responseData = response->getData();
    ASSERT_TRUE(dynamic_cast<const AdjustmentData*>(responseData) != nullptr);
    const AdjustmentData* resp = dynamic_cast<const AdjustmentData*>(responseData);

    // We expect 4 valid adjustment candidates
    ASSERT_EQ(resp->getAdjustments().size(), static_cast<size_t>(4));

    for (auto const& adj : resp->getAdjustments()) {
        ASSERT_TRUE(adj.m_history.getLastFormulationAndRoute().isCompatible(route));
    }

    // Delete all dynamically allocated objects
    delete component;


    // Tests with only a candidate per interval
    {

        IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

        ASSERT_TRUE(component != nullptr);

        BuildImatinib builder;
        auto drugModel = builder.buildDrugModel();
        ASSERT_TRUE(drugModel != nullptr);

        const FormulationAndRoute route(
                Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);

        DateTime startSept2018(
                date::year_month_day(date::year(2018), date::month(9), date::day(1)),
                Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));

        auto drugTreatment = buildDrugTreatment(route, startSept2018);


        // Construct the adjustment traits object
        RequestResponseId requestResponseId = "1";
        Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
        Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
        double nbPointsPerHour = 10.0;
        ComputingOption computingOption(PredictionParameterType::Population, CompartmentsOption::MainCompartment);
        Tucuxi::Common::DateTime adjustmentTime(2018_y / sep / 4, 8h + 0min);
        BestCandidatesOption adjustmentOption = BestCandidatesOption::BestDosagePerInterval;
        std::unique_ptr<ComputingTraitAdjustment> adjustmentsTraits = std::make_unique<ComputingTraitAdjustment>(
                requestResponseId,
                start,
                end,
                nbPointsPerHour,
                computingOption,
                adjustmentTime,
                adjustmentOption,
                LoadingOption::NoLoadingDose,
                RestPeriodOption::NoRestPeriod,
                SteadyStateTargetOption::AtSteadyState,
                TargetExtractionOption::PopulationValues,
                FormulationAndRouteSelectionOption::LastFormulationAndRoute);

        ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(adjustmentsTraits));

        std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

        ComputingStatus result;
        result = component->compute(request, response);

        ASSERT_EQ(result, ComputingStatus::Ok);


        const ComputedData* responseData = response->getData();
        ASSERT_TRUE(dynamic_cast<const AdjustmentData*>(responseData) != nullptr);
        const AdjustmentData* resp = dynamic_cast<const AdjustmentData*>(responseData);

        // We expect 2 valid adjustment candidates
        ASSERT_EQ(resp->getAdjustments().size(), static_cast<size_t>(2));

        for (auto const& adj : resp->getAdjustments()) {
            ASSERT_TRUE(adj.m_history.getLastFormulationAndRoute().isCompatible(route));
        }

        // Delete all dynamically allocated objects
        delete component;
    }
}

TEST(Core_TestComputingComponentAdjusements, ImatinibLastFormulationAndRouteAllDosagesAtSteadyState)
{
    IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

    ASSERT_TRUE(component != nullptr);

    BuildImatinib builder;
    auto drugModel = builder.buildDrugModel();
    ASSERT_TRUE(drugModel != nullptr);

    const FormulationAndRoute route(
            Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);

    auto drugTreatment = std::make_unique<DrugTreatment>();

    // List of time ranges that will be pushed into the history
    DosageTimeRangeList timeRangeList;

    // Create a time range starting at the beginning of june 2017, with no upper end (to test that the repetitions
    // are handled correctly)
    DateTime startJun2018(
            date::year_month_day(date::year(2018), date::month(6), date::day(1)),
            Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));


    //const FormulationAndRoute route("formulation", AdministrationRoute::IntravenousBolus, AbsorptionModel::Intravascular);
    // Add a treatment intake every ten days in June
    // 200mg via a intravascular at 08h30, starting the 01.06
    LastingDose periodicDose(DoseValue(200.0), TucuUnit("mg"), route, Duration(), Duration(std::chrono::hours(24)));
    DosageRepeat repeatedDose(periodicDose, 300);
    auto jun2018 = std::make_unique<Tucuxi::Core::DosageTimeRange>(startJun2018, repeatedDose);

    drugTreatment->getModifiableDosageHistory().addTimeRange(*jun2018);

    // Construct the adjustment traits object
    RequestResponseId requestResponseId = "1";
    Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
    Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
    double nbPointsPerHour = 10.0;
    ComputingOption computingOption(PredictionParameterType::Population, CompartmentsOption::MainCompartment);
    Tucuxi::Common::DateTime adjustmentTime(2018_y / sep / 4, 8h + 0min);
    BestCandidatesOption adjustmentOption = BestCandidatesOption::AllDosages;
    std::unique_ptr<ComputingTraitAdjustment> adjustmentsTraits = std::make_unique<ComputingTraitAdjustment>(
            requestResponseId,
            start,
            end,
            nbPointsPerHour,
            computingOption,
            adjustmentTime,
            adjustmentOption,
            LoadingOption::NoLoadingDose,
            RestPeriodOption::NoRestPeriod,
            SteadyStateTargetOption::WithinTreatmentTimeRange,
            TargetExtractionOption::PopulationValues,
            FormulationAndRouteSelectionOption::LastFormulationAndRoute);

    ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(adjustmentsTraits));

    std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

    ComputingStatus result;
    result = component->compute(request, response);

    ASSERT_EQ(result, ComputingStatus::Ok);


    const ComputedData* responseData = response->getData();
    ASSERT_TRUE(dynamic_cast<const AdjustmentData*>(responseData) != nullptr);
    const AdjustmentData* resp = dynamic_cast<const AdjustmentData*>(responseData);

    // We expect 4 valid adjustment candidates
    ASSERT_EQ(resp->getAdjustments().size(), static_cast<size_t>(4));

    for (auto const& adj : resp->getAdjustments()) {
        ASSERT_EQ(adj.m_history.getDosageTimeRanges()[0]->getStartDate(), adjustmentTime);
        ASSERT_TRUE(adj.m_history.getLastFormulationAndRoute().isCompatible(route));
    }

    // Delete all dynamically allocated objects
    delete component;
}

TEST(Core_TestComputingComponentAdjusements, DISABLED_ImatinibAllFormulationAndRouteBestDosageLoadingDose)
{
    // TODO : Check this test. It does not work anymore because of multiple routes not supported by the PK model

    IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

    ASSERT_TRUE(component != nullptr);

    BuildImatinib builder;
    auto drugModel = builder.buildDrugModel();
    ASSERT_TRUE(drugModel != nullptr);

    const FormulationAndRoute route(
            Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);

    DateTime startSept2018(
            date::year_month_day(date::year(2018), date::month(9), date::day(1)),
            Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));

    auto drugTreatment = buildDrugTreatment(route, startSept2018);

    // Construct the adjustment traits object
    RequestResponseId requestResponseId = "1";
    Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
    Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
    double nbPointsPerHour = 10.0;
    ComputingOption computingOption(PredictionParameterType::Population, CompartmentsOption::MainCompartment);
    Tucuxi::Common::DateTime adjustmentTime(2018_y / sep / 4, 8h + 0min);
    BestCandidatesOption adjustmentOption = BestCandidatesOption::BestDosage;
    std::unique_ptr<ComputingTraitAdjustment> adjustmentsTraits = std::make_unique<ComputingTraitAdjustment>(
            requestResponseId,
            start,
            end,
            nbPointsPerHour,
            computingOption,
            adjustmentTime,
            adjustmentOption,
            LoadingOption::LoadingDoseAllowed,
            RestPeriodOption::NoRestPeriod,
            SteadyStateTargetOption::WithinTreatmentTimeRange,
            TargetExtractionOption::PopulationValues,
            FormulationAndRouteSelectionOption::AllFormulationAndRoutes);

    ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(adjustmentsTraits));

    std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

    ComputingStatus result;
    result = component->compute(request, response);

    ASSERT_EQ(result, ComputingStatus::Ok);


    const ComputedData* responseData = response->getData();
    ASSERT_TRUE(dynamic_cast<const AdjustmentData*>(responseData) != nullptr);
    const AdjustmentData* resp = dynamic_cast<const AdjustmentData*>(responseData);

    // We expect 2 dosage time range (loading dose)
    ASSERT_EQ(resp->getAdjustments()[0].getDosageHistory().getDosageTimeRanges().size(), static_cast<size_t>(2));

    // Delete all dynamically allocated objects
    delete component;
}

TEST(Core_TestComputingComponentAdjusements, ImatinibAllFormulationAndRouteBestDosageRestPeriod)
{
    IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

    ASSERT_TRUE(component != nullptr);

    BuildImatinib builder;
    auto drugModel = builder.buildDrugModel();
    ASSERT_TRUE(drugModel != nullptr);

    const FormulationAndRoute route(
            Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);

    DateTime startSept2018(
            date::year_month_day(date::year(2018), date::month(9), date::day(1)),
            Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));

    auto drugTreatment = buildDrugTreatment(route, startSept2018, DoseValue(20000));


    // Construct the adjustment traits object
    RequestResponseId requestResponseId = "1";
    Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
    Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
    double nbPointsPerHour = 10.0;
    ComputingOption computingOption(PredictionParameterType::Population, CompartmentsOption::MainCompartment);
    Tucuxi::Common::DateTime adjustmentTime(2018_y / sep / 3, 8h + 0min);
    BestCandidatesOption adjustmentOption = BestCandidatesOption::BestDosage;
    std::unique_ptr<ComputingTraitAdjustment> adjustmentsTraits = std::make_unique<ComputingTraitAdjustment>(
            requestResponseId,
            start,
            end,
            nbPointsPerHour,
            computingOption,
            adjustmentTime,
            adjustmentOption,
            LoadingOption::NoLoadingDose,
            RestPeriodOption::RestPeriodAllowed,
            SteadyStateTargetOption::AtSteadyState,
            TargetExtractionOption::PopulationValues,
            FormulationAndRouteSelectionOption::AllFormulationAndRoutes);

    ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(adjustmentsTraits));

    std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

    ComputingStatus result;
    result = component->compute(request, response);

    ASSERT_EQ(result, ComputingStatus::Ok);


    const ComputedData* responseData = response->getData();
    ASSERT_TRUE(dynamic_cast<const AdjustmentData*>(responseData) != nullptr);

    const AdjustmentData* resp = dynamic_cast<const AdjustmentData*>(responseData);

    if (resp) {
        // We expect 2 dosage time range (rest period)
        ASSERT_EQ(resp->getAdjustments()[0].getDosageHistory().getDosageTimeRanges().size(), static_cast<size_t>(2));
    }

    // Delete all dynamically allocated objects
    delete component;
}


TEST(Core_TestComputingComponentAdjusements, GentamicinTwoTargets)
{
    IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

    ASSERT_TRUE(component != nullptr);

    BuildGentamicinFuchs2014 builder;
    auto drugModel = builder.buildDrugModel();
    ASSERT_TRUE(drugModel != nullptr);

    // We start with an empty treatment
    auto drugTreatment = std::make_unique<DrugTreatment>();

    // Construct the adjustment traits object
    RequestResponseId requestResponseId = "1";
    Tucuxi::Common::DateTime start(2018_y / sep / 4, 8h + 0min);
    Tucuxi::Common::DateTime end(2018_y / sep / 10, 8h + 0min);
    double nbPointsPerHour = 10.0;
    ComputingOption computingOption(PredictionParameterType::Population, CompartmentsOption::MainCompartment);
    Tucuxi::Common::DateTime adjustmentTime(2018_y / sep / 4, 8h + 0min);
    BestCandidatesOption adjustmentOption = BestCandidatesOption::AllDosages;
    std::unique_ptr<ComputingTraitAdjustment> adjustmentsTraits = std::make_unique<ComputingTraitAdjustment>(
            requestResponseId,
            start,
            end,
            nbPointsPerHour,
            computingOption,
            adjustmentTime,
            adjustmentOption,
            LoadingOption::NoLoadingDose,
            RestPeriodOption::NoRestPeriod,
            SteadyStateTargetOption::WithinTreatmentTimeRange,
            TargetExtractionOption::PopulationValues,
            FormulationAndRouteSelectionOption::DefaultFormulationAndRoute);

    ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(adjustmentsTraits));

    std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

    ComputingStatus result;
    result = component->compute(request, response);

    ASSERT_EQ(result, ComputingStatus::Ok);

    const ComputedData* responseData = response->getData();
    ASSERT_TRUE(dynamic_cast<const AdjustmentData*>(responseData) != nullptr);
    const AdjustmentData* resp = dynamic_cast<const AdjustmentData*>(responseData);
    ASSERT_GT(resp->getAdjustments().size(), 0);

    // Delete all dynamically allocated objects
    delete component;
}
