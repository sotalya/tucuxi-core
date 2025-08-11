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
#include "tucucore/intakeintervalcalculator.h"
#include "tucucore/multiconcentrationcalculator.h"
#include "tucucore/multiconcentrationprediction.h"
#include "tucucore/pkmodels/onecompartmentbolus.h"
#include "tucucore/pkmodels/onecompartmentextra.h"
#include "tucucore/pkmodels/onecompartmentinfusion.h"
#include "tucucore/pkmodels/threecompartmentbolus.h"
#include "tucucore/pkmodels/threecompartmentextra.h"
#include "tucucore/pkmodels/threecompartmentinfusion.h"
#include "tucucore/pkmodels/twocompartmentbolus.h"
#include "tucucore/pkmodels/twocompartmentextra.h"
#include "tucucore/pkmodels/twocompartmentinfusion.h"

#include "gtest_core.h"
#include "pkmodels/constanteliminationbolus.h"
#include "pkmodels/multiconstanteliminationbolus.h"

using namespace Tucuxi::Core;

template<class CalculatorClass>
static void testMultiConcentrationCalculator(
        const Tucuxi::Core::ParameterSetSeries& _parameters,
        double _dose,
        Tucuxi::Core::AbsorptionModel _route,
        std::chrono::hours _interval,
        std::chrono::seconds _infusionTime,
        CycleSize _nbPoints)
{

    // Just a reminder that this test is not yet written
    //ASSERT_TRUE(false);
    // The following 6 lines should be removed when implementing the function


    // Compare the result on one interval
    // with ConcentrationCalculator vs directly with the IntakeIntervalCalculator
    {
        Tucuxi::Core::ComputingStatus res;
        std::shared_ptr<IntakeIntervalCalculator> calculator = std::make_shared<CalculatorClass>();

        DateTime now = DateTime::now();
        Tucuxi::Common::Duration offsetTime = 0s;
        Tucuxi::Common::Duration interval = _interval;
        Tucuxi::Common::Duration infusionTime = _infusionTime;

        unsigned int residualSize = calculator->getResidualSize();
        bool isAll = false;

        MultiCompConcentrations concentrations;
        concentrations.resize(residualSize);

        Tucuxi::Core::TimeOffsets times;
        Tucuxi::Core::IntakeEvent intakeEvent(
                now,
                offsetTime,
                _dose,
                TucuUnit("mg"),
                interval,
                Tucuxi::Core::FormulationAndRoute(),
                infusionTime,
                _nbPoints);

        // std::cout << typeid(calculator).name() << std::endl;

        {
            Tucuxi::Core::Residuals inResiduals(residualSize);
            Tucuxi::Core::Residuals outResiduals(residualSize);

            std::fill(inResiduals.begin(), inResiduals.end(), 0);

            Tucuxi::Core::ParameterSetEvent event = *(_parameters.getAtTime(now));
            res = calculator->calculateIntakePoints(
                    concentrations, times, intakeEvent, event, inResiduals, isAll, outResiduals, true);

            ASSERT_EQ(res, Tucuxi::Core::ComputingStatus::Ok);
        }

        Tucuxi::Core::MultiConcentrationPredictionPtr predictionPtr;
        {
            predictionPtr = std::make_unique<Tucuxi::Core::MultiConcentrationPrediction>();

            DateTime recordFrom = now;
            DateTime recordTo = now + intakeEvent.getInterval();

            Tucuxi::Core::IntakeSeries intakeSeries;
            std::shared_ptr<IntakeIntervalCalculator> calculator2 = std::make_shared<CalculatorClass>();
            intakeEvent.setCalculator(calculator2);
            intakeSeries.push_back(intakeEvent);
            Tucuxi::Core::IMultiConcentrationCalculator* concentrationCalculator =
                    new Tucuxi::Core::MultiConcentrationCalculator();
            auto status = concentrationCalculator->computeConcentrations(
                    predictionPtr, isAll, recordFrom, recordTo, intakeSeries, _parameters);
            delete concentrationCalculator;
            ASSERT_EQ(status, ComputingStatus::Ok);
        }

        //int analytes = getNbAnalytes();
        for (size_t i = 0; i < _nbPoints; i++) {
            MultiCompConcentrations concentration2 = predictionPtr->getValues()[0];
            // std::cout << i <<  " :: " << concentrations[0][i] << " : " << concentration2[i] << std::endl;
            // compare concentrations of compartment 1
            ASSERT_DOUBLE_EQ(concentrations[0][i], concentration2[0][i]);
        }
    }


    // Use the ConcentrationCalculator in order to calculate multiple cycles
    // and compare it with the sum of individual calculations made with the
    // IntakeIntervalCalculators. Be careful, the interval for the later need
    // to be longer, and the number of points modified accordingly
    //
    // With the new calculation of pertinent times for infusion, this test fails.
    // The test should behave differently in case of infusion
    if (_route != AbsorptionModel::Infusion) {
        size_t nbCycles = 10;

        Tucuxi::Core::ComputingStatus res;
        CalculatorClass calculator;

        DateTime now = DateTime::now();
        Tucuxi::Common::Duration offsetTime = 0s;
        Tucuxi::Common::Duration interval = _interval;
        Tucuxi::Common::Duration infusionTime = _infusionTime;

        unsigned int residualSize = calculator.getResidualSize();
        bool isAll = false;
        MultiCompConcentrations concentrations;
        concentrations.resize(residualSize);
        Tucuxi::Core::TimeOffsets times;
        {
            // Be careful, the intakeEvent embedds the nb of points, but the intervalintakecalculator also. They have to agree

            Tucuxi::Core::IntakeEvent intakeEvent(
                    now,
                    offsetTime,
                    _dose,
                    TucuUnit("mg"),
                    interval * nbCycles,
                    Tucuxi::Core::FormulationAndRoute(),
                    infusionTime,
                    (_nbPoints - 1) * nbCycles + 1);

            Tucuxi::Core::Residuals inResiduals(residualSize);
            Tucuxi::Core::Residuals outResiduals(residualSize);

            std::fill(inResiduals.begin(), inResiduals.end(), 0);

            Tucuxi::Core::ParameterSetEvent event = *(_parameters.getAtTime(now));
            res = calculator.calculateIntakePoints(
                    concentrations, times, intakeEvent, event, inResiduals, isAll, outResiduals, true);

#if GTEST_VERBOSE
            for (int testPoint = 0; testPoint < (_nbPoints - 1) * nbCycles + 1; testPoint++) {
                std::cout << "concentration[" << testPoint << "]: " << concentrations[0][testPoint] << std::endl;
            }
#endif

            ASSERT_EQ(res, Tucuxi::Core::ComputingStatus::Ok);
        }

        Tucuxi::Core::MultiConcentrationPredictionPtr predictionPtr;
        {
            predictionPtr = std::make_unique<Tucuxi::Core::MultiConcentrationPrediction>();

            Tucuxi::Core::IntakeSeries intakeSeries;
            std::shared_ptr<IntakeIntervalCalculator> calculator2 = std::make_shared<CalculatorClass>();

            DateTime recordFrom = now;
            DateTime recordTo = now + interval * nbCycles;

            for (size_t i = 0; i < nbCycles; i++) {
                Tucuxi::Core::IntakeEvent event(
                        now + interval * i,
                        offsetTime,
                        _dose,
                        TucuUnit("mg"),
                        interval,
                        Tucuxi::Core::FormulationAndRoute(),
                        infusionTime,
                        _nbPoints);
                event.setCalculator(calculator2);
                intakeSeries.push_back(event);
            }
            Tucuxi::Core::IMultiConcentrationCalculator* concentrationCalculator =
                    new Tucuxi::Core::MultiConcentrationCalculator();
            auto status = concentrationCalculator->computeConcentrations(
                    predictionPtr, isAll, recordFrom, recordTo, intakeSeries, _parameters);
            delete concentrationCalculator;
            ASSERT_EQ(status, ComputingStatus::Ok);

#if GTEST_VERBOSE
            for (int testCycle = 0; testCycle < nbCycles; testCycle++) {
                for (int testNbPoint = 0; testNbPoint < _nbPoints; testNbPoint++) {
                    std::cout << "concentration[" << testCycle << "]"
                              << "[" << testNbPoint << "]"
                              << ": " << predictionPtr->getValues()[testCycle][testNbPoint] << std::endl;
                }
            }
#endif
        }

        // Only works for linear elimination, so do not perform that for some classes
        if (!(typeid(CalculatorClass) == typeid(ConstantEliminationBolus)
              || typeid(CalculatorClass) == typeid(MultiConstantEliminationBolus))) {
            for (size_t cycle = 0; cycle < nbCycles; cycle++) {
                MultiCompConcentrations concentration2 = predictionPtr->getValues()[cycle];
                for (CycleSize i = 0; i < _nbPoints - 1; i++) {
                    double sumConcentration = 0.0;
                    for (size_t c = 0; c < cycle + 1; c++) {
                        sumConcentration += concentrations[0][c * (_nbPoints - 1) + i];
                        // std::cout << c <<  " : " << sumConcentration << " : " << concentrations[0][c * (_nbPoints - 1) + i] << std::endl;
                    }
                    // std::cout << cycle <<  " : " << i << " :: " << predictionPtr->getTimes()[cycle][i] << " . " << sumConcentration << " : " << concentration2[i] << std::endl;
                    ASSERT_PRED4(
                            double_eq_rel_abs,
                            sumConcentration,
                            concentration2[0][i],
                            DEFAULT_PRECISION,
                            DEFAULT_PRECISION);
                }
            }
        }
    }

    // Create 2 samples and compare the result of computeConcentrations() and pointsAtTime().
    //
    // This test fails for infusion. It should be redesigned according to non linear times
    if (_route != AbsorptionModel::Infusion) {
        CalculatorClass calculator;

        CycleSize nbPoints = 201;
        bool isAll = false;

        DateTime now = DateTime::now();
        Tucuxi::Common::Duration offsetTime = 0s;
        Tucuxi::Common::Duration interval = _interval;
        Tucuxi::Common::Duration infusionTime = _infusionTime;


        MultiCompConcentrations concentrations;
        Tucuxi::Core::TimeOffsets times;
        Tucuxi::Core::IntakeEvent intakeEvent(
                now,
                offsetTime,
                _dose,
                TucuUnit("mg"),
                interval,
                Tucuxi::Core::FormulationAndRoute(),
                infusionTime,
                nbPoints);

        Tucuxi::Core::MultiConcentrationPredictionPtr predictionPtr;
        {
            predictionPtr = std::make_unique<Tucuxi::Core::MultiConcentrationPrediction>();

            DateTime recordFrom = now;
            DateTime recordTo = recordFrom + interval;

            Tucuxi::Core::IntakeSeries intakeSeries;
            std::shared_ptr<IntakeIntervalCalculator> calculator2 = std::make_shared<CalculatorClass>();
            intakeEvent.setCalculator(calculator2);
            intakeSeries.push_back(intakeEvent);
            Tucuxi::Core::IMultiConcentrationCalculator* concentrationCalculator =
                    new Tucuxi::Core::MultiConcentrationCalculator();
            auto status = concentrationCalculator->computeConcentrations(
                    predictionPtr, isAll, recordFrom, recordTo, intakeSeries, _parameters);
            delete concentrationCalculator;
            ASSERT_EQ(status, ComputingStatus::Ok);

#if GTEST_VERBOSE
            for (int i = 0; i < nbPoints; i++) {
                std::cout << i << ":" << predictionPtr->getValues()[0][i] << std::endl;
            }
#endif
        }
        {
            Tucuxi::Core::IntakeSeries intakeSeries;
            std::shared_ptr<IntakeIntervalCalculator> calculator2 = std::make_shared<CalculatorClass>();
            intakeEvent.setCalculator(calculator2);
            intakeSeries.push_back(intakeEvent);

            Tucuxi::Core::SampleSeries sampleSeries;
            DateTime date0 = now + interval / 4.0;

            Tucuxi::Core::SampleEvent s0(date0);
            sampleSeries.push_back(s0);
            DateTime date1 = now + interval * 3.0 / 4.0;

            Tucuxi::Core::SampleEvent s1(date1);
            sampleSeries.push_back(s1);

            Tucuxi::Core::IMultiConcentrationCalculator* concentrationCalculator =
                    new Tucuxi::Core::MultiConcentrationCalculator();
            ComputingStatus res = concentrationCalculator->computeConcentrationsAtTimes(
                    concentrations, isAll, intakeSeries, _parameters, sampleSeries);

            ASSERT_EQ(res, ComputingStatus::Ok);

            delete concentrationCalculator;
        }

        size_t n0 = (nbPoints - 1) / 4;
        size_t n1 = (nbPoints - 1) * 3 / 4;
        std::shared_ptr<IntakeIntervalCalculator> calculator2 = std::make_shared<CalculatorClass>();

        //asser that the vector size is the one we expect
        ASSERT_EQ(concentrations.size(), static_cast<size_t>(2));
        ASSERT_EQ(concentrations[0].size(), static_cast<size_t>(calculator2->getNbAnalytes()));
        ASSERT_EQ(concentrations[0].size(), static_cast<size_t>(calculator2->getNbAnalytes()));

        // compare the result of compartment1
        for (size_t i = 0; i < calculator2->getNbAnalytes(); i++) {
            const auto values0 = predictionPtr->getValues();
            ASSERT_DOUBLE_EQ(concentrations[0][i], predictionPtr->getValues()[0][i][n0]);
            ASSERT_DOUBLE_EQ(concentrations[1][i], predictionPtr->getValues()[0][i][n1]);
        }
    }

    // Create samples and compare the result of computeConcentrations() and pointsAtTime().
    // This can be done by creating samples corresponding to the number of points asked, and
    // synchronized with the times at which the concentration points are expected
}

TEST(Core_TestMultiConcentrationCalculator, ConstantEliminationBolus)
{
    Tucuxi::Core::ParameterDefinitions parameterDefs;
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestA", 1.0, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestR", 2.0, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestS", 0.03, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestM", 4.0, Tucuxi::Core::ParameterVariabilityType::None));
    Tucuxi::Core::ParameterSetEvent parameters(DateTime::now(), parameterDefs);
    Tucuxi::Core::ParameterSetSeries parametersSeries;
    parametersSeries.addParameterSetEvent(parameters);

    testMultiConcentrationCalculator<Tucuxi::Core::ConstantEliminationBolus>(
            parametersSeries, 400.0, Tucuxi::Core::AbsorptionModel::Extravascular, 12h, 0s, CYCLE_SIZE);
}

TEST(Core_TestMultiConcentrationCalculator, oneCompBolus)
{
    Tucuxi::Core::ParameterDefinitions parameterDefs;
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "V", 347, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "Ke", 22.97, Tucuxi::Core::ParameterVariabilityType::None));
    Tucuxi::Core::ParameterSetEvent parameters(DateTime::now(), parameterDefs);
    Tucuxi::Core::ParameterSetSeries parametersSeries;
    parametersSeries.addParameterSetEvent(parameters);

    testMultiConcentrationCalculator<Tucuxi::Core::OneCompartmentBolusMicro>(
            parametersSeries, 400.0, Tucuxi::Core::AbsorptionModel::Intravascular, 12h, 0s, CYCLE_SIZE);
}

TEST(Core_TestMultiConcentrationCalculator, oneCompExtra)
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

    testMultiConcentrationCalculator<Tucuxi::Core::OneCompartmentExtraMicro>(
            parametersSeries, 400.0, Tucuxi::Core::AbsorptionModel::Extravascular, 12h, 0s, CYCLE_SIZE);
}

TEST(Core_TestMultiConcentrationCalculator, oneCompInfusion)
{
    Tucuxi::Core::ParameterDefinitions parameterDefs;
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "V", 347, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "Ke", 22.97, Tucuxi::Core::ParameterVariabilityType::None));
    Tucuxi::Core::ParameterSetEvent parameters(DateTime::now(), parameterDefs);
    Tucuxi::Core::ParameterSetSeries parametersSeries;
    parametersSeries.addParameterSetEvent(parameters);

    testMultiConcentrationCalculator<Tucuxi::Core::OneCompartmentInfusionMicro>(
            parametersSeries, 400.0, Tucuxi::Core::AbsorptionModel::Infusion, 12h, 1h, CYCLE_SIZE);
}

TEST(Core_TestMultiConcentrationCalculator, twoCompBolus)
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

    testMultiConcentrationCalculator<Tucuxi::Core::TwoCompartmentBolusMicro>(
            parametersSeries, 400.0, Tucuxi::Core::AbsorptionModel::Intravascular, 12h, 0s, CYCLE_SIZE);
}

TEST(Core_TestMultiConcentrationCalculator, twoCompExtra)
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

    testMultiConcentrationCalculator<Tucuxi::Core::TwoCompartmentExtraMicro>(
            parametersSeries, 400.0, Tucuxi::Core::AbsorptionModel::Extravascular, 12h, 0s, CYCLE_SIZE);
}

TEST(Core_TestMultiConcentrationCalculator, twoCompInfusion)
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

    testMultiConcentrationCalculator<Tucuxi::Core::TwoCompartmentInfusionMicro>(
            parametersSeries, 400.0, Tucuxi::Core::AbsorptionModel::Infusion, 12h, 1h, CYCLE_SIZE);
}

TEST(Core_TestMultiConcentrationCalculator, MultiConstantEliminationBolus)
{
    Tucuxi::Core::ParameterDefinitions parameterDefs;
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestA0", 1.0, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestR0", 2.0, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestS0", 0.03, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestM0", 4.0, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestA1", 10.0, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestR1", 20.0, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestS1", 0.003, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestM1", 40.0, Tucuxi::Core::ParameterVariabilityType::None));
    Tucuxi::Core::ParameterSetEvent parameters(DateTime::now(), parameterDefs);
    Tucuxi::Core::ParameterSetSeries parametersSeries;
    parametersSeries.addParameterSetEvent(parameters);

    testMultiConcentrationCalculator<Tucuxi::Core::MultiConstantEliminationBolus>(
            parametersSeries, 400.0, Tucuxi::Core::AbsorptionModel::Extravascular, 12h, 0s, CYCLE_SIZE);
}

TEST(Core_TestMultiConcentrationCalculator, DISABLED_threeCompBolus)
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

    testMultiConcentrationCalculator<Tucuxi::Core::ThreeCompartmentBolusMicro>(
            parametersSeries, 400.0, Tucuxi::Core::AbsorptionModel::Intravascular, 12h, 0s, CYCLE_SIZE);
}

TEST(Core_TestMultiConcentrationCalculator, DISABLED_threeCompExtra)
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

    testMultiConcentrationCalculator<Tucuxi::Core::ThreeCompartmentExtraMicro>(
            parametersSeries, 400.0, Tucuxi::Core::AbsorptionModel::Extravascular, 12h, 0s, CYCLE_SIZE);
}

TEST(Core_TestMultiConcentrationCalculator, DISABLED_threeCompInfusion)
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

    testMultiConcentrationCalculator<Tucuxi::Core::ThreeCompartmentInfusionMicro>(
            parametersSeries, 400.0, Tucuxi::Core::AbsorptionModel::Infusion, 12h, 1h, CYCLE_SIZE);
}
