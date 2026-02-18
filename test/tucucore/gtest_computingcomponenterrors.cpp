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

using namespace Tucuxi::Core;

using namespace std::chrono_literals;
using namespace date;

namespace Tucuxi {
namespace Core {

///
/// \brief Helper class granting test access to ComputingComponent's private constructor.
///
/// This allows tests to create an uninitialized ComputingComponent (m_utils == nullptr)
/// in order to cover the early-exit error path in ComputingComponent::compute().
///
class ComputingComponentTestHelper
{
public:
    static ComputingComponent* createUninitialized()
    {
        return new ComputingComponent();
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

    delete component;
}

/// \brief Test that compute() returns NoPkModels when the PkModel collection is null
/// (m_utils->m_models == nullptr).
TEST(Core_TestComputingComponentErrors, NullPkModelCollection)
{
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

    delete component;
}

/// \brief Test that compute() returns NoPkModels when the PkModel collection is empty
/// (getPkModelList().empty()).
TEST(Core_TestComputingComponentErrors, EmptyPkModelCollection)
{
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
    LastingDose dose(DoseValue(200.0), TucuUnit("mg"), incompatibleRoute, Duration(), Duration(std::chrono::hours(24)));
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
