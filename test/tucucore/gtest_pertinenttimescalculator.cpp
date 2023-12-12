//@@license@@

#include <chrono>

#include <gtest/gtest.h>

#include "tucucore/intakeevent.h"
#include "tucucore/intakeintervalcalculator.h"

using namespace Tucuxi::Core;
using namespace Tucuxi::Common;

TEST (Core_TestPertinentTimesCalculator, Standard){
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
            ASSERT_DOUBLE_EQ(times[i], static_cast<double>(i) * delta);
        }
    }
}

TEST (Core_TestPertinentTimesCalculator, Infusion){
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
            ASSERT_DOUBLE_EQ(times[i], static_cast<double>(i) * delta);
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

        ASSERT_DOUBLE_EQ(times[0], 0.0);
        ASSERT_DOUBLE_EQ(times[1], 1.0);
        ASSERT_DOUBLE_EQ(times[2], 24.0);
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

        ASSERT_DOUBLE_EQ(times[0], 0.0);
        ASSERT_DOUBLE_EQ(times[1], 1.0);
        ASSERT_DOUBLE_EQ(times[2], 1.0 + 23.0 / 3.0);
        ASSERT_DOUBLE_EQ(times[3], 1.0 + 23.0 * 2.0 / 3.0);
        ASSERT_DOUBLE_EQ(times[4], 24.0);
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

        ASSERT_DOUBLE_EQ(times[0], 0.0);
        ASSERT_DOUBLE_EQ(times[1], 10.0);
        ASSERT_DOUBLE_EQ(times[2], 10.0 + 14.0 / 3.0);
        ASSERT_DOUBLE_EQ(times[3], 10.0 + 28.0 / 3.0);
        ASSERT_DOUBLE_EQ(times[4], 24.0);
    }
}
