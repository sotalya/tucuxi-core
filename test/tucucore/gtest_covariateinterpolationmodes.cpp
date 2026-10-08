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

#include <limits>
#include <memory>
#include <string>
#include <vector>

#include <date/date.h>
#include <gtest/gtest.h>

#include "tucucore/computingcomponent.h"
#include "tucucore/computingservice/computingrequest.h"
#include "tucucore/drugmodelchecker.h"
#include "tucucore/drugtreatment/patientcovariate.h"
#include "tucucore/pkmodel.h"

#include "drugmodels/buildconstantelimination.h"

#include "pkmodels/constanteliminationbolus.h"

#include "gtest_core.h"
#include "testutils.h"
#include "computingcomponentfactory.h"

using namespace std::chrono_literals;
using namespace date;
using namespace Tucuxi::Core;

// Return the value of the parameter with the given ID for a cycle, or NaN if it
// is absent (the per-cycle vectors are not ordered by ID).
static Value cycleParameter(CycleData const& _cycle, std::string const& _id)
{
    for (auto const& p : _cycle.m_parameters) {
        if (p.m_parameterId == _id) {
            return p.m_value;
        }
    }
    return std::numeric_limits<Value>::quiet_NaN();
}

// Return the value of the covariate with the given ID for a cycle, or NaN if it
// is absent.
static Value cycleCovariate(CycleData const& _cycle, std::string const& _id)
{
    for (auto const& c : _cycle.m_covariates) {
        if (c.m_covariateId == _id) {
            return c.m_value;
        }
    }
    return std::numeric_limits<Value>::quiet_NaN();
}

// Build a ConstantEliminationBolus model where each of the four covariates uses
// one interpolation mode, run an a-priori computation retrieving the parameters
// and covariates per cycle, and return the per-cycle data.
//
// The four covariates share the same measurements (a 1, 3, 5 ramp on 01.09,
// 03.09 and 05.09 at 8h, i.e. the linear function 1 + days-since-01.09), so the
// retrieved parameter equals its covariate and the only difference between
// covariates is the interpolation mode.
// The treatment doses every 6 hours over four days (= 16 cycles).
static std::vector<CycleData> computeConstantEliminationModes(
        InterpolationType _interpA, InterpolationType _interpM, InterpolationType _interpR, InterpolationType _interpS)
{
    BuildConstantElimination builder;
    auto drugModel = builder.buildDrugModel(
            ResidualErrorType::NONE,
            {0.0},
            ParameterVariabilityType::None,
            ParameterVariabilityType::None,
            ParameterVariabilityType::None,
            ParameterVariabilityType::None,
            0.0,
            1.0,
            0.0,
            0.0,
            _interpA,
            _interpM,
            _interpR,
            _interpS);

    DrugModelChecker checker;
    std::shared_ptr<PkModel> sharedPkModel =
            std::make_shared<PkModel>("test.constantelimination", PkModel::AllowMultipleRoutes::No);
    sharedPkModel->addIntakeIntervalCalculatorFactory(
            AbsorptionModel::Extravascular, ConstantEliminationBolus::getCreator());
    sharedPkModel->addParameterList(AbsorptionModel::Extravascular, ConstantEliminationBolus::getParametersId());

    std::shared_ptr<PkModelCollection> collection = std::make_shared<PkModelCollection>();
    collection->addPkModel(sharedPkModel);
    DrugModelChecker::CheckerResult_t checkerResult = checker.checkDrugModel(drugModel.get(), collection.get());
    EXPECT_TRUE(checkerResult.m_ok);

    auto component = ComputingComponentFactory::createComputingService();
    EXPECT_TRUE(component != nullptr);
    component->setPkModelCollection(collection);

    const FormulationAndRoute route(Formulation::OralSolution, AdministrationRoute::Oral);
    DateTime const startSept2018(2018_y / sep / 1, 8h + 0min);

    auto drugTreatment = buildDrugTreatment(route, startSept2018);

    for (auto const& covId : {std::string("covS"), std::string("covA"), std::string("covR"), std::string("covM")}) {
        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                covId, "1.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2018, 9, 1, 8, 0, 0)));
        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                covId, "3.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2018, 9, 3, 8, 0, 0)));
        drugTreatment->addCovariate(std::make_unique<PatientCovariate>(
                covId, "5.0", DataType::Double, TucuUnit(""), DATE_TIME_NO_VAR(2018, 9, 5, 8, 0, 0)));
    }

    RequestResponseId const requestResponseId = "1";
    Tucuxi::Common::DateTime const start(2018_y / sep / 1, 8h + 0min);
    Tucuxi::Common::DateTime const end(2018_y / sep / 5, 8h + 0min);
    double const nbPointsPerHour = 10.0;
    ComputingOption const computingOption(
            PredictionParameterType::Apriori,
            CompartmentsOption::MainCompartment,
            RetrieveStatisticsOption::DoNotRetrieveStatistics,
            RetrieveParametersOption::RetrieveParameters,
            RetrieveCovariatesOption::RetrieveCovariates);
    std::unique_ptr<ComputingTraitConcentration> traits = std::make_unique<ComputingTraitConcentration>(
            requestResponseId, start, end, nbPointsPerHour, computingOption);

    ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(traits));
    std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

    ComputingStatus const result = component->compute(request, response);
    EXPECT_EQ(result, ComputingStatus::Ok);

    const ComputedData* responseData = response->getData();
    const SinglePredictionData* resp = dynamic_cast<const SinglePredictionData*>(responseData);
    EXPECT_TRUE(resp != nullptr);
    if (resp == nullptr) {
        return {};
    }
    return resp->getData();
}

/// \brief Global test of the covariate interpolation modes.
///
/// One covariate per mode, all fed the same 1, 3, 5 ramp. For each cycle, the
/// retrieved parameter equals its covariate, and the covariate follows the value
// expected for its mode.
TEST(Core_TestCovariateInterpolationModes, PerCycleParameterValues)
{
    std::vector<CycleData> data = computeConstantEliminationModes(
            InterpolationType::Backward,
            InterpolationType::InterpolationBackward,
            InterpolationType::InterpolationForward,
            InterpolationType::Forward);

    ASSERT_EQ(data.size(), static_cast<size_t>(16));

    for (size_t i = 0; i < data.size(); ++i) {
        CycleData const& cycle = data[i];

        ASSERT_DOUBLE_EQ(cycleParameter(cycle, "TestS"), cycleCovariate(cycle, "covS"));
        ASSERT_DOUBLE_EQ(cycleParameter(cycle, "TestA"), cycleCovariate(cycle, "covA"));
        ASSERT_DOUBLE_EQ(cycleParameter(cycle, "TestR"), cycleCovariate(cycle, "covR"));
        ASSERT_DOUBLE_EQ(cycleParameter(cycle, "TestM"), cycleCovariate(cycle, "covM"));

        // Interpolated modes: the covariate ramps by 0.25 per 6h cycle.
        // InterpolationForward uses the value at this dose (interval start).
        ASSERT_DOUBLE_EQ(cycleCovariate(cycle, "covR"), 1.0 + 0.25 * static_cast<double>(i));
        // InterpolationBackward uses the value at the next dose (interval end).
        ASSERT_DOUBLE_EQ(cycleCovariate(cycle, "covM"), 1.25 + 0.25 * static_cast<double>(i));

        // Forward holds the previous measured value: 1 until 03.09 8h (cycle 8),
        // then 3.
        double const expectedForward = (i < 8) ? 1.0 : 3.0;
        ASSERT_DOUBLE_EQ(cycleCovariate(cycle, "covS"), expectedForward);

        // Backward applies the future measured value per dose: for doses between
        // 01.09 and 03.09 (cycles 1-7) the next measurement is 3. Cycle 8 is on
        // the 03.09 measurement (3). For doses after it (cycles 9-15) the next
        // measurement is 5. Cycle 0 sits on the first measurement, so it is 1.
        double const expectedBackward = (i == 0) ? 1.0 : (i <= 8) ? 3.0 : 5.0;
        ASSERT_DOUBLE_EQ(cycleCovariate(cycle, "covA"), expectedBackward);
    }
}

/// \brief Default must resolve to InterpolationBackward.
///
/// covM uses Default and covR uses InterpolationBackward, on the same ramp, so
/// they must produce identical per-cycle values (the InterpolationBackward ramp).
TEST(Core_TestCovariateInterpolationModes, DefaultResolvesToInterpolationBackward)
{
    std::vector<CycleData> data = computeConstantEliminationModes(
            InterpolationType::Forward,
            InterpolationType::Default,
            InterpolationType::InterpolationBackward,
            InterpolationType::Forward);

    ASSERT_EQ(data.size(), static_cast<size_t>(16));

    for (size_t i = 0; i < data.size(); ++i) {
        CycleData const& cycle = data[i];
        // Default behaves as InterpolationBackward, so covM equals covR.
        ASSERT_DOUBLE_EQ(cycleCovariate(cycle, "covM"), cycleCovariate(cycle, "covR"));
        // Both follow the InterpolationBackward ramp.
        ASSERT_DOUBLE_EQ(cycleCovariate(cycle, "covM"), 1.25 + 0.25 * static_cast<double>(i));
    }
}
