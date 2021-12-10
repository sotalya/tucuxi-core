/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TEST_INTAKEINTERVALCALCULATOR_H
#define TEST_INTAKEINTERVALCALCULATOR_H

#include <iostream>
#include <memory>

#include "tucucommon/duration.h"

#include "tucucore/drugmodel/formulationandroute.h"
#include "tucucore/intakeevent.h"
#include "tucucore/pkmodels/onecompartmentbolus.h"
#include "tucucore/pkmodels/onecompartmentextra.h"
#include "tucucore/pkmodels/onecompartmentextralag.h"
#include "tucucore/pkmodels/onecompartmentinfusion.h"
#include "tucucore/pkmodels/rkonecompartmentextra.h"
#include "tucucore/pkmodels/rkonecompartmentgammaextra.h"
#include "tucucore/pkmodels/rktwocompartmenterlang.h"
#include "tucucore/pkmodels/rktwocompartmentextralag.h"
#include "tucucore/pkmodels/threecompartmentbolus.h"
#include "tucucore/pkmodels/threecompartmentextra.h"
#include "tucucore/pkmodels/threecompartmentinfusion.h"
#include "tucucore/pkmodels/twocompartmentbolus.h"
#include "tucucore/pkmodels/twocompartmentextra.h"
#include "tucucore/pkmodels/twocompartmentextralag.h"
#include "tucucore/pkmodels/twocompartmentinfusion.h"

#include "fructose/fructose.h"

using namespace Tucuxi::Core;
using namespace std::chrono_literals;

struct TestIntervalCalculator : public fructose::test_base<TestIntervalCalculator>
{
    static const int CYCLE_SIZE = 251;

    const double DELTA = 0.000001;
    const unsigned int maxResidualSize = 3;

    TestIntervalCalculator() {}

    template<class CalculatorMicroClass, class CalculatorMacroClass>
    void testSteadyState(
            const Tucuxi::Core::ParameterSetEvent& _microParameters,
            const Tucuxi::Core::ParameterSetEvent& _macroParameters,
            double _dose,
            Tucuxi::Core::AbsorptionModel _route,
            std::chrono::hours _interval,
            std::chrono::seconds _infusionTime,
            size_t _nbPoints)
    {

        Tucuxi::Core::ComputingStatus res;
        CalculatorMicroClass microCalculator;
        CalculatorMacroClass macroCalculator;

        DateTime now = DateTime::now();
        Tucuxi::Common::Duration offsetTime = 0s;
        Tucuxi::Common::Duration interval = _interval;
        Tucuxi::Common::Duration infusionTime = _infusionTime;

        unsigned int residualSize = (microCalculator.getResidualSize() == microCalculator.getResidualSize())
                                            ? microCalculator.getResidualSize()
                                            : maxResidualSize;
        bool isAll = false;

        std::vector<Tucuxi::Core::Concentrations> concentrations;
        concentrations.resize(residualSize);

        Tucuxi::Core::TimeOffsets times;
        Tucuxi::Core::IntakeEvent intakeEvent(
                now,
                offsetTime,
                _dose,
                TucuUnit("mg"),
                interval,
                Tucuxi::Core::FormulationAndRoute(_route),
                _route,
                infusionTime,
                _nbPoints);

        // Checking if steady state is reached by iterative 100 times a calculation and
        // passing residuals to the next iteration
        Tucuxi::Core::Residuals inMicroResiduals(residualSize), inMacroResiduals(residualSize),
                outMicroResiduals(residualSize), outMacroResiduals(residualSize);

        std::fill(outMicroResiduals.begin(), outMicroResiduals.end(), 0);
        std::fill(outMacroResiduals.begin(), outMacroResiduals.end(), 0);

        for (unsigned int cycle = 0; cycle < 300; cycle++) {
            // Check Micro class
            inMicroResiduals = outMicroResiduals;
            res = microCalculator.calculateIntakeSinglePoint(
                    concentrations,
                    intakeEvent,
                    _microParameters,
                    inMicroResiduals,
                    interval.toHours(),
                    isAll,
                    outMicroResiduals);
            fructose_assert(res == Tucuxi::Core::ComputingStatus::Ok);

            // Check Macro class
            inMacroResiduals = outMacroResiduals;
            res = macroCalculator.calculateIntakeSinglePoint(
                    concentrations,
                    intakeEvent,
                    _macroParameters,
                    inMacroResiduals,
                    interval.toHours(),
                    isAll,
                    outMacroResiduals);
            fructose_assert(res == Tucuxi::Core::ComputingStatus::Ok);

#if 0
            std::cout << "[" << cycle << "]";
            for (unsigned int i = 0; i < residualSize; i++) {
                std::cout << inMicroResiduals[i] << ":" << outMicroResiduals[i] << " " << inMacroResiduals[i] << ":" << outMacroResiduals[i] << " ";
            }
            std::cout << std::endl;
#endif
        }

        if (res == Tucuxi::Core::ComputingStatus::Ok) {
            for (unsigned int i = 0; i < residualSize; i++) {
                fructose_assert_double_eq_rel_abs(inMicroResiduals[i], outMicroResiduals[i], 0.01, 0.01)
                        fructose_assert_double_eq_rel_abs(inMacroResiduals[i], outMacroResiduals[i], 0.01, 0.01)
            }
        }
    }


    template<class CalculatorMicroClass, class CalculatorMacroClass>
    void testSingleVsMultiple(
            const Tucuxi::Core::ParameterSetEvent& _microParameters,
            const Tucuxi::Core::ParameterSetEvent& _macroParameters,
            double _dose,
            Tucuxi::Core::AbsorptionModel _route,
            std::chrono::hours _interval,
            std::chrono::seconds _infusionTime,
            CycleSize _nbPoints)
    {
        Tucuxi::Core::ComputingStatus res;
        CalculatorMicroClass microCalculator;
        CalculatorMacroClass macroCalculator;

        DateTime now = DateTime::now();
        Tucuxi::Common::Duration offsetTime = 0s;
        Tucuxi::Common::Duration interval = _interval;
        Tucuxi::Common::Duration infusionTime = _infusionTime;

        unsigned int residualSize = (microCalculator.getResidualSize() == microCalculator.getResidualSize())
                                            ? microCalculator.getResidualSize()
                                            : maxResidualSize;
        bool isAll = false;

        std::vector<Tucuxi::Core::Concentrations> concentrations;
        concentrations.resize(residualSize);

        std::vector<Tucuxi::Core::Concentrations> concentrationsInterval2;
        concentrationsInterval2.resize(residualSize);

        std::vector<Tucuxi::Core::Concentrations> concentrations2;
        concentrations2.resize(residualSize);

        std::vector<Tucuxi::Core::Concentrations> concentrations2Interval2;
        concentrations2Interval2.resize(residualSize);

        Tucuxi::Core::TimeOffsets times;
        Tucuxi::Core::IntakeEvent intakeEvent(
                now,
                offsetTime,
                _dose,
                TucuUnit("mg"),
                interval,
                Tucuxi::Core::FormulationAndRoute(_route),
                _route,
                infusionTime,
                _nbPoints);

        // Start with an empty residual
        const Tucuxi::Core::Residuals firstInResidual(residualSize, 0);
        Tucuxi::Core::Residuals outMicroMultiResiduals(residualSize);
        Tucuxi::Core::Residuals outMicroSingleResiduals(residualSize);
        Tucuxi::Core::Residuals outMacroMultiResiduals(residualSize);
        Tucuxi::Core::Residuals outMacroMultiResiduals2(residualSize);
        Tucuxi::Core::Residuals outMacroSingleResiduals(residualSize);
        Tucuxi::Core::Residuals outMacroSingleResiduals2(residualSize);

        // Calculation of Micro Class
        res = microCalculator.calculateIntakePoints(
                concentrations,
                times,
                intakeEvent,
                _microParameters,
                firstInResidual,
                isAll,
                outMicroMultiResiduals,
                true);

        if (verbose()) {
            std::cout << "[Micro Class Calculation]" << std::endl;
            for (unsigned int i = 0; i < residualSize; i++) {
                std::cout << "Multiple Out residual[" << i << "] = " << outMicroMultiResiduals[i] << std::endl;
            }
        }

        fructose_assert(res == Tucuxi::Core::ComputingStatus::Ok);

        res = microCalculator.calculateIntakeSinglePoint(
                concentrations,
                intakeEvent,
                _microParameters,
                firstInResidual,
                interval.toHours(),
                isAll,
                outMicroSingleResiduals);

        if (verbose()) {
            for (unsigned int i = 0; i < residualSize; i++) {
                std::cout << "Single   Out residual[" << i << "] = " << outMicroSingleResiduals[i] << std::endl;
            }
        }

        fructose_assert(res == Tucuxi::Core::ComputingStatus::Ok);

        // Calculation of Macro Class
        res = macroCalculator.calculateIntakePoints(
                concentrations,
                times,
                intakeEvent,
                _macroParameters,
                firstInResidual,
                isAll,
                outMacroMultiResiduals,
                true);

        if (verbose()) {
            std::cout << "\n[Macro Class Calculation]" << std::endl;
            for (unsigned int i = 0; i < residualSize; i++) {
                std::cout << "Multiple Out residual[" << i << "] = " << outMacroMultiResiduals[i] << std::endl;
            }
        }

        fructose_assert(res == Tucuxi::Core::ComputingStatus::Ok);


        // Just check the residuals
        res = macroCalculator.calculateIntakeSinglePoint(
                concentrations2,
                intakeEvent,
                _macroParameters,
                firstInResidual,
                interval.toHours(),
                isAll,
                outMacroSingleResiduals);

        if (verbose()) {
            for (unsigned int i = 0; i < residualSize; i++) {
                std::cout << "Single   Out residual[" << i << "] = " << outMacroSingleResiduals[i] << std::endl;
            }
        }

        fructose_assert(res == Tucuxi::Core::ComputingStatus::Ok);

        if (res == Tucuxi::Core::ComputingStatus::Ok) {
            for (unsigned int i = 0; i < residualSize; i++) {
                fructose_assert_double_eq_rel_abs(outMicroMultiResiduals[i], outMicroSingleResiduals[i], 0.0001, 0.0)
                        fructose_assert_double_eq_rel_abs(
                                outMacroMultiResiduals[i], outMacroSingleResiduals[i], 0.0001, 0.0)
                                fructose_assert_double_eq_rel_abs(
                                        outMicroMultiResiduals[i], outMacroMultiResiduals[i], 0.001, 0.001)
                                        fructose_assert_double_eq_rel_abs(
                                                outMicroSingleResiduals[i], outMacroSingleResiduals[i], 0.001, 0.001)
            }
        }


        // Then a full check on all points
        for (size_t t = 0; t < times.size(); t++) {

            std::vector<Tucuxi::Core::Concentrations> concentrations3;
            concentrations3.resize(residualSize);

            Tucuxi::Core::Residuals outResiduals(residualSize);

            res = macroCalculator.calculateIntakeSinglePoint(
                    concentrations3, intakeEvent, _macroParameters, firstInResidual, times[t], isAll, outResiduals);

            fructose_assert(res == Tucuxi::Core::ComputingStatus::Ok);

            if (res == Tucuxi::Core::ComputingStatus::Ok) {
                fructose_assert_double_eq_rel_abs(concentrations[0][t], concentrations3[0][0], 0.0001, 0.0);
            }
        }



        // Calculation of Macro Class with the previous residual
        res = macroCalculator.calculateIntakePoints(
                concentrationsInterval2,
                times,
                intakeEvent,
                _macroParameters,
                outMacroMultiResiduals,
                isAll,
                outMacroMultiResiduals2,
                true);

        if (verbose()) {
            std::cout << "\n[Macro Class Calculation]" << std::endl;
            for (unsigned int i = 0; i < residualSize; i++) {
                std::cout << "Multiple Out residual[" << i << "] = " << outMacroMultiResiduals[i] << std::endl;
            }
        }

        fructose_assert(res == Tucuxi::Core::ComputingStatus::Ok);



        // Then a full check on all points
        for (size_t t = 0; t < times.size(); t++) {

            std::vector<Tucuxi::Core::Concentrations> concentrations3;
            concentrations3.resize(residualSize);

            res = macroCalculator.calculateIntakeSinglePoint(
                    concentrations3,
                    intakeEvent,
                    _macroParameters,
                    outMacroSingleResiduals,
                    times[t],
                    isAll,
                    outMacroSingleResiduals2);

            fructose_assert(res == Tucuxi::Core::ComputingStatus::Ok);

            if (res == Tucuxi::Core::ComputingStatus::Ok) {
                fructose_assert_double_eq_rel_abs(concentrationsInterval2[0][t], concentrations3[0][0], 0.0001, 0.0);
            }
        }
    }


    template<class CalculatorClass0, class CalculatorClass1>
    void testCompare(
            const Tucuxi::Core::ParameterSetEvent& _Parameters,
            double _dose,
            Tucuxi::Core::AbsorptionModel _route,
            std::chrono::hours _interval,
            std::chrono::seconds _infusionTime,
            size_t _nbPoints)
    {
        Tucuxi::Core::ComputingStatus res;
        CalculatorClass0 calculator0;
        CalculatorClass1 calculator1;

        DateTime now = DateTime::now();
        Tucuxi::Common::Duration offsetTime = 0s;
        Tucuxi::Common::Duration interval = _interval;
        Tucuxi::Common::Duration infusionTime = _infusionTime;

        unsigned int residualSize = (calculator0.getResidualSize() == calculator0.getResidualSize())
                                            ? calculator0.getResidualSize()
                                            : maxResidualSize;
        bool isAll = false;

        std::vector<Tucuxi::Core::Concentrations> concentrations0;
        concentrations0.resize(residualSize);

        Tucuxi::Core::TimeOffsets times;
        Tucuxi::Core::IntakeEvent intakeEvent(
                now,
                offsetTime,
                _dose,
                TucuUnit("mg"),
                interval,
                Tucuxi::Core::FormulationAndRoute(_route),
                _route,
                infusionTime,
                _nbPoints);
        Tucuxi::Core::Residuals inResiduals(residualSize);
        Tucuxi::Core::Residuals outResiduals0(residualSize);
        Tucuxi::Core::Residuals outResiduals1(residualSize);

        std::fill(inResiduals.begin(), inResiduals.end(), 0);

        // Calculation of first Class
        res = calculator0.calculateIntakePoints(
                concentrations0, times, intakeEvent, _Parameters, inResiduals, isAll, outResiduals0, true);

        fructose_assert(res == Tucuxi::Core::ComputingStatus::Ok);


        std::vector<Tucuxi::Core::Concentrations> concentrations1;
        concentrations1.resize(residualSize);

        // Calculation of second Class
        res = calculator1.calculateIntakePoints(
                concentrations1, times, intakeEvent, _Parameters, inResiduals, isAll, outResiduals1, true);

        fructose_assert(res == Tucuxi::Core::ComputingStatus::Ok);

        if (res == Tucuxi::Core::ComputingStatus::Ok) {
            for (unsigned int i = 0; i < static_cast<unsigned int>(_nbPoints); i++) {
                //                std::cout << i << " : " << concentrations0[0][i] << " , " << concentrations1[0][i] << std::endl;
                fructose_assert_double_eq_rel_abs(concentrations0[0][i], concentrations1[0][i], 0.0001, 0.0);
            }

            for (unsigned int i = 0; i < residualSize; i++) {
                fructose_assert_double_eq_rel_abs(outResiduals0[i], outResiduals1[i], 0.0001, 0.0);
            }
        }
    }


    template<class CalculatorMicroClass, class CalculatorMacroClass>
    void testCalculator(
            const Tucuxi::Core::ParameterSetEvent& _microParameters,
            const Tucuxi::Core::ParameterSetEvent& _macroParameters,
            double _dose,
            Tucuxi::Core::AbsorptionModel _route,
            std::chrono::hours _interval,
            std::chrono::seconds _infusionTime,
            size_t _nbPoints)
    {
        testSingleVsMultiple<CalculatorMicroClass, CalculatorMacroClass>(
                _microParameters, _macroParameters, _dose, _route, _interval, _infusionTime, _nbPoints);
        testSteadyState<CalculatorMicroClass, CalculatorMacroClass>(
                _microParameters, _macroParameters, _dose, _route, _interval, _infusionTime, _nbPoints);
    }


    /// \brief Test the residual calculation of Bolus. Compares single point vs multiple points
    /// \param _testName Test name.
    /// A calculator is instanciated, and residuals are computed with both
    ///     calculateIntakePoints and calculateIntakeSinglePoint.
    /// The residuals are then compared and shall be identical.
    void test1compBolusSingleVsMultiple(const std::string& /* _testName */)
    {
        if (verbose()) {
            std::cout << __FUNCTION__ << std::endl;
        }

        // parameter for micro class
        Tucuxi::Core::ParameterDefinitions microParameterDefs;
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "V", 347, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "Ke", 0.0435331, Tucuxi::Core::ParameterVariabilityType::None));
        Tucuxi::Core::ParameterSetEvent microParameters(DateTime::now(), microParameterDefs);

        // parameter for macro class
        Tucuxi::Core::ParameterDefinitions macroParameterDefs;
        macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "V", 347, Tucuxi::Core::ParameterVariabilityType::None));
        macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "CL", 15.106, Tucuxi::Core::ParameterVariabilityType::None));
        Tucuxi::Core::ParameterSetEvent macroParameters(DateTime::now(), macroParameterDefs);
        testCalculator<Tucuxi::Core::OneCompartmentBolusMicro, Tucuxi::Core::OneCompartmentBolusMacro>(
                microParameters,
                macroParameters,
                400.0,
                Tucuxi::Core::AbsorptionModel::Intravascular,
                1h,
                0s,
                CYCLE_SIZE);
    }

    /// \brief Test the residual calculation of extravascular. Compares single point vs multiple points
    /// \param _testName Test name.
    /// A calculator is instanciated, and residuals are computed with both
    ///     calculateIntakePoints and calculateIntakeSinglePoint.
    /// The residuals are then compared and shall be identical.
    void test1compExtraSingleVsMultiple(const std::string& /* _testName */)
    {
        // parameter for micro class
        Tucuxi::Core::ParameterDefinitions microParameterDefs;
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "V", 347, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "Ke", 0.0435331, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "Ka", 0.609, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "F", 1, Tucuxi::Core::ParameterVariabilityType::None));
        Tucuxi::Core::ParameterSetEvent microParameters(DateTime::now(), microParameterDefs);

        // parameter for macro class
        Tucuxi::Core::ParameterDefinitions macroParameterDefs;
        macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "V", 347, Tucuxi::Core::ParameterVariabilityType::None));
        macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "CL", 15.106, Tucuxi::Core::ParameterVariabilityType::None));
        macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "Ka", 0.609, Tucuxi::Core::ParameterVariabilityType::None));
        macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "F", 1, Tucuxi::Core::ParameterVariabilityType::None));
        Tucuxi::Core::ParameterSetEvent macroParameters(DateTime::now(), macroParameterDefs);

        testCalculator<Tucuxi::Core::OneCompartmentExtraMicro, Tucuxi::Core::OneCompartmentExtraMacro>(
                microParameters,
                macroParameters,
                400.0,
                Tucuxi::Core::AbsorptionModel::Extravascular,
                12h,
                0s,
                CYCLE_SIZE);
    }

    /// \brief Test the concentration calculation of extravascular. Compares analytical and Rk4 intake calculators
    /// \param _testName Test name.
    /// Two calculators are instanciated, and concentrations are computed with both.
    /// The concentrations are then compared and shall be identical.
    void test1compExtraAnalyticalVsRk4(const std::string& /* _testName */)
    {
        // parameter for micro class
        Tucuxi::Core::ParameterDefinitions microParameterDefs;
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "V", 347, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "Ke", 0.0435331, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "Ka", 0.609, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "F", 1, Tucuxi::Core::ParameterVariabilityType::None));
        Tucuxi::Core::ParameterSetEvent microParameters(DateTime::now(), microParameterDefs);

        testCompare<Tucuxi::Core::OneCompartmentExtraMicro, Tucuxi::Core::RK4OneCompartmentExtraMicro>(
                microParameters, 400.0, Tucuxi::Core::AbsorptionModel::Extravascular, 12h, 0s, CYCLE_SIZE);
    }

    void test1compExtraLagSingleVsMultiple(const std::string& /* _testName */)
    {
        // parameter for micro class
        Tucuxi::Core::ParameterDefinitions microParameterDefs;
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "V", 347, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "Ke", 0.0435331, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "Ka", 0.609, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "F", 1, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "Tlag", 1, Tucuxi::Core::ParameterVariabilityType::None));
        Tucuxi::Core::ParameterSetEvent microParameters(DateTime::now(), microParameterDefs);

        // parameter for macro class
        Tucuxi::Core::ParameterDefinitions macroParameterDefs;
        macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "V", 347, Tucuxi::Core::ParameterVariabilityType::None));
        macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "CL", 15.106, Tucuxi::Core::ParameterVariabilityType::None));
        macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "Ka", 0.609, Tucuxi::Core::ParameterVariabilityType::None));
        macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "F", 1, Tucuxi::Core::ParameterVariabilityType::None));
        macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "Tlag", 1, Tucuxi::Core::ParameterVariabilityType::None));
        Tucuxi::Core::ParameterSetEvent macroParameters(DateTime::now(), macroParameterDefs);

        testCalculator<Tucuxi::Core::OneCompartmentExtraLagMicro, Tucuxi::Core::OneCompartmentExtraLagMacro>(
                microParameters,
                macroParameters,
                400.0,
                Tucuxi::Core::AbsorptionModel::ExtravascularLag,
                12h,
                0s,
                CYCLE_SIZE);
    }

    /// \brief Test the concentration calculation of erlang transit compartments.
    /// \param _testName Test name.
    void test2compErlang(const std::string& /* _testName */)
    {
        // parameter for micro class
        Tucuxi::Core::ParameterDefinitions microParameterDefs;
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "V1", 347, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "V2", 347, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "Q", 347, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "CL", 0.0435331, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "Ktr", 0.609, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "F", 1, Tucuxi::Core::ParameterVariabilityType::None));
        Tucuxi::Core::ParameterSetEvent microParameters(DateTime::now(), microParameterDefs);

        Tucuxi::Core::ComputingStatus res;
        RK4TwoCompartmentErlangMacro<4> calculator0;

        DateTime now = DateTime::now();
        Tucuxi::Common::Duration offsetTime = 0s;
        Tucuxi::Common::Duration interval = 12h;
        Tucuxi::Common::Duration infusionTime = 0h;

        unsigned int residualSize = calculator0.getResidualSize();
        bool isAll = false;

        std::vector<Tucuxi::Core::Concentrations> concentrations0;
        concentrations0.resize(residualSize);

        Tucuxi::Core::TimeOffsets times;
        Tucuxi::Core::IntakeEvent intakeEvent(
                now,
                offsetTime,
                400,
                TucuUnit("mg"),
                interval,
                Tucuxi::Core::FormulationAndRoute(Tucuxi::Core::AbsorptionModel::Extravascular),
                Tucuxi::Core::AbsorptionModel::Extravascular,
                infusionTime,
                CYCLE_SIZE);
        Tucuxi::Core::Residuals inResiduals(residualSize);
        Tucuxi::Core::Residuals outResiduals0(residualSize);
        Tucuxi::Core::Residuals outResiduals1(residualSize);

        std::fill(inResiduals.begin(), inResiduals.end(), 0);

        // Calculation of first Class
        res = calculator0.calculateIntakePoints(
                concentrations0, times, intakeEvent, microParameters, inResiduals, isAll, outResiduals0, true);

        fructose_assert(res == Tucuxi::Core::ComputingStatus::Ok);
    }


    /// \brief Test the residual calculation of extravascular. Compares single point vs multiple points
    /// \param _testName Test name.
    /// A calculator is instanciated, and residuals are computed with both
    ///     calculateIntakePoints and calculateIntakeSinglePoint.
    /// The residuals are then compared and shall be identical.
    void testRK41compExtraSingleVsMultiple(const std::string& /* _testName */)
    {
        // parameter for micro class
        Tucuxi::Core::ParameterDefinitions microParameterDefs;
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "V", 347, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "Ke", 0.0435331, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "Ka", 0.609, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "F", 1, Tucuxi::Core::ParameterVariabilityType::None));
        Tucuxi::Core::ParameterSetEvent microParameters(DateTime::now(), microParameterDefs);

        // parameter for macro class
        Tucuxi::Core::ParameterDefinitions macroParameterDefs;
        macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "V", 347, Tucuxi::Core::ParameterVariabilityType::None));
        macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "CL", 15.106, Tucuxi::Core::ParameterVariabilityType::None));
        macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "Ka", 0.609, Tucuxi::Core::ParameterVariabilityType::None));
        macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "F", 1, Tucuxi::Core::ParameterVariabilityType::None));
        Tucuxi::Core::ParameterSetEvent macroParameters(DateTime::now(), macroParameterDefs);

        testCalculator<Tucuxi::Core::RK4OneCompartmentExtraMicro, Tucuxi::Core::RK4OneCompartmentExtraMacro>(
                microParameters,
                macroParameters,
                400.0,
                Tucuxi::Core::AbsorptionModel::Extravascular,
                12h,
                0s,
                CYCLE_SIZE);
    }

    /// \brief Test the residual calculation of extravascular. Compares single point vs multiple points
    /// \param _testName Test name.
    /// A calculator is instanciated, and residuals are computed with both
    ///     calculateIntakePoints and calculateIntakeSinglePoint.
    /// The residuals are then compared and shall be identical.
    void testRK41compGammaExtraSingleVsMultiple(const std::string& /* _testName */)
    {
        // parameter for micro class
        Tucuxi::Core::ParameterDefinitions microParameterDefs;
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "V", 347, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "Ke", 0.0435331, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "F", 1, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "a", 6, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "b", 10, Tucuxi::Core::ParameterVariabilityType::None));
        Tucuxi::Core::ParameterSetEvent microParameters(DateTime::now(), microParameterDefs);

        // parameter for macro class
        Tucuxi::Core::ParameterDefinitions macroParameterDefs;
        macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "V", 347, Tucuxi::Core::ParameterVariabilityType::None));
        macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "CL", 15.106, Tucuxi::Core::ParameterVariabilityType::None));
        macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "F", 1, Tucuxi::Core::ParameterVariabilityType::None));
        macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "a", 6, Tucuxi::Core::ParameterVariabilityType::None));
        macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "b", 10, Tucuxi::Core::ParameterVariabilityType::None));
        Tucuxi::Core::ParameterSetEvent macroParameters(DateTime::now(), macroParameterDefs);

        testCalculator<Tucuxi::Core::RK4OneCompartmentGammaExtraMicro, Tucuxi::Core::RK4OneCompartmentGammaExtraMacro>(
                microParameters,
                macroParameters,
                400.0,
                Tucuxi::Core::AbsorptionModel::Extravascular,
                12h,
                0s,
                CYCLE_SIZE);
    }

    /// \brief Test the residual calculation of infusion. Compares single point vs multiple points
    /// \param _testName Test name.
    /// A calculator is instanciated, and residuals are computed with both
    ///     calculateIntakePoints and calculateIntakeSinglePoint.
    /// The residuals are then compared and shall be identical.
    void test1compInfusionSingleVsMultiple(const std::string& /* _testName */)
    {
        // parameter for micro class
        Tucuxi::Core::ParameterDefinitions microParameterDefs;
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "Ke", 0.0435331, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "V", 347, Tucuxi::Core::ParameterVariabilityType::None));
        Tucuxi::Core::ParameterSetEvent microParameters(DateTime::now(), microParameterDefs);

        // parameter for macro class
        Tucuxi::Core::ParameterDefinitions macroParameterDefs;
        macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "CL", 15.106, Tucuxi::Core::ParameterVariabilityType::None));
        macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "V", 347, Tucuxi::Core::ParameterVariabilityType::None));
        Tucuxi::Core::ParameterSetEvent macroParameters(DateTime::now(), macroParameterDefs);

        testCalculator<Tucuxi::Core::OneCompartmentInfusionMicro, Tucuxi::Core::OneCompartmentInfusionMacro>(
                microParameters, macroParameters, 400.0, Tucuxi::Core::AbsorptionModel::Infusion, 12h, 1h, CYCLE_SIZE);
    }


    /// \brief Test the residual calculation of Bolus. Compares single point vs multiple points
    /// \param _testName Test name.
    /// A calculator is instanciated, and residuals are computed with both
    ///     calculateIntakePoints and calculateIntakeSinglePoint.
    /// The residuals are then compared and shall be identical.
    void test2compBolusSingleVsMultiple(const std::string& /* _testName */)
    {
        // parameter for micro class
        Tucuxi::Core::ParameterDefinitions microParameterDefs;
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "V1", 340, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "Ke", 0.0444294, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "K12", 0.0588235, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "K21", 0.0584795, Tucuxi::Core::ParameterVariabilityType::None));
        Tucuxi::Core::ParameterSetEvent microParameters(DateTime::now(), microParameterDefs);

        // parameter for macro class
        Tucuxi::Core::ParameterDefinitions macroParameterDefs;
        macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "CL", 15.106, Tucuxi::Core::ParameterVariabilityType::None));
        macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "Q", 20, Tucuxi::Core::ParameterVariabilityType::None));
        macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "V1", 340, Tucuxi::Core::ParameterVariabilityType::None));
        macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "V2", 342, Tucuxi::Core::ParameterVariabilityType::None));
        Tucuxi::Core::ParameterSetEvent macroParameters(DateTime::now(), macroParameterDefs);

        testCalculator<Tucuxi::Core::TwoCompartmentBolusMicro, Tucuxi::Core::TwoCompartmentBolusMacro>(
                microParameters,
                macroParameters,
                400.0,
                Tucuxi::Core::AbsorptionModel::Intravascular,
                12h,
                0s,
                CYCLE_SIZE);
    }

    /// \brief Test the residual calculation of extravascular. Compares single point vs multiple points
    /// \param _testName Test name.
    /// A calculator is instanciated, and residuals are computed with both
    ///     calculateIntakePoints and calculateIntakeSinglePoint.
    /// The residuals are then compared and shall be identical.
    void test2compExtraSingleVsMultiple(const std::string& /* _testName */)
    {
        // parameter for micro class
        Tucuxi::Core::ParameterDefinitions microParameterDefs;
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "V1", 340, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "Ke", 0.0444294, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "K12", 0.0588235, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "K21", 0.0584795, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "Ka", 0.609, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "F", 1, Tucuxi::Core::ParameterVariabilityType::None));
        Tucuxi::Core::ParameterSetEvent microParameters(DateTime::now(), microParameterDefs);

        // parameter for macro class
        Tucuxi::Core::ParameterDefinitions macroParameterDefs;
        macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "CL", 15.106, Tucuxi::Core::ParameterVariabilityType::None));
        macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "Q", 20, Tucuxi::Core::ParameterVariabilityType::None));
        macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "V2", 342, Tucuxi::Core::ParameterVariabilityType::None));
        macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "V1", 340, Tucuxi::Core::ParameterVariabilityType::None));
        macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "Ka", 0.609, Tucuxi::Core::ParameterVariabilityType::None));
        macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "F", 1, Tucuxi::Core::ParameterVariabilityType::None));
        Tucuxi::Core::ParameterSetEvent macroParameters(DateTime::now(), macroParameterDefs);

        testCalculator<Tucuxi::Core::TwoCompartmentExtraMicro, Tucuxi::Core::TwoCompartmentExtraMacro>(
                microParameters,
                macroParameters,
                400.0,
                Tucuxi::Core::AbsorptionModel::Extravascular,
                12h,
                0s,
                CYCLE_SIZE);
    }

    /// \brief Test the residual calculation of infusion. Compares single point vs multiple points
    /// \param _testName Test name.
    /// A calculator is instanciated, and residuals are computed with both
    ///     calculateIntakePoints and calculateIntakeSinglePoint.
    /// The residuals are then compared and shall be identical.
    void test2compInfusionSingleVsMultiple(const std::string& /* _testName */)
    {
        // parameter for micro class
        Tucuxi::Core::ParameterDefinitions microParameterDefs;
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "V1", 340, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "Ke", 0.0444294, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "K12", 0.0588235, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "K21", 0.0584795, Tucuxi::Core::ParameterVariabilityType::None));
        Tucuxi::Core::ParameterSetEvent microParameters(DateTime::now(), microParameterDefs);

        // parameter for macro class
        Tucuxi::Core::ParameterDefinitions macroParameterDefs;
        macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "CL", 15.106, Tucuxi::Core::ParameterVariabilityType::None));
        macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "Q", 20, Tucuxi::Core::ParameterVariabilityType::None));
        macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "V1", 340, Tucuxi::Core::ParameterVariabilityType::None));
        macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "V2", 342, Tucuxi::Core::ParameterVariabilityType::None));
        Tucuxi::Core::ParameterSetEvent macroParameters(DateTime::now(), macroParameterDefs);

        testCalculator<Tucuxi::Core::TwoCompartmentInfusionMicro, Tucuxi::Core::TwoCompartmentInfusionMacro>(
                microParameters, macroParameters, 400.0, Tucuxi::Core::AbsorptionModel::Infusion, 12h, 1h, CYCLE_SIZE);
    }

    void test2compExtraLagSingleVsMultiple(const std::string& /* _testName */)
    {
        // parameter for micro class
        Tucuxi::Core::ParameterDefinitions microParameterDefs;
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "V1", 58.0, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "K12", 10.0, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "K21", 6.0, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "Ke", 0.075, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "Ka", 0.609, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "F", 1, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "Tlag", 1, Tucuxi::Core::ParameterVariabilityType::None));
        Tucuxi::Core::ParameterSetEvent microParameters(DateTime::now(), microParameterDefs);

        // parameter for macro class
        Tucuxi::Core::ParameterDefinitions macroParameterDefs;
        macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "V1", 58.0, Tucuxi::Core::ParameterVariabilityType::None));
        macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "V2", 96.6666666, Tucuxi::Core::ParameterVariabilityType::None));
        macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "Q", 580.0, Tucuxi::Core::ParameterVariabilityType::None));
        macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "CL", 4.35, Tucuxi::Core::ParameterVariabilityType::None));
        macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "Ka", 0.609, Tucuxi::Core::ParameterVariabilityType::None));
        macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "F", 1, Tucuxi::Core::ParameterVariabilityType::None));
        macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "Tlag", 1, Tucuxi::Core::ParameterVariabilityType::None));
        Tucuxi::Core::ParameterSetEvent macroParameters(DateTime::now(), macroParameterDefs);

        testCalculator<Tucuxi::Core::TwoCompartmentExtraLagMicro, Tucuxi::Core::TwoCompartmentExtraLagMacro>(
                microParameters,
                macroParameters,
                400.0,
                Tucuxi::Core::AbsorptionModel::ExtravascularLag,
                12h,
                0s,
                CYCLE_SIZE);
    }

    /// \brief Test the concentration calculation of extravascular lag for 2 compartments. Compares analytical and Rk4 intake calculators
    /// \param _testName Test name.
    /// Two calculators are instanciated, and concentrations are computed with both.
    /// The concentrations are then compared and shall be identical.
    void test2compExtraLagAnalyticalVsRk4(const std::string& /* _testName */)
    {
        // parameter for micro class
        Tucuxi::Core::ParameterDefinitions microParameterDefs;
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "V1", 58.0, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "K12", 10.0, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "K21", 6.0, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "Ke", 0.075, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "Ka", 0.609, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "F", 1, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "Tlag", 4.0, Tucuxi::Core::ParameterVariabilityType::None));
        Tucuxi::Core::ParameterSetEvent microParameters(DateTime::now(), microParameterDefs);

        testCompare<Tucuxi::Core::TwoCompartmentExtraLagMicro, Tucuxi::Core::RK4TwoCompartmentExtraLagMicro>(
                microParameters,
                400.0,
                Tucuxi::Core::AbsorptionModel::ExtravascularLag,
                12h,
                0s,
                241); // 241 allows to be aligned on the hours, else the test fails because of RK4 lag time
                      //            CYCLE_SIZE);
    }

    /// \brief Test the residual calculation of Bolus. Compares single point vs multiple points
    /// \param _testName Test name.
    /// A calculator is instanciated, and residuals are computed with both
    ///     calculateIntakePoints and calculateIntakeSinglePoint.
    /// The residuals are then compared and shall be identical.
    void test3compBolusSingleVsMultiple(const std::string& /* _testName */)
    {
        // parameter for micro class
        Tucuxi::Core::ParameterDefinitions microParameterDefs;
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "F", 2, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "V1", 340, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "Ke", 0.0444294, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "K12", 0.0588235, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "K21", 0.0584795, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "K13", 0.0882353, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "K31", 0.0877193, Tucuxi::Core::ParameterVariabilityType::None));
        Tucuxi::Core::ParameterSetEvent microParameters(DateTime::now(), microParameterDefs);

        // parameter for macro class
        Tucuxi::Core::ParameterDefinitions macroParameterDefs;
        macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "CL", 15.106, Tucuxi::Core::ParameterVariabilityType::None));
        macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "F", 2, Tucuxi::Core::ParameterVariabilityType::None));
        macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "Q1", 20, Tucuxi::Core::ParameterVariabilityType::None));
        macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "Q2", 30, Tucuxi::Core::ParameterVariabilityType::None));
        macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "V1", 340, Tucuxi::Core::ParameterVariabilityType::None));
        macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "V2", 342, Tucuxi::Core::ParameterVariabilityType::None));
        Tucuxi::Core::ParameterSetEvent macroParameters(DateTime::now(), macroParameterDefs);

        testCalculator<Tucuxi::Core::ThreeCompartmentBolusMicro, Tucuxi::Core::ThreeCompartmentBolusMacro>(
                microParameters,
                macroParameters,
                400.0,
                Tucuxi::Core::AbsorptionModel::Intravascular,
                12h,
                0s,
                CYCLE_SIZE);
    }

    /// \brief Test the residual calculation of Extra. Compares single point vs multiple points
    /// \param _testName Test name.
    /// A calculator is instanciated, and residuals are computed with both
    ///     calculateIntakePoints and calculateIntakeSinglePoint.
    /// The residuals are then compared and shall be identical.
    void test3compExtraSingleVsMultiple(const std::string& /* _testName */)
    {
        // parameter for micro class
        Tucuxi::Core::ParameterDefinitions microParameterDefs;
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "F", 2, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "V1", 340, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "Ka", 0.609, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "Ke", 0.0444294, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "K12", 0.0588235, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "K21", 0.0584795, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "K13", 0.0882353, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "K31", 0.0877193, Tucuxi::Core::ParameterVariabilityType::None));
        Tucuxi::Core::ParameterSetEvent microParameters(DateTime::now(), microParameterDefs);

        // parameter for macro class
        Tucuxi::Core::ParameterDefinitions macroParameterDefs;
        macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "CL", 15.106, Tucuxi::Core::ParameterVariabilityType::None));
        macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "F", 2, Tucuxi::Core::ParameterVariabilityType::None));
        macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "Q1", 20, Tucuxi::Core::ParameterVariabilityType::None));
        macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "Q2", 30, Tucuxi::Core::ParameterVariabilityType::None));
        macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "V1", 340, Tucuxi::Core::ParameterVariabilityType::None));
        macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "V2", 342, Tucuxi::Core::ParameterVariabilityType::None));
        macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "Ka", 0.609, Tucuxi::Core::ParameterVariabilityType::None));
        Tucuxi::Core::ParameterSetEvent macroParameters(DateTime::now(), macroParameterDefs);

        testCalculator<Tucuxi::Core::ThreeCompartmentExtraMicro, Tucuxi::Core::ThreeCompartmentExtraMacro>(
                microParameters,
                macroParameters,
                400.0,
                Tucuxi::Core::AbsorptionModel::Extravascular,
                12h,
                0s,
                CYCLE_SIZE);
    }


    /// \brief Test the residual calculation of Infusion. Compares single point vs multiple points
    /// \param _testName Test name.
    /// A calculator is instanciated, and residuals are computed with both
    ///     calculateIntakePoints and calculateIntakeSinglePoint.
    /// The residuals are then compared and shall be identical.
    void test3compInfusionSingleVsMultiple(const std::string& /* _testName */)
    {
        // parameter for micro class
        Tucuxi::Core::ParameterDefinitions microParameterDefs;
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "F", 2, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "V1", 340, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "Ke", 0.0444294, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "K12", 0.0588235, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "K21", 0.0584795, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "K13", 0.0882353, Tucuxi::Core::ParameterVariabilityType::None));
        microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "K31", 0.0877193, Tucuxi::Core::ParameterVariabilityType::None));
        Tucuxi::Core::ParameterSetEvent microParameters(DateTime::now(), microParameterDefs);

        // parameter for macro class
        Tucuxi::Core::ParameterDefinitions macroParameterDefs;
        macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "CL", 15.106, Tucuxi::Core::ParameterVariabilityType::None));
        macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "F", 2, Tucuxi::Core::ParameterVariabilityType::None));
        macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "Q1", 20, Tucuxi::Core::ParameterVariabilityType::None));
        macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "Q2", 30, Tucuxi::Core::ParameterVariabilityType::None));
        macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "V1", 340, Tucuxi::Core::ParameterVariabilityType::None));
        macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "V2", 342, Tucuxi::Core::ParameterVariabilityType::None));
        Tucuxi::Core::ParameterSetEvent macroParameters(DateTime::now(), macroParameterDefs);

        testCalculator<Tucuxi::Core::ThreeCompartmentInfusionMicro, Tucuxi::Core::ThreeCompartmentInfusionMacro>(
                microParameters, macroParameters, 400.0, Tucuxi::Core::AbsorptionModel::Infusion, 12h, 1h, CYCLE_SIZE);
    }
};

#endif // TEST_INTAKEINTERVALCALCULATOR_H
