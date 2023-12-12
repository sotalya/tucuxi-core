//@@license@@

#include <chrono>
#include <iostream>
#include <memory>
#include <date/date.h>

#include <gtest/gtest.h>

#include "../gtest_core.h"

#include "tucucore/computingcomponent.h"
#include "tucucore/computingservice/computingrequest.h"
#include "tucucore/drugmodelchecker.h"
#include "tucucore/drugmodelimport.h"
#include "tucucore/drugtreatment/drugtreatment.h"
#include "tucucore/drugtreatment/patientcovariate.h"
#include "tucucore/pkmodel.h"

#include "../pkmodels/pkasymptotic.h"
#include "../testutils.h"
#include "buildpkasymptotic.h"

using namespace std::chrono_literals;
using namespace date;

using namespace Tucuxi::Core;

TEST (Core_TestPkAsymptotic, test0){
    BuildPkAsymptotic builder;
    auto drugModel = builder.buildDrugModel();

    ASSERT_TRUE(drugModel != nullptr);

    ASSERT_TRUE(drugModel->checkInvariants());

    DrugModelChecker checker;

    std::shared_ptr<PkModel> sharedPkModel;
    sharedPkModel = std::make_shared<PkModel>("test.pkasymptotic", PkModel::AllowMultipleRoutes::No);

    bool addResult = sharedPkModel->addIntakeIntervalCalculatorFactory(
            AbsorptionModel::Extravascular, PkAsymptotic::getCreator());
    ASSERT_TRUE(addResult);

    sharedPkModel->addParameterList(AbsorptionModel::Extravascular, PkAsymptotic::getParametersId());

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
        const FormulationAndRoute route(
                Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);

        DateTime startSept2018(
                date::year_month_day(date::year(2018), date::month(9), date::day(1)),
                Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));

        auto drugTreatment = buildDrugTreatment(route, startSept2018);

        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covR", "0.5", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covT", "1.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));

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
        ASSERT_TRUE(dynamic_cast<const SinglePredictionData*>(responseData) != nullptr);
        const SinglePredictionData* resp = dynamic_cast<const SinglePredictionData*>(responseData);

        ASSERT_EQ(resp->getCompartmentInfos().size(), static_cast<size_t>(1));
        ASSERT_EQ(resp->getCompartmentInfos()[0].getId(), "analyte");
        ASSERT_EQ(
                resp->getCompartmentInfos()[0].getType(), CompartmentInfo::CompartmentType::ActiveMoietyAndAnalyte);

        std::vector<CycleData> data = resp->getData();
        ASSERT_EQ(data.size(), static_cast<size_t>(16));
        ASSERT_EQ(data[0].m_concentrations.size(), static_cast<size_t>(1));
        ASSERT_DOUBLE_EQ(data[0].m_concentrations[0][0], 0.0);

        ASSERT_DOUBLE_EQ(data[0].m_start.toSeconds() + data[0].m_times[0][0] * 3600.0, startSept2018.toSeconds());
        ASSERT_DOUBLE_EQ(
                data[1].m_start.toSeconds() + data[1].m_times[0][0] * 3600.0,
                startSept2018.toSeconds() + 3600.0 * 6.0);

        DateTime statTime = DateTime::now();
        Value statValue = 0.0;
        data[0].m_statistics.getStatistic(0, CycleStatisticType::AUC).getValue(statTime, statValue);
        ASSERT_DOUBLE_EQ(statValue, (0.0 + 200000.0 * 0.5) / 2.0 * 6.0);

        data[0].m_statistics.getStatistic(0, CycleStatisticType::CumulativeAuc).getValue(statTime, statValue);
        ASSERT_DOUBLE_EQ(statValue, (0.0 + 200000.0 * 0.5) / 2.0 * 6.0);

        data[0].m_statistics.getStatistic(0, CycleStatisticType::Peak).getValue(statTime, statValue);
        ASSERT_DOUBLE_EQ(statValue, 200000.0 * 0.5);

        data[0].m_statistics.getStatistic(0, CycleStatisticType::Mean).getValue(statTime, statValue);
        ASSERT_DOUBLE_EQ(statValue, 200000.0 * 0.5 / 2.0);

        data[1].m_statistics.getStatistic(0, CycleStatisticType::AUC).getValue(statTime, statValue);
        ASSERT_DOUBLE_EQ(statValue, (100000.0 + ((200000.0 - 100000.0) * 0.5) / 2.0) * 6.0);

        data[1].m_statistics.getStatistic(0, CycleStatisticType::CumulativeAuc).getValue(statTime, statValue);
        ASSERT_DOUBLE_EQ(
                statValue,
                (0.0 + 200000.0 * 0.5) / 2.0 * 6.0 + (100000.0 + ((200000.0 - 100000.0) * 0.5) / 2.0) * 6.0);

        data[1].m_statistics.getStatistic(0, CycleStatisticType::Peak).getValue(statTime, statValue);
        ASSERT_DOUBLE_EQ(statValue, 150000.0);

        data[1].m_statistics.getStatistic(0, CycleStatisticType::Mean).getValue(statTime, statValue);
        ASSERT_DOUBLE_EQ(statValue, 100000.0 + 25000.0);

        double residual = 0.0;
        double dose = 200000.0;
        double cumulativeAuc = 0.0;
        double rate = 0.5;
        for (size_t c = 0; c < 16; c++) {

            double auc = (residual + ((dose - residual) * rate) / 2.0) * 6.0;
            cumulativeAuc += auc;

            data[c].m_statistics.getStatistic(0, CycleStatisticType::AUC).getValue(statTime, statValue);
            ASSERT_DOUBLE_EQ(statValue, auc);

            data[c].m_statistics.getStatistic(0, CycleStatisticType::CumulativeAuc).getValue(statTime, statValue);
            ASSERT_DOUBLE_EQ(statValue, cumulativeAuc);
            residual = residual + ((dose - residual) * rate);
        }
    }

    // Delete all dynamically allocated objects
    delete component;
}

TEST (Core_TestPkAsymptotic, Adjustments){
    BuildPkAsymptotic builder;
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
    sharedPkModel = std::make_shared<PkModel>("test.pkasymptotic", PkModel::AllowMultipleRoutes::No);

    bool addResult = sharedPkModel->addIntakeIntervalCalculatorFactory(
            AbsorptionModel::Extravascular, PkAsymptotic::getCreator());
    ASSERT_TRUE(addResult);
    sharedPkModel->addParameterList(AbsorptionModel::Extravascular, PkAsymptotic::getParametersId());

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
        const FormulationAndRoute route(
                Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);

        DateTime startSept2018(
                date::year_month_day(date::year(2018), date::month(9), date::day(1)),
                Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));

        auto drugTreatment = buildDrugTreatment(route, startSept2018);

        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covR", "0.5", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covT", "1.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));

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

        // 8 doses reach the target, for the 3 possible intervals
        ASSERT_EQ(resp->getAdjustments().size(), static_cast<size_t>(24));

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

TEST (Core_TestPkAsymptotic, AdjustmentsSlowRate){
    BuildPkAsymptotic builder;
    auto drugModel = builder.buildDrugModel();

    ASSERT_TRUE(drugModel != nullptr);


    // Add targetsDrugModel *
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
    sharedPkModel = std::make_shared<PkModel>("test.pkasymptotic", PkModel::AllowMultipleRoutes::No);

    bool addResult = sharedPkModel->addIntakeIntervalCalculatorFactory(
            AbsorptionModel::Extravascular, PkAsymptotic::getCreator());
    ASSERT_TRUE(addResult);
    sharedPkModel->addParameterList(AbsorptionModel::Extravascular, PkAsymptotic::getParametersId());

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
        const FormulationAndRoute route(
                Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);

        DateTime startSept2018(
                date::year_month_day(date::year(2018), date::month(9), date::day(1)),
                Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));

        auto drugTreatment = buildDrugTreatment(route, startSept2018);

        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covR", "0.05", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covT", "1.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));

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

        // 8 doses reach the target, for the 3 possible intervals
        ASSERT_EQ(resp->getAdjustments().size(), static_cast<size_t>(24));

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

TEST (Core_TestPkAsymptotic, AdjustmentsLoadingDose){
    BuildPkAsymptotic builder;
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
    sharedPkModel = std::make_shared<PkModel>("test.pkasymptotic", PkModel::AllowMultipleRoutes::No);

    bool addResult = sharedPkModel->addIntakeIntervalCalculatorFactory(
            AbsorptionModel::Extravascular, PkAsymptotic::getCreator());
    ASSERT_TRUE(addResult);
    sharedPkModel->addParameterList(AbsorptionModel::Extravascular, PkAsymptotic::getParametersId());

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
        const FormulationAndRoute route(
                Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);

        DateTime startSept2018(
                date::year_month_day(date::year(2018), date::month(9), date::day(1)),
                Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));

        auto drugTreatment = buildDrugTreatment(route, startSept2018);

        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covR", "0.5", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covT", "1.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));

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
        LoadingOption loadingOption = LoadingOption::LoadingDoseAllowed;
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

        // 8 doses reach the target, for the 3 possible intervals
        ASSERT_EQ(resp->getAdjustments().size(), static_cast<size_t>(24));

        for (auto& adjustment : resp->getAdjustments()) {

            ASSERT_EQ(adjustment.m_history.getDosageTimeRanges().size(), static_cast<size_t>(2));

            //                    double r = adjustment.m_data[0].m_concentrations[0].back();
            ASSERT_PRED4(double_le_rel_abs,
                         std::abs(adjustment.getData()[0].m_concentrations[0].back()
                                  - adjustment.getData().back().m_concentrations[0].back()),
                         400.0, 0.01, 0.01);

            {
                const auto& timeRange = adjustment.m_history.getDosageTimeRanges()[0];
                const DosageRepeat* dosageRepeat = dynamic_cast<const DosageRepeat*>(timeRange->getDosage());
                ASSERT_TRUE(dosageRepeat != nullptr);
                //                        const LastingDose *dosage = dynamic_cast<const LastingDose*>(dosageRepeat->getDosage());
                //                        double interval0 = dosage->getTimeStep().toHours();
                //                        double dose0 = dosage->getDose();
                //                        std::cout << interval0 << "\t" << dose0 << "\t" << r << std::endl;
            }

            const auto& timeRange = adjustment.m_history.getDosageTimeRanges().back();
            const DosageRepeat* dosageRepeat = dynamic_cast<const DosageRepeat*>(timeRange->getDosage());
            ASSERT_TRUE(dosageRepeat != nullptr);
            const LastingDose* dosage = dynamic_cast<const LastingDose*>(dosageRepeat->getDosage());
            //                    std::cout << dosage->getTimeStep().toHours() << "\t" << dosage->getDose() << "\t" << adjustment.m_data.back().m_concentrations[0].back() << std::endl;


            ASSERT_TRUE(dosage != nullptr);
            ASSERT_PRED4(double_ge_rel_abs, dosage->getDose(), 750.0, 0.01, 0.01);
            ASSERT_PRED4(double_le_rel_abs, dosage->getDose(), 1500.0, 0.01, 0.01);
        }
    }

    // Delete all dynamically allocated objects
    delete component;
}

TEST (Core_TestPkAsymptotic, AdjustmentsFirstDose){
    BuildPkAsymptotic builder;
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
    sharedPkModel = std::make_shared<PkModel>("test.pkasymptotic", PkModel::AllowMultipleRoutes::No);

    bool addResult = sharedPkModel->addIntakeIntervalCalculatorFactory(
            AbsorptionModel::Extravascular, PkAsymptotic::getCreator());
    ASSERT_TRUE(addResult);
    sharedPkModel->addParameterList(AbsorptionModel::Extravascular, PkAsymptotic::getParametersId());

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
        auto drugTreatment = std::make_unique<DrugTreatment>();
        // const FormulationAndRoute route(Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);

        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covR", "0.5", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covT", "1.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));

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

        if (result != ComputingStatus::Ok) {
            return;
        }

        const ComputedData* responseData = response->getData();
        ASSERT_TRUE(dynamic_cast<const AdjustmentData*>(responseData) != nullptr);
        const AdjustmentData* resp = dynamic_cast<const AdjustmentData*>(responseData);

        // 8 doses reach the target, for the 3 possible intervals
        ASSERT_EQ(resp->getAdjustments().size(), static_cast<size_t>(24));

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

TEST (Core_TestPkAsymptotic, AdjustmentsLoadingDoseFirstDose){
    BuildPkAsymptotic builder;
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
    sharedPkModel = std::make_shared<PkModel>("test.pkasymptotic", PkModel::AllowMultipleRoutes::No);

    bool addResult = sharedPkModel->addIntakeIntervalCalculatorFactory(
            AbsorptionModel::Extravascular, PkAsymptotic::getCreator());
    ASSERT_TRUE(addResult);
    sharedPkModel->addParameterList(AbsorptionModel::Extravascular, PkAsymptotic::getParametersId());

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
        const FormulationAndRoute route(
                Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);

        DateTime startSept2018(
                date::year_month_day(date::year(2018), date::month(9), date::day(1)),
                Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));

        auto drugTreatment = buildDrugTreatment(route, startSept2018);

        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covR", "0.5", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                "covT", "1.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));

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
        LoadingOption loadingOption = LoadingOption::LoadingDoseAllowed;
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

        // 8 doses reach the target, for the 3 possible intervals
        ASSERT_EQ(resp->getAdjustments().size(), static_cast<size_t>(24));

        for (auto& adjustment : resp->getAdjustments()) {

            ASSERT_EQ(adjustment.m_history.getDosageTimeRanges().size(), static_cast<size_t>(2));

            //                    double r = adjustment.m_data[0].m_concentrations[0].back();
            ASSERT_PRED4(double_le_rel_abs,
                         std::abs(adjustment.getData()[0].m_concentrations[0].back()
                                  - adjustment.getData().back().m_concentrations[0].back()),
                         400.0, 0.01, 0.01);

            {
                const auto& timeRange = adjustment.m_history.getDosageTimeRanges()[0];
                const DosageRepeat* dosageRepeat = dynamic_cast<const DosageRepeat*>(timeRange->getDosage());
                ASSERT_TRUE(dosageRepeat != nullptr);
                //                        const LastingDose *dosage = dynamic_cast<const LastingDose*>(dosageRepeat->getDosage());
                //                        double interval0 = dosage->getTimeStep().toHours();
                //                        double dose0 = dosage->getDose();
                //                        std::cout << interval0 << "\t" << dose0 << "\t" << r << std::endl;
            }

            const auto& timeRange = adjustment.m_history.getDosageTimeRanges().back();
            const DosageRepeat* dosageRepeat = dynamic_cast<const DosageRepeat*>(timeRange->getDosage());
            ASSERT_TRUE(dosageRepeat != nullptr);
            const LastingDose* dosage = dynamic_cast<const LastingDose*>(dosageRepeat->getDosage());
            //                    std::cout << dosage->getTimeStep().toHours() << "\t" << dosage->getDose() << "\t" << adjustment.m_data.back().m_concentrations[0].back() << std::endl;


            ASSERT_TRUE(dosage != nullptr);
            ASSERT_PRED4(double_ge_rel_abs, dosage->getDose(), 750.0, 0.01, 0.01);
            ASSERT_PRED4(double_le_rel_abs, dosage->getDose(), 1500.0, 0.01, 0.01);
        }
    }

    // Delete all dynamically allocated objects
    delete component;
}
