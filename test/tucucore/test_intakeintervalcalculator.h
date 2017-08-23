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
#include "tucucore/threecompartmentbolus.h"
#include "tucucore/threecompartmentextra.h"
#include "tucucore/threecompartmentinfusion.h"


using namespace Tucuxi::Core;

struct TestIntervalCalculator : public fructose::test_base<TestIntervalCalculator>
{

    const double DELTA = 0.000001;

    TestIntervalCalculator() { }


    template <class CalculatorClass>
    void testSteadyState(const Tucuxi::Core::ParameterSetEvent &_parameters,
                                       double _dose,
                                       Tucuxi::Core::RouteOfAdministration _route,
                                       std::chrono::hours _interval,
                                       std::chrono::seconds _infusionTime,
                                       int _nbPoints)
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

        // Checking if steady state is reached by iterative 100 times a calculation and
        // passing residuals to the next iteration
        Tucuxi::Core::Residuals inResiduals;
        Tucuxi::Core::Residuals outResiduals;

        for(unsigned int i = 0; i < calculator.getResidualSize(); i++)
            outResiduals.push_back(0);

        for(int cycle = 0; cycle < 100; cycle ++)
        {
            inResiduals = outResiduals;
            res = calculator.calculateIntakeSinglePoint(
                        concentrations,
                        intakeEvent,
                        _parameters,
                        inResiduals,
                        interval.toHours(),
                        outResiduals);
            fructose_assert(res == Tucuxi::Core::IntakeIntervalCalculator::Result::Ok);


        }

        if (res == Tucuxi::Core::IntakeIntervalCalculator::Result::Ok) {
            for (unsigned int i = 0; i < calculator.getResidualSize(); i++) {
                fructose_assert_double_eq(inResiduals[i], outResiduals[i])
            }
        }

    }


    template <class CalculatorClass>
    void testSingleVsMultiple(const Tucuxi::Core::ParameterSetEvent &_parameters,
                                       double _dose,
                                       Tucuxi::Core::RouteOfAdministration _route,
                                       std::chrono::hours _interval,
                                       std::chrono::seconds _infusionTime,
                                       int _nbPoints)
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
        Tucuxi::Core::Residuals inResiduals;
        Tucuxi::Core::Residuals outMultiResiduals, outSingleResiduals;

        for(unsigned int i = 0; i < calculator.getResidualSize(); i++)
            inResiduals.push_back(0);

        res = calculator.calculateIntakePoints(
                    concentrations,
                    times,
                    intakeEvent,
                    _parameters,
                    inResiduals,
                    _nbPoints,
                    outMultiResiduals,
                    true);
        if (verbose()) {
            for(unsigned int i = 0; i < calculator.getResidualSize(); i++) {
                std::cout << "Multiple Out residual["
                          << i
                          << "] = "
                          << outMultiResiduals[i]
                          << std::endl;
            }
        }

        fructose_assert(res == Tucuxi::Core::IntakeIntervalCalculator::Result::Ok);

        res = calculator.calculateIntakeSinglePoint(
                    concentrations,
                    intakeEvent,
                    _parameters,
                    inResiduals,
                    interval.toHours(),
                    outSingleResiduals);
        if (verbose()) {
            for(unsigned int i = 0; i < calculator.getResidualSize(); i++) {
                std::cout << "Single   Out residual["
                          << i
                          << "] = "
                          << outSingleResiduals[i]
                          << std::endl;
            }
        }

        fructose_assert(res == Tucuxi::Core::IntakeIntervalCalculator::Result::Ok);

        if (res == Tucuxi::Core::IntakeIntervalCalculator::Result::Ok) {
            for (unsigned int i = 0; i < calculator.getResidualSize(); i++) {
                fructose_assert_double_eq(outMultiResiduals[i], outSingleResiduals[i])
//                fructose_assert (abs(outMultiResiduals[i]/outSingleResiduals[i]-1.0) < DELTA);
            }
        }
    }


    template <class CalculatorClass>
    void testCalculator(const Tucuxi::Core::ParameterSetEvent &_parameters,
                                       double _dose,
                                       Tucuxi::Core::RouteOfAdministration _route,
                                       std::chrono::hours _interval,
                                       std::chrono::seconds _infusionTime,
                                       int _nbPoints)
    {

        testSingleVsMultiple<CalculatorClass>(_parameters, _dose, _route, _interval, _infusionTime, _nbPoints);
        testSteadyState<CalculatorClass>(_parameters, _dose, _route, _interval, _infusionTime, _nbPoints);
    }


    /// \brief Test the residual calculation of Bolus. Compares single point vs multiple points
    /// \param _testName Test name.
    /// A calculator is instanciated, and residuals are computed with both
    ///     calculateIntakePoints and calculateIntakeSinglePoint.
    /// The residuals are then compared and shall be identical.
    void test1compBolusSingleVsMultiple(const std::string& /* _testName */)
    {
        if (verbose()) {
	    std::cout << "Micro"<< std::endl;
        }

        {
            Tucuxi::Core::ParameterDefinitions parameterDefs;
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("V", 347, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("Ke", 0.0435331, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            Tucuxi::Core::ParameterSetEvent parameters(DateTime(), parameterDefs);

            testCalculator<Tucuxi::Core::OneCompartmentBolusMicro>
                    (parameters,
                     400.0,
                     Tucuxi::Core::RouteOfAdministration::INTRAVASCULAR,
                     1h,
                     0s,
                     250);
        }

        if (verbose()) {
	    std::cout << "\nMacro"<< std::endl;
        }

        {
            Tucuxi::Core::ParameterDefinitions parameterDefs;
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("V", 347, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("Cl", 15.106, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            Tucuxi::Core::ParameterSetEvent parameters(DateTime(), parameterDefs);

            testCalculator<Tucuxi::Core::OneCompartmentBolusMacro>
                    (parameters,
                     400.0,
                     Tucuxi::Core::RouteOfAdministration::INTRAVASCULAR,
                     1h,
                     0s,
                     250);
        }

    }


    /// \brief Test the residual calculation of extravascular. Compares single point vs multiple points
    /// \param _testName Test name.
    /// A calculator is instanciated, and residuals are computed with both
    ///     calculateIntakePoints and calculateIntakeSinglePoint.
    /// The residuals are then compared and shall be identical.
    void test1compExtraSingleVsMultiple(const std::string& /* _testName */)
    {
        if (verbose()) {
	    std::cout << "Micro"<< std::endl;
        }

        {
            Tucuxi::Core::ParameterDefinitions parameterDefs;
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("V", 347, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("Ke", 0.0435331, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("Ka", 0.609, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("F", 1, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            Tucuxi::Core::ParameterSetEvent parameters(DateTime(), parameterDefs);

            testCalculator<Tucuxi::Core::OneCompartmentExtraMicro>
                    (parameters,
                     400.0,
                     Tucuxi::Core::RouteOfAdministration::EXTRAVASCULAR,
                     12h,
                     0s,
                     250);
        }

        if (verbose()) {
	    std::cout << "\nMacro"<< std::endl;
        }

        {
            Tucuxi::Core::ParameterDefinitions parameterDefs;
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("V", 347, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("Cl", 15.106, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("Ka", 0.609, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("F", 1, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            Tucuxi::Core::ParameterSetEvent parameters(DateTime(), parameterDefs);

            testCalculator<Tucuxi::Core::OneCompartmentExtraMacro>
                    (parameters,
                     400.0,
                     Tucuxi::Core::RouteOfAdministration::EXTRAVASCULAR,
                     12h,
                     0s,
                     250);
        }

    }


    /// \brief Test the residual calculation of infusion. Compares single point vs multiple points
    /// \param _testName Test name.
    /// A calculator is instanciated, and residuals are computed with both
    ///     calculateIntakePoints and calculateIntakeSinglePoint.
    /// The residuals are then compared and shall be identical.
    void test1compInfusionSingleVsMultiple(const std::string& /* _testName */)
    {

        if (verbose()) {
	    std::cout << "Micro"<< std::endl;
        }

        {
            Tucuxi::Core::ParameterDefinitions parameterDefs;
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("Ke", 0.0435331, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("V", 347, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            Tucuxi::Core::ParameterSetEvent parameters(DateTime(), parameterDefs);

            testCalculator<Tucuxi::Core::OneCompartmentInfusionMicro>
                    (parameters,
                     400.0,
                     Tucuxi::Core::RouteOfAdministration::INFUSION,
                     12h,
                     1h,
                     250);
        }

        if (verbose()) {
	    std::cout << "\nMacro"<< std::endl;
        }

        {
            Tucuxi::Core::ParameterDefinitions parameterDefs;
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("Cl", 15.106, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("V", 347, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            Tucuxi::Core::ParameterSetEvent parameters(DateTime(), parameterDefs);

            testCalculator<Tucuxi::Core::OneCompartmentInfusionMacro>
                    (parameters,
                     400.0,
                     Tucuxi::Core::RouteOfAdministration::INFUSION,
                     12h,
                     1h,
                     250);
        }
    }

    /// \brief Test the residual calculation of Bolus. Compares single point vs multiple points
    /// \param _testName Test name.
    /// A calculator is instanciated, and residuals are computed with both
    ///     calculateIntakePoints and calculateIntakeSinglePoint.
    /// The residuals are then compared and shall be identical.
    void test2compBolusSingleVsMultiple(const std::string& /* _testName */)
    {
        if (verbose()) {
	    std::cout << "Micro"<< std::endl;
        }

        {
            Tucuxi::Core::ParameterDefinitions parameterDefs;
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("V1", 340, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("Ke", 0.0444294, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("K12", 0.0588235, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("K21", 0.0584795, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            Tucuxi::Core::ParameterSetEvent parameters(DateTime(), parameterDefs);

            testCalculator<Tucuxi::Core::TwoCompartmentBolusMicro>
                    (parameters,
                     400.0,
                     Tucuxi::Core::RouteOfAdministration::INTRAVASCULAR,
                     12h,
                     0s,
                     250);
        }

        if (verbose()) {
	    std::cout << "\nMacro"<< std::endl;
        }

        {
            Tucuxi::Core::ParameterDefinitions parameterDefs;
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("Cl", 15.106, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("Q", 20, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("V1", 340, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("V2", 342, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            Tucuxi::Core::ParameterSetEvent parameters(DateTime(), parameterDefs);

            testCalculator<Tucuxi::Core::TwoCompartmentBolusMacro>
                    (parameters,
                     400.0,
                     Tucuxi::Core::RouteOfAdministration::INTRAVASCULAR,
                     12h,
                     0s,
                     250);
        }
    }

    /// \brief Test the residual calculation of extravascular. Compares single point vs multiple points
    /// \param _testName Test name.
    /// A calculator is instanciated, and residuals are computed with both
    ///     calculateIntakePoints and calculateIntakeSinglePoint.
    /// The residuals are then compared and shall be identical.
    void test2compExtraSingleVsMultiple(const std::string& /* _testName */)
    {
        if (verbose()) {
	    std::cout << "Micro"<< std::endl;
        }

        {
            Tucuxi::Core::ParameterDefinitions parameterDefs;
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("V1", 340, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("Ke", 0.0444294, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("K12", 0.0588235, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("K21", 0.0584795, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("Ka", 0.609, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("F", 1, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            Tucuxi::Core::ParameterSetEvent parameters(DateTime(), parameterDefs);

            testCalculator<Tucuxi::Core::TwoCompartmentExtraMicro>
                    (parameters,
                     400.0,
                     Tucuxi::Core::RouteOfAdministration::EXTRAVASCULAR,
                     12h,
                     0s,
                     250);
        }

        if (verbose()) {
	    std::cout << "\nMacro"<< std::endl;
        }

        {
            Tucuxi::Core::ParameterDefinitions parameterDefs;
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("Cl", 15.106, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("Q", 20, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("V2", 342, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("V1", 340, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("Ka", 0.609, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("F", 1, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            Tucuxi::Core::ParameterSetEvent parameters(DateTime(), parameterDefs);

            testCalculator<Tucuxi::Core::TwoCompartmentExtraMacro>
                    (parameters,
                     400.0,
                     Tucuxi::Core::RouteOfAdministration::EXTRAVASCULAR,
                     12h,
                     0s,
                     250);
        }
    }

    /// \brief Test the residual calculation of infusion. Compares single point vs multiple points
    /// \param _testName Test name.
    /// A calculator is instanciated, and residuals are computed with both
    ///     calculateIntakePoints and calculateIntakeSinglePoint.
    /// The residuals are then compared and shall be identical.
    void test2compInfusionSingleVsMultiple(const std::string& /* _testName */)
    {
        if (verbose()) {
	    std::cout << "Micro"<< std::endl;
        }

        {
            Tucuxi::Core::ParameterDefinitions parameterDefs;
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("V1", 340, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("Ke", 0.0444294, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("K12", 0.0588235, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("K21", 0.0584795, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            Tucuxi::Core::ParameterSetEvent parameters(DateTime(), parameterDefs);

            testCalculator<Tucuxi::Core::TwoCompartmentInfusionMicro>
                    (parameters,
                     400.0,
                     Tucuxi::Core::RouteOfAdministration::INFUSION,
                     12h,
                     1h,
                     250);
        }

        if (verbose()) {
	    std::cout << "\nMacro"<< std::endl;
        }

        {
            Tucuxi::Core::ParameterDefinitions parameterDefs;
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("Cl", 15.106, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("Q", 20, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("V1", 340, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("V2", 342, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            Tucuxi::Core::ParameterSetEvent parameters(DateTime(), parameterDefs);

            testCalculator<Tucuxi::Core::TwoCompartmentInfusionMacro>
                    (parameters,
                     400.0,
                     Tucuxi::Core::RouteOfAdministration::INFUSION,
                     12h,
                     1h,
                     250);
        }
    }

    /// \brief Test the residual calculation of Bolus. Compares single point vs multiple points
    /// \param _testName Test name.
    /// A calculator is instanciated, and residuals are computed with both
    ///     calculateIntakePoints and calculateIntakeSinglePoint.
    /// The residuals are then compared and shall be identical.
    void test3compBolusSingleVsMultiple(const std::string& /* _testName */)
    {
        if (verbose()) {
	    std::cout << "Micro"<< std::endl;
        }

        {
            Tucuxi::Core::ParameterDefinitions parameterDefs;
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("F", 2, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("V1", 340, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("Ke", 0.0444294, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("K12", 0.0588235, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("K21", 0.0584795, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("K13", 0.0882353, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("K31", 0.0877193, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            Tucuxi::Core::ParameterSetEvent parameters(DateTime(), parameterDefs);

            testCalculator<Tucuxi::Core::ThreeCompartmentBolusMicro>
                    (parameters,
                     400.0,
                     Tucuxi::Core::RouteOfAdministration::INTRAVASCULAR,
                     12h,
                     0s,
                     250);
        }

        if (verbose()) {
	    std::cout << "\nMacro"<< std::endl;
        }

        {
            Tucuxi::Core::ParameterDefinitions parameterDefs;
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("Cl", 15.106, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("F", 2, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("Q1", 20, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("Q2", 30, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("V1", 340, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("V2", 342, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            Tucuxi::Core::ParameterSetEvent parameters(DateTime(), parameterDefs);

            testCalculator<Tucuxi::Core::ThreeCompartmentBolusMacro>
                    (parameters,
                     400.0,
                     Tucuxi::Core::RouteOfAdministration::INTRAVASCULAR,
                     12h,
                     0s,
                     250);
        }
    }

    /// \brief Test the residual calculation of Extra. Compares single point vs multiple points
    /// \param _testName Test name.
    /// A calculator is instanciated, and residuals are computed with both
    ///     calculateIntakePoints and calculateIntakeSinglePoint.
    /// The residuals are then compared and shall be identical.
    void test3compExtraSingleVsMultiple(const std::string& /* _testName */)
    {
        // Does not work now. To be checked

        /*
        if (verbose()) {
	    std::cout << "Micro"<< std::endl;
        }

        {
            Tucuxi::Core::ParameterDefinitions parameterDefs;
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("F", 2, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("V1", 340, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("Ka", 0.609, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("Ke", 0.0444294, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("K12", 0.0588235, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("K21", 0.0584795, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("K13", 0.0882353, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("K31", 0.0877193, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            Tucuxi::Core::ParameterSetEvent parameters(DateTime(), parameterDefs);

            testCalculator<Tucuxi::Core::ThreeCompartmentExtraMicro>
                    (parameters,
                     400.0,
                     Tucuxi::Core::RouteOfAdministration::INTRAVASCULAR,
                     12h,
                     0s,
                     250);
        }

        if (verbose()) {
	    std::cout << "\nMacro"<< std::endl;
        }

        {
            Tucuxi::Core::ParameterDefinitions parameterDefs;
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("Cl", 15.106, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("F", 2, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("Q1", 20, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("Q2", 30, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("V1", 340, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("V2", 342, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("Ka", 0.609, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            Tucuxi::Core::ParameterSetEvent parameters(DateTime(), parameterDefs);

            testCalculator<Tucuxi::Core::ThreeCompartmentExtraMacro>
                    (parameters,
                     400.0,
                     Tucuxi::Core::RouteOfAdministration::INTRAVASCULAR,
                     12h,
                     0s,
                     250);
        }
        */
    }


    /// \brief Test the residual calculation of Infusion. Compares single point vs multiple points
    /// \param _testName Test name.
    /// A calculator is instanciated, and residuals are computed with both
    ///     calculateIntakePoints and calculateIntakeSinglePoint.
    /// The residuals are then compared and shall be identical.
    void test3compInfusionSingleVsMultiple(const std::string& /* _testName */)
    {
        if (verbose()) {
	    std::cout << "Micro"<< std::endl;
        }

        {
            Tucuxi::Core::ParameterDefinitions parameterDefs;
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("F", 2, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("V1", 340, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("Ke", 0.0444294, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("K12", 0.0588235, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("K21", 0.0584795, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("K13", 0.0882353, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("K31", 0.0877193, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            Tucuxi::Core::ParameterSetEvent parameters(DateTime(), parameterDefs);

            testCalculator<Tucuxi::Core::ThreeCompartmentInfusionMicro>
                    (parameters,
                     400.0,
                     Tucuxi::Core::RouteOfAdministration::INTRAVASCULAR,
                     12h,
                     1h,
                     250);
        }

        if (verbose()) {
	    std::cout << "\nMacro"<< std::endl;
        }

        {
            Tucuxi::Core::ParameterDefinitions parameterDefs;
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("Cl", 15.106, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("F", 2, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("Q1", 20, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("Q2", 30, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("V1", 340, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("V2", 342, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            Tucuxi::Core::ParameterSetEvent parameters(DateTime(), parameterDefs);

            testCalculator<Tucuxi::Core::ThreeCompartmentInfusionMacro>
                    (parameters,
                     400.0,
                     Tucuxi::Core::RouteOfAdministration::INTRAVASCULAR,
                     12h,
                     1h,
                     250);
        }
    }
};

#endif // TEST_INTAKEINTERVALCALCULATOR_H
