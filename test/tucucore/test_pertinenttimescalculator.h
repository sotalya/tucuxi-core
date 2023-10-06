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


#ifndef TEST_PERTINENTTIMESCALCULATOR_H
#define TEST_PERTINENTTIMESCALCULATOR_H


#include <chrono>
#include <iostream>
#include <memory>

#include "tucucore/intakeevent.h"
#include "tucucore/intakeintervalcalculator.h"

#include "fructose/fructose.h"

using namespace Tucuxi::Core;
using namespace Tucuxi::Common;


struct TestPertinentTimesCalculator : public fructose::test_base<TestPertinentTimesCalculator>
{
    TestPertinentTimesCalculator() {}

    /// \brief Test basic functionalities of a PkModel.
    void testStandard(const std::string& /* _testName */)
    {
        {
            PertinentTimesCalculatorStandard calculator;

            Eigen::Index nbPoints = 10;
            Eigen::VectorXd times(nbPoints);
            Duration interval(Duration(std::chrono::hours(24)));
            Duration infusionTime(Duration(std::chrono::minutes(0)));
            IntakeEvent intake(
                    DateTime::now(),
                    Duration(),
                    0.0,
                    Tucuxi::Common::TucuUnit("mg"),
                    interval,
                    Tucuxi::Core::FormulationAndRoute(AbsorptionModel::Extravascular),
                    AbsorptionModel::Extravascular,
                    infusionTime,
                    static_cast<size_t>(nbPoints));
            calculator.calculateTimes(intake, nbPoints, times);

            double delta = (interval.toHours() / static_cast<double>(nbPoints - 1));
            for (Eigen::Index i = 0; i < nbPoints; i++) {
                fructose_assert_double_eq(times[i], static_cast<double>(i) * delta);
            }
        }
    }

    void testInfusion(const std::string& /* _testName */)
    {
        PertinentTimesCalculatorInfusion calculator;

        {
            // Test a linear time, as the second point is exactly at the end of infusion time
            size_t nbPoints = 25;
            Eigen::VectorXd times(nbPoints);
            Duration interval(Duration(std::chrono::hours(24)));
            Duration infusionTime(Duration(std::chrono::minutes(60)));
            IntakeEvent intake(
                    DateTime::now(),
                    Duration(),
                    0.0,
                    Tucuxi::Common::TucuUnit("mg"),
                    interval,
                    Tucuxi::Core::FormulationAndRoute(AbsorptionModel::Infusion),
                    AbsorptionModel::Infusion,
                    infusionTime,
                    nbPoints);
            calculator.calculateTimes(intake, static_cast<Eigen::Index>(nbPoints), times);

            double delta = (interval.toHours() / static_cast<double>(nbPoints - 1));
            for (Eigen::Index i = 0; i < static_cast<Eigen::Index>(nbPoints); i++) {
                fructose_assert_double_eq(times[i], static_cast<double>(i) * delta);
            }
        }

        {
            // Test three points
            size_t nbPoints = 3;
            Eigen::VectorXd times(nbPoints);
            Duration interval(Duration(std::chrono::hours(24)));
            Duration infusionTime(Duration(std::chrono::minutes(60)));
            IntakeEvent intake(
                    DateTime::now(),
                    Duration(),
                    0.0,
                    Tucuxi::Common::TucuUnit("mg"),
                    interval,
                    Tucuxi::Core::FormulationAndRoute(AbsorptionModel::Infusion),
                    AbsorptionModel::Infusion,
                    infusionTime,
                    nbPoints);
            calculator.calculateTimes(intake, static_cast<Eigen::Index>(nbPoints), times);

            fructose_assert_double_eq(times[0], 0.0);
            fructose_assert_double_eq(times[1], 1.0);
            fructose_assert_double_eq(times[2], 24.0);
        }

        {
            // Test five points
            size_t nbPoints = 5;
            Eigen::VectorXd times(nbPoints);
            Duration interval(Duration(std::chrono::hours(24)));
            Duration infusionTime(Duration(std::chrono::minutes(60)));
            IntakeEvent intake(
                    DateTime::now(),
                    Duration(),
                    0.0,
                    Tucuxi::Common::TucuUnit("mg"),
                    interval,
                    Tucuxi::Core::FormulationAndRoute(AbsorptionModel::Infusion),
                    AbsorptionModel::Infusion,
                    infusionTime,
                    nbPoints);
            calculator.calculateTimes(intake, static_cast<Eigen::Index>(nbPoints), times);

            fructose_assert_double_eq(times[0], 0.0);
            fructose_assert_double_eq(times[1], 1.0);
            fructose_assert_double_eq(times[2], 1.0 + 23.0 / 3.0);
            fructose_assert_double_eq(times[3], 1.0 + 23.0 * 2.0 / 3.0);
            fructose_assert_double_eq(times[4], 24.0);
        }
        {
            // Test five points
            size_t nbPoints = 5;
            Eigen::VectorXd times(nbPoints);
            Duration interval(Duration(std::chrono::hours(24)));
            Duration infusionTime(Duration(std::chrono::minutes(60 * 10)));
            IntakeEvent intake(
                    DateTime::now(),
                    Duration(),
                    0.0,
                    Tucuxi::Common::TucuUnit("mg"),
                    interval,
                    Tucuxi::Core::FormulationAndRoute(AbsorptionModel::Infusion),
                    AbsorptionModel::Infusion,
                    infusionTime,
                    nbPoints);
            calculator.calculateTimes(intake, static_cast<Eigen::Index>(nbPoints), times);

            fructose_assert_double_eq(times[0], 0.0);
            fructose_assert_double_eq(times[1], 10.0);
            fructose_assert_double_eq(times[2], 10.0 + 14.0 / 3.0);
            fructose_assert_double_eq(times[3], 10.0 + 28.0 / 3.0);
            fructose_assert_double_eq(times[4], 24.0);
        }
    }
};


#endif // TEST_PERTINENTTIMESCALCULATOR_H
