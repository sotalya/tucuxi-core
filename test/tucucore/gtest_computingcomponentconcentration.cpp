//@@license@@

#include <memory>

#include <gtest/gtest.h>

#include "gtest_core.h"

#include "tucucommon/datetime.h"

#include "tucucore/computingcomponent.h"
#include "tucucore/computingservice/computingrequest.h"
#include "tucucore/computingservice/computingresponse.h"
#include "tucucore/computingservice/computingtrait.h"
#include "tucucore/drugtreatment/drugtreatment.h"

#include "drugmodels/buildconstantelimination.h"
#include "drugmodels/buildimatinib.h"

using namespace Tucuxi::Core;

using namespace std::chrono_literals;
using namespace date;

static std::unique_ptr<DrugTreatment> buildSimpleDrugTreatment(
        FormulationAndRoute _route, DateTime& _startTime, Duration _interval, Duration _treatmentDuration)
{
    auto drugTreatment = std::make_unique<DrugTreatment>();

    // List of time ranges that will be pushed into the history
    DosageTimeRangeList timeRangeList;

    // Create a time range starting at the beginning of june 2018, with no upper end (to test that the repetitions
    // are handled correctly)


    //const FormulationAndRoute route("formulation", AdministrationRoute::IntravenousBolus, AbsorptionModel::Intravascular);
    // Add a treatment intake every ten days in June
    // 200mg via a intravascular at 08h30, starting the 01.06
    LastingDose periodicDose(DoseValue(200.0), TucuUnit("mg"), _route, Duration(), _interval);
    DosageRepeat repeatedDose(periodicDose, static_cast<int>(_treatmentDuration / _interval));
    auto dosageTimeRange = std::make_unique<Tucuxi::Core::DosageTimeRange>(_startTime, repeatedDose);

    drugTreatment->getModifiableDosageHistory().addTimeRange(*dosageTimeRange);

    return drugTreatment;
}

TEST (Core_TestComputingComponentConcentration, Simple1){
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

    {
        RequestResponseId requestResponseId = "1";
        Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
        Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
        double nbPointsPerHour = 10.0;
        ComputingOption computingOption(
                PredictionParameterType::Population,
                CompartmentsOption::MainCompartment,
                RetrieveStatisticsOption::DoNotRetrieveStatistics,
                RetrieveParametersOption::DoNotRetrieveParameters,
                RetrieveCovariatesOption::DoNotRetrieveCovariates,
                ForceUgPerLiterOption::Force);
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
        ASSERT_EQ(resp->getCompartmentInfos()[0].getId(), "imatinib");
        ASSERT_EQ(
                resp->getCompartmentInfos()[0].getType(), CompartmentInfo::CompartmentType::ActiveMoietyAndAnalyte);
        std::vector<CycleData> data = resp->getData();
        ASSERT_EQ(data.size(), static_cast<size_t>(16));
        ASSERT_EQ(data[0].m_concentrations.size(), static_cast<size_t>(1));
        ASSERT_DOUBLE_EQ(data[0].m_concentrations[0][0], 0.0);
        DateTime startSept2018(
                date::year_month_day(date::year(2018), date::month(9), date::day(1)),
                Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));

        ASSERT_DOUBLE_EQ(
                data[0].m_start.toSeconds() + data[0].m_times[0][0] * 3600.0, startSept2018.toSeconds());
        ASSERT_DOUBLE_EQ(
                data[1].m_start.toSeconds() + data[1].m_times[0][0] * 3600.0,
                startSept2018.toSeconds() + 3600.0 * 6.0);
    }

    {
        // Ask for 15 intakes, without the first one.
        RequestResponseId requestResponseId = "1";
        Tucuxi::Common::DateTime start(2018_y / sep / 1, 14h + 0min);
        Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
        double nbPointsPerHour = 10.0;
        ComputingOption computingOption(PredictionParameterType::Population, CompartmentsOption::MainCompartment);
        std::unique_ptr<ComputingTraitConcentration> traits = std::make_unique<ComputingTraitConcentration>(
                requestResponseId, start, end, nbPointsPerHour, computingOption);

        ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(traits));

        std::unique_ptr<ComputingResponse> partialResponse = std::make_unique<ComputingResponse>(requestResponseId);

        ComputingStatus result;
        result = component->compute(request, partialResponse);

        ASSERT_EQ(result, ComputingStatus::Ok);

        const ComputedData* responseData = partialResponse->getData();
        ASSERT_TRUE(dynamic_cast<const SinglePredictionData*>(responseData) != nullptr);
        const SinglePredictionData* resp = dynamic_cast<const SinglePredictionData*>(responseData);

        ASSERT_EQ(resp->getCompartmentInfos().size(), static_cast<size_t>(1));
        ASSERT_EQ(resp->getCompartmentInfos()[0].getId(), "imatinib");
        ASSERT_EQ(
                resp->getCompartmentInfos()[0].getType(), CompartmentInfo::CompartmentType::ActiveMoietyAndAnalyte);

        std::vector<CycleData> data = resp->getData();
        ASSERT_EQ(data.size(), static_cast<size_t>(15));
        ASSERT_EQ(data[0].m_concentrations.size(), static_cast<size_t>(1));
        ASSERT_DOUBLE_EQ(data[0].m_concentrations[0][0], 0.0);
        DateTime startSept2018(
                date::year_month_day(date::year(2018), date::month(9), date::day(1)),
                Duration(std::chrono::hours(14), std::chrono::minutes(0), std::chrono::seconds(0)));

        ASSERT_DOUBLE_EQ(
                data[0].m_start.toSeconds() + data[0].m_times[0][0] * 3600.0, startSept2018.toSeconds());
        ASSERT_DOUBLE_EQ(
                data[1].m_start.toSeconds() + data[1].m_times[0][0] * 3600.0,
                startSept2018.toSeconds() + 3600.0 * 6.0);
    }

    // Delete all dynamically allocated objects
    delete component;
}

TEST (Core_TestComputingComponentConcentration, ImatinibSteadyState){
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
    DosageRepeat repeatedDose(periodicDose, 1000);
    auto jun2018 = std::make_unique<Tucuxi::Core::DosageTimeRange>(startJun2018, repeatedDose);


    drugTreatment->getModifiableDosageHistory().addTimeRange(*jun2018);


    RequestResponseId requestResponseId = "1";
    Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
    Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
    double nbPointsPerHour = 10.0;
    ComputingOption computingOption(PredictionParameterType::Population, CompartmentsOption::MainCompartment);
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

    // The response is what we excepted
    ASSERT_TRUE(resp != nullptr);

    ASSERT_EQ(resp->getCompartmentInfos().size(), static_cast<size_t>(1));
    ASSERT_EQ(resp->getCompartmentInfos()[0].getId(), "imatinib");
    ASSERT_EQ(
            resp->getCompartmentInfos()[0].getType(), CompartmentInfo::CompartmentType::ActiveMoietyAndAnalyte);

    std::vector<CycleData> data = resp->getData();

    // We asked for 4 cycles
    ASSERT_EQ(data.size(), static_cast<size_t>(4));

    // Only a single compartment
    ASSERT_EQ(data[0].m_concentrations.size(), static_cast<size_t>(1));

    // Here we check that the relative difference of starting concentration for each cycle is small compared
    // to its residual (less than 0.0001)
    ASSERT_PRED4(
            double_eq_rel_abs, data[0].m_concentrations[0][0], data[0].m_concentrations[0].back(), 0.0001, 0.0001);
    ASSERT_PRED4(
            double_eq_rel_abs, data[1].m_concentrations[0][0], data[1].m_concentrations[0].back(), 0.0001, 0.0001);
    ASSERT_PRED4(
            double_eq_rel_abs, data[2].m_concentrations[0][0], data[2].m_concentrations[0].back(), 0.0001, 0.0001);
    ASSERT_PRED4(
            double_eq_rel_abs, data[3].m_concentrations[0][0], data[3].m_concentrations[0].back(), 0.0001, 0.0001);

    // Here we check that the relative difference of starting concentration for different cycles is small (less than 0.0001)
    // Actually if the residual are correctly implemented these assertions are equivalent to the four previous ones
    ASSERT_PRED4(
            double_eq_rel_abs, data[0].m_concentrations[0][0], data[1].m_concentrations[0][0], 0.0001, 0.0001);
    ASSERT_PRED4(
            double_eq_rel_abs, data[0].m_concentrations[0][0], data[2].m_concentrations[0][0], 0.0001, 0.0001);
    ASSERT_PRED4(
            double_eq_rel_abs, data[0].m_concentrations[0][0], data[3].m_concentrations[0][0], 0.0001, 0.0001);

    // Delete all dynamically allocated objects
    delete component;
}

TEST (Core_TestComputingComponentConcentration, SampleBeforeTreatmentStart){
    IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

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

    const FormulationAndRoute route(
            Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);


    DateTime startTreatment(
            date::year_month_day(date::year(2018), date::month(9), date::day(1)),
            Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));
    Duration interval(std::chrono::hours(6));
    Duration treatmentDuration(std::chrono::hours(24 * 60));
    DateTime endTreatment = startTreatment + treatmentDuration;

    // Test of a posteriori concentration prediction with one unvalid sample too early in time
    auto drugTreatment = buildSimpleDrugTreatment(route, startTreatment, interval, treatmentDuration);
    // The sample is prior to the treatment start
    drugTreatment->addSample(std::make_unique<Sample>(
            startTreatment - Duration(std::chrono::hours(3)), AnalyteId("analyte"), 100.0, TucuUnit("mg/l")));

    RequestResponseId requestResponseId = "1";
    Tucuxi::Common::DateTime start = startTreatment;
    Tucuxi::Common::DateTime end = startTreatment + Duration(std::chrono::hours(48));
    PercentileRanks percentileRanks({5, 25, 50, 75, 95});
    double nbPointsPerHour = 10.0;
    ComputingOption computingOption(PredictionParameterType::Aposteriori, CompartmentsOption::MainCompartment);

    std::unique_ptr<ComputingTraitConcentration> traitsC = std::make_unique<ComputingTraitConcentration>(
            requestResponseId, start, end, nbPointsPerHour, computingOption);
    ComputingRequest requestC(requestResponseId, *drugModel, *drugTreatment, std::move(traitsC));

    std::unique_ptr<ComputingResponse> responseC = std::make_unique<ComputingResponse>(requestResponseId);

    ComputingStatus resultC;
    resultC = component->compute(requestC, responseC);

    ASSERT_EQ(resultC, ComputingStatus::SampleBeforeTreatmentStart);


    auto traitsM = std::make_unique<ComputingTraitAtMeasures>(requestResponseId, computingOption);
    ComputingRequest requestM(requestResponseId, *drugModel, *drugTreatment, std::move(traitsM));

    std::unique_ptr<ComputingResponse> responseM = std::make_unique<ComputingResponse>(requestResponseId);

    ComputingStatus resultM;
    resultM = component->compute(requestM, responseM);

    ASSERT_EQ(resultM, ComputingStatus::SampleBeforeTreatmentStart);

    delete component;
}
