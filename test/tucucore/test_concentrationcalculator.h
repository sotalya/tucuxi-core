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
#include "tucucore/onecompartmentbolus.h"
#include "tucucore/onecompartmentinfusion.h"
#include "tucucore/onecompartmentextra.h"

struct TestConcentrationCalculator : public fructose::test_base<TestConcentrationCalculator>
{

    TestConcentrationCalculator() { }

    template<class CalculatorClass>
    void testCalculator(const Tucuxi::Core::ParameterSetSeries &_parameters,
                        double _dose,
                        Tucuxi::Core::RouteOfAdministration _route,
                        std::chrono::hours _interval,
                        std::chrono::seconds _infusionTime,
                        int _nbPoints)
    {
        // Compare the result on one interval
        // with ConcentrationCalculator vs directly with the IntakeIntervalCalculator
        {

            Tucuxi::Core::IntakeIntervalCalculator::Result res;
            CalculatorClass calculator;

            DateTime now;
            Tucuxi::Common::Duration offsetTime = 0s;
            Tucuxi::Common::Duration interval = _interval;
            Tucuxi::Common::Duration infusionTime = _infusionTime;

            Tucuxi::Core::Concentrations concentrations;
            Tucuxi::Core::TimeOffsets times;
            Tucuxi::Core::IntakeEvent intakeEvent(now, offsetTime, _dose, interval, _route, infusionTime, _nbPoints);



            {

                Tucuxi::Core::Residuals inResiduals;
                Tucuxi::Core::Residuals outResiduals;
                for(unsigned int i = 0; i < calculator.getResidualSize(); i++)
                    inResiduals.push_back(0);

                Tucuxi::Core::ParameterSetEvent event = *(_parameters.getAtTime(now));
                res = calculator.calculateIntakePoints(
                            concentrations,
                            times,
                            intakeEvent,
                            event,
                            inResiduals,
                            _nbPoints,
                            outResiduals,
                            true);

                fructose_assert(res == Tucuxi::Core::IntakeIntervalCalculator::Result::Ok);
            }

            Tucuxi::Core::ConcentrationPredictionPtr predictionPtr;
            {
                predictionPtr = std::make_unique<Tucuxi::Core::ConcentrationPrediction>();

                Tucuxi::Core::IntakeSeries intakeSeries;
                CalculatorClass calculator2;
                intakeEvent.setCalculator(&calculator2);
                intakeSeries.push_back(intakeEvent);
                Tucuxi::Core::ConcentrationCalculator::computeConcentrations(
                            predictionPtr,
                            _nbPoints,
                            intakeSeries,
                            _parameters);
            }


            for(int i = 0; i < _nbPoints; i++) {
                Tucuxi::Core::Concentrations concentration2;
                concentration2 = predictionPtr->getValues()[0];
                fructose_assert_double_eq(concentrations[i], concentration2[i]);
            }
        }


        // Use the ConcentrationCalculator in order to calculate multiple cycles
        // and compare it with the sum of individual calculations made with the
        // IntakeIntervalCalculators. Be careful, the interval for the later need
        // to be longer, and the number of points modified accordingly
        {

            int nbCycles = 10;

            Tucuxi::Core::IntakeIntervalCalculator::Result res;
            CalculatorClass calculator;

            DateTime now;
            Tucuxi::Common::Duration offsetTime = 0s;
            Tucuxi::Common::Duration interval = _interval;
            Tucuxi::Common::Duration infusionTime = _infusionTime;

            Tucuxi::Core::Concentrations concentrations;
            Tucuxi::Core::TimeOffsets times;

            {
                // TOCHECK : Be careful, the intakeEvent embedds the nb of points, but the intervalintakecalculator also. They have to agree
                // Bad design for this.

                Tucuxi::Core::IntakeEvent intakeEvent(now, offsetTime, _dose, interval * nbCycles, _route, infusionTime, (_nbPoints - 1 ) * nbCycles + 1);

                Tucuxi::Core::Residuals inResiduals;
                Tucuxi::Core::Residuals outResiduals;
                for(unsigned int i = 0; i < calculator.getResidualSize(); i++)
                    inResiduals.push_back(0);

                Tucuxi::Core::ParameterSetEvent event = *(_parameters.getAtTime(now));
                res = calculator.calculateIntakePoints(
                            concentrations,
                            times,
                            intakeEvent,
                            event,
                            inResiduals,
                            (_nbPoints - 1 ) * nbCycles + 1,
                            outResiduals,
                            true);

                fructose_assert(res == Tucuxi::Core::IntakeIntervalCalculator::Result::Ok);
            }

            Tucuxi::Core::ConcentrationPredictionPtr predictionPtr;
            {
                predictionPtr = std::make_unique<Tucuxi::Core::ConcentrationPrediction>();

                Tucuxi::Core::IntakeSeries intakeSeries;
                CalculatorClass calculator2;

                for(int i = 0; i < nbCycles; i++) {
                    Tucuxi::Core::IntakeEvent event(now + interval * i, offsetTime, _dose, interval, _route, infusionTime, _nbPoints);
                    event.setCalculator(&calculator2);
                    intakeSeries.push_back(event);
                }
                Tucuxi::Core::ConcentrationCalculator::computeConcentrations(
                            predictionPtr,
                            _nbPoints,
                            intakeSeries,
                            _parameters);
            }


            for(int cycle = 0; cycle < nbCycles; cycle ++) {
                Tucuxi::Core::Concentrations concentration2;
                concentration2 = predictionPtr->getValues()[cycle];
                for(int i = 0; i < _nbPoints - 1; i++) {

                    double sumConcentration = 0.0;
                    for(int c = 0; c < cycle + 1; c++)
                        sumConcentration += concentrations[c * (_nbPoints - 1) + i];

               //     std::cout << cycle <<  " : " << i << " :: " << predictionPtr->getTimes()[cycle][i] << " . " << sumConcentration << " : " << concentration2[i] << std::endl;

                    fructose_assert_double_eq(sumConcentration, concentration2[i]);
                }
            }
        }



        // Create samples and compare the result of computeConcentrations() and pointsAtTime().
        // This can be done by creating samples corresponding to the number of points asked, and
        // synchronized with the times at which the concentration points are expected

    }

    void test1compBolus(const std::string& /* _testName */)
    {
        {
            Tucuxi::Core::ParameterDefinitions parameterDefs;
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("V", 347, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("Ke", 22.97, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            Tucuxi::Core::ParameterSetEvent parameters(DateTime(), parameterDefs);
            Tucuxi::Core::ParameterSetSeries parametersSeries;
            parametersSeries.addParameterSetEvent(parameters);

            testCalculator<Tucuxi::Core::OneCompartmentBolusMicro>
                    (parametersSeries,
                     400.0,
                     Tucuxi::Core::RouteOfAdministration::INTRAVASCULAR,
                     12h,
                     0s,
                     250);
        }

    }


    void test1compExtra(const std::string& /* _testName */)
    {

        {
            Tucuxi::Core::ParameterDefinitions parameterDefs;
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("V", 347, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("Ke", 0.0435331, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("Ka", 0.609, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("F", 1, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            Tucuxi::Core::ParameterSetEvent parameters(DateTime(), parameterDefs);
            Tucuxi::Core::ParameterSetSeries parametersSeries;
            parametersSeries.addParameterSetEvent(parameters);

            testCalculator<Tucuxi::Core::OneCompartmentExtraMicro>
                    (parametersSeries,
                     400.0,
                     Tucuxi::Core::RouteOfAdministration::EXTRAVASCULAR,
                     12h,
                     0s,
                     250);
        }
    }


    void test1compInfusion(const std::string& /* _testName */)
    {
        {
            Tucuxi::Core::ParameterDefinitions parameterDefs;
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("V", 347, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("Ke", 22.97, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            Tucuxi::Core::ParameterSetEvent parameters(DateTime(), parameterDefs);
            Tucuxi::Core::ParameterSetSeries parametersSeries;
            parametersSeries.addParameterSetEvent(parameters);

            testCalculator<Tucuxi::Core::OneCompartmentInfusionMicro>
                    (parametersSeries,
                     400.0,
                     Tucuxi::Core::RouteOfAdministration::INFUSION,
                     12h,
                     1h,
                     250);
        }

    }

};

#endif // TEST_CONCENTRATIONCALCULATOR_H
