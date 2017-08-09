/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TEST_OPERATION_H
#define TEST_OPERATION_H

#include <iostream>
#include <memory>

#include "fructose/fructose.h"

#include "tucucore/operation.h"

using namespace Tucuxi::Core;

/// \brief Example implementation of an hardcoded 'a - b' operation.
/// In the example, to further test the implementation, 'a' is a double while 'b' is an integer.
class DiffOperation : public HardcodedOperation
{
    /// \brief Implementation of the inherited clone operation
    /// \return Pointer to a new object of subclass' type.
    std::unique_ptr<Operation> clone() const
    {
        return std::unique_ptr<Operation>(new DiffOperation(*this));
    }


    /// \brief Perform the desired computation (difference) on the inputs.
    /// \param _inputs List of inputs that have to be used by the operation.
    /// \param _result Result of the operation.
    /// \return true if the operation could be performed (that is, inputs can be successfully retrieved), false
    ///         otherwise.
    /// \pre check(_inputs) == true
    bool compute(const OperationInputList &_inputs, double &_result) const
    {
        double a;
        int b;

        if (!getInputValue(inputs, "a", a) || !getInputValue(inputs, "b", b)) {
            return false;
        }

        _result = a - b;

        return true;
    }


    /// \brief Fill the vector of required inputs.
    void fillRequiredInputs()
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
        // fructose_assert(ptr->getFirstIntakeInterval(dtAfter) == dtAfter_Correct);
    }

};

#endif // TEST_OPERATION_H
