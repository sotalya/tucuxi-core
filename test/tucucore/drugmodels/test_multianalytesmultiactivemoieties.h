#ifndef TEST_MULTIANALYTESMULTIACTIVEMOIETIES_H
#define TEST_MULTIANALYTESMULTIACTIVEMOIETIES_H

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

#include "buildmultianalytesmultiactivemoieties.h"
#include "../pkmodels/constanteliminationbolus.h"
#include "../testutils.h"

using namespace std::chrono_literals;
using namespace date;

using namespace Tucuxi::Core;


struct TestMultiAnalytesMultiActiveMoieties : public fructose::test_base<TestMultiAnalytesMultiActiveMoieties>
{
    TestMultiAnalytesMultiActiveMoieties() { }

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


        //const FormulationAndRoute route("formulation", AdministrationRoute::IntravenousBolus, AbsorptionModel::Extravascular);
        // Add a treatment intake every ten days in June
        // 200mg via a intravascular at 08h30, starting the 01.06
        LastingDose periodicDose(DoseValue(200.0),
                                 TucuUnit("mg"),
                                 _route,
                                 Duration(),
                                 Duration(std::chrono::hours(6)));
        DosageRepeat repeatedDose(periodicDose, 16);
        std::unique_ptr<Tucuxi::Core::DosageTimeRange> sept2018(new Tucuxi::Core::DosageTimeRange(startSept2018, repeatedDose));


        _drugTreatment->getModifiableDosageHistory().addTimeRange(*sept2018);
    }

    void testMultiAnalytesMultiActiveMoieties(const std::string& /* _testName */) {
        BuildMultiAnalytesMultiActiveMoieties builder;
        DrugModel *drugModel = builder.buildDrugModel();

        fructose_assert(drugModel != nullptr);

        fructose_assert(drugModel->checkInvariants());

        DrugModelChecker checker;

        std::shared_ptr<PkModel> sharedPkModel;
        sharedPkModel = std::make_shared<PkModel>("test.constantelimination", PkModel::AllowMultipleRoutes::No);

        bool addResult = sharedPkModel->addIntakeIntervalCalculatorFactory(AbsorptionModel::Extravascular, ConstantEliminationBolus::getCreator());
        fructose_assert(addResult);
        sharedPkModel->addParameterList(AbsorptionModel::Extravascular, ConstantEliminationBolus::getParametersId());

        std::shared_ptr<PkModelCollection> collection = std::make_shared<PkModelCollection>();
        collection->addPkModel(sharedPkModel);
        DrugModelChecker::CheckerResult_t checkerResult = checker.checkDrugModel(drugModel, collection.get());

        fructose_assert(checkerResult.m_ok);

        if (!checkerResult.m_ok) {
            std::cout << checkerResult.m_errorMessage << std::endl;
        }

        // Now the drug model is ready to be used


        IComputingService *component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

        fructose_assert( component != nullptr);

        static_cast<ComputingComponent *>(component)->setPkModelCollection(collection);


        {

            DrugTreatment *drugTreatment;
            const FormulationAndRoute route(Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);

            buildDrugTreatment(drugTreatment, route);

            drugTreatment->addCovariate(std::make_unique<PatientCovariate>("covS", "0.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
            drugTreatment->addCovariate(std::make_unique<PatientCovariate>("covA", "0.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
            drugTreatment->addCovariate(std::make_unique<PatientCovariate>("covR", "0.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));

            RequestResponseId requestResponseId = "1";
            Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
            Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
            double nbPointsPerHour = 10.0;
            ComputingOption computingOption(PredictionParameterType::Apriori, CompartmentsOption::MainCompartment,
                                            RetrieveStatisticsOption::RetrieveStatistics,
                                            RetrieveParametersOption::RetrieveParameters,
                                            RetrieveCovariatesOption::RetrieveCovariates);
            std::unique_ptr<ComputingTraitConcentration> traits =
                    std::make_unique<ComputingTraitConcentration>(
                        requestResponseId, start, end, nbPointsPerHour, computingOption);

            ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(traits));

            std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

            ComputingStatus result;
            result = component->compute(request, response);

            fructose_assert_eq( result, ComputingStatus::Ok);

            const ComputedData* responseData = response->getData();

            {
                fructose_assert(responseData != nullptr);
                const SinglePredictionData *resp = dynamic_cast<const SinglePredictionData*>(responseData);
                fructose_assert(resp != nullptr);
                if (resp == nullptr) {
                    return;
                }

                fructose_assert_eq(resp->getCompartmentInfos().size(), size_t{3});
                fructose_assert_eq(resp->getCompartmentInfos()[0].getId(), "activeMoietyMulti");
                fructose_assert(resp->getCompartmentInfos()[0].getType() == CompartmentInfo::CompartmentType::ActiveMoiety);
                fructose_assert_eq(resp->getCompartmentInfos()[1].getId(), "analyte0");
                fructose_assert(resp->getCompartmentInfos()[1].getType() == CompartmentInfo::CompartmentType::Analyte);
                fructose_assert_eq(resp->getCompartmentInfos()[2].getId(), "analyte1");
                fructose_assert(resp->getCompartmentInfos()[2].getType() == CompartmentInfo::CompartmentType::Analyte);

                std::vector<CycleData> data = resp->getData();
                fructose_assert(data.size() == 16);
                fructose_assert(data[0].m_concentrations.size() == 3);
                fructose_assert_double_eq(data[0].m_concentrations[0][0] , 400000.0);
                fructose_assert_double_eq(data[0].m_concentrations[1][0] , 200000.0);
                fructose_assert_double_eq(data[0].m_concentrations[2][0] , 200000.0);
                fructose_assert_eq(data[0].m_concentrations[0].size() , size_t{61});
                fructose_assert_eq(data[0].m_concentrations[1].size() , size_t{61});
                fructose_assert_eq(data[0].m_concentrations[2].size() , size_t{61});

                DateTime startSept2018(date::year_month_day(date::year(2018), date::month(9), date::day(1)),
                                       Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));

                fructose_assert_eq(data[0].m_start.toSeconds() + data[0].m_times[0][0] * 3600.0 , startSept2018.toSeconds());
                fructose_assert_eq(data[1].m_start.toSeconds() + data[1].m_times[0][0] * 3600.0 , startSept2018.toSeconds() + 3600.0 * 6.0);

                DateTime statTime = DateTime::now();
                Value statValue;
                data[2].m_statistics.getStatistic(2, CycleStatisticType::AUC).getValue(statTime, statValue);
                fructose_assert_double_eq(statValue, 200000.0 * 6.0);

                data[2].m_statistics.getStatistic(2, CycleStatisticType::CumulativeAuc).getValue(statTime, statValue);
                fructose_assert_double_eq(statValue, 200000.0 * 6.0 * 3.0);

                data[2].m_statistics.getStatistic(2, CycleStatisticType::Peak).getValue(statTime, statValue);
                fructose_assert_double_eq(statValue, 200000.0);

                data[2].m_statistics.getStatistic(2, CycleStatisticType::Mean).getValue(statTime, statValue);
                fructose_assert_double_eq(statValue, 200000.0);

                data[1].m_statistics.getStatistic(2, CycleStatisticType::AUC).getValue(statTime, statValue);
                fructose_assert_double_eq(statValue, 200000.0 * 6.0);

                data[1].m_statistics.getStatistic(2, CycleStatisticType::CumulativeAuc).getValue(statTime, statValue);
                fructose_assert_double_eq(statValue, 200000.0 * 12.0);

                data[1].m_statistics.getStatistic(2, CycleStatisticType::Peak).getValue(statTime, statValue);
                fructose_assert_double_eq(statValue, 200000.0);

                data[1].m_statistics.getStatistic(2, CycleStatisticType::Mean).getValue(statTime, statValue);
                fructose_assert_double_eq(statValue, 200000.0);

            }

            delete drugTreatment;
        }

        // Delete all dynamically allocated objects
        delete component;
        delete drugModel;

    }


    void testMultiAnalytesMultiActiveMoietiesConversion(const std::string& /* _testName */) {
        BuildMultiAnalytesMultiActiveMoieties builder;
        DrugModel *drugModel = builder.buildDrugModel(0.3,0.5);

        fructose_assert(drugModel != nullptr);

        fructose_assert(drugModel->checkInvariants());

        DrugModelChecker checker;

        std::shared_ptr<PkModel> sharedPkModel;
        sharedPkModel = std::make_shared<PkModel>("test.constantelimination", PkModel::AllowMultipleRoutes::No);

        bool addResult = sharedPkModel->addIntakeIntervalCalculatorFactory(AbsorptionModel::Extravascular, ConstantEliminationBolus::getCreator());
        fructose_assert(addResult);
        sharedPkModel->addParameterList(AbsorptionModel::Extravascular, ConstantEliminationBolus::getParametersId());

        std::shared_ptr<PkModelCollection> collection = std::make_shared<PkModelCollection>();
        collection->addPkModel(sharedPkModel);
        DrugModelChecker::CheckerResult_t checkerResult = checker.checkDrugModel(drugModel, collection.get());

        fructose_assert(checkerResult.m_ok);

        if (!checkerResult.m_ok) {
            std::cout << checkerResult.m_errorMessage << std::endl;
        }

        // Now the drug model is ready to be used


        IComputingService *component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

        fructose_assert( component != nullptr);

        static_cast<ComputingComponent *>(component)->setPkModelCollection(collection);


        {

            DrugTreatment *drugTreatment;
            const FormulationAndRoute route(Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);

            buildDrugTreatment(drugTreatment, route);

            drugTreatment->addCovariate(std::make_unique<PatientCovariate>("covS", "0.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
            drugTreatment->addCovariate(std::make_unique<PatientCovariate>("covA", "0.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
            drugTreatment->addCovariate(std::make_unique<PatientCovariate>("covR", "0.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));

            RequestResponseId requestResponseId = "1";
            Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
            Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
            double nbPointsPerHour = 10.0;
            ComputingOption computingOption(PredictionParameterType::Apriori, CompartmentsOption::MainCompartment,
                                            RetrieveStatisticsOption::RetrieveStatistics,
                                            RetrieveParametersOption::RetrieveParameters,
                                            RetrieveCovariatesOption::RetrieveCovariates);
            std::unique_ptr<ComputingTraitConcentration> traits =
                    std::make_unique<ComputingTraitConcentration>(
                        requestResponseId, start, end, nbPointsPerHour, computingOption);

            ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(traits));

            std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

            ComputingStatus result;
            result = component->compute(request, response);

            fructose_assert_eq( result, ComputingStatus::Ok);

            const ComputedData* responseData = response->getData();

            fructose_assert(responseData != nullptr);
            if (responseData == nullptr) {
                return;
            }

            {
                fructose_assert(dynamic_cast<const SinglePredictionData*>(responseData) != nullptr);
                const SinglePredictionData *resp = dynamic_cast<const SinglePredictionData*>(responseData);

                fructose_assert_eq(resp->getCompartmentInfos().size(), size_t{3});
                fructose_assert_eq(resp->getCompartmentInfos()[0].getId(), "activeMoietyMulti");
                fructose_assert(resp->getCompartmentInfos()[0].getType() == CompartmentInfo::CompartmentType::ActiveMoiety);
                fructose_assert_eq(resp->getCompartmentInfos()[1].getId(), "analyte0");
                fructose_assert(resp->getCompartmentInfos()[1].getType() == CompartmentInfo::CompartmentType::Analyte);
                fructose_assert_eq(resp->getCompartmentInfos()[2].getId(), "analyte1");
                fructose_assert(resp->getCompartmentInfos()[2].getType() == CompartmentInfo::CompartmentType::Analyte);

                std::vector<CycleData> data = resp->getData();
                fructose_assert(data.size() == 16);
                fructose_assert(data[0].m_concentrations.size() == 3);
                fructose_assert_double_eq(data[0].m_concentrations[0][0] , 200000.0 * 0.8);
                fructose_assert_double_eq(data[0].m_concentrations[1][0] , 200000.0 * 0.3);
                fructose_assert_double_eq(data[0].m_concentrations[2][0] , 200000.0 * 0.5);
                fructose_assert_eq(data[0].m_concentrations[0].size() , size_t{61});
                fructose_assert_eq(data[0].m_concentrations[1].size() , size_t{61});
                fructose_assert_eq(data[0].m_concentrations[2].size() , size_t{61});

                DateTime startSept2018(date::year_month_day(date::year(2018), date::month(9), date::day(1)),
                                       Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));

                fructose_assert_eq(data[0].m_start.toSeconds() + data[0].m_times[0][0] * 3600.0 , startSept2018.toSeconds());
                fructose_assert_eq(data[1].m_start.toSeconds() + data[1].m_times[0][0] * 3600.0 , startSept2018.toSeconds() + 3600.0 * 6.0);

                DateTime statTime = DateTime::now();
                Value statValue;
                data[0].m_statistics.getStatistic(0, CycleStatisticType::AUC).getValue(statTime, statValue);
                fructose_assert_double_eq(statValue, 200000.0 * 6.0 * 0.8);

                data[0].m_statistics.getStatistic(0, CycleStatisticType::CumulativeAuc).getValue(statTime, statValue);
                fructose_assert_double_eq(statValue, 200000.0 * 6.0 * 0.8);

                data[0].m_statistics.getStatistic(0, CycleStatisticType::Peak).getValue(statTime, statValue);
                fructose_assert_double_eq(statValue, 200000.0 * 0.8);

                data[0].m_statistics.getStatistic(0, CycleStatisticType::Mean).getValue(statTime, statValue);
                fructose_assert_double_eq(statValue, 200000.0 * 0.8);


                data[1].m_statistics.getStatistic(0, CycleStatisticType::AUC).getValue(statTime, statValue);
                fructose_assert_double_eq(statValue, 200000.0 * 6.0 * 0.8);

                data[1].m_statistics.getStatistic(0, CycleStatisticType::CumulativeAuc).getValue(statTime, statValue);
                fructose_assert_double_eq(statValue, 200000.0 * 6.0 * 0.8 * 2.0);

                data[1].m_statistics.getStatistic(0, CycleStatisticType::Peak).getValue(statTime, statValue);
                fructose_assert_double_eq(statValue, 200000.0 * 0.8);

                data[1].m_statistics.getStatistic(0, CycleStatisticType::Mean).getValue(statTime, statValue);
                fructose_assert_double_eq(statValue, 200000.0 * 0.8);


                data[1].m_statistics.getStatistic(1, CycleStatisticType::AUC).getValue(statTime, statValue);
                fructose_assert_double_eq(statValue, 200000.0 * 6.0 * 0.3);

                data[1].m_statistics.getStatistic(1, CycleStatisticType::CumulativeAuc).getValue(statTime, statValue);
                fructose_assert_double_eq(statValue, 200000.0 * 12.0 * 0.3);

                data[1].m_statistics.getStatistic(1, CycleStatisticType::Peak).getValue(statTime, statValue);
                fructose_assert_double_eq(statValue, 200000.0 * 0.3);

                data[1].m_statistics.getStatistic(1, CycleStatisticType::Mean).getValue(statTime, statValue);
                fructose_assert_double_eq(statValue, 200000.0 * 0.3);

                data[0].m_statistics.getStatistic(2, CycleStatisticType::AUC).getValue(statTime, statValue);
                fructose_assert_double_eq(statValue, 200000.0 * 6.0 * 0.5);

                data[0].m_statistics.getStatistic(2, CycleStatisticType::CumulativeAuc).getValue(statTime, statValue);
                fructose_assert_double_eq(statValue, 200000.0 * 6.0 * 0.5);

                data[0].m_statistics.getStatistic(2, CycleStatisticType::Peak).getValue(statTime, statValue);
                fructose_assert_double_eq(statValue, 200000.0 * 0.5);

                data[0].m_statistics.getStatistic(2, CycleStatisticType::Mean).getValue(statTime, statValue);
                fructose_assert_double_eq(statValue, 200000.0 * 0.5);

            }

            delete drugTreatment;
        }

        // Delete all dynamically allocated objects
        delete component;
        delete drugModel;

    }


    void testAdjustments(const std::string& /* _testName */) {
        BuildMultiAnalytesMultiActiveMoieties builder;
        DrugModel *drugModel = builder.buildDrugModel(0.3, 0.7);

        fructose_assert(drugModel != nullptr);


        // Add targets
        TargetDefinition *target = new TargetDefinition(TargetType::Residual,
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

        fructose_assert(drugModel->checkInvariants());

        DrugModelChecker checker;

        std::shared_ptr<PkModel> sharedPkModel;
        sharedPkModel = std::make_shared<PkModel>("test.constantelimination", PkModel::AllowMultipleRoutes::No);

        bool addResult = sharedPkModel->addIntakeIntervalCalculatorFactory(AbsorptionModel::Extravascular, ConstantEliminationBolus::getCreator());
        fructose_assert(addResult);
        sharedPkModel->addParameterList(AbsorptionModel::Extravascular, ConstantEliminationBolus::getParametersId());

        std::shared_ptr<PkModelCollection> collection = std::make_shared<PkModelCollection>();
        collection->addPkModel(sharedPkModel);
        DrugModelChecker::CheckerResult_t checkerResult = checker.checkDrugModel(drugModel, collection.get());

        fructose_assert(checkerResult.m_ok);

        if (!checkerResult.m_ok) {
            std::cout << checkerResult.m_errorMessage << std::endl;
        }

        // Now the drug model is ready to be used


        IComputingService *component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

        fructose_assert( component != nullptr);

        static_cast<ComputingComponent *>(component)->setPkModelCollection(collection);


        {

            DrugTreatment *drugTreatment;
            const FormulationAndRoute route(Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);

            buildDrugTreatment(drugTreatment, route);

            drugTreatment->addCovariate(std::make_unique<PatientCovariate>("covS", "0.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
            drugTreatment->addCovariate(std::make_unique<PatientCovariate>("covA", "0.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
            drugTreatment->addCovariate(std::make_unique<PatientCovariate>("covR", "0.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));

            RequestResponseId requestResponseId = "1";
            Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
            Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
            double nbPointsPerHour = 10.0;
            ComputingOption computingOption(PredictionParameterType::Apriori, CompartmentsOption::MainCompartment,
                                            RetrieveStatisticsOption::RetrieveStatistics,
                                            RetrieveParametersOption::RetrieveParameters,
                                            RetrieveCovariatesOption::RetrieveCovariates);

            Tucuxi::Common::DateTime adjustmentTime(2018_y / sep / 3, 8h + 0min);
            BestCandidatesOption adjustmentOption = BestCandidatesOption::AllDosages;
            LoadingOption loadingOption = LoadingOption::NoLoadingDose;
            RestPeriodOption restPeriodOption = RestPeriodOption::NoRestPeriod;
            SteadyStateTargetOption steadyStateTargetOption = SteadyStateTargetOption::AtSteadyState;
            TargetExtractionOption targetExtractionOption = TargetExtractionOption::PopulationValues;
            FormulationAndRouteSelectionOption formulationAndRouteOption = FormulationAndRouteSelectionOption::LastFormulationAndRoute;

            std::unique_ptr<ComputingTraitAdjustment> traits =
                    std::make_unique<ComputingTraitAdjustment>(
                        requestResponseId, start, end, nbPointsPerHour, computingOption, adjustmentTime, adjustmentOption,
                        loadingOption, restPeriodOption, steadyStateTargetOption, targetExtractionOption, formulationAndRouteOption);

            ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(traits));

            std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

            ComputingStatus result;
            result = component->compute(request, response);

            fructose_assert_eq( result, ComputingStatus::Ok);

            const ComputedData* responseData = response->getData();

            fructose_assert(responseData != nullptr);

            if (responseData == nullptr) {
                return;
            }

            {
                fructose_assert(dynamic_cast<const AdjustmentData*>(responseData) != nullptr);
                const AdjustmentData *resp = dynamic_cast<const AdjustmentData*>(responseData);

                fructose_assert_eq(resp->getAdjustments().size(), size_t{9});

                for (auto &adjustment : resp->getAdjustments()) {

                    fructose_assert_eq(adjustment.m_history.getDosageTimeRanges().size(), size_t{1});

                    for (const auto &timeRange : adjustment.m_history.getDosageTimeRanges()) {
                        const DosageRepeat *dosageRepeat = dynamic_cast<const DosageRepeat*>(timeRange->getDosage());
                        fructose_assert(dosageRepeat != nullptr);
                        const SingleDose *dosage = dynamic_cast<const SingleDose*>(dosageRepeat->getDosage());
                        fructose_assert(dosage != nullptr);
                        fructose_assert_double_ge(dosage->getDose(), 750.0);
                        fructose_assert_double_le(dosage->getDose(), 1500.0);
                    }
                }


            }

            delete drugTreatment;
        }

        // Delete all dynamically allocated objects
        delete component;
        delete drugModel;

    }

};


#endif // TEST_MULTIANALYTESMULTIACTIVEMOIETIES_H
