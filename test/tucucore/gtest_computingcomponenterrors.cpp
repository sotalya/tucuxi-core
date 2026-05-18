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
#include <stdexcept>

#include <gtest/gtest.h>

#include "tucucommon/datetime.h"

#include "tucucore/computingcomponent.h"
#include "tucucore/computingservice/computingrequest.h"
#include "tucucore/computingservice/computingresponse.h"
#include "tucucore/computingservice/computingtrait.h"
#include "tucucore/dosage.h"
#include "tucucore/drugtreatment/drugtreatment.h"
#include "tucucore/pkmodel.h"

#include "drugmodels/buildimatinib.h"
#include "mocklogger.h"

using namespace Tucuxi::Core;

using namespace std::chrono_literals;
using namespace date;

namespace Tucuxi {
namespace Core {

///
/// \brief Helper class granting test access to ComputingComponent's private constructor
/// and private compute() overloads.
///
/// This allows tests to create an uninitialized ComputingComponent and to call private
/// compute() overloads directly with null traits, in order to cover the early-exit
/// null-check branches that cannot be reached through the normal dispatch path.
///
class ComputingComponentTestHelper
{
public:
    static ComputingComponent* createUninitialized()
    {
        return new ComputingComponent();
    }

    ///
    /// \brief Calls compute(TraitConcentration) with a null trait pointer.
    ///
    /// Covers the nullptr guard at the top of compute(ComputingTraitConcentration*,...).
    ///
    static ComputingStatus computeNullConcentrationTraits(
            ComputingComponent* _cc, const ComputingRequest& _request, std::unique_ptr<ComputingResponse>& _response)
    {
        return _cc->compute(static_cast<const ComputingTraitConcentration*>(nullptr), _request, _response);
    }

    ///
    /// \brief Calls computePercentilesSimple() with a null trait pointer.
    ///
    /// Covers the nullptr guard at the top of computePercentilesSimple().
    ///
    static ComputingStatus computeNullPercentilesSimpleTraits(
            ComputingComponent* _cc, const ComputingRequest& _request, std::unique_ptr<ComputingResponse>& _response)
    {
        return _cc->computePercentilesSimple(
                static_cast<const ComputingTraitPercentiles*>(nullptr), _request, _response);
    }

    ///
    /// \brief Calls compute(TraitAdjustment) with a null trait pointer.
    ///
    /// Covers the nullptr guard at the top of compute(ComputingTraitAdjustment*,...).
    ///
    static ComputingStatus computeNullAdjustmentTraits(
            ComputingComponent* _cc, const ComputingRequest& _request, std::unique_ptr<ComputingResponse>& _response)
    {
        return _cc->compute(static_cast<const ComputingTraitAdjustment*>(nullptr), _request, _response);
    }

    ///
    /// \brief Calls compute(TraitSinglePoints) with a null trait pointer.
    ///
    /// Covers the nullptr guard at the top of compute(ComputingTraitSinglePoints*,...).
    ///
    static ComputingStatus computeNullSinglePointsTraits(
            ComputingComponent* _cc, const ComputingRequest& _request, std::unique_ptr<ComputingResponse>& _response)
    {
        return _cc->compute(static_cast<const ComputingTraitSinglePoints*>(nullptr), _request, _response);
    }
};

///
/// \brief A ComputingTrait that unconditionally throws, used to exercise the
/// TUCU_CATCH(...) block in ComputingComponent::compute().
///
class ThrowingComputingTrait : public ComputingTrait
{
public:
    ThrowingComputingTrait(const RequestResponseId& _id, const ComputingOption& _computingOption)
        : ComputingTrait(_id, _computingOption)
    {
    }

private:
    ComputingStatus compute(
            ComputingComponent& /*_computingComponent*/,
            const ComputingRequest& /*_request*/,
            std::unique_ptr<ComputingResponse>& /*_response*/) const override
    {
        throw std::runtime_error("Deliberate exception for coverage testing");
    }

    ComputingStatus compute(
            MultiComputingComponent& /*_computingComponent*/,
            const ComputingRequest& /*_request*/,
            std::unique_ptr<ComputingResponse>& /*_response*/) const override
    {
        throw std::runtime_error("Deliberate exception for coverage testing");
    }
};

} // namespace Core
} // namespace Tucuxi


/// \brief Builds an imatinib treatment with four daily oral doses starting 2018-09-01.
///
/// Used by tests that need a real drug treatment to reach computation branches
/// that come after the early-exit guards (e.g. statistics, parameters, covariates).
///
static std::unique_ptr<DrugTreatment> buildImatinibTreatment()
{
    const FormulationAndRoute route(Formulation::OralSolution, AdministrationRoute::Oral);
    const DateTime startTime(
            date::year_month_day(date::year(2018), date::month(9), date::day(1)), Duration(std::chrono::hours(8)));
    auto drugTreatment = std::make_unique<DrugTreatment>();
    LastingDose dose(DoseValue{200.0}, TucuUnit("mg"), route, Duration(), Duration(std::chrono::hours(24)));
    DosageRepeat repeatedDose(dose, 4);
    auto timeRange = std::make_unique<DosageTimeRange>(startTime, repeatedDose);
    drugTreatment->getModifiableDosageHistory().addTimeRange(*timeRange);
    return drugTreatment;
}


/// \brief Builds a standard ComputingRequest and calls compute(), returning the status.
///
/// All error-path tests share the same request parameters (fixed dates, population prediction,
/// imatinib drug model). Only the component state and the drug treatment vary per test.
static ComputingStatus computeWithStandardRequest(
        IComputingService* _component, const DrugModel& _drugModel, DrugTreatment& _drugTreatment)
{
    const RequestResponseId requestResponseId = "1";
    const Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
    const Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
    ComputingOption computingOption(PredictionParameterType::Population, CompartmentsOption::MainCompartment);
    auto traits = std::make_unique<ComputingTraitConcentration>(requestResponseId, start, end, 10.0, computingOption);

    ComputingRequest request(requestResponseId, _drugModel, _drugTreatment, std::move(traits));
    std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

    return _component->compute(request, response);
}


/// \brief Test that compute() returns ComputingComponentNotInitialized when the component
/// has not been initialized (m_utils == nullptr), covering the early-exit branch.
TEST(Core_TestComputingComponentErrors, NotInitialized)
{
    // Install MockLogger before creating the component so its m_logger picks it up
    Tucuxi::Common::ScopedMockLogger mockLogger;

    // Create a ComputingComponent without calling initialize(), so m_utils remains nullptr
    ComputingComponent* rawComponent = ComputingComponentTestHelper::createUninitialized();
    IComputingService* component = dynamic_cast<IComputingService*>(rawComponent);
    ASSERT_NE(component, nullptr);

    BuildImatinib builder;
    auto drugModel = builder.buildDrugModel();
    ASSERT_NE(drugModel, nullptr);

    auto drugTreatment = std::make_unique<DrugTreatment>();

    ASSERT_EQ(
            computeWithStandardRequest(component, *drugModel, *drugTreatment),
            ComputingStatus::ComputingComponentNotInitialized);
    EXPECT_TRUE(mockLogger.hasEntry(Tucuxi::Common::LogLevel::Error, "has not been initialized"));

    delete component;
}

/// \brief Test that compute() returns NoPkModels when the PkModel collection is null
/// (m_utils->m_models == nullptr).
TEST(Core_TestComputingComponentErrors, NullPkModelCollection)
{
    Tucuxi::Common::ScopedMockLogger mockLogger;

    auto* rawComponent = dynamic_cast<ComputingComponent*>(ComputingComponent::createComponent());
    IComputingService* component = dynamic_cast<IComputingService*>(rawComponent);
    ASSERT_NE(component, nullptr);

    // Set the models collection to nullptr so the second guard triggers
    rawComponent->setPkModelCollection(nullptr);

    BuildImatinib builder;
    auto drugModel = builder.buildDrugModel();
    ASSERT_NE(drugModel, nullptr);

    auto drugTreatment = std::make_unique<DrugTreatment>();

    ASSERT_EQ(computeWithStandardRequest(component, *drugModel, *drugTreatment), ComputingStatus::NoPkModels);
    EXPECT_TRUE(mockLogger.hasEntry(Tucuxi::Common::LogLevel::Error, "No Pk Model loaded"));

    delete component;
}

/// \brief Test that compute() returns NoPkModels when the PkModel collection is empty
/// (getPkModelList().empty()).
TEST(Core_TestComputingComponentErrors, EmptyPkModelCollection)
{
    Tucuxi::Common::ScopedMockLogger mockLogger;

    auto* rawComponent = dynamic_cast<ComputingComponent*>(ComputingComponent::createComponent());
    IComputingService* component = dynamic_cast<IComputingService*>(rawComponent);
    ASSERT_NE(component, nullptr);

    // Replace the default populated collection with an empty one
    rawComponent->setPkModelCollection(std::make_shared<PkModelCollection>());

    BuildImatinib builder;
    auto drugModel = builder.buildDrugModel();
    ASSERT_NE(drugModel, nullptr);

    auto drugTreatment = std::make_unique<DrugTreatment>();

    ASSERT_EQ(computeWithStandardRequest(component, *drugModel, *drugTreatment), ComputingStatus::NoPkModels);
    EXPECT_TRUE(mockLogger.hasEntry(Tucuxi::Common::LogLevel::Error, "No Pk Model loaded"));

    delete component;
}

/// \brief Test that compute() returns IncompatibleTreatmentModel when the drug treatment uses
/// a formulation/route not supported by the drug model.
TEST(Core_TestComputingComponentErrors, IncompatibleTreatmentModel)
{
    IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());
    ASSERT_NE(component, nullptr);

    BuildImatinib builder;
    auto drugModel = builder.buildDrugModel();
    ASSERT_NE(drugModel, nullptr);

    // Build a treatment using a route not defined in the imatinib drug model
    const FormulationAndRoute incompatibleRoute(Formulation::ParenteralSolution, AdministrationRoute::Nasal);
    const DateTime startTime(
            date::year_month_day(date::year(2018), date::month(9), date::day(1)),
            Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));
    auto drugTreatment = std::make_unique<DrugTreatment>();
    LastingDose dose(DoseValue{200.0}, TucuUnit("mg"), incompatibleRoute, Duration(), Duration(std::chrono::hours(24)));
    DosageRepeat repeatedDose(dose, 4);
    auto dosageTimeRange = std::make_unique<DosageTimeRange>(startTime, repeatedDose);
    drugTreatment->getModifiableDosageHistory().addTimeRange(*dosageTimeRange);

    ASSERT_EQ(
            computeWithStandardRequest(component, *drugModel, *drugTreatment),
            ComputingStatus::IncompatibleTreatmentModel);

    delete component;
}

/// \brief Test that compute() returns ComputingComponentExceptionError when a trait throws,
/// covering the TUCU_CATCH(...) block in ComputingComponent::compute().
TEST(Core_TestComputingComponentErrors, ExceptionInTrait)
{
    IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());
    ASSERT_NE(component, nullptr);

    BuildImatinib builder;
    auto drugModel = builder.buildDrugModel();
    ASSERT_NE(drugModel, nullptr);

    // An empty treatment passes the compatibility check (no routes to validate)
    auto drugTreatment = std::make_unique<DrugTreatment>();

    const RequestResponseId requestResponseId = "1";
    ComputingOption computingOption(PredictionParameterType::Population, CompartmentsOption::MainCompartment);
    auto traits = std::make_unique<ThrowingComputingTrait>(requestResponseId, computingOption);

    ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(traits));
    std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

    ComputingStatus result = component->compute(request, response);

    ASSERT_EQ(result, ComputingStatus::ComputingComponentExceptionError);

    delete component;
}

/// \brief Test that compute(TraitConcentration) returns NoComputingTraits when
/// called directly with a null trait pointer, covering the nullptr guard.
TEST(Core_TestComputingComponentErrors, NullTraitsConcentration)
{
    Tucuxi::Common::ScopedMockLogger mockLogger;

    auto* rawComponent = dynamic_cast<ComputingComponent*>(ComputingComponent::createComponent());
    IComputingService* component = dynamic_cast<IComputingService*>(rawComponent);
    ASSERT_NE(component, nullptr);

    BuildImatinib builder;
    auto drugModel = builder.buildDrugModel();
    ASSERT_NE(drugModel, nullptr);

    auto drugTreatment = std::make_unique<DrugTreatment>();

    const RequestResponseId requestResponseId = "1";
    const Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
    const Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
    ComputingOption computingOption(PredictionParameterType::Population, CompartmentsOption::MainCompartment);
    // The request's embedded trait is irrelevant; the null check fires before any dispatch.
    auto reqTrait = std::make_unique<ComputingTraitConcentration>(requestResponseId, start, end, 10.0, computingOption);
    ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(reqTrait));
    std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

    ASSERT_EQ(
            ComputingComponentTestHelper::computeNullConcentrationTraits(rawComponent, request, response),
            ComputingStatus::NoComputingTraits);
    EXPECT_TRUE(
            mockLogger.hasEntry(Tucuxi::Common::LogLevel::Error, "computing traits sent for computation are nullptr"));

    delete component;
}

/// \brief Test that computePercentilesSimple() returns NoComputingTraits when
/// called directly with a null trait pointer, covering the nullptr guard.
TEST(Core_TestComputingComponentErrors, NullTraitsPercentilesSimple)
{
    Tucuxi::Common::ScopedMockLogger mockLogger;

    auto* rawComponent = dynamic_cast<ComputingComponent*>(ComputingComponent::createComponent());
    IComputingService* component = dynamic_cast<IComputingService*>(rawComponent);
    ASSERT_NE(component, nullptr);

    BuildImatinib builder;
    auto drugModel = builder.buildDrugModel();
    ASSERT_NE(drugModel, nullptr);

    auto drugTreatment = std::make_unique<DrugTreatment>();

    const RequestResponseId requestResponseId = "1";
    const Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
    const Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
    ComputingOption computingOption(PredictionParameterType::Population, CompartmentsOption::MainCompartment);
    auto reqTrait = std::make_unique<ComputingTraitConcentration>(requestResponseId, start, end, 10.0, computingOption);
    ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(reqTrait));
    std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

    ASSERT_EQ(
            ComputingComponentTestHelper::computeNullPercentilesSimpleTraits(rawComponent, request, response),
            ComputingStatus::NoComputingTraits);
    EXPECT_TRUE(
            mockLogger.hasEntry(Tucuxi::Common::LogLevel::Error, "computing traits sent for computation are nullptr"));

    delete component;
}

/// \brief Test that compute(TraitAdjustment) returns NoComputingTraits when
/// called directly with a null trait pointer, covering the nullptr guard.
TEST(Core_TestComputingComponentErrors, NullTraitsAdjustment)
{
    Tucuxi::Common::ScopedMockLogger mockLogger;

    auto* rawComponent = dynamic_cast<ComputingComponent*>(ComputingComponent::createComponent());
    IComputingService* component = dynamic_cast<IComputingService*>(rawComponent);
    ASSERT_NE(component, nullptr);

    BuildImatinib builder;
    auto drugModel = builder.buildDrugModel();
    ASSERT_NE(drugModel, nullptr);

    auto drugTreatment = std::make_unique<DrugTreatment>();

    const RequestResponseId requestResponseId = "1";
    const Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
    const Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
    ComputingOption computingOption(PredictionParameterType::Population, CompartmentsOption::MainCompartment);
    auto reqTrait = std::make_unique<ComputingTraitConcentration>(requestResponseId, start, end, 10.0, computingOption);
    ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(reqTrait));
    std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

    ASSERT_EQ(
            ComputingComponentTestHelper::computeNullAdjustmentTraits(rawComponent, request, response),
            ComputingStatus::NoComputingTraits);
    EXPECT_TRUE(
            mockLogger.hasEntry(Tucuxi::Common::LogLevel::Error, "computing traits sent for computation are nullptr"));

    delete component;
}

/// \brief Test that compute(TraitSinglePoints) returns NoComputingTraits when
/// called directly with a null trait pointer, covering the nullptr guard.
TEST(Core_TestComputingComponentErrors, NullTraitsSinglePoints)
{
    Tucuxi::Common::ScopedMockLogger mockLogger;

    auto* rawComponent = dynamic_cast<ComputingComponent*>(ComputingComponent::createComponent());
    IComputingService* component = dynamic_cast<IComputingService*>(rawComponent);
    ASSERT_NE(component, nullptr);

    BuildImatinib builder;
    auto drugModel = builder.buildDrugModel();
    ASSERT_NE(drugModel, nullptr);

    auto drugTreatment = std::make_unique<DrugTreatment>();

    const RequestResponseId requestResponseId = "1";
    const Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
    const Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
    ComputingOption computingOption(PredictionParameterType::Population, CompartmentsOption::MainCompartment);
    auto reqTrait = std::make_unique<ComputingTraitConcentration>(requestResponseId, start, end, 10.0, computingOption);
    ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(reqTrait));
    std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

    ASSERT_EQ(
            ComputingComponentTestHelper::computeNullSinglePointsTraits(rawComponent, request, response),
            ComputingStatus::NoComputingTraits);
    EXPECT_TRUE(
            mockLogger.hasEntry(Tucuxi::Common::LogLevel::Error, "computing traits sent for computation are nullptr"));

    delete component;
}

/// \brief Test that compute() returns OutOfBoundsPercentileRank when a percentile rank
/// exceeds PERCENTILE_RANK_MAX (100), covering the bounds check in compute(TraitPercentiles).
TEST(Core_TestComputingComponentErrors, OutOfBoundsPercentileRank)
{
    IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());
    ASSERT_NE(component, nullptr);

    BuildImatinib builder;
    auto drugModel = builder.buildDrugModel();
    ASSERT_NE(drugModel, nullptr);

    auto drugTreatment = std::make_unique<DrugTreatment>();

    const RequestResponseId requestResponseId = "1";
    const Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
    const Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
    PercentileRanks outOfBoundsRanks({150.0}); // 150 > PERCENTILE_RANK_MAX (100)
    ComputingOption computingOption(PredictionParameterType::Population, CompartmentsOption::MainCompartment);
    auto traits = std::make_unique<ComputingTraitPercentiles>(
            requestResponseId, start, end, outOfBoundsRanks, 10.0, computingOption);

    ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(traits));
    std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

    ASSERT_EQ(component->compute(request, response), ComputingStatus::OutOfBoundsPercentileRank);

    delete component;
}

/// \brief Test that compute() returns Ok with an empty response when the
/// ComputingTraitSinglePoints has no requested times, covering the early-return branch.
TEST(Core_TestComputingComponentErrors, EmptySinglePointsTimes)
{
    IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());
    ASSERT_NE(component, nullptr);

    BuildImatinib builder;
    auto drugModel = builder.buildDrugModel();
    ASSERT_NE(drugModel, nullptr);

    auto drugTreatment = std::make_unique<DrugTreatment>();

    const RequestResponseId requestResponseId = "1";
    ComputingOption computingOption(PredictionParameterType::Population, CompartmentsOption::MainCompartment);
    std::vector<Tucuxi::Common::DateTime> emptyTimes;
    auto traits = std::make_unique<ComputingTraitSinglePoints>(requestResponseId, emptyTimes, computingOption);

    ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(traits));
    std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

    ASSERT_EQ(component->compute(request, response), ComputingStatus::Ok);

    delete component;
}

/// \brief Test concentration with RetrieveStatistics, covering the statistics branch in endRecord().
TEST(Core_TestComputingComponentErrors, ConcentrationRetrieveStatistics)
{
    IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());
    ASSERT_NE(component, nullptr);

    BuildImatinib builder;
    auto drugModel = builder.buildDrugModel();
    ASSERT_NE(drugModel, nullptr);

    auto drugTreatment = buildImatinibTreatment();

    const RequestResponseId requestResponseId = "1";
    const Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
    const Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
    ComputingOption computingOption(
            PredictionParameterType::Population,
            CompartmentsOption::MainCompartment,
            RetrieveStatisticsOption::RetrieveStatistics);
    auto traits = std::make_unique<ComputingTraitConcentration>(requestResponseId, start, end, 10.0, computingOption);

    ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(traits));
    std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

    ASSERT_EQ(component->compute(request, response), ComputingStatus::Ok);

    delete component;
}

/// \brief Test concentration with RetrieveParameters, covering the parameters branch in recordCycle().
TEST(Core_TestComputingComponentErrors, ConcentrationRetrieveParameters)
{
    IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());
    ASSERT_NE(component, nullptr);

    BuildImatinib builder;
    auto drugModel = builder.buildDrugModel();
    ASSERT_NE(drugModel, nullptr);

    auto drugTreatment = buildImatinibTreatment();

    const RequestResponseId requestResponseId = "1";
    const Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
    const Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
    ComputingOption computingOption(
            PredictionParameterType::Population,
            CompartmentsOption::MainCompartment,
            RetrieveStatisticsOption::DoNotRetrieveStatistics,
            RetrieveParametersOption::RetrieveParameters);
    auto traits = std::make_unique<ComputingTraitConcentration>(requestResponseId, start, end, 10.0, computingOption);

    ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(traits));
    std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

    ASSERT_EQ(component->compute(request, response), ComputingStatus::Ok);

    delete component;
}

/// \brief Test concentration with RetrieveCovariates, covering the covariates branch in recordCycle().
TEST(Core_TestComputingComponentErrors, ConcentrationRetrieveCovariates)
{
    IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());
    ASSERT_NE(component, nullptr);

    BuildImatinib builder;
    auto drugModel = builder.buildDrugModel();
    ASSERT_NE(drugModel, nullptr);

    auto drugTreatment = buildImatinibTreatment();

    const RequestResponseId requestResponseId = "1";
    const Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
    const Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
    ComputingOption computingOption(
            PredictionParameterType::Population,
            CompartmentsOption::MainCompartment,
            RetrieveStatisticsOption::DoNotRetrieveStatistics,
            RetrieveParametersOption::DoNotRetrieveParameters,
            RetrieveCovariatesOption::RetrieveCovariates);
    auto traits = std::make_unique<ComputingTraitConcentration>(requestResponseId, start, end, 10.0, computingOption);

    ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(traits));
    std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

    ASSERT_EQ(component->compute(request, response), ComputingStatus::Ok);

    delete component;
}
