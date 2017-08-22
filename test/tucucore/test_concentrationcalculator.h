/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TEST_CONCENTRATIONCALCULATOR_H
#define TEST_CONCENTRATIONCALCULATOR_H

#include <iostream>
#include <memory>

#include "fructose/fructose.h"

#include "tucucommon/general.h"

#include "tucucore/dosage.h"
#include "tucucore/intakeextractor.h"
#include "tucucore/intakeintervalcalculator.h"
#include "tucucore/onecompartmentbolus.h"

struct TestConcentrationCalculator : public fructose::test_base<TestConcentrationCalculator>
{

    TestConcentrationCalculator() { }

    template<class CalculatorClass, int nbComp>
    void testCalculator(const Tucuxi::Core::ParameterSetEvent &_parameters,
                        double _dose,
                        Tucuxi::Core::RouteOfAdministration _route,
                        std::chrono::hours _interval,
                        std::chrono::seconds _infusionTime,
                        int _nbPoints)
    {
        TMP_UNUSED_PARAMETER(_parameters);
        TMP_UNUSED_PARAMETER(_dose);
        TMP_UNUSED_PARAMETER(_route);
        TMP_UNUSED_PARAMETER(_interval);
        TMP_UNUSED_PARAMETER(_infusionTime);
        TMP_UNUSED_PARAMETER(_nbPoints);
        // Compare the result on one interval
        // with ConcentrationCalculator vs directly with the IntakeIntervalCalculator



        // Use the ConcentrationCalculator in order to calculate multiple cycles
        // and compare it with the sum of individual calculations made with the
        // IntakeIntervalCalculators. Be careful, the interval for the later need
        // to be longer, and the number of points modified accordingly




        // Create samples and compare the result of computeConcentrations() and pointsAtTime().
        // This can be done by creating samples corresponding to the number of points asked, and
        // synchronized with the times at which the concentration points are expected

    }

    void test1compBolus(const std::string& /* _testName */)
    {
        {
            Tucuxi::Core::ParameterDefinitions parameterDefs;
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("V", 347, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            parameterDefs.push_back(Tucuxi::Core::ParameterDefinition("Ke", 22.97, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            Tucuxi::Core::ParameterSetEvent parameters(DateTime(), parameterDefs);

            testCalculator<Tucuxi::Core::OneCompartmentBolusMicro, 1>
                    (parameters,
                     400.0,
                     Tucuxi::Core::RouteOfAdministration::INTRAVASCULAR,
                     12h,
                     0s,
                     250);
        }

    }


    void test1compExtra(const std::string& /* _testName */)
    {

    }


    void test1compInfusion(const std::string& /* _testName */)
    {

    }

};

#endif // TEST_CONCENTRATIONCALCULATOR_H
