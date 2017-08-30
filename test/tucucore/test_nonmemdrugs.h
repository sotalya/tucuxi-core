/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TEST_NONMEMDRUGS_H
#define TEST_NONMEMDRUGS_H

#include <iostream>
#include <memory>
#include <iostream>
#include <fstream>
#include <string>


#include "fructose/fructose.h"

#include "tucucommon/general.h"

#include "tucucore/dosage.h"
#include "tucucore/intakeextractor.h"
#include "tucucore/intakeintervalcalculator.h"
#include "tucucore/concentrationcalculator.h"
#include "tucucore/onecompartmentbolus.h"
#include "tucucore/onecompartmentinfusion.h"
#include "tucucore/onecompartmentextra.h"

struct TestNonMemDrugs : public fructose::test_base<TestNonMemDrugs>
{

    TestNonMemDrugs() { }

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

	    // std::cout << typeid(calculator).name() << std::endl;

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
                Tucuxi::Core::IConcentrationCalculator *concentrationCalculator = new Tucuxi::Core::ConcentrationCalculator();
                concentrationCalculator->computeConcentrations(
                            predictionPtr,
                            _nbPoints,
                            intakeSeries,
                            _parameters);
                delete concentrationCalculator;
            }


            for(int i = 0; i < _nbPoints; i++) {
                Tucuxi::Core::Concentrations concentration2;
                concentration2 = predictionPtr->getValues()[0];
                // std::cout << i <<  " :: " << concentrations[i] << " : " << concentration2[i] << std::endl;
                fructose_assert_double_eq(concentrations[i], concentration2[i]);
            }
        }

    }

    void testImatinib(const std::string& /* _testName */)
    {
        typedef Tucuxi::Core::OneCompartmentExtraMacro CalculatorClass;

        Tucuxi::Core::ConcentrationPredictionPtr predictionPtr;

        predictionPtr = std::make_unique<Tucuxi::Core::ConcentrationPrediction>();

        int nbPoints = 251;

        CalculatorClass calculator2;


        Tucuxi::Common::Duration offsetTime = 0s;
        Tucuxi::Common::Duration interval24 = 24h;
        Tucuxi::Common::Duration interval6 = 6h;
        Tucuxi::Common::Duration infusionTime = 0h;

        Tucuxi::Core::Concentrations concentrations;
        //            Tucuxi::Core::IntakeEvent intakeEvent(now, offsetTime, _dose, interval, _route, infusionTime, nbPoints);

        Tucuxi::Core::RouteOfAdministration route = Tucuxi::Core::RouteOfAdministration::INTRAVASCULAR;

        Tucuxi::Core::IntakeSeries intakeSeries;
        {
            // Build the intake series


            {
                Tucuxi::Core::IntakeEvent intakeEvent(DateTime("2015-09-05-12:00:00","YYYY-MM-DD-HH:mm:ss"),
                                                      offsetTime,
                                                      600,
                                                      interval24,
                                                      route,
                                                      infusionTime,
                                                      nbPoints);

                intakeEvent.setCalculator(&calculator2);
                intakeSeries.push_back(intakeEvent);
            }

            {
                Tucuxi::Core::IntakeEvent intakeEvent(DateTime("2015-09-06-12:00:00","YYYY-MM-DD-HH:mm:ss"),
                                                      offsetTime,
                                                      600,
                                                      interval24,
                                                      route,
                                                      infusionTime,
                                                      nbPoints);

                intakeEvent.setCalculator(&calculator2);
                intakeSeries.push_back(intakeEvent);
            }
            {
                Tucuxi::Core::IntakeEvent intakeEvent(DateTime("2015-09-07-12:00:00","YYYY-MM-DD-HH:mm:ss"),
                                                      offsetTime,
                                                      600,
                                                      interval24,
                                                      route,
                                                      infusionTime,
                                                      nbPoints);

                intakeEvent.setCalculator(&calculator2);
                intakeSeries.push_back(intakeEvent);
            }

            Tucuxi::Common::Duration shortInterval = 370min;

            {
                Tucuxi::Core::IntakeEvent intakeEvent(DateTime("2015-09-08-12:00:00","YYYY-MM-DD-HH:mm:ss"),
                                                      offsetTime,
                                                      600,
                                                      shortInterval,
                                                      route,
                                                      infusionTime,
                                                      nbPoints);

                intakeEvent.setCalculator(&calculator2);
                intakeSeries.push_back(intakeEvent);
            }


            {
                Tucuxi::Core::IntakeEvent intakeEvent(DateTime("2015-09-08-18:10:00","YYYY-MM-DD-HH:mm:ss"),
                                                      offsetTime,
                                                      400,
                                                      interval6,
                                                      route,
                                                      infusionTime,
                                                      nbPoints);

                intakeEvent.setCalculator(&calculator2);
                intakeSeries.push_back(intakeEvent);
            }

            {
                Tucuxi::Core::IntakeEvent intakeEvent(DateTime("2015-09-09-00:10:00","YYYY-MM-DD-HH:mm:ss"),
                                                      offsetTime,
                                                      400,
                                                      interval6,
                                                      route,
                                                      infusionTime,
                                                      nbPoints);

                intakeEvent.setCalculator(&calculator2);
                intakeSeries.push_back(intakeEvent);
            }

            {
                Tucuxi::Common::Duration shortInterval = 350min;

                Tucuxi::Core::IntakeEvent intakeEvent(DateTime("2015-09-09-06:10:00","YYYY-MM-DD-HH:mm:ss"),
                                                      offsetTime,
                                                      400,
                                                      shortInterval,
                                                      route,
                                                      infusionTime,
                                                      nbPoints);

                intakeEvent.setCalculator(&calculator2);
                intakeSeries.push_back(intakeEvent);
            }



            {
                Tucuxi::Common::Duration shortInterval = 10min;

                Tucuxi::Core::IntakeEvent intakeEvent(DateTime("2015-09-09-12:00:00","YYYY-MM-DD-HH:mm:ss"),
                                                      offsetTime,
                                                      600,
                                                      shortInterval,
                                                      route,
                                                      infusionTime,
                                                      nbPoints);

                intakeEvent.setCalculator(&calculator2);
                intakeSeries.push_back(intakeEvent);
            }


            {
                Tucuxi::Core::IntakeEvent intakeEvent(DateTime("2015-09-09-12:10:00","YYYY-MM-DD-HH:mm:ss"),
                                                      offsetTime,
                                                      400,
                                                      interval6,
                                                      route,
                                                      infusionTime,
                                                      nbPoints);

                intakeEvent.setCalculator(&calculator2);
                intakeSeries.push_back(intakeEvent);
            }
            {
                Tucuxi::Core::IntakeEvent intakeEvent(DateTime("2015-09-09-18:10:00","YYYY-MM-DD-HH:mm:ss"),
                                                      offsetTime,
                                                      400,
                                                      interval6,
                                                      route,
                                                      infusionTime,
                                                      nbPoints);

                intakeEvent.setCalculator(&calculator2);
                intakeSeries.push_back(intakeEvent);
            }


            {
                Tucuxi::Core::IntakeEvent intakeEvent(DateTime("2015-09-10-00:10:00","YYYY-MM-DD-HH:mm:ss"),
                                                      offsetTime,
                                                      400,
                                                      interval6,
                                                      route,
                                                      infusionTime,
                                                      nbPoints);

                intakeEvent.setCalculator(&calculator2);
                intakeSeries.push_back(intakeEvent);
            }

            {
                Tucuxi::Core::IntakeEvent intakeEvent(DateTime("2015-09-10-06:10:00","YYYY-MM-DD-HH:mm:ss"),
                                                      offsetTime,
                                                      400,
                                                      interval6,
                                                      route,
                                                      infusionTime,
                                                      nbPoints);

                intakeEvent.setCalculator(&calculator2);
                intakeSeries.push_back(intakeEvent);
            }
            {
                Tucuxi::Core::IntakeEvent intakeEvent(DateTime("2015-09-10-12:10:00","YYYY-MM-DD-HH:mm:ss"),
                                                      offsetTime,
                                                      400,
                                                      interval6,
                                                      route,
                                                      infusionTime,
                                                      nbPoints);

                intakeEvent.setCalculator(&calculator2);
                intakeSeries.push_back(intakeEvent);
            }
            {
                Tucuxi::Core::IntakeEvent intakeEvent(DateTime("2015-09-10-18:10:00","YYYY-MM-DD-HH:mm:ss"),
                                                      offsetTime,
                                                      400,
                                                      interval6,
                                                      route,
                                                      infusionTime,
                                                      nbPoints);

                intakeEvent.setCalculator(&calculator2);
                intakeSeries.push_back(intakeEvent);
            }

            {
                Tucuxi::Core::IntakeEvent intakeEvent(DateTime("2015-09-11-00:10:00","YYYY-MM-DD-HH:mm:ss"),
                                                      offsetTime,
                                                      400,
                                                      interval6,
                                                      route,
                                                      infusionTime,
                                                      nbPoints);

                intakeEvent.setCalculator(&calculator2);
                intakeSeries.push_back(intakeEvent);
            }

            {
                Tucuxi::Core::IntakeEvent intakeEvent(DateTime("2015-09-11-06:10:00","YYYY-MM-DD-HH:mm:ss"),
                                                      offsetTime,
                                                      400,
                                                      interval6,
                                                      route,
                                                      infusionTime,
                                                      nbPoints);

                intakeEvent.setCalculator(&calculator2);
                intakeSeries.push_back(intakeEvent);
            }

        }


        // Imatinib parameters, as in the XML drug file
        Tucuxi::Core::ParameterDefinitions parameterDefs;
        parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("V", 347, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
        parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("CL", 15.106, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
        parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("Ka", 0.609, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
        parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("F", 1, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
        Tucuxi::Core::ParameterSetEvent parameters(DateTime(), parameterDefs);
        Tucuxi::Core::ParameterSetSeries parametersSeries;
        parametersSeries.addParameterSetEvent(parameters);



        Tucuxi::Core::IConcentrationCalculator *concentrationCalculator = new Tucuxi::Core::ConcentrationCalculator();
        concentrationCalculator->computeConcentrations(
                    predictionPtr,
                    nbPoints,
                    intakeSeries,
                    parametersSeries);
        delete concentrationCalculator;

        predictionPtr->streamToFile("values_imatinib.dat");
    }
};

#endif // TEST_NONMEMDRUGS_H
