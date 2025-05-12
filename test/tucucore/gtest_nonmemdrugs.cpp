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
#include <string>

#include <gtest/gtest.h>

#include "tucucore/computingservice/computingresult.h"
#include "tucucore/concentrationcalculator.h"
#include "tucucore/dosage.h"
#include "tucucore/intakeextractor.h"
#include "tucucore/intakeintervalcalculator.h"
#include "tucucore/pkmodels/onecompartmentextra.h"

#include "gtest_core.h"

template<class CalculatorClass>
static void testNonmemCalculator(
        const Tucuxi::Core::ParameterSetSeries& _parameters,
        double _dose,
        Tucuxi::Core::FormulationAndRoute _formulationAndRoute,
        Tucuxi::Core::AbsorptionModel _route,
        std::chrono::hours _interval,
        std::chrono::seconds _infusionTime,
        size_t _nbPoints)
{
    // Compare the result on one interval
    // with ConcentrationCalculator vs directly with the IntakeIntervalCalculator
    {

        Tucuxi::Core::ComputingStatus res;
        CalculatorClass calculator;

        DateTime now = DateTime::now();
        Tucuxi::Common::Duration offsetTime = 0s;
        Tucuxi::Common::Duration interval = _interval;
        Tucuxi::Common::Duration infusionTime = _infusionTime;

        Tucuxi::Core::Concentrations concentrations;
        Tucuxi::Core::TimeOffsets times;
        Tucuxi::Core::IntakeEvent intakeEvent(
                now,
                offsetTime,
                _dose,
                Tucuxi::Common::TucuUnit("mg"),
                interval,
                _formulationAndRoute,
                _route,
                infusionTime,
                _nbPoints);

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
                    concentrations, times, intakeEvent, event, inResiduals, isAll, outResiduals, true);

            ASSERT_EQ(res, Tucuxi::Core::ComputingStatus::Ok);
        }

        Tucuxi::Core::ConcentrationPredictionPtr predictionPtr;
        {
            predictionPtr = std::make_unique<Tucuxi::Core::ConcentrationPrediction>();

            Tucuxi::Core::IntakeSeries intakeSeries;
            CalculatorClass calculator2;
            intakeEvent.setCalculator(&calculator2);
            intakeSeries.push_back(intakeEvent);
            auto concentrationCalculator = std::make_unique<Tucuxi::Core::ConcentrationCalculator>();
            auto status = concentrationCalculator->computeConcentrations(
                    predictionPtr,
                    isAll,
                    DateTime::now(), // YJ: Fix this with a meaningfull date
                    DateTime::now(), // YJ: Fix this with a meaningfull date
                    intakeSeries,
                    _parameters);
            ASSERT_EQ(status, Tucuxi::Core::ComputingStatus::Ok);
        }


        for (size_t i = 0; i < _nbPoints; i++) {
            Tucuxi::Core::Concentrations concentration2;
            concentration2 = predictionPtr->getValues()[0];
            // std::cout << i <<  " :: " << concentrations[i] << " : " << concentration2[i] << std::endl;
            ASSERT_DOUBLE_EQ(concentrations[i], concentration2[i]);
        }
    }
}

TEST(Core_TestNonMemDrugs, Imatinib)
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
            Tucuxi::Core::Formulation::ParenteralSolution, Tucuxi::Core::AdministrationRoute::IntravenousBolus, "");

    Tucuxi::Core::IntakeSeries intakeSeries;
    {
        // Build the intake series


        {
            Tucuxi::Core::IntakeEvent intakeEvent(
                    DateTime("2015-09-05-12:00:00", "%Y-%m-%d-%H:%M:%S"),
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
            Tucuxi::Core::IntakeEvent intakeEvent(
                    DateTime("2015-09-06-12:00:00", "%Y-%m-%d-%H:%M:%S"),
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
            Tucuxi::Core::IntakeEvent intakeEvent(
                    DateTime("2015-09-07-12:00:00", "%Y-%m-%d-%H:%M:%S"),
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
            Tucuxi::Core::IntakeEvent intakeEvent(
                    DateTime("2015-09-08-12:00:00", "%Y-%m-%d-%H:%M:%S"),
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
            Tucuxi::Core::IntakeEvent intakeEvent(
                    DateTime("2015-09-08-18:10:00", "%Y-%m-%d-%H:%M:%S"),
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
            Tucuxi::Core::IntakeEvent intakeEvent(
                    DateTime("2015-09-09-00:10:00", "%Y-%m-%d-%H:%M:%S"),
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

            Tucuxi::Core::IntakeEvent intakeEvent(
                    DateTime("2015-09-09-06:10:00", "%Y-%m-%d-%H:%M:%S"),
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

            Tucuxi::Core::IntakeEvent intakeEvent(
                    DateTime("2015-09-09-12:00:00", "%Y-%m-%d-%H:%M:%S"),
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
            Tucuxi::Core::IntakeEvent intakeEvent(
                    DateTime("2015-09-09-12:10:00", "%Y-%m-%d-%H:%M:%S"),
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
            Tucuxi::Core::IntakeEvent intakeEvent(
                    DateTime("2015-09-09-18:10:00", "%Y-%m-%d-%H:%M:%S"),
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
            Tucuxi::Core::IntakeEvent intakeEvent(
                    DateTime("2015-09-10-00:10:00", "%Y-%m-%d-%H:%M:%S"),
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
            Tucuxi::Core::IntakeEvent intakeEvent(
                    DateTime("2015-09-10-06:10:00", "%Y-%m-%d-%H:%M:%S"),
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
            Tucuxi::Core::IntakeEvent intakeEvent(
                    DateTime("2015-09-10-12:10:00", "%Y-%m-%d-%H:%M:%S"),
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
            Tucuxi::Core::IntakeEvent intakeEvent(
                    DateTime("2015-09-10-18:10:00", "%Y-%m-%d-%H:%M:%S"),
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
            Tucuxi::Core::IntakeEvent intakeEvent(
                    DateTime("2015-09-11-00:10:00", "%Y-%m-%d-%H:%M:%S"),
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
            Tucuxi::Core::IntakeEvent intakeEvent(
                    DateTime("2015-09-11-06:10:00", "%Y-%m-%d-%H:%M:%S"),
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
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "V", 347, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "CL", 15.106, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "Ka", 0.609, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(
            std::make_unique<Tucuxi::Core::ParameterDefinition>("F", 1, Tucuxi::Core::ParameterVariabilityType::None));
    Tucuxi::Core::ParameterSetEvent parameters(DateTime::now(), parameterDefs);
    Tucuxi::Core::ParameterSetSeries parametersSeries;
    parametersSeries.addParameterSetEvent(parameters);

    auto concentrationCalculator = std::make_unique<Tucuxi::Core::ConcentrationCalculator>();
    auto status = concentrationCalculator->computeConcentrations(
            predictionPtr,
            isAll,
            DateTime::now(), // YJ: Fix this with a meaningfull date
            DateTime::now(), // YJ: Fix this with a meaningfull date
            intakeSeries,
            parametersSeries);

    ASSERT_EQ(status, Tucuxi::Core::ComputingStatus::Ok);

    predictionPtr->streamToFile("values_imatinib_nonmemdrugs.dat");
}
