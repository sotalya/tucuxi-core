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


    template <class CalculatorClass, int nbComp>
    void testSingleVsMultipleResiduals(const Tucuxi::Core::ParameterSetEvent &_parameters,
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

        for(int i = 0; i < nbComp; i++)
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
            for(int i = 0; i < nbComp; i++) {
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
            for(int i = 0; i < nbComp; i++) {
                std::cout << "Single   Out residual["
                          << i
                          << "] = "
                          << outSingleResiduals[i]
                          << std::endl;
            }
        }

        fructose_assert(res == Tucuxi::Core::IntakeIntervalCalculator::Result::Ok);

        if (res == Tucuxi::Core::IntakeIntervalCalculator::Result::Ok) {
            for (int i = 0; i < nbComp; i++) {
                fructose_assert (abs(outMultiResiduals[i]/outSingleResiduals[i]-1) < DELTA);
            }
        }
    }

    /// \brief Test the residual calculation of Bolus. Compares single point vs multiple points
    /// \param _testName Test name.
    /// A calculator is instanciated, and residuals are computed with both
    ///     calculateIntakePoints and calculateIntakeSinglePoint.
    /// The residuals are then compared and shall be identical.
    void test1compBolusSingleVsMultiple(const std::string& /* _testName */)
    {
        {
            Tucuxi::Core::ParameterDefinitions parameterDefs;
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("V", 347, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("Ke", 22.97, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            Tucuxi::Core::ParameterSetEvent parameters(DateTime(), parameterDefs);

            testSingleVsMultipleResiduals<Tucuxi::Core::OneCompartmentBolusMicro, 1>
                    (parameters,
                     400.0,
                     Tucuxi::Core::RouteOfAdministration::INTRAVASCULAR,
                     12h,
                     0s,
                     250);
        }

        {
            Tucuxi::Core::ParameterDefinitions parameterDefs;
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("V", 347, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("Cl", 15.106, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            Tucuxi::Core::ParameterSetEvent parameters(DateTime(), parameterDefs);

            testSingleVsMultipleResiduals<Tucuxi::Core::OneCompartmentBolusMacro, 1>
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
    void test1compExtraSingleVsMultiple(const std::string& /* _testName */)
    {
        {
            Tucuxi::Core::ParameterDefinitions parameterDefs;
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("V", 347, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("Ke", 22.97, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("Ka", 0.609, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("F", 1, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            Tucuxi::Core::ParameterSetEvent parameters(DateTime(), parameterDefs);

            testSingleVsMultipleResiduals<Tucuxi::Core::OneCompartmentExtraMicro, 2>
                    (parameters,
                     400.0,
                     Tucuxi::Core::RouteOfAdministration::INTRAVASCULAR,
                     12h,
                     0s,
                     250);
        }

        {
            Tucuxi::Core::ParameterDefinitions parameterDefs;
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("V", 347, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("Cl", 15.106, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("Ka", 0.609, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("F", 1, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            Tucuxi::Core::ParameterSetEvent parameters(DateTime(), parameterDefs);

            testSingleVsMultipleResiduals<Tucuxi::Core::OneCompartmentExtraMacro, 2>
                    (parameters,
                     400.0,
                     Tucuxi::Core::RouteOfAdministration::INTRAVASCULAR,
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

        {
            Tucuxi::Core::ParameterDefinitions parameterDefs;
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("V", 347, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("Ke", 22.97, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            Tucuxi::Core::ParameterSetEvent parameters(DateTime(), parameterDefs);

            testSingleVsMultipleResiduals<Tucuxi::Core::OneCompartmentInfusionMicro, 1>
                    (parameters,
                     400.0,
                     Tucuxi::Core::RouteOfAdministration::INFUSION,
                     12h,
                     1h,
                     250);
        }

        {
            Tucuxi::Core::ParameterDefinitions parameterDefs;
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("V", 347, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("Cl", 15.106, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            Tucuxi::Core::ParameterSetEvent parameters(DateTime(), parameterDefs);

            testSingleVsMultipleResiduals<Tucuxi::Core::OneCompartmentInfusionMacro, 1>
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
        {
            Tucuxi::Core::ParameterDefinitions parameterDefs;
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("Cl", 15.106, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("Q", 20, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("V1", 340, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("V2", 342, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            Tucuxi::Core::ParameterSetEvent parameters(DateTime(), parameterDefs);

            testSingleVsMultipleResiduals<Tucuxi::Core::TwoCompartmentBolusMacro, 2>
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

        {
            Tucuxi::Core::ParameterDefinitions parameterDefs;
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("Cl", 15.106, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("Q", 20, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("V1", 347, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("V2", 200, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("Ka", 0.609, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("F", 1, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            Tucuxi::Core::ParameterSetEvent parameters(DateTime(), parameterDefs);

            testSingleVsMultipleResiduals<Tucuxi::Core::TwoCompartmentExtraMacro, 3>
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

        {
            Tucuxi::Core::ParameterDefinitions parameterDefs;
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("Cl", 15.106, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("Q", 20, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("V1", 340, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("V2", 342, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            Tucuxi::Core::ParameterSetEvent parameters(DateTime(), parameterDefs);

            testSingleVsMultipleResiduals<Tucuxi::Core::TwoCompartmentInfusion, 2>
                    (parameters,
                     400.0,
                     Tucuxi::Core::RouteOfAdministration::INFUSION,
                     12h,
                     1h,
                     250);
        }
    }

};

#endif // TEST_INTAKEINTERVALCALCULATOR_H
