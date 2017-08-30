/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TEST_OPGRAPHMANAGER_H
#define TEST_OPGRAPHMANAGER_H

#include <algorithm>
#include <iostream>
#include <memory>

#include "fructose/fructose.h"

#include "tucucore/operablegraphmanager.h"
#include "tucucore/operation.h"

using namespace Tucuxi::Core;


struct TestOpGraph : public fructose::test_base<TestOpGraph>
{
    TestOpGraph() { }

    /// \brief Test basic functionalities of an Operable.
    void testOperableFunctions(const std::string& /* _testName */)
    {
        // Register three inputs, then perform a simple operation with them.
        std::shared_ptr<Operable> a = std::make_shared<Operable>(1.234);
        std::shared_ptr<Operable> b = std::make_shared<Operable>(2);
        std::shared_ptr<Operable> c = std::make_shared<Operable>(1);

        OperableGraphManager ogm;

        fructose_assert (ogm.registerInput(a, "a") == true);
        fructose_assert (ogm.registerInput(b, "b") == true);
        fructose_assert (ogm.registerInput(c, "c") == true);
        // Prevent double insertion.
        fructose_assert (ogm.registerInput(a, "a") == false);

        std::shared_ptr<JSOperation> jsOp1
                = std::make_shared<JSOperation>(JSOperation("a*(b+c)", { OperationInput("a", InputType::DOUBLE),
                                                                         OperationInput("b", InputType::DOUBLE),
                                                                         OperationInput("c", InputType::DOUBLE) }));

        std::shared_ptr<Operable> op1 = std::make_shared<Operable>(jsOp1);

        fructose_assert (ogm.registerOperable(op1) == true);
        fructose_assert (ogm.evaluate() == true);

        double ret = op1->getValue();
        fructose_assert_double_eq (ret, 1.234 * 3);

        // Reuse the previous operation, giving it a name, as input for another one.
        std::shared_ptr<JSOperation> jsOp1_bis
                = std::make_shared<JSOperation>(JSOperation("a*(b+c)", { OperationInput("a", InputType::DOUBLE),
                                                                         OperationInput("b", InputType::DOUBLE),
                                                                         OperationInput("c", InputType::DOUBLE) }));
        std::shared_ptr<Operable> op1_bis = std::make_shared<Operable>(Operable(jsOp1_bis));
        fructose_assert (ogm.registerOperable(op1_bis, "op1_bis") == true);
        // Check that we cannot register it twice.
        fructose_assert (ogm.registerOperable(op1_bis, "op1_bis") == false);

        std::shared_ptr<JSOperation> jsOp2
                = std::make_shared<JSOperation>(JSOperation("op1_bis - d", { OperationInput("op1_bis", InputType::DOUBLE),
                                                                             OperationInput("d", InputType::DOUBLE) }));

        std::shared_ptr<Operable> op2 = std::make_shared<Operable>(Operable(jsOp2));
        // Check that we can register the operation even before all the inputs are available.
        fructose_assert (ogm.registerOperable(op2, "op2") == true);

        std::shared_ptr<Operable> d = std::make_shared<Operable>(4.321);
        fructose_assert (ogm.registerInput(d, "d") == true);

        fructose_assert (ogm.evaluate() == true);

        // Check that previous value unaltered.
        ret = op1->getValue();
        fructose_assert_double_eq (ret, 1.234 * 3);
        fructose_assert_double_eq (op1->getValue(), op1_bis->getValue());
        // Check that new value correct
        ret = op2->getValue();
        fructose_assert_double_eq (ret, 1.234 * 3 - 4.321);
        // Check that values can be retrieved via the graph.
        ret = 0.0;
        fructose_assert (ogm.getValue("op2", ret) == true);
        fructose_assert_double_eq (ret, 1.234 * 3 - 4.321);
        fructose_assert (ogm.getValue("op1_bis", ret) == true);
        fructose_assert_double_eq (ret, 1.234 * 3);

        // Alter values and check that the computations adjust.
        a->setValue(2.345);
        b->setValue(5);
        c->setValue(3.231);
        fructose_assert (ogm.evaluate() == true);

        ret = op1->getValue();
        fructose_assert_double_eq (ret, 2.345 * (5 + 3.231));
        ret = op2->getValue();
        fructose_assert_double_eq (ret, 2.345 * (5 + 3.231) - 4.321);
        ret = 0.0;
        fructose_assert (ogm.getValue("op2", ret) == true);
        fructose_assert_double_eq (ret, 2.345 * (5 + 3.231) - 4.321);
        fructose_assert (ogm.getValue("op1_bis", ret) == true);
        fructose_assert_double_eq (ret, 2.345 * (5 + 3.231));
    }


    void testOperableCockcroftGaultIBW(const std::string& /* _testName */)
    {
        std::shared_ptr<Operable> weight = std::make_shared<Operable>(71.4);
        std::shared_ptr<Operable> height = std::make_shared<Operable>(165);
        std::shared_ptr<Operable> age = std::make_shared<Operable>(49);
        std::shared_ptr<Operable> creatinine = std::make_shared<Operable>(23.4);
        std::shared_ptr<Operable> isMale = std::make_shared<Operable>(1);

        OperableGraphManager ogm;

        fructose_assert (ogm.registerInput(weight, "weight") == true);
        fructose_assert (ogm.registerInput(height, "height") == true);
        fructose_assert (ogm.registerInput(age, "age") == true);
        fructose_assert (ogm.registerInput(creatinine, "creatinine") == true);
        fructose_assert (ogm.registerInput(isMale, "isMale") == true);

        std::shared_ptr<JSOperation> jsIBW
                = std::make_shared<JSOperation>(JSOperation("height - 100 - (height - 150) / ((isMale > 0.5)*4.0 + (!(isMale > 0.5))*2.5)",
        { OperationInput("height", InputType::DOUBLE),
          OperationInput("isMale", InputType::DOUBLE) }));
        std::shared_ptr<Operable> opIBW = std::make_shared<Operable>(jsIBW);
        fructose_assert (ogm.registerOperable(opIBW, "IBW") == true);

        std::shared_ptr<JSOperation> jsCG_IBW
                = std::make_shared<JSOperation>(JSOperation("(140 - age) * (weight * (weight < IBW) + IBW * (weight >= IBW)) / creatinine * (1.23 * (isMale > 0.5) + 1.04 * (!(isMale > 0.5)))",
        { OperationInput("weight", InputType::DOUBLE),
          OperationInput("IBW", InputType::DOUBLE),
          OperationInput("age", InputType::DOUBLE),
          OperationInput("creatinine", InputType::DOUBLE),
          OperationInput("isMale", InputType::DOUBLE) }));
        std::shared_ptr<Operable> opCG_IBW = std::make_shared<Operable>(jsCG_IBW);
        fructose_assert (ogm.registerOperable(opCG_IBW) == true);

        fructose_assert (ogm.evaluate() == true);

        fructose_assert_double_eq (61.25, opIBW->getValue());
        fructose_assert_double_eq (292.979167, opCG_IBW->getValue());
    }


    void testOperableCyclic(const std::string& /* _testName */)
    {
        // Register three inputs, then perform a simple operation with them.
        std::shared_ptr<Operable> a = std::make_shared<Operable>(1.234);
        std::shared_ptr<Operable> b = std::make_shared<Operable>(2);
        std::shared_ptr<Operable> c = std::make_shared<Operable>(1);
        std::shared_ptr<Operable> d = std::make_shared<Operable>(143);

        OperableGraphManager ogm;

        fructose_assert (ogm.registerInput(a, "a") == true);
        fructose_assert (ogm.registerInput(b, "b") == true);
        fructose_assert (ogm.registerInput(c, "c") == true);
        fructose_assert (ogm.registerInput(d, "d") == true);

        std::shared_ptr<JSOperation> jsOp1
                = std::make_shared<JSOperation>(JSOperation("a*(b+c)-op2", { OperationInput("a", InputType::DOUBLE),
                                                                             OperationInput("b", InputType::DOUBLE),
                                                                             OperationInput("c", InputType::DOUBLE),
                                                                             OperationInput("op2", InputType::DOUBLE) }));
        std::shared_ptr<Operable> op1 = std::make_shared<Operable>(jsOp1);
        fructose_assert (ogm.registerOperable(op1, "op1") == true);

        std::shared_ptr<JSOperation> jsOp2
                = std::make_shared<JSOperation>(JSOperation("op1 - d", { OperationInput("op1", InputType::DOUBLE),
                                                                         OperationInput("d", InputType::DOUBLE) }));
        std::shared_ptr<Operable> op2 = std::make_shared<Operable>(jsOp2);
        fructose_assert (ogm.registerOperable(op2, "op2") == true);

        fructose_assert (ogm.evaluate() == false);
    }
};

#endif // TEST_OPGRAPHMANAGER_H
