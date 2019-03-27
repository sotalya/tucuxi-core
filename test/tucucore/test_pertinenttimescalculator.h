#ifndef TEST_PERTINENTTIMESCALCULATOR_H
#define TEST_PERTINENTTIMESCALCULATOR_H


#include <iostream>
#include <memory>
#include <chrono>

#include "fructose/fructose.h"

#include "tucucore/intakeintervalcalculator.h"
#include "tucucore/intakeevent.h"

using namespace Tucuxi::Core;
using namespace Tucuxi::Common;


struct TestPertinentTimesCalculator : public fructose::test_base<TestPertinentTimesCalculator>
{
    TestPertinentTimesCalculator() { }

    /// \brief Test basic functionalities of a PkModel.
    void testStandard(const std::string& /* _testName */)
    {
        {
            PertinentTimesCalculatorStandard calculator;

            int nbPoints = 10;
            Eigen::VectorXd times(nbPoints);
            Duration interval(Duration(std::chrono::hours(24)));
            Duration infusionTime(Duration(std::chrono::minutes(0)));
            IntakeEvent intake(DateTime(), Duration(), 0.0, interval, Tucuxi::Core::FormulationAndRoute(AbsorptionModel::Extravascular), AbsorptionModel::Extravascular, infusionTime, nbPoints);
            calculator.calculateTimes(intake, nbPoints, times);

            double delta = (interval.toHours() / (nbPoints - 1));
            for (int i = 0; i < nbPoints; i++) {
                fructose_assert_double_eq(times[i], i * delta);
            }
        }
    }

    void testInfusion(const std::string& /* _testName */)
    {
        PertinentTimesCalculatorInfusion calculator;

        {
            // Test a linear time, as the second point is exactly at the end of infusion time
            int nbPoints = 25;
            Eigen::VectorXd times(nbPoints);
            Duration interval(Duration(std::chrono::hours(24)));
            Duration infusionTime(Duration(std::chrono::minutes(60)));
            IntakeEvent intake(DateTime(), Duration(), 0.0, interval, Tucuxi::Core::FormulationAndRoute(AbsorptionModel::Infusion), AbsorptionModel::Infusion, infusionTime, nbPoints);
            calculator.calculateTimes(intake, nbPoints, times);

            double delta = (interval.toHours() / (nbPoints - 1));
            for (int i = 0; i < nbPoints; i++) {
                fructose_assert_double_eq(times[i], i * delta);
            }
        }

        {
            // Test three points
            int nbPoints = 3;
            Eigen::VectorXd times(nbPoints);
            Duration interval(Duration(std::chrono::hours(24)));
            Duration infusionTime(Duration(std::chrono::minutes(60)));
            IntakeEvent intake(DateTime(), Duration(), 0.0, interval, Tucuxi::Core::FormulationAndRoute(AbsorptionModel::Infusion), AbsorptionModel::Infusion, infusionTime, nbPoints);
            calculator.calculateTimes(intake, nbPoints, times);

            fructose_assert_double_eq(times[0], 0.0);
            fructose_assert_double_eq(times[1], 1.0);
            fructose_assert_double_eq(times[2], 24.0);
        }

        {
            // Test five points
            int nbPoints = 5;
            Eigen::VectorXd times(nbPoints);
            Duration interval(Duration(std::chrono::hours(24)));
            Duration infusionTime(Duration(std::chrono::minutes(60)));
            IntakeEvent intake(DateTime(), Duration(), 0.0, interval, Tucuxi::Core::FormulationAndRoute(AbsorptionModel::Infusion), AbsorptionModel::Infusion, infusionTime, nbPoints);
            calculator.calculateTimes(intake, nbPoints, times);

            fructose_assert_double_eq(times[0], 0.0);
            fructose_assert_double_eq(times[1], 1.0);
            fructose_assert_double_eq(times[2], 1.0 + 23.0 / 3.0);
            fructose_assert_double_eq(times[3], 1.0 + 23.0 * 2.0 / 3.0);
            fructose_assert_double_eq(times[4], 24.0);
        }
        {
            // Test five points
            int nbPoints = 5;
            Eigen::VectorXd times(nbPoints);
            Duration interval(Duration(std::chrono::hours(24)));
            Duration infusionTime(Duration(std::chrono::minutes(60*10)));
            IntakeEvent intake(DateTime(), Duration(), 0.0, interval, Tucuxi::Core::FormulationAndRoute(AbsorptionModel::Infusion), AbsorptionModel::Infusion, infusionTime, nbPoints);
            calculator.calculateTimes(intake, nbPoints, times);

            fructose_assert_double_eq(times[0], 0.0);
            fructose_assert_double_eq(times[1], 10.0);
            fructose_assert_double_eq(times[2], 10.0 + 14.0/3.0);
            fructose_assert_double_eq(times[3], 10.0 + 28.0/3.0);
            fructose_assert_double_eq(times[4], 24.0);
        }
    }

};


#endif // TEST_PERTINENTTIMESCALCULATOR_H
