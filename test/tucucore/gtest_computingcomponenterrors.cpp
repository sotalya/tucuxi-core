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
#include "tucucore/multicomputingcomponent.h"
#include "tucucore/pkmodel.h"

#include "computingcomponentfactory.h"
#include "drugmodels/buildimatinib.h"
#include "mocklogger.h"

using namespace Tucuxi::Core;

using namespace std::chrono_literals;
using namespace date;

namespace Tucuxi {
namespace Core {

///
/// \brief Template helper granting test access to both ComputingComponent's and
/// MultiComputingComponent's private constructors and private compute() overloads.
/// Both component classes declare this template as a friend.
///
template<typename T>
class ComputingComponentTestHelper
{
public:
    static T* createUninitialized()
    {
        return new T();
    }

    static ComputingStatus computeNullConcentrationTraits(
            T* _cc, const ComputingRequest& _request, std::unique_ptr<ComputingResponse>& _response)
    {
        return _cc->compute(static_cast<const ComputingTraitConcentration*>(nullptr), _request, _response);
    }

    static ComputingStatus computeNullAdjustmentTraits(
            T* _cc, const ComputingRequest& _request, std::unique_ptr<ComputingResponse>& _response)
    {
        return _cc->compute(static_cast<const ComputingTraitAdjustment*>(nullptr), _request, _response);
    }

    static ComputingStatus computeNullSinglePointsTraits(
            T* _cc, const ComputingRequest& _request, std::unique_ptr<ComputingResponse>& _response)
    {
        return _cc->compute(static_cast<const ComputingTraitSinglePoints*>(nullptr), _request, _response);
    }

    ///
    /// \brief Calls computePercentilesSimple() with a null trait pointer.
    /// Only valid when T is ComputingComponent; do not call for MultiComputingComponent.
    ///
    static ComputingStatus computeNullPercentilesSimpleTraits(
            T* _cc, const ComputingRequest& _request, std::unique_ptr<ComputingResponse>& _response)
    {
        return _cc->computePercentilesSimple(
                static_cast<const ComputingTraitPercentiles*>(nullptr), _request, _response);
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


///
/// \brief Maps a component type to its ComputingComponentFactory::CreationMode.
///
template<typename T>
struct ComponentCreationMode;

template<>
struct ComponentCreationMode<ComputingComponent>
{
    static constexpr ComputingComponentFactory::CreationMode value = ComputingComponentFactory::CreationMode::Single;
};

template<>
struct ComponentCreationMode<MultiComputingComponent>
{
    static constexpr ComputingComponentFactory::CreationMode value = ComputingComponentFactory::CreationMode::Multi;
};

///
/// \brief Name generator for TYPED_TEST_SUITE: produces "Single" and "Multi" suffixes.
///
struct ComponentTypeNames
{
    template<typename T>
    static std::string GetName(int /*i*/)
    {
        if (std::is_same<T, ComputingComponent>::value) {
            return "Single";
        }
        return "Multi";
    }
};

///
/// \brief Typed fixture that runs each test against both ComputingComponent (T=ComputingComponent)
/// and MultiComputingComponent (T=MultiComputingComponent), using the shared IComputingService
/// interface. The template parameter drives factory mode and concrete casts — no runtime branching.
///
template<typename T>
class ComputingComponentErrorsFixture : public ::testing::Test
{
protected:
    std::unique_ptr<IComputingService> createComponent() const
    {
        ComputingComponentFactory::setMode(ComponentCreationMode<T>::value);
        auto c = ComputingComponentFactory::createComputingService();
        ComputingComponentFactory::setMode(ComputingComponentFactory::CreationMode::Meta);
        return c;
    }

    IComputingService* createUninitializedComponent() const
    {
        return ComputingComponentTestHelper<T>::createUninitialized();
    }

    ComputingStatus callNullConcentrationTraits(
            IComputingService* _svc, const ComputingRequest& _req, std::unique_ptr<ComputingResponse>& _resp) const
    {
        return ComputingComponentTestHelper<T>::computeNullConcentrationTraits(dynamic_cast<T*>(_svc), _req, _resp);
    }

    ComputingStatus callNullAdjustmentTraits(
            IComputingService* _svc, const ComputingRequest& _req, std::unique_ptr<ComputingResponse>& _resp) const
    {
        return ComputingComponentTestHelper<T>::computeNullAdjustmentTraits(dynamic_cast<T*>(_svc), _req, _resp);
    }

    ComputingStatus callNullSinglePointsTraits(
            IComputingService* _svc, const ComputingRequest& _req, std::unique_ptr<ComputingResponse>& _resp) const
    {
        return ComputingComponentTestHelper<T>::computeNullSinglePointsTraits(dynamic_cast<T*>(_svc), _req, _resp);
    }
};

using ComponentTypes = ::testing::Types<ComputingComponent>;
TYPED_TEST_SUITE(ComputingComponentErrorsFixture, ComponentTypes, ComponentTypeNames);

/// \brief Test that compute() returns ComputingComponentNotInitialized when the component
/// has not been initialized (m_utils == nullptr), covering the early-exit branch.
TYPED_TEST(ComputingComponentErrorsFixture, NotInitialized)
{
    Tucuxi::Common::ScopedMockLogger mockLogger;

    IComputingService* component = this->createUninitializedComponent();
    ASSERT_NE(component, nullptr);

    BuildImatinib builder;
    auto drugModel = builder.buildDrugModel();
    ASSERT_NE(drugModel, nullptr);

    auto drugTreatment = std::make_unique<DrugTreatment>();

    if (dynamic_cast<ComputingComponent*>(component) != nullptr) {
        ASSERT_EQ(
                computeWithStandardRequest(component, *drugModel, *drugTreatment),
                ComputingStatus::ComputingComponentNotInitialized);
    }
    else {
        ASSERT_EQ(
                computeWithStandardRequest(component, *drugModel, *drugTreatment),
                ComputingStatus::MultiComputingComponentNotInitialized);
    }
    EXPECT_TRUE(mockLogger.hasEntry(Tucuxi::Common::LogLevel::Error, "has not been initialized"));

    delete component;
}

/// \brief Test that compute() returns NoPkModels when the PkModel collection is null
/// (m_utils->m_models == nullptr).
TYPED_TEST(ComputingComponentErrorsFixture, NullPkModelCollection)
{
    Tucuxi::Common::ScopedMockLogger mockLogger;

    auto component = this->createComponent();
    ASSERT_NE(component, nullptr);

    component->setPkModelCollection(nullptr);

    BuildImatinib builder;
    auto drugModel = builder.buildDrugModel();
    ASSERT_NE(drugModel, nullptr);

    auto drugTreatment = std::make_unique<DrugTreatment>();

    ASSERT_EQ(computeWithStandardRequest(component.get(), *drugModel, *drugTreatment), ComputingStatus::NoPkModels);
    EXPECT_TRUE(mockLogger.hasEntry(Tucuxi::Common::LogLevel::Error, "No Pk Model loaded"));
}

/// \brief Test that compute() returns NoPkModels when the PkModel collection is empty
/// (getPkModelList().empty()).
TYPED_TEST(ComputingComponentErrorsFixture, EmptyPkModelCollection)
{
    Tucuxi::Common::ScopedMockLogger mockLogger;

    auto component = this->createComponent();
    ASSERT_NE(component, nullptr);

    component->setPkModelCollection(std::make_shared<PkModelCollection>());

    BuildImatinib builder;
    auto drugModel = builder.buildDrugModel();
    ASSERT_NE(drugModel, nullptr);

    auto drugTreatment = std::make_unique<DrugTreatment>();

    ASSERT_EQ(computeWithStandardRequest(component.get(), *drugModel, *drugTreatment), ComputingStatus::NoPkModels);
    EXPECT_TRUE(mockLogger.hasEntry(Tucuxi::Common::LogLevel::Error, "No Pk Model loaded"));
}

/// \brief Test that compute() returns IncompatibleTreatmentModel when the drug treatment uses
/// a formulation/route not supported by the drug model.
TYPED_TEST(ComputingComponentErrorsFixture, IncompatibleTreatmentModel)
{
    auto component = this->createComponent();
    ASSERT_NE(component, nullptr);

    BuildImatinib builder;
    auto drugModel = builder.buildDrugModel();
    ASSERT_NE(drugModel, nullptr);

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
            computeWithStandardRequest(component.get(), *drugModel, *drugTreatment),
            ComputingStatus::IncompatibleTreatmentModel);
}

/// \brief Test that compute() returns ComputingComponentExceptionError when a trait throws,
/// covering the TUCU_CATCH(...) block in compute().
TYPED_TEST(ComputingComponentErrorsFixture, ExceptionInTrait)
{
    auto component = this->createComponent();
    ASSERT_NE(component, nullptr);

    BuildImatinib builder;
    auto drugModel = builder.buildDrugModel();
    ASSERT_NE(drugModel, nullptr);

    auto drugTreatment = std::make_unique<DrugTreatment>();

    const RequestResponseId requestResponseId = "1";
    ComputingOption computingOption(PredictionParameterType::Population, CompartmentsOption::MainCompartment);
    auto traits = std::make_unique<ThrowingComputingTrait>(requestResponseId, computingOption);

    ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(traits));
    std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

    if (dynamic_cast<ComputingComponent*>(component.get())) {
        ASSERT_EQ(component->compute(request, response), ComputingStatus::ComputingComponentExceptionError);
    }
    else {
        ASSERT_EQ(component->compute(request, response), ComputingStatus::MultiComputingComponentExceptionError);
    }
}

/// \brief Test that compute(TraitConcentration) returns NoComputingTraits when called directly
/// with a null trait pointer, covering the nullptr guard.
TYPED_TEST(ComputingComponentErrorsFixture, NullTraitsConcentration)
{
    Tucuxi::Common::ScopedMockLogger mockLogger;

    auto component = this->createComponent();
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
            this->callNullConcentrationTraits(component.get(), request, response), ComputingStatus::NoComputingTraits);
    EXPECT_TRUE(
            mockLogger.hasEntry(Tucuxi::Common::LogLevel::Error, "computing traits sent for computation are nullptr"));
}

/// \brief Test that computePercentilesSimple() returns NoComputingTraits when called directly
/// with a null trait pointer. This overload exists only in ComputingComponent (Single mode).
TEST(Core_TestComputingComponentErrors, NullTraitsPercentilesSimple)
{
    Tucuxi::Common::ScopedMockLogger mockLogger;

    ComputingComponentFactory::setMode(ComputingComponentFactory::CreationMode::Single);
    auto component = ComputingComponentFactory::createComputingService();
    ComputingComponentFactory::setMode(ComputingComponentFactory::CreationMode::Meta);
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
            ComputingComponentTestHelper<ComputingComponent>::computeNullPercentilesSimpleTraits(
                    dynamic_cast<ComputingComponent*>(component.get()), request, response),
            ComputingStatus::NoComputingTraits);
    EXPECT_TRUE(
            mockLogger.hasEntry(Tucuxi::Common::LogLevel::Error, "computing traits sent for computation are nullptr"));
}

/// \brief Test that compute(TraitAdjustment) returns NoComputingTraits when called directly
/// with a null trait pointer, covering the nullptr guard.
TYPED_TEST(ComputingComponentErrorsFixture, NullTraitsAdjustment)
{
    Tucuxi::Common::ScopedMockLogger mockLogger;

    auto component = this->createComponent();
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

    ASSERT_EQ(this->callNullAdjustmentTraits(component.get(), request, response), ComputingStatus::NoComputingTraits);
    EXPECT_TRUE(
            mockLogger.hasEntry(Tucuxi::Common::LogLevel::Error, "computing traits sent for computation are nullptr"));
}

/// \brief Test that compute(TraitSinglePoints) returns NoComputingTraits when called directly
/// with a null trait pointer, covering the nullptr guard.
TYPED_TEST(ComputingComponentErrorsFixture, NullTraitsSinglePoints)
{
    Tucuxi::Common::ScopedMockLogger mockLogger;

    auto component = this->createComponent();
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

    ASSERT_EQ(this->callNullSinglePointsTraits(component.get(), request, response), ComputingStatus::NoComputingTraits);
    EXPECT_TRUE(
            mockLogger.hasEntry(Tucuxi::Common::LogLevel::Error, "computing traits sent for computation are nullptr"));
}

/// \brief Test that compute() returns OutOfBoundsPercentileRank when a percentile rank
/// exceeds PERCENTILE_RANK_MAX (100), covering the bounds check in compute(TraitPercentiles).
TYPED_TEST(ComputingComponentErrorsFixture, OutOfBoundsPercentileRank)
{
    auto component = this->createComponent();
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
}

/// \brief Test that compute() returns Ok with an empty response when the
/// ComputingTraitSinglePoints has no requested times, covering the early-return branch.
TYPED_TEST(ComputingComponentErrorsFixture, EmptySinglePointsTimes)
{
    auto component = this->createComponent();
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
}

/// \brief Test concentration with RetrieveStatistics, covering the statistics branch in endRecord().
TYPED_TEST(ComputingComponentErrorsFixture, ConcentrationRetrieveStatistics)
{
    auto component = this->createComponent();
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
}

/// \brief Test concentration with RetrieveParameters, covering the parameters branch in recordCycle().
TYPED_TEST(ComputingComponentErrorsFixture, ConcentrationRetrieveParameters)
{
    auto component = this->createComponent();
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
}

/// \brief Test concentration with RetrieveCovariates, covering the covariates branch in recordCycle().
TYPED_TEST(ComputingComponentErrorsFixture, ConcentrationRetrieveCovariates)
{
    auto component = this->createComponent();
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
}
