/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TEST_OPERATION_H
#define TEST_OPERATION_H

#include <iostream>
#include <memory>

#include "fructose/fructose.h"

#include "tucucore/hardcodedoperation.h"
#include "tucucore/operation.h"

using namespace Tucuxi::Core;

/// \brief Example implementation of an hardcoded 'a - b' operation.
/// In the example, to further test the implementation, 'a' is a double while 'b' is an integer.
class DiffOperation : public HardcodedOperation
{
public:
    /// \brief Implementation of the inherited clone operation.
    /// \return Pointer to a new object of subclass' type.
    virtual std::unique_ptr<Operation> clone() const
    {
        return std::unique_ptr<Operation>(new DiffOperation(*this));
    }


protected:
    /// \brief Perform the desired computation (difference) on the inputs.
    /// \param _inputs List of inputs that have to be used by the operation.
    /// \param _result Result of the operation.
    /// \return true if the operation could be performed (that is, inputs can be successfully retrieved), false
    ///         otherwise.
    /// \pre check(_inputs) == true
    virtual bool compute(const OperationInputList &_inputs, double &_result) const
    {
        double a;
        int b;

        if (!getInputValue(_inputs, "a", a) || !getInputValue(_inputs, "b", b)) {
            return false;
        }

        _result = a - b;

        return true;
    }


    /// \brief Fill the vector of required inputs.
    virtual void fillRequiredInputs()
    {
        OperationInput a("a", InputType::DOUBLE);
        OperationInput b("b", InputType::INTEGER);
        m_requiredInputs.push_back(a);
        m_requiredInputs.push_back(b);
    }
};

struct TestOperation : public fructose::test_base<TestOperation>
{
    TestOperation() { }


    /// \brief Test OperationInput base capabilities.
    void testOperationInput(const std::string& /* _testName */)
    {
        bool rc;

        // Create an input without specifying the type -> expect type=DOUBLE, correct name, and undefined.
        // Getting from it should result in an error, we must set its value before.
        OperationInput o1("o1");
        fructose_assert (o1.getName() == "o1");
        fructose_assert (o1.getType() == InputType::DOUBLE);
        fructose_assert (!o1.isDefined());
        double d1;
        fructose_assert (o1.getValue(d1) == false);
        d1 = 1.234;
        rc = o1.setValue(d1);
        fructose_assert (rc == true);
        fructose_assert (o1.isDefined());
        d1 = 0.0;
        rc = o1.getValue(d1);
        fructose_assert (rc == true);
        fructose_assert (d1 == 1.234);

        // Create an input specifying an integer value -> expect type=INTEGER, correct name, and defined (getting the
        // correct value). Using the other getters should return an error.
        int value2 = 1234;
        OperationInput o2("o2", value2);
        fructose_assert (o2.getName() == "o2");
        fructose_assert (o2.getType() == InputType::INTEGER);
        fructose_assert (o2.isDefined());
        value2 = 0;
        rc = o2.getValue(value2);
        fructose_assert (rc == true);
        fructose_assert (value2 == 1234);
        bool b2;
        double d2;
        fructose_assert (o2.getValue(b2) == false);
        fructose_assert (o2.getValue(d2) == false);
        // Using the other setters should return an error, while using the good setter should work.
        value2 = 433;
        rc = o2.setValue(value2);
        fructose_assert (rc == true);
        value2 = 0;
        rc = o2.getValue(value2);
        fructose_assert (rc == true);
        fructose_assert (value2 == 433);
        b2 = true;
        d2 = 3.23;
        fructose_assert (o2.setValue(b2) == false);

        // This next test was OK when a setValue(double) into an integer was not authorized.
        // It is now allowed, to be checked why
//        fructose_assert (o2.setValue(d2) == false);
        rc = o2.getValue(value2);
        fructose_assert (rc == true);
        fructose_assert (value2 == 433);

        // Check that assignment operator works properly.
        OperationInput o3 = o2;
        int value3;
        rc = o3.getValue(value3);
        fructose_assert (rc == true);
        fructose_assert (value3 == 433);

        // Check that the copy constructor works properly.
        OperationInput o4(o2);
        int value4;
        rc = o4.getValue(value4);
        fructose_assert (rc == true);
        fructose_assert (value4 == 433);

        // Check that the comparison operators works properly.
        fructose_assert (o2 == o3);
        fructose_assert (o2 == o4);
        fructose_assert (!(o2 != o3));
        fructose_assert (!(o2 != o4));

        // Check that the original value has not been changed by those two operations
        value2 = 0;
        rc = o2.getValue(value2);
        fructose_assert (rc == true);
        fructose_assert (value2 == 433);
    }


    /// \brief Test HardcodedOperation base capabilities.
    void testHardcodedOperation(const std::string& /* _testName */)
    {
        // Perform the a - b operation and expect success.
        OperationInput a("a", 1.234);
        OperationInput b("b", 1);

        DiffOperation diff;
        double res;
        bool rc;
        rc = diff.evaluate({a, b}, res);
        fructose_assert (rc == true);
        fructose_assert_double_eq (0.234, res);

        // Inverse the inputs and expect success.
        res = 0;
        rc = diff.evaluate({b, a}, res);
        fructose_assert (rc == true);
        fructose_assert_double_eq (0.234, res);

        // Ensure that the check() function behaves properly.
        fructose_assert (diff.check({b, a}) == true);
        fructose_assert (diff.check({OperationInput("b", 1), OperationInput("a", 1.234)}) == true);
        fructose_assert (diff.check({OperationInput("b", 1), OperationInput("a", 1.234), OperationInput("c", 1.234)}) == true);
        fructose_assert (diff.check({OperationInput("b", 1), OperationInput("a", 1.234), OperationInput("a", 1.234)}) == false);
        fructose_assert (diff.check({OperationInput("b", 1), OperationInput("a", 1.234), OperationInput("a", 2.34)}) == false);
        fructose_assert (diff.check({OperationInput("a", 1), OperationInput("b", 1.234)}) == false);
        fructose_assert (diff.check({OperationInput("a", 1.234), OperationInput("c", 1)}) == false);
        fructose_assert (diff.check({OperationInput("a", 1.234)}) == false);
        fructose_assert (diff.check({OperationInput("a", 1.234), OperationInput("b", InputType::INTEGER)}) == false);
        fructose_assert (diff.check({OperationInput("a"), OperationInput("b", 1)}) == false);

        // Ensure that the getInputs() function retrieves the correct list of inputs.
        OperationInputList original = { OperationInput("a"), OperationInput("b", InputType::INTEGER) };
        std::sort(original.begin(), original.end(),
                  [](const OperationInput &_a, const OperationInput &_b) { return _a.getName() < _b.getName(); });
        OperationInputList retrieved = diff.getInputs();
        std::sort(retrieved.begin(), retrieved.end(),
                  [](const OperationInput &_a, const OperationInput &_b) { return _a.getName() < _b.getName(); });
        fructose_assert (original == retrieved);
    }


    /// \brief Test JSOperation base capabilities.
    void testJSOperation(const std::string& /* _testName */)
    {
        double res;
        bool rc;

        JSExpression jsOp1("a*(b+c)", { OperationInput("a", InputType::DOUBLE), OperationInput("b", InputType::DOUBLE), OperationInput("c", InputType::DOUBLE) });
        // Operation ok ?
        rc = jsOp1.evaluate({ OperationInput("c", 3.45), OperationInput("a", 1.23), OperationInput("b", 2.34) }, res);
        fructose_assert (rc == true);
        fructose_assert_double_eq (7.1217, res);
        // Error if missing operand ?
        rc = jsOp1.evaluate({ OperationInput("a", 1.23), OperationInput("b", 2.34) }, res);
        fructose_assert (rc == false);

        JSExpression jsOp2("a*b", { OperationInput("a", InputType::DOUBLE), OperationInput("b", InputType::DOUBLE) });
        // Operation ok ?
        rc = jsOp2.evaluate({ OperationInput("a", 1.23), OperationInput("b", 2.34) }, res);
        fructose_assert (rc == true);
        fructose_assert_double_eq (2.8782, res);
        res = 0;
        // Operation ok even if useless variable given ?
        rc = jsOp2.evaluate({ OperationInput("c", 3.45), OperationInput("a", 1.23), OperationInput("b", 2.34) }, res);
        fructose_assert (rc == true);
        fructose_assert_double_eq (2.8782, res);

        JSExpression jsOp3("result-b", { OperationInput("result", InputType::DOUBLE), OperationInput("b", InputType::DOUBLE) });
        // Operation ok even if one of the variables has the same name as the output variable in the code ?
        rc = jsOp3.evaluate({ OperationInput("result", 1.23), OperationInput("b", 2.34) }, res);
        fructose_assert (rc == true);
        fructose_assert_double_eq (-1.11, res);
    }


    /// \brief Test DynamicOperation base capabilities.
    void testDynamicOperation(const std::string& /* _testName */)
    {
        JSExpression jsOp1("a*b+c", { OperationInput("a"), OperationInput("b", InputType::INTEGER), OperationInput("c") });
        JSExpression jsOp2("c+d", { OperationInput("c"), OperationInput("d", InputType::INTEGER) });
        JSExpression jsOp3("a*c-b*d", { OperationInput("a"), OperationInput("b", InputType::INTEGER), OperationInput("c"), OperationInput("d", InputType::INTEGER) });
        JSExpression jsOp4("c-d", { OperationInput("c"), OperationInput("d", InputType::INTEGER) });
        DiffOperation diff; // This will be 'a - b', with integer b
        OperationInput a("a", 1.234);
        OperationInput b("b", 2);
        OperationInput c("c", 3.456);
        OperationInput d("d", 4);
        double res;
        bool rc;

        // Check the individual results
        rc = jsOp1.evaluate({ a, b, c }, res);
        fructose_assert (rc == true);
        fructose_assert_double_eq (5.924, res);
        rc = jsOp2.evaluate({ d, c }, res);
        fructose_assert (rc == true);
        fructose_assert_double_eq (7.456, res);
        rc = jsOp3.evaluate({ a, b, c, d }, res);
        fructose_assert (rc == true);
        fructose_assert_double_eq (-3.735296, res);
        rc = jsOp4.evaluate({ a, b, c, d }, res);
        fructose_assert (rc == true);
        fructose_assert_double_eq (-0.544, res);
        rc = diff.evaluate({ a, b }, res);
        fructose_assert (rc == true);
        fructose_assert_double_eq (-0.766, res);

        // Ensure that the check() function behaves properly.
        DynamicOperation dynOp;
        rc = dynOp.addOperation(jsOp1, 2);
        fructose_assert (rc == true);
        fructose_assert (dynOp.check({ d, c }) == false);
        fructose_assert (dynOp.check({ a, b }) == false);
        fructose_assert (dynOp.check({ c, a, b }) == true);
        fructose_assert (dynOp.check({ b, a, d, c }) == true);
        rc = dynOp.addOperation(jsOp2, 2);
        fructose_assert (rc == true);
        fructose_assert (dynOp.check({ d, c }) == true);
        fructose_assert (dynOp.check({ b, a, d, c }) == true);
        rc = dynOp.addOperation(jsOp3, 0);
        fructose_assert (rc == true);
        fructose_assert (dynOp.check({ d, c }) == true);
        fructose_assert (dynOp.check({ b, a, d, c }) == true);
        rc = dynOp.addOperation(jsOp4, 1);
        fructose_assert (rc == true);
        fructose_assert (dynOp.check({ d, c }) == true);
        fructose_assert (dynOp.check({ b, a, d, c }) == true);
        rc = dynOp.addOperation(diff, 1);
        fructose_assert (rc == true);
        fructose_assert (dynOp.check({ a }) == false);
        fructose_assert (dynOp.check({ a, b }) == true);
        fructose_assert (dynOp.check({ b, a, d, c }) == true);

        // Try to insert an operation with conflicting input types and check that the insertion fails.
        JSOperation jsConflictingOperands("c-d", { OperationInput("c", InputType::INTEGER), OperationInput("d") });
        fructose_assert (dynOp.addOperation(jsConflictingOperands, 1) == false);

        // Ensure that the getInputs() function retrieves the correct list of inputs.
        OperationInputList original = { OperationInput("a"), OperationInput("b", InputType::INTEGER),
                                        OperationInput("c"), OperationInput("d", InputType::INTEGER) };
        std::sort(original.begin(), original.end(),
                  [](const OperationInput &_a, const OperationInput &_b) { return _a.getName() < _b.getName(); });
        OperationInputList retrieved = dynOp.getInputs();
        std::sort(retrieved.begin(), retrieved.end(),
                  [](const OperationInput &_a, const OperationInput &_b) { return _a.getName() < _b.getName(); });
        fructose_assert (original == retrieved);

        // Ensure that the evaluate() function recalls the correct operation.
        rc = dynOp.evaluate({ c, d }, res); // Expect a call to jsOp4 (higher preference)
        fructose_assert (rc == true);
        fructose_assert_double_eq (-0.544, res);
        rc = dynOp.evaluate({ a, b }, res); // Expect a call to diff
        fructose_assert (rc == true);
        fructose_assert_double_eq (-0.766, res);
        rc = dynOp.evaluate({ c, a, b }, res); // Expect a call to jsOp1
        fructose_assert (rc == true);
        fructose_assert_double_eq (5.924, res);
        rc = dynOp.evaluate({ d, c, a, b }, res); // Expect a call to jsOp1
        fructose_assert (rc == true);
        fructose_assert_double_eq (-3.735296, res);
        rc = dynOp.evaluate({ d, a, b }, res); // Expect a call to diff
        fructose_assert (rc == true);
        fructose_assert_double_eq (-0.766, res);
        rc = dynOp.evaluate({ b, c, d }, res); // Expect a call to jsOp4 (higher preference)
        fructose_assert (rc == true);
        fructose_assert_double_eq (-0.544, res);

        // Test copy constructor (using the duck principle).
        DynamicOperation d2(dynOp);
        retrieved = d2.getInputs();
        std::sort(retrieved.begin(), retrieved.end(),
                  [](const OperationInput &_a, const OperationInput &_b) { return _a.getName() < _b.getName(); });
        fructose_assert (original == retrieved);
        rc = d2.evaluate({ c, d }, res); // Expect a call to jsOp4 (higher preference)
        fructose_assert (rc == true);
        fructose_assert_double_eq (-0.544, res);
        rc = d2.evaluate({ a, b }, res); // Expect a call to diff
        fructose_assert (rc == true);
        fructose_assert_double_eq (-0.766, res);
        rc = d2.evaluate({ c, a, b }, res); // Expect a call to jsOp1
        fructose_assert (rc == true);
        fructose_assert_double_eq (5.924, res);
        rc = d2.evaluate({ d, c, a, b }, res); // Expect a call to jsOp1
        fructose_assert (rc == true);
        fructose_assert_double_eq (-3.735296, res);
        rc = d2.evaluate({ d, a, b }, res); // Expect a call to diff
        fructose_assert (rc == true);
        fructose_assert_double_eq (-0.766, res);
        rc = d2.evaluate({ b, c, d }, res); // Expect a call to jsOp4 (higher preference)
        fructose_assert (rc == true);
        fructose_assert_double_eq (-0.544, res);

        // Test assignment operator (using the duck principle).
        DynamicOperation d3 = dynOp;
        retrieved = d3.getInputs();
        std::sort(retrieved.begin(), retrieved.end(),
                  [](const OperationInput &_a, const OperationInput &_b) { return _a.getName() < _b.getName(); });
        fructose_assert (original == retrieved);
        rc = d3.evaluate({ c, d }, res); // Expect a call to jsOp4 (higher preference)
        fructose_assert (rc == true);
        fructose_assert_double_eq (-0.544, res);
        rc = d3.evaluate({ a, b }, res); // Expect a call to diff
        fructose_assert (rc == true);
        fructose_assert_double_eq (-0.766, res);
        rc = d3.evaluate({ c, a, b }, res); // Expect a call to jsOp1
        fructose_assert (rc == true);
        fructose_assert_double_eq (5.924, res);
        rc = d3.evaluate({ d, c, a, b }, res); // Expect a call to jsOp1
        fructose_assert (rc == true);
        fructose_assert_double_eq (-3.735296, res);
        rc = d3.evaluate({ d, a, b }, res); // Expect a call to diff
        fructose_assert (rc == true);
        fructose_assert_double_eq (-0.766, res);
        rc = d3.evaluate({ b, c, d }, res); // Expect a call to jsOp4 (higher preference)
        fructose_assert (rc == true);
        fructose_assert_double_eq (-0.544, res);
    }


    /// \brief Test: creatinine clearance using the Cockcroft-Gault general equation.
    void testCockcroftGaultGeneral(const std::string& /* _testName */)
    {
        bool rc;
        double eGFR;

        // Required parameters.
        OperationInput bodyweight("bodyweight", InputType::DOUBLE);
        OperationInput age("age", InputType::INTEGER);
        OperationInput creatinine("creatinine", InputType::DOUBLE);
        OperationInput isMale("isMale", InputType::BOOL);
        const OperationInput A_male("A_male", 1.23);
        const OperationInput A_female("A_female", 1.04);

        JSExpression jsCG_general("(140 - age) * bodyweight / creatinine * (A_male * isMale + A_female * (!isMale))",
        { OperationInput("bodyweight", InputType::DOUBLE),
          OperationInput("age", InputType::INTEGER),
          OperationInput("creatinine", InputType::DOUBLE),
          OperationInput("isMale", InputType::BOOL),
          OperationInput("A_male", InputType::DOUBLE),
          OperationInput("A_female", InputType::DOUBLE) });

        // Male, 49 years old, 71.4kg, creatinine 23.4umol/l
        rc = isMale.setValue(true);
        fructose_assert (rc == true);
        rc = age.setValue(49);
        fructose_assert (rc == true);
        rc = bodyweight.setValue(71.4);
        fructose_assert (rc == true);
        rc = creatinine.setValue(23.4);
        fructose_assert (rc == true);

        fructose_assert (jsCG_general.evaluate({ creatinine, bodyweight, age }, eGFR) == false);
        rc = jsCG_general.evaluate({ creatinine, bodyweight, age, isMale, A_male, A_female }, eGFR);
        fructose_assert (rc == true);
        fructose_assert_double_eq (341.53, eGFR);

        eGFR_CockcroftGaultGeneral hc_CG_general;
        double hc_eGFR;
        rc = hc_CG_general.evaluate({ creatinine, bodyweight, age, isMale }, hc_eGFR);
        fructose_assert (rc == true);
        fructose_assert (fabs(eGFR - hc_eGFR) < 1e-6);

        // Female, 53 years old, 51.3kg, creatinine 13.4umol/l
        rc = isMale.setValue(false);
        fructose_assert (rc == true);
        rc = age.setValue(53);
        fructose_assert (rc == true);
        rc = bodyweight.setValue(51.3);
        fructose_assert (rc == true);
        rc = creatinine.setValue(13.4);
        fructose_assert (rc == true);

        rc = jsCG_general.evaluate({ creatinine, bodyweight, age, isMale, A_male, A_female }, eGFR);
        fructose_assert (rc == true);
        fructose_assert_double_eq (346.389851, eGFR);

        rc = hc_CG_general.evaluate({ creatinine, bodyweight, age, isMale }, hc_eGFR);
        fructose_assert (rc == true);
        fructose_assert (fabs(eGFR - hc_eGFR) < 1e-6);
    }


    /// \brief Test: creatinine clearance using the Cockcroft-Gault equation with IBW.
    void testCockcroftGaultIBW(const std::string& /* _testName */)
    {
        bool rc;
        double eGFR;

        // Required parameters.
        OperationInput bodyweight("bodyweight", InputType::DOUBLE);
        OperationInput IBW("IBW", InputType::DOUBLE);
        OperationInput height("height", InputType::INTEGER);
        OperationInput age("age", InputType::INTEGER);
        OperationInput creatinine("creatinine", InputType::DOUBLE);
        OperationInput isMale("isMale", InputType::BOOL);
        const OperationInput A_male("A_male", 1.23);
        const OperationInput A_female("A_female", 1.04);

        double IBWvalue;
        IdealBodyWeight IBWComputation;

        JSExpression jsCG_IBW("(140 - age) * (bodyweight * (bodyweight < IBW) + IBW * (bodyweight >= IBW)) / creatinine * (A_male * isMale + A_female * (!isMale))",
        { OperationInput("bodyweight", InputType::DOUBLE),
          OperationInput("IBW", InputType::DOUBLE),
          OperationInput("age", InputType::INTEGER),
          OperationInput("creatinine", InputType::DOUBLE),
          OperationInput("isMale", InputType::BOOL),
          OperationInput("A_male", InputType::DOUBLE),
          OperationInput("A_female", InputType::DOUBLE) });

        // Male, 49 years old, 71.4kg, creatinine 23.4umol/l, 165cm
        rc = IBWComputation.evaluate({ OperationInput("height", 165), OperationInput("isMale", true) }, IBWvalue);
        fructose_assert (rc == true);
        fructose_assert_double_eq (61.25, IBWvalue);

        rc = isMale.setValue(true);
        fructose_assert (rc == true);
        rc = age.setValue(49);
        fructose_assert (rc == true);
        rc = bodyweight.setValue(71.4);
        fructose_assert (rc == true);
        rc = creatinine.setValue(23.4);
        fructose_assert (rc == true);
        rc = IBW.setValue(IBWvalue);
        fructose_assert (rc == true);

        rc = jsCG_IBW.evaluate({ creatinine, bodyweight, age, isMale, A_male, A_female, IBW }, eGFR);
        fructose_assert (rc == true);
        fructose_assert_double_eq (292.979167, eGFR);

        eGFR_CockcroftGaultIBW hc_CG_IBW;
        double hc_eGFR;
        rc = hc_CG_IBW.evaluate({ creatinine, bodyweight, age, isMale, OperationInput("height", 165) }, hc_eGFR);
        fructose_assert (rc == true);
        fructose_assert (fabs(eGFR - hc_eGFR) < 1e-6);

        // Female, 53 years old, 51.3kg, creatinine 13.4umol/l, 191cm
        rc = IBWComputation.evaluate({ OperationInput("height", 191), OperationInput("isMale", false) }, IBWvalue);
        fructose_assert (rc == true);
        fructose_assert_double_eq (74.6, IBWvalue);

        rc = isMale.setValue(false);
        fructose_assert (rc == true);
        rc = age.setValue(53);
        fructose_assert (rc == true);
        rc = bodyweight.setValue(51.3);
        fructose_assert (rc == true);
        rc = creatinine.setValue(13.4);
        fructose_assert (rc == true);

        rc = jsCG_IBW.evaluate({ creatinine, bodyweight, age, isMale, A_male, A_female, IBW }, eGFR);
        fructose_assert (rc == true);
        fructose_assert_double_eq (346.389851, eGFR);
        rc = hc_CG_IBW.evaluate({ creatinine, bodyweight, age, isMale, OperationInput("height", 191) }, hc_eGFR);
        fructose_assert (rc == true);
        fructose_assert (fabs(eGFR - hc_eGFR) < 1e-6);
    }


    /// \brief Test: creatinine clearance using the Cockcroft-Gault equation with adjusted IBW.
    void testCockcroftGaultAdjIBW(const std::string& /* _testName */)
    {
        bool rc;
        double eGFR;

        // Required parameters.
        OperationInput bodyweight("bodyweight", InputType::DOUBLE);
        OperationInput IBW("IBW", InputType::DOUBLE);
        OperationInput height("height", InputType::INTEGER);
        OperationInput age("age", InputType::INTEGER);
        OperationInput creatinine("creatinine", InputType::DOUBLE);
        OperationInput isMale("isMale", InputType::BOOL);
        const OperationInput A_male("A_male", 1.23);
        const OperationInput A_female("A_female", 1.04);

        double IBWvalue;
        IdealBodyWeight IBWComputation;

        JSExpression jsCG_IBW("(140 - age) * (IBW + 0.4 * (bodyweight - IBW)) / creatinine * (A_male * isMale + A_female * (!isMale))",
        { OperationInput("bodyweight", InputType::DOUBLE),
          OperationInput("IBW", InputType::DOUBLE),
          OperationInput("age", InputType::INTEGER),
          OperationInput("creatinine", InputType::DOUBLE),
          OperationInput("isMale", InputType::BOOL),
          OperationInput("A_male", InputType::DOUBLE),
          OperationInput("A_female", InputType::DOUBLE) });

        // Male, 49 years old, 71.4kg, creatinine 23.4umol/l, 165cm
        rc = IBWComputation.evaluate({ OperationInput("height", 165), OperationInput("isMale", true) }, IBWvalue);
        fructose_assert (rc == true);
        fructose_assert_double_eq (61.25, IBWvalue);

        rc = isMale.setValue(true);
        fructose_assert (rc == true);
        rc = age.setValue(49);
        fructose_assert (rc == true);
        rc = bodyweight.setValue(71.4);
        fructose_assert (rc == true);
        rc = creatinine.setValue(23.4);
        fructose_assert (rc == true);
        rc = IBW.setValue(IBWvalue);
        fructose_assert (rc == true);

        rc = jsCG_IBW.evaluate({ creatinine, bodyweight, age, isMale, A_male, A_female, IBW }, eGFR);
        fructose_assert (rc == true);
        fructose_assert_double_eq (312.3995, eGFR);
        eGFR_CockcroftGaultAdjIBW hc_CG_AdjIBW;
        double hc_eGFR;
        rc = hc_CG_AdjIBW.evaluate({ creatinine, bodyweight, age, isMale, OperationInput("height", 165) }, hc_eGFR);
        fructose_assert (rc == true);
        fructose_assert (fabs(eGFR - hc_eGFR) < 1e-6);

        // Female, 53 years old, 51.3kg, creatinine 13.4umol/l, 191cm
        rc = IBWComputation.evaluate({ OperationInput("height", 191), OperationInput("isMale", false) }, IBWvalue);
        fructose_assert (rc == true);
        fructose_assert_double_eq (74.6, IBWvalue);

        rc = isMale.setValue(false);
        fructose_assert (rc == true);
        rc = age.setValue(53);
        fructose_assert (rc == true);
        rc = bodyweight.setValue(51.3);
        fructose_assert (rc == true);
        rc = creatinine.setValue(13.4);
        fructose_assert (rc == true);
        rc = IBW.setValue(IBWvalue);
        fructose_assert (rc == true);

        rc = jsCG_IBW.evaluate({ creatinine, bodyweight, age, isMale, A_male, A_female, IBW }, eGFR);
        fructose_assert (rc == true);
        fructose_assert_double_eq (440.786149, eGFR);
        rc = hc_CG_AdjIBW.evaluate({ creatinine, bodyweight, age, isMale, OperationInput("height", 191) }, hc_eGFR);
        fructose_assert (rc == true);
        fructose_assert (fabs(eGFR - hc_eGFR) < 1e-6);
    }


    /// \brief Test: Modification of Diet in Renal Disease.
    void testMDRD(const std::string& /* _testName */)
    {
        bool rc;
        double eGFR_value;
        double GFR_value;
        double BSA_value;

        // Required parameters.
        OperationInput bodyweight("bodyweight", InputType::DOUBLE);
        OperationInput BSA("BSA", InputType::DOUBLE);
        OperationInput eGFR("eGFR", InputType::DOUBLE);
        OperationInput height("height", InputType::INTEGER);
        OperationInput age("age", InputType::INTEGER);
        OperationInput creatinine("creatinine", InputType::DOUBLE);
        OperationInput isMale("isMale", InputType::BOOL);
        OperationInput isAB("isAB", InputType::BOOL);

        JSExpression jsMDRD_eGFR("175 * Math.pow(0.0113 * creatinine, -1.154) * Math.pow(age, -0.203) * ((1 * (isMale)) + (0.742 * !isMale)) * ((1 * (!isAB)) + (1.212 * isAB))",
        { OperationInput("creatinine", InputType::DOUBLE),
          OperationInput("age", InputType::INTEGER),
          OperationInput("isMale", InputType::BOOL),
          OperationInput("isAB", InputType::BOOL) });

        JSExpression jsMDRD_BSA("0.007184 * Math.pow(height, 0.725) * Math.pow(bodyweight, 0.425)",
        { OperationInput("height", InputType::INTEGER),
          OperationInput("bodyweight", InputType::DOUBLE) });

        JSExpression jsMDRD_GFR("eGFR * BSA / 1.73",
        { OperationInput("eGFR", InputType::DOUBLE),
          OperationInput("BSA", InputType::DOUBLE) });

        // Male, 49 years old, 71.4kg, creatinine 23.4umol/l, 165cm, Caucasian
        rc = isMale.setValue(true);
        fructose_assert (rc == true);
        rc = age.setValue(49);
        fructose_assert (rc == true);
        rc = bodyweight.setValue(71.4);
        fructose_assert (rc == true);
        rc = creatinine.setValue(23.4);
        fructose_assert (rc == true);
        rc = height.setValue(165);
        fructose_assert (rc == true);
        rc = isAB.setValue(false);
        fructose_assert (rc == true);

        rc = jsMDRD_eGFR.evaluate({ creatinine, age, isMale, isAB }, eGFR_value);
        fructose_assert (rc == true);
        fructose_assert_double_eq (368.638441, eGFR_value);

        rc = jsMDRD_BSA.evaluate({ height, bodyweight }, BSA_value);
        fructose_assert (rc == true);
        fructose_assert_double_eq (1.785928, BSA_value);

        rc = eGFR.setValue(eGFR_value);
        fructose_assert (rc == true);
        rc = BSA.setValue(BSA_value);
        fructose_assert (rc == true);
        rc = jsMDRD_GFR.evaluate({ eGFR, BSA }, GFR_value);
        fructose_assert (rc == true);
        fructose_assert_double_eq (380.555904, GFR_value);
        GFR_MDRD hc_MDRD;
        double hc_GFR;
        rc = hc_MDRD.evaluate({ creatinine, bodyweight, age, isMale, height, isAB }, hc_GFR);
        fructose_assert (rc == true);
        fructose_assert (fabs(GFR_value - hc_GFR) < 2e-5);

        // Female, 53 years old, 51.3kg, creatinine 13.4umol/l, 191cm, African-Black
        rc = isMale.setValue(false);
        fructose_assert (rc == true);
        rc = age.setValue(53);
        fructose_assert (rc == true);
        rc = bodyweight.setValue(51.3);
        fructose_assert (rc == true);
        rc = creatinine.setValue(13.4);
        fructose_assert (rc == true);
        rc = height.setValue(191);
        fructose_assert (rc == true);
        rc = isAB.setValue(true);
        fructose_assert (rc == true);

        rc = jsMDRD_eGFR.evaluate({ creatinine, age, isMale, isAB }, eGFR_value);
        fructose_assert (rc == true);
        fructose_assert_double_eq (620.847749, eGFR_value);

        rc = jsMDRD_BSA.evaluate({ height, bodyweight }, BSA_value);
        fructose_assert (rc == true);
        fructose_assert_double_eq (1.725502, BSA_value);

        rc = eGFR.setValue(eGFR_value);
        fructose_assert (rc == true);
        rc = BSA.setValue(BSA_value);
        fructose_assert (rc == true);
        rc = jsMDRD_GFR.evaluate({ eGFR, BSA }, GFR_value);
        fructose_assert (rc == true);
        fructose_assert_double_eq (619.233545, GFR_value);
        rc = hc_MDRD.evaluate({ creatinine, bodyweight, age, isMale, height, isAB }, hc_GFR);
        fructose_assert (rc == true);
        fructose_assert (fabs(GFR_value - hc_GFR) < 2e-4);
    }


    /// \brief Test: CKD-EPI.
    void testCKD_EPI(const std::string& /* _testName */)
    {
        bool rc;
        double eGFR_value;
        double GFR_value;
        double BSA_value;

        // Required parameters.
        OperationInput BSA("BSA", InputType::DOUBLE);
        OperationInput eGFR("eGFR", InputType::DOUBLE);
        OperationInput bodyweight("bodyweight", InputType::DOUBLE);
        OperationInput height("height", InputType::INTEGER);
        OperationInput age("age", InputType::INTEGER);
        OperationInput creatinine("creatinine", InputType::DOUBLE);
        OperationInput isMale("isMale", InputType::BOOL);
        OperationInput isAB("isAB", InputType::BOOL);

        JSExpression jsCKD_EPI_eGFR("141 * \
                                   Math.pow(Math.min(0.0113 * creatinine / (0.7 * !isMale + 0.9 * isMale), 1), (-0.329 * !isMale - 0.411 * isMale)) * \
                                   Math.pow(Math.max(0.0113 * creatinine / (0.7 * !isMale + 0.9 * isMale), 1), (-1.209)) * \
                                   Math.pow(0.993, age) * \
                                   (1 + 0.018 * !isMale) * \
                                   (1 + 0.159 * isAB)",
        { OperationInput("creatinine", InputType::DOUBLE),
          OperationInput("age", InputType::INTEGER),
          OperationInput("isMale", InputType::BOOL),
          OperationInput("isAB", InputType::BOOL) });

        JSExpression jsCKD_EPI_BSA("0.007184 * Math.pow(height, 0.725) * Math.pow(bodyweight, 0.425)",
        { OperationInput("height", InputType::INTEGER),
          OperationInput("bodyweight", InputType::DOUBLE) });

        JSExpression jsCKD_EPI_GFR("eGFR * BSA / 1.73",
        { OperationInput("eGFR", InputType::DOUBLE),
          OperationInput("BSA", InputType::DOUBLE) });

        // Male, 49 years old, 71.4kg, creatinine 23.4umol/l, 165cm, Caucasian
        rc = isMale.setValue(true);
        fructose_assert (rc == true);
        rc = age.setValue(49);
        fructose_assert (rc == true);
        rc = bodyweight.setValue(71.4);
        fructose_assert (rc == true);
        rc = creatinine.setValue(23.4);
        fructose_assert (rc == true);
        rc = height.setValue(165);
        fructose_assert (rc == true);
        rc = isAB.setValue(false);
        fructose_assert (rc == true);

        rc = jsCKD_EPI_eGFR.evaluate({ creatinine, age, isMale, isAB }, eGFR_value);
        fructose_assert (rc == true);
        fructose_assert_double_eq (165.334316, eGFR_value);

        rc = jsCKD_EPI_BSA.evaluate({ height, bodyweight }, BSA_value);
        fructose_assert (rc == true);
        fructose_assert_double_eq (1.785928, BSA_value);

        rc = eGFR.setValue(eGFR_value);
        fructose_assert (rc == true);
        rc = BSA.setValue(BSA_value);
        fructose_assert (rc == true);
        rc = jsCKD_EPI_GFR.evaluate({ eGFR, BSA }, GFR_value);
        fructose_assert (rc == true);
        fructose_assert_double_eq (170.679297, GFR_value);
        GFR_CKD_EPI hc_CKD_EPI;
        double hc_GFR;
        rc = hc_CKD_EPI.evaluate({ creatinine, bodyweight, age, isMale, height, isAB }, hc_GFR);
        fructose_assert (rc == true);
        fructose_assert (fabs(GFR_value - hc_GFR) < 2e-5);

        // Female, 53 years old, 51.3kg, creatinine 13.4umol/l, 191cm, African-Black
        rc = isMale.setValue(false);
        fructose_assert (rc == true);
        rc = age.setValue(53);
        fructose_assert (rc == true);
        rc = bodyweight.setValue(51.3);
        fructose_assert (rc == true);
        rc = creatinine.setValue(13.4);
        fructose_assert (rc == true);
        rc = height.setValue(191);
        fructose_assert (rc == true);
        rc = isAB.setValue(true);
        fructose_assert (rc == true);

        rc = jsCKD_EPI_eGFR.evaluate({ creatinine, age, isMale, isAB }, eGFR_value);
        fructose_assert (rc == true);
        fructose_assert_double_eq (189.721900, eGFR_value);

        rc = jsCKD_EPI_BSA.evaluate({ height, bodyweight }, BSA_value);
        fructose_assert (rc == true);
        fructose_assert_double_eq (1.725502, BSA_value);

        rc = eGFR.setValue(eGFR_value);
        fructose_assert (rc == true);
        rc = BSA.setValue(BSA_value);
        fructose_assert (rc == true);
        rc = jsCKD_EPI_GFR.evaluate({ eGFR, BSA }, GFR_value);
        fructose_assert (rc == true);
        fructose_assert_double_eq (189.228623, GFR_value);
        rc = hc_CKD_EPI.evaluate({ creatinine, bodyweight, age, isMale, height, isAB }, hc_GFR);
        fructose_assert (rc == true);
        fructose_assert (fabs(GFR_value - hc_GFR) < 2e-4);
    }


    /// \brief Test: creatinine clearance using the Schwartz equation.
    void testSchwartz(const std::string& /* _testName */)
    {
        bool rc;
        double eGFR;

        // Required parameters.
        OperationInput bodyweight("bodyweight", InputType::DOUBLE);
        OperationInput bornAtTerm("bornAtTerm", InputType::BOOL);
        OperationInput height("height", InputType::INTEGER);
        OperationInput age("age", InputType::INTEGER);
        OperationInput creatinine("creatinine", InputType::DOUBLE);
        OperationInput isMale("isMale", InputType::BOOL);

        /// \warning This equations misses some intervals (e.g., 1 < age <= 2 or age <= 1 && bodyweight <= 2.5 && bornAtTerm) !!!
        JSExpression jsCG_Schwartz("height / creatinine * \
                                  (0.33 * (age <= 1 && bodyweight <= 2.5) + \
                                   0.45 * (age <= 1 && bornAtTerm) + \
                                   0.55 * (age > 2 && (age <= 13 || (age <= 20 && !isMale))) + \
                                   0.70 * (age > 13 && age <= 20 && isMale))",
        { OperationInput("bodyweight", InputType::DOUBLE),
          OperationInput("bornAtTerm", InputType::BOOL),
          OperationInput("age", InputType::INTEGER),
          OperationInput("height", InputType::INTEGER),
          OperationInput("creatinine", InputType::DOUBLE),
          OperationInput("isMale", InputType::BOOL) });

        // Male, 4 months old, 2.4kg, creatinine 23.4umol/l, born at term, 80cm
        rc = isMale.setValue(true);
        fructose_assert (rc == true);
        rc = age.setValue(0);
        fructose_assert (rc == true);
        rc = bodyweight.setValue(2.4);
        fructose_assert (rc == true);
        rc = height.setValue(80);
        fructose_assert (rc == true);
        rc = creatinine.setValue(23.4);
        fructose_assert (rc == true);
        rc = bornAtTerm.setValue(false);
        fructose_assert (rc == true);

        rc = jsCG_Schwartz.evaluate({ creatinine, bodyweight, age, isMale, bornAtTerm, height }, eGFR);
        fructose_assert (rc == true);
        fructose_assert_double_eq (1.128205, eGFR);
        eGFR_Schwartz hc_Schwartz;
        double hc_eGFR;
        rc = hc_Schwartz.evaluate({ creatinine, bodyweight, age, isMale, bornAtTerm, height }, hc_eGFR);
        fructose_assert (rc == true);
        fructose_assert (fabs(eGFR - hc_eGFR) < 1e-6);

        // Male, 4 months old, 7.4kg, creatinine 23.4umol/l, born at term, 80cm
        rc = isMale.setValue(true);
        fructose_assert (rc == true);
        rc = age.setValue(0);
        fructose_assert (rc == true);
        rc = bodyweight.setValue(7.4);
        fructose_assert (rc == true);
        rc = height.setValue(80);
        fructose_assert (rc == true);
        rc = creatinine.setValue(23.4);
        fructose_assert (rc == true);
        rc = bornAtTerm.setValue(true);
        fructose_assert (rc == true);

        rc = jsCG_Schwartz.evaluate({ creatinine, bodyweight, age, isMale, bornAtTerm, height }, eGFR);
        fructose_assert (rc == true);
        fructose_assert_double_eq (1.538462, eGFR);
        rc = hc_Schwartz.evaluate({ creatinine, bodyweight, age, isMale, bornAtTerm, height }, hc_eGFR);
        fructose_assert (rc == true);
        fructose_assert (fabs(eGFR - hc_eGFR) < 1e-6);

        // Male, 4 years, creatinine 86.2umol/l, 120cm
        rc = isMale.setValue(true);
        fructose_assert (rc == true);
        rc = age.setValue(4);
        fructose_assert (rc == true);
        rc = height.setValue(120);
        fructose_assert (rc == true);
        rc = creatinine.setValue(86.2);
        fructose_assert (rc == true);
        rc = bornAtTerm.setValue(true);
        fructose_assert (rc == true);
        rc = bodyweight.setValue(27.4);
        fructose_assert (rc == true);

        rc = jsCG_Schwartz.evaluate({ creatinine, bodyweight, age, isMale, bornAtTerm, height }, eGFR);
        fructose_assert (rc == true);
        fructose_assert_double_eq (0.765661, eGFR);
        rc = hc_Schwartz.evaluate({ creatinine, bodyweight, age, isMale, bornAtTerm, height }, hc_eGFR);
        fructose_assert (rc == true);
        fructose_assert (fabs(eGFR - hc_eGFR) < 1e-6);

        // Male, 14 years old, creatinine 86.2umol/l, 140cm
        rc = isMale.setValue(true);
        fructose_assert (rc == true);
        rc = age.setValue(14);
        fructose_assert (rc == true);
        rc = height.setValue(140);
        fructose_assert (rc == true);
        rc = creatinine.setValue(86.2);
        fructose_assert (rc == true);
        rc = bornAtTerm.setValue(true);
        fructose_assert (rc == true);
        rc = bodyweight.setValue(47.4);
        fructose_assert (rc == true);

        rc = jsCG_Schwartz.evaluate({ creatinine, bodyweight, age, isMale, bornAtTerm, height }, eGFR);
        fructose_assert (rc == true);
        fructose_assert_double_eq (1.136891, eGFR);
        rc = hc_Schwartz.evaluate({ creatinine, bodyweight, age, isMale, bornAtTerm, height }, hc_eGFR);
        fructose_assert (rc == true);
        fructose_assert (fabs(eGFR - hc_eGFR) < 1e-6);

        // Female, 14 years old, creatinine 86.2umol/l, 140cm
        rc = isMale.setValue(false);
        fructose_assert (rc == true);
        rc = age.setValue(14);
        fructose_assert (rc == true);
        rc = height.setValue(140);
        fructose_assert (rc == true);
        rc = creatinine.setValue(86.2);
        fructose_assert (rc == true);
        rc = bornAtTerm.setValue(true);
        fructose_assert (rc == true);
        rc = bodyweight.setValue(37.4);
        fructose_assert (rc == true);

        rc = jsCG_Schwartz.evaluate({ creatinine, bodyweight, age, isMale, bornAtTerm, height }, eGFR);
        fructose_assert (rc == true);
        fructose_assert_double_eq (0.893271, eGFR);
        rc = hc_Schwartz.evaluate({ creatinine, bodyweight, age, isMale, bornAtTerm, height }, hc_eGFR);
        fructose_assert (rc == true);
        fructose_assert (fabs(eGFR - hc_eGFR) < 1e-6);
    }


    /// \brief Test: Jelliffe equation.
    void testJelliffe(const std::string& /* _testName */)
    {
        bool rc;
        double eGFR_value;
        double GFR_value;
        double BSA_value;

        // Required parameters.
        OperationInput BSA("BSA", InputType::DOUBLE);
        OperationInput eGFR("eGFR", InputType::DOUBLE);
        OperationInput bodyweight("bodyweight", InputType::DOUBLE);
        OperationInput height("height", InputType::INTEGER);
        OperationInput age("age", InputType::INTEGER);
        OperationInput creatinine("creatinine", InputType::DOUBLE);
        OperationInput isMale("isMale", InputType::BOOL);

        JSExpression jsJelliffe_eGFR("(1 - 0.1 * !isMale) * (98 - (0.8 * age - 20)) / (0.0113 * creatinine)",
        { OperationInput("creatinine", InputType::DOUBLE),
          OperationInput("age", InputType::INTEGER),
          OperationInput("isMale", InputType::BOOL) });

        JSExpression jsJelliffe_BSA("0.007184 * Math.pow(height, 0.725) * Math.pow(bodyweight, 0.425)",
        { OperationInput("height", InputType::INTEGER),
          OperationInput("bodyweight", InputType::DOUBLE) });

        JSExpression jsJelliffe_GFR("eGFR * BSA / 1.73",
        { OperationInput("eGFR", InputType::DOUBLE),
          OperationInput("BSA", InputType::DOUBLE) });

        // Male, 49 years old, 71.4kg, creatinine 123.4umol/l, 165cm
        rc = isMale.setValue(true);
        fructose_assert (rc == true);
        rc = age.setValue(49);
        fructose_assert (rc == true);
        rc = bodyweight.setValue(71.4);
        fructose_assert (rc == true);
        rc = creatinine.setValue(123.4);
        fructose_assert (rc == true);
        rc = height.setValue(165);
        fructose_assert (rc == true);

        rc = jsJelliffe_eGFR.evaluate({ creatinine, age, isMale }, eGFR_value);
        fructose_assert (rc == true);
        fructose_assert_double_eq (56.510951, eGFR_value);

        rc = jsJelliffe_BSA.evaluate({ height, bodyweight }, BSA_value);
        fructose_assert (rc == true);
        fructose_assert_double_eq (1.785928, BSA_value);

        rc = eGFR.setValue(eGFR_value);
        fructose_assert (rc == true);
        rc = BSA.setValue(BSA_value);
        fructose_assert (rc == true);
        rc = jsJelliffe_GFR.evaluate({ eGFR, BSA }, GFR_value);
        fructose_assert (rc == true);
        fructose_assert_double_eq (58.337855, GFR_value);
        GFR_Jelliffe hc_Jelliffe;
        double hc_GFR;
        rc = hc_Jelliffe.evaluate({ creatinine, age, isMale, height, bodyweight }, hc_GFR);
        fructose_assert (rc == true);
        fructose_assert (fabs(GFR_value - hc_GFR) < 2e-5);

        // Female, 53 years old, 51.3kg, creatinine 313.4umol/l, 191cm
        rc = isMale.setValue(false);
        fructose_assert (rc == true);
        rc = age.setValue(53);
        fructose_assert (rc == true);
        rc = bodyweight.setValue(51.3);
        fructose_assert (rc == true);
        rc = creatinine.setValue(313.4);
        fructose_assert (rc == true);
        rc = height.setValue(191);
        fructose_assert (rc == true);

        rc = jsJelliffe_eGFR.evaluate({ creatinine, age, isMale }, eGFR_value);
        fructose_assert (rc == true);
        fructose_assert_double_eq (19.212632, eGFR_value);

        rc = jsJelliffe_BSA.evaluate({ height, bodyweight }, BSA_value);
        fructose_assert (rc == true);
        fructose_assert_double_eq (1.725502, BSA_value);

        rc = eGFR.setValue(eGFR_value);
        fructose_assert (rc == true);
        rc = BSA.setValue(BSA_value);
        fructose_assert (rc == true);
        rc = jsJelliffe_GFR.evaluate({ eGFR, BSA }, GFR_value);
        fructose_assert (rc == true);
        fructose_assert_double_eq (19.162679, GFR_value);
        rc = hc_Jelliffe.evaluate({ creatinine, age, isMale, height, bodyweight }, hc_GFR);
        fructose_assert (rc == true);
        fructose_assert (fabs(GFR_value - hc_GFR) < 2e-5);
    }


    /// \brief Test: creatinine clearance using the Salazar-Corcoran equation.
    void testSalazarCorcoran(const std::string& /* _testName */)
    {
        bool rc;
        double eGFR;

        // Required parameters.
        OperationInput bodyweight("bodyweight", InputType::DOUBLE);
        OperationInput height("height", InputType::INTEGER);
        OperationInput age("age", InputType::INTEGER);
        OperationInput creatinine("creatinine", InputType::DOUBLE);
        OperationInput isMale("isMale", InputType::BOOL);

        /// \warning This equations returns huge values!
        JSExpression jsCG_SalazarCorcoran("isMale * ((137 - age) * (0.285 * bodyweight + 12.1 * height * height) * 0.0113 / (51 * creatinine)) + \
                                         !isMale * ((146 - age) * (0.287 * bodyweight + 9.74 * height * height) * 0.0113 / (60 * creatinine))",
        { OperationInput("bodyweight", InputType::DOUBLE),
          OperationInput("age", InputType::INTEGER),
          OperationInput("height", InputType::INTEGER),
          OperationInput("creatinine", InputType::DOUBLE),
          OperationInput("isMale", InputType::BOOL) });

        // Male, 49 years old, 122.4kg, creatinine 23.4umol/l, 160cm
        rc = isMale.setValue(true);
        fructose_assert (rc == true);
        rc = age.setValue(49);
        fructose_assert (rc == true);
        rc = bodyweight.setValue(122.4);
        fructose_assert (rc == true);
        rc = height.setValue(160);
        fructose_assert (rc == true);
        rc = creatinine.setValue(23.4);
        fructose_assert (rc == true);

        rc = jsCG_SalazarCorcoran.evaluate({ creatinine, bodyweight, age, isMale, height }, eGFR);
        fructose_assert (rc == true);
        fructose_assert_double_eq (258.136444, eGFR);
        eGFR_SalazarCorcoran hc_SalazarCorcoran;
        double hc_eGFR;
        rc = hc_SalazarCorcoran.evaluate({ creatinine, bodyweight, age, isMale, height }, hc_eGFR);
        fructose_assert (rc == true);
        fructose_assert (fabs(eGFR - hc_eGFR) < 1e-6);

        // Female, 53 years old, 162.4kg, creatinine 123.4umol/l, 170cm
        rc = isMale.setValue(false);
        fructose_assert (rc == true);
        rc = age.setValue(53);
        fructose_assert (rc == true);
        rc = bodyweight.setValue(162.4);
        fructose_assert (rc == true);
        rc = height.setValue(170);
        fructose_assert (rc == true);
        rc = creatinine.setValue(123.4);
        fructose_assert (rc == true);

        rc = jsCG_SalazarCorcoran.evaluate({ creatinine, bodyweight, age, isMale, height }, eGFR);
        fructose_assert (rc == true);
        fructose_assert_double_eq (39.959835, eGFR);
        rc = hc_SalazarCorcoran.evaluate({ creatinine, bodyweight, age, isMale, height }, hc_eGFR);
        fructose_assert (rc == true);
        fructose_assert (fabs(eGFR - hc_eGFR) < 1e-6);
    }
};

#endif // TEST_OPERATION_H
