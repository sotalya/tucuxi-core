/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TEST_CONCENTRATIONCALCULATOR_H
#define TEST_CONCENTRATIONCALCULATOR_H

#include <iostream>
#include <memory>

#include "fructose/fructose.h"

#include "tucucommon/general.h"

#include "tucucore/dosage.h"
#include "tucucore/intakeextractor.h"
#include "tucucore/intakeintervalcalculator.h"
#include "tucucore/concentrationcalculator.h"
#include "tucucore/pkmodels/onecompartmentbolus.h"
#include "tucucore/pkmodels/onecompartmentinfusion.h"
#include "tucucore/pkmodels/onecompartmentextra.h"
#include "tucucore/pkmodels/twocompartmentbolus.h"
#include "tucucore/pkmodels/twocompartmentinfusion.h"
#include "tucucore/pkmodels/twocompartmentextra.h"
#include "tucucore/pkmodels/threecompartmentbolus.h"
#include "tucucore/pkmodels/threecompartmentinfusion.h"
#include "tucucore/pkmodels/threecompartmentextra.h"
#include "pkmodels/constanteliminationbolus.h"

using namespace Tucuxi::Core;

template<typename T>
std::ostream& operator<<(typename std::enable_if<std::is_enum<T>::value, std::ostream>::type& stream, const T& e)
{
    return stream << static_cast<typename std::underlying_type<T>::type>(e);
}

struct TestConcentrationCalculator : public fructose::test_base<TestConcentrationCalculator>
{
    static const int CYCLE_SIZE = 251;

    TestConcentrationCalculator() { }

    template<class CalculatorClass>
    void testCalculator(const Tucuxi::Core::ParameterSetSeries &_parameters,
                        double _dose,
                        Tucuxi::Core::AbsorptionModel _route,
                        std::chrono::hours _interval,
                        std::chrono::seconds _infusionTime,
                        int _nbPoints)
    {
        // Compare the result on one interval
        // with ConcentrationCalculator vs directly with the IntakeIntervalCalculator
        {
            Tucuxi::Core::IntakeIntervalCalculator::Result res;
            std::shared_ptr<IntakeIntervalCalculator> calculator = std::make_shared<CalculatorClass>();

            DateTime now;
            Tucuxi::Common::Duration offsetTime = 0s;
            Tucuxi::Common::Duration interval = _interval;
            Tucuxi::Common::Duration infusionTime = _infusionTime;

            unsigned int residualSize = calculator->getResidualSize();
            bool isAll = false;

            std::vector<Tucuxi::Core::Concentrations> concentrations;
            concentrations.resize(residualSize);

            Tucuxi::Core::TimeOffsets times;
            Tucuxi::Core::IntakeEvent intakeEvent(now, offsetTime, _dose, interval, _route, infusionTime, _nbPoints);

            // std::cout << typeid(calculator).name() << std::endl;

            {
                Tucuxi::Core::Residuals inResiduals(residualSize);
                Tucuxi::Core::Residuals outResiduals(residualSize);

                std::fill(inResiduals.begin(), inResiduals.end(), 0);

                Tucuxi::Core::ParameterSetEvent event = *(_parameters.getAtTime(now));
                res = calculator->calculateIntakePoints(
                    concentrations,
                    times,
                    intakeEvent,
                    event,
                    inResiduals,
                    isAll,
                    outResiduals,
                    true);

                fructose_assert(res == Tucuxi::Core::IntakeIntervalCalculator::Result::Ok);
            }

            Tucuxi::Core::ConcentrationPredictionPtr predictionPtr;
            {
                predictionPtr = std::make_unique<Tucuxi::Core::ConcentrationPrediction>();

                DateTime recordFrom = now;
                DateTime recordTo = now + intakeEvent.getInterval();

                Tucuxi::Core::IntakeSeries intakeSeries;
                std::shared_ptr<IntakeIntervalCalculator> calculator2 = std::make_shared<CalculatorClass>();
                intakeEvent.setCalculator(calculator2);
                intakeSeries.push_back(intakeEvent);
                Tucuxi::Core::IConcentrationCalculator *concentrationCalculator = new Tucuxi::Core::ConcentrationCalculator();
                concentrationCalculator->computeConcentrations(
                    predictionPtr,
                    isAll,
                    recordFrom,
                    recordTo,
                    intakeSeries,
                    _parameters);
                delete concentrationCalculator;
            }

            for (int i = 0; i < _nbPoints; i++) {
                Tucuxi::Core::Concentrations concentration2;
                concentration2 = predictionPtr->getValues()[0];
                // std::cout << i <<  " :: " << concentrations[0][i] << " : " << concentration2[i] << std::endl;
                // compare concentrations of compartment 1
                fructose_assert_double_eq(concentrations[0][i], concentration2[i]);
            }
        }


        // Use the ConcentrationCalculator in order to calculate multiple cycles
        // and compare it with the sum of individual calculations made with the
        // IntakeIntervalCalculators. Be careful, the interval for the later need
        // to be longer, and the number of points modified accordingly
        //
        // With the new calculation of pertinent times for infusion, this test fails.
        // The test should behave differently in case of infusion
        if (_route != AbsorptionModel::INFUSION) {
            int nbCycles = 10;

            Tucuxi::Core::IntakeIntervalCalculator::Result res;
            CalculatorClass calculator;

            DateTime now;
            Tucuxi::Common::Duration offsetTime = 0s;
            Tucuxi::Common::Duration interval = _interval;
            Tucuxi::Common::Duration infusionTime = _infusionTime;

            unsigned int residualSize = calculator.getResidualSize();
            bool isAll = false;
            std::vector<Tucuxi::Core::Concentrations> concentrations;
            concentrations.resize(residualSize);
            Tucuxi::Core::TimeOffsets times;
            {
                // Be careful, the intakeEvent embedds the nb of points, but the intervalintakecalculator also. They have to agree

                Tucuxi::Core::IntakeEvent intakeEvent(now, offsetTime, _dose, interval * nbCycles, _route, infusionTime, (_nbPoints - 1 ) * nbCycles + 1);

                Tucuxi::Core::Residuals inResiduals(residualSize);
                Tucuxi::Core::Residuals outResiduals(residualSize);

                std::fill(inResiduals.begin(), inResiduals.end(), 0);

                Tucuxi::Core::ParameterSetEvent event = *(_parameters.getAtTime(now));
                res = calculator.calculateIntakePoints(
                    concentrations,
                    times,
                    intakeEvent,
                    event,
                    inResiduals,
                    isAll,
                    outResiduals,
                    true);

#if 0
        for(int testPoint = 0; testPoint < (_nbPoints - 1 ) * nbCycles + 1; testPoint++) {
            std::cout << "concentration[" << testPoint << "]: " << concentrations[0][testPoint] << std::endl;
        }
#endif

                fructose_assert(res == Tucuxi::Core::IntakeIntervalCalculator::Result::Ok);
            }

            Tucuxi::Core::ConcentrationPredictionPtr predictionPtr;
            {
                predictionPtr = std::make_unique<Tucuxi::Core::ConcentrationPrediction>();

                Tucuxi::Core::IntakeSeries intakeSeries;
                std::shared_ptr<IntakeIntervalCalculator> calculator2 = std::make_shared<CalculatorClass>();

                DateTime recordFrom = now;
                DateTime recordTo = now + interval * nbCycles;

                for(int i = 0; i < nbCycles; i++) {
                    Tucuxi::Core::IntakeEvent event(now + interval * i, offsetTime, _dose, interval, _route, infusionTime, _nbPoints);
                    event.setCalculator(calculator2);
                    intakeSeries.push_back(event);
                }
                Tucuxi::Core::IConcentrationCalculator *concentrationCalculator = new Tucuxi::Core::ConcentrationCalculator();
                concentrationCalculator->computeConcentrations(
                    predictionPtr,
                    isAll,
                    recordFrom,
                    recordTo,
                    intakeSeries,
                    _parameters);
                delete concentrationCalculator;

#if 0
                for(int testCycle = 0; testCycle < nbCycles; testCycle++) {
                    for(int testNbPoint = 0; testNbPoint < _nbPoints; testNbPoint++)  {
                        std::cout << "concentration[" << testCycle << "]" << "[" << testNbPoint<< "]" << ": " << predictionPtr->getValues()[testCycle][testNbPoint] << std::endl;
                    }
                }
#endif
            }


            for (int cycle = 0; cycle < nbCycles; cycle ++) {
                Tucuxi::Core::Concentrations concentration2;
                concentration2 = predictionPtr->getValues()[cycle];
                for (int i = 0; i < _nbPoints - 1; i++) {
                    double sumConcentration = 0.0;
                    for (int c = 0; c < cycle + 1; c++) {
                        sumConcentration += concentrations[0][c * (_nbPoints - 1) + i];
                        // std::cout << c <<  " : " << sumConcentration << " : " << concentrations[0][c * (_nbPoints - 1) + i] << std::endl;
                    }
                    // std::cout << cycle <<  " : " << i << " :: " << predictionPtr->getTimes()[cycle][i] << " . " << sumConcentration << " : " << concentration2[i] << std::endl;
                    fructose_assert_double_eq(sumConcentration, concentration2[i]);
                }
            }
        }

        // Create 2 samples and compare the result of computeConcentrations() and pointsAtTime().
        //
        // This test fails for infusion. It should be redesigned according to non linear times
        if (_route != AbsorptionModel::INFUSION){
            CalculatorClass calculator;

            int nbPoints = 201;
            bool isAll = false;

            DateTime now;
            Tucuxi::Common::Duration offsetTime = 0s;
            Tucuxi::Common::Duration interval = _interval;
            Tucuxi::Common::Duration infusionTime = _infusionTime;

            Tucuxi::Core::Concentrations concentrations;
            Tucuxi::Core::TimeOffsets times;
            Tucuxi::Core::IntakeEvent intakeEvent(now, offsetTime, _dose, interval, _route, infusionTime, nbPoints);

            Tucuxi::Core::ConcentrationPredictionPtr predictionPtr;
            {
                predictionPtr = std::make_unique<Tucuxi::Core::ConcentrationPrediction>();

                DateTime recordFrom = now;
                DateTime recordTo = recordFrom + interval;

                Tucuxi::Core::IntakeSeries intakeSeries;
                std::shared_ptr<IntakeIntervalCalculator> calculator2 = std::make_shared<CalculatorClass>();
                intakeEvent.setCalculator(calculator2);
                intakeSeries.push_back(intakeEvent);
                Tucuxi::Core::IConcentrationCalculator *concentrationCalculator = new Tucuxi::Core::ConcentrationCalculator();
                concentrationCalculator->computeConcentrations(
                    predictionPtr,
                    isAll,
                    recordFrom,
                    recordTo,
                    intakeSeries,
                    _parameters);
                delete concentrationCalculator;

#if 0
                for (int i = 0; i<nbPoints; i++) {
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
                DateTime date0 = now + interval/4.0;

                Tucuxi::Core::SampleEvent s0(date0);
                sampleSeries.push_back(s0);
                DateTime date1 = now + interval * 3.0 /4.0;

                Tucuxi::Core::SampleEvent s1(date1);
                sampleSeries.push_back(s1);

                Tucuxi::Core::IConcentrationCalculator *concentrationCalculator = new Tucuxi::Core::ConcentrationCalculator();
                ComputationResult res = concentrationCalculator->computeConcentrationsAtTimes(
                    concentrations,
                    isAll,
                    intakeSeries,
                    _parameters,
                    sampleSeries);

                fructose_assert(res == ComputationResult::Success);
                fructose_assert_eq(res , ComputationResult::Success);

                delete concentrationCalculator;
            }

            int n0 = (nbPoints - 1) / 4;
            int n1 = (nbPoints - 1) * 3 / 4;

            // compare the result of compartment1
            fructose_assert_double_eq(concentrations[0], predictionPtr->getValues()[0][n0]);
            fructose_assert_double_eq(concentrations[1], predictionPtr->getValues()[0][n1]);
        }

        // Create samples and compare the result of computeConcentrations() and pointsAtTime().
        // This can be done by creating samples corresponding to the number of points asked, and
        // synchronized with the times at which the concentration points are expected
    }


    void testConstantEliminationBolus(const std::string& /* _testName */)
    {
        Tucuxi::Core::ParameterDefinitions parameterDefs;
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestA", 0.0, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestR", 0.0, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestS", 0.0, Tucuxi::Core::ParameterVariabilityType::None)));
        Tucuxi::Core::ParameterSetEvent parameters(DateTime(), parameterDefs);
        Tucuxi::Core::ParameterSetSeries parametersSeries;
        parametersSeries.addParameterSetEvent(parameters);

        testCalculator<Tucuxi::Core::ConstantEliminationBolus>(
            parametersSeries,
            400.0,
            Tucuxi::Core::AbsorptionModel::INTRAVASCULAR,
            12h,
            0s,
            CYCLE_SIZE);
    }


    void test1compBolus(const std::string& /* _testName */)
    {
        Tucuxi::Core::ParameterDefinitions parameterDefs;
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("V", 347, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("Ke", 22.97, Tucuxi::Core::ParameterVariabilityType::None)));
        Tucuxi::Core::ParameterSetEvent parameters(DateTime(), parameterDefs);
        Tucuxi::Core::ParameterSetSeries parametersSeries;
        parametersSeries.addParameterSetEvent(parameters);

        testCalculator<Tucuxi::Core::OneCompartmentBolusMicro>(
            parametersSeries,
            400.0,
            Tucuxi::Core::AbsorptionModel::INTRAVASCULAR,
            12h,
            0s,
            CYCLE_SIZE);
    }


    void test1compExtra(const std::string& /* _testName */)
    {
        Tucuxi::Core::ParameterDefinitions parameterDefs;
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("V", 347, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("Ke", 0.0435331, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("Ka", 0.609, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("F", 1, Tucuxi::Core::ParameterVariabilityType::None)));
        Tucuxi::Core::ParameterSetEvent parameters(DateTime(), parameterDefs);
        Tucuxi::Core::ParameterSetSeries parametersSeries;
        parametersSeries.addParameterSetEvent(parameters);

        testCalculator<Tucuxi::Core::OneCompartmentExtraMicro>(
            parametersSeries,
            400.0,
            Tucuxi::Core::AbsorptionModel::EXTRAVASCULAR,
            12h,
            0s,
            CYCLE_SIZE);
    }


    void test1compInfusion(const std::string& /* _testName */)
    {
        Tucuxi::Core::ParameterDefinitions parameterDefs;
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("V", 347, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("Ke", 22.97, Tucuxi::Core::ParameterVariabilityType::None)));
        Tucuxi::Core::ParameterSetEvent parameters(DateTime(), parameterDefs);
        Tucuxi::Core::ParameterSetSeries parametersSeries;
        parametersSeries.addParameterSetEvent(parameters);

        testCalculator<Tucuxi::Core::OneCompartmentInfusionMicro>(
            parametersSeries,
            400.0,
            Tucuxi::Core::AbsorptionModel::INFUSION,
            12h,
            1h,
            CYCLE_SIZE);
    }

    void test2compBolus(const std::string& /* _testName */)
    {
        Tucuxi::Core::ParameterDefinitions parameterDefs;
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("V1", 340, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("Ke", 0.0444294, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("K12", 0.0588235, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("K21", 0.0584795, Tucuxi::Core::ParameterVariabilityType::None)));
        Tucuxi::Core::ParameterSetEvent parameters(DateTime(), parameterDefs);
        Tucuxi::Core::ParameterSetSeries parametersSeries;
        parametersSeries.addParameterSetEvent(parameters);

        testCalculator<Tucuxi::Core::TwoCompartmentBolusMicro>(
            parametersSeries,
            400.0,
            Tucuxi::Core::AbsorptionModel::INTRAVASCULAR,
            12h,
            0s,
            CYCLE_SIZE);
    }


    void test2compExtra(const std::string& /* _testName */)
    {
        Tucuxi::Core::ParameterDefinitions parameterDefs;
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("V1", 340, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("Ke", 0.0444294, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("K12", 0.0588235, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("K21", 0.0584795, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("Ka", 0.609, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("F", 1, Tucuxi::Core::ParameterVariabilityType::None)));
        Tucuxi::Core::ParameterSetEvent parameters(DateTime(), parameterDefs);
        Tucuxi::Core::ParameterSetSeries parametersSeries;
        parametersSeries.addParameterSetEvent(parameters);

        testCalculator<Tucuxi::Core::TwoCompartmentExtraMicro>(
            parametersSeries,
            400.0,
            Tucuxi::Core::AbsorptionModel::EXTRAVASCULAR,
            12h,
            0s,
            CYCLE_SIZE);
    }


    void test2compInfusion(const std::string& /* _testName */)
    {
        Tucuxi::Core::ParameterDefinitions parameterDefs;
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("V1", 340, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("Ke", 0.0444294, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("K12", 0.0588235, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("K21", 0.0584795, Tucuxi::Core::ParameterVariabilityType::None)));
        Tucuxi::Core::ParameterSetEvent parameters(DateTime(), parameterDefs);
        Tucuxi::Core::ParameterSetSeries parametersSeries;
        parametersSeries.addParameterSetEvent(parameters);

        testCalculator<Tucuxi::Core::TwoCompartmentInfusionMicro>(
            parametersSeries,
            400.0,
            Tucuxi::Core::AbsorptionModel::INFUSION,
            12h,
            1h,
            CYCLE_SIZE);
    }

    void test3compBolus(const std::string& /* _testName */)
    {
        Tucuxi::Core::ParameterDefinitions parameterDefs;
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("F", 2, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("V1", 340, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("Ke", 0.0444294, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("K12", 0.0588235, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("K21", 0.0584795, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("K13", 0.0882353, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("K31", 0.0877193, Tucuxi::Core::ParameterVariabilityType::None)));
        Tucuxi::Core::ParameterSetEvent parameters(DateTime(), parameterDefs);
        Tucuxi::Core::ParameterSetSeries parametersSeries;
        parametersSeries.addParameterSetEvent(parameters);

        testCalculator<Tucuxi::Core::ThreeCompartmentBolusMicro>(
            parametersSeries,
            400.0,
            Tucuxi::Core::AbsorptionModel::INTRAVASCULAR,
            12h,
            0s,
            CYCLE_SIZE);
    }


    void test3compExtra(const std::string& /* _testName */)
    {
        Tucuxi::Core::ParameterDefinitions parameterDefs;
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("F", 2, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("V1", 340, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("Ka", 0.609, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("Ke", 0.0444294, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("K12", 0.0588235, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("K21", 0.0584795, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("K13", 0.0882353, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("K31", 0.0877193, Tucuxi::Core::ParameterVariabilityType::None)));
        Tucuxi::Core::ParameterSetEvent parameters(DateTime(), parameterDefs);
        Tucuxi::Core::ParameterSetSeries parametersSeries;
        parametersSeries.addParameterSetEvent(parameters);

        testCalculator<Tucuxi::Core::ThreeCompartmentExtraMicro>(
            parametersSeries,
            400.0,
            Tucuxi::Core::AbsorptionModel::EXTRAVASCULAR,
            12h,
            0s,
            CYCLE_SIZE);
    }


    void test3compInfusion(const std::string& /* _testName */)
    {
        Tucuxi::Core::ParameterDefinitions parameterDefs;
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("F", 2, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("V1", 340, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("Ke", 0.0444294, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("K12", 0.0588235, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("K21", 0.0584795, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("K13", 0.0882353, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("K31", 0.0877193, Tucuxi::Core::ParameterVariabilityType::None)));
        Tucuxi::Core::ParameterSetEvent parameters(DateTime(), parameterDefs);
        Tucuxi::Core::ParameterSetSeries parametersSeries;
        parametersSeries.addParameterSetEvent(parameters);

        testCalculator<Tucuxi::Core::ThreeCompartmentInfusionMicro>(
            parametersSeries,
            400.0,
            Tucuxi::Core::AbsorptionModel::INFUSION,
            12h,
            1h,
            CYCLE_SIZE);
    }
};

#endif // TEST_CONCENTRATIONCALCULATOR_H
