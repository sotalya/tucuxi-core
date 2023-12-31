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


#ifndef TEST_CONSTANTELIMINATIONBOLUS_H
#define TEST_CONSTANTELIMINATIONBOLUS_H

#include <chrono>
#include <iostream>
#include <math.h>
#include <memory>

#include <date/date.h>

#include "tucucore/computingcomponent.h"
#include "tucucore/computingservice/computingrequest.h"
#include "tucucore/drugmodelchecker.h"
#include "tucucore/drugmodelimport.h"
#include "tucucore/drugtreatment/drugtreatment.h"
#include "tucucore/drugtreatment/patientcovariate.h"
#include "tucucore/pkmodel.h"

#include "../pkmodels/constanteliminationbolus.h"
#include "../testutils.h"
#include "buildconstantelimination.h"
#include "fructose/fructose.h"

using namespace std::chrono_literals;
using namespace date;

using namespace Tucuxi::Core;

class GeneralTest
{
};

struct TestConstantEliminationBolus : public fructose::test_base<TestConstantEliminationBolus>, public GeneralTest
{
    TestConstantEliminationBolus() {}

    // The inv CDF is calculated using the probit() function in octave:
    // probit(0.05)
    // probit(0.1)
    // probit(0.25)
    // probit(0.5)
    // probit(0.75)
    // probit(0.9)
    // probit(0.95)
    std::vector<double> invCdf = {-1.6449, -1.2816, -0.67449, 0.0, 0.67449, 1.2816, 1.6449};


    std::vector<Value> percentileRanks = {5, 10, 25, 50, 75, 90, 95};

    std::unique_ptr<DrugTreatment> buildDrugTreatment(const FormulationAndRoute& _route)
    {

        auto drugTreatment = std::make_unique<DrugTreatment>();

        // List of time ranges that will be pushed into the history
        DosageTimeRangeList timeRangeList;

        // Create a time range starting at the beginning of june 2017, with no upper end (to test that the repetitions
        // are handled correctly)
        DateTime startSept2018(
                date::year_month_day(date::year(2018), date::month(9), date::day(1)),
                Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));


        //const FormulationAndRoute route("formulation", AdministrationRoute::IntravenousBolus, AbsorptionModel::Extravascular);
        // Add a treatment intake every ten days in June
        // 200mg via a intravascular at 08h30, starting the 01.06
        LastingDose periodicDose(DoseValue(200.0), TucuUnit("mg"), _route, Duration(), Duration(std::chrono::hours(6)));
        DosageRepeat repeatedDose(periodicDose, 16);
        auto sept2018 = std::make_unique<Tucuxi::Core::DosageTimeRange>(startSept2018, repeatedDose);


        drugTreatment->getModifiableDosageHistory().addTimeRange(*sept2018);

        return drugTreatment;
    }


    void test0()
    {
        BuildConstantElimination builder;
        auto drugModel = builder.buildDrugModel();

        fructose_assert(drugModel != nullptr);

        fructose_assert(drugModel->checkInvariants());

        DrugModelChecker checker;

        std::shared_ptr<PkModel> sharedPkModel;
        sharedPkModel = std::make_shared<PkModel>("test.constantelimination", PkModel::AllowMultipleRoutes::No);

        bool addResult = sharedPkModel->addIntakeIntervalCalculatorFactory(
                AbsorptionModel::Extravascular, ConstantEliminationBolus::getCreator());
        fructose_assert(addResult);

        sharedPkModel->addParameterList(AbsorptionModel::Extravascular, ConstantEliminationBolus::getParametersId());

        std::shared_ptr<PkModelCollection> collection = std::make_shared<PkModelCollection>();
        collection->addPkModel(sharedPkModel);
        DrugModelChecker::CheckerResult_t checkerResult = checker.checkDrugModel(drugModel.get(), collection.get());

        fructose_assert(checkerResult.m_ok);

        if (!checkerResult.m_ok) {
            std::cout << checkerResult.m_errorMessage << std::endl;
        }

        // Now the drug model is ready to be used


        IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

        fructose_assert(component != nullptr);

        static_cast<ComputingComponent*>(component)->setPkModelCollection(collection);


        {
            const FormulationAndRoute route(
                    Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);

            auto drugTreatment = buildDrugTreatment(route);

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

                fructose_assert_eq(result, ComputingStatus::Ok);

                const ComputedData* responseData = response->getData();
                fructose_assert(dynamic_cast<const PercentilesData*>(responseData) != nullptr);
                const PercentilesData* resp = dynamic_cast<const PercentilesData*>(responseData);

                fructose_assert_eq(resp->getNbRanks(), percentileRanks.size());

                for (size_t p = 0; p < resp->getNbRanks(); p++) {
                    DateTime statTime = DateTime::now();
                    Value statValue = 0.0;
                    resp->getData(p, 0)
                            .m_statistics.getStatistic(0, CycleStatisticType::Mean)
                            .getValue(statTime, statValue);
                    fructose_assert_double_eq(statValue, 200000.0);
                }
            }
            {
                ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(traits));

                std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

                ComputingStatus result;
                result = component->compute(request, response);

                fructose_assert_eq(result, ComputingStatus::Ok);

                const ComputedData* responseData = response->getData();
                fructose_assert(dynamic_cast<const SinglePredictionData*>(responseData) != nullptr);
                const SinglePredictionData* resp = dynamic_cast<const SinglePredictionData*>(responseData);

                fructose_assert_eq(resp->getCompartmentInfos().size(), size_t{1});
                fructose_assert_eq(resp->getCompartmentInfos()[0].getId(), "analyte");
                fructose_assert_eq(
                        resp->getCompartmentInfos()[0].getType(),
                        CompartmentInfo::CompartmentType::ActiveMoietyAndAnalyte);

                std::vector<CycleData> data = resp->getData();
                fructose_assert(data.size() == 16);
                fructose_assert(data[0].m_concentrations.size() == 1);
                fructose_assert_double_eq(data[0].m_concentrations[0][0], 200000.0);
                DateTime startSept2018(
                        date::year_month_day(date::year(2018), date::month(9), date::day(1)),
                        Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));

                fructose_assert_eq(
                        data[0].m_start.toSeconds() + data[0].m_times[0][0] * 3600.0, startSept2018.toSeconds());
                fructose_assert_eq(
                        data[1].m_start.toSeconds() + data[1].m_times[0][0] * 3600.0,
                        startSept2018.toSeconds() + 3600.0 * 6.0);

                DateTime statTime = DateTime::now();
                Value statValue = 0.0;
                data[0].m_statistics.getStatistic(0, CycleStatisticType::AUC).getValue(statTime, statValue);
                fructose_assert_double_eq(statValue, 200000.0 * 6.0);

                data[0].m_statistics.getStatistic(0, CycleStatisticType::CumulativeAuc).getValue(statTime, statValue);
                fructose_assert_double_eq(statValue, 200000.0 * 6.0);

                data[0].m_statistics.getStatistic(0, CycleStatisticType::Peak).getValue(statTime, statValue);
                fructose_assert_double_eq(statValue, 200000.0);

                data[0].m_statistics.getStatistic(0, CycleStatisticType::Mean).getValue(statTime, statValue);
                fructose_assert_double_eq(statValue, 200000.0);

                data[1].m_statistics.getStatistic(0, CycleStatisticType::AUC).getValue(statTime, statValue);
                fructose_assert_double_eq(statValue, 200000.0 * 6.0);

                data[1].m_statistics.getStatistic(0, CycleStatisticType::CumulativeAuc).getValue(statTime, statValue);
                fructose_assert_double_eq(statValue, 200000.0 * 12.0);

                data[1].m_statistics.getStatistic(0, CycleStatisticType::Peak).getValue(statTime, statValue);
                fructose_assert_double_eq(statValue, 200000.0);

                data[1].m_statistics.getStatistic(0, CycleStatisticType::Mean).getValue(statTime, statValue);
                fructose_assert_double_eq(statValue, 200000.0);
            }
        }

        // Delete all dynamically allocated objects
        delete component;
    }


    void test1()
    {
        BuildConstantElimination builder;
        auto drugModel = builder.buildDrugModel();

        fructose_assert(drugModel != nullptr);

        fructose_assert(drugModel->checkInvariants());

        DrugModelChecker checker;

        std::shared_ptr<PkModel> sharedPkModel;
        sharedPkModel = std::make_shared<PkModel>("test.constantelimination", PkModel::AllowMultipleRoutes::No);

        bool addResult = sharedPkModel->addIntakeIntervalCalculatorFactory(
                AbsorptionModel::Extravascular, ConstantEliminationBolus::getCreator());
        fructose_assert(addResult);

        sharedPkModel->addParameterList(AbsorptionModel::Extravascular, ConstantEliminationBolus::getParametersId());


        std::shared_ptr<PkModelCollection> collection = std::make_shared<PkModelCollection>();
        collection->addPkModel(sharedPkModel);
        DrugModelChecker::CheckerResult_t checkerResult = checker.checkDrugModel(drugModel.get(), collection.get());

        fructose_assert(checkerResult.m_ok);

        if (!checkerResult.m_ok) {
            std::cout << checkerResult.m_errorMessage << std::endl;
        }

        // Now the drug model is ready to be used


        IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

        fructose_assert(component != nullptr);

        static_cast<ComputingComponent*>(component)->setPkModelCollection(collection);


        {
            const FormulationAndRoute route(
                    Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);

            auto drugTreatment = buildDrugTreatment(route);

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

                fructose_assert_eq(result, ComputingStatus::Ok);

                const ComputedData* responseData = response->getData();
                fructose_assert(dynamic_cast<const PercentilesData*>(responseData) != nullptr);
                const PercentilesData* resp = dynamic_cast<const PercentilesData*>(responseData);

                fructose_assert_eq(resp->getNbRanks(), percentileRanks.size());

                for (size_t p = 0; p < resp->getNbRanks(); p++) {
                    DateTime statTime = DateTime::now();
                    Value statValue = 0.0;
                    resp->getData(p, 0)
                            .m_statistics.getStatistic(0, CycleStatisticType::Mean)
                            .getValue(statTime, statValue);
                    fructose_assert_double_eq(statValue, 200001.0);
                }
            }
            {
                ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(traits));

                std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

                ComputingStatus result;
                result = component->compute(request, response);

                fructose_assert_eq(result, ComputingStatus::Ok);


                const ComputedData* responseData = response->getData();
                fructose_assert(dynamic_cast<const SinglePredictionData*>(responseData) != nullptr);
                const SinglePredictionData* resp = dynamic_cast<const SinglePredictionData*>(responseData);

                fructose_assert_eq(resp->getCompartmentInfos().size(), size_t{1});
                fructose_assert_eq(resp->getCompartmentInfos()[0].getId(), "analyte");
                fructose_assert_eq(
                        resp->getCompartmentInfos()[0].getType(),
                        CompartmentInfo::CompartmentType::ActiveMoietyAndAnalyte);

                std::vector<CycleData> data = resp->getData();
                fructose_assert(data.size() == 16);
                fructose_assert(data[0].m_concentrations.size() == 1);
                fructose_assert_double_eq(data[0].m_concentrations[0][0], 200001.0);
                DateTime startSept2018(
                        date::year_month_day(date::year(2018), date::month(9), date::day(1)),
                        Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));

                fructose_assert_eq(
                        data[0].m_start.toSeconds() + data[0].m_times[0][0] * 3600.0, startSept2018.toSeconds());
                fructose_assert_eq(
                        data[1].m_start.toSeconds() + data[1].m_times[0][0] * 3600.0,
                        startSept2018.toSeconds() + 3600.0 * 6.0);

                DateTime statTime = DateTime::now();
                Value statValue = 0.0;
                data[0].m_statistics.getStatistic(0, CycleStatisticType::AUC).getValue(statTime, statValue);
                fructose_assert_double_eq(statValue, 200001.0 * 6.0);

                data[0].m_statistics.getStatistic(0, CycleStatisticType::CumulativeAuc).getValue(statTime, statValue);
                fructose_assert_double_eq(statValue, 200001.0 * 6.0);

                data[0].m_statistics.getStatistic(0, CycleStatisticType::Peak).getValue(statTime, statValue);
                fructose_assert_double_eq(statValue, 200001.0);

                data[0].m_statistics.getStatistic(0, CycleStatisticType::Mean).getValue(statTime, statValue);
                fructose_assert_double_eq(statValue, 200001.0);

                data[1].m_statistics.getStatistic(0, CycleStatisticType::AUC).getValue(statTime, statValue);
                fructose_assert_double_eq(statValue, 200001.0 * 6.0);

                data[1].m_statistics.getStatistic(0, CycleStatisticType::CumulativeAuc).getValue(statTime, statValue);
                fructose_assert_double_eq(statValue, 200001.0 * 12.0);

                data[1].m_statistics.getStatistic(0, CycleStatisticType::Peak).getValue(statTime, statValue);
                fructose_assert_double_eq(statValue, 200001.0);

                data[1].m_statistics.getStatistic(0, CycleStatisticType::Mean).getValue(statTime, statValue);
                fructose_assert_double_eq(statValue, 200001.0);
            }
        }

        // Delete all dynamically allocated objects
        delete component;
    }

    void testResidualErrorModelAdditive()
    {
        BuildConstantElimination builder;
        auto drugModel = builder.buildDrugModel(ResidualErrorType::ADDITIVE, std::vector<Value>({10000.0}));

        fructose_assert(drugModel != nullptr);

        fructose_assert(drugModel->checkInvariants());

        DrugModelChecker checker;

        std::shared_ptr<PkModel> sharedPkModel;
        sharedPkModel = std::make_shared<PkModel>("test.constantelimination", PkModel::AllowMultipleRoutes::No);

        bool addResult = sharedPkModel->addIntakeIntervalCalculatorFactory(
                AbsorptionModel::Extravascular, ConstantEliminationBolus::getCreator());
        fructose_assert(addResult);

        sharedPkModel->addParameterList(AbsorptionModel::Extravascular, ConstantEliminationBolus::getParametersId());

        std::shared_ptr<PkModelCollection> collection = std::make_shared<PkModelCollection>();
        collection->addPkModel(sharedPkModel);
        DrugModelChecker::CheckerResult_t checkerResult = checker.checkDrugModel(drugModel.get(), collection.get());

        fructose_assert(checkerResult.m_ok);

        if (!checkerResult.m_ok) {
            std::cout << checkerResult.m_errorMessage << std::endl;
        }

        // Now the drug model is ready to be used


        IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

        fructose_assert(component != nullptr);

        static_cast<ComputingComponent*>(component)->setPkModelCollection(collection);


        {
            const FormulationAndRoute route(
                    Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);

            auto drugTreatment = buildDrugTreatment(route);

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

            fructose_assert_eq(result, ComputingStatus::Ok);

            const ComputedData* responseData = response->getData();
            fructose_assert(dynamic_cast<const PercentilesData*>(responseData) != nullptr);
            const PercentilesData* resp = dynamic_cast<const PercentilesData*>(responseData);

            fructose_assert_eq(resp->getNbRanks(), percentileRanks.size());

            for (size_t p = 0; p < resp->getNbRanks(); p++) {
                DateTime statTime = DateTime::now();
                Value statValue = 0.0;
                resp->getData(p, 0)
                        .m_statistics.getStatistic(0, CycleStatisticType::Mean)
                        .getValue(statTime, statValue);
                //fructose_assert_eq(statValue, 200001.0);

                // Multiply the Inv CDF by SD (10), and by 1000.0 because of mg/l
                double expectedValue = invCdf[p] * 10.0 * 1000.0;
                fructose_assert_double_eq_rel_abs(
                        statValue - 200000.0, expectedValue, 0.02, 10.0 * 10.0 * 1000.0 * 0.06);
            }
        }

        // Delete all dynamically allocated objects
        delete component;
    }

    void testResidualErrorModelExponential()
    {
        BuildConstantElimination builder;
        auto drugModel = builder.buildDrugModel(ResidualErrorType::EXPONENTIAL, std::vector<Value>({0.2}));

        fructose_assert(drugModel != nullptr);

        fructose_assert(drugModel->checkInvariants());

        DrugModelChecker checker;

        std::shared_ptr<PkModel> sharedPkModel;
        sharedPkModel = std::make_shared<PkModel>("test.constantelimination", PkModel::AllowMultipleRoutes::No);

        bool addResult = sharedPkModel->addIntakeIntervalCalculatorFactory(
                AbsorptionModel::Extravascular, ConstantEliminationBolus::getCreator());
        fructose_assert(addResult);
        sharedPkModel->addParameterList(AbsorptionModel::Extravascular, ConstantEliminationBolus::getParametersId());

        std::shared_ptr<PkModelCollection> collection = std::make_shared<PkModelCollection>();
        collection->addPkModel(sharedPkModel);
        DrugModelChecker::CheckerResult_t checkerResult = checker.checkDrugModel(drugModel.get(), collection.get());

        fructose_assert(checkerResult.m_ok);

        if (!checkerResult.m_ok) {
            std::cout << checkerResult.m_errorMessage << std::endl;
        }

        // Now the drug model is ready to be used


        IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

        fructose_assert(component != nullptr);

        static_cast<ComputingComponent*>(component)->setPkModelCollection(collection);


        {
            const FormulationAndRoute route(
                    Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);

            auto drugTreatment = buildDrugTreatment(route);

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

            fructose_assert_eq(result, ComputingStatus::Ok);

            const ComputedData* responseData = response->getData();
            fructose_assert(dynamic_cast<const PercentilesData*>(responseData) != nullptr);
            const PercentilesData* resp = dynamic_cast<const PercentilesData*>(responseData);

            fructose_assert_eq(resp->getNbRanks(), percentileRanks.size());

            for (size_t p = 0; p < resp->getNbRanks(); p++) {
                DateTime statTime = DateTime::now();
                Value statValue = 0.0;
                resp->getData(p, 0)
                        .m_statistics.getStatistic(0, CycleStatisticType::Mean)
                        .getValue(statTime, statValue);
                //fructose_assert_eq(statValue, 200001.0);

                // Multiply the Inv CDF by SD (0.2)
                double expectedValue = 200000.0 * std::exp(invCdf[p] * 0.2);
                fructose_assert_double_eq_rel_abs(statValue, expectedValue, 0.05, 0.0);
            }
        }

        // Delete all dynamically allocated objects
        delete component;
    }


    void testResidualErrorModelProportional()
    {
        BuildConstantElimination builder;
        auto drugModel = builder.buildDrugModel(ResidualErrorType::PROPORTIONAL, std::vector<Value>({0.2}));

        fructose_assert(drugModel != nullptr);

        fructose_assert(drugModel->checkInvariants());

        DrugModelChecker checker;

        std::shared_ptr<PkModel> sharedPkModel;
        sharedPkModel = std::make_shared<PkModel>("test.constantelimination", PkModel::AllowMultipleRoutes::No);

        bool addResult = sharedPkModel->addIntakeIntervalCalculatorFactory(
                AbsorptionModel::Extravascular, ConstantEliminationBolus::getCreator());
        fructose_assert(addResult);
        sharedPkModel->addParameterList(AbsorptionModel::Extravascular, ConstantEliminationBolus::getParametersId());

        std::shared_ptr<PkModelCollection> collection = std::make_shared<PkModelCollection>();
        collection->addPkModel(sharedPkModel);
        DrugModelChecker::CheckerResult_t checkerResult = checker.checkDrugModel(drugModel.get(), collection.get());

        fructose_assert(checkerResult.m_ok);

        if (!checkerResult.m_ok) {
            std::cout << checkerResult.m_errorMessage << std::endl;
        }

        // Now the drug model is ready to be used


        IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

        fructose_assert(component != nullptr);

        static_cast<ComputingComponent*>(component)->setPkModelCollection(collection);

        {
            const FormulationAndRoute route(
                    Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);

            auto drugTreatment = buildDrugTreatment(route);

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

            fructose_assert_eq(result, ComputingStatus::Ok);

            const ComputedData* responseData = response->getData();
            fructose_assert(dynamic_cast<const PercentilesData*>(responseData) != nullptr);
            const PercentilesData* resp = dynamic_cast<const PercentilesData*>(responseData);

            fructose_assert_eq(resp->getNbRanks(), percentileRanks.size());

            for (size_t p = 0; p < resp->getNbRanks(); p++) {
                DateTime statTime = DateTime::now();
                Value statValue = 0.0;
                resp->getData(p, 0)
                        .m_statistics.getStatistic(0, CycleStatisticType::Mean)
                        .getValue(statTime, statValue);
                //fructose_assert_eq(statValue, 200001.0);

                // Multiply the Inv CDF by SD (0.2)
                double expectedValue = 200000.0 * (1 + invCdf[p] * 0.2);
                fructose_assert_double_eq_rel_abs(statValue, expectedValue, 0.05, 0.0);
            }
        }

        // Delete all dynamically allocated objects
        delete component;
    }

    void testResidualErrorModelMixed()
    {
        BuildConstantElimination builder;
        auto drugModel = builder.buildDrugModel(ResidualErrorType::MIXED, std::vector<Value>({10.0, 0.2}));

        fructose_assert(drugModel != nullptr);

        fructose_assert(drugModel->checkInvariants());

        DrugModelChecker checker;

        std::shared_ptr<PkModel> sharedPkModel;
        sharedPkModel = std::make_shared<PkModel>("test.constantelimination", PkModel::AllowMultipleRoutes::No);

        bool addResult = sharedPkModel->addIntakeIntervalCalculatorFactory(
                AbsorptionModel::Extravascular, ConstantEliminationBolus::getCreator());
        fructose_assert(addResult);
        sharedPkModel->addParameterList(AbsorptionModel::Extravascular, ConstantEliminationBolus::getParametersId());

        std::shared_ptr<PkModelCollection> collection = std::make_shared<PkModelCollection>();
        collection->addPkModel(sharedPkModel);
        DrugModelChecker::CheckerResult_t checkerResult = checker.checkDrugModel(drugModel.get(), collection.get());

        fructose_assert(checkerResult.m_ok);

        if (!checkerResult.m_ok) {
            std::cout << checkerResult.m_errorMessage << std::endl;
        }

        // Now the drug model is ready to be used


        IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

        fructose_assert(component != nullptr);

        static_cast<ComputingComponent*>(component)->setPkModelCollection(collection);


        {
            const FormulationAndRoute route(
                    Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);

            auto drugTreatment = buildDrugTreatment(route);

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

            fructose_assert_eq(result, ComputingStatus::Ok);

            const ComputedData* responseData = response->getData();
            fructose_assert(dynamic_cast<const PercentilesData*>(responseData) != nullptr);
            const PercentilesData* resp = dynamic_cast<const PercentilesData*>(responseData);

            fructose_assert_eq(resp->getNbRanks(), percentileRanks.size());

            for (size_t p = 0; p < resp->getNbRanks(); p++) {
                DateTime statTime = DateTime::now();
                Value statValue = 0.0;
                resp->getData(p, 0)
                        .m_statistics.getStatistic(0, CycleStatisticType::Mean)
                        .getValue(statTime, statValue);
                //fructose_assert_eq(statValue, 200001.0);

                // Multiply the Inv CDF by SD (0.2)
                double expectedValue = 200000.0 + invCdf[p] * std::sqrt(std::pow(200000.0 * 0.2, 2));
                fructose_assert_double_eq_rel_abs(statValue, expectedValue, 0.04, 0.0);
            }
        }

        // Delete all dynamically allocated objects
        delete component;
    }



    void testParamAdditive()
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

        fructose_assert(drugModel != nullptr);

        fructose_assert(drugModel->checkInvariants());

        DrugModelChecker checker;

        std::shared_ptr<PkModel> sharedPkModel;
        sharedPkModel = std::make_shared<PkModel>("test.constantelimination", PkModel::AllowMultipleRoutes::No);

        bool addResult = sharedPkModel->addIntakeIntervalCalculatorFactory(
                AbsorptionModel::Extravascular, ConstantEliminationBolus::getCreator());
        fructose_assert(addResult);
        sharedPkModel->addParameterList(AbsorptionModel::Extravascular, ConstantEliminationBolus::getParametersId());

        std::shared_ptr<PkModelCollection> collection = std::make_shared<PkModelCollection>();
        collection->addPkModel(sharedPkModel);
        DrugModelChecker::CheckerResult_t checkerResult = checker.checkDrugModel(drugModel.get(), collection.get());

        fructose_assert(checkerResult.m_ok);

        if (!checkerResult.m_ok) {
            std::cout << checkerResult.m_errorMessage << std::endl;
        }

        // Now the drug model is ready to be used


        IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

        fructose_assert(component != nullptr);

        static_cast<ComputingComponent*>(component)->setPkModelCollection(collection);


        {
            const FormulationAndRoute route(
                    Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);

            auto drugTreatment = buildDrugTreatment(route);

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

            fructose_assert_eq(result, ComputingStatus::Ok);

            const ComputedData* responseData = response->getData();
            fructose_assert(dynamic_cast<const PercentilesData*>(responseData) != nullptr);
            const PercentilesData* resp = dynamic_cast<const PercentilesData*>(responseData);

            fructose_assert_eq(resp->getNbRanks(), percentileRanks.size());

            for (size_t p = 0; p < resp->getNbRanks(); p++) {
                DateTime statTime = DateTime::now();
                Value statValue = 0.0;
                resp->getData(p, 0)
                        .m_statistics.getStatistic(0, CycleStatisticType::Mean)
                        .getValue(statTime, statValue);
                //fructose_assert_eq(statValue, 200001.0);

                // Multiply the Inv CDF by SD (10), and by 1000.0 because of mg/l
                double expectedValue = 200000.0 + invCdf[p] * 1000.0;
                fructose_assert_double_eq_rel_abs(statValue, expectedValue, .02, 0.02);
            }
        }

        // Delete all dynamically allocated objects
        delete component;
    }


    void testParamAdditiveResidualErrorModelAdditive()
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

        fructose_assert(drugModel != nullptr);

        fructose_assert(drugModel->checkInvariants());

        DrugModelChecker checker;

        std::shared_ptr<PkModel> sharedPkModel;
        sharedPkModel = std::make_shared<PkModel>("test.constantelimination", PkModel::AllowMultipleRoutes::No);

        bool addResult = sharedPkModel->addIntakeIntervalCalculatorFactory(
                AbsorptionModel::Extravascular, ConstantEliminationBolus::getCreator());
        fructose_assert(addResult);
        sharedPkModel->addParameterList(AbsorptionModel::Extravascular, ConstantEliminationBolus::getParametersId());

        std::shared_ptr<PkModelCollection> collection = std::make_shared<PkModelCollection>();
        collection->addPkModel(sharedPkModel);
        DrugModelChecker::CheckerResult_t checkerResult = checker.checkDrugModel(drugModel.get(), collection.get());

        fructose_assert(checkerResult.m_ok);

        if (!checkerResult.m_ok) {
            std::cout << checkerResult.m_errorMessage << std::endl;
        }

        // Now the drug model is ready to be used


        IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

        fructose_assert(component != nullptr);

        static_cast<ComputingComponent*>(component)->setPkModelCollection(collection);


        {
            const FormulationAndRoute route(
                    Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);

            auto drugTreatment = buildDrugTreatment(route);

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

            fructose_assert_eq(result, ComputingStatus::Ok);

            const ComputedData* responseData = response->getData();
            fructose_assert(dynamic_cast<const PercentilesData*>(responseData) != nullptr);
            const PercentilesData* resp = dynamic_cast<const PercentilesData*>(responseData);

            fructose_assert_eq(resp->getNbRanks(), percentileRanks.size());

            for (size_t p = 0; p < resp->getNbRanks(); p++) {
                DateTime statTime = DateTime::now();
                Value statValue = 0.0;
                resp->getData(p, 0)
                        .m_statistics.getStatistic(0, CycleStatisticType::Mean)
                        .getValue(statTime, statValue);
                //fructose_assert_eq(statValue, 200001.0);

                // Multiply the Inv CDF by SD (10), and by 1000.0 because of mg/l
                double expectedValue =
                        200000.0 + invCdf[p] * std::sqrt(std::pow(1000.0, 2) + std::pow(10.0 * 1000.0, 2));

                fructose_assert_double_eq_rel_abs(statValue, expectedValue, .01, 0.01);
            }
        }

        // Delete all dynamically allocated objects
        delete component;
    }

    void testParamExponentialResidualErrorModelExponential()
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

        fructose_assert(drugModel != nullptr);

        fructose_assert(drugModel->checkInvariants());

        DrugModelChecker checker;

        std::shared_ptr<PkModel> sharedPkModel;
        sharedPkModel = std::make_shared<PkModel>("test.constantelimination", PkModel::AllowMultipleRoutes::No);

        bool addResult = sharedPkModel->addIntakeIntervalCalculatorFactory(
                AbsorptionModel::Extravascular, ConstantEliminationBolus::getCreator());
        fructose_assert(addResult);
        sharedPkModel->addParameterList(AbsorptionModel::Extravascular, ConstantEliminationBolus::getParametersId());

        std::shared_ptr<PkModelCollection> collection = std::make_shared<PkModelCollection>();
        collection->addPkModel(sharedPkModel);
        DrugModelChecker::CheckerResult_t checkerResult = checker.checkDrugModel(drugModel.get(), collection.get());

        fructose_assert(checkerResult.m_ok);

        if (!checkerResult.m_ok) {
            std::cout << checkerResult.m_errorMessage << std::endl;
        }

        // Now the drug model is ready to be used


        IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

        fructose_assert(component != nullptr);

        static_cast<ComputingComponent*>(component)->setPkModelCollection(collection);


        {
            const FormulationAndRoute route(
                    Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);

            auto drugTreatment = buildDrugTreatment(route);

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

            fructose_assert_eq(result, ComputingStatus::Ok);

            const ComputedData* responseData = response->getData();
            fructose_assert(dynamic_cast<const PercentilesData*>(responseData) != nullptr);
            const PercentilesData* resp = dynamic_cast<const PercentilesData*>(responseData);

            fructose_assert_eq(resp->getNbRanks(), percentileRanks.size());

            for (size_t p = 0; p < resp->getNbRanks(); p++) {
                DateTime statTime = DateTime::now();
                Value statValue = 0.0;
                resp->getData(p, 0)
                        .m_statistics.getStatistic(0, CycleStatisticType::Mean)
                        .getValue(statTime, statValue);

                // Calculate the resulting standard deviation
                double newStd = std::sqrt(std::pow(0.2, 2) + std::pow(0.3, 2));

                // Multiply the Inv CDF by the new standard deviation
                double expectedValue = 200000.0 * std::exp(invCdf[p] * newStd);
                fructose_assert_double_eq_rel_abs(statValue, expectedValue, 0.05, 0.0);
            }
        }

        // Delete all dynamically allocated objects
        delete component;
    }

    void testParamProportionalResidualErrorModelProportional()
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

        fructose_assert(drugModel != nullptr);

        fructose_assert(drugModel->checkInvariants());

        DrugModelChecker checker;

        std::shared_ptr<PkModel> sharedPkModel;
        sharedPkModel = std::make_shared<PkModel>("test.constantelimination", PkModel::AllowMultipleRoutes::No);

        bool addResult = sharedPkModel->addIntakeIntervalCalculatorFactory(
                AbsorptionModel::Extravascular, ConstantEliminationBolus::getCreator());
        fructose_assert(addResult);
        sharedPkModel->addParameterList(AbsorptionModel::Extravascular, ConstantEliminationBolus::getParametersId());

        std::shared_ptr<PkModelCollection> collection = std::make_shared<PkModelCollection>();
        collection->addPkModel(sharedPkModel);
        DrugModelChecker::CheckerResult_t checkerResult = checker.checkDrugModel(drugModel.get(), collection.get());

        fructose_assert(checkerResult.m_ok);

        if (!checkerResult.m_ok) {
            std::cout << checkerResult.m_errorMessage << std::endl;
        }

        // Now the drug model is ready to be used


        IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

        fructose_assert(component != nullptr);

        static_cast<ComputingComponent*>(component)->setPkModelCollection(collection);


        {
            const FormulationAndRoute route(
                    Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);

            auto drugTreatment = buildDrugTreatment(route);

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

            fructose_assert_eq(result, ComputingStatus::Ok);

            const ComputedData* responseData = response->getData();
            fructose_assert(dynamic_cast<const PercentilesData*>(responseData) != nullptr);
            const PercentilesData* resp = dynamic_cast<const PercentilesData*>(responseData);

            fructose_assert_eq(resp->getNbRanks(), percentileRanks.size());

            for (size_t p = 0; p < resp->getNbRanks(); p++) {
                DateTime statTime = DateTime::now();
                Value statValue = 0.0;
                resp->getData(p, 0)
                        .m_statistics.getStatistic(0, CycleStatisticType::Mean)
                        .getValue(statTime, statValue);
                //fructose_assert_eq(statValue, 200001.0);


                // Calculate the resulting standard deviation
                //double newStd = std::sqrt(1.0 / ( 1.0 /std::pow(0.2, 2) + 1.0 / std::pow(0.3, 2)));
                // double newStd = std::sqrt(std::pow(0.2, 2) * std::pow(0.3, 2) / ( std::pow(0.2, 2) + std::pow(0.3, 2)));
                //double newStd = std::sqrt(std::pow(0.2, 2) * std::pow(0.3, 2) / ( std::pow(0.2, 2) + std::pow(0.3, 2)));
                //std::cout << newStd << std::endl;


                // Multiply the Inv CDF by the new standard deviation
                // double expectedValue = 200000.0 * ( 1.0 + invCdf[p] * newStd);

                // YTA : I do not know how to calculate the product of two Normal variables

                //fructose_assert_double_eq_rel_abs(statValue, expectedValue, 0.02, 0.0);
            }
        }

        // Delete all dynamically allocated objects
        delete component;
    }


    void testAdjustments()
    {
        BuildConstantElimination builder;
        auto drugModel = builder.buildDrugModel();

        fructose_assert(drugModel != nullptr);


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

        fructose_assert(drugModel->checkInvariants());

        DrugModelChecker checker;

        std::shared_ptr<PkModel> sharedPkModel;
        sharedPkModel = std::make_shared<PkModel>("test.constantelimination", PkModel::AllowMultipleRoutes::No);

        bool addResult = sharedPkModel->addIntakeIntervalCalculatorFactory(
                AbsorptionModel::Extravascular, ConstantEliminationBolus::getCreator());
        fructose_assert(addResult);
        sharedPkModel->addParameterList(AbsorptionModel::Extravascular, ConstantEliminationBolus::getParametersId());

        std::shared_ptr<PkModelCollection> collection = std::make_shared<PkModelCollection>();
        collection->addPkModel(sharedPkModel);
        DrugModelChecker::CheckerResult_t checkerResult = checker.checkDrugModel(drugModel.get(), collection.get());

        fructose_assert(checkerResult.m_ok);

        if (!checkerResult.m_ok) {
            std::cout << checkerResult.m_errorMessage << std::endl;
        }

        // Now the drug model is ready to be used


        IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

        fructose_assert(component != nullptr);

        static_cast<ComputingComponent*>(component)->setPkModelCollection(collection);


        {
            const FormulationAndRoute route(
                    Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);

            auto drugTreatment = buildDrugTreatment(route);

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

            fructose_assert_eq(result, ComputingStatus::Ok);

            const ComputedData* responseData = response->getData();
            fructose_assert(dynamic_cast<const AdjustmentData*>(responseData) != nullptr);
            const AdjustmentData* resp = dynamic_cast<const AdjustmentData*>(responseData);

            fructose_assert_eq(resp->getAdjustments().size(), size_t{9});

            for (auto& adjustment : resp->getAdjustments()) {

                fructose_assert_eq(adjustment.m_history.getDosageTimeRanges().size(), size_t{1});

                for (const auto& timeRange : adjustment.m_history.getDosageTimeRanges()) {
                    const DosageRepeat* dosageRepeat = dynamic_cast<const DosageRepeat*>(timeRange->getDosage());
                    fructose_assert(dosageRepeat != nullptr);
                    const SingleDose* dosage = dynamic_cast<const SingleDose*>(dosageRepeat->getDosage());
                    fructose_assert(dosage != nullptr);
                    fructose_assert_double_ge(dosage->getDose(), 750.0);
                    fructose_assert_double_le(dosage->getDose(), 1500.0);
                }
            }
        }

        // Delete all dynamically allocated objects
        delete component;
    }


    void testAdjustments2()
    {
        BuildConstantElimination builder;
        auto drugModel = builder.buildDrugModel();

        fructose_assert(drugModel != nullptr);


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

        fructose_assert(drugModel->checkInvariants());

        DrugModelChecker checker;

        std::shared_ptr<PkModel> sharedPkModel;
        sharedPkModel = std::make_shared<PkModel>("test.constantelimination", PkModel::AllowMultipleRoutes::No);

        bool addResult = sharedPkModel->addIntakeIntervalCalculatorFactory(
                AbsorptionModel::Extravascular, ConstantEliminationBolus::getCreator());
        fructose_assert(addResult);
        sharedPkModel->addParameterList(AbsorptionModel::Extravascular, ConstantEliminationBolus::getParametersId());

        std::shared_ptr<PkModelCollection> collection = std::make_shared<PkModelCollection>();
        collection->addPkModel(sharedPkModel);
        DrugModelChecker::CheckerResult_t checkerResult = checker.checkDrugModel(drugModel.get(), collection.get());

        fructose_assert(checkerResult.m_ok);

        if (!checkerResult.m_ok) {
            std::cout << checkerResult.m_errorMessage << std::endl;
        }

        // Now the drug model is ready to be used


        IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

        fructose_assert(component != nullptr);

        static_cast<ComputingComponent*>(component)->setPkModelCollection(collection);


        {
            const FormulationAndRoute route(
                    Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);

            auto drugTreatment = buildDrugTreatment(route);

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

            fructose_assert_eq(result, ComputingStatus::Ok);

            const ComputedData* responseData = response->getData();
            fructose_assert(dynamic_cast<const AdjustmentData*>(responseData) != nullptr);
            const AdjustmentData* resp = dynamic_cast<const AdjustmentData*>(responseData);

            // 18 possibilities:
            // 3 intervals for doses 300, 400, 500, 600, 700, 800
            fructose_assert_eq(resp->getAdjustments().size(), size_t{18});

            for (auto& adjustment : resp->getAdjustments()) {

                fructose_assert_eq(adjustment.m_history.getDosageTimeRanges().size(), size_t{1});

                for (const auto& timeRange : adjustment.m_history.getDosageTimeRanges()) {
                    const DosageRepeat* dosageRepeat = dynamic_cast<const DosageRepeat*>(timeRange->getDosage());
                    fructose_assert(dosageRepeat != nullptr);
                    const SingleDose* dosage = dynamic_cast<const SingleDose*>(dosageRepeat->getDosage());
                    fructose_assert(dosage != nullptr);
                    fructose_assert_double_ge((200.0 + dosage->getDose()) / 2.0, 250.0);
                    fructose_assert_double_le((200.0 + dosage->getDose()) / 2.0, 500.0);
                }
            }
        }

        // Delete all dynamically allocated objects
        delete component;
    }


    /// \brief Check that objects are correctly constructed by the constructor.
    void testConstantElimination(const std::string& /* _testName */)
    {
        test0();
        test1();
        testResidualErrorModelAdditive();
        testResidualErrorModelExponential();
        testResidualErrorModelProportional();
        testResidualErrorModelMixed();
        testParamAdditive();
        testParamAdditiveResidualErrorModelAdditive();
        testParamExponentialResidualErrorModelExponential();
        testAdjustments();
        testAdjustments2();
        // testParamProportionalResidualErrorModelProportional();
    }
};



#endif // TEST_CONSTANTELIMINATIONBOLUS_H
