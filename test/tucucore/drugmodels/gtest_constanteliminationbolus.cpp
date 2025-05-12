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


#include <chrono>
#include <iostream>
#include <memory>

#include <date/date.h>
#include <gtest/gtest.h>

#include "tucucore/computingcomponent.h"
#include "tucucore/computingservice/computingrequest.h"
#include "tucucore/drugmodelchecker.h"
#include "tucucore/drugmodelimport.h"
#include "tucucore/drugtreatment/patientcovariate.h"
#include "tucucore/pkmodel.h"

#include "../gtest_core.h"
#include "../pkmodels/constanteliminationbolus.h"
#include "../testutils.h"
#include "buildconstantelimination.h"

using namespace std::chrono_literals;
using namespace date;

using namespace Tucuxi::Core;

// The inv CDF is calculated using the probit() function in octave:
// probit(0.05)
// probit(0.1)
// probit(0.25)
// probit(0.5)
// probit(0.75)
// probit(0.9)
// probit(0.95)
static std::vector<double> invCdf = {-1.6449, -1.2816, -0.67449, 0.0, 0.67449, 1.2816, 1.6449};

static std::vector<Value> percentileRanks = {5, 10, 25, 50, 75, 90, 95};

TEST(Core_TestConstantEliminationBolus, Test0)
{
    BuildConstantElimination builder;
    auto drugModel = builder.buildDrugModel();

    ASSERT_TRUE(drugModel != nullptr);

    ASSERT_TRUE(drugModel->checkInvariants());

    DrugModelChecker checker;

    std::shared_ptr<PkModel> sharedPkModel;
    sharedPkModel = std::make_shared<PkModel>("test.constantelimination", PkModel::AllowMultipleRoutes::No);

    bool addResult = sharedPkModel->addIntakeIntervalCalculatorFactory(
            AbsorptionModel::Extravascular, ConstantEliminationBolus::getCreator());
    ASSERT_TRUE(addResult);

    sharedPkModel->addParameterList(AbsorptionModel::Extravascular, ConstantEliminationBolus::getParametersId());

    std::shared_ptr<PkModelCollection> collection = std::make_shared<PkModelCollection>();
    collection->addPkModel(sharedPkModel);
    DrugModelChecker::CheckerResult_t checkerResult = checker.checkDrugModel(drugModel.get(), collection.get());

    ASSERT_TRUE(checkerResult.m_ok);

    if (!checkerResult.m_ok) {
        std::cout << checkerResult.m_errorMessage << std::endl;
    }

    // Now the drug model is ready to be used


    IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

    ASSERT_TRUE(component != nullptr);

    static_cast<ComputingComponent*>(component)->setPkModelCollection(collection);


    {
        const FormulationAndRoute route(Formulation::OralSolution, AdministrationRoute::Oral);

        DateTime startSept2018(
                date::year_month_day(date::year(2018), date::month(9), date::day(1)),
                Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));

        auto drugTreatment = buildDrugTreatment(route, startSept2018);

        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covS", "0.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covA", "0.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covR", "0.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));


        std::unique_ptr<ComputingTraits> computingTraits = std::make_unique<ComputingTraits>();

        RequestResponseId requestResponseId = "1";
        Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
        Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
        double nbPointsPerHour = 10.0;
        ComputingOption computingOption(
                PredictionParameterType::Apriori,
                CompartmentsOption::MainCompartment,
                RetrieveStatisticsOption::RetrieveStatistics,
                RetrieveParametersOption::RetrieveParameters,
                RetrieveCovariatesOption::RetrieveCovariates,
                ForceUgPerLiterOption::Force);
        std::unique_ptr<ComputingTraitConcentration> traits = std::make_unique<ComputingTraitConcentration>(
                requestResponseId, start, end, nbPointsPerHour, computingOption);


        std::unique_ptr<ComputingTraitPercentiles> traitsPercentiles = std::make_unique<ComputingTraitPercentiles>(
                requestResponseId, start, end, percentileRanks, nbPointsPerHour, computingOption);

        {
            ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(traitsPercentiles));

            std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

            ComputingStatus result;
            result = component->compute(request, response);

            ASSERT_EQ(result, ComputingStatus::Ok);

            const ComputedData* responseData = response->getData();
            ASSERT_TRUE(dynamic_cast<const PercentilesData*>(responseData) != nullptr);
            const PercentilesData* resp = dynamic_cast<const PercentilesData*>(responseData);

            ASSERT_EQ(resp->getNbRanks(), percentileRanks.size());

            for (size_t p = 0; p < resp->getNbRanks(); p++) {
                DateTime statTime = DateTime::now();
                Value statValue = 0.0;
                resp->getData(p, 0)
                        .m_statistics.getStatistic(0, CycleStatisticType::Mean)
                        .getValue(statTime, statValue);
                ASSERT_DOUBLE_EQ(statValue, 200000.0);
            }
        }
        {
            ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(traits));

            std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

            ComputingStatus result;
            result = component->compute(request, response);

            ASSERT_EQ(result, ComputingStatus::Ok);

            const ComputedData* responseData = response->getData();
            ASSERT_TRUE(dynamic_cast<const SinglePredictionData*>(responseData) != nullptr);
            const SinglePredictionData* resp = dynamic_cast<const SinglePredictionData*>(responseData);

            ASSERT_EQ(resp->getCompartmentInfos().size(), static_cast<size_t>(1));
            ASSERT_EQ(resp->getCompartmentInfos()[0].getId(), "analyte");
            ASSERT_EQ(
                    resp->getCompartmentInfos()[0].getType(), CompartmentInfo::CompartmentType::ActiveMoietyAndAnalyte);

            std::vector<CycleData> data = resp->getData();
            ASSERT_EQ(data.size(), static_cast<size_t>(16));
            ASSERT_EQ(data[0].m_concentrations.size(), static_cast<size_t>(1));
            ASSERT_DOUBLE_EQ(data[0].m_concentrations[0][0], 200000.0);
            DateTime startSept2018(
                    date::year_month_day(date::year(2018), date::month(9), date::day(1)),
                    Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));

            ASSERT_DOUBLE_EQ(data[0].m_start.toSeconds() + data[0].m_times[0][0] * 3600.0, startSept2018.toSeconds());
            ASSERT_DOUBLE_EQ(
                    data[1].m_start.toSeconds() + data[1].m_times[0][0] * 3600.0,
                    startSept2018.toSeconds() + 3600.0 * 6.0);

            DateTime statTime = DateTime::now();
            Value statValue = 0.0;
            data[0].m_statistics.getStatistic(0, CycleStatisticType::AUC).getValue(statTime, statValue);
            ASSERT_DOUBLE_EQ(statValue, 200000.0 * 6.0);

            data[0].m_statistics.getStatistic(0, CycleStatisticType::CumulativeAuc).getValue(statTime, statValue);
            ASSERT_DOUBLE_EQ(statValue, 200000.0 * 6.0);

            data[0].m_statistics.getStatistic(0, CycleStatisticType::Peak).getValue(statTime, statValue);
            ASSERT_DOUBLE_EQ(statValue, 200000.0);

            data[0].m_statistics.getStatistic(0, CycleStatisticType::Mean).getValue(statTime, statValue);
            ASSERT_DOUBLE_EQ(statValue, 200000.0);

            data[1].m_statistics.getStatistic(0, CycleStatisticType::AUC).getValue(statTime, statValue);
            ASSERT_DOUBLE_EQ(statValue, 200000.0 * 6.0);

            data[1].m_statistics.getStatistic(0, CycleStatisticType::CumulativeAuc).getValue(statTime, statValue);
            ASSERT_DOUBLE_EQ(statValue, 200000.0 * 12.0);

            data[1].m_statistics.getStatistic(0, CycleStatisticType::Peak).getValue(statTime, statValue);
            ASSERT_DOUBLE_EQ(statValue, 200000.0);

            data[1].m_statistics.getStatistic(0, CycleStatisticType::Mean).getValue(statTime, statValue);
            ASSERT_DOUBLE_EQ(statValue, 200000.0);
        }
    }

    // Delete all dynamically allocated objects
    delete component;
}

TEST(Core_TestConstantEliminationBolus, Test1)
{
    BuildConstantElimination builder;
    auto drugModel = builder.buildDrugModel();

    ASSERT_TRUE(drugModel != nullptr);

    ASSERT_TRUE(drugModel->checkInvariants());

    DrugModelChecker checker;

    std::shared_ptr<PkModel> sharedPkModel;
    sharedPkModel = std::make_shared<PkModel>("test.constantelimination", PkModel::AllowMultipleRoutes::No);

    bool addResult = sharedPkModel->addIntakeIntervalCalculatorFactory(
            AbsorptionModel::Extravascular, ConstantEliminationBolus::getCreator());
    ASSERT_TRUE(addResult);

    sharedPkModel->addParameterList(AbsorptionModel::Extravascular, ConstantEliminationBolus::getParametersId());


    std::shared_ptr<PkModelCollection> collection = std::make_shared<PkModelCollection>();
    collection->addPkModel(sharedPkModel);
    DrugModelChecker::CheckerResult_t checkerResult = checker.checkDrugModel(drugModel.get(), collection.get());

    ASSERT_TRUE(checkerResult.m_ok);

    if (!checkerResult.m_ok) {
        std::cout << checkerResult.m_errorMessage << std::endl;
    }

    // Now the drug model is ready to be used


    IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

    ASSERT_TRUE(component != nullptr);

    static_cast<ComputingComponent*>(component)->setPkModelCollection(collection);


    {
        const FormulationAndRoute route(Formulation::OralSolution, AdministrationRoute::Oral);

        DateTime startSept2018(
                date::year_month_day(date::year(2018), date::month(9), date::day(1)),
                Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));

        auto drugTreatment = buildDrugTreatment(route, startSept2018);

        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covS", "0.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covA", "1.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covR", "0.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));

        RequestResponseId requestResponseId = "1";
        Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
        Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
        double nbPointsPerHour = 10.0;
        ComputingOption computingOption(
                PredictionParameterType::Apriori,
                CompartmentsOption::MainCompartment,
                RetrieveStatisticsOption::RetrieveStatistics,
                RetrieveParametersOption::RetrieveParameters,
                RetrieveCovariatesOption::RetrieveCovariates);
        std::unique_ptr<ComputingTraitConcentration> traits = std::make_unique<ComputingTraitConcentration>(
                requestResponseId, start, end, nbPointsPerHour, computingOption);


        std::unique_ptr<ComputingTraitPercentiles> traitsPercentiles = std::make_unique<ComputingTraitPercentiles>(
                requestResponseId, start, end, percentileRanks, nbPointsPerHour, computingOption);


        {

            ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(traitsPercentiles));

            std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

            ComputingStatus result;
            result = component->compute(request, response);

            ASSERT_EQ(result, ComputingStatus::Ok);

            const ComputedData* responseData = response->getData();
            ASSERT_TRUE(dynamic_cast<const PercentilesData*>(responseData) != nullptr);
            const PercentilesData* resp = dynamic_cast<const PercentilesData*>(responseData);

            ASSERT_EQ(resp->getNbRanks(), percentileRanks.size());

            for (size_t p = 0; p < resp->getNbRanks(); p++) {
                DateTime statTime = DateTime::now();
                Value statValue = 0.0;
                resp->getData(p, 0)
                        .m_statistics.getStatistic(0, CycleStatisticType::Mean)
                        .getValue(statTime, statValue);
                ASSERT_DOUBLE_EQ(statValue, 200001.0);
            }
        }
        {
            ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(traits));

            std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

            ComputingStatus result;
            result = component->compute(request, response);

            ASSERT_EQ(result, ComputingStatus::Ok);


            const ComputedData* responseData = response->getData();
            ASSERT_TRUE(dynamic_cast<const SinglePredictionData*>(responseData) != nullptr);
            const SinglePredictionData* resp = dynamic_cast<const SinglePredictionData*>(responseData);

            ASSERT_EQ(resp->getCompartmentInfos().size(), static_cast<size_t>(1));
            ASSERT_EQ(resp->getCompartmentInfos()[0].getId(), "analyte");
            ASSERT_EQ(
                    resp->getCompartmentInfos()[0].getType(), CompartmentInfo::CompartmentType::ActiveMoietyAndAnalyte);

            std::vector<CycleData> data = resp->getData();
            ASSERT_EQ(data.size(), static_cast<size_t>(16));
            ASSERT_EQ(data[0].m_concentrations.size(), static_cast<size_t>(1));
            ASSERT_DOUBLE_EQ(data[0].m_concentrations[0][0], 200001.0);
            DateTime startSept2018(
                    date::year_month_day(date::year(2018), date::month(9), date::day(1)),
                    Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));

            ASSERT_DOUBLE_EQ(data[0].m_start.toSeconds() + data[0].m_times[0][0] * 3600.0, startSept2018.toSeconds());
            ASSERT_DOUBLE_EQ(
                    data[1].m_start.toSeconds() + data[1].m_times[0][0] * 3600.0,
                    startSept2018.toSeconds() + 3600.0 * 6.0);

            DateTime statTime = DateTime::now();
            Value statValue = 0.0;
            data[0].m_statistics.getStatistic(0, CycleStatisticType::AUC).getValue(statTime, statValue);
            ASSERT_DOUBLE_EQ(statValue, 200001.0 * 6.0);

            data[0].m_statistics.getStatistic(0, CycleStatisticType::CumulativeAuc).getValue(statTime, statValue);
            ASSERT_DOUBLE_EQ(statValue, 200001.0 * 6.0);

            data[0].m_statistics.getStatistic(0, CycleStatisticType::Peak).getValue(statTime, statValue);
            ASSERT_DOUBLE_EQ(statValue, 200001.0);

            data[0].m_statistics.getStatistic(0, CycleStatisticType::Mean).getValue(statTime, statValue);
            ASSERT_DOUBLE_EQ(statValue, 200001.0);

            data[1].m_statistics.getStatistic(0, CycleStatisticType::AUC).getValue(statTime, statValue);
            ASSERT_DOUBLE_EQ(statValue, 200001.0 * 6.0);

            data[1].m_statistics.getStatistic(0, CycleStatisticType::CumulativeAuc).getValue(statTime, statValue);
            ASSERT_DOUBLE_EQ(statValue, 200001.0 * 12.0);

            data[1].m_statistics.getStatistic(0, CycleStatisticType::Peak).getValue(statTime, statValue);
            ASSERT_DOUBLE_EQ(statValue, 200001.0);

            data[1].m_statistics.getStatistic(0, CycleStatisticType::Mean).getValue(statTime, statValue);
            ASSERT_DOUBLE_EQ(statValue, 200001.0);
        }
    }

    // Delete all dynamically allocated objects
    delete component;
}

TEST(Core_TestConstantEliminationBolus, ResidualErrorModelAdditive)
{
    BuildConstantElimination builder;
    auto drugModel = builder.buildDrugModel(ResidualErrorType::ADDITIVE, std::vector<Value>({10000.0}));

    ASSERT_TRUE(drugModel != nullptr);

    ASSERT_TRUE(drugModel->checkInvariants());

    DrugModelChecker checker;

    std::shared_ptr<PkModel> sharedPkModel;
    sharedPkModel = std::make_shared<PkModel>("test.constantelimination", PkModel::AllowMultipleRoutes::No);

    bool addResult = sharedPkModel->addIntakeIntervalCalculatorFactory(
            AbsorptionModel::Extravascular, ConstantEliminationBolus::getCreator());
    ASSERT_TRUE(addResult);

    sharedPkModel->addParameterList(AbsorptionModel::Extravascular, ConstantEliminationBolus::getParametersId());

    std::shared_ptr<PkModelCollection> collection = std::make_shared<PkModelCollection>();
    collection->addPkModel(sharedPkModel);
    DrugModelChecker::CheckerResult_t checkerResult = checker.checkDrugModel(drugModel.get(), collection.get());

    ASSERT_TRUE(checkerResult.m_ok);

    if (!checkerResult.m_ok) {
        std::cout << checkerResult.m_errorMessage << std::endl;
    }

    // Now the drug model is ready to be used


    IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

    ASSERT_TRUE(component != nullptr);

    static_cast<ComputingComponent*>(component)->setPkModelCollection(collection);


    {
        const FormulationAndRoute route(Formulation::OralSolution, AdministrationRoute::Oral);

        DateTime startSept2018(
                date::year_month_day(date::year(2018), date::month(9), date::day(1)),
                Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));

        auto drugTreatment = buildDrugTreatment(route, startSept2018);

        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covS", "0.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covA", "0.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covR", "0.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));

        RequestResponseId requestResponseId = "1";
        Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
        Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
        double nbPointsPerHour = 10.0;
        ComputingOption computingOption(
                PredictionParameterType::Apriori,
                CompartmentsOption::MainCompartment,
                RetrieveStatisticsOption::RetrieveStatistics,
                RetrieveParametersOption::RetrieveParameters,
                RetrieveCovariatesOption::RetrieveCovariates);


        std::unique_ptr<ComputingTraitPercentiles> traitsPercentiles = std::make_unique<ComputingTraitPercentiles>(
                requestResponseId, start, end, percentileRanks, nbPointsPerHour, computingOption);

        ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(traitsPercentiles));

        std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

        ComputingStatus result;
        result = component->compute(request, response);

        ASSERT_EQ(result, ComputingStatus::Ok);

        const ComputedData* responseData = response->getData();
        ASSERT_TRUE(dynamic_cast<const PercentilesData*>(responseData) != nullptr);
        const PercentilesData* resp = dynamic_cast<const PercentilesData*>(responseData);

        ASSERT_EQ(resp->getNbRanks(), percentileRanks.size());

        for (size_t p = 0; p < resp->getNbRanks(); p++) {
            DateTime statTime = DateTime::now();
            Value statValue = 0.0;
            resp->getData(p, 0).m_statistics.getStatistic(0, CycleStatisticType::Mean).getValue(statTime, statValue);
            //ASSERT_DOUBLE_EQ(statValue, 200001.0);

            // Multiply the Inv CDF by SD (10), and by 1000.0 because of mg/l
            double expectedValue = invCdf[p] * 10.0 * 1000.0;
            ASSERT_PRED4(double_eq_rel_abs, statValue - 200000.0, expectedValue, 0.02, 10.0 * 10.0 * 1000.0 * 0.06);
        }
    }

    // Delete all dynamically allocated objects
    delete component;
}

TEST(Core_TestConstantEliminationBolus, ResidualErrorModelExponential)
{
    BuildConstantElimination builder;
    auto drugModel = builder.buildDrugModel(ResidualErrorType::EXPONENTIAL, std::vector<Value>({0.2}));

    ASSERT_TRUE(drugModel != nullptr);

    ASSERT_TRUE(drugModel->checkInvariants());

    DrugModelChecker checker;

    std::shared_ptr<PkModel> sharedPkModel;
    sharedPkModel = std::make_shared<PkModel>("test.constantelimination", PkModel::AllowMultipleRoutes::No);

    bool addResult = sharedPkModel->addIntakeIntervalCalculatorFactory(
            AbsorptionModel::Extravascular, ConstantEliminationBolus::getCreator());
    ASSERT_TRUE(addResult);
    sharedPkModel->addParameterList(AbsorptionModel::Extravascular, ConstantEliminationBolus::getParametersId());

    std::shared_ptr<PkModelCollection> collection = std::make_shared<PkModelCollection>();
    collection->addPkModel(sharedPkModel);
    DrugModelChecker::CheckerResult_t checkerResult = checker.checkDrugModel(drugModel.get(), collection.get());

    ASSERT_TRUE(checkerResult.m_ok);

    if (!checkerResult.m_ok) {
        std::cout << checkerResult.m_errorMessage << std::endl;
    }

    // Now the drug model is ready to be used


    IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

    ASSERT_TRUE(component != nullptr);

    static_cast<ComputingComponent*>(component)->setPkModelCollection(collection);


    {
        const FormulationAndRoute route(Formulation::OralSolution, AdministrationRoute::Oral);

        DateTime startSept2018(
                date::year_month_day(date::year(2018), date::month(9), date::day(1)),
                Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));

        auto drugTreatment = buildDrugTreatment(route, startSept2018);

        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covS", "0.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covA", "0.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covR", "0.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));

        RequestResponseId requestResponseId = "1";
        Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
        Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
        double nbPointsPerHour = 10.0;
        ComputingOption computingOption(
                PredictionParameterType::Apriori,
                CompartmentsOption::MainCompartment,
                RetrieveStatisticsOption::RetrieveStatistics,
                RetrieveParametersOption::RetrieveParameters,
                RetrieveCovariatesOption::RetrieveCovariates);

        std::unique_ptr<ComputingTraitPercentiles> traitsPercentiles = std::make_unique<ComputingTraitPercentiles>(
                requestResponseId, start, end, percentileRanks, nbPointsPerHour, computingOption);

        ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(traitsPercentiles));

        std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

        ComputingStatus result;
        result = component->compute(request, response);

        ASSERT_EQ(result, ComputingStatus::Ok);

        const ComputedData* responseData = response->getData();
        ASSERT_TRUE(dynamic_cast<const PercentilesData*>(responseData) != nullptr);
        const PercentilesData* resp = dynamic_cast<const PercentilesData*>(responseData);

        ASSERT_EQ(resp->getNbRanks(), percentileRanks.size());

        for (size_t p = 0; p < resp->getNbRanks(); p++) {
            DateTime statTime = DateTime::now();
            Value statValue = 0.0;
            resp->getData(p, 0).m_statistics.getStatistic(0, CycleStatisticType::Mean).getValue(statTime, statValue);
            //ASSERT_DOUBLE_EQ(statValue, 200001.0);

            // Multiply the Inv CDF by SD (0.2)
            double expectedValue = 200000.0 * std::exp(invCdf[p] * 0.2);
            ASSERT_PRED4(double_eq_rel_abs, statValue, expectedValue, 0.05, 0.0);
        }
    }

    // Delete all dynamically allocated objects
    delete component;
}

TEST(Core_TestConstantEliminationBolus, ResidualErrorModelProportional)
{
    BuildConstantElimination builder;
    auto drugModel = builder.buildDrugModel(ResidualErrorType::PROPORTIONAL, std::vector<Value>({0.2}));

    ASSERT_TRUE(drugModel != nullptr);

    ASSERT_TRUE(drugModel->checkInvariants());

    DrugModelChecker checker;

    std::shared_ptr<PkModel> sharedPkModel;
    sharedPkModel = std::make_shared<PkModel>("test.constantelimination", PkModel::AllowMultipleRoutes::No);

    bool addResult = sharedPkModel->addIntakeIntervalCalculatorFactory(
            AbsorptionModel::Extravascular, ConstantEliminationBolus::getCreator());
    ASSERT_TRUE(addResult);
    sharedPkModel->addParameterList(AbsorptionModel::Extravascular, ConstantEliminationBolus::getParametersId());

    std::shared_ptr<PkModelCollection> collection = std::make_shared<PkModelCollection>();
    collection->addPkModel(sharedPkModel);
    DrugModelChecker::CheckerResult_t checkerResult = checker.checkDrugModel(drugModel.get(), collection.get());

    ASSERT_TRUE(checkerResult.m_ok);

    if (!checkerResult.m_ok) {
        std::cout << checkerResult.m_errorMessage << std::endl;
    }

    // Now the drug model is ready to be used


    IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

    ASSERT_TRUE(component != nullptr);

    static_cast<ComputingComponent*>(component)->setPkModelCollection(collection);

    {
        const FormulationAndRoute route(Formulation::OralSolution, AdministrationRoute::Oral);

        DateTime startSept2018(
                date::year_month_day(date::year(2018), date::month(9), date::day(1)),
                Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));

        auto drugTreatment = buildDrugTreatment(route, startSept2018);

        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covS", "0.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covA", "0.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covR", "0.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));

        RequestResponseId requestResponseId = "1";
        Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
        Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
        double nbPointsPerHour = 10.0;
        ComputingOption computingOption(
                PredictionParameterType::Apriori,
                CompartmentsOption::MainCompartment,
                RetrieveStatisticsOption::RetrieveStatistics,
                RetrieveParametersOption::RetrieveParameters,
                RetrieveCovariatesOption::RetrieveCovariates);

        std::unique_ptr<ComputingTraitPercentiles> traitsPercentiles = std::make_unique<ComputingTraitPercentiles>(
                requestResponseId, start, end, percentileRanks, nbPointsPerHour, computingOption);

        ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(traitsPercentiles));

        std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

        ComputingStatus result;
        result = component->compute(request, response);

        ASSERT_EQ(result, ComputingStatus::Ok);

        const ComputedData* responseData = response->getData();
        ASSERT_TRUE(dynamic_cast<const PercentilesData*>(responseData) != nullptr);
        const PercentilesData* resp = dynamic_cast<const PercentilesData*>(responseData);

        ASSERT_EQ(resp->getNbRanks(), percentileRanks.size());

        for (size_t p = 0; p < resp->getNbRanks(); p++) {
            DateTime statTime = DateTime::now();
            Value statValue = 0.0;
            resp->getData(p, 0).m_statistics.getStatistic(0, CycleStatisticType::Mean).getValue(statTime, statValue);
            //ASSERT_DOUBLE_EQ(statValue, 200001.0);

            // Multiply the Inv CDF by SD (0.2)
            double expectedValue = 200000.0 * (1 + invCdf[p] * 0.2);
            ASSERT_PRED4(double_eq_rel_abs, statValue, expectedValue, 0.05, 0.0);
        }
    }

    // Delete all dynamically allocated objects
    delete component;
}

TEST(Core_TestConstantEliminationBolus, ResidualErrorModelMixed)
{
    BuildConstantElimination builder;
    auto drugModel = builder.buildDrugModel(ResidualErrorType::MIXED, std::vector<Value>({10.0, 0.2}));

    ASSERT_TRUE(drugModel != nullptr);

    ASSERT_TRUE(drugModel->checkInvariants());

    DrugModelChecker checker;

    std::shared_ptr<PkModel> sharedPkModel;
    sharedPkModel = std::make_shared<PkModel>("test.constantelimination", PkModel::AllowMultipleRoutes::No);

    bool addResult = sharedPkModel->addIntakeIntervalCalculatorFactory(
            AbsorptionModel::Extravascular, ConstantEliminationBolus::getCreator());
    ASSERT_TRUE(addResult);
    sharedPkModel->addParameterList(AbsorptionModel::Extravascular, ConstantEliminationBolus::getParametersId());

    std::shared_ptr<PkModelCollection> collection = std::make_shared<PkModelCollection>();
    collection->addPkModel(sharedPkModel);
    DrugModelChecker::CheckerResult_t checkerResult = checker.checkDrugModel(drugModel.get(), collection.get());

    ASSERT_TRUE(checkerResult.m_ok);

    if (!checkerResult.m_ok) {
        std::cout << checkerResult.m_errorMessage << std::endl;
    }

    // Now the drug model is ready to be used


    IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

    ASSERT_TRUE(component != nullptr);

    static_cast<ComputingComponent*>(component)->setPkModelCollection(collection);


    {
        const FormulationAndRoute route(Formulation::OralSolution, AdministrationRoute::Oral);

        DateTime startSept2018(
                date::year_month_day(date::year(2018), date::month(9), date::day(1)),
                Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));

        auto drugTreatment = buildDrugTreatment(route, startSept2018);

        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covS", "0.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covA", "0.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covR", "0.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));

        RequestResponseId requestResponseId = "1";
        Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
        Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
        double nbPointsPerHour = 10.0;
        ComputingOption computingOption(
                PredictionParameterType::Apriori,
                CompartmentsOption::MainCompartment,
                RetrieveStatisticsOption::RetrieveStatistics,
                RetrieveParametersOption::RetrieveParameters,
                RetrieveCovariatesOption::RetrieveCovariates);

        std::unique_ptr<ComputingTraitPercentiles> traitsPercentiles = std::make_unique<ComputingTraitPercentiles>(
                requestResponseId, start, end, percentileRanks, nbPointsPerHour, computingOption);

        ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(traitsPercentiles));

        std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

        ComputingStatus result;
        result = component->compute(request, response);

        ASSERT_EQ(result, ComputingStatus::Ok);

        const ComputedData* responseData = response->getData();
        ASSERT_TRUE(dynamic_cast<const PercentilesData*>(responseData) != nullptr);
        const PercentilesData* resp = dynamic_cast<const PercentilesData*>(responseData);

        ASSERT_EQ(resp->getNbRanks(), percentileRanks.size());

        for (size_t p = 0; p < resp->getNbRanks(); p++) {
            DateTime statTime = DateTime::now();
            Value statValue = 0.0;
            resp->getData(p, 0).m_statistics.getStatistic(0, CycleStatisticType::Mean).getValue(statTime, statValue);
            //ASSERT_DOUBLE_EQ(statValue, 200001.0);

            // Multiply the Inv CDF by SD (0.2)
            double expectedValue = 200000.0 + invCdf[p] * std::sqrt(std::pow(200000.0 * 0.2, 2));
            ASSERT_PRED4(double_eq_rel_abs, statValue, expectedValue, 0.04, 0.0);
        }
    }

    // Delete all dynamically allocated objects
    delete component;
}

TEST(Core_TestConstantEliminationBolus, ParamAdditive)
{
    BuildConstantElimination builder;
    auto drugModel = builder.buildDrugModel(
            ResidualErrorType::NONE,
            std::vector<Value>({0.0}),
            ParameterVariabilityType::Additive,
            ParameterVariabilityType::None,
            ParameterVariabilityType::None,
            ParameterVariabilityType::None,
            1000.0,
            0.0,
            0.0,
            0.0);

    ASSERT_TRUE(drugModel != nullptr);

    ASSERT_TRUE(drugModel->checkInvariants());

    DrugModelChecker checker;

    std::shared_ptr<PkModel> sharedPkModel;
    sharedPkModel = std::make_shared<PkModel>("test.constantelimination", PkModel::AllowMultipleRoutes::No);

    bool addResult = sharedPkModel->addIntakeIntervalCalculatorFactory(
            AbsorptionModel::Extravascular, ConstantEliminationBolus::getCreator());
    ASSERT_TRUE(addResult);
    sharedPkModel->addParameterList(AbsorptionModel::Extravascular, ConstantEliminationBolus::getParametersId());

    std::shared_ptr<PkModelCollection> collection = std::make_shared<PkModelCollection>();
    collection->addPkModel(sharedPkModel);
    DrugModelChecker::CheckerResult_t checkerResult = checker.checkDrugModel(drugModel.get(), collection.get());

    ASSERT_TRUE(checkerResult.m_ok);

    if (!checkerResult.m_ok) {
        std::cout << checkerResult.m_errorMessage << std::endl;
    }

    // Now the drug model is ready to be used


    IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

    ASSERT_TRUE(component != nullptr);

    static_cast<ComputingComponent*>(component)->setPkModelCollection(collection);


    {
        const FormulationAndRoute route(Formulation::OralSolution, AdministrationRoute::Oral);

        DateTime startSept2018(
                date::year_month_day(date::year(2018), date::month(9), date::day(1)),
                Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));

        auto drugTreatment = buildDrugTreatment(route, startSept2018);

        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covS", "0.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covA", "0.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covR", "0.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));

        RequestResponseId requestResponseId = "1";
        Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
        Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
        double nbPointsPerHour = 10.0;
        ComputingOption computingOption(
                PredictionParameterType::Apriori,
                CompartmentsOption::MainCompartment,
                RetrieveStatisticsOption::RetrieveStatistics,
                RetrieveParametersOption::RetrieveParameters,
                RetrieveCovariatesOption::RetrieveCovariates);

        std::unique_ptr<ComputingTraitPercentiles> traitsPercentiles = std::make_unique<ComputingTraitPercentiles>(
                requestResponseId, start, end, percentileRanks, nbPointsPerHour, computingOption);

        ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(traitsPercentiles));

        std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

        ComputingStatus result;
        result = component->compute(request, response);

        ASSERT_EQ(result, ComputingStatus::Ok);

        const ComputedData* responseData = response->getData();
        ASSERT_TRUE(dynamic_cast<const PercentilesData*>(responseData) != nullptr);
        const PercentilesData* resp = dynamic_cast<const PercentilesData*>(responseData);

        ASSERT_EQ(resp->getNbRanks(), percentileRanks.size());

        for (size_t p = 0; p < resp->getNbRanks(); p++) {
            DateTime statTime = DateTime::now();
            Value statValue = 0.0;
            resp->getData(p, 0).m_statistics.getStatistic(0, CycleStatisticType::Mean).getValue(statTime, statValue);
            //ASSERT_DOUBLE_EQ(statValue, 200001.0);

            // Multiply the Inv CDF by SD (10), and by 1000.0 because of mg/l
            double expectedValue = 200000.0 + invCdf[p] * 1000.0;
            ASSERT_PRED4(double_eq_rel_abs, statValue, expectedValue, .02, 0.02);
        }
    }

    // Delete all dynamically allocated objects
    delete component;
}

TEST(Core_TestConstantEliminationBolus, ParamAdditiveResidualErrorModelAdditive)
{
    BuildConstantElimination builder;
    auto drugModel = builder.buildDrugModel(
            ResidualErrorType::ADDITIVE,
            std::vector<Value>({10000.0}),
            ParameterVariabilityType::Additive,
            ParameterVariabilityType::None,
            ParameterVariabilityType::None,
            ParameterVariabilityType::None,
            1000.0,
            0.0,
            0.0,
            0.0);

    ASSERT_TRUE(drugModel != nullptr);

    ASSERT_TRUE(drugModel->checkInvariants());

    DrugModelChecker checker;

    std::shared_ptr<PkModel> sharedPkModel;
    sharedPkModel = std::make_shared<PkModel>("test.constantelimination", PkModel::AllowMultipleRoutes::No);

    bool addResult = sharedPkModel->addIntakeIntervalCalculatorFactory(
            AbsorptionModel::Extravascular, ConstantEliminationBolus::getCreator());
    ASSERT_TRUE(addResult);
    sharedPkModel->addParameterList(AbsorptionModel::Extravascular, ConstantEliminationBolus::getParametersId());

    std::shared_ptr<PkModelCollection> collection = std::make_shared<PkModelCollection>();
    collection->addPkModel(sharedPkModel);
    DrugModelChecker::CheckerResult_t checkerResult = checker.checkDrugModel(drugModel.get(), collection.get());

    ASSERT_TRUE(checkerResult.m_ok);

    if (!checkerResult.m_ok) {
        std::cout << checkerResult.m_errorMessage << std::endl;
    }

    // Now the drug model is ready to be used


    IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

    ASSERT_TRUE(component != nullptr);

    static_cast<ComputingComponent*>(component)->setPkModelCollection(collection);


    {
        const FormulationAndRoute route(Formulation::OralSolution, AdministrationRoute::Oral);

        DateTime startSept2018(
                date::year_month_day(date::year(2018), date::month(9), date::day(1)),
                Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));

        auto drugTreatment = buildDrugTreatment(route, startSept2018);

        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covS", "0.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covA", "0.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covR", "0.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));

        RequestResponseId requestResponseId = "1";
        Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
        Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
        double nbPointsPerHour = 10.0;
        ComputingOption computingOption(
                PredictionParameterType::Apriori,
                CompartmentsOption::MainCompartment,
                RetrieveStatisticsOption::RetrieveStatistics,
                RetrieveParametersOption::RetrieveParameters,
                RetrieveCovariatesOption::RetrieveCovariates);

        std::unique_ptr<ComputingTraitPercentiles> traitsPercentiles = std::make_unique<ComputingTraitPercentiles>(
                requestResponseId, start, end, percentileRanks, nbPointsPerHour, computingOption);

        ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(traitsPercentiles));

        std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

        ComputingStatus result;
        result = component->compute(request, response);

        ASSERT_EQ(result, ComputingStatus::Ok);

        const ComputedData* responseData = response->getData();
        ASSERT_TRUE(dynamic_cast<const PercentilesData*>(responseData) != nullptr);
        const PercentilesData* resp = dynamic_cast<const PercentilesData*>(responseData);

        ASSERT_EQ(resp->getNbRanks(), percentileRanks.size());

        for (size_t p = 0; p < resp->getNbRanks(); p++) {
            DateTime statTime = DateTime::now();
            Value statValue = 0.0;
            resp->getData(p, 0).m_statistics.getStatistic(0, CycleStatisticType::Mean).getValue(statTime, statValue);
            //ASSERT_DOUBLE_EQ(statValue, 200001.0);

            // Multiply the Inv CDF by SD (10), and by 1000.0 because of mg/l
            double expectedValue = 200000.0 + invCdf[p] * std::sqrt(std::pow(1000.0, 2) + std::pow(10.0 * 1000.0, 2));

            ASSERT_PRED4(double_eq_rel_abs, statValue, expectedValue, .01, 0.01);
        }
    }

    // Delete all dynamically allocated objects
    delete component;
}

TEST(Core_TestConstantEliminationBolus, ParamExponentialResidualErrorModelExponential)
{
    BuildConstantElimination builder;
    auto drugModel = builder.buildDrugModel(
            ResidualErrorType::EXPONENTIAL,
            std::vector<Value>({0.3}),
            ParameterVariabilityType::None,
            ParameterVariabilityType::Exponential,
            ParameterVariabilityType::None,
            ParameterVariabilityType::None,
            0.0,
            0.2,
            0.0,
            0.0);

    ASSERT_TRUE(drugModel != nullptr);

    ASSERT_TRUE(drugModel->checkInvariants());

    DrugModelChecker checker;

    std::shared_ptr<PkModel> sharedPkModel;
    sharedPkModel = std::make_shared<PkModel>("test.constantelimination", PkModel::AllowMultipleRoutes::No);

    bool addResult = sharedPkModel->addIntakeIntervalCalculatorFactory(
            AbsorptionModel::Extravascular, ConstantEliminationBolus::getCreator());
    ASSERT_TRUE(addResult);
    sharedPkModel->addParameterList(AbsorptionModel::Extravascular, ConstantEliminationBolus::getParametersId());

    std::shared_ptr<PkModelCollection> collection = std::make_shared<PkModelCollection>();
    collection->addPkModel(sharedPkModel);
    DrugModelChecker::CheckerResult_t checkerResult = checker.checkDrugModel(drugModel.get(), collection.get());

    ASSERT_TRUE(checkerResult.m_ok);

    if (!checkerResult.m_ok) {
        std::cout << checkerResult.m_errorMessage << std::endl;
    }

    // Now the drug model is ready to be used


    IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

    ASSERT_TRUE(component != nullptr);

    static_cast<ComputingComponent*>(component)->setPkModelCollection(collection);


    {
        const FormulationAndRoute route(Formulation::OralSolution, AdministrationRoute::Oral);

        DateTime startSept2018(
                date::year_month_day(date::year(2018), date::month(9), date::day(1)),
                Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));

        auto drugTreatment = buildDrugTreatment(route, startSept2018);

        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covS", "0.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covA", "0.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covR", "0.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));

        RequestResponseId requestResponseId = "1";
        Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
        Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
        double nbPointsPerHour = 10.0;
        ComputingOption computingOption(
                PredictionParameterType::Apriori,
                CompartmentsOption::MainCompartment,
                RetrieveStatisticsOption::RetrieveStatistics,
                RetrieveParametersOption::RetrieveParameters,
                RetrieveCovariatesOption::RetrieveCovariates);

        std::unique_ptr<ComputingTraitPercentiles> traitsPercentiles = std::make_unique<ComputingTraitPercentiles>(
                requestResponseId, start, end, percentileRanks, nbPointsPerHour, computingOption);

        ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(traitsPercentiles));

        std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

        ComputingStatus result;
        result = component->compute(request, response);

        ASSERT_EQ(result, ComputingStatus::Ok);

        const ComputedData* responseData = response->getData();
        ASSERT_TRUE(dynamic_cast<const PercentilesData*>(responseData) != nullptr);
        const PercentilesData* resp = dynamic_cast<const PercentilesData*>(responseData);

        ASSERT_EQ(resp->getNbRanks(), percentileRanks.size());

        for (size_t p = 0; p < resp->getNbRanks(); p++) {
            DateTime statTime = DateTime::now();
            Value statValue = 0.0;
            resp->getData(p, 0).m_statistics.getStatistic(0, CycleStatisticType::Mean).getValue(statTime, statValue);

            // Calculate the resulting standard deviation
            double newStd = std::sqrt(std::pow(0.2, 2) + std::pow(0.3, 2));

            // Multiply the Inv CDF by the new standard deviation
            double expectedValue = 200000.0 * std::exp(invCdf[p] * newStd);
            ASSERT_PRED4(double_eq_rel_abs, statValue, expectedValue, 0.05, 0.0);
        }
    }

    // Delete all dynamically allocated objects
    delete component;
}

TEST(Core_TestConstantEliminationBolus, ParamProportionalResidualErrorModelProportional)
{
    BuildConstantElimination builder;
    auto drugModel = builder.buildDrugModel(
            ResidualErrorType::PROPORTIONAL,
            std::vector<Value>({0.3}),
            ParameterVariabilityType::None,
            ParameterVariabilityType::Proportional,
            ParameterVariabilityType::None,
            ParameterVariabilityType::None,
            0.0,
            0.2,
            0.0,
            0.0);

    ASSERT_TRUE(drugModel != nullptr);

    ASSERT_TRUE(drugModel->checkInvariants());

    DrugModelChecker checker;

    std::shared_ptr<PkModel> sharedPkModel;
    sharedPkModel = std::make_shared<PkModel>("test.constantelimination", PkModel::AllowMultipleRoutes::No);

    bool addResult = sharedPkModel->addIntakeIntervalCalculatorFactory(
            AbsorptionModel::Extravascular, ConstantEliminationBolus::getCreator());
    ASSERT_TRUE(addResult);
    sharedPkModel->addParameterList(AbsorptionModel::Extravascular, ConstantEliminationBolus::getParametersId());

    std::shared_ptr<PkModelCollection> collection = std::make_shared<PkModelCollection>();
    collection->addPkModel(sharedPkModel);
    DrugModelChecker::CheckerResult_t checkerResult = checker.checkDrugModel(drugModel.get(), collection.get());

    ASSERT_TRUE(checkerResult.m_ok);

    if (!checkerResult.m_ok) {
        std::cout << checkerResult.m_errorMessage << std::endl;
    }

    // Now the drug model is ready to be used


    IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

    ASSERT_TRUE(component != nullptr);

    static_cast<ComputingComponent*>(component)->setPkModelCollection(collection);


    {
        const FormulationAndRoute route(Formulation::OralSolution, AdministrationRoute::Oral);

        DateTime startSept2018(
                date::year_month_day(date::year(2018), date::month(9), date::day(1)),
                Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));

        auto drugTreatment = buildDrugTreatment(route, startSept2018);

        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covS", "0.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covA", "0.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covR", "0.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));

        RequestResponseId requestResponseId = "1";
        Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
        Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
        double nbPointsPerHour = 10.0;
        ComputingOption computingOption(
                PredictionParameterType::Apriori,
                CompartmentsOption::MainCompartment,
                RetrieveStatisticsOption::RetrieveStatistics,
                RetrieveParametersOption::RetrieveParameters,
                RetrieveCovariatesOption::RetrieveCovariates);

        std::unique_ptr<ComputingTraitPercentiles> traitsPercentiles = std::make_unique<ComputingTraitPercentiles>(
                requestResponseId, start, end, percentileRanks, nbPointsPerHour, computingOption);

        ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(traitsPercentiles));

        std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

        ComputingStatus result;
        result = component->compute(request, response);

        ASSERT_EQ(result, ComputingStatus::Ok);

        const ComputedData* responseData = response->getData();
        ASSERT_TRUE(dynamic_cast<const PercentilesData*>(responseData) != nullptr);
        const PercentilesData* resp = dynamic_cast<const PercentilesData*>(responseData);

        ASSERT_EQ(resp->getNbRanks(), percentileRanks.size());

        for (size_t p = 0; p < resp->getNbRanks(); p++) {
            DateTime statTime = DateTime::now();
            Value statValue = 0.0;
            resp->getData(p, 0).m_statistics.getStatistic(0, CycleStatisticType::Mean).getValue(statTime, statValue);
            //ASSERT_DOUBLE_EQ(statValue, 200001.0);


            // Calculate the resulting standard deviation
            //double newStd = std::sqrt(1.0 / ( 1.0 /std::pow(0.2, 2) + 1.0 / std::pow(0.3, 2)));
            // double newStd = std::sqrt(std::pow(0.2, 2) * std::pow(0.3, 2) / ( std::pow(0.2, 2) + std::pow(0.3, 2)));
            //double newStd = std::sqrt(std::pow(0.2, 2) * std::pow(0.3, 2) / ( std::pow(0.2, 2) + std::pow(0.3, 2)));
            //std::cout << newStd << std::endl;


            // Multiply the Inv CDF by the new standard deviation
            // double expectedValue = 200000.0 * ( 1.0 + invCdf[p] * newStd);

            // YTA : I do not know how to calculate the product of two Normal variables

            //ASSERT_PRED4(double_eq_rel_abs, statValue, expectedValue, 0.02, 0.0);
        }
    }

    // Delete all dynamically allocated objects
    delete component;
}

TEST(Core_TestConstantEliminationBolus, Adjustments)
{
    BuildConstantElimination builder;
    auto drugModel = builder.buildDrugModel();

    ASSERT_TRUE(drugModel != nullptr);


    // Add targets
    auto target = std::make_unique<TargetDefinition>(
            TargetType::Residual,
            Unit("mg/l"),
            ActiveMoietyId("analyte"),
            std::make_unique<SubTargetDefinition>("cMin", 750.0, nullptr),
            std::make_unique<SubTargetDefinition>("cMax", 1500.0, nullptr),
            std::make_unique<SubTargetDefinition>("cBest", 1000.0, nullptr),
            std::make_unique<SubTargetDefinition>("mic", 0.0, nullptr),
            std::make_unique<SubTargetDefinition>("tMin", 1000.0, nullptr),
            std::make_unique<SubTargetDefinition>("tMax", 1000.0, nullptr),
            std::make_unique<SubTargetDefinition>("tBest", 1000.0, nullptr),
            std::make_unique<SubTargetDefinition>("toxicity", 10000.0, nullptr),
            std::make_unique<SubTargetDefinition>("inefficacy", 000.0, nullptr));

    drugModel->m_activeMoieties[0]->addTarget(std::move(target));

    ASSERT_TRUE(drugModel->checkInvariants());

    DrugModelChecker checker;

    std::shared_ptr<PkModel> sharedPkModel;
    sharedPkModel = std::make_shared<PkModel>("test.constantelimination", PkModel::AllowMultipleRoutes::No);

    bool addResult = sharedPkModel->addIntakeIntervalCalculatorFactory(
            AbsorptionModel::Extravascular, ConstantEliminationBolus::getCreator());
    ASSERT_TRUE(addResult);
    sharedPkModel->addParameterList(AbsorptionModel::Extravascular, ConstantEliminationBolus::getParametersId());

    std::shared_ptr<PkModelCollection> collection = std::make_shared<PkModelCollection>();
    collection->addPkModel(sharedPkModel);
    DrugModelChecker::CheckerResult_t checkerResult = checker.checkDrugModel(drugModel.get(), collection.get());

    ASSERT_TRUE(checkerResult.m_ok);

    if (!checkerResult.m_ok) {
        std::cout << checkerResult.m_errorMessage << std::endl;
    }

    // Now the drug model is ready to be used


    IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

    ASSERT_TRUE(component != nullptr);

    static_cast<ComputingComponent*>(component)->setPkModelCollection(collection);


    {
        const FormulationAndRoute route(Formulation::OralSolution, AdministrationRoute::Oral);

        DateTime startSept2018(
                date::year_month_day(date::year(2018), date::month(9), date::day(1)),
                Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));

        auto drugTreatment = buildDrugTreatment(route, startSept2018);

        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covS", "0.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covA", "0.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covR", "0.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));

        RequestResponseId requestResponseId = "1";
        Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
        Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
        double nbPointsPerHour = 10.0;
        ComputingOption computingOption(
                PredictionParameterType::Apriori,
                CompartmentsOption::MainCompartment,
                RetrieveStatisticsOption::RetrieveStatistics,
                RetrieveParametersOption::RetrieveParameters,
                RetrieveCovariatesOption::RetrieveCovariates);

        Tucuxi::Common::DateTime adjustmentTime(2018_y / sep / 3, 8h + 0min);
        BestCandidatesOption adjustmentOption = BestCandidatesOption::AllDosages;
        LoadingOption loadingOption = LoadingOption::NoLoadingDose;
        RestPeriodOption restPeriodOption = RestPeriodOption::NoRestPeriod;
        SteadyStateTargetOption steadyStateTargetOption = SteadyStateTargetOption::AtSteadyState;
        TargetExtractionOption targetExtractionOption = TargetExtractionOption::PopulationValues;
        FormulationAndRouteSelectionOption formulationAndRouteOption =
                FormulationAndRouteSelectionOption::LastFormulationAndRoute;

        std::unique_ptr<ComputingTraitAdjustment> traits = std::make_unique<ComputingTraitAdjustment>(
                requestResponseId,
                start,
                end,
                nbPointsPerHour,
                computingOption,
                adjustmentTime,
                adjustmentOption,
                loadingOption,
                restPeriodOption,
                steadyStateTargetOption,
                targetExtractionOption,
                formulationAndRouteOption);


        ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(traits));

        std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

        ComputingStatus result;
        result = component->compute(request, response);

        ASSERT_EQ(result, ComputingStatus::Ok);

        const ComputedData* responseData = response->getData();
        ASSERT_TRUE(dynamic_cast<const AdjustmentData*>(responseData) != nullptr);
        const AdjustmentData* resp = dynamic_cast<const AdjustmentData*>(responseData);

        ASSERT_EQ(resp->getAdjustments().size(), static_cast<size_t>(9));

        for (auto& adjustment : resp->getAdjustments()) {

            ASSERT_EQ(adjustment.m_history.getDosageTimeRanges().size(), static_cast<size_t>(1));

            for (const auto& timeRange : adjustment.m_history.getDosageTimeRanges()) {
                const DosageRepeat* dosageRepeat = dynamic_cast<const DosageRepeat*>(timeRange->getDosage());
                ASSERT_TRUE(dosageRepeat != nullptr);
                const SingleDose* dosage = dynamic_cast<const SingleDose*>(dosageRepeat->getDosage());
                ASSERT_TRUE(dosage != nullptr);
                ASSERT_PRED4(double_ge_rel_abs, dosage->getDose(), 750.0, 0.01, 0.01);
                ASSERT_PRED4(double_le_rel_abs, dosage->getDose(), 1500.0, 0.01, 0.01);
            }
        }
    }

    // Delete all dynamically allocated objects
    delete component;
}

TEST(Core_TestConstantEliminationBolus, Adjustments2)
{
    BuildConstantElimination builder;
    auto drugModel = builder.buildDrugModel();

    ASSERT_TRUE(drugModel != nullptr);


    // Add targets
    auto target = std::make_unique<TargetDefinition>(
            TargetType::CumulativeAuc,
            Unit("mg*h/l"),
            ActiveMoietyId("analyte"),
            std::make_unique<SubTargetDefinition>("cMin", 4.0 * 24.0 * 250.0, nullptr),
            std::make_unique<SubTargetDefinition>("cMax", 4.0 * 24.0 * 500.0, nullptr),
            std::make_unique<SubTargetDefinition>("cBest", 4.0 * 24.0 * 300.0, nullptr),
            std::make_unique<SubTargetDefinition>("mic", 0.0, nullptr),
            std::make_unique<SubTargetDefinition>("tMin", 1000.0, nullptr),
            std::make_unique<SubTargetDefinition>("tMax", 1000.0, nullptr),
            std::make_unique<SubTargetDefinition>("tBest", 1000.0, nullptr),
            std::make_unique<SubTargetDefinition>("toxicity", 10000.0, nullptr),
            std::make_unique<SubTargetDefinition>("inefficacy", 000.0, nullptr));

    drugModel->m_activeMoieties[0]->addTarget(std::move(target));

    ASSERT_TRUE(drugModel->checkInvariants());

    DrugModelChecker checker;

    std::shared_ptr<PkModel> sharedPkModel;
    sharedPkModel = std::make_shared<PkModel>("test.constantelimination", PkModel::AllowMultipleRoutes::No);

    bool addResult = sharedPkModel->addIntakeIntervalCalculatorFactory(
            AbsorptionModel::Extravascular, ConstantEliminationBolus::getCreator());
    ASSERT_TRUE(addResult);
    sharedPkModel->addParameterList(AbsorptionModel::Extravascular, ConstantEliminationBolus::getParametersId());

    std::shared_ptr<PkModelCollection> collection = std::make_shared<PkModelCollection>();
    collection->addPkModel(sharedPkModel);
    DrugModelChecker::CheckerResult_t checkerResult = checker.checkDrugModel(drugModel.get(), collection.get());

    ASSERT_TRUE(checkerResult.m_ok);

    if (!checkerResult.m_ok) {
        std::cout << checkerResult.m_errorMessage << std::endl;
    }

    // Now the drug model is ready to be used


    IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

    ASSERT_TRUE(component != nullptr);

    static_cast<ComputingComponent*>(component)->setPkModelCollection(collection);


    {
        const FormulationAndRoute route(Formulation::OralSolution, AdministrationRoute::Oral);

        DateTime startSept2018(
                date::year_month_day(date::year(2018), date::month(9), date::day(1)),
                Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));

        auto drugTreatment = buildDrugTreatment(route, startSept2018);

        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covS", "0.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covA", "0.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covR", "0.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));

        RequestResponseId requestResponseId = "1";
        Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
        Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
        double nbPointsPerHour = 10.0;
        ComputingOption computingOption(
                PredictionParameterType::Apriori,
                CompartmentsOption::MainCompartment,
                RetrieveStatisticsOption::RetrieveStatistics,
                RetrieveParametersOption::RetrieveParameters,
                RetrieveCovariatesOption::RetrieveCovariates);

        Tucuxi::Common::DateTime adjustmentTime(2018_y / sep / 3, 8h + 0min);
        BestCandidatesOption adjustmentOption = BestCandidatesOption::AllDosages;
        LoadingOption loadingOption = LoadingOption::NoLoadingDose;
        RestPeriodOption restPeriodOption = RestPeriodOption::NoRestPeriod;
        SteadyStateTargetOption steadyStateTargetOption = SteadyStateTargetOption::WithinTreatmentTimeRange;
        TargetExtractionOption targetExtractionOption = TargetExtractionOption::PopulationValues;
        FormulationAndRouteSelectionOption formulationAndRouteOption =
                FormulationAndRouteSelectionOption::LastFormulationAndRoute;

        std::unique_ptr<ComputingTraitAdjustment> traits = std::make_unique<ComputingTraitAdjustment>(
                requestResponseId,
                start,
                end,
                nbPointsPerHour,
                computingOption,
                adjustmentTime,
                adjustmentOption,
                loadingOption,
                restPeriodOption,
                steadyStateTargetOption,
                targetExtractionOption,
                formulationAndRouteOption);

        ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(traits));

        std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

        ComputingStatus result;
        result = component->compute(request, response);

        ASSERT_EQ(result, ComputingStatus::Ok);

        const ComputedData* responseData = response->getData();
        ASSERT_TRUE(dynamic_cast<const AdjustmentData*>(responseData) != nullptr);
        const AdjustmentData* resp = dynamic_cast<const AdjustmentData*>(responseData);

        // 18 possibilities:
        // 3 intervals for doses 300, 400, 500, 600, 700, 800
        ASSERT_EQ(resp->getAdjustments().size(), static_cast<size_t>(18));

        for (auto& adjustment : resp->getAdjustments()) {

            ASSERT_EQ(adjustment.m_history.getDosageTimeRanges().size(), static_cast<size_t>(1));

            for (const auto& timeRange : adjustment.m_history.getDosageTimeRanges()) {
                const DosageRepeat* dosageRepeat = dynamic_cast<const DosageRepeat*>(timeRange->getDosage());
                ASSERT_TRUE(dosageRepeat != nullptr);
                const SingleDose* dosage = dynamic_cast<const SingleDose*>(dosageRepeat->getDosage());
                ASSERT_TRUE(dosage != nullptr);
                ASSERT_PRED4(double_ge_rel_abs, (200.0 + dosage->getDose()) / 2.0, 250.0, 0.01, 0.01);
                ASSERT_PRED4(double_le_rel_abs, (200.0 + dosage->getDose()) / 2.0, 500.0, 0.01, 0.01);
            }
        }
    }

    // Delete all dynamically allocated objects
    delete component;
}
