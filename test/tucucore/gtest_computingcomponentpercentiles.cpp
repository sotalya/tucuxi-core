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
#include "tucucore/definitions.h"
#include "tucucore/drugtreatment/drugtreatment.h"
#include "tucucore/montecarlopercentilecalculator.h"

#include "drugmodels/buildconstantelimination.h"
#include "drugmodels/buildimatinib.h"
#include "gtest_core.h"

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

TEST(Core_TestComputingComponentPercentiles, Imatinib1)
{
    // We reduce the number of patients to speed up the tests
    MonteCarloPercentileCalculatorBase::setStaticNumberPatients(1000);

    IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

    ASSERT_TRUE(component != nullptr);

    BuildImatinib builder;
    auto drugModel = builder.buildDrugModel();
    ASSERT_TRUE(drugModel != nullptr);

    const FormulationAndRoute route(Formulation::OralSolution, AdministrationRoute::Oral);

    DateTime startSept2018(
            date::year_month_day(date::year(2018), date::month(9), date::day(1)),
            Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));

    auto drugTreatment = buildDrugTreatment(route, startSept2018);

    RequestResponseId requestResponseId = "1";
    Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
    Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
    PercentileRanks percentileRanks({5, 25, 50, 75, 95});
    double nbPointsPerHour = 10.0;
    ComputingOption computingOption(PredictionParameterType::Population, CompartmentsOption::MainCompartment);
    std::unique_ptr<ComputingTraitPercentiles> traits = std::make_unique<ComputingTraitPercentiles>(
            requestResponseId, start, end, percentileRanks, nbPointsPerHour, computingOption);

    ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(traits));

    std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

    ComputingStatus result;
    result = component->compute(request, response);

    ASSERT_EQ(result, ComputingStatus::Ok);

    const ComputedData* responseData = response->getData();
    ASSERT_TRUE(dynamic_cast<const PercentilesData*>(responseData) != nullptr);
    const PercentilesData* resp = dynamic_cast<const PercentilesData*>(responseData);

    ASSERT_EQ(resp->getCompartmentInfos().size(), static_cast<size_t>(1));
    ASSERT_EQ(resp->getCompartmentInfos()[0].getId(), "imatinib");
    ASSERT_EQ(resp->getCompartmentInfos()[0].getType(), CompartmentInfo::CompartmentType::ActiveMoietyAndAnalyte);

    /*
            std::vector<CycleData> data = resp->getData();
            ASSERT_EQ(data.size(), static_cast<size_t>(16));
            ASSERT_EQ(data[0].m_concentrations.size(), static_cast<size_t>(1));
            ASSERT_DOUBLE_EQ(data[0].m_concentrations[0][0], 0.0);
            DateTime startSept2018(date::year_month_day(date::year(2018), date::month(9), date::day(1)),
                                   Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));

            ASSERT_DOUBLE_EQ(data[0].m_start.toSeconds() + data[0].m_times[0][0] * 3600.0, startSept2018.toSeconds());
            ASSERT_DOUBLE_EQ(data[1].m_start.toSeconds() + data[1].m_times[0][0] * 3600.0, startSept2018.toSeconds() + 3600.0 * 6.0);
            */

    /*
        {
            // Ask for 15 intakes, without the first one.
            RequestResponseId requestResponseId = "1";
            Tucuxi::Common::DateTime start(2018_y / sep / 1, 14h + 0min);
            Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
        double nbPointsPerHour = 10.0;
            ComputingOption computingOption(PredictionParameterType::Population, CompartmentsOption::MainCompartment);
            std::unique_ptr<ComputingTraitConcentration> traits =
                    std::make_unique<ComputingTraitConcentration>(
                        requestResponseId, start, end, nbPointsPerHour, computingOption);

            std::unique_ptr<ComputingTraits> computingTraits = std::make_unique<ComputingTraits>();
            computingTraits->addTrait(std::move(traits));

            ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(computingTraits));

            std::unique_ptr<ComputingResponse> partialResponse = std::make_unique<ComputingResponse>(requestResponseId);

            ComputingResult result;
            result = component->compute(request, partialResponse);

            ASSERT_EQ(result, ComputingResult::Success);

            const std::vector<std::unique_ptr<SingleComputingResponse> > &responses = partialResponse->getResponses();
            for(std::size_t i = 0; i < responses.size(); i++) {
                ASSERT_TRUE(dynamic_cast<SinglePredictionResponse*>(responses[i].get()) != nullptr);
                const SinglePredictionResponse *resp = dynamic_cast<SinglePredictionResponse*>(responses[i].get());
                std::vector<CycleData> data = resp->getData();
                ASSERT_EQ(data.size(), static_cast<size_t>(15));
                ASSERT_EQ(data[0].m_concentrations.size(), static_cast<size_t>(1));
                ASSERT_NE(data[0].m_concentrations[0][0], 0.0);
                DateTime startSept2018(date::year_month_day(date::year(2018), date::month(9), date::day(1)),
                                       Duration(std::chrono::hours(14), std::chrono::minutes(0), std::chrono::seconds(0)));

                ASSERT_DOUBLE_EQ(data[0].m_start.toSeconds() + data[0].m_times[0][0] * 3600.0, startSept2018.toSeconds());
                ASSERT_DOUBLE_EQ(data[1].m_start.toSeconds() + data[1].m_times[0][0] * 3600.0, startSept2018.toSeconds() + 3600.0 * 6.0);
            }
        }
*/
    // Delete all dynamically allocated objects
    delete component;
}

TEST(Core_TestComputingComponentPercentiles, ImatinibSteadyState)
{
    // We reduce the number of patients to speed up the tests
    MonteCarloPercentileCalculatorBase::setStaticNumberPatients(1000);

    IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

    ASSERT_TRUE(component != nullptr);

    BuildImatinib builder;
    auto drugModel = builder.buildDrugModel();
    ASSERT_TRUE(drugModel != nullptr);

    const FormulationAndRoute route(Formulation::OralSolution, AdministrationRoute::Oral);

    DateTime startSept2018(
            date::year_month_day(date::year(2018), date::month(9), date::day(1)),
            Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));

    auto drugTreatment = buildDrugTreatment(route, startSept2018, DoseValue(200), TucuUnit("mg"), 24, 90);

    RequestResponseId requestResponseId = "1";
    Tucuxi::Common::DateTime start(2018_y / date::literals::oct / 1, 8h + 0min);
    Tucuxi::Common::DateTime end(2018_y / date::literals::oct / 5, 8h + 0min);
    PercentileRanks percentileRanks({5, 25, 50, 75, 95});
    double nbPointsPerHour = 10.0;
    ComputingOption computingOption(PredictionParameterType::Population, CompartmentsOption::MainCompartment);
    std::unique_ptr<ComputingTraitPercentiles> traits = std::make_unique<ComputingTraitPercentiles>(
            requestResponseId, start, end, percentileRanks, nbPointsPerHour, computingOption);

    ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(traits));

    std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

    ComputingStatus result;
    result = component->compute(request, response);

    ASSERT_EQ(result, ComputingStatus::Ok);

    const ComputedData* responseData = response->getData();
    ASSERT_TRUE(dynamic_cast<const PercentilesData*>(responseData) != nullptr);
    const PercentilesData* resp = dynamic_cast<const PercentilesData*>(responseData);

    ASSERT_EQ(resp->getCompartmentInfos().size(), static_cast<size_t>(1));
    ASSERT_EQ(resp->getCompartmentInfos()[0].getId(), "imatinib");
    ASSERT_EQ(resp->getCompartmentInfos()[0].getType(), CompartmentInfo::CompartmentType::ActiveMoietyAndAnalyte);
    /*
            std::vector<CycleData> data = resp->getData();
            ASSERT_EQ(data.size(), static_cast<size_t>(16));
            ASSERT_EQ(data[0].m_concentrations.size(), static_cast<size_t>(1));
            ASSERT_DOUBLE_EQ(data[0].m_concentrations[0][0], 0.0);
            DateTime startSept2018(date::year_month_day(date::year(2018), date::month(9), date::day(1)),
                                   Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));

            ASSERT_DOUBLE_EQ(data[0].m_start.toSeconds() + data[0].m_times[0][0] * 3600.0, startSept2018.toSeconds());
            ASSERT_DOUBLE_EQ(data[1].m_start.toSeconds() + data[1].m_times[0][0] * 3600.0, startSept2018.toSeconds() + 3600.0 * 6.0);
            */

    /*
        {
            // Ask for 15 intakes, without the first one.
            RequestResponseId requestResponseId = "1";
            Tucuxi::Common::DateTime start(2018_y / sep / 1, 14h + 0min);
            Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
        double nbPointsPerHour = 10.0;
            ComputingOption computingOption(PredictionParameterType::Population, CompartmentsOption::MainCompartment);
            std::unique_ptr<ComputingTraitConcentration> traits =
                    std::make_unique<ComputingTraitConcentration>(
                        requestResponseId, start, end, nbPointsPerHour, computingOption);

            std::unique_ptr<ComputingTraits> computingTraits = std::make_unique<ComputingTraits>();
            computingTraits->addTrait(std::move(traits));

            ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(computingTraits));

            std::unique_ptr<ComputingResponse> partialResponse = std::make_unique<ComputingResponse>(requestResponseId);

            ComputingResult result;
            result = component->compute(request, partialResponse);

            ASSERT_EQ(result, ComputingResult::Success);

            const std::vector<std::unique_ptr<SingleComputingResponse> > &responses = partialResponse->getResponses();
            for(std::size_t i = 0; i < responses.size(); i++) {
                ASSERT_TRUE(dynamic_cast<SinglePredictionResponse*>(responses[i].get()) != nullptr);
                const SinglePredictionResponse *resp = dynamic_cast<SinglePredictionResponse*>(responses[i].get());
                std::vector<CycleData> data = resp->getData();
                ASSERT_EQ(data.size(), static_cast<size_t>(15));
                ASSERT_EQ(data[0].m_concentrations.size(), static_cast<size_t>(1));
                ASSERT_NE(data[0].m_concentrations[0][0], 0.0);
                DateTime startSept2018(date::year_month_day(date::year(2018), date::month(9), date::day(1)),
                                       Duration(std::chrono::hours(14), std::chrono::minutes(0), std::chrono::seconds(0)));

                ASSERT_DOUBLE_EQ(data[0].m_start.toSeconds() + data[0].m_times[0][0] * 3600.0, startSept2018.toSeconds());
                ASSERT_DOUBLE_EQ(data[1].m_start.toSeconds() + data[1].m_times[0][0] * 3600.0, startSept2018.toSeconds() + 3600.0 * 6.0);
            }
        }
*/
    // Delete all dynamically allocated objects
    delete component;
}

TEST(Core_TestComputingComponentPercentiles, AposterioriPercentiles)
{
    // We reduce the number of patients to speed up the tests
    MonteCarloPercentileCalculatorBase::setStaticNumberPatients(1000);

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

    const FormulationAndRoute route(Formulation::OralSolution, AdministrationRoute::Oral);


    DateTime startTreatment(
            date::year_month_day(date::year(2018), date::month(9), date::day(1)),
            Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));
    Duration interval(std::chrono::hours(6));
    Duration treatmentDuration(std::chrono::hours(24 * 60));
    DateTime endTreatment = startTreatment + treatmentDuration;

    {
        // Test of a posteriori percentiles with no samples
        auto drugTreatment = buildSimpleDrugTreatment(route, startTreatment, interval, treatmentDuration);

        RequestResponseId requestResponseId = "1";
        Tucuxi::Common::DateTime start = startTreatment;
        Tucuxi::Common::DateTime end = startTreatment + Duration(std::chrono::hours(48));
        PercentileRanks percentileRanks({5, 25, 50, 75, 95});
        double nbPointsPerHour = 10.0;
        ComputingOption computingOption(PredictionParameterType::Aposteriori, CompartmentsOption::MainCompartment);
        std::unique_ptr<ComputingTraitPercentiles> traits = std::make_unique<ComputingTraitPercentiles>(
                requestResponseId, start, end, percentileRanks, nbPointsPerHour, computingOption);

        ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(traits));

        std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

        ComputingStatus result;
        result = component->compute(request, response);

        ASSERT_EQ(result, ComputingStatus::AposterioriPercentilesNoSamplesError);

        ASSERT_TRUE(response->getData() == nullptr);
    }

    {
        // Test of a posteriori percentiles with one valid sample
        auto drugTreatment = buildSimpleDrugTreatment(route, startTreatment, interval, treatmentDuration);
        drugTreatment->addSample(std::make_unique<Sample>(
                startTreatment + Duration(std::chrono::hours(3)), AnalyteId("analyte"), 100.0, TucuUnit("mg/l")));

        RequestResponseId requestResponseId = "1";
        Tucuxi::Common::DateTime start = startTreatment;
        Tucuxi::Common::DateTime end = startTreatment + Duration(std::chrono::hours(48));
        PercentileRanks percentileRanks({5, 25, 50, 75, 95});
        double nbPointsPerHour = 10.0;
        ComputingOption computingOption(PredictionParameterType::Aposteriori, CompartmentsOption::MainCompartment);
        std::unique_ptr<ComputingTraitPercentiles> traits = std::make_unique<ComputingTraitPercentiles>(
                requestResponseId, start, end, percentileRanks, nbPointsPerHour, computingOption);

        ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(traits));

        std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

        ComputingStatus result;
        result = component->compute(request, response);

        ASSERT_EQ(result, ComputingStatus::Ok);

        const ComputedData* responseData = response->getData();
        ASSERT_TRUE(dynamic_cast<const PercentilesData*>(responseData) != nullptr);
        const PercentilesData* resp = dynamic_cast<const PercentilesData*>(responseData);

        ASSERT_EQ(resp->getCompartmentInfos().size(), static_cast<size_t>(1));
        ASSERT_EQ(resp->getCompartmentInfos()[0].getId(), "analyte");
        ASSERT_EQ(resp->getCompartmentInfos()[0].getType(), CompartmentInfo::CompartmentType::ActiveMoietyAndAnalyte);
    }

    {
        // Test of a posteriori percentiles with one valid sample just before the end of treatment
        auto drugTreatment = buildSimpleDrugTreatment(route, startTreatment, interval, treatmentDuration);
        drugTreatment->addSample(std::make_unique<Sample>(
                endTreatment - Duration(std::chrono::hours(3)), AnalyteId("analyte"), 100.0, TucuUnit("mg/l")));

        RequestResponseId requestResponseId = "1";
        Tucuxi::Common::DateTime start = startTreatment;
        Tucuxi::Common::DateTime end = startTreatment + Duration(std::chrono::hours(48));
        PercentileRanks percentileRanks({5, 25, 50, 75, 95});
        double nbPointsPerHour = 10.0;
        ComputingOption computingOption(PredictionParameterType::Aposteriori, CompartmentsOption::MainCompartment);
        std::unique_ptr<ComputingTraitPercentiles> traits = std::make_unique<ComputingTraitPercentiles>(
                requestResponseId, start, end, percentileRanks, nbPointsPerHour, computingOption);

        ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(traits));

        std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

        ComputingStatus result;
        result = component->compute(request, response);

        ASSERT_EQ(result, ComputingStatus::Ok);

        const ComputedData* responseData = response->getData();
        ASSERT_TRUE(dynamic_cast<const PercentilesData*>(responseData) != nullptr);
        const PercentilesData* resp = dynamic_cast<const PercentilesData*>(responseData);
        ASSERT_EQ(resp->getCompartmentInfos().size(), static_cast<size_t>(1));
        ASSERT_EQ(resp->getCompartmentInfos()[0].getId(), "analyte");
        ASSERT_EQ(resp->getCompartmentInfos()[0].getType(), CompartmentInfo::CompartmentType::ActiveMoietyAndAnalyte);
    }

    if (false) {
        // Test of a posteriori percentiles with one valid sample just after the end of treatment
        auto drugTreatment = buildSimpleDrugTreatment(route, startTreatment, interval, treatmentDuration);
        drugTreatment->addSample(std::make_unique<Sample>(
                endTreatment + Duration(std::chrono::hours(3)), AnalyteId("analyte"), 100.0, TucuUnit("mg/l")));

        RequestResponseId requestResponseId = "1";
        Tucuxi::Common::DateTime start = startTreatment;
        Tucuxi::Common::DateTime end = startTreatment + Duration(std::chrono::hours(48));
        PercentileRanks percentileRanks({5, 25, 50, 75, 95});
        double nbPointsPerHour = 10.0;
        ComputingOption computingOption(PredictionParameterType::Aposteriori, CompartmentsOption::MainCompartment);
        std::unique_ptr<ComputingTraitPercentiles> traits = std::make_unique<ComputingTraitPercentiles>(
                requestResponseId, start, end, percentileRanks, nbPointsPerHour, computingOption);

        ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(traits));

        std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

        ComputingStatus result;
        result = component->compute(request, response);

        ASSERT_EQ(result, ComputingStatus::Ok);

        const ComputedData* responseData = response->getData();
        ASSERT_TRUE(dynamic_cast<const PercentilesData*>(responseData) != nullptr);
        const PercentilesData* resp = dynamic_cast<const PercentilesData*>(responseData);
        ASSERT_EQ(resp->getCompartmentInfos().size(), static_cast<size_t>(1));
        ASSERT_EQ(resp->getCompartmentInfos()[0].getId(), "analyte");
        ASSERT_EQ(resp->getCompartmentInfos()[0].getType(), CompartmentInfo::CompartmentType::ActiveMoietyAndAnalyte);
    }

    {
        // Test of a posteriori percentiles with one valid sample but a prediction start not being the treatment start
        auto drugTreatment = buildSimpleDrugTreatment(route, startTreatment, interval, treatmentDuration);
        drugTreatment->addSample(std::make_unique<Sample>(
                startTreatment + Duration(std::chrono::hours(3)), AnalyteId("analyte"), 100.0, TucuUnit("mg/l")));

        RequestResponseId requestResponseId = "1";
        Tucuxi::Common::DateTime start = startTreatment + Duration(std::chrono::hours(48));
        Tucuxi::Common::DateTime end = startTreatment + Duration(std::chrono::hours(48));
        PercentileRanks percentileRanks({5, 25, 50, 75, 95});
        double nbPointsPerHour = 10.0;
        ComputingOption computingOption(PredictionParameterType::Aposteriori, CompartmentsOption::MainCompartment);
        std::unique_ptr<ComputingTraitPercentiles> traits = std::make_unique<ComputingTraitPercentiles>(
                requestResponseId, start, end, percentileRanks, nbPointsPerHour, computingOption);

        ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(traits));

        std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

        ComputingStatus result;
        result = component->compute(request, response);

        ASSERT_EQ(result, ComputingStatus::Ok);

        const ComputedData* responseData = response->getData();
        ASSERT_TRUE(dynamic_cast<const PercentilesData*>(responseData) != nullptr);
        const PercentilesData* resp = dynamic_cast<const PercentilesData*>(responseData);
        ASSERT_EQ(resp->getCompartmentInfos().size(), static_cast<size_t>(1));
        ASSERT_EQ(resp->getCompartmentInfos()[0].getId(), "analyte");
        ASSERT_EQ(resp->getCompartmentInfos()[0].getType(), CompartmentInfo::CompartmentType::ActiveMoietyAndAnalyte);
    }



    {
        // Test of a posteriori percentiles with one unvalid sample too far away in time
        auto drugTreatment = buildSimpleDrugTreatment(route, startTreatment, interval, treatmentDuration);
        // The sample is prior to the treatment start
        drugTreatment->addSample(std::make_unique<Sample>(
                endTreatment + Duration(std::chrono::hours(3000)), AnalyteId("analyte"), 100.0, TucuUnit("mg/l")));

        RequestResponseId requestResponseId = "1";
        Tucuxi::Common::DateTime start = startTreatment;
        Tucuxi::Common::DateTime end = startTreatment + Duration(std::chrono::hours(48));
        PercentileRanks percentileRanks({5, 25, 50, 75, 95});
        double nbPointsPerHour = 10.0;
        ComputingOption computingOption(PredictionParameterType::Aposteriori, CompartmentsOption::MainCompartment);
        std::unique_ptr<ComputingTraitPercentiles> traits = std::make_unique<ComputingTraitPercentiles>(
                requestResponseId, start, end, percentileRanks, nbPointsPerHour, computingOption);

        ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(traits));

        std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

        ComputingStatus result;
        result = component->compute(request, response);

        ASSERT_EQ(result, ComputingStatus::AposterioriPercentilesOutOfScopeSamplesError);

        ASSERT_TRUE(response->getData() == nullptr);
    }

    delete component;
}
TEST(Core_TestComputingComponentPercentiles, InvalidRanks)
{
    // We reduce the number of patients to speed up the tests
    MonteCarloPercentileCalculatorBase::setStaticNumberPatients(1000);

    IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

    ASSERT_TRUE(component != nullptr);

    BuildImatinib builder;
    auto drugModel = builder.buildDrugModel();
    ASSERT_TRUE(drugModel != nullptr);

    const FormulationAndRoute route(Formulation::OralSolution, AdministrationRoute::Oral);

    DateTime startSept2018(
            date::year_month_day(date::year(2018), date::month(9), date::day(1)),
            Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));

    auto drugTreatment = buildDrugTreatment(route, startSept2018);

    RequestResponseId requestResponseId = "1";
    Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
    Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
    double nbPointsPerHour = 10.0;
    ComputingOption computingOption(PredictionParameterType::Population, CompartmentsOption::MainCompartment);

    {
        PercentileRanks percentileRanks({PERCENTILE_RANK_MAX, PERCENTILE_RANK_MIN});
        std::unique_ptr<ComputingTraitPercentiles> traits = std::make_unique<ComputingTraitPercentiles>(
                requestResponseId, start, end, percentileRanks, nbPointsPerHour, computingOption);
        ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(traits));
        std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);
        ComputingStatus result = component->compute(request, response);

        ASSERT_NE(result, ComputingStatus::OutOfBoundsPercentileRank);
    }
    {
        PercentileRanks percentileRanks({PERCENTILE_RANK_MAX, PERCENTILE_RANK_MIN - 1});
        std::unique_ptr<ComputingTraitPercentiles> traits = std::make_unique<ComputingTraitPercentiles>(
                requestResponseId, start, end, percentileRanks, nbPointsPerHour, computingOption);

        ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(traits));

        std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

        ComputingStatus result = component->compute(request, response);
        ASSERT_EQ(result, ComputingStatus::OutOfBoundsPercentileRank);
    }
    {
        PercentileRanks percentileRanks({PERCENTILE_RANK_MAX + 1, PERCENTILE_RANK_MIN});
        std::unique_ptr<ComputingTraitPercentiles> traits = std::make_unique<ComputingTraitPercentiles>(
                requestResponseId, start, end, percentileRanks, nbPointsPerHour, computingOption);

        ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(traits));

        std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);
        ComputingStatus result = component->compute(request, response);
        ASSERT_EQ(result, ComputingStatus::OutOfBoundsPercentileRank);
    }
}
