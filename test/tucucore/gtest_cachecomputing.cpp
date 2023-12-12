//@@license@@

#include <memory>

#include <gtest/gtest.h>

#include "gtest_core.h"

#include "tucucommon/datetime.h"

#include "tucucore/cachecomputing.h"
#include "tucucore/computingcomponent.h"
#include "tucucore/computingservice/computingrequest.h"
#include "tucucore/computingservice/computingresponse.h"
#include "tucucore/computingservice/computingtrait.h"
#include "tucucore/drugmodel/drugmodel.h"
#include "tucucore/drugtreatment/drugtreatment.h"
#include "tucucore/montecarlopercentilecalculator.h"

#include "drugmodels/buildimatinib.h"

using namespace Tucuxi::Core;

using namespace std::chrono_literals;
using namespace date;

static std::unique_ptr<CacheComputing> m_cache;

static void compute(
        DrugTreatment* _drugTreatment,
        DrugModel* _drugModel,
        DateTime _start,
        DateTime _end,
        double _nbPointsPerHour,
        bool _isHitExpected)
{

    RequestResponseId requestResponseId = "1";
    PercentileRanks percentileRanks({5, 25, 50, 75, 95});
    ComputingOption computingOption(PredictionParameterType::Population, CompartmentsOption::MainCompartment);
    std::unique_ptr<ComputingTraitPercentiles> traits = std::make_unique<ComputingTraitPercentiles>(
            requestResponseId, _start, _end, percentileRanks, _nbPointsPerHour, computingOption);

    ComputingRequest request(requestResponseId, *_drugModel, *_drugTreatment, std::move(traits));


    std::unique_ptr<ComputingResponse> response2 = std::make_unique<ComputingResponse>(requestResponseId);

    ComputingStatus result;
    result = m_cache->compute(request, response2);

    ASSERT_EQ(result, ComputingStatus::Ok);

    ASSERT_EQ(m_cache->isLastCallaHit(), _isHitExpected);

    const ComputedData* responseData = response2->getData();
    ASSERT_TRUE(dynamic_cast<const PercentilesData*>(responseData) != nullptr);
    const PercentilesData* resp = dynamic_cast<const PercentilesData*>(responseData);

    TMP_UNUSED_PARAMETER(resp);
}

///
/// \brief Tests the cache with single intervals
///
/// This test verifies that if the cache contains an interval surrounding the asked interval, then
/// it will return the surrounding interval
///
TEST (Core_TestCacheComputing, ImatinibSplitInterval){
    // We reduce the number of patients to speed up the tests
    MonteCarloPercentileCalculatorBase::setStaticNumberPatients(10);

    IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

    ASSERT_TRUE(component != nullptr);

    m_cache = std::make_unique<CacheComputing>(component);

    BuildImatinib builder;
    auto drugModel = builder.buildDrugModel();
    ASSERT_TRUE(drugModel != nullptr);

    const FormulationAndRoute route(
            Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);

    DateTime startSept2018(
            date::year_month_day(date::year(2018), date::month(9), date::day(1)),
            Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));

    auto drugTreatment = buildDrugTreatment(route, startSept2018, DoseValue(200.0), TucuUnit("mg"), 6, 30);


    RequestResponseId requestResponseId = "1";
    Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
    Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
    PercentileRanks percentileRanks({5, 25, 50, 75, 95});
    double nbPointsPerHour = 10.0;
    ComputingOption computingOption(PredictionParameterType::Population, CompartmentsOption::MainCompartment);
    std::unique_ptr<ComputingTraitPercentiles> traits = std::make_unique<ComputingTraitPercentiles>(
            requestResponseId, start, end, percentileRanks, nbPointsPerHour, computingOption);

    ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(traits));

    compute(drugTreatment.get(), drugModel.get(), start, end, nbPointsPerHour, false);

    Tucuxi::Common::DateTime start2(2018_y / sep / 4, 8h + 0min);
    Tucuxi::Common::DateTime end2(2018_y / sep / 10, 8h + 0min);

    compute(drugTreatment.get(), drugModel.get(), start2, end2, nbPointsPerHour, false);

    Tucuxi::Common::DateTime start3(2018_y / sep / 3, 8h + 0min);
    Tucuxi::Common::DateTime end3(2018_y / sep / 7, 8h + 0min);

    // The points already exist, so they should be found in the cache
    compute(drugTreatment.get(), drugModel.get(), start3, end3, nbPointsPerHour, true);

    // Delete all dynamically allocated objects
    delete component;
    m_cache.reset();
}

TEST (Core_TestCacheComputing, ImatinibFullInterval){
    // We reduce the number of patients to speed up the tests
    MonteCarloPercentileCalculatorBase::setStaticNumberPatients(10);

    IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

    ASSERT_TRUE(component != nullptr);

    m_cache = std::make_unique<CacheComputing>(component);

    BuildImatinib builder;
    auto drugModel = builder.buildDrugModel();
    ASSERT_TRUE(drugModel != nullptr);

    const FormulationAndRoute route(
            Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);

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

    compute(drugTreatment.get(), drugModel.get(), start, end, nbPointsPerHour, false);
    compute(drugTreatment.get(), drugModel.get(), start, end, nbPointsPerHour, true);
    compute(drugTreatment.get(), drugModel.get(), start, end, nbPointsPerHour, true);
    m_cache->clear();
    compute(drugTreatment.get(), drugModel.get(), start, end, nbPointsPerHour, false);
    compute(drugTreatment.get(), drugModel.get(), start, end, nbPointsPerHour, true);
    compute(drugTreatment.get(), drugModel.get(), start, end, nbPointsPerHour, true);

    Tucuxi::Common::DateTime start2(2018_y / sep / 9, 8h + 0min);
    Tucuxi::Common::DateTime end2(2018_y / sep / 12, 8h + 0min);

    compute(drugTreatment.get(), drugModel.get(), start2, end2, nbPointsPerHour, false);
    compute(drugTreatment.get(), drugModel.get(), start2, end2, nbPointsPerHour, true);
    compute(drugTreatment.get(), drugModel.get(), start2, end2, nbPointsPerHour / 1.1, true);

    compute(drugTreatment.get(), drugModel.get(), start, end, nbPointsPerHour, true);

    compute(drugTreatment.get(), drugModel.get(), start2, end2, nbPointsPerHour * 1.1, false);
    compute(drugTreatment.get(), drugModel.get(), start2, end2, nbPointsPerHour * 1.1, true);

    // Delete all dynamically allocated objects
    delete component;
    m_cache.reset();
}
