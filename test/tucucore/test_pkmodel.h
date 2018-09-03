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

#include "tucucore/pkmodels/onecompartmentbolus.h"
#include "tucucore/pkmodels/onecompartmentinfusion.h"
#include "tucucore/pkmodels/onecompartmentextra.h"
#include "tucucore/pkmodels/twocompartmentbolus.h"
#include "tucucore/pkmodels/twocompartmentinfusion.h"
#include "tucucore/pkmodels/twocompartmentextra.h"
#include "tucucore/pkmodels/threecompartmentbolus.h"
#include "tucucore/pkmodels/threecompartmentinfusion.h"
#include "tucucore/pkmodels/threecompartmentextra.h"

using namespace Tucuxi::Core;

#define ADD_CALCULATOR_FACTORIES(_PK_MODEL, _COMP_NO_LIT, _TYPE_NAME)  \
do { \
    bool rc; \
    rc = _PK_MODEL->addIntakeIntervalCalculatorFactory(AbsorptionModel::EXTRAVASCULAR, \
                                                       Tucuxi::Core::_COMP_NO_LIT ## CompartmentExtra ## _TYPE_NAME::getCreator()); \
    fructose_assert (rc == true); \
    rc = _PK_MODEL->addIntakeIntervalCalculatorFactory(AbsorptionModel::INTRAVASCULAR, \
                                                       Tucuxi::Core::_COMP_NO_LIT ## CompartmentBolus ## _TYPE_NAME::getCreator()); \
    fructose_assert (rc == true); \
    /* Avoid duplicate insertion */ \
    rc = _PK_MODEL->addIntakeIntervalCalculatorFactory(AbsorptionModel::INTRAVASCULAR, \
                                                       Tucuxi::Core::_COMP_NO_LIT ## CompartmentBolus ## _TYPE_NAME::getCreator()); \
    fructose_assert (rc == false); \
    rc = _PK_MODEL->addIntakeIntervalCalculatorFactory(AbsorptionModel::INFUSION, \
                                                       Tucuxi::Core::_COMP_NO_LIT ## CompartmentInfusion ## _TYPE_NAME::getCreator()); \
    fructose_assert (rc == true); \
} while (0);

#define CHECK_CALCULATOR_TYPE(_PK_MODEL, _COMP_NO_LIT, _TYPE_NAME)  \
do { \
    std::shared_ptr<IntakeIntervalCalculator> CExtraCalc = \
            _PK_MODEL->getCalculatorForRoute(AbsorptionModel::EXTRAVASCULAR); \
    fructose_assert (typeid(*CExtraCalc) == typeid(_COMP_NO_LIT ## CompartmentExtra ## _TYPE_NAME)); \
    std::shared_ptr<IntakeIntervalCalculator> CBolusCalc = \
            _PK_MODEL->getCalculatorForRoute(AbsorptionModel::INTRAVASCULAR); \
    fructose_assert (typeid(*CBolusCalc) == typeid(_COMP_NO_LIT ## CompartmentBolus ## _TYPE_NAME)); \
    std::shared_ptr<IntakeIntervalCalculator> CInfusionCalc = \
            _PK_MODEL->getCalculatorForRoute(AbsorptionModel::INFUSION); \
    fructose_assert (typeid(*CInfusionCalc) == typeid(_COMP_NO_LIT ## CompartmentInfusion ## _TYPE_NAME)); \
} while (0);

struct TestPkModel : public fructose::test_base<TestPkModel>
{
    TestPkModel() { }

    /// \brief Test basic functionalities of a PkModel.
    void testPkModelFunctions(const std::string& /* _testName */)
    {
        std::shared_ptr<PkModel> l1CMicro = std::make_shared<PkModel>("linear.1comp.micro");
        std::shared_ptr<PkModel> l1CMacro = std::make_shared<PkModel>("linear.1comp.macro");
        std::shared_ptr<PkModel> l2CMicro = std::make_shared<PkModel>("linear.2comp.micro");
        std::shared_ptr<PkModel> l2CMacro = std::make_shared<PkModel>("linear.2comp.macro");

        // Check correct model ID
        fructose_assert (l1CMicro->getPkModelId() == "linear.1comp.micro");
        fructose_assert (l1CMacro->getPkModelId() == "linear.1comp.macro");

        ADD_CALCULATOR_FACTORIES(l1CMicro, One, Micro);
        ADD_CALCULATOR_FACTORIES(l1CMacro, One, Macro);
        ADD_CALCULATOR_FACTORIES(l2CMicro, Two, Micro);
        ADD_CALCULATOR_FACTORIES(l2CMacro, Two, Macro);

        // Check that there are no missing route of administration
        std::vector<AbsorptionModel> l1MicroROA = l1CMicro->getAvailableRoutes();
        std::vector<AbsorptionModel> l1MacroROA = l1CMacro->getAvailableRoutes();
        fructose_assert (std::find(l1MicroROA.begin(), l1MicroROA.end(), AbsorptionModel::EXTRAVASCULAR) != l1MicroROA.end());
        fructose_assert (std::find(l1MacroROA.begin(), l1MacroROA.end(), AbsorptionModel::EXTRAVASCULAR) != l1MacroROA.end());
        fructose_assert (std::find(l1MicroROA.begin(), l1MicroROA.end(), AbsorptionModel::INTRAVASCULAR) != l1MicroROA.end());
        fructose_assert (std::find(l1MacroROA.begin(), l1MacroROA.end(), AbsorptionModel::INTRAVASCULAR) != l1MacroROA.end());
        fructose_assert (std::find(l1MicroROA.begin(), l1MicroROA.end(), AbsorptionModel::INFUSION) != l1MicroROA.end());
        fructose_assert (std::find(l1MacroROA.begin(), l1MacroROA.end(), AbsorptionModel::INFUSION) != l1MacroROA.end());

        // Check that the retrieved calculator is of the correct type
        CHECK_CALCULATOR_TYPE(l1CMicro, One, Micro);
        CHECK_CALCULATOR_TYPE(l1CMacro, One, Macro);
        CHECK_CALCULATOR_TYPE(l2CMicro, Two, Micro);
        CHECK_CALCULATOR_TYPE(l2CMacro, Two, Macro);
    }

};

#endif // TEST_PKMODEL_H
