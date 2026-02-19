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

#include "tucucore/dosage.h"
#include "tucucore/intakeextractor.h"
#include "tucucore/pkmodels/onecompartmentbolus.h"
#include "tucucore/pkmodels/onecompartmentextra.h"
#include "tucucore/pkmodels/onecompartmentinfusion.h"
// #include "tucucore/pkmodels/threecompartmentbolus.h"
#include "tucucore/pkmodels/rkthreecompartment.h"
#include "tucucore/pkmodels/threecompartmentextra.h"
#include "tucucore/pkmodels/threecompartmentinfusion.h"
#include "tucucore/pkmodels/twocompartmentbolus.h"
#include "tucucore/pkmodels/twocompartmentextra.h"
#include "tucucore/pkmodels/twocompartmentinfusion.h"

#include "gtest_core.h"
#include "mocklogger.h"
#include "pkmodels/constanteliminationbolus.h"

using namespace Tucuxi::Core;

TEST(Core_TestConcentrationCalculator, ConstantEliminationBolus)
{
    Tucuxi::Core::ParameterDefinitions parameterDefs;
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestA", 0.0, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestR", 0.0, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestS", 0.0, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestM", 1.0, Tucuxi::Core::ParameterVariabilityType::None));
    Tucuxi::Core::ParameterSetEvent parameters(DateTime::now(), parameterDefs);
    Tucuxi::Core::ParameterSetSeries parametersSeries;
    parametersSeries.addParameterSetEvent(parameters);

    testCalculator<Tucuxi::Core::ConstantEliminationBolus>(
            parametersSeries, 400.0, Tucuxi::Core::AbsorptionModel::Extravascular, 12h, 0s, CYCLE_SIZE);
}

TEST(Core_TestConcentrationCalculator, oneCompBolus)
{
    Tucuxi::Core::ParameterDefinitions parameterDefs;
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "V", 347, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "Ke", 22.97, Tucuxi::Core::ParameterVariabilityType::None));
    Tucuxi::Core::ParameterSetEvent parameters(DateTime::now(), parameterDefs);
    Tucuxi::Core::ParameterSetSeries parametersSeries;
    parametersSeries.addParameterSetEvent(parameters);

    testCalculator<Tucuxi::Core::OneCompartmentBolusMicro>(
            parametersSeries, 400.0, Tucuxi::Core::AbsorptionModel::Intravascular, 12h, 0s, CYCLE_SIZE);
}

TEST(Core_TestConcentrationCalculator, oneCompExtra)
{
    Tucuxi::Core::ParameterDefinitions parameterDefs;
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "V", 347, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "Ke", 0.0435331, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "Ka", 0.609, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(
            std::make_unique<Tucuxi::Core::ParameterDefinition>("F", 1, Tucuxi::Core::ParameterVariabilityType::None));
    Tucuxi::Core::ParameterSetEvent parameters(DateTime::now(), parameterDefs);
    Tucuxi::Core::ParameterSetSeries parametersSeries;
    parametersSeries.addParameterSetEvent(parameters);

    testCalculator<Tucuxi::Core::OneCompartmentExtraMicro>(
            parametersSeries, 400.0, Tucuxi::Core::AbsorptionModel::Extravascular, 12h, 0s, CYCLE_SIZE);
}

TEST(Core_TestConcentrationCalculator, oneCompInfusion)
{
    Tucuxi::Core::ParameterDefinitions parameterDefs;
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "V", 347, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "Ke", 22.97, Tucuxi::Core::ParameterVariabilityType::None));
    Tucuxi::Core::ParameterSetEvent parameters(DateTime::now(), parameterDefs);
    Tucuxi::Core::ParameterSetSeries parametersSeries;
    parametersSeries.addParameterSetEvent(parameters);

    testCalculator<Tucuxi::Core::OneCompartmentInfusionMicro>(
            parametersSeries, 400.0, Tucuxi::Core::AbsorptionModel::Infusion, 12h, 1h, CYCLE_SIZE);
}

TEST(Core_TestConcentrationCalculator, twoCompBolus)
{
    Tucuxi::Core::ParameterDefinitions parameterDefs;
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "V1", 340, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "Ke", 0.0444294, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "K12", 0.0588235, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "K21", 0.0584795, Tucuxi::Core::ParameterVariabilityType::None));
    Tucuxi::Core::ParameterSetEvent parameters(DateTime::now(), parameterDefs);
    Tucuxi::Core::ParameterSetSeries parametersSeries;
    parametersSeries.addParameterSetEvent(parameters);

    testCalculator<Tucuxi::Core::TwoCompartmentBolusMicro>(
            parametersSeries, 400.0, Tucuxi::Core::AbsorptionModel::Intravascular, 12h, 0s, CYCLE_SIZE);
}

TEST(Core_TestConcentrationCalculator, twoCompExtra)
{
    Tucuxi::Core::ParameterDefinitions parameterDefs;
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "V1", 340, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "Ke", 0.0444294, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "K12", 0.0588235, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "K21", 0.0584795, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "Ka", 0.609, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(
            std::make_unique<Tucuxi::Core::ParameterDefinition>("F", 1, Tucuxi::Core::ParameterVariabilityType::None));
    Tucuxi::Core::ParameterSetEvent parameters(DateTime::now(), parameterDefs);
    Tucuxi::Core::ParameterSetSeries parametersSeries;
    parametersSeries.addParameterSetEvent(parameters);

    testCalculator<Tucuxi::Core::TwoCompartmentExtraMicro>(
            parametersSeries, 400.0, Tucuxi::Core::AbsorptionModel::Extravascular, 12h, 0s, CYCLE_SIZE);
}

TEST(Core_TestConcentrationCalculator, twoCompInfusion)
{
    Tucuxi::Core::ParameterDefinitions parameterDefs;
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "V1", 340, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "Ke", 0.0444294, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "K12", 0.0588235, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "K21", 0.0584795, Tucuxi::Core::ParameterVariabilityType::None));
    Tucuxi::Core::ParameterSetEvent parameters(DateTime::now(), parameterDefs);
    Tucuxi::Core::ParameterSetSeries parametersSeries;
    parametersSeries.addParameterSetEvent(parameters);

    testCalculator<Tucuxi::Core::TwoCompartmentInfusionMicro>(
            parametersSeries, 400.0, Tucuxi::Core::AbsorptionModel::Infusion, 12h, 1h, CYCLE_SIZE);
}

// TODO Active following test after fixing input parameters
TEST(Core_TestConcentrationCalculator, threeCompBolus)
{
    Tucuxi::Core::ParameterDefinitions parameterDefs;
    //parameterDefs.push_back(
    //        std::make_unique<Tucuxi::Core::ParameterDefinition>("F", 2, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "V1", 340, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "Ke", 0.0444294, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "K12", 0.0588235, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "K21", 0.0584795, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "K13", 0.0882353, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "K31", 0.0877193, Tucuxi::Core::ParameterVariabilityType::None));
    Tucuxi::Core::ParameterSetEvent parameters(DateTime::now(), parameterDefs);
    Tucuxi::Core::ParameterSetSeries parametersSeries;
    parametersSeries.addParameterSetEvent(parameters);

    testCalculator<Tucuxi::Core::RkThreeCompartmentBolusMicro>(
            parametersSeries, 400.0, Tucuxi::Core::AbsorptionModel::Intravascular, 12h, 0s, CYCLE_SIZE);
}

TEST(Core_TestConcentrationCalculator, threeCompExtra)
{
    Tucuxi::Core::ParameterDefinitions parameterDefs;
    parameterDefs.push_back(
            std::make_unique<Tucuxi::Core::ParameterDefinition>("F", 2, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "V1", 340, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "Ka", 0.609, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "Ke", 0.0444294, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "K12", 0.0588235, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "K21", 0.0584795, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "K13", 0.0882353, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "K31", 0.0877193, Tucuxi::Core::ParameterVariabilityType::None));
    Tucuxi::Core::ParameterSetEvent parameters(DateTime::now(), parameterDefs);
    Tucuxi::Core::ParameterSetSeries parametersSeries;
    parametersSeries.addParameterSetEvent(parameters);

    testCalculator<Tucuxi::Core::RkThreeCompartmentExtraMicro>(
            parametersSeries, 400.0, Tucuxi::Core::AbsorptionModel::Extravascular, 12h, 0s, CYCLE_SIZE);
}

TEST(Core_TestConcentrationCalculator, threeCompInfusion)
{
    Tucuxi::Core::ParameterDefinitions parameterDefs;
    parameterDefs.push_back(
            std::make_unique<Tucuxi::Core::ParameterDefinition>("F", 2, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "V1", 340, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "Ke", 0.0444294, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "K12", 0.0588235, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "K21", 0.0584795, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "K13", 0.0882353, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "K31", 0.0877193, Tucuxi::Core::ParameterVariabilityType::None));
    Tucuxi::Core::ParameterSetEvent parameters(DateTime::now(), parameterDefs);
    Tucuxi::Core::ParameterSetSeries parametersSeries;
    parametersSeries.addParameterSetEvent(parameters);

    testCalculator<Tucuxi::Core::RkThreeCompartmentInfusionMicro>(
            parametersSeries, 400.0, Tucuxi::Core::AbsorptionModel::Infusion, 12h, 1h, CYCLE_SIZE);
}


// ===========================================================================
// Helper: build a one-intake series with a ConstantEliminationBolus calculator
// ===========================================================================
static IntakeSeries buildOneIntakeSeries(
        const DateTime& _time, double _dose, std::chrono::hours _interval, CycleSize _nbPoints)
{
    IntakeSeries series;
    IntakeEvent ev(
            _time,
            0s,
            _dose,
            TucuUnit("mg"),
            Tucuxi::Common::Duration(_interval),
            FormulationAndRoute(),
            0s,
            _nbPoints);
    ev.setCalculator(std::make_shared<ConstantEliminationBolus>());
    series.push_back(ev);
    return series;
}

// ===========================================================================
// Helper: build a ParameterSetSeries for ConstantEliminationBolus
// ===========================================================================
static ParameterSetSeries buildConstantEliminationParams(const DateTime& _time, double _s = 0.0, double _r = 0.0)
{
    ParameterDefinitions defs;
    defs.push_back(std::make_unique<ParameterDefinition>("TestA", 0.0, ParameterVariabilityType::None));
    defs.push_back(std::make_unique<ParameterDefinition>("TestR", _r, ParameterVariabilityType::None));
    defs.push_back(std::make_unique<ParameterDefinition>("TestS", _s, ParameterVariabilityType::None));
    defs.push_back(std::make_unique<ParameterDefinition>("TestM", 1.0, ParameterVariabilityType::None));
    ParameterSetEvent event(_time, defs);
    ParameterSetSeries series;
    series.addParameterSetEvent(event);
    return series;
}


// ===========================================================================
// computeConcentrations: undefined recordFrom triggers error logging
// ===========================================================================
TEST(Core_TestConcentrationCalculator, UndefinedRecordFrom)
{
    Tucuxi::Common::MockLogger mockLogger;
    mockLogger.install();

    DateTime now = DateTime::now();
    auto params = buildConstantEliminationParams(now);
    auto intakes = buildOneIntakeSeries(now, 400.0, 12h, 10);
    auto prediction = std::make_unique<ConcentrationPrediction>();

    ConcentrationCalculator calc;
    auto status = calc.computeConcentrations(
            prediction, false, DateTime(), now + Tucuxi::Common::Duration(12h), intakes, params);
    // The function logs an error but continues; computation still succeeds.
    EXPECT_EQ(status, ComputingStatus::Ok);
    EXPECT_TRUE(mockLogger.hasEntry(Tucuxi::Common::LogLevel::Error, "Invalid record from"));

    mockLogger.uninstall();
}


// ===========================================================================
// computeConcentrations: undefined recordTo triggers error logging
// ===========================================================================
TEST(Core_TestConcentrationCalculator, UndefinedRecordTo)
{
    Tucuxi::Common::MockLogger mockLogger;
    mockLogger.install();

    DateTime now = DateTime::now();
    auto params = buildConstantEliminationParams(now);
    auto intakes = buildOneIntakeSeries(now, 400.0, 12h, 10);
    auto prediction = std::make_unique<ConcentrationPrediction>();

    ConcentrationCalculator calc;
    auto status = calc.computeConcentrations(prediction, false, now, DateTime(), intakes, params);
    // The function logs an error but continues; computation still succeeds.
    EXPECT_EQ(status, ComputingStatus::Ok);
    EXPECT_TRUE(mockLogger.hasEntry(Tucuxi::Common::LogLevel::Error, "Invalid record to"));

    mockLogger.uninstall();
}


// ===========================================================================
// computeConcentrations: empty ParameterSetSeries returns NoParameters
// ===========================================================================
TEST(Core_TestConcentrationCalculator, NoParametersComputeConcentrations)
{
    Tucuxi::Common::MockLogger mockLogger;
    mockLogger.install();

    DateTime now = DateTime::now();
    ParameterSetSeries emptyParams;
    auto intakes = buildOneIntakeSeries(now, 400.0, 12h, 10);
    auto prediction = std::make_unique<ConcentrationPrediction>();

    ConcentrationCalculator calc;
    auto status = calc.computeConcentrations(
            prediction, false, now, now + Tucuxi::Common::Duration(12h), intakes, emptyParams);
    EXPECT_EQ(status, ComputingStatus::ConcentrationCalculatorNoParameters);
    EXPECT_TRUE(mockLogger.hasEntry(Tucuxi::Common::LogLevel::Error, "No parameters found"));

    mockLogger.uninstall();
}


// ===========================================================================
// computeConcentrationsAtSteadyState: undefined recordFrom triggers error logging
// ===========================================================================
TEST(Core_TestConcentrationCalculator, UndefinedRecordFromSteadyState)
{
    Tucuxi::Common::MockLogger mockLogger;
    mockLogger.install();

    DateTime now = DateTime::now();
    auto params = buildConstantEliminationParams(now);
    auto intakes = buildOneIntakeSeries(now, 400.0, 12h, 10);
    auto prediction = std::make_unique<ConcentrationPrediction>();

    ConcentrationCalculator calc;
    auto status = calc.computeConcentrationsAtSteadyState(
            prediction, false, DateTime(), now + Tucuxi::Common::Duration(12h), intakes, params);
    EXPECT_EQ(status, ComputingStatus::Ok);
    EXPECT_TRUE(mockLogger.hasEntry(Tucuxi::Common::LogLevel::Error, "Invalid record from"));

    mockLogger.uninstall();
}


// ===========================================================================
// computeConcentrationsAtSteadyState: undefined recordTo triggers error logging
// ===========================================================================
TEST(Core_TestConcentrationCalculator, UndefinedRecordToSteadyState)
{
    Tucuxi::Common::MockLogger mockLogger;
    mockLogger.install();

    DateTime now = DateTime::now();
    auto params = buildConstantEliminationParams(now);
    auto intakes = buildOneIntakeSeries(now, 400.0, 12h, 10);
    auto prediction = std::make_unique<ConcentrationPrediction>();

    ConcentrationCalculator calc;
    auto status = calc.computeConcentrationsAtSteadyState(prediction, false, now, DateTime(), intakes, params);
    EXPECT_EQ(status, ComputingStatus::Ok);
    EXPECT_TRUE(mockLogger.hasEntry(Tucuxi::Common::LogLevel::Error, "Invalid record to"));

    mockLogger.uninstall();
}


// ===========================================================================
// computeConcentrationsAtSteadyState: empty ParameterSetSeries returns NoParameters
// ===========================================================================
TEST(Core_TestConcentrationCalculator, NoParametersSteadyState)
{
    Tucuxi::Common::MockLogger mockLogger;
    mockLogger.install();

    DateTime now = DateTime::now();
    ParameterSetSeries emptyParams;
    auto intakes = buildOneIntakeSeries(now, 400.0, 12h, 10);
    auto prediction = std::make_unique<ConcentrationPrediction>();

    ConcentrationCalculator calc;
    auto status = calc.computeConcentrationsAtSteadyState(
            prediction, false, now, now + Tucuxi::Common::Duration(12h), intakes, emptyParams);
    EXPECT_EQ(status, ComputingStatus::ConcentrationCalculatorNoParameters);
    EXPECT_TRUE(mockLogger.hasEntry(Tucuxi::Common::LogLevel::Error, "No parameters found"));

    mockLogger.uninstall();
}


// ===========================================================================
// computeConcentrationsAtSteadyState: calculation error in the recording phase.
// The first intake uses dose=0 so that outResidual=0=inResidual, achieving
// immediate convergence (reachedSteadyState=true). The second intake then
// enters the recording branch and fails because its parameters have S=-1
// which triggers a checkInputs failure.
// ===========================================================================
TEST(Core_TestConcentrationCalculator, SteadyStateErrorRecordingPhase)
{
    DateTime now = DateTime::now();
    Tucuxi::Common::Duration interval(12h);

    // First intake at 'now' -> valid parameters (S=0)
    // Second intake at 'now+12h' -> invalid parameters (S=-1)
    ParameterSetSeries params;
    {
        ParameterDefinitions defs;
        defs.push_back(std::make_unique<ParameterDefinition>("TestA", 0.0, ParameterVariabilityType::None));
        defs.push_back(std::make_unique<ParameterDefinition>("TestR", 0.0, ParameterVariabilityType::None));
        defs.push_back(std::make_unique<ParameterDefinition>("TestS", 0.0, ParameterVariabilityType::None));
        defs.push_back(std::make_unique<ParameterDefinition>("TestM", 1.0, ParameterVariabilityType::None));
        params.addParameterSetEvent(ParameterSetEvent(now, defs));
    }
    {
        ParameterDefinitions defs;
        defs.push_back(std::make_unique<ParameterDefinition>("TestA", 0.0, ParameterVariabilityType::None));
        defs.push_back(std::make_unique<ParameterDefinition>("TestR", 0.0, ParameterVariabilityType::None));
        defs.push_back(std::make_unique<ParameterDefinition>("TestS", -1.0, ParameterVariabilityType::None));
        defs.push_back(std::make_unique<ParameterDefinition>("TestM", 1.0, ParameterVariabilityType::None));
        params.addParameterSetEvent(ParameterSetEvent(now + interval, defs));
    }

    IntakeSeries intakes;
    {
        // Dose=0 so residuals stay at 0, matching the initial inResiduals → immediate convergence
        IntakeEvent ev1(now, 0s, 0.0, TucuUnit("mg"), interval, FormulationAndRoute(), 0s, 10);
        ev1.setCalculator(std::make_shared<ConstantEliminationBolus>());
        intakes.push_back(ev1);

        IntakeEvent ev2(now + interval, 0s, 100.0, TucuUnit("mg"), interval, FormulationAndRoute(), 0s, 10);
        ev2.setCalculator(std::make_shared<ConstantEliminationBolus>());
        intakes.push_back(ev2);
    }

    auto prediction = std::make_unique<ConcentrationPrediction>();
    ConcentrationCalculator calc;
    auto status =
            calc.computeConcentrationsAtSteadyState(prediction, false, now, now + interval + interval, intakes, params);
    EXPECT_EQ(status, ComputingStatus::BadParameters);
}


// ===========================================================================
// computeConcentrationsAtSteadyState: residual error model is applied
// after reaching steady state in the recording phase.
// ===========================================================================
TEST(Core_TestConcentrationCalculator, SteadyStateResidualErrorModel)
{
    DateTime now = DateTime::now();
    auto params = buildConstantEliminationParams(now);
    auto intakes = buildOneIntakeSeries(now, 400.0, 12h, 10);
    auto prediction = std::make_unique<ConcentrationPrediction>();

    SigmaResidualErrorModel errModel;
    Sigma sigma(1);
    sigma[0] = 0.1;
    errModel.setSigma(sigma);
    errModel.setErrorModel(ResidualErrorType::ADDITIVE);
    Deviations epsilons = {0.01};

    ConcentrationCalculator calc;
    auto status = calc.computeConcentrationsAtSteadyState(
            prediction, false, now, now + Tucuxi::Common::Duration(12h), intakes, params, Etas(0), errModel, epsilons);
    EXPECT_EQ(status, ComputingStatus::Ok);
}


// ===========================================================================
// computeConcentrationsAtSteadyState: calculation error in the non-recording
// phase (reachedSteadyState is false on the first iteration).
// ===========================================================================
TEST(Core_TestConcentrationCalculator, SteadyStateErrorNonRecordingPhase)
{
    DateTime now = DateTime::now();
    // S=-1 makes checkInputs fail, returning BadParameters on the very first intake
    auto params = buildConstantEliminationParams(now, -1.0);
    auto intakes = buildOneIntakeSeries(now, 400.0, 12h, 10);
    auto prediction = std::make_unique<ConcentrationPrediction>();

    ConcentrationCalculator calc;
    auto status = calc.computeConcentrationsAtSteadyState(
            prediction, false, now, now + Tucuxi::Common::Duration(12h), intakes, params);
    EXPECT_EQ(status, ComputingStatus::BadParameters);
}


// ===========================================================================
// computeConcentrationsAtSteadyState: residuals fail to converge after
// exceeding the maximum number of iterations (NoSteadyState).
// R=1 makes residuals grow monotonically without reaching a fixed point.
// ===========================================================================
TEST(Core_TestConcentrationCalculator, SteadyStateNoConvergence)
{
    DateTime now = DateTime::now();
    // R=1.0, S=0.00001: residuals grow but never converge within the iteration limit
    auto params = buildConstantEliminationParams(now, 0.00001, 1.0);
    auto intakes = buildOneIntakeSeries(now, 100.0, 12h, 3);
    auto prediction = std::make_unique<ConcentrationPrediction>();

    ConcentrationCalculator calc;
    auto status = calc.computeConcentrationsAtSteadyState(
            prediction, false, now, now + Tucuxi::Common::Duration(12h), intakes, params);
    EXPECT_EQ(status, ComputingStatus::NoSteadyState);
}


// ===========================================================================
// computeConcentrationsAtTimes: empty ParameterSetSeries returns NoParameters
// ===========================================================================
TEST(Core_TestConcentrationCalculator, NoParametersAtTimes)
{
    DateTime now = DateTime::now();
    ParameterSetSeries emptyParams;
    auto intakes = buildOneIntakeSeries(now, 400.0, 12h, 10);

    SampleSeries samples;
    samples.push_back(SampleEvent(now + Tucuxi::Common::Duration(6h)));

    Concentrations concentrations;
    ConcentrationCalculator calc;
    auto status = calc.computeConcentrationsAtTimes(concentrations, false, intakes, emptyParams, samples);
    EXPECT_EQ(status, ComputingStatus::ConcentrationCalculatorNoParameters);
}


// ===========================================================================
// computeConcentrationsAtTimes: calculateIntakeSinglePoint returns error
// when the sample is beyond the current intake cycle and the intake's
// single-point calculation fails due to bad parameters.
// ===========================================================================
TEST(Core_TestConcentrationCalculator, AtTimesCalculationError)
{
    DateTime now = DateTime::now();
    Tucuxi::Common::Duration interval(12h);

    // S=-1 makes checkInputs fail -> BadParameters
    auto params = buildConstantEliminationParams(now, -1.0);

    // Two intakes so the sample is within the overall range
    IntakeSeries intakes;
    {
        IntakeEvent ev1(now, 0s, 400.0, TucuUnit("mg"), interval, FormulationAndRoute(), 0s, 10);
        ev1.setCalculator(std::make_shared<ConstantEliminationBolus>());
        intakes.push_back(ev1);

        IntakeEvent ev2(now + interval, 0s, 400.0, TucuUnit("mg"), interval, FormulationAndRoute(), 0s, 10);
        ev2.setCalculator(std::make_shared<ConstantEliminationBolus>());
        intakes.push_back(ev2);
    }

    // Sample placed in the second intake's range so it is beyond the first intake's end.
    // nextSampleTime (T0+18h) > nextIntakeTime (T0+12h) for the first intake,
    // triggering calculateIntakeSinglePoint which will fail.
    SampleSeries samples;
    samples.push_back(SampleEvent(now + interval + Tucuxi::Common::Duration(6h)));

    Concentrations concentrations;
    ConcentrationCalculator calc;
    auto status = calc.computeConcentrationsAtTimes(concentrations, false, intakes, params, samples);
    EXPECT_EQ(status, ComputingStatus::BadParameters);
    EXPECT_TRUE(concentrations.empty());
}
