/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TEST_CONSTANTELIMINATIONBOLUS_H
#define TEST_CONSTANTELIMINATIONBOLUS_H

#include <chrono>
#include <iostream>
#include <memory>

#include <date/date.h>
#include <math.h>

#include "fructose/fructose.h"

#include "tucucore/drugmodelimport.h"
#include "tucucore/pkmodel.h"
#include "tucucore/drugmodelchecker.h"
#include "tucucore/drugtreatment/drugtreatment.h"
#include "tucucore/computingcomponent.h"
#include "tucucore/computingservice/computingrequest.h"
#include "tucucore/drugtreatment/patientcovariate.h"

#include "buildconstantelimination.h"
#include "../pkmodels/constanteliminationbolus.h"
#include "../testutils.h"

using namespace std::chrono_literals;
using namespace date;

using namespace Tucuxi::Core;


struct TestConstantEliminationBolus : public fructose::test_base<TestConstantEliminationBolus>
{
    TestConstantEliminationBolus() { }

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

    void buildDrugTreatment(DrugTreatment *&_drugTreatment, FormulationAndRoute _route)
    {

        _drugTreatment = new DrugTreatment();

        // List of time ranges that will be pushed into the history
        DosageTimeRangeList timeRangeList;

        // Create a time range starting at the beginning of june 2017, with no upper end (to test that the repetitions
        // are handled correctly)
        DateTime startSept2018(date::year_month_day(date::year(2018), date::month(9), date::day(1)),
                               Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));


        //const FormulationAndRoute route("formulation", AdministrationRoute::IntravenousBolus, AbsorptionModel::INTRAVASCULAR);
        // Add a treatment intake every ten days in June
        // 200mg via a intravascular at 08h30, starting the 01.06
        LastingDose periodicDose(DoseValue(200.0),
                                 _route,
                                 Duration(),
                                 Duration(std::chrono::hours(6)));
        DosageRepeat repeatedDose(periodicDose, 16);
        std::unique_ptr<Tucuxi::Core::DosageTimeRange> sept2018(new Tucuxi::Core::DosageTimeRange(startSept2018, repeatedDose));


        _drugTreatment->getModifiableDosageHistory().addTimeRange(*sept2018);
    }


    void test0() {
        BuildConstantElimination builder;
        DrugModel *drugModel = builder.buildDrugModel();

        fructose_assert(drugModel != nullptr);

        fructose_assert(drugModel->checkInvariants());

        DrugModelChecker checker;

        std::shared_ptr<PkModel> sharedPkModel;
        sharedPkModel = std::make_shared<PkModel>("test.constantelimination");

        bool addResult = sharedPkModel->addIntakeIntervalCalculatorFactory(AbsorptionModel::INTRAVASCULAR, ConstantEliminationBolus::getCreator());
        fructose_assert(addResult);

        PkModelCollection *collection = new PkModelCollection();
        collection->addPkModel(sharedPkModel);
        DrugModelChecker::CheckerResult_t checkerResult = checker.checkDrugModel(drugModel, collection);

        fructose_assert(checkerResult.ok);

        if (!checkerResult.ok) {
            std::cout << checkerResult.errorMessage << std::endl;
        }

        // Now the drug model is ready to be used


        IComputingService *component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

        fructose_assert( component != nullptr);

        static_cast<ComputingComponent *>(component)->setPkModelCollection(collection);


        {

            DrugTreatment *drugTreatment;
            const FormulationAndRoute route(Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::INTRAVASCULAR);

            buildDrugTreatment(drugTreatment, route);

            drugTreatment->addCovariate(std::make_unique<PatientCovariate>("covS", "0.0", DataType::Double, Unit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
            drugTreatment->addCovariate(std::make_unique<PatientCovariate>("covA", "0.0", DataType::Double, Unit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
            drugTreatment->addCovariate(std::make_unique<PatientCovariate>("covR", "0.0", DataType::Double, Unit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));


            std::unique_ptr<ComputingTraits> computingTraits = std::make_unique<ComputingTraits>();

            RequestResponseId requestResponseId = "1";
            Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
            Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
            double nbPointsPerHour = 10.0;
            ComputingOption computingOption(PredictionParameterType::Apriori, CompartmentsOption::MainCompartment, true);
            std::unique_ptr<ComputingTraitConcentration> traits =
                    std::make_unique<ComputingTraitConcentration>(
                        requestResponseId, start, end, nbPointsPerHour, computingOption);
            computingTraits->addTrait(std::move(traits));


            std::unique_ptr<ComputingTraitPercentiles> traitsPercentiles =
                    std::make_unique<ComputingTraitPercentiles>(
                        requestResponseId, start, end, percentileRanks, nbPointsPerHour, computingOption);

            computingTraits->addTrait(std::move(traitsPercentiles));


            ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(computingTraits));

            std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

            ComputingResult result;
            result = component->compute(request, response);

            fructose_assert( result == ComputingResult::Success);

            const std::vector<std::unique_ptr<SingleComputingResponse> > &responses = response.get()->getResponses();

            fructose_assert_eq(responses.size(), size_t{2});

            {
                fructose_assert(dynamic_cast<PercentilesResponse*>(responses[1].get()) != nullptr);
                const PercentilesResponse *resp = dynamic_cast<PercentilesResponse*>(responses[1].get());

                fructose_assert_eq(resp->getNbRanks(), percentileRanks.size());

                for(size_t p = 0; p < resp->getNbRanks(); p++) {
                    DateTime statTime;
                    Value statValue;
                    resp->getData(p,0).m_statistics.getStatistic(0, CycleStatisticType::Mean).getValue(statTime, statValue);
                    fructose_assert_double_eq(statValue, 200000.0);
                }

            }
            {
                fructose_assert(dynamic_cast<SinglePredictionResponse*>(responses[0].get()) != nullptr);
                const SinglePredictionResponse *resp = dynamic_cast<SinglePredictionResponse*>(responses[0].get());
                std::vector<CycleData> data = resp->getData();
                fructose_assert(data.size() == 16);
                fructose_assert(data[0].m_concentrations.size() == 1);
                fructose_assert_eq(data[0].m_concentrations[0][0] , 200000.0);
                DateTime startSept2018(date::year_month_day(date::year(2018), date::month(9), date::day(1)),
                                       Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));

                fructose_assert_eq(data[0].m_start.toSeconds() + data[0].m_times[0][0] * 3600.0 , startSept2018.toSeconds());
                fructose_assert_eq(data[1].m_start.toSeconds() + data[1].m_times[0][0] * 3600.0 , startSept2018.toSeconds() + 3600.0 * 6.0);

                DateTime statTime;
                Value statValue;
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

            delete drugTreatment;
        }

        // Delete all dynamically allocated objects
        delete component;
        delete drugModel;

    }


    void test1() {
        BuildConstantElimination builder;
        DrugModel *drugModel = builder.buildDrugModel();

        fructose_assert(drugModel != nullptr);

        fructose_assert(drugModel->checkInvariants());

        DrugModelChecker checker;

        std::shared_ptr<PkModel> sharedPkModel;
        sharedPkModel = std::make_shared<PkModel>("test.constantelimination");

        bool addResult = sharedPkModel->addIntakeIntervalCalculatorFactory(AbsorptionModel::INTRAVASCULAR, ConstantEliminationBolus::getCreator());
        fructose_assert(addResult);

        PkModelCollection *collection = new PkModelCollection();
        collection->addPkModel(sharedPkModel);
        DrugModelChecker::CheckerResult_t checkerResult = checker.checkDrugModel(drugModel, collection);

        fructose_assert(checkerResult.ok);

        if (!checkerResult.ok) {
            std::cout << checkerResult.errorMessage << std::endl;
        }

        // Now the drug model is ready to be used


        IComputingService *component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

        fructose_assert( component != nullptr);

        static_cast<ComputingComponent *>(component)->setPkModelCollection(collection);


        {

            DrugTreatment *drugTreatment;
            const FormulationAndRoute route(Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::INTRAVASCULAR);

            buildDrugTreatment(drugTreatment, route);

            drugTreatment->addCovariate(std::make_unique<PatientCovariate>("covS", "0.0", DataType::Double, Unit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
            drugTreatment->addCovariate(std::make_unique<PatientCovariate>("covA", "1.0", DataType::Double, Unit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
            drugTreatment->addCovariate(std::make_unique<PatientCovariate>("covR", "0.0", DataType::Double, Unit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));


            std::unique_ptr<ComputingTraits> computingTraits = std::make_unique<ComputingTraits>();

            RequestResponseId requestResponseId = "1";
            Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
            Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
            double nbPointsPerHour = 10.0;
            ComputingOption computingOption(PredictionParameterType::Apriori, CompartmentsOption::MainCompartment, true);
            std::unique_ptr<ComputingTraitConcentration> traits =
                    std::make_unique<ComputingTraitConcentration>(
                        requestResponseId, start, end, nbPointsPerHour, computingOption);
            computingTraits->addTrait(std::move(traits));


            std::unique_ptr<ComputingTraitPercentiles> traitsPercentiles =
                    std::make_unique<ComputingTraitPercentiles>(
                        requestResponseId, start, end, percentileRanks, nbPointsPerHour, computingOption);

            computingTraits->addTrait(std::move(traitsPercentiles));


            ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(computingTraits));

            std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

            ComputingResult result;
            result = component->compute(request, response);

            fructose_assert( result == ComputingResult::Success);

            const std::vector<std::unique_ptr<SingleComputingResponse> > &responses = response.get()->getResponses();

            fructose_assert_eq(responses.size(), size_t{2});

            {
                fructose_assert(dynamic_cast<PercentilesResponse*>(responses[1].get()) != nullptr);
                const PercentilesResponse *resp = dynamic_cast<PercentilesResponse*>(responses[1].get());

                fructose_assert_eq(resp->getNbRanks(), percentileRanks.size());

                for(size_t p = 0; p < resp->getNbRanks(); p++) {
                    DateTime statTime;
                    Value statValue;
                    resp->getData(p,0).m_statistics.getStatistic(0, CycleStatisticType::Mean).getValue(statTime, statValue);
                    fructose_assert_double_eq(statValue, 200001.0);
                }

            }
            {
                fructose_assert(dynamic_cast<SinglePredictionResponse*>(responses[0].get()) != nullptr);
                const SinglePredictionResponse *resp = dynamic_cast<SinglePredictionResponse*>(responses[0].get());
                std::vector<CycleData> data = resp->getData();
                fructose_assert(data.size() == 16);
                fructose_assert(data[0].m_concentrations.size() == 1);
                fructose_assert_eq(data[0].m_concentrations[0][0] , 200001.0);
                DateTime startSept2018(date::year_month_day(date::year(2018), date::month(9), date::day(1)),
                                       Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));

                fructose_assert_eq(data[0].m_start.toSeconds() + data[0].m_times[0][0] * 3600.0 , startSept2018.toSeconds());
                fructose_assert_eq(data[1].m_start.toSeconds() + data[1].m_times[0][0] * 3600.0 , startSept2018.toSeconds() + 3600.0 * 6.0);

                DateTime statTime;
                Value statValue;
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

            delete drugTreatment;
        }

        // Delete all dynamically allocated objects
        delete component;
        delete drugModel;

    }

    void testResidualErrorModelAdditive() {
        BuildConstantElimination builder;
        DrugModel *drugModel = builder.buildDrugModel(
                    ResidualErrorType::ADDITIVE,
                    std::vector<Value>({10.0}));

        fructose_assert(drugModel != nullptr);

        fructose_assert(drugModel->checkInvariants());

        DrugModelChecker checker;

        std::shared_ptr<PkModel> sharedPkModel;
        sharedPkModel = std::make_shared<PkModel>("test.constantelimination");

        bool addResult = sharedPkModel->addIntakeIntervalCalculatorFactory(AbsorptionModel::INTRAVASCULAR, ConstantEliminationBolus::getCreator());
        fructose_assert(addResult);

        PkModelCollection *collection = new PkModelCollection();
        collection->addPkModel(sharedPkModel);
        DrugModelChecker::CheckerResult_t checkerResult = checker.checkDrugModel(drugModel, collection);

        fructose_assert(checkerResult.ok);

        if (!checkerResult.ok) {
            std::cout << checkerResult.errorMessage << std::endl;
        }

        // Now the drug model is ready to be used


        IComputingService *component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

        fructose_assert( component != nullptr);

        static_cast<ComputingComponent *>(component)->setPkModelCollection(collection);


        {

            DrugTreatment *drugTreatment;
            const FormulationAndRoute route(Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::INTRAVASCULAR);

            buildDrugTreatment(drugTreatment, route);

            drugTreatment->addCovariate(std::make_unique<PatientCovariate>("covS", "0.0", DataType::Double, Unit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
            drugTreatment->addCovariate(std::make_unique<PatientCovariate>("covA", "0.0", DataType::Double, Unit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
            drugTreatment->addCovariate(std::make_unique<PatientCovariate>("covR", "0.0", DataType::Double, Unit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));


            std::unique_ptr<ComputingTraits> computingTraits = std::make_unique<ComputingTraits>();

            RequestResponseId requestResponseId = "1";
            Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
            Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
            double nbPointsPerHour = 10.0;
            ComputingOption computingOption(PredictionParameterType::Apriori, CompartmentsOption::MainCompartment, true);


            std::unique_ptr<ComputingTraitPercentiles> traitsPercentiles =
                    std::make_unique<ComputingTraitPercentiles>(
                        requestResponseId, start, end, percentileRanks, nbPointsPerHour, computingOption);

            computingTraits->addTrait(std::move(traitsPercentiles));


            ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(computingTraits));

            std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

            ComputingResult result;
            result = component->compute(request, response);

            fructose_assert( result == ComputingResult::Success);

            const std::vector<std::unique_ptr<SingleComputingResponse> > &responses = response.get()->getResponses();

            fructose_assert_eq(responses.size(), size_t{1});

            {
                fructose_assert(dynamic_cast<PercentilesResponse*>(responses[0].get()) != nullptr);
                const PercentilesResponse *resp = dynamic_cast<PercentilesResponse*>(responses[0].get());

                fructose_assert_eq(resp->getNbRanks(), percentileRanks.size());

                for(size_t p = 0; p < resp->getNbRanks(); p++) {
                    DateTime statTime;
                    Value statValue;
                    resp->getData(p,0).m_statistics.getStatistic(0, CycleStatisticType::Mean).getValue(statTime, statValue);
                    //fructose_assert_eq(statValue, 200001.0);

                    // Multiply the Inv CDF by SD (10), and by 1000.0 because of mg/l
                    double expectedValue = invCdf[p] * 10.0 * 1000.0;
                    fructose_assert_double_eq_rel_abs(statValue - 200000.0, expectedValue, 0.02, 10.0 * 1000.0 * 0.06);
                }

            }

            delete drugTreatment;
        }

        // Delete all dynamically allocated objects
        delete component;
        delete drugModel;

    }

    void testResidualErrorModelExponential() {
        BuildConstantElimination builder;
        DrugModel *drugModel = builder.buildDrugModel(
                    ResidualErrorType::EXPONENTIAL,
                    std::vector<Value>({0.2}));

        fructose_assert(drugModel != nullptr);

        fructose_assert(drugModel->checkInvariants());

        DrugModelChecker checker;

        std::shared_ptr<PkModel> sharedPkModel;
        sharedPkModel = std::make_shared<PkModel>("test.constantelimination");

        bool addResult = sharedPkModel->addIntakeIntervalCalculatorFactory(AbsorptionModel::INTRAVASCULAR, ConstantEliminationBolus::getCreator());
        fructose_assert(addResult);

        PkModelCollection *collection = new PkModelCollection();
        collection->addPkModel(sharedPkModel);
        DrugModelChecker::CheckerResult_t checkerResult = checker.checkDrugModel(drugModel, collection);

        fructose_assert(checkerResult.ok);

        if (!checkerResult.ok) {
            std::cout << checkerResult.errorMessage << std::endl;
        }

        // Now the drug model is ready to be used


        IComputingService *component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

        fructose_assert( component != nullptr);

        static_cast<ComputingComponent *>(component)->setPkModelCollection(collection);


        {

            DrugTreatment *drugTreatment;
            const FormulationAndRoute route(Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::INTRAVASCULAR);

            buildDrugTreatment(drugTreatment, route);

            drugTreatment->addCovariate(std::make_unique<PatientCovariate>("covS", "0.0", DataType::Double, Unit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
            drugTreatment->addCovariate(std::make_unique<PatientCovariate>("covA", "0.0", DataType::Double, Unit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
            drugTreatment->addCovariate(std::make_unique<PatientCovariate>("covR", "0.0", DataType::Double, Unit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));


            std::unique_ptr<ComputingTraits> computingTraits = std::make_unique<ComputingTraits>();

            RequestResponseId requestResponseId = "1";
            Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
            Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
            double nbPointsPerHour = 10.0;
            ComputingOption computingOption(PredictionParameterType::Apriori, CompartmentsOption::MainCompartment, true);

            std::unique_ptr<ComputingTraitPercentiles> traitsPercentiles =
                    std::make_unique<ComputingTraitPercentiles>(
                        requestResponseId, start, end, percentileRanks, nbPointsPerHour, computingOption);

            computingTraits->addTrait(std::move(traitsPercentiles));


            ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(computingTraits));

            std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

            ComputingResult result;
            result = component->compute(request, response);

            fructose_assert( result == ComputingResult::Success);

            const std::vector<std::unique_ptr<SingleComputingResponse> > &responses = response.get()->getResponses();

            fructose_assert_eq(responses.size(), size_t{1});

            {
                fructose_assert(dynamic_cast<PercentilesResponse*>(responses[0].get()) != nullptr);
                const PercentilesResponse *resp = dynamic_cast<PercentilesResponse*>(responses[0].get());

                fructose_assert_eq(resp->getNbRanks(), percentileRanks.size());

                for(size_t p = 0; p < resp->getNbRanks(); p++) {
                    DateTime statTime;
                    Value statValue;
                    resp->getData(p,0).m_statistics.getStatistic(0, CycleStatisticType::Mean).getValue(statTime, statValue);
                    //fructose_assert_eq(statValue, 200001.0);

                    // Multiply the Inv CDF by SD (0.2)
                    double expectedValue = 200000.0 * std::exp(invCdf[p] * 0.2);
                    fructose_assert_double_eq_rel_abs(statValue, expectedValue, 0.01, 0.0);
                }

            }
            delete drugTreatment;
        }

        // Delete all dynamically allocated objects
        delete component;
        delete drugModel;

    }


    void testResidualErrorModelProportional() {
        BuildConstantElimination builder;
        DrugModel *drugModel = builder.buildDrugModel(
                    ResidualErrorType::PROPORTIONAL,
                    std::vector<Value>({0.2}));

        fructose_assert(drugModel != nullptr);

        fructose_assert(drugModel->checkInvariants());

        DrugModelChecker checker;

        std::shared_ptr<PkModel> sharedPkModel;
        sharedPkModel = std::make_shared<PkModel>("test.constantelimination");

        bool addResult = sharedPkModel->addIntakeIntervalCalculatorFactory(AbsorptionModel::INTRAVASCULAR, ConstantEliminationBolus::getCreator());
        fructose_assert(addResult);

        PkModelCollection *collection = new PkModelCollection();
        collection->addPkModel(sharedPkModel);
        DrugModelChecker::CheckerResult_t checkerResult = checker.checkDrugModel(drugModel, collection);

        fructose_assert(checkerResult.ok);

        if (!checkerResult.ok) {
            std::cout << checkerResult.errorMessage << std::endl;
        }

        // Now the drug model is ready to be used


        IComputingService *component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

        fructose_assert( component != nullptr);

        static_cast<ComputingComponent *>(component)->setPkModelCollection(collection);

        {

            DrugTreatment *drugTreatment;
            const FormulationAndRoute route(Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::INTRAVASCULAR);

            buildDrugTreatment(drugTreatment, route);

            drugTreatment->addCovariate(std::make_unique<PatientCovariate>("covS", "0.0", DataType::Double, Unit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
            drugTreatment->addCovariate(std::make_unique<PatientCovariate>("covA", "0.0", DataType::Double, Unit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
            drugTreatment->addCovariate(std::make_unique<PatientCovariate>("covR", "0.0", DataType::Double, Unit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));


            std::unique_ptr<ComputingTraits> computingTraits = std::make_unique<ComputingTraits>();

            RequestResponseId requestResponseId = "1";
            Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
            Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
            double nbPointsPerHour = 10.0;
            ComputingOption computingOption(PredictionParameterType::Apriori, CompartmentsOption::MainCompartment, true);

            std::unique_ptr<ComputingTraitPercentiles> traitsPercentiles =
                    std::make_unique<ComputingTraitPercentiles>(
                        requestResponseId, start, end, percentileRanks, nbPointsPerHour, computingOption);

            computingTraits->addTrait(std::move(traitsPercentiles));


            ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(computingTraits));

            std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

            ComputingResult result;
            result = component->compute(request, response);

            fructose_assert( result == ComputingResult::Success);

            const std::vector<std::unique_ptr<SingleComputingResponse> > &responses = response.get()->getResponses();

            fructose_assert_eq(responses.size(), size_t{1});

            {
                fructose_assert(dynamic_cast<PercentilesResponse*>(responses[0].get()) != nullptr);
                const PercentilesResponse *resp = dynamic_cast<PercentilesResponse*>(responses[0].get());

                fructose_assert_eq(resp->getNbRanks(), percentileRanks.size());

                for(size_t p = 0; p < resp->getNbRanks(); p++) {
                    DateTime statTime;
                    Value statValue;
                    resp->getData(p,0).m_statistics.getStatistic(0, CycleStatisticType::Mean).getValue(statTime, statValue);
                    //fructose_assert_eq(statValue, 200001.0);

                    // Multiply the Inv CDF by SD (0.2)
                    double expectedValue = 200000.0 * (1 + invCdf[p] * 0.2);
                    fructose_assert_double_eq_rel_abs(statValue, expectedValue, 0.01, 0.0);
                }

            }

            delete drugTreatment;
        }

        // Delete all dynamically allocated objects
        delete component;
        delete drugModel;

    }

    void testResidualErrorModelMixed() {
        BuildConstantElimination builder;
        DrugModel *drugModel = builder.buildDrugModel(
                    ResidualErrorType::MIXED,
                    std::vector<Value>({10.0, 0.2}));

        fructose_assert(drugModel != nullptr);

        fructose_assert(drugModel->checkInvariants());

        DrugModelChecker checker;

        std::shared_ptr<PkModel> sharedPkModel;
        sharedPkModel = std::make_shared<PkModel>("test.constantelimination");

        bool addResult = sharedPkModel->addIntakeIntervalCalculatorFactory(AbsorptionModel::INTRAVASCULAR, ConstantEliminationBolus::getCreator());
        fructose_assert(addResult);

        PkModelCollection *collection = new PkModelCollection();
        collection->addPkModel(sharedPkModel);
        DrugModelChecker::CheckerResult_t checkerResult = checker.checkDrugModel(drugModel, collection);

        fructose_assert(checkerResult.ok);

        if (!checkerResult.ok) {
            std::cout << checkerResult.errorMessage << std::endl;
        }

        // Now the drug model is ready to be used


        IComputingService *component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

        fructose_assert( component != nullptr);

        static_cast<ComputingComponent *>(component)->setPkModelCollection(collection);


        {

            DrugTreatment *drugTreatment;
            const FormulationAndRoute route(Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::INTRAVASCULAR);

            buildDrugTreatment(drugTreatment, route);

            drugTreatment->addCovariate(std::make_unique<PatientCovariate>("covS", "0.0", DataType::Double, Unit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
            drugTreatment->addCovariate(std::make_unique<PatientCovariate>("covA", "0.0", DataType::Double, Unit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
            drugTreatment->addCovariate(std::make_unique<PatientCovariate>("covR", "0.0", DataType::Double, Unit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));


            std::unique_ptr<ComputingTraits> computingTraits = std::make_unique<ComputingTraits>();

            RequestResponseId requestResponseId = "1";
            Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
            Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
            double nbPointsPerHour = 10.0;
            ComputingOption computingOption(PredictionParameterType::Apriori, CompartmentsOption::MainCompartment, true);

            std::unique_ptr<ComputingTraitPercentiles> traitsPercentiles =
                    std::make_unique<ComputingTraitPercentiles>(
                        requestResponseId, start, end, percentileRanks, nbPointsPerHour, computingOption);

            computingTraits->addTrait(std::move(traitsPercentiles));


            ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(computingTraits));

            std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

            ComputingResult result;
            result = component->compute(request, response);

            fructose_assert( result == ComputingResult::Success);

            const std::vector<std::unique_ptr<SingleComputingResponse> > &responses = response.get()->getResponses();

            fructose_assert_eq(responses.size(), size_t{1});

            {
                fructose_assert(dynamic_cast<PercentilesResponse*>(responses[0].get()) != nullptr);
                const PercentilesResponse *resp = dynamic_cast<PercentilesResponse*>(responses[0].get());

                fructose_assert_eq(resp->getNbRanks(), percentileRanks.size());

                for(size_t p = 0; p < resp->getNbRanks(); p++) {
                    DateTime statTime;
                    Value statValue;
                    resp->getData(p,0).m_statistics.getStatistic(0, CycleStatisticType::Mean).getValue(statTime, statValue);
                    //fructose_assert_eq(statValue, 200001.0);

                    // Multiply the Inv CDF by SD (0.2)
                    double expectedValue = 200000.0 + invCdf[p] * std::sqrt(std::pow(200000.0 * 0.2, 2)  + std::pow(10.0, 2));
                    fructose_assert_double_eq_rel_abs(statValue, expectedValue, 0.02, 0.0);
                }

            }

            delete drugTreatment;
        }

        // Delete all dynamically allocated objects
        delete component;
        delete drugModel;

    }



    void testParamAdditive() {
        BuildConstantElimination builder;
        DrugModel *drugModel = builder.buildDrugModel(
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
        sharedPkModel = std::make_shared<PkModel>("test.constantelimination");

        bool addResult = sharedPkModel->addIntakeIntervalCalculatorFactory(AbsorptionModel::INTRAVASCULAR, ConstantEliminationBolus::getCreator());
        fructose_assert(addResult);

        PkModelCollection *collection = new PkModelCollection();
        collection->addPkModel(sharedPkModel);
        DrugModelChecker::CheckerResult_t checkerResult = checker.checkDrugModel(drugModel, collection);

        fructose_assert(checkerResult.ok);

        if (!checkerResult.ok) {
            std::cout << checkerResult.errorMessage << std::endl;
        }

        // Now the drug model is ready to be used


        IComputingService *component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

        fructose_assert( component != nullptr);

        static_cast<ComputingComponent *>(component)->setPkModelCollection(collection);


        {

            DrugTreatment *drugTreatment;
            const FormulationAndRoute route(Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::INTRAVASCULAR);

            buildDrugTreatment(drugTreatment, route);

            drugTreatment->addCovariate(std::make_unique<PatientCovariate>("covS", "0.0", DataType::Double, Unit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
            drugTreatment->addCovariate(std::make_unique<PatientCovariate>("covA", "0.0", DataType::Double, Unit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
            drugTreatment->addCovariate(std::make_unique<PatientCovariate>("covR", "0.0", DataType::Double, Unit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));


            std::unique_ptr<ComputingTraits> computingTraits = std::make_unique<ComputingTraits>();

            RequestResponseId requestResponseId = "1";
            Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
            Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
            double nbPointsPerHour = 10.0;
            ComputingOption computingOption(PredictionParameterType::Apriori, CompartmentsOption::MainCompartment, true);

            std::unique_ptr<ComputingTraitPercentiles> traitsPercentiles =
                    std::make_unique<ComputingTraitPercentiles>(
                        requestResponseId, start, end, percentileRanks, nbPointsPerHour, computingOption);

            computingTraits->addTrait(std::move(traitsPercentiles));


            ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(computingTraits));

            std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

            ComputingResult result;
            result = component->compute(request, response);

            fructose_assert( result == ComputingResult::Success);

            const std::vector<std::unique_ptr<SingleComputingResponse> > &responses = response.get()->getResponses();

            fructose_assert_eq(responses.size(), size_t{1});

            {
                fructose_assert(dynamic_cast<PercentilesResponse*>(responses[0].get()) != nullptr);
                const PercentilesResponse *resp = dynamic_cast<PercentilesResponse*>(responses[0].get());

                fructose_assert_eq(resp->getNbRanks(), percentileRanks.size());

                for(size_t p = 0; p < resp->getNbRanks(); p++) {
                    DateTime statTime;
                    Value statValue;
                    resp->getData(p,0).m_statistics.getStatistic(0, CycleStatisticType::Mean).getValue(statTime, statValue);
                    //fructose_assert_eq(statValue, 200001.0);

                    // Multiply the Inv CDF by SD (10), and by 1000.0 because of mg/l
                    double expectedValue = 200000.0 + invCdf[p] * 1000.0;
                    fructose_assert_double_eq_rel_abs(statValue, expectedValue, .02, 0.02);
                }

            }

            delete drugTreatment;
        }

        // Delete all dynamically allocated objects
        delete component;
        delete drugModel;

    }


    void testParamAdditiveResidualErrorModelAdditive() {
        BuildConstantElimination builder;
        DrugModel *drugModel = builder.buildDrugModel(
                    ResidualErrorType::ADDITIVE,
                    std::vector<Value>({10.0}),
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
        sharedPkModel = std::make_shared<PkModel>("test.constantelimination");

        bool addResult = sharedPkModel->addIntakeIntervalCalculatorFactory(AbsorptionModel::INTRAVASCULAR, ConstantEliminationBolus::getCreator());
        fructose_assert(addResult);

        PkModelCollection *collection = new PkModelCollection();
        collection->addPkModel(sharedPkModel);
        DrugModelChecker::CheckerResult_t checkerResult = checker.checkDrugModel(drugModel, collection);

        fructose_assert(checkerResult.ok);

        if (!checkerResult.ok) {
            std::cout << checkerResult.errorMessage << std::endl;
        }

        // Now the drug model is ready to be used


        IComputingService *component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

        fructose_assert( component != nullptr);

        static_cast<ComputingComponent *>(component)->setPkModelCollection(collection);


        {

            DrugTreatment *drugTreatment;
            const FormulationAndRoute route(Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::INTRAVASCULAR);

            buildDrugTreatment(drugTreatment, route);

            drugTreatment->addCovariate(std::make_unique<PatientCovariate>("covS", "0.0", DataType::Double, Unit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
            drugTreatment->addCovariate(std::make_unique<PatientCovariate>("covA", "0.0", DataType::Double, Unit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
            drugTreatment->addCovariate(std::make_unique<PatientCovariate>("covR", "0.0", DataType::Double, Unit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));


            std::unique_ptr<ComputingTraits> computingTraits = std::make_unique<ComputingTraits>();

            RequestResponseId requestResponseId = "1";
            Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
            Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
            double nbPointsPerHour = 10.0;
            ComputingOption computingOption(PredictionParameterType::Apriori, CompartmentsOption::MainCompartment, true);

            std::unique_ptr<ComputingTraitPercentiles> traitsPercentiles =
                    std::make_unique<ComputingTraitPercentiles>(
                        requestResponseId, start, end, percentileRanks, nbPointsPerHour, computingOption);

            computingTraits->addTrait(std::move(traitsPercentiles));


            ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(computingTraits));

            std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

            ComputingResult result;
            result = component->compute(request, response);

            fructose_assert( result == ComputingResult::Success);

            const std::vector<std::unique_ptr<SingleComputingResponse> > &responses = response.get()->getResponses();

            fructose_assert_eq(responses.size(), size_t{1});

            {
                fructose_assert(dynamic_cast<PercentilesResponse*>(responses[0].get()) != nullptr);
                const PercentilesResponse *resp = dynamic_cast<PercentilesResponse*>(responses[0].get());

                fructose_assert_eq(resp->getNbRanks(), percentileRanks.size());

                for(size_t p = 0; p < resp->getNbRanks(); p++) {
                    DateTime statTime;
                    Value statValue;
                    resp->getData(p,0).m_statistics.getStatistic(0, CycleStatisticType::Mean).getValue(statTime, statValue);
                    //fructose_assert_eq(statValue, 200001.0);

                    // Multiply the Inv CDF by SD (10), and by 1000.0 because of mg/l
                    double expectedValue = 200000.0 + invCdf[p]*  std::sqrt(std::pow(1000.0, 2) + std::pow(10.0 * 1000.0, 2));

                    fructose_assert_double_eq_rel_abs(statValue, expectedValue, .01, 0.01);
                }

            }

            delete drugTreatment;
        }

        // Delete all dynamically allocated objects
        delete component;
        delete drugModel;

    }

    void testParamExponentialResidualErrorModelExponential() {
        BuildConstantElimination builder;
        DrugModel *drugModel = builder.buildDrugModel(
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
        sharedPkModel = std::make_shared<PkModel>("test.constantelimination");

        bool addResult = sharedPkModel->addIntakeIntervalCalculatorFactory(AbsorptionModel::INTRAVASCULAR, ConstantEliminationBolus::getCreator());
        fructose_assert(addResult);

        PkModelCollection *collection = new PkModelCollection();
        collection->addPkModel(sharedPkModel);
        DrugModelChecker::CheckerResult_t checkerResult = checker.checkDrugModel(drugModel, collection);

        fructose_assert(checkerResult.ok);

        if (!checkerResult.ok) {
            std::cout << checkerResult.errorMessage << std::endl;
        }

        // Now the drug model is ready to be used


        IComputingService *component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

        fructose_assert( component != nullptr);

        static_cast<ComputingComponent *>(component)->setPkModelCollection(collection);


        {

            DrugTreatment *drugTreatment;
            const FormulationAndRoute route(Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::INTRAVASCULAR);

            buildDrugTreatment(drugTreatment, route);

            drugTreatment->addCovariate(std::make_unique<PatientCovariate>("covS", "0.0", DataType::Double, Unit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
            drugTreatment->addCovariate(std::make_unique<PatientCovariate>("covA", "0.0", DataType::Double, Unit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
            drugTreatment->addCovariate(std::make_unique<PatientCovariate>("covR", "0.0", DataType::Double, Unit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));


            std::unique_ptr<ComputingTraits> computingTraits = std::make_unique<ComputingTraits>();

            RequestResponseId requestResponseId = "1";
            Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
            Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
            double nbPointsPerHour = 10.0;
            ComputingOption computingOption(PredictionParameterType::Apriori, CompartmentsOption::MainCompartment, true);

            std::unique_ptr<ComputingTraitPercentiles> traitsPercentiles =
                    std::make_unique<ComputingTraitPercentiles>(
                        requestResponseId, start, end, percentileRanks, nbPointsPerHour, computingOption);

            computingTraits->addTrait(std::move(traitsPercentiles));


            ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(computingTraits));

            std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

            ComputingResult result;
            result = component->compute(request, response);

            fructose_assert( result == ComputingResult::Success);

            const std::vector<std::unique_ptr<SingleComputingResponse> > &responses = response.get()->getResponses();

            fructose_assert_eq(responses.size(), size_t{1});

            {
                fructose_assert(dynamic_cast<PercentilesResponse*>(responses[0].get()) != nullptr);
                const PercentilesResponse *resp = dynamic_cast<PercentilesResponse*>(responses[0].get());

                fructose_assert_eq(resp->getNbRanks(), percentileRanks.size());

                for(size_t p = 0; p < resp->getNbRanks(); p++) {
                    DateTime statTime;
                    Value statValue;
                    resp->getData(p,0).m_statistics.getStatistic(0, CycleStatisticType::Mean).getValue(statTime, statValue);
                    //fructose_assert_eq(statValue, 200001.0);


                    // Calculate the resulting standard deviation
                    double newStd = std::sqrt(std::pow(0.2, 2) + std::pow(0.3, 2));


                    // Multiply the Inv CDF by the new standard deviation
                    double expectedValue = 200000.0 * std::exp(invCdf[p] * newStd);
                    fructose_assert_double_eq_rel_abs(statValue, expectedValue, 0.02, 0.0);
                }

            }
            delete drugTreatment;
        }

        // Delete all dynamically allocated objects
        delete component;
        delete drugModel;

    }

    void testParamProportionalResidualErrorModelProportional() {
        BuildConstantElimination builder;
        DrugModel *drugModel = builder.buildDrugModel(
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
        sharedPkModel = std::make_shared<PkModel>("test.constantelimination");

        bool addResult = sharedPkModel->addIntakeIntervalCalculatorFactory(AbsorptionModel::INTRAVASCULAR, ConstantEliminationBolus::getCreator());
        fructose_assert(addResult);

        PkModelCollection *collection = new PkModelCollection();
        collection->addPkModel(sharedPkModel);
        DrugModelChecker::CheckerResult_t checkerResult = checker.checkDrugModel(drugModel, collection);

        fructose_assert(checkerResult.ok);

        if (!checkerResult.ok) {
            std::cout << checkerResult.errorMessage << std::endl;
        }

        // Now the drug model is ready to be used


        IComputingService *component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

        fructose_assert( component != nullptr);

        static_cast<ComputingComponent *>(component)->setPkModelCollection(collection);


        {

            DrugTreatment *drugTreatment;
            const FormulationAndRoute route(Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::INTRAVASCULAR);

            buildDrugTreatment(drugTreatment, route);

            drugTreatment->addCovariate(std::make_unique<PatientCovariate>("covS", "0.0", DataType::Double, Unit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
            drugTreatment->addCovariate(std::make_unique<PatientCovariate>("covA", "0.0", DataType::Double, Unit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
            drugTreatment->addCovariate(std::make_unique<PatientCovariate>("covR", "0.0", DataType::Double, Unit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));


            std::unique_ptr<ComputingTraits> computingTraits = std::make_unique<ComputingTraits>();

            RequestResponseId requestResponseId = "1";
            Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
            Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
            double nbPointsPerHour = 10.0;
            ComputingOption computingOption(PredictionParameterType::Apriori, CompartmentsOption::MainCompartment, true);

            std::unique_ptr<ComputingTraitPercentiles> traitsPercentiles =
                    std::make_unique<ComputingTraitPercentiles>(
                        requestResponseId, start, end, percentileRanks, nbPointsPerHour, computingOption);

            computingTraits->addTrait(std::move(traitsPercentiles));


            ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(computingTraits));

            std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

            ComputingResult result;
            result = component->compute(request, response);

            fructose_assert( result == ComputingResult::Success);

            const std::vector<std::unique_ptr<SingleComputingResponse> > &responses = response.get()->getResponses();

            fructose_assert_eq(responses.size(), size_t{1});

            {
                fructose_assert(dynamic_cast<PercentilesResponse*>(responses[0].get()) != nullptr);
                const PercentilesResponse *resp = dynamic_cast<PercentilesResponse*>(responses[0].get());

                fructose_assert_eq(resp->getNbRanks(), percentileRanks.size());

                for(size_t p = 0; p < resp->getNbRanks(); p++) {
                    DateTime statTime;
                    Value statValue;
                    resp->getData(p,0).m_statistics.getStatistic(0, CycleStatisticType::Mean).getValue(statTime, statValue);
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
            delete drugTreatment;
        }

        // Delete all dynamically allocated objects
        delete component;
        delete drugModel;

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
        // testParamProportionalResidualErrorModelProportional();
    }

};



#endif // TEST_CONSTANTELIMINATIONBOLUS_H
