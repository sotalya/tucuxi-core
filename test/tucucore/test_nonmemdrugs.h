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

#include "tucucore/computingservice/computingresult.h"
#include "tucucore/dosage.h"
#include "tucucore/intakeextractor.h"
#include "tucucore/intakeintervalcalculator.h"
#include "tucucore/concentrationcalculator.h"
#include "tucucore/pkmodels/onecompartmentbolus.h"
#include "tucucore/pkmodels/onecompartmentinfusion.h"
#include "tucucore/pkmodels/onecompartmentextra.h"

struct TestNonMemDrugs : public fructose::test_base<TestNonMemDrugs>
{
    static const int CYCLE_SIZE = 251;

    TestNonMemDrugs() { }

    template<class CalculatorClass>
    void testCalculator(const Tucuxi::Core::ParameterSetSeries &_parameters,
                        double _dose,
                        Tucuxi::Core::FormulationAndRoute _formulationAndRoute,
                        Tucuxi::Core::AbsorptionModel _route,
                        std::chrono::hours _interval,
                        std::chrono::seconds _infusionTime,
                        int _nbPoints)
    {
        // Compare the result on one interval
        // with ConcentrationCalculator vs directly with the IntakeIntervalCalculator
        {

            Tucuxi::Core::ComputingStatus res;
            CalculatorClass calculator;

            DateTime now;
            Tucuxi::Common::Duration offsetTime = 0s;
            Tucuxi::Common::Duration interval = _interval;
            Tucuxi::Common::Duration infusionTime = _infusionTime;

            Tucuxi::Core::Concentrations concentrations;
            Tucuxi::Core::TimeOffsets times;
            Tucuxi::Core::IntakeEvent intakeEvent(now, offsetTime, _dose, Tucuxi::Common::TucuUnit("mg"), interval, _formulationAndRoute, _route, infusionTime, _nbPoints);

            bool isAll = false;

            // std::cout << typeid(calculator).name() << std::endl;

            {
                Tucuxi::Core::Residuals inResiduals;
                Tucuxi::Core::Residuals outResiduals;
                for (unsigned int i = 0; i < calculator.getResidualSize(); i++) {
                    inResiduals.push_back(0);
                }

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

                fructose_assert(res == Tucuxi::Core::ComputingStatus::Ok);
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
                    isAll,
                    DateTime(), // YJ: Fix this with a meaningfull date
                    DateTime(), // YJ: Fix this with a meaningfull date
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

        bool isAll = false;

        std::shared_ptr<Tucuxi::Core::IntakeIntervalCalculator> calculator2 = std::make_shared<CalculatorClass>();

        Tucuxi::Common::Duration offsetTime = 0s;
        Tucuxi::Common::Duration interval24 = 24h;
        Tucuxi::Common::Duration interval6 = 6h;
        Tucuxi::Common::Duration infusionTime = 0h;

        Tucuxi::Core::Concentrations concentrations;
        //            Tucuxi::Core::IntakeEvent intakeEvent(now, offsetTime, _dose, interval, _route, infusionTime, nbPoints);

        Tucuxi::Core::AbsorptionModel route = Tucuxi::Core::AbsorptionModel::Intravascular;

        Tucuxi::Core::FormulationAndRoute formulationAndRoute(
                    Tucuxi::Core::Formulation::ParenteralSolution,
                    Tucuxi::Core::AdministrationRoute::IntravenousBolus,
                    Tucuxi::Core::AbsorptionModel::Intravascular,
                    "");

        Tucuxi::Core::IntakeSeries intakeSeries;
        {
            // Build the intake series


            {
                Tucuxi::Core::IntakeEvent intakeEvent(DateTime("2015-09-05-12:00:00","%Y-%m-%d-%H:%M:%S"),
                                                      offsetTime,
                                                      600,
                                                      Tucuxi::Common::TucuUnit("mg"),
                                                      interval24,
                                                      formulationAndRoute,
                                                      route,
                                                      infusionTime,
                                                      CYCLE_SIZE);

                intakeEvent.setCalculator(calculator2);
                intakeSeries.push_back(intakeEvent);
            }

            {
                Tucuxi::Core::IntakeEvent intakeEvent(DateTime("2015-09-06-12:00:00","%Y-%m-%d-%H:%M:%S"),
                                                      offsetTime,
                                                      600,
                                                      Tucuxi::Common::TucuUnit("mg"),
                                                      interval24,
                                                      formulationAndRoute,
                                                      route,
                                                      infusionTime,
                                                      CYCLE_SIZE);

                intakeEvent.setCalculator(calculator2);
                intakeSeries.push_back(intakeEvent);
            }
            {
                Tucuxi::Core::IntakeEvent intakeEvent(DateTime("2015-09-07-12:00:00","%Y-%m-%d-%H:%M:%S"),
                                                      offsetTime,
                                                      600,
                                                      Tucuxi::Common::TucuUnit("mg"),
                                                      interval24,
                                                      formulationAndRoute,
                                                      route,
                                                      infusionTime,
                                                      CYCLE_SIZE);

                intakeEvent.setCalculator(calculator2);
                intakeSeries.push_back(intakeEvent);
            }

            Tucuxi::Common::Duration shortInterval = 370min;

            {
                Tucuxi::Core::IntakeEvent intakeEvent(DateTime("2015-09-08-12:00:00","%Y-%m-%d-%H:%M:%S"),
                                                      offsetTime,
                                                      600,
                                                      Tucuxi::Common::TucuUnit("mg"),
                                                      shortInterval,
                                                      formulationAndRoute,
                                                      route,
                                                      infusionTime,
                                                      CYCLE_SIZE);

                intakeEvent.setCalculator(calculator2);
                intakeSeries.push_back(intakeEvent);
            }


            {
                Tucuxi::Core::IntakeEvent intakeEvent(DateTime("2015-09-08-18:10:00","%Y-%m-%d-%H:%M:%S"),
                                                      offsetTime,
                                                      400,
                                                      Tucuxi::Common::TucuUnit("mg"),
                                                      interval6,
                                                      formulationAndRoute,
                                                      route,
                                                      infusionTime,
                                                      CYCLE_SIZE);

                intakeEvent.setCalculator(calculator2);
                intakeSeries.push_back(intakeEvent);
            }

            {
                Tucuxi::Core::IntakeEvent intakeEvent(DateTime("2015-09-09-00:10:00","%Y-%m-%d-%H:%M:%S"),
                                                      offsetTime,
                                                      400,
                                                      Tucuxi::Common::TucuUnit("mg"),
                                                      interval6,
                                                      formulationAndRoute,
                                                      route,
                                                      infusionTime,
                                                      CYCLE_SIZE);

                intakeEvent.setCalculator(calculator2);
                intakeSeries.push_back(intakeEvent);
            }

            {
                Tucuxi::Common::Duration shortInterval = 350min;

                Tucuxi::Core::IntakeEvent intakeEvent(DateTime("2015-09-09-06:10:00","%Y-%m-%d-%H:%M:%S"),
                                                      offsetTime,
                                                      400,
                                                      Tucuxi::Common::TucuUnit("mg"),
                                                      shortInterval,
                                                      formulationAndRoute,
                                                      route,
                                                      infusionTime,
                                                      CYCLE_SIZE);

                intakeEvent.setCalculator(calculator2);
                intakeSeries.push_back(intakeEvent);
            }



            {
                Tucuxi::Common::Duration shortInterval = 10min;

                Tucuxi::Core::IntakeEvent intakeEvent(DateTime("2015-09-09-12:00:00","%Y-%m-%d-%H:%M:%S"),
                                                      offsetTime,
                                                      600,
                                                      Tucuxi::Common::TucuUnit("mg"),
                                                      shortInterval,
                                                      formulationAndRoute,
                                                      route,
                                                      infusionTime,
                                                      CYCLE_SIZE);

                intakeEvent.setCalculator(calculator2);
                intakeSeries.push_back(intakeEvent);
            }


            {
                Tucuxi::Core::IntakeEvent intakeEvent(DateTime("2015-09-09-12:10:00","%Y-%m-%d-%H:%M:%S"),
                                                      offsetTime,
                                                      400,
                                                      Tucuxi::Common::TucuUnit("mg"),
                                                      interval6,
                                                      formulationAndRoute,
                                                      route,
                                                      infusionTime,
                                                      CYCLE_SIZE);

                intakeEvent.setCalculator(calculator2);
                intakeSeries.push_back(intakeEvent);
            }
            {
                Tucuxi::Core::IntakeEvent intakeEvent(DateTime("2015-09-09-18:10:00","%Y-%m-%d-%H:%M:%S"),
                                                      offsetTime,
                                                      400,
                                                      Tucuxi::Common::TucuUnit("mg"),
                                                      interval6,
                                                      formulationAndRoute,
                                                      route,
                                                      infusionTime,
                                                      CYCLE_SIZE);

                intakeEvent.setCalculator(calculator2);
                intakeSeries.push_back(intakeEvent);
            }


            {
                Tucuxi::Core::IntakeEvent intakeEvent(DateTime("2015-09-10-00:10:00","%Y-%m-%d-%H:%M:%S"),
                                                      offsetTime,
                                                      400,
                                                      Tucuxi::Common::TucuUnit("mg"),
                                                      interval6,
                                                      formulationAndRoute,
                                                      route,
                                                      infusionTime,
                                                      CYCLE_SIZE);

                intakeEvent.setCalculator(calculator2);
                intakeSeries.push_back(intakeEvent);
            }

            {
                Tucuxi::Core::IntakeEvent intakeEvent(DateTime("2015-09-10-06:10:00","%Y-%m-%d-%H:%M:%S"),
                                                      offsetTime,
                                                      400,
                                                      Tucuxi::Common::TucuUnit("mg"),
                                                      interval6,
                                                      formulationAndRoute,
                                                      route,
                                                      infusionTime,
                                                      CYCLE_SIZE);

                intakeEvent.setCalculator(calculator2);
                intakeSeries.push_back(intakeEvent);
            }
            {
                Tucuxi::Core::IntakeEvent intakeEvent(DateTime("2015-09-10-12:10:00","%Y-%m-%d-%H:%M:%S"),
                                                      offsetTime,
                                                      400,
                                                      Tucuxi::Common::TucuUnit("mg"),
                                                      interval6,
                                                      formulationAndRoute,
                                                      route,
                                                      infusionTime,
                                                      CYCLE_SIZE);

                intakeEvent.setCalculator(calculator2);
                intakeSeries.push_back(intakeEvent);
            }
            {
                Tucuxi::Core::IntakeEvent intakeEvent(DateTime("2015-09-10-18:10:00","%Y-%m-%d-%H:%M:%S"),
                                                      offsetTime,
                                                      400,
                                                      Tucuxi::Common::TucuUnit("mg"),
                                                      interval6,
                                                      formulationAndRoute,
                                                      route,
                                                      infusionTime,
                                                      CYCLE_SIZE);

                intakeEvent.setCalculator(calculator2);
                intakeSeries.push_back(intakeEvent);
            }

            {
                Tucuxi::Core::IntakeEvent intakeEvent(DateTime("2015-09-11-00:10:00","%Y-%m-%d-%H:%M:%S"),
                                                      offsetTime,
                                                      400,
                                                      Tucuxi::Common::TucuUnit("mg"),
                                                      interval6,
                                                      formulationAndRoute,
                                                      route,
                                                      infusionTime,
                                                      CYCLE_SIZE);

                intakeEvent.setCalculator(calculator2);
                intakeSeries.push_back(intakeEvent);
            }

            {
                Tucuxi::Core::IntakeEvent intakeEvent(DateTime("2015-09-11-06:10:00","%Y-%m-%d-%H:%M:%S"),
                                                      offsetTime,
                                                      400,
                                                      Tucuxi::Common::TucuUnit("mg"),
                                                      interval6,
                                                      formulationAndRoute,
                                                      route,
                                                      infusionTime,
                                                      CYCLE_SIZE);

                intakeEvent.setCalculator(calculator2);
                intakeSeries.push_back(intakeEvent);
            }
        }

        // Imatinib parameters, as in the XML drug file
        Tucuxi::Core::ParameterDefinitions parameterDefs;
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("V", 347, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("CL", 15.106, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("Ka", 0.609, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("F", 1, Tucuxi::Core::ParameterVariabilityType::None)));
        Tucuxi::Core::ParameterSetEvent parameters(DateTime(), parameterDefs);
        Tucuxi::Core::ParameterSetSeries parametersSeries;
        parametersSeries.addParameterSetEvent(parameters);

        Tucuxi::Core::IConcentrationCalculator *concentrationCalculator = new Tucuxi::Core::ConcentrationCalculator();
        auto status = concentrationCalculator->computeConcentrations(
            predictionPtr,
            isAll,
            DateTime(), // YJ: Fix this with a meaningfull date
            DateTime(), // YJ: Fix this with a meaningfull date
            intakeSeries,
            parametersSeries);
        delete concentrationCalculator;

        fructose_assert_eq(status, ComputingStatus::Ok);

        predictionPtr->streamToFile("values_imatinib_nonmemdrugs.dat");
    }
};

#endif // TEST_NONMEMDRUGS_H
