//@@license@@

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
#include "buildmultianalytessingleactivemoiety.h"

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

TEST(Core_TestMultiAnalytesSingleActiveMoiety, MultiAnalytesSingleActiveMoiety)
{
    BuildMultiAnalytesSingleActiveMoiety builder;
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
        const FormulationAndRoute route(
                Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);

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
        std::unique_ptr<ComputingTraitConcentration> traits = std::make_unique<ComputingTraitConcentration>(
                requestResponseId, start, end, nbPointsPerHour, computingOption);

        ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(traits));

        std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

        ComputingStatus result;
        result = component->compute(request, response);

        ASSERT_EQ(result, ComputingStatus::Ok);

        const ComputedData* responseData = response->getData();

        {
            ASSERT_TRUE(responseData != nullptr);
            const SinglePredictionData* resp = dynamic_cast<const SinglePredictionData*>(responseData);
            ASSERT_TRUE(resp != nullptr);
            if (resp == nullptr) {
                return;
            }

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
            ASSERT_DOUBLE_EQ(data[0].m_concentrations[0][0], 400000.0);
            ASSERT_DOUBLE_EQ(data[0].m_concentrations[1][0], 200000.0);
            ASSERT_DOUBLE_EQ(data[0].m_concentrations[2][0], 200000.0);
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
            data[2].m_statistics.getStatistic(2, CycleStatisticType::AUC).getValue(statTime, statValue);
            ASSERT_DOUBLE_EQ(statValue, 200000.0 * 6.0);

            data[2].m_statistics.getStatistic(2, CycleStatisticType::CumulativeAuc).getValue(statTime, statValue);
            ASSERT_DOUBLE_EQ(statValue, 200000.0 * 6.0 * 3.0);

            data[2].m_statistics.getStatistic(2, CycleStatisticType::Peak).getValue(statTime, statValue);
            ASSERT_DOUBLE_EQ(statValue, 200000.0);

            data[2].m_statistics.getStatistic(2, CycleStatisticType::Mean).getValue(statTime, statValue);
            ASSERT_DOUBLE_EQ(statValue, 200000.0);

            data[1].m_statistics.getStatistic(2, CycleStatisticType::AUC).getValue(statTime, statValue);
            ASSERT_DOUBLE_EQ(statValue, 200000.0 * 6.0);

            data[1].m_statistics.getStatistic(2, CycleStatisticType::CumulativeAuc).getValue(statTime, statValue);
            ASSERT_DOUBLE_EQ(statValue, 200000.0 * 12.0);

            data[1].m_statistics.getStatistic(2, CycleStatisticType::Peak).getValue(statTime, statValue);
            ASSERT_DOUBLE_EQ(statValue, 200000.0);

            data[1].m_statistics.getStatistic(2, CycleStatisticType::Mean).getValue(statTime, statValue);
            ASSERT_DOUBLE_EQ(statValue, 200000.0);
        }
    }

    // Delete all dynamically allocated objects
    delete component;
}

TEST(Core_TestMultiAnalytesSingleActiveMoiety, MultiAnalytesSingleActiveMoietyConversion)
{
    BuildMultiAnalytesSingleActiveMoiety builder;
    auto drugModel = builder.buildDrugModel(0.3, 0.5);

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
        const FormulationAndRoute route(
                Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);

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
            ASSERT_PRED4(double_eq_rel_abs, statValue, 200000.0 * 6.0 * 0.8, DEFAULT_PRECISION, DEFAULT_PRECISION);

            data[0].m_statistics.getStatistic(0, CycleStatisticType::CumulativeAuc).getValue(statTime, statValue);
            ASSERT_PRED4(double_eq_rel_abs, statValue, 200000.0 * 6.0 * 0.8, DEFAULT_PRECISION, DEFAULT_PRECISION);

            data[0].m_statistics.getStatistic(0, CycleStatisticType::Peak).getValue(statTime, statValue);
            ASSERT_PRED4(double_eq_rel_abs, statValue, 200000.0 * 0.8, DEFAULT_PRECISION, DEFAULT_PRECISION);

            data[0].m_statistics.getStatistic(0, CycleStatisticType::Mean).getValue(statTime, statValue);
            ASSERT_PRED4(double_eq_rel_abs, statValue, 200000.0 * 0.8, DEFAULT_PRECISION, DEFAULT_PRECISION);


            data[1].m_statistics.getStatistic(0, CycleStatisticType::AUC).getValue(statTime, statValue);
            ASSERT_PRED4(double_eq_rel_abs, statValue, 200000.0 * 6.0 * 0.8, DEFAULT_PRECISION, DEFAULT_PRECISION);

            data[1].m_statistics.getStatistic(0, CycleStatisticType::CumulativeAuc).getValue(statTime, statValue);
            ASSERT_PRED4(
                    double_eq_rel_abs, statValue, 200000.0 * 6.0 * 0.8 * 2.0, DEFAULT_PRECISION, DEFAULT_PRECISION);

            data[1].m_statistics.getStatistic(0, CycleStatisticType::Peak).getValue(statTime, statValue);
            ASSERT_PRED4(double_eq_rel_abs, statValue, 200000.0 * 0.8, DEFAULT_PRECISION, DEFAULT_PRECISION);

            data[1].m_statistics.getStatistic(0, CycleStatisticType::Mean).getValue(statTime, statValue);
            ASSERT_PRED4(double_eq_rel_abs, statValue, 200000.0 * 0.8, DEFAULT_PRECISION, DEFAULT_PRECISION);


            data[1].m_statistics.getStatistic(1, CycleStatisticType::AUC).getValue(statTime, statValue);
            ASSERT_PRED4(double_eq_rel_abs, statValue, 200000.0 * 6.0 * 0.3, DEFAULT_PRECISION, DEFAULT_PRECISION);

            data[1].m_statistics.getStatistic(1, CycleStatisticType::CumulativeAuc).getValue(statTime, statValue);
            ASSERT_PRED4(double_eq_rel_abs, statValue, 200000.0 * 12.0 * 0.3, DEFAULT_PRECISION, DEFAULT_PRECISION);

            data[1].m_statistics.getStatistic(1, CycleStatisticType::Peak).getValue(statTime, statValue);
            ASSERT_PRED4(double_eq_rel_abs, statValue, 200000.0 * 0.3, DEFAULT_PRECISION, DEFAULT_PRECISION);

            data[1].m_statistics.getStatistic(1, CycleStatisticType::Mean).getValue(statTime, statValue);
            ASSERT_PRED4(double_eq_rel_abs, statValue, 200000.0 * 0.3, DEFAULT_PRECISION, DEFAULT_PRECISION);

            data[0].m_statistics.getStatistic(2, CycleStatisticType::AUC).getValue(statTime, statValue);
            ASSERT_PRED4(double_eq_rel_abs, statValue, 200000.0 * 6.0 * 0.5, DEFAULT_PRECISION, DEFAULT_PRECISION);

            data[0].m_statistics.getStatistic(2, CycleStatisticType::CumulativeAuc).getValue(statTime, statValue);
            ASSERT_PRED4(double_eq_rel_abs, statValue, 200000.0 * 6.0 * 0.5, DEFAULT_PRECISION, DEFAULT_PRECISION);

            data[0].m_statistics.getStatistic(2, CycleStatisticType::Peak).getValue(statTime, statValue);
            ASSERT_PRED4(double_eq_rel_abs, statValue, 200000.0 * 0.5, DEFAULT_PRECISION, DEFAULT_PRECISION);

            data[0].m_statistics.getStatistic(2, CycleStatisticType::Mean).getValue(statTime, statValue);
            ASSERT_PRED4(double_eq_rel_abs, statValue, 200000.0 * 0.5, DEFAULT_PRECISION, DEFAULT_PRECISION);
        }
    }

    // Delete all dynamically allocated objects
    delete component;
}

TEST(Core_TestMultiAnalytesSingleActiveMoiety, Adjustments)
{
    BuildMultiAnalytesSingleActiveMoiety builder;
    auto drugModel = builder.buildDrugModel(0.3, 0.7);

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
        const FormulationAndRoute route(
                Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);

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
                    ASSERT_PRED4(double_ge_rel_abs, dosage->getDose(), 750.0, DEFAULT_PRECISION, DEFAULT_PRECISION);
                    ASSERT_PRED4(double_le_rel_abs, dosage->getDose(), 1500.0, DEFAULT_PRECISION, DEFAULT_PRECISION);
                }
            }
        }
    }

    // Delete all dynamically allocated objects
    delete component;
}
