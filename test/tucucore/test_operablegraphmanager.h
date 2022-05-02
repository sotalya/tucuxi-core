//@@license@@

#ifndef TEST_OPERABLEGRAPHMANAGER_H
#define TEST_OPERABLEGRAPHMANAGER_H

#include <algorithm>
#include <iostream>
#include <memory>

#include "tucucore/operablegraphmanager.h"
#include "tucucore/operation.h"

#include "fructose/fructose.h"

using namespace Tucuxi::Core;


struct TestOpGraph : public fructose::test_base<TestOpGraph>
{
    TestOpGraph() {}

    /// \brief Test basic functionalities of an Operable.
    void testOperableFunctions(const std::string& /* _testName */)
    {
        // Register three inputs, then perform a simple operation with them.
        std::shared_ptr<OperableImpl> a = std::make_shared<OperableImpl>(1.234);
        std::shared_ptr<OperableImpl> b = std::make_shared<OperableImpl>(2);
        std::shared_ptr<OperableImpl> c = std::make_shared<OperableImpl>(1);

        OperableGraphManager ogm;

        fructose_assert(ogm.registerInput(a, "a") == true);
        fructose_assert(ogm.registerInput(b, "b") == true);
        fructose_assert(ogm.registerInput(c, "c") == true);
        // Prevent double insertion.
        fructose_assert(ogm.registerInput(a, "a") == false);

        std::shared_ptr<JSOperation> jsOp1 = std::make_shared<JSOperation>(JSExpression(
                "a*(b+c)",
                {OperationInput("a", InputType::DOUBLE),
                 OperationInput("b", InputType::DOUBLE),
                 OperationInput("c", InputType::DOUBLE)}));

        std::shared_ptr<OperableImpl> op1 = std::make_shared<OperableImpl>(jsOp1);

        fructose_assert(ogm.registerOperable(op1) == true);
        fructose_assert(ogm.evaluate() == true);

        double ret = op1->getValue();
        fructose_assert_double_eq(ret, 1.234 * 3);

        // Reuse the previous operation, giving it a name, as input for another one.
        std::shared_ptr<JSOperation> jsOp1_bis = std::make_shared<JSOperation>(JSExpression(
                "a*(b+c)",
                {OperationInput("a", InputType::DOUBLE),
                 OperationInput("b", InputType::DOUBLE),
                 OperationInput("c", InputType::DOUBLE)}));
        std::shared_ptr<OperableImpl> op1_bis = std::make_shared<OperableImpl>(OperableImpl(jsOp1_bis));
        fructose_assert(ogm.registerOperable(op1_bis, "op1_bis") == true);
        // Check that we cannot register it twice.
        fructose_assert(ogm.registerOperable(op1_bis, "op1_bis") == false);

        std::shared_ptr<JSOperation> jsOp2 = std::make_shared<JSOperation>(JSExpression(
                "op1_bis - d", {OperationInput("op1_bis", InputType::DOUBLE), OperationInput("d", InputType::DOUBLE)}));

        std::shared_ptr<OperableImpl> op2 = std::make_shared<OperableImpl>(OperableImpl(jsOp2));
        // Check that we can register the operation even before all the inputs are available.
        fructose_assert(ogm.registerOperable(op2, "op2") == true);

        std::shared_ptr<OperableImpl> d = std::make_shared<OperableImpl>(4.321);
        fructose_assert(ogm.registerInput(d, "d") == true);

        fructose_assert(ogm.evaluate() == true);

        // Check that previous value unaltered.
        ret = op1->getValue();
        fructose_assert_double_eq(ret, 1.234 * 3);
        fructose_assert_double_eq(op1->getValue(), op1_bis->getValue());
        // Check that new value correct
        ret = op2->getValue();
        fructose_assert_double_eq(ret, 1.234 * 3 - 4.321);
        // Check that values can be retrieved via the graph.
        ret = 0.0;
        fructose_assert(ogm.getValue("op2", ret) == true);
        fructose_assert_double_eq(ret, 1.234 * 3 - 4.321);
        fructose_assert(ogm.getValue("op1_bis", ret) == true);
        fructose_assert_double_eq(ret, 1.234 * 3);

        // Alter values and check that the computations adjust.
        a->setValue(2.345);
        b->setValue(5);
        c->setValue(3.231);
        fructose_assert(ogm.evaluate() == true);

        ret = op1->getValue();
        fructose_assert_double_eq(ret, 2.345 * (5 + 3.231));
        ret = op2->getValue();
        fructose_assert_double_eq(ret, 2.345 * (5 + 3.231) - 4.321);
        ret = 0.0;
        fructose_assert(ogm.getValue("op2", ret) == true);
        fructose_assert_double_eq(ret, 2.345 * (5 + 3.231) - 4.321);
        fructose_assert(ogm.getValue("op1_bis", ret) == true);
        fructose_assert_double_eq(ret, 2.345 * (5 + 3.231));
    }


    void testOperableCockcroftGaultIBW(const std::string& /* _testName */)
    {
        std::shared_ptr<OperableImpl> weight = std::make_shared<OperableImpl>(71.4);
        std::shared_ptr<OperableImpl> height = std::make_shared<OperableImpl>(165);
        std::shared_ptr<OperableImpl> age = std::make_shared<OperableImpl>(49);
        std::shared_ptr<OperableImpl> creatinine = std::make_shared<OperableImpl>(23.4);
        std::shared_ptr<OperableImpl> isMale = std::make_shared<OperableImpl>(1);

        OperableGraphManager ogm;

        fructose_assert(ogm.registerInput(weight, "bodyweight") == true);
        fructose_assert(ogm.registerInput(height, "height") == true);
        fructose_assert(ogm.registerInput(age, "age") == true);
        fructose_assert(ogm.registerInput(creatinine, "creatinine") == true);
        fructose_assert(ogm.registerInput(isMale, "isMale") == true);

        std::shared_ptr<JSOperation> jsIBW = std::make_shared<JSOperation>(JSExpression(
                "height - 100 - (height - 150) / ((isMale > 0.5)*4.0 + (!(isMale > 0.5))*2.5)",
                {OperationInput("height", InputType::DOUBLE), OperationInput("isMale", InputType::DOUBLE)}));
        std::shared_ptr<OperableImpl> opIBW = std::make_shared<OperableImpl>(jsIBW);
        fructose_assert(ogm.registerOperable(opIBW, "IBW") == true);

        std::shared_ptr<JSOperation> jsCG_IBW = std::make_shared<JSOperation>(JSExpression(
                "(140 - age) * (bodyweight * (bodyweight < IBW) + IBW * (bodyweight >= IBW)) / creatinine * (1.23 * (isMale > 0.5) + 1.04 * (!(isMale > 0.5)))",
                {OperationInput("bodyweight", InputType::DOUBLE),
                 OperationInput("IBW", InputType::DOUBLE),
                 OperationInput("age", InputType::DOUBLE),
                 OperationInput("creatinine", InputType::DOUBLE),
                 OperationInput("isMale", InputType::DOUBLE)}));
        std::shared_ptr<OperableImpl> opCG_IBW = std::make_shared<OperableImpl>(jsCG_IBW);
        fructose_assert(ogm.registerOperable(opCG_IBW) == true);

        fructose_assert(ogm.evaluate() == true);

        fructose_assert_double_eq(61.25, opIBW->getValue());
        fructose_assert_double_eq(292.979167, opCG_IBW->getValue());
    }


    void testOperableCyclic(const std::string& /* _testName */)
    {
        // Register three inputs, then perform a simple operation with them.
        std::shared_ptr<OperableImpl> a = std::make_shared<OperableImpl>(1.234);
        std::shared_ptr<OperableImpl> b = std::make_shared<OperableImpl>(2);
        std::shared_ptr<OperableImpl> c = std::make_shared<OperableImpl>(1);
        std::shared_ptr<OperableImpl> d = std::make_shared<OperableImpl>(143);

        OperableGraphManager ogm;

        fructose_assert(ogm.registerInput(a, "a") == true);
        fructose_assert(ogm.registerInput(b, "b") == true);
        fructose_assert(ogm.registerInput(c, "c") == true);
        fructose_assert(ogm.registerInput(d, "d") == true);

        std::shared_ptr<JSOperation> jsOp1 = std::make_shared<JSOperation>(JSExpression(
                "a*(b+c)-op2",
                {OperationInput("a", InputType::DOUBLE),
                 OperationInput("b", InputType::DOUBLE),
                 OperationInput("c", InputType::DOUBLE),
                 OperationInput("op2", InputType::DOUBLE)}));
        std::shared_ptr<OperableImpl> op1 = std::make_shared<OperableImpl>(jsOp1);
        fructose_assert(ogm.registerOperable(op1, "op1") == true);

        std::shared_ptr<JSOperation> jsOp2 = std::make_shared<JSOperation>(JSExpression(
                "op1 - d", {OperationInput("op1", InputType::DOUBLE), OperationInput("d", InputType::DOUBLE)}));
        std::shared_ptr<OperableImpl> op2 = std::make_shared<OperableImpl>(jsOp2);
        fructose_assert(ogm.registerOperable(op2, "op2") == true);

        fructose_assert(ogm.evaluate() == false);
    }
};

#endif // TEST_OPERABLEGRAPHMANAGER_H
