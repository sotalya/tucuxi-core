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

#include "tucucore/computingservice/computingrequest.h"
#include "tucucore/drugmodelchecker.h"
#include "tucucore/drugmodelimport.h"
#include "tucucore/drugtreatment/patientcovariate.h"
#include "tucucore/multicomputingcomponent.h"
#include "tucucore/pkmodel.h"

#include "../gtest_core.h"
#include "../pkmodels/multiconstanteliminationbolus.h"
#include "../testutils.h"
#include "buildmultianalytesmultiactivemoieties.h"

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

TEST(Core_TestMultiAnalytesMultiActiveMoieties, DISABLED_MultiAnalytesMultiActiveMoieties)
{
#if GTEST_VERBOSE
    std::cout << __FUNCTION__ << std::endl;
#endif


    BuildMultiAnalytesMultiActiveMoieties builder;
    auto drugModel = builder.buildDrugModel();

    ASSERT_TRUE(drugModel != nullptr);

    ASSERT_TRUE(drugModel->checkInvariants());

    DrugModelChecker checker;

    std::shared_ptr<PkModel> sharedPkModel;
    sharedPkModel = std::make_shared<PkModel>("test.multiconstantelimination", PkModel::AllowMultipleRoutes::No);

    bool addResult = sharedPkModel->addIntakeIntervalCalculatorFactory(
            AbsorptionModel::Extravascular, MultiConstantEliminationBolus::getCreator());
    ASSERT_TRUE(addResult);
    sharedPkModel->addParameterList(AbsorptionModel::Extravascular, MultiConstantEliminationBolus::getParametersId());

    std::shared_ptr<PkModelCollection> collection = std::make_shared<PkModelCollection>();
    defaultPopulate(*collection.get());
    //        collection->addPkModel(sharedPkModel);
    DrugModelChecker::CheckerResult_t checkerResult = checker.checkDrugModel(drugModel.get(), collection.get());

    ASSERT_TRUE(checkerResult.m_ok);

    if (!checkerResult.m_ok) {
        std::cout << checkerResult.m_errorMessage << std::endl;
    }

    // Now the drug model is ready to be used


    IComputingService* component = dynamic_cast<IComputingService*>(MultiComputingComponent::createComponent());

    ASSERT_TRUE(component != nullptr);

    static_cast<MultiComputingComponent*>(component)->setPkModelCollection(collection);


    {
        const FormulationAndRoute route(Formulation::OralSolution, AdministrationRoute::Oral);

        DateTime startSept2018(
                date::year_month_day(date::year(2018), date::month(9), date::day(1)),
                Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));

        auto drugTreatment = buildDrugTreatment(route, startSept2018);

        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covS0", "0.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covA0", "0.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covR0", "0.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covM0", "1.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));

        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covS1", "0.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covA1", "0.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covR1", "0.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covM1", "1.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));


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

        ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(traits));

        std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

        ComputingStatus result;
        result = component->compute(request, response);

        ASSERT_EQ(result, ComputingStatus::Ok);
        if (result != ComputingStatus::Ok) {
            return;
        }

        const ComputedData* responseData = response->getData();

        {
            ASSERT_TRUE(responseData != nullptr);
            const SinglePredictionData* resp = dynamic_cast<const SinglePredictionData*>(responseData);
            ASSERT_TRUE(resp != nullptr);
            if (resp == nullptr) {
                return;
            }

            ASSERT_EQ(resp->getCompartmentInfos().size(), static_cast<size_t>(5));
            ASSERT_EQ(resp->getCompartmentInfos()[0].getId(), "activeMoietyMulti");
            ASSERT_EQ(resp->getCompartmentInfos()[0].getType(), CompartmentInfo::CompartmentType::ActiveMoiety);
            ASSERT_EQ(resp->getCompartmentInfos()[1].getId(), "analyte0");
            ASSERT_EQ(resp->getCompartmentInfos()[1].getType(), CompartmentInfo::CompartmentType::Analyte);
            ASSERT_EQ(resp->getCompartmentInfos()[2].getId(), "analyte1");
            ASSERT_EQ(resp->getCompartmentInfos()[2].getType(), CompartmentInfo::CompartmentType::Analyte);
            ASSERT_EQ(resp->getCompartmentInfos()[3].getId(), "analyte2");
            ASSERT_EQ(resp->getCompartmentInfos()[3].getType(), CompartmentInfo::CompartmentType::Analyte);
            ASSERT_EQ(resp->getCompartmentInfos()[4].getId(), "analyte3");
            ASSERT_EQ(resp->getCompartmentInfos()[4].getType(), CompartmentInfo::CompartmentType::Analyte);


            std::vector<CycleData> data = resp->getData();
            ASSERT_EQ(data.size(), static_cast<size_t>(16));
            ASSERT_EQ(data[0].m_concentrations.size(), static_cast<size_t>(5));
            ASSERT_DOUBLE_EQ(data[0].m_concentrations[0][0], 800000.0);
            ASSERT_DOUBLE_EQ(data[0].m_concentrations[1][0], 200000.0);
            ASSERT_DOUBLE_EQ(data[0].m_concentrations[2][0], 200000.0);
            ASSERT_DOUBLE_EQ(data[0].m_concentrations[3][0], 200000.0);
            ASSERT_DOUBLE_EQ(data[0].m_concentrations[4][0], 200000.0);
            ASSERT_EQ(data[0].m_concentrations[0].size(), static_cast<size_t>(61));
            ASSERT_EQ(data[0].m_concentrations[1].size(), static_cast<size_t>(61));
            ASSERT_EQ(data[0].m_concentrations[2].size(), static_cast<size_t>(61));
            ASSERT_EQ(data[0].m_concentrations[3].size(), static_cast<size_t>(61));
            ASSERT_EQ(data[0].m_concentrations[4].size(), static_cast<size_t>(61));

            DateTime startSept2018(
                    date::year_month_day(date::year(2018), date::month(9), date::day(1)),
                    Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));

            ASSERT_DOUBLE_EQ(data[0].m_start.toSeconds() + data[0].m_times[0][0] * 3600.0, startSept2018.toSeconds());
            ASSERT_DOUBLE_EQ(
                    data[1].m_start.toSeconds() + data[1].m_times[0][0] * 3600.0,
                    startSept2018.toSeconds() + 3600.0 * 6.0);

            DateTime statTime = DateTime::now();
            Value statValue;
            data[2].m_statistics.getStatistic(2, CycleStatisticType::AUC).getValue(statTime, statValue);
            ASSERT_PRED4(double_eq_rel_abs, statValue, 200000.0 * 6.0, 0.01, 0.01);

            data[2].m_statistics.getStatistic(2, CycleStatisticType::CumulativeAuc).getValue(statTime, statValue);
            ASSERT_PRED4(double_eq_rel_abs, statValue, 200000.0 * 6.0 * 3.0, 0.01, 0.01);

            data[2].m_statistics.getStatistic(2, CycleStatisticType::Peak).getValue(statTime, statValue);
            ASSERT_PRED4(double_eq_rel_abs, statValue, 200000.0, 0.01, 0.01);

            data[2].m_statistics.getStatistic(2, CycleStatisticType::Mean).getValue(statTime, statValue);
            ASSERT_PRED4(double_eq_rel_abs, statValue, 200000.0, 0.01, 0.01);

            data[1].m_statistics.getStatistic(2, CycleStatisticType::AUC).getValue(statTime, statValue);
            ASSERT_PRED4(double_eq_rel_abs, statValue, 200000.0 * 6.0, 0.01, 0.01);

            data[1].m_statistics.getStatistic(2, CycleStatisticType::CumulativeAuc).getValue(statTime, statValue);
            ASSERT_PRED4(double_eq_rel_abs, statValue, 200000.0 * 12.0, 0.01, 0.01);

            data[1].m_statistics.getStatistic(2, CycleStatisticType::Peak).getValue(statTime, statValue);
            ASSERT_PRED4(double_eq_rel_abs, statValue, 200000.0, 0.01, 0.01);

            data[1].m_statistics.getStatistic(2, CycleStatisticType::Mean).getValue(statTime, statValue);
            ASSERT_PRED4(double_eq_rel_abs, statValue, 200000.0, 0.01, 0.01);
        }
    }




    {
        //second test, using samples and intakes
#if GTEST_VERBOSE
        std::cout << __FUNCTION__ << std::endl;
#endif

        std::vector<SampleSeries> samples;
        IntakeSeries intakes;

        Tucuxi::Core::SampleSeries sampleSeries0;
        DateTime date0 = DateTime(
                date::year_month_day(date::year(2017), date::month(6), date::day(6)),
                Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0)));
        Tucuxi::Core::SampleEvent s0(date0, 200.0);
        sampleSeries0.push_back(s0);
        samples.push_back(sampleSeries0);

        Tucuxi::Core::SampleSeries sampleSeries1;
        DateTime date1 = DateTime(
                date::year_month_day(date::year(2017), date::month(6), date::day(6)),
                Duration(std::chrono::hours(16), std::chrono::minutes(30), std::chrono::seconds(0)));
        Tucuxi::Core::SampleEvent s1(date1, 200.0);
        sampleSeries1.push_back(s1);
        samples.push_back(sampleSeries1);

        //definition of the intakes


        intakes.emplace_back(
                DateTime(
                        date::year_month_day(date::year(2017), date::month(6), date::day(6)),
                        Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
                Duration(),
                DoseValue(200.0),
                TucuUnit("mg"),
                Duration(std::chrono::hours(24)),
                getBolusFormulationAndRoute(),
                Duration(std::chrono::minutes(20)),
                static_cast<int>(251));
        intakes.emplace_back(
                DateTime(
                        date::year_month_day(date::year(2017), date::month(6), date::day(7)),
                        Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
                Duration(),
                DoseValue(200.0),
                TucuUnit("mg"),
                Duration(std::chrono::hours(24)),
                getBolusFormulationAndRoute(),
                Duration(std::chrono::minutes(20)),
                static_cast<int>(251));

        // Associate the intake calculator to the intakes
        std::shared_ptr<MultiConstantEliminationBolus> intakeCalculator =
                std::make_shared<MultiConstantEliminationBolus>();
        intakes[0].setCalculator(intakeCalculator);
        intakes[1].setCalculator(intakeCalculator);


        Tucuxi::Core::ParameterDefinitions parameterDefs;
        parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "TestA0", 0.0, Tucuxi::Core::ParameterVariabilityType::Additive));
        parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "TestM0", 1.0, Tucuxi::Core::ParameterVariabilityType::None));
        parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "TestR0", 0.0, Tucuxi::Core::ParameterVariabilityType::None));
        parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "TestS0", 0.0, Tucuxi::Core::ParameterVariabilityType::None));

        parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "TestA1", 0.0, Tucuxi::Core::ParameterVariabilityType::Additive));
        parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "TestM1", 1.0, Tucuxi::Core::ParameterVariabilityType::None));
        parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "TestR1", 0.0, Tucuxi::Core::ParameterVariabilityType::None));
        parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "TestS1", 0.0, Tucuxi::Core::ParameterVariabilityType::None));


        Tucuxi::Core::ParameterSetEvent parameterset(DateTime::now(), parameterDefs);

        ParameterSetSeries parameters;
        parameters.addParameterSetEvent(parameterset);
    }



    // Delete all dynamically allocated objects
    delete component;
}

TEST(Core_TestMultiAnalytesMultiActiveMoieties, DISABLED_MultiAnalytesMultiActiveMoietiesConversion)
{
    BuildMultiAnalytesMultiActiveMoieties builder;
    auto drugModel = builder.buildDrugModel(0.3, 0.5);

    ASSERT_TRUE(drugModel != nullptr);

    ASSERT_TRUE(drugModel->checkInvariants());

    DrugModelChecker checker;

    std::shared_ptr<PkModel> sharedPkModel;
    sharedPkModel = std::make_shared<PkModel>("test.constantelimination", PkModel::AllowMultipleRoutes::No);

    bool addResult = sharedPkModel->addIntakeIntervalCalculatorFactory(
            AbsorptionModel::Extravascular, MultiConstantEliminationBolus::getCreator());
    ASSERT_TRUE(addResult);
    sharedPkModel->addParameterList(AbsorptionModel::Extravascular, MultiConstantEliminationBolus::getParametersId());

    std::shared_ptr<PkModelCollection> collection = std::make_shared<PkModelCollection>();
    collection->addPkModel(sharedPkModel);
    DrugModelChecker::CheckerResult_t checkerResult = checker.checkDrugModel(drugModel.get(), collection.get());

    ASSERT_TRUE(checkerResult.m_ok);

    if (!checkerResult.m_ok) {
        std::cout << checkerResult.m_errorMessage << std::endl;
        return;
    }

    // Now the drug model is ready to be used


    IComputingService* component = dynamic_cast<IComputingService*>(MultiComputingComponent::createComponent());

    ASSERT_TRUE(component != nullptr);

    static_cast<MultiComputingComponent*>(component)->setPkModelCollection(collection);


    {
        const FormulationAndRoute route(Formulation::OralSolution, AdministrationRoute::Oral);

        DateTime startSept2018(
                date::year_month_day(date::year(2018), date::month(9), date::day(1)),
                Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));

        auto drugTreatment = buildDrugTreatment(route, startSept2018);

        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covS0", "0.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covA0", "0.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covR0", "0.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covM0", "1.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));

        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covS1", "0.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covA1", "0.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covR1", "0.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covM1", "1.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));

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

        ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(traits));

        std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

        ComputingStatus result;
        result = component->compute(request, response);

        ASSERT_EQ(result, ComputingStatus::Ok);

        const ComputedData* responseData = response->getData();

        ASSERT_TRUE(responseData != nullptr);
        if (responseData == nullptr) {
            return;
        }

        {
            ASSERT_TRUE(dynamic_cast<const SinglePredictionData*>(responseData) != nullptr);
            const SinglePredictionData* resp = dynamic_cast<const SinglePredictionData*>(responseData);

            ASSERT_EQ(resp->getCompartmentInfos().size(), static_cast<size_t>(3));
            ASSERT_EQ(resp->getCompartmentInfos()[0].getId(), "activeMoietyMulti");
            ASSERT_EQ(resp->getCompartmentInfos()[0].getType(), CompartmentInfo::CompartmentType::ActiveMoiety);
            ASSERT_EQ(resp->getCompartmentInfos()[1].getId(), "analyte0");
            ASSERT_EQ(resp->getCompartmentInfos()[1].getType(), CompartmentInfo::CompartmentType::Analyte);
            ASSERT_EQ(resp->getCompartmentInfos()[2].getId(), "analyte1");
            ASSERT_EQ(resp->getCompartmentInfos()[2].getType(), CompartmentInfo::CompartmentType::Analyte);

            std::vector<CycleData> data = resp->getData();
            ASSERT_EQ(data.size(), static_cast<size_t>(16));
            ASSERT_EQ(data[0].m_concentrations.size(), static_cast<size_t>(3));
            ASSERT_DOUBLE_EQ(data[0].m_concentrations[0][0], 200000.0 * 0.8);
            ASSERT_DOUBLE_EQ(data[0].m_concentrations[1][0], 200000.0 * 0.3);
            ASSERT_DOUBLE_EQ(data[0].m_concentrations[2][0], 200000.0 * 0.5);
            ASSERT_EQ(data[0].m_concentrations[0].size(), static_cast<size_t>(61));
            ASSERT_EQ(data[0].m_concentrations[1].size(), static_cast<size_t>(61));
            ASSERT_EQ(data[0].m_concentrations[2].size(), static_cast<size_t>(61));

            DateTime startSept2018(
                    date::year_month_day(date::year(2018), date::month(9), date::day(1)),
                    Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));

            ASSERT_DOUBLE_EQ(data[0].m_start.toSeconds() + data[0].m_times[0][0] * 3600.0, startSept2018.toSeconds());
            ASSERT_DOUBLE_EQ(
                    data[1].m_start.toSeconds() + data[1].m_times[0][0] * 3600.0,
                    startSept2018.toSeconds() + 3600.0 * 6.0);

            DateTime statTime = DateTime::now();
            Value statValue;
            data[0].m_statistics.getStatistic(0, CycleStatisticType::AUC).getValue(statTime, statValue);
            ASSERT_PRED4(double_eq_rel_abs, statValue, 200000.0 * 6.0 * 0.8, 0.01, 0.01);

            data[0].m_statistics.getStatistic(0, CycleStatisticType::CumulativeAuc).getValue(statTime, statValue);
            ASSERT_PRED4(double_eq_rel_abs, statValue, 200000.0 * 6.0 * 0.8, 0.01, 0.01);

            data[0].m_statistics.getStatistic(0, CycleStatisticType::Peak).getValue(statTime, statValue);
            ASSERT_PRED4(double_eq_rel_abs, statValue, 200000.0 * 0.8, 0.01, 0.01);

            data[0].m_statistics.getStatistic(0, CycleStatisticType::Mean).getValue(statTime, statValue);
            ASSERT_PRED4(double_eq_rel_abs, statValue, 200000.0 * 0.8, 0.01, 0.01);


            data[1].m_statistics.getStatistic(0, CycleStatisticType::AUC).getValue(statTime, statValue);
            ASSERT_PRED4(double_eq_rel_abs, statValue, 200000.0 * 6.0 * 0.8, 0.01, 0.01);

            data[1].m_statistics.getStatistic(0, CycleStatisticType::CumulativeAuc).getValue(statTime, statValue);
            ASSERT_PRED4(double_eq_rel_abs, statValue, 200000.0 * 6.0 * 0.8 * 2.0, 0.01, 0.01);

            data[1].m_statistics.getStatistic(0, CycleStatisticType::Peak).getValue(statTime, statValue);
            ASSERT_PRED4(double_eq_rel_abs, statValue, 200000.0 * 0.8, 0.01, 0.01);

            data[1].m_statistics.getStatistic(0, CycleStatisticType::Mean).getValue(statTime, statValue);
            ASSERT_PRED4(double_eq_rel_abs, statValue, 200000.0 * 0.8, 0.01, 0.01);


            data[1].m_statistics.getStatistic(1, CycleStatisticType::AUC).getValue(statTime, statValue);
            ASSERT_PRED4(double_eq_rel_abs, statValue, 200000.0 * 6.0 * 0.3, 0.01, 0.01);

            data[1].m_statistics.getStatistic(1, CycleStatisticType::CumulativeAuc).getValue(statTime, statValue);
            ASSERT_PRED4(double_eq_rel_abs, statValue, 200000.0 * 12.0 * 0.3, 0.01, 0.01);

            data[1].m_statistics.getStatistic(1, CycleStatisticType::Peak).getValue(statTime, statValue);
            ASSERT_PRED4(double_eq_rel_abs, statValue, 200000.0 * 0.3, 0.01, 0.01);

            data[1].m_statistics.getStatistic(1, CycleStatisticType::Mean).getValue(statTime, statValue);
            ASSERT_PRED4(double_eq_rel_abs, statValue, 200000.0 * 0.3, 0.01, 0.01);

            data[0].m_statistics.getStatistic(2, CycleStatisticType::AUC).getValue(statTime, statValue);
            ASSERT_PRED4(double_eq_rel_abs, statValue, 200000.0 * 6.0 * 0.5, 0.01, 0.01);

            data[0].m_statistics.getStatistic(2, CycleStatisticType::CumulativeAuc).getValue(statTime, statValue);
            ASSERT_PRED4(double_eq_rel_abs, statValue, 200000.0 * 6.0 * 0.5, 0.01, 0.01);

            data[0].m_statistics.getStatistic(2, CycleStatisticType::Peak).getValue(statTime, statValue);
            ASSERT_PRED4(double_eq_rel_abs, statValue, 200000.0 * 0.5, 0.01, 0.01);

            data[0].m_statistics.getStatistic(2, CycleStatisticType::Mean).getValue(statTime, statValue);
            ASSERT_PRED4(double_eq_rel_abs, statValue, 200000.0 * 0.5, 0.01, 0.01);
        }
    }

    // Delete all dynamically allocated objects
    delete component;
}

TEST(Core_TestMultiAnalytesMultiActiveMoieties, DISABLED_Adjustments)
{
    BuildMultiAnalytesMultiActiveMoieties builder;
    auto drugModel = builder.buildDrugModel(0.3, 0.7);

    ASSERT_TRUE(drugModel != nullptr);


    // Add targets
    TargetDefinition* target = new TargetDefinition(
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

    drugModel->m_activeMoieties[0]->addTarget(std::unique_ptr<TargetDefinition>(target));

    ASSERT_TRUE(drugModel->checkInvariants());

    DrugModelChecker checker;

    std::shared_ptr<PkModel> sharedPkModel;
    sharedPkModel = std::make_shared<PkModel>("test.multipleconstantelimination", PkModel::AllowMultipleRoutes::No);

    bool addResult = sharedPkModel->addIntakeIntervalCalculatorFactory(
            AbsorptionModel::Extravascular, MultiConstantEliminationBolus::getCreator());
    ASSERT_TRUE(addResult);
    sharedPkModel->addParameterList(AbsorptionModel::Extravascular, MultiConstantEliminationBolus::getParametersId());

    std::shared_ptr<PkModelCollection> collection = std::make_shared<PkModelCollection>();
    collection->addPkModel(sharedPkModel);
    DrugModelChecker::CheckerResult_t checkerResult = checker.checkDrugModel(drugModel.get(), collection.get());

    ASSERT_TRUE(checkerResult.m_ok);

    if (!checkerResult.m_ok) {
        std::cout << checkerResult.m_errorMessage << std::endl;
        return;
    }

    // Now the drug model is ready to be used


    IComputingService* component = dynamic_cast<IComputingService*>(MultiComputingComponent::createComponent());

    ASSERT_TRUE(component != nullptr);

    static_cast<MultiComputingComponent*>(component)->setPkModelCollection(collection);


    {
        const FormulationAndRoute route(Formulation::OralSolution, AdministrationRoute::Oral);

        DateTime startSept2018(
                date::year_month_day(date::year(2018), date::month(9), date::day(1)),
                Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));

        auto drugTreatment = buildDrugTreatment(route, startSept2018);

        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covS0", "0.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covA0", "0.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covR0", "0.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covM0", "1.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));

        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covS1", "0.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covA1", "0.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covR1", "0.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covM1", "1.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));

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

        ASSERT_TRUE(responseData != nullptr);

        if (responseData == nullptr) {
            return;
        }

        {
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
    }

    // Delete all dynamically allocated objects
    delete component;
}
