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
#include "tucucore/onecompartmentinfusion.h"
#include "tucucore/twocompartmentbolus.h"
#include "tucucore/twocompartmentextra.h"
#include "tucucore/twocompartmentinfusion.h"


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
        Tucuxi::Core::OneCompartmentBolusMicro calculator;

        DateTime now;
        int nbPoints = 251;
        Tucuxi::Common::Duration offsetTime = 0s;
        Tucuxi::Common::Duration interval = 1h;
        Tucuxi::Common::Duration infusionTime = 0s;

        Tucuxi::Core::Concentrations concentrations;
        Tucuxi::Core::TimeOffsets times;
        Tucuxi::Core::IntakeEvent intakeEvent(now, offsetTime, 400, interval, Tucuxi::Core::RouteOfAdministration::INTRAVASCULAR, infusionTime, nbPoints);
        Tucuxi::Core::ParameterSetEvent parameters;
        Tucuxi::Core::Residuals inResiduals;
        Tucuxi::Core::Residuals outMultiResiduals, outSingleResiduals;

        inResiduals.push_back(0);

        parameters.push_back(Tucuxi::Core::Parameter("V", 347));
        // TODO: check whether input needs to be changed to Cl or not
        parameters.push_back(Tucuxi::Core::Parameter("Ke", 15.106));

        res = calculator.calculateIntakePoints(
                    concentrations,
                    times,
                    intakeEvent,
                    parameters,
                    inResiduals,
                    nbPoints,
                    outMultiResiduals,
                    true);
        if (verbose()) {
            printf("Multiple Out residual = %f\n", outMultiResiduals[0]);
        }

        fructose_assert(res == Tucuxi::Core::IntakeIntervalCalculator::Result::Ok);

        res = calculator.calculateIntakeSinglePoint(
                    concentrations,
                    intakeEvent,
                    parameters,
                    inResiduals,
                    interval.toHours(),
                    outSingleResiduals);
        if (verbose()) {
            printf("Single   Out residual = %f\n", outSingleResiduals[0]);
        }

        fructose_assert(res == Tucuxi::Core::IntakeIntervalCalculator::Result::Ok);

        if (res == Tucuxi::Core::IntakeIntervalCalculator::Result::Ok) {
            for (int i = 0; i < 1; i++) {
                fructose_assert (abs(outMultiResiduals[i]/outSingleResiduals[i]-1) < DELTA);
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
        Tucuxi::Core::OneCompartmentExtraMacro calculator;

        DateTime now;
        int nbPoints = 251;
        Tucuxi::Common::Duration offsetTime = 0s;
        Tucuxi::Common::Duration interval = 12h;
        Tucuxi::Common::Duration infusionTime = 0s;

        Tucuxi::Core::Concentrations concentrations;
        Tucuxi::Core::TimeOffsets times;
        Tucuxi::Core::IntakeEvent intakeEvent(now, offsetTime, 400, interval, Tucuxi::Core::RouteOfAdministration::EXTRAVASCULAR, infusionTime, nbPoints);
        Tucuxi::Core::ParameterList parameters;
        Tucuxi::Core::Residuals inResiduals;
        Tucuxi::Core::Residuals outMultiResiduals, outSingleResiduals;

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
                    outMultiResiduals,
                    true);

        if (verbose()) {
            printf("Multiple residual = %f\n", outMultiResiduals[0]);
            printf("Multiple residual = %f\n", outMultiResiduals[1]);
        }

        fructose_assert(res == Tucuxi::Core::IntakeIntervalCalculator::Result::Ok);

        res = calculator.calculateIntakeSinglePoint(
                    concentrations,
                    intakeEvent,
                    parameters,
                    inResiduals,
                    nbPoints,
                    outSingleResiduals);

        if (verbose()) {
            printf("Single   residual = %f\n", outSingleResiduals[0]);
            printf("Single   residual = %f\n", outSingleResiduals[1]);
        }

        fructose_assert(res == Tucuxi::Core::IntakeIntervalCalculator::Result::Ok);

        if (res == Tucuxi::Core::IntakeIntervalCalculator::Result::Ok) {
            for (int i = 0; i < 2; i++) {
                fructose_assert (abs(outMultiResiduals[i]/outSingleResiduals[i]-1) < DELTA);
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
        Tucuxi::Core::OneCompartmentInfusionMacro calculator;

        DateTime now;
        int nbPoints = 251;
        Tucuxi::Common::Duration offsetTime = 0s;
        Tucuxi::Common::Duration interval = 12h;
        Tucuxi::Common::Duration infusionTime = 1h;


        Tucuxi::Core::Concentrations concentrations;
        Tucuxi::Core::TimeOffsets times;
        Tucuxi::Core::IntakeEvent intakeEvent(now, offsetTime, 400, interval, Tucuxi::Core::RouteOfAdministration::INFUSION, infusionTime, nbPoints);
        Tucuxi::Core::ParameterList parameters;
        Tucuxi::Core::Residuals inResiduals;
        Tucuxi::Core::Residuals outMultiResiduals, outSingleResiduals;

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
                    outMultiResiduals,
                    true);

        if (verbose()) {
            printf("Multiple residual = %f\n", outMultiResiduals[0]);
        }

        fructose_assert(res == Tucuxi::Core::IntakeIntervalCalculator::Result::Ok);

        res = calculator.calculateIntakeSinglePoint(
                    concentrations,
                    intakeEvent,
                    parameters,
                    inResiduals,
                    nbPoints,
                    outSingleResiduals);

        if (verbose()) {
            printf("Single   residual = %f\n", outSingleResiduals[0]);
        }

        fructose_assert(res == Tucuxi::Core::IntakeIntervalCalculator::Result::Ok);

        if (res == Tucuxi::Core::IntakeIntervalCalculator::Result::Ok) {
            for (int i = 0; i < 1; i++) {
                fructose_assert (abs(outMultiResiduals[i]/outSingleResiduals[i]-1) < DELTA);
            }
        }
    }

    /// \brief Test the residual calculation of Bolus. Compares single point vs multiple points
    /// \param _testName Test name.
    /// A calculator is instanciated, and residuals are computed with both
    ///     calculateIntakePoints and calculateIntakeSinglePoint.
    /// The residuals are then compared and shall be identical.
    void test2compBolusSingleVsMultiple(const std::string& /* _testName */)
    {
        Tucuxi::Core::IntakeIntervalCalculator::Result res;
        Tucuxi::Core::TwoCompartmentBolusMacro calculator;

        DateTime now;
        int nbPoints = 251;
        Tucuxi::Common::Duration offsetTime = 0s;
        Tucuxi::Common::Duration interval = 12h;
        Tucuxi::Common::Duration infusionTime = 0s;


        Tucuxi::Core::Concentrations concentrations;
        Tucuxi::Core::TimeOffsets times;
        Tucuxi::Core::IntakeEvent intakeEvent(now, offsetTime, 400, interval, Tucuxi::Core::RouteOfAdministration::INTRAVASCULAR, infusionTime, nbPoints);
        Tucuxi::Core::ParameterList parameters;
        Tucuxi::Core::Residuals inResiduals;
        Tucuxi::Core::Residuals outMultiResiduals, outSingleResiduals;

        inResiduals.push_back(0);

        parameters.push_back(Tucuxi::Core::Parameter("CL", 15.106));
        parameters.push_back(Tucuxi::Core::Parameter("Q", 20));
        parameters.push_back(Tucuxi::Core::Parameter("V1", 340));
        parameters.push_back(Tucuxi::Core::Parameter("V2", 342));

        res = calculator.calculateIntakePoints(
                    concentrations,
                    times,
                    intakeEvent,
                    parameters,
                    inResiduals,
                    nbPoints,
                    outMultiResiduals,
                    true);
        if (verbose()) {
            printf("Multiple Out residual = %f\n", outMultiResiduals[0]);
            printf("Multiple Out residual = %f\n", outMultiResiduals[1]);
        }

        fructose_assert(res == Tucuxi::Core::IntakeIntervalCalculator::Result::Ok);

        res = calculator.calculateIntakeSinglePoint(
                    concentrations,
                    intakeEvent,
                    parameters,
                    inResiduals,
                    nbPoints,
                    outSingleResiduals);
        if (verbose()) {
            printf("Single   Out residual = %f\n", outSingleResiduals[0]);
            printf("Single   Out residual = %f\n", outSingleResiduals[1]);
        }

        fructose_assert(res == Tucuxi::Core::IntakeIntervalCalculator::Result::Ok);

        if (res == Tucuxi::Core::IntakeIntervalCalculator::Result::Ok) {
            for (int i = 0; i < 1; i++) {
                fructose_assert (abs(outMultiResiduals[i]/outSingleResiduals[i]-1) < DELTA);
            }
        }
    }

    /// \brief Test the residual calculation of extravascular. Compares single point vs multiple points
    /// \param _testName Test name.
    /// A calculator is instanciated, and residuals are computed with both
    ///     calculateIntakePoints and calculateIntakeSinglePoint.
    /// The residuals are then compared and shall be identical.
    void test2compExtraSingleVsMultiple(const std::string& /* _testName */)
    {

        Tucuxi::Core::IntakeIntervalCalculator::Result res;
        Tucuxi::Core::TwoCompartmentExtraMacro calculator;

        DateTime now;
        int nbPoints = 251;
        Tucuxi::Common::Duration offsetTime = 0s;
        Tucuxi::Common::Duration interval = 12h;
        Tucuxi::Common::Duration infusionTime = 0s;


        Tucuxi::Core::Concentrations concentrations;
        Tucuxi::Core::TimeOffsets times;
        Tucuxi::Core::IntakeEvent intakeEvent(now, offsetTime, 400, interval, Tucuxi::Core::RouteOfAdministration::EXTRAVASCULAR, infusionTime, nbPoints);
        Tucuxi::Core::ParameterList parameters;
        Tucuxi::Core::Residuals inResiduals;
        Tucuxi::Core::Residuals outMultiResiduals, outSingleResiduals;

        inResiduals.push_back(0);
        inResiduals.push_back(0);

        parameters.push_back(Tucuxi::Core::Parameter("CL", 15.106));
        parameters.push_back(Tucuxi::Core::Parameter("F", 1));
        parameters.push_back(Tucuxi::Core::Parameter("Ka", 0.609));
        parameters.push_back(Tucuxi::Core::Parameter("Q", 20));
        parameters.push_back(Tucuxi::Core::Parameter("V1", 347));
        parameters.push_back(Tucuxi::Core::Parameter("V2", 200));

        res = calculator.calculateIntakePoints(
                    concentrations,
                    times,
                    intakeEvent,
                    parameters,
                    inResiduals,
                    nbPoints,
                    outMultiResiduals,
                    true);

        if (verbose()) {
            printf("Multiple residual = %f\n", outMultiResiduals[0]);
            printf("Multiple residual = %f\n", outMultiResiduals[1]);
            printf("Multiple residual = %f\n", outMultiResiduals[2]);
        }

        fructose_assert(res == Tucuxi::Core::IntakeIntervalCalculator::Result::Ok);

        res = calculator.calculateIntakeSinglePoint(
                    concentrations,
                    intakeEvent,
                    parameters,
                    inResiduals,
                    nbPoints,
                    outSingleResiduals);

        if (verbose()) {
            printf("Single   residual = %f\n", outSingleResiduals[0]);
            printf("Single   residual = %f\n", outSingleResiduals[1]);
            printf("Multiple residual = %f\n", outSingleResiduals[2]);
        }

        fructose_assert(res == Tucuxi::Core::IntakeIntervalCalculator::Result::Ok);

        if (res == Tucuxi::Core::IntakeIntervalCalculator::Result::Ok) {
            for (int i = 0; i < 3; i++) {
                fructose_assert (abs(outMultiResiduals[i]/outSingleResiduals[i]-1) < DELTA);
            }
        }
    }

    /// \brief Test the residual calculation of infusion. Compares single point vs multiple points
    /// \param _testName Test name.
    /// A calculator is instanciated, and residuals are computed with both
    ///     calculateIntakePoints and calculateIntakeSinglePoint.
    /// The residuals are then compared and shall be identical.
    void test2compInfusionSingleVsMultiple(const std::string& /* _testName */)
    {

        Tucuxi::Core::IntakeIntervalCalculator::Result res;
        Tucuxi::Core::TwoCompartmentInfusion calculator;

        DateTime now;
        int nbPoints = 251;
        Tucuxi::Common::Duration offsetTime = 0s;
        Tucuxi::Common::Duration interval = 12h;
        Tucuxi::Common::Duration infusionTime = 1h;


        Tucuxi::Core::Concentrations concentrations;
        Tucuxi::Core::TimeOffsets times;
        Tucuxi::Core::IntakeEvent intakeEvent(now, offsetTime, 400, interval, Tucuxi::Core::RouteOfAdministration::INFUSION, infusionTime, nbPoints);
        Tucuxi::Core::ParameterList parameters;
        Tucuxi::Core::Residuals inResiduals;
        Tucuxi::Core::Residuals outMultiResiduals, outSingleResiduals;

        inResiduals.push_back(0);

        parameters.push_back(Tucuxi::Core::Parameter("CL", 15.106));
        parameters.push_back(Tucuxi::Core::Parameter("Q", 20));
        parameters.push_back(Tucuxi::Core::Parameter("V1", 347));
        parameters.push_back(Tucuxi::Core::Parameter("V2", 200));

        res = calculator.calculateIntakePoints(
                    concentrations,
                    times,
                    intakeEvent,
                    parameters,
                    inResiduals,
                    nbPoints,
                    outMultiResiduals,
                    true);

        if (verbose()) {
            printf("Multiple residual = %f\n", outMultiResiduals[0]);
            printf("Multiple residual = %f\n", outMultiResiduals[1]);
        }

        fructose_assert(res == Tucuxi::Core::IntakeIntervalCalculator::Result::Ok);

        res = calculator.calculateIntakeSinglePoint(
                    concentrations,
                    intakeEvent,
                    parameters,
                    inResiduals,
                    nbPoints,
                    outSingleResiduals);

        if (verbose()) {
            printf("Single   residual = %f\n", outSingleResiduals[0]);
            printf("Multiple residual = %f\n", outSingleResiduals[1]);
        }

        fructose_assert(res == Tucuxi::Core::IntakeIntervalCalculator::Result::Ok);

        if (res == Tucuxi::Core::IntakeIntervalCalculator::Result::Ok) {
            for (int i = 0; i < 1; i++) {
                fructose_assert (abs(outMultiResiduals[i]/outSingleResiduals[i]-1) < DELTA);
            }
        }
    }

};

#endif // TEST_INTAKEINTERVALCALCULATOR_H
