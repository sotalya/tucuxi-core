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

// Add (time, value string) measurements for one covariate to a treatment.
static void addCovariateMeasurements(
        DrugTreatment& _treatment,
        std::string const& _covId,
        std::vector<std::pair<Tucuxi::Common::DateTime, std::string>> const& _measurements)
{
    for (auto const& measurement : _measurements) {
        _treatment.addCovariate(std::make_unique<PatientCovariate>(
                _covId, measurement.second, DataType::Double, TucuUnit(""), measurement.first));
    }
}

// Build the ConstantEliminationBolus model with its four covariates covA, covM,
// covR, covS each using the given interpolation mode, feed each the given
// (time, value string) measurements (an empty list leaves the covariate at its
// default constant value), run an a-priori computation over 01.09-05.09, and
// return the per-cycle data. The treatment doses every 6 hours over four days
// (16 cycles).
static std::vector<CycleData> computeMultiCovariate(
        InterpolationType _interpA,
        std::vector<std::pair<Tucuxi::Common::DateTime, std::string>> const& _measA,
        InterpolationType _interpM,
        std::vector<std::pair<Tucuxi::Common::DateTime, std::string>> const& _measM,
        InterpolationType _interpR,
        std::vector<std::pair<Tucuxi::Common::DateTime, std::string>> const& _measR,
        InterpolationType _interpS,
        std::vector<std::pair<Tucuxi::Common::DateTime, std::string>> const& _measS,
        int _intervalHours = 6,
        unsigned int _nbrDoses = 16)
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

    auto drugTreatment = buildDrugTreatment(
            route, startSept2018, Tucuxi::Core::DoseValue{200}, TucuUnit("mg"), _intervalHours, _nbrDoses);
    addCovariateMeasurements(*drugTreatment, "covA", _measA);
    addCovariateMeasurements(*drugTreatment, "covM", _measM);
    addCovariateMeasurements(*drugTreatment, "covR", _measR);
    addCovariateMeasurements(*drugTreatment, "covS", _measS);

    RequestResponseId const requestResponseId = "1";
    Tucuxi::Common::DateTime const start(2018_y / sep / 1, 8h + 0min);
    Tucuxi::Common::DateTime const end =
            startSept2018 + Tucuxi::Common::Duration(std::chrono::hours(_intervalHours * static_cast<int>(_nbrDoses)));
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

// A covariate measurement as an offset in hours from the treatment start (which
// is also the first dose) plus its value. Offsets may be negative (before the 
// start) or beyond the end.
struct TimedMeasurement
{
    int m_hoursFromStart;
    double m_value;
};

// Golden reference models for the interpolations.

// Clamped linear interpolation: hold the first or last measurement outside the
// measured range, linear between the two bracketing measurements.
static double refLinear(std::vector<TimedMeasurement> const& _measurements, double const _tHours)
{
    double const tFront = static_cast<double>(_measurements.front().m_hoursFromStart);
    if (_tHours <= tFront) {
        return _measurements.front().m_value;
    }
    double const tBack = static_cast<double>(_measurements.back().m_hoursFromStart);
    if (_tHours >= tBack) {
        return _measurements.back().m_value;
    }
    for (size_t k = 1; k < _measurements.size(); ++k) {
        double const t2 = static_cast<double>(_measurements[k].m_hoursFromStart);
        if (_tHours <= t2) {
            double const t1 = static_cast<double>(_measurements[k - 1].m_hoursFromStart);
            double const slope = (_measurements[k].m_value - _measurements[k - 1].m_value) / (t2 - t1);
            return _measurements[k - 1].m_value + (slope * (_tHours - t1));
        }
    }
    return _measurements.back().m_value;
}

// Forward step: hold the latest measurement at or before the instant, clamping
// to the first measurement before the measured range.
static double refForward(std::vector<TimedMeasurement> const& _measurements, double const _tHours)
{
    double result = _measurements.front().m_value;
    for (auto const& measurement : _measurements) {
        if (static_cast<double>(measurement.m_hoursFromStart) <= _tHours) {
            result = measurement.m_value;
        }
        else {
            break;
        }
    }
    return result;
}

// Backward step: use the earliest measurement at or after the instant, clamping
// to the last measurement past the measured range.
static double refBackward(std::vector<TimedMeasurement> const& _measurements, double const _tHours)
{
    for (auto const& measurement : _measurements) {
        if (static_cast<double>(measurement.m_hoursFromStart) >= _tHours) {
            return measurement.m_value;
        }
    }
    return _measurements.back().m_value;
}

// Expected covariate value for one cycle under a given mode. InterpolationForward
// and Backward are evaluated at the dose (interval start, intervalHours * cycle).
// The other interpolated modes at the next dose (interval end, intervalHours *
// (cycle + 1)).
static double refExpected(
        InterpolationType const _mode,
        std::vector<TimedMeasurement> const& _measurements,
        size_t const _cycle,
        double const _intervalHours = 6.0)
{
    double const tStart = _intervalHours * static_cast<double>(_cycle);
    double const tEnd = _intervalHours * static_cast<double>(_cycle + 1);
    switch (_mode) {
    case InterpolationType::Forward:
        return refForward(_measurements, tStart);
    case InterpolationType::Backward:
        return refBackward(_measurements, tStart);
    case InterpolationType::InterpolationForward:
        return refLinear(_measurements, tStart);
    case InterpolationType::InterpolationBackward:
    case InterpolationType::Default:
        return refLinear(_measurements, tEnd);
    default:
        return std::numeric_limits<double>::quiet_NaN();
    }
}

static std::string interpName(InterpolationType const _mode)
{
    switch (_mode) {
    case InterpolationType::Forward:
        return "Forward";
    case InterpolationType::Backward:
        return "Backward";
    case InterpolationType::InterpolationForward:
        return "InterpolationForward";
    case InterpolationType::InterpolationBackward:
        return "InterpolationBackward";
    case InterpolationType::Default:
        return "Default";
    default:
        return "other";
    }
}

// Convert hour-offset measurements into dated (time, value string) pairs anchored
// at the treatment start (01.09.2018 08h00).
static std::vector<std::pair<Tucuxi::Common::DateTime, std::string>> datedMeasurements(
        std::vector<TimedMeasurement> const& _measurements)
{
    DateTime const treatmentStart(2018_y / sep / 1, 8h + 0min);
    std::vector<std::pair<Tucuxi::Common::DateTime, std::string>> result;
    for (auto const& measurement : _measurements) {
        result.emplace_back(
                treatmentStart + Tucuxi::Common::Duration(std::chrono::hours(measurement.m_hoursFromStart)),
                std::to_string(measurement.m_value));
    }
    return result;
}

// Check one covariate (and its parameter) per cycle against the reference value
// for its mode and measurements. The retrieved parameter must equal its
// covariate, and the covariate must follow the mode.
static void expectCovariate(
        std::vector<CycleData> const& _data,
        std::string const& _covId,
        std::string const& _paramId,
        InterpolationType const _mode,
        std::vector<TimedMeasurement> const& _measurements,
        double const _intervalHours = 6.0,
        size_t const _expectedCycles = 16)
{
    ASSERT_EQ(_data.size(), _expectedCycles);
    for (size_t i = 0; i < _data.size(); ++i) {
        SCOPED_TRACE(_covId + " " + interpName(_mode) + ", cycle " + std::to_string(i));
        EXPECT_DOUBLE_EQ(cycleParameter(_data[i], _paramId), cycleCovariate(_data[i], _covId));
        EXPECT_NEAR(cycleCovariate(_data[i], _covId), refExpected(_mode, _measurements, i, _intervalHours), 1e-6);
    }
}

// Single-covariate case: drive covR with the given mode and measurements, leave
// the other covariates at their defaults, and check covR.
static void runTimingCase(InterpolationType const _mode, std::vector<TimedMeasurement> const& _measurements)
{
    std::vector<CycleData> data = computeMultiCovariate(
            InterpolationType::Forward,
            {},
            InterpolationType::Forward,
            {},
            _mode,
            datedMeasurements(_measurements),
            InterpolationType::Forward,
            {});
    expectCovariate(data, "covR", "TestR", _mode, _measurements);
}

/// \brief (Scenario 1) Measurements that do not fall on a dose time.
///
/// Doses are at 08h00/14h00/20h00/02h00; the measurements are at 11h00, off the
/// grid, on the line 1 + hoursFromStart / 24. This proves interpolation keys off
/// the measurement times rather than assuming dose-aligned measurements.
TEST(Core_TestCovariateInterpolationModes, TimingOffDoseGrid)
{
    std::vector<TimedMeasurement> const measurements = {{3, 1.125}, {51, 3.125}, {99, 5.125}};
    runTimingCase(InterpolationType::InterpolationForward, measurements);
    runTimingCase(InterpolationType::InterpolationBackward, measurements);
    runTimingCase(InterpolationType::Backward, measurements);
}

/// \brief (Scenario 2) A measurement before the treatment start.
///
/// 31.08 08h00 (0.0) and 02.09 08h00 (2.0). The pre-start measurement must be
/// kept as the left border so the early cycles interpolate from it (the
/// interpolated modes give 1.0 at cycle 0, not the 2.0 they would give if it
/// were dropped).
TEST(Core_TestCovariateInterpolationModes, TimingMeasurementBeforeStart)
{
    std::vector<TimedMeasurement> const measurements = {{-24, 0.0}, {24, 2.0}};
    runTimingCase(InterpolationType::Forward, measurements);
    runTimingCase(InterpolationType::Backward, measurements);
    runTimingCase(InterpolationType::InterpolationForward, measurements);
    runTimingCase(InterpolationType::InterpolationBackward, measurements);
}

/// \brief (Scenario 3) A measurement after the treatment end.
///
/// 03.09 08h00 (3.0) and 06.09 08h00 (6.0). The post-end measurement must be
/// kept as the right border so the late cycles interpolate toward it instead of
/// clamping to 3.0.
TEST(Core_TestCovariateInterpolationModes, TimingMeasurementAfterEnd)
{
    std::vector<TimedMeasurement> const measurements = {{48, 3.0}, {120, 6.0}};
    runTimingCase(InterpolationType::Forward, measurements);
    runTimingCase(InterpolationType::Backward, measurements);
    runTimingCase(InterpolationType::InterpolationForward, measurements);
    runTimingCase(InterpolationType::InterpolationBackward, measurements);
}

/// \brief (Scenario 4) Three values bounded strictly inside the treatment.
///
/// 02.09, 03.09 and 04.09 at 08h00 (first after the start, last before the final
/// interval). Exercises clamping at both ends plus interpolation between, and
/// confirms Default matches InterpolationBackward on these measurements.
TEST(Core_TestCovariateInterpolationModes, TimingMeasurementsBoundedInside)
{
    std::vector<TimedMeasurement> const measurements = {{24, 2.0}, {48, 4.0}, {72, 6.0}};
    runTimingCase(InterpolationType::Forward, measurements);
    runTimingCase(InterpolationType::Backward, measurements);
    runTimingCase(InterpolationType::InterpolationForward, measurements);
    runTimingCase(InterpolationType::InterpolationBackward, measurements);
    runTimingCase(InterpolationType::Default, measurements);
}

/// \brief Several covariates computed together, each with a different mode AND a
/// different measurement timing.
///
/// covA (Backward) is measured before the start, covM (InterpolationBackward)
/// after the end, covR (InterpolationForward) bounded strictly inside, and covS
/// (Forward) on a dose-aligned spanning ramp. Each is checked independently,
/// confirming simultaneous covariates do not interfere.
TEST(Core_TestCovariateInterpolationModes, MultipleCovariatesDifferentModesAndTimings)
{
    std::vector<TimedMeasurement> const beforeStart = {{-24, 0.0}, {24, 2.0}};
    std::vector<TimedMeasurement> const afterEnd = {{48, 3.0}, {120, 6.0}};
    std::vector<TimedMeasurement> const insideRamp = {{24, 2.0}, {48, 4.0}, {72, 6.0}};
    std::vector<TimedMeasurement> const spanningRamp = {{0, 1.0}, {48, 3.0}, {96, 5.0}};

    std::vector<CycleData> data = computeMultiCovariate(
            InterpolationType::Backward,
            datedMeasurements(beforeStart),
            InterpolationType::InterpolationBackward,
            datedMeasurements(afterEnd),
            InterpolationType::InterpolationForward,
            datedMeasurements(insideRamp),
            InterpolationType::Forward,
            datedMeasurements(spanningRamp));

    expectCovariate(data, "covA", "TestA", InterpolationType::Backward, beforeStart);
    expectCovariate(data, "covM", "TestM", InterpolationType::InterpolationBackward, afterEnd);
    expectCovariate(data, "covR", "TestR", InterpolationType::InterpolationForward, insideRamp);
    expectCovariate(data, "covS", "TestS", InterpolationType::Forward, spanningRamp);
}

/// \brief Several covariates computed together, all using InterpolationForward
/// but each with a different measurement timing.
///
/// covA before the start, covM after the end, covR bounded inside, covS off the
/// dose grid. Confirms each covariate is interpolated from its own measurement
/// series only, with no cross-contamination between them.
TEST(Core_TestCovariateInterpolationModes, MultipleCovariatesSameModeDistinctTimings)
{
    std::vector<TimedMeasurement> const beforeStart = {{-24, 0.0}, {24, 2.0}};
    std::vector<TimedMeasurement> const afterEnd = {{48, 3.0}, {120, 6.0}};
    std::vector<TimedMeasurement> const insideRamp = {{24, 2.0}, {48, 4.0}, {72, 6.0}};
    std::vector<TimedMeasurement> const offGrid = {{3, 1.125}, {51, 3.125}, {99, 5.125}};

    InterpolationType const mode = InterpolationType::InterpolationForward;
    std::vector<CycleData> data = computeMultiCovariate(
            mode,
            datedMeasurements(beforeStart),
            mode,
            datedMeasurements(afterEnd),
            mode,
            datedMeasurements(insideRamp),
            mode,
            datedMeasurements(offGrid));

    expectCovariate(data, "covA", "TestA", mode, beforeStart);
    expectCovariate(data, "covM", "TestM", mode, afterEnd);
    expectCovariate(data, "covR", "TestR", mode, insideRamp);
    expectCovariate(data, "covS", "TestS", mode, offGrid);
}

/// \brief Covariate interpolation on a 12-hour dosing interval (8 cycles).
///
/// Everything else uses the 6-hourly grid, here the dose is every 12 hours, so
/// the covariate is sampled at 12h * cycle. covR (InterpolationForward) is
/// measured bounded inside (02.09, 03.09, 04.09 at 08h00), confirming the
/// per-cycle value follows the actual dose grid rather than a fixed 6h step.
TEST(Core_TestCovariateInterpolationModes, DosingInterval12Hours)
{
    int const intervalHours = 12;
    unsigned int const nbrDoses = 8;
    std::vector<TimedMeasurement> const measurements = {{24, 2.0}, {48, 4.0}, {72, 6.0}};

    std::vector<CycleData> data = computeMultiCovariate(
            InterpolationType::Forward,
            {},
            InterpolationType::Forward,
            {},
            InterpolationType::InterpolationForward,
            datedMeasurements(measurements),
            InterpolationType::Forward,
            {},
            intervalHours,
            nbrDoses);

    expectCovariate(data, "covR", "TestR", InterpolationType::InterpolationForward, measurements, 12.0, nbrDoses);
}

/// \brief Covariate interpolation on a 24-hour (daily) dosing interval (4 cycles).
///
/// covR (Backward) is measured at 01.09, 02.09, 03.09 at 08h00. At each daily
/// dose Backward returns the next measured value, and the final day (no later
/// measurement) holds the last value.
TEST(Core_TestCovariateInterpolationModes, DosingInterval24Hours)
{
    int const intervalHours = 24;
    unsigned int const nbrDoses = 4;
    std::vector<TimedMeasurement> const measurements = {{0, 1.0}, {24, 2.0}, {48, 3.0}};

    std::vector<CycleData> data = computeMultiCovariate(
            InterpolationType::Forward,
            {},
            InterpolationType::Forward,
            {},
            InterpolationType::Backward,
            datedMeasurements(measurements),
            InterpolationType::Forward,
            {},
            intervalHours,
            nbrDoses);

    expectCovariate(data, "covR", "TestR", InterpolationType::Backward, measurements, 24.0, nbrDoses);
}
