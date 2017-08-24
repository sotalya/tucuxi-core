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


    template <class CalculatorMicroClass, class CalculatorMacroClass, int nbComp>
    void testSteadyState(const Tucuxi::Core::ParameterSetEvent &_microParameters,
                                       const Tucuxi::Core::ParameterSetEvent &_macroParameters,
                                       double _dose,
                                       Tucuxi::Core::RouteOfAdministration _route,
                                       std::chrono::hours _interval,
                                       std::chrono::seconds _infusionTime,
                                       int _nbPoints)
    {

        Tucuxi::Core::IntakeIntervalCalculator::Result res;
        CalculatorMicroClass microCalculator;
        CalculatorMacroClass macroCalculator;

        DateTime now;
        Tucuxi::Common::Duration offsetTime = 0s;
        Tucuxi::Common::Duration interval = _interval;
        Tucuxi::Common::Duration infusionTime = _infusionTime;

        Tucuxi::Core::Concentrations concentrations;
        Tucuxi::Core::TimeOffsets times;
        Tucuxi::Core::IntakeEvent intakeEvent(now, offsetTime, _dose, interval, _route, infusionTime, _nbPoints);

        // Checking if steady state is reached by iterative 100 times a calculation and
        // passing residuals to the next iteration
        Tucuxi::Core::Residuals inMicroResiduals, inMacroResiduals;
        Tucuxi::Core::Residuals outMicroResiduals, outMacroResiduals;

        for(unsigned int i = 0; i < microCalculator.getResidualSize(); i++)
	{
            outMicroResiduals.push_back(0);
            outMacroResiduals.push_back(0);
	}

        for(int cycle = 0; cycle < 100; cycle ++)
        {
	    	// Check Micro class
            inMicroResiduals = outMicroResiduals;
            res = microCalculator.calculateIntakeSinglePoint(
                        concentrations,
                        intakeEvent,
                        _microParameters,
                        inMicroResiduals,
                        interval.toHours(),
                        outMacroResiduals);
            fructose_assert(res == Tucuxi::Core::IntakeIntervalCalculator::Result::Ok);

	    // Check Macro class
            inMacroResiduals = outMacroResiduals;
            res = macroCalculator.calculateIntakeSinglePoint(
                        concentrations,
                        intakeEvent,
                        _macroParameters,
                        inMacroResiduals,
                        interval.toHours(),
                        outMacroResiduals);
            fructose_assert(res == Tucuxi::Core::IntakeIntervalCalculator::Result::Ok);
        }

        if (res == Tucuxi::Core::IntakeIntervalCalculator::Result::Ok) {
            for (unsigned int i = 0; i < microCalculator.getResidualSize(); i++) {
                fructose_assert_double_eq(inMicroResiduals[i], outMicroResiduals[i])
                fructose_assert_double_eq(inMacroResiduals[i], outMacroResiduals[i])
            }
        }

    }


    template <class CalculatorMicroClass,class CalculatorMacroClass, int nbComp>
    void testSingleVsMultiple(const Tucuxi::Core::ParameterSetEvent &_microParameters,
                                       const Tucuxi::Core::ParameterSetEvent &_macroParameters,
                                       double _dose,
                                       Tucuxi::Core::RouteOfAdministration _route,
                                       std::chrono::hours _interval,
                                       std::chrono::seconds _infusionTime,
                                       int _nbPoints)
    {

        Tucuxi::Core::IntakeIntervalCalculator::Result res;
        CalculatorMicroClass microCalculator;
        CalculatorMacroClass macroCalculator;

        DateTime now;
        Tucuxi::Common::Duration offsetTime = 0s;
        Tucuxi::Common::Duration interval = _interval;
        Tucuxi::Common::Duration infusionTime = _infusionTime;

        Tucuxi::Core::Concentrations concentrations;
        Tucuxi::Core::TimeOffsets times;
        Tucuxi::Core::IntakeEvent intakeEvent(now, offsetTime, _dose, interval, _route, infusionTime, _nbPoints);
        Tucuxi::Core::Residuals inResiduals;
        Tucuxi::Core::Residuals outMicroMultiResiduals, outMicroSingleResiduals, outMacroMultiResiduals, outMacroSingleResiduals;

        for(unsigned int i = 0; i < microCalculator.getResidualSize(); i++)
            inResiduals.push_back(0);

        // Calculation of Micro Class
        res = microCalculator.calculateIntakePoints(
                    concentrations,
                    times,
                    intakeEvent,
                    _microParameters,
                    inResiduals,
                    _nbPoints,
                    outMicroMultiResiduals,
                    true);
        if (verbose()) {
            for(unsigned int i = 0; i < microCalculator.getResidualSize(); i++) {
                std::cout << "Micro Multiple Out residual["
                          << i
                          << "] = "
                          << outMicroMultiResiduals[i]
                          << std::endl;
            }
        }

        fructose_assert(res == Tucuxi::Core::IntakeIntervalCalculator::Result::Ok);

        res = microCalculator.calculateIntakeSinglePoint(
                    concentrations,
                    intakeEvent,
                    _microParameters,
                    inResiduals,
                    interval.toHours(),
                    outMicroSingleResiduals);
        if (verbose()) {
            for(unsigned int i = 0; i < microCalculator.getResidualSize(); i++) {
                std::cout << "Micro Single   Out residual["
                          << i
                          << "] = "
                          << outMicroSingleResiduals[i]
                          << std::endl;
            }
        }

        fructose_assert(res == Tucuxi::Core::IntakeIntervalCalculator::Result::Ok);

        // Calculation of Macro Class
        res = macroCalculator.calculateIntakePoints(
                    concentrations,
                    times,
                    intakeEvent,
                    _macroParameters,
                    inResiduals,
                    _nbPoints,
                    outMacroMultiResiduals,
                    true);
        if (verbose()) {
            for(int i = 0; i < nbComp; i++) {
                std::cout << "Macro Multiple Out residual["
                          << i
                          << "] = "
                          << outMacroMultiResiduals[i]
                          << std::endl;
            }
        }

        fructose_assert(res == Tucuxi::Core::IntakeIntervalCalculator::Result::Ok);

        res = macroCalculator.calculateIntakeSinglePoint(
                    concentrations,
                    intakeEvent,
                    _macroParameters,
                    inResiduals,
                    interval.toHours(),
                    outMacroSingleResiduals);
        if (verbose()) {
            for(int i = 0; i < nbComp; i++) {
                std::cout << "Macro Single   Out residual["
                          << i
                          << "] = "
                          << outMacroSingleResiduals[i]
                          << std::endl;
            }
        }

        fructose_assert(res == Tucuxi::Core::IntakeIntervalCalculator::Result::Ok);

        if (res == Tucuxi::Core::IntakeIntervalCalculator::Result::Ok) {
            for (unsigned int i = 0; i < microCalculator.getResidualSize(); i++) {
                fructose_assert_double_eq(outMicroMultiResiduals[i], outMicroSingleResiduals[i])
                fructose_assert_double_eq(outMacroMultiResiduals[i], outMacroSingleResiduals[i])
                fructose_assert_double_eq_rel_abs(outMicroMultiResiduals[i], outMacroMultiResiduals[i], 0.001, 0.001)
                fructose_assert_double_eq_rel_abs(outMicroSingleResiduals[i], outMacroSingleResiduals[i], 0.001, 0.001)
            }
        }
    }


    template <class CalculatorMicroClass, class CalculatorMacroClass, int nbComp>
    void testCalculator(const Tucuxi::Core::ParameterSetEvent &_microParameters,
                                       const Tucuxi::Core::ParameterSetEvent &_macroParameters,
                                       double _dose,
                                       Tucuxi::Core::RouteOfAdministration _route,
                                       std::chrono::hours _interval,
                                       std::chrono::seconds _infusionTime,
                                       int _nbPoints)
    {

        testSingleVsMultiple<CalculatorMicroClass, CalculatorMacroClass, nbComp>(_microParameters, _macroParameters, _dose, _route, _interval, _infusionTime, _nbPoints);
        testSteadyState<CalculatorMicroClass, CalculatorMacroClass, nbComp>(_microParameters, _macroParameters, _dose, _route, _interval, _infusionTime, _nbPoints);
    }


    /// \brief Test the residual calculation of Bolus. Compares single point vs multiple points
    /// \param _testName Test name.
    /// A calculator is instanciated, and residuals are computed with both
    ///     calculateIntakePoints and calculateIntakeSinglePoint.
    /// The residuals are then compared and shall be identical.
    void test1compBolusSingleVsMultiple(const std::string& /* _testName */)
    {
        {
            Tucuxi::Core::ParameterDefinitions microParameterDefs;
            microParameterDefs.push_back(Tucuxi::Core::ParameterDefinition("V", 347, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            microParameterDefs.push_back(Tucuxi::Core::ParameterDefinition("Ke", 0.0435331, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            Tucuxi::Core::ParameterSetEvent microParameters(DateTime(), microParameterDefs);

            Tucuxi::Core::ParameterDefinitions macroParameterDefs;
            macroParameterDefs.push_back(Tucuxi::Core::ParameterDefinition("V", 347, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            macroParameterDefs.push_back(Tucuxi::Core::ParameterDefinition("Cl", 15.106, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            Tucuxi::Core::ParameterSetEvent macroParameters(DateTime(), macroParameterDefs);
            testCalculator<Tucuxi::Core::OneCompartmentBolusMicro, Tucuxi::Core::OneCompartmentBolusMacro, 1>
                    (microParameters,
					 macroParameters,
                     400.0,
                     Tucuxi::Core::RouteOfAdministration::INTRAVASCULAR,
                     1h,
                     0s,
                     250);
        }

    }

#if 0


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
#endif
};

#endif // TEST_INTAKEINTERVALCALCULATOR_H
