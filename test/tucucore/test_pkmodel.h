/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TEST_PKMODEL_H
#define TEST_PKMODEL_H

#include <algorithm>
#include <iostream>
#include <memory>
#include <typeinfo>

#include "fructose/fructose.h"

#include "tucucore/pkmodel.h"

using namespace Tucuxi::Core;

struct TestPkModel : public fructose::test_base<TestPkModel>
{
    TestPkModel() { }

    /// \brief Test basic functionalities of a PkModel.
    void testPkModelFunctions(const std::string& /* _testName */)
    {
        std::shared_ptr<PkModel> l1CMicro = std::make_shared<PkModel>("linear.1comp.micro");
        std::shared_ptr<PkModel> l1CMacro = std::make_shared<PkModel>("linear.1comp.macro");

        // Check correct model ID
        fructose_assert (l1CMicro->getPkModelId() == "linear.1comp.micro");
        fructose_assert (l1CMacro->getPkModelId() == "linear.1comp.macro");

        bool rc;
        rc = l1CMicro->addIntakeIntervalCalculatorFactory(RouteOfAdministration::EXTRAVASCULAR,
                                                          Tucuxi::Core::OneCompartmentExtraMicro::getCreator());
        fructose_assert (rc == true);
        rc = l1CMacro->addIntakeIntervalCalculatorFactory(RouteOfAdministration::EXTRAVASCULAR,
                                                          Tucuxi::Core::OneCompartmentExtraMacro::getCreator());
        fructose_assert (rc == true);

        rc = l1CMicro->addIntakeIntervalCalculatorFactory(RouteOfAdministration::INTRAVASCULAR,
                                                          Tucuxi::Core::OneCompartmentBolusMicro::getCreator());
        fructose_assert (rc == true);
        // Avoid duplicate insertion
        rc = l1CMicro->addIntakeIntervalCalculatorFactory(RouteOfAdministration::INTRAVASCULAR,
                                                          Tucuxi::Core::OneCompartmentBolusMicro::getCreator());
        fructose_assert (rc == false);
        rc = l1CMacro->addIntakeIntervalCalculatorFactory(RouteOfAdministration::INTRAVASCULAR,
                                                          Tucuxi::Core::OneCompartmentBolusMacro::getCreator());
        fructose_assert (rc == true);

        rc = l1CMicro->addIntakeIntervalCalculatorFactory(RouteOfAdministration::INFUSION,
                                                          Tucuxi::Core::OneCompartmentInfusionMicro::getCreator());
        fructose_assert (rc == true);
        rc = l1CMacro->addIntakeIntervalCalculatorFactory(RouteOfAdministration::INFUSION,
                                                          Tucuxi::Core::OneCompartmentInfusionMacro::getCreator());
        fructose_assert (rc == true);
        // Avoid duplicate insertion
        rc = l1CMacro->addIntakeIntervalCalculatorFactory(RouteOfAdministration::INFUSION,
                                                          Tucuxi::Core::OneCompartmentExtraMicro::getCreator());
        fructose_assert (rc == false);

        // Check that there are no missing route of administration
        std::vector<RouteOfAdministration> l1MicroROA = l1CMicro->getAvailableRoutes();
        std::vector<RouteOfAdministration> l1MacroROA = l1CMacro->getAvailableRoutes();
        fructose_assert (std::find(l1MicroROA.begin(), l1MicroROA.end(), RouteOfAdministration::EXTRAVASCULAR) != l1MicroROA.end());
        fructose_assert (std::find(l1MacroROA.begin(), l1MacroROA.end(), RouteOfAdministration::EXTRAVASCULAR) != l1MacroROA.end());
        fructose_assert (std::find(l1MicroROA.begin(), l1MicroROA.end(), RouteOfAdministration::INTRAVASCULAR) != l1MicroROA.end());
        fructose_assert (std::find(l1MacroROA.begin(), l1MacroROA.end(), RouteOfAdministration::INTRAVASCULAR) != l1MacroROA.end());
        fructose_assert (std::find(l1MicroROA.begin(), l1MicroROA.end(), RouteOfAdministration::INFUSION) != l1MicroROA.end());
        fructose_assert (std::find(l1MacroROA.begin(), l1MacroROA.end(), RouteOfAdministration::INFUSION) != l1MacroROA.end());

        // Check that the retrieved calculator is of the correct type
        std::shared_ptr<IntakeIntervalCalculator> l1CMicroExtraCalc =
                l1CMicro->getCalculatorForRoute(RouteOfAdministration::EXTRAVASCULAR);
        fructose_assert (typeid(*l1CMicroExtraCalc) == typeid(OneCompartmentExtraMicro));
        std::shared_ptr<IntakeIntervalCalculator> l1CMacroExtraCalc =
                l1CMacro->getCalculatorForRoute(RouteOfAdministration::EXTRAVASCULAR);
        fructose_assert (typeid(*l1CMacroExtraCalc) == typeid(OneCompartmentExtraMacro));
        std::shared_ptr<IntakeIntervalCalculator> l1CMicroBolusCalc =
                l1CMicro->getCalculatorForRoute(RouteOfAdministration::INTRAVASCULAR);
        fructose_assert (typeid(*l1CMicroBolusCalc) == typeid(OneCompartmentBolusMicro));
        std::shared_ptr<IntakeIntervalCalculator> l1CMacroBolusCalc =
                l1CMacro->getCalculatorForRoute(RouteOfAdministration::INTRAVASCULAR);
        fructose_assert (typeid(*l1CMacroBolusCalc) == typeid(OneCompartmentBolusMacro));
        std::shared_ptr<IntakeIntervalCalculator> l1CMicroInfusionCalc =
                l1CMicro->getCalculatorForRoute(RouteOfAdministration::INFUSION);
        fructose_assert (typeid(*l1CMicroInfusionCalc) == typeid(OneCompartmentInfusionMicro));
        std::shared_ptr<IntakeIntervalCalculator> l1CMacroInfusionCalc =
                l1CMacro->getCalculatorForRoute(RouteOfAdministration::INFUSION);
        fructose_assert (typeid(*l1CMacroInfusionCalc) == typeid(OneCompartmentInfusionMacro));
    }

};

#endif // TEST_PKMODEL_H
