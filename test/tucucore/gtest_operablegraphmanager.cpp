/*
 * Tucuxi - Tucuxi-core library and command line tool.
 * This code allows to perform prediction of drug concentration in blood
 * and to propose dosage adaptations.
 * It has been developed by HEIG-VD, in close collaboration with CHUV.
 * Copyright (C) 2023 HEIG-VD, maintained by Yann Thoma  <yann.thoma@heig-vd.ch>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */


#include <memory>

#include <gtest/gtest.h>

#include "tucucore/operablegraphmanager.h"
#include "tucucore/operation.h"

using namespace Tucuxi::Core;

TEST(Core_TestOpGraph, OperableFunctions)
{
    // Register three inputs, then perform a simple operation with them.
    std::shared_ptr<OperableImpl> a = std::make_shared<OperableImpl>(1.234);
    std::shared_ptr<OperableImpl> b = std::make_shared<OperableImpl>(2);
    std::shared_ptr<OperableImpl> c = std::make_shared<OperableImpl>(1);

    OperableGraphManager ogm;

    ASSERT_TRUE(ogm.registerInput(a, "a"));
    ASSERT_TRUE(ogm.registerInput(b, "b"));
    ASSERT_TRUE(ogm.registerInput(c, "c"));
    // Prevent double insertion.
    ASSERT_FALSE(ogm.registerInput(a, "a"));

    std::shared_ptr<JSOperation> jsOp1 = std::make_shared<JSOperation>(JSExpression(
            "a*(b+c)",
            {OperationInput("a", InputType::DOUBLE),
             OperationInput("b", InputType::DOUBLE),
             OperationInput("c", InputType::DOUBLE)}));

    std::shared_ptr<OperableImpl> op1 = std::make_shared<OperableImpl>(jsOp1);

    ASSERT_TRUE(ogm.registerOperable(op1));
    ASSERT_TRUE(ogm.evaluate());

    double ret = op1->getValue();
    ASSERT_DOUBLE_EQ(ret, 1.234 * 3);

    // Reuse the previous operation, giving it a name, as input for another one.
    std::shared_ptr<JSOperation> jsOp1_bis = std::make_shared<JSOperation>(JSExpression(
            "a*(b+c)",
            {OperationInput("a", InputType::DOUBLE),
             OperationInput("b", InputType::DOUBLE),
             OperationInput("c", InputType::DOUBLE)}));
    std::shared_ptr<OperableImpl> op1_bis = std::make_shared<OperableImpl>(OperableImpl(jsOp1_bis));
    ASSERT_TRUE(ogm.registerOperable(op1_bis, "op1_bis"));
    // Check that we cannot register it twice.
    ASSERT_FALSE(ogm.registerOperable(op1_bis, "op1_bis"));

    std::shared_ptr<JSOperation> jsOp2 = std::make_shared<JSOperation>(JSExpression(
            "op1_bis - d", {OperationInput("op1_bis", InputType::DOUBLE), OperationInput("d", InputType::DOUBLE)}));

    std::shared_ptr<OperableImpl> op2 = std::make_shared<OperableImpl>(OperableImpl(jsOp2));
    // Check that we can register the operation even before all the inputs are available.
    ASSERT_TRUE(ogm.registerOperable(op2, "op2"));

    std::shared_ptr<OperableImpl> d = std::make_shared<OperableImpl>(4.321);
    ASSERT_TRUE(ogm.registerInput(d, "d"));

    ASSERT_TRUE(ogm.evaluate());

    // Check that previous value unaltered.
    ret = op1->getValue();
    ASSERT_DOUBLE_EQ(ret, 1.234 * 3);
    ASSERT_DOUBLE_EQ(op1->getValue(), op1_bis->getValue());
    // Check that new value correct
    ret = op2->getValue();
    ASSERT_DOUBLE_EQ(ret, 1.234 * 3 - 4.321);
    // Check that values can be retrieved via the graph.
    ret = 0.0;
    ASSERT_TRUE(ogm.getValue("op2", ret));
    ASSERT_DOUBLE_EQ(ret, 1.234 * 3 - 4.321);
    ASSERT_TRUE(ogm.getValue("op1_bis", ret));
    ASSERT_DOUBLE_EQ(ret, 1.234 * 3);

    // Alter values and check that the computations adjust.
    a->setValue(2.345);
    b->setValue(5);
    c->setValue(3.231);
    ASSERT_TRUE(ogm.evaluate());

    ret = op1->getValue();
    ASSERT_DOUBLE_EQ(ret, 2.345 * (5 + 3.231));
    ret = op2->getValue();
    ASSERT_DOUBLE_EQ(ret, 2.345 * (5 + 3.231) - 4.321);
    ret = 0.0;
    ASSERT_TRUE(ogm.getValue("op2", ret));
    ASSERT_DOUBLE_EQ(ret, 2.345 * (5 + 3.231) - 4.321);
    ASSERT_TRUE(ogm.getValue("op1_bis", ret));
    ASSERT_DOUBLE_EQ(ret, 2.345 * (5 + 3.231));
}

TEST(Core_TestOpGraph, OperableCockcroftGaultIBW)
{
    std::shared_ptr<OperableImpl> weight = std::make_shared<OperableImpl>(71.4);
    std::shared_ptr<OperableImpl> height = std::make_shared<OperableImpl>(165);
    std::shared_ptr<OperableImpl> age = std::make_shared<OperableImpl>(49);
    std::shared_ptr<OperableImpl> creatinine = std::make_shared<OperableImpl>(23.4);
    std::shared_ptr<OperableImpl> isMale = std::make_shared<OperableImpl>(1);

    OperableGraphManager ogm;

    ASSERT_TRUE(ogm.registerInput(weight, "bodyweight"));
    ASSERT_TRUE(ogm.registerInput(height, "height"));
    ASSERT_TRUE(ogm.registerInput(age, "age"));
    ASSERT_TRUE(ogm.registerInput(creatinine, "creatinine"));
    ASSERT_TRUE(ogm.registerInput(isMale, "isMale"));

    std::shared_ptr<JSOperation> jsIBW = std::make_shared<JSOperation>(JSExpression(
            "height - 100 - (height - 150) / ((isMale > 0.5)*4.0 + (!(isMale > 0.5))*2.5)",
            {OperationInput("height", InputType::DOUBLE), OperationInput("isMale", InputType::DOUBLE)}));
    std::shared_ptr<OperableImpl> opIBW = std::make_shared<OperableImpl>(jsIBW);
    ASSERT_TRUE(ogm.registerOperable(opIBW, "IBW"));

    std::shared_ptr<JSOperation> jsCG_IBW = std::make_shared<JSOperation>(JSExpression(
            "(140 - age) * (bodyweight * (bodyweight < IBW) + IBW * (bodyweight >= IBW)) / creatinine * (1.23 * (isMale > 0.5) + 1.04 * (!(isMale > 0.5)))",
            {OperationInput("bodyweight", InputType::DOUBLE),
             OperationInput("IBW", InputType::DOUBLE),
             OperationInput("age", InputType::DOUBLE),
             OperationInput("creatinine", InputType::DOUBLE),
             OperationInput("isMale", InputType::DOUBLE)}));
    std::shared_ptr<OperableImpl> opCG_IBW = std::make_shared<OperableImpl>(jsCG_IBW);
    ASSERT_TRUE(ogm.registerOperable(opCG_IBW));

    ASSERT_TRUE(ogm.evaluate());

    ASSERT_DOUBLE_EQ(61.25, opIBW->getValue());
    ASSERT_DOUBLE_EQ(292.979167, opCG_IBW->getValue());
}

TEST(Core_TestOpGraph, OperableCyclic)
{
    // Register three inputs, then perform a simple operation with them.
    std::shared_ptr<OperableImpl> a = std::make_shared<OperableImpl>(1.234);
    std::shared_ptr<OperableImpl> b = std::make_shared<OperableImpl>(2);
    std::shared_ptr<OperableImpl> c = std::make_shared<OperableImpl>(1);
    std::shared_ptr<OperableImpl> d = std::make_shared<OperableImpl>(143);

    OperableGraphManager ogm;

    ASSERT_TRUE(ogm.registerInput(a, "a"));
    ASSERT_TRUE(ogm.registerInput(b, "b"));
    ASSERT_TRUE(ogm.registerInput(c, "c"));
    ASSERT_TRUE(ogm.registerInput(d, "d"));

    std::shared_ptr<JSOperation> jsOp1 = std::make_shared<JSOperation>(JSExpression(
            "a*(b+c)-op2",
            {OperationInput("a", InputType::DOUBLE),
             OperationInput("b", InputType::DOUBLE),
             OperationInput("c", InputType::DOUBLE),
             OperationInput("op2", InputType::DOUBLE)}));
    std::shared_ptr<OperableImpl> op1 = std::make_shared<OperableImpl>(jsOp1);
    ASSERT_TRUE(ogm.registerOperable(op1, "op1"));

    std::shared_ptr<JSOperation> jsOp2 = std::make_shared<JSOperation>(JSExpression(
            "op1 - d", {OperationInput("op1", InputType::DOUBLE), OperationInput("d", InputType::DOUBLE)}));
    std::shared_ptr<OperableImpl> op2 = std::make_shared<OperableImpl>(jsOp2);
    ASSERT_TRUE(ogm.registerOperable(op2, "op2"));

    ASSERT_FALSE(ogm.evaluate());
}
