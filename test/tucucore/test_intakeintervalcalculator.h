/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TEST_INTAKEINTERVALCALCULATOR_H
#define TEST_INTAKEINTERVALCALCULATOR_H

#include <iostream>
#include <memory>

#include "fructose/fructose.h"

#include "tucucore/onecompartmentbolus.h"
#include "tucucore/onecompartmentextra.h"
#include "tucucore/onecompartmentintra.h"


using namespace Tucuxi::Core;

struct TestIntervalCalculator : public fructose::test_base<TestIntervalCalculator>
{

    const double DELTA = 0.000001;

    TestIntervalCalculator() { }



    /// \brief Test the residual calculation of Bolus. Compares single point vs multiple points
    /// \param _testName Test name.
    /// A calculator is instanciated, and residuals are computed with both
    ///     calculateIntakePoints and calculateIntakeSinglePoint.
    /// The residuals are then compared and shall be identical.
    void test1compBolusSingleVsMultiple(const std::string& /* _testName */)
    {

        Tucuxi::Core::IntakeIntervalCalculator::Result res;
        Tucuxi::Core::OneCompartmentBolus calculator;

        DateTime now;
        int nbPoints = 251;

        Tucuxi::Core::Concentrations concentrations;
        Tucuxi::Core::TimeOffsets times;
        Tucuxi::Core::IntakeEvent intakeEvent(now, 0s, 400, 12h, Tucuxi::Core::RouteOfAdministration::INTRAVASCULAR, 0s, nbPoints);
        Tucuxi::Core::ParameterList parameters;
        Tucuxi::Core::Residuals inResiduals;
        Tucuxi::Core::Residuals outResiduals1, outResiduals2;

        inResiduals.push_back(0);

        parameters.push_back(Tucuxi::Core::Parameter("CL", 15.106));
        parameters.push_back(Tucuxi::Core::Parameter("V", 347));

        res = calculator.calculateIntakePoints(
            concentrations,
            times,
            intakeEvent,
            parameters,
            inResiduals,
            nbPoints,
            outResiduals1,
            true);
        if (verbose()) {
            printf("Multiple Out residual = %f\n", outResiduals1[0]);
        }

        fructose_assert(res == Tucuxi::Core::IntakeIntervalCalculator::Result::Ok);

        res = calculator.calculateIntakeSinglePoint(
            concentrations,
            intakeEvent,
            parameters,
            inResiduals,
            nbPoints,
            outResiduals2);
        if (verbose()) {
            printf("Single   Out residual = %f\n", outResiduals1[0]);
        }

        fructose_assert(res == Tucuxi::Core::IntakeIntervalCalculator::Result::Ok);

        if (res == Tucuxi::Core::IntakeIntervalCalculator::Result::Ok) {
            for (int i = 0; i < 1; i++) {
                fructose_assert (abs(outResiduals1[i]/outResiduals2[i]-1) < DELTA);
            }
        }
    }


    /// \brief Test the residual calculation of extravascular. Compares single point vs multiple points
    /// \param _testName Test name.
    /// A calculator is instanciated, and residuals are computed with both
    ///     calculateIntakePoints and calculateIntakeSinglePoint.
    /// The residuals are then compared and shall be identical.
    void test1compExtraSingleVsMultiple(const std::string& /* _testName */)
    {

        Tucuxi::Core::IntakeIntervalCalculator::Result res;
        Tucuxi::Core::OneCompartmentExtra calculator;

        DateTime now;
        int nbPoints = 251;

        Tucuxi::Core::Concentrations concentrations;
        Tucuxi::Core::TimeOffsets times;
        Tucuxi::Core::IntakeEvent intakeEvent(now, 0s, 400, 12h, Tucuxi::Core::RouteOfAdministration::EXTRAVASCULAR, 0s, nbPoints);
        Tucuxi::Core::ParameterList parameters;
        Tucuxi::Core::Residuals inResiduals;
        Tucuxi::Core::Residuals outResiduals1, outResiduals2;

        inResiduals.push_back(0);
        inResiduals.push_back(0);

        parameters.push_back(Tucuxi::Core::Parameter("CL", 15.106));
        parameters.push_back(Tucuxi::Core::Parameter("F", 1));
        parameters.push_back(Tucuxi::Core::Parameter("Ka", 0.609));
        parameters.push_back(Tucuxi::Core::Parameter("V", 347));

        res = calculator.calculateIntakePoints(
            concentrations,
            times,
            intakeEvent,
            parameters,
            inResiduals,
            nbPoints,
            outResiduals1,
            true);

        if (verbose()) {
            printf("Multiple residual = %f\n", outResiduals1[0]);
            printf("Multiple residual = %f\n", outResiduals1[1]);
        }

        fructose_assert(res == Tucuxi::Core::IntakeIntervalCalculator::Result::Ok);

        res = calculator.calculateIntakeSinglePoint(
            concentrations,
            intakeEvent,
            parameters,
            inResiduals,
            nbPoints,
            outResiduals2);

        if (verbose()) {
            printf("Single   residual = %f\n", outResiduals1[0]);
            printf("Single   residual = %f\n", outResiduals1[1]);
        }

        fructose_assert(res == Tucuxi::Core::IntakeIntervalCalculator::Result::Ok);

        if (res == Tucuxi::Core::IntakeIntervalCalculator::Result::Ok) {
            for (int i = 0; i < 2; i++) {
                fructose_assert (abs(outResiduals1[i]/outResiduals2[i]-1) < DELTA);
            }
        }
    }


    /// \brief Test the residual calculation of infusion. Compares single point vs multiple points
    /// \param _testName Test name.
    /// A calculator is instanciated, and residuals are computed with both
    ///     calculateIntakePoints and calculateIntakeSinglePoint.
    /// The residuals are then compared and shall be identical.
    void test1compInfusionSingleVsMultiple(const std::string& /* _testName */)
    {

        Tucuxi::Core::IntakeIntervalCalculator::Result res;
        Tucuxi::Core::OneCompartmentIntra calculator;

        DateTime now;
        int nbPoints = 251;

        Tucuxi::Core::Concentrations concentrations;
        Tucuxi::Core::TimeOffsets times;
        Tucuxi::Core::IntakeEvent intakeEvent(now, 0s, 400, 12h, Tucuxi::Core::RouteOfAdministration::INFUSION, 1h, nbPoints);
        Tucuxi::Core::ParameterList parameters;
        Tucuxi::Core::Residuals inResiduals;
        Tucuxi::Core::Residuals outResiduals1, outResiduals2;

        inResiduals.push_back(0);

        parameters.push_back(Tucuxi::Core::Parameter("CL", 15.106));
        parameters.push_back(Tucuxi::Core::Parameter("V", 347));

        res = calculator.calculateIntakePoints(
            concentrations,
            times,
            intakeEvent,
            parameters,
            inResiduals,
            nbPoints,
            outResiduals1,
            true);

        if (verbose()) {
            printf("Multiple residual = %f\n", outResiduals1[0]);
        }

        fructose_assert(res == Tucuxi::Core::IntakeIntervalCalculator::Result::Ok);

        res = calculator.calculateIntakeSinglePoint(
            concentrations,
            intakeEvent,
            parameters,
            inResiduals,
            nbPoints,
            outResiduals2);

        if (verbose()) {
            printf("Single   residual = %f\n", outResiduals1[0]);
        }

        fructose_assert(res == Tucuxi::Core::IntakeIntervalCalculator::Result::Ok);

        if (res == Tucuxi::Core::IntakeIntervalCalculator::Result::Ok) {
            for (int i = 0; i < 1; i++) {
                fructose_assert (abs(outResiduals1[i]/outResiduals2[i]-1) < DELTA);
            }
        }
    }

};

#endif // TEST_INTAKEINTERVALCALCULATOR_H
