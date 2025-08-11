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

#include "drugmodels/buildconstantelimination.h"
#include "gtest_core.h"

using namespace Tucuxi::Core;

using namespace std::chrono_literals;
using namespace date;


std::unique_ptr<DrugTreatment> buildDrugTreatmentForAdjustments(
        const FormulationAndRoute& _route,
        const DateTime startDateTime,
        DoseValue _doseValue = DoseValue(200),
        TucuUnit _unit = TucuUnit("mg"),
        int interval = 6,
        unsigned int nbrDoses = 16)
{
    auto drugTreatment = std::make_unique<DrugTreatment>();

    // List of time ranges that will be pushed into the history
    DosageTimeRangeList timeRangeList;

    LastingDose periodicDose(_doseValue, _unit, _route, Duration(), Duration(std::chrono::hours(interval)));
    DosageRepeat repeatedDose(periodicDose, nbrDoses);
    auto dosageTimeRange = std::make_unique<Tucuxi::Core::DosageTimeRange>(startDateTime, repeatedDose);

    drugTreatment->getModifiableDosageHistory().addTimeRange(*dosageTimeRange);

    return drugTreatment;
}

std::unique_ptr<DrugTreatment> buildDrugTreatmentForAdjustmentsDaily(
        const FormulationAndRoute& _route,
        const DateTime startDateTime,
        DoseValue _doseValue = DoseValue(200),
        TucuUnit _unit = TucuUnit("mg"),
        int interval = 6,
        unsigned int nbrDoses = 16)
{
    auto drugTreatment = std::make_unique<DrugTreatment>();

    // List of time ranges that will be pushed into the history
    DosageTimeRangeList timeRangeList;

    DailyDose dailyDose(
            _doseValue, _unit, _route, Duration(std::chrono::hours(1)), TimeOfDay(Duration(std::chrono::hours(8))));
    DosageLoop loop(dailyDose);

    auto dosageTimeRange = std::make_unique<Tucuxi::Core::DosageTimeRange>(startDateTime, loop);

    drugTreatment->getModifiableDosageHistory().addTimeRange(*dosageTimeRange);

    return drugTreatment;
}

std::unique_ptr<DosageTimeRange> buildDosageTimeRangeForAdjustments(
        const FormulationAndRoute& _route,
        const DateTime startDateTime,
        DoseValue _doseValue,
        TucuUnit _unit,
        int interval,
        unsigned int nbrDoses)
{
    auto drugTreatment = std::make_unique<DrugTreatment>();

    // List of time ranges that will be pushed into the history
    DosageTimeRangeList timeRangeList;

    LastingDose periodicDose(_doseValue, _unit, _route, Duration(), Duration(std::chrono::hours(interval)));
    DosageRepeat repeatedDose(periodicDose, nbrDoses);
    auto dosageTimeRange = std::make_unique<Tucuxi::Core::DosageTimeRange>(startDateTime, repeatedDose);
    return dosageTimeRange;
}


TEST(Core_TestComputingComponentAdjusements, ConstantElimination)
{
    IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

    ASSERT_TRUE(component != nullptr);

    BuildConstantElimination builder;
    auto drugModel = builder.buildDrugModel();
    ASSERT_TRUE(drugModel != nullptr);


    std::unique_ptr<SubTargetDefinition> cMin(new SubTargetDefinition("cMin", 800.0, nullptr));
    std::unique_ptr<SubTargetDefinition> cMax(new SubTargetDefinition("cMax", 1200.0, nullptr));
    std::unique_ptr<SubTargetDefinition> cBest(new SubTargetDefinition("cBest", 1000.0, nullptr));
    std::unique_ptr<TargetDefinition> target = std::make_unique<TargetDefinition>(
            TargetType::Residual,
            Unit("mg/l"),
            ActiveMoietyId("analyte"),
            std::make_unique<SubTargetDefinition>("cMin", 1160.0, nullptr),
            std::make_unique<SubTargetDefinition>("cMax", 1240.0, nullptr),
            std::make_unique<SubTargetDefinition>("cBest", 1200.0, nullptr),
            std::make_unique<SubTargetDefinition>("mic", 0.0, nullptr),
            std::make_unique<SubTargetDefinition>("tMin", 1000.0, nullptr),
            std::make_unique<SubTargetDefinition>("tMax", 1000.0, nullptr),
            std::make_unique<SubTargetDefinition>("tBest", 1000.0, nullptr),
            std::make_unique<SubTargetDefinition>("toxicity", 10000.0, nullptr),
            std::make_unique<SubTargetDefinition>("inefficacy", 000.0, nullptr));

    drugModel->getActiveMoieties()[0]->addTarget(std::move(target));

    drugModel->getCovariates()[0]->setRefreshPeriod(
            Duration(std::chrono::hours(1), std::chrono::minutes(0), std::chrono::seconds(0)));
    drugModel->getCovariates()[1]->setRefreshPeriod(
            Duration(std::chrono::hours(1), std::chrono::minutes(0), std::chrono::seconds(0)));
    drugModel->getCovariates()[2]->setRefreshPeriod(
            Duration(std::chrono::hours(1), std::chrono::minutes(0), std::chrono::seconds(0)));
    drugModel->getCovariates()[3]->setRefreshPeriod(
            Duration(std::chrono::hours(1), std::chrono::minutes(0), std::chrono::seconds(0)));

    const FormulationAndRoute route(Formulation::OralSolution, AdministrationRoute::Oral);

    DateTime treatmentStartDate(2018_y / sep / 1, 8h + 0min);

    auto drugTreatment = buildDrugTreatmentForAdjustments(route, treatmentStartDate);

    Duration day(std::chrono::hours(24), std::chrono::minutes(0), std::chrono::seconds(0));
    // Add a covM covariate that increases every day for 100 days
    // Day 0: 1
    // Day 1: 2
    // Day n: n+1
    for (int i = 0; i < 100; i++) {
        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covM", std::to_string(i + 1), DataType::Double, TucuUnit(), treatmentStartDate + day * i));
    }



    // Construct the adjustment traits object
    RequestResponseId requestResponseId = "1";
    Tucuxi::Common::DateTime start(treatmentStartDate);
    Tucuxi::Common::DateTime end(treatmentStartDate + day * 4);
    double nbPointsPerHour = 10.0;
    ComputingOption computingOption(PredictionParameterType::Apriori, CompartmentsOption::MainCompartment);
    Tucuxi::Common::DateTime adjustmentTime(treatmentStartDate + day * 3);
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

    ASSERT_EQ(resp->getCompartmentInfos().size(), static_cast<size_t>(1));
    ASSERT_EQ(resp->getCompartmentInfos()[0].getId(), "analyte");
    ASSERT_EQ(resp->getCompartmentInfos()[0].getType(), CompartmentInfo::CompartmentType::ActiveMoietyAndAnalyte);

    // The adjustment is at beginning of 4th day, so M==4, then the residual concentration should be 4x200=800
    EXPECT_FALSE(resp->isCurrentInRange());
    EXPECT_DOUBLE_EQ(resp->getCurrentDosageWithScore().getGlobalScore(), 0.0);
    EXPECT_EQ(resp->getCurrentDosageWithScore().m_targetsEvaluation.size(), 1);
    EXPECT_DOUBLE_EQ(resp->getCurrentDosageWithScore().m_targetsEvaluation[0].getValue(), 800);

    // We expect 3 valid adjustment candidates with all three being with dose 300, which ends up with a target evaluation value of 1200
    EXPECT_EQ(resp->getAdjustments().size(), static_cast<size_t>(3));

    for (auto const& adj : resp->getAdjustments()) {
        EXPECT_EQ(adj.m_history.getDosageTimeRanges()[0]->getStartDate(), adjustmentTime);
        EXPECT_TRUE(adj.m_history.getLastFormulationAndRoute().isCompatible(route));
        EXPECT_DOUBLE_EQ(adj.m_targetsEvaluation[0].getValue(), 1200);
    }


    // Delete all dynamically allocated objects
    delete component;
}



TEST(Core_TestComputingComponentAdjusements, ConstantEliminationDaily)
{
    IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

    ASSERT_TRUE(component != nullptr);

    BuildConstantElimination builder;
    auto drugModel = builder.buildDrugModel();
    ASSERT_TRUE(drugModel != nullptr);


    std::unique_ptr<SubTargetDefinition> cMin(new SubTargetDefinition("cMin", 800.0, nullptr));
    std::unique_ptr<SubTargetDefinition> cMax(new SubTargetDefinition("cMax", 1200.0, nullptr));
    std::unique_ptr<SubTargetDefinition> cBest(new SubTargetDefinition("cBest", 1000.0, nullptr));
    std::unique_ptr<TargetDefinition> target = std::make_unique<TargetDefinition>(
            TargetType::Residual,
            Unit("mg/l"),
            ActiveMoietyId("analyte"),
            std::make_unique<SubTargetDefinition>("cMin", 1160.0, nullptr),
            std::make_unique<SubTargetDefinition>("cMax", 1240.0, nullptr),
            std::make_unique<SubTargetDefinition>("cBest", 1200.0, nullptr),
            std::make_unique<SubTargetDefinition>("mic", 0.0, nullptr),
            std::make_unique<SubTargetDefinition>("tMin", 1000.0, nullptr),
            std::make_unique<SubTargetDefinition>("tMax", 1000.0, nullptr),
            std::make_unique<SubTargetDefinition>("tBest", 1000.0, nullptr),
            std::make_unique<SubTargetDefinition>("toxicity", 10000.0, nullptr),
            std::make_unique<SubTargetDefinition>("inefficacy", 000.0, nullptr));

    drugModel->getActiveMoieties()[0]->addTarget(std::move(target));

    drugModel->getCovariates()[0]->setRefreshPeriod(
            Duration(std::chrono::hours(1), std::chrono::minutes(0), std::chrono::seconds(0)));
    drugModel->getCovariates()[1]->setRefreshPeriod(
            Duration(std::chrono::hours(1), std::chrono::minutes(0), std::chrono::seconds(0)));
    drugModel->getCovariates()[2]->setRefreshPeriod(
            Duration(std::chrono::hours(1), std::chrono::minutes(0), std::chrono::seconds(0)));
    drugModel->getCovariates()[3]->setRefreshPeriod(
            Duration(std::chrono::hours(1), std::chrono::minutes(0), std::chrono::seconds(0)));

    const FormulationAndRoute route(Formulation::OralSolution, AdministrationRoute::Oral);

    DateTime treatmentStartDate(2018_y / sep / 1, 8h + 0min);

    auto drugTreatment = buildDrugTreatmentForAdjustmentsDaily(route, treatmentStartDate);

    Duration day(std::chrono::hours(24), std::chrono::minutes(0), std::chrono::seconds(0));
    // Add a covM covariate that increases every day for 100 days
    // Day 0: 1
    // Day 1: 2
    // Day n: n+1
    for (int i = 0; i < 100; i++) {
        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covM", std::to_string(i + 1), DataType::Double, TucuUnit(), treatmentStartDate + day * i));
    }



    // Construct the adjustment traits object
    RequestResponseId requestResponseId = "1";
    Tucuxi::Common::DateTime start(treatmentStartDate);
    Tucuxi::Common::DateTime end(treatmentStartDate + day * 4);
    double nbPointsPerHour = 10.0;
    ComputingOption computingOption(PredictionParameterType::Apriori, CompartmentsOption::MainCompartment);
    Tucuxi::Common::DateTime adjustmentTime(treatmentStartDate + day * 3.2);
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

    ASSERT_EQ(resp->getCompartmentInfos().size(), static_cast<size_t>(1));
    ASSERT_EQ(resp->getCompartmentInfos()[0].getId(), "analyte");
    ASSERT_EQ(resp->getCompartmentInfos()[0].getType(), CompartmentInfo::CompartmentType::ActiveMoietyAndAnalyte);

    // The adjustment is at beginning of 4th day, so M==4, then the residual concentration should be 4x200=800
    EXPECT_FALSE(resp->isCurrentInRange());
    EXPECT_DOUBLE_EQ(resp->getCurrentDosageWithScore().getGlobalScore(), 0.0);
    EXPECT_EQ(resp->getCurrentDosageWithScore().m_targetsEvaluation.size(), 1);
    EXPECT_DOUBLE_EQ(resp->getCurrentDosageWithScore().m_targetsEvaluation[0].getValue(), 800);

    // We expect 3 valid adjustment candidates with all three being with dose 300, which ends up with a target evaluation value of 1200
    EXPECT_EQ(resp->getAdjustments().size(), static_cast<size_t>(3));

    for (auto const& adj : resp->getAdjustments()) {
        EXPECT_EQ(adj.m_history.getDosageTimeRanges()[0]->getStartDate(), adjustmentTime);
        EXPECT_TRUE(adj.m_history.getLastFormulationAndRoute().isCompatible(route));
        EXPECT_DOUBLE_EQ(adj.m_targetsEvaluation[0].getValue(), 1200);
    }


    // Delete all dynamically allocated objects
    delete component;
}
