#ifndef TUCUXI_OPERATION_H
#define TUCUXI_OPERATION_H

#include <iostream>
#include <vector>

#include "tucucore/definitions.h"

#include "tucucommon/jsengine.h"

using Tucuxi::Common::JSEngine;

namespace Tucuxi {
namespace Core {

/// \ingroup TucuCore
/// \brief Input operand to an operation.
class OperationInput
{
public:
    /// \brief Create an operand from its name and type.
    /// \param _name Name of the operand.
    /// \param _type Type of the operand.
    OperationInput(const std::string &_name, const InputType &_type = InputType::DOUBLE);

    /// \brief Return the type of the input
    /// \return Type of the input.
    InputType getType() const;

    /// \brief Return whether the input value is defined or not.
    /// \return true if the value is defined, false otherwise.
    bool isDefined() const;

    /// \brief Get the boolean value stored in the operand (if present and the stored type is indeed a boolean).
    /// \return true if the value can be successfully retrieved, false otherwise.
    /// \post if (isDefined() && m_type == InputType::BOOL) { _value == m_value && [RETURN] == true }
    ///       else { [RETURN] == false };
    bool getValue(bool &_value) const;

    /// \brief Set the value of a boolean input (exploiting function overloading).
    /// \param _value Boolean value to set.
    /// \return true if the value could be set, false otherwise (type mismatch).
    /// \post if (m_type == InputType::BOOL) { m_value == _value && [RETURN] == true }
    ///       else { m_value == PREV(m_value) && [RETURN] == false };
    bool setValue(const bool &_value);

    /// \brief Get the integer value stored in the operand (if present and the stored type is indeed an integer).
    /// \return true if the value can be successfully retrieved, false otherwise.
    /// \post if (isDefined() && m_type == InputType::INTEGER) { _value == m_value && [RETURN] == true }
    ///       else { [RETURN] == false };
    bool getValue(int &_value) const;

    /// \brief Set the value of an integer input (exploiting function overloading).
    /// \param _value Integer value to set.
    /// \return true if the value could be set, false otherwise (type mismatch).
    /// \post if (m_type == InputType::INTEGER) { m_value == _value && [RETURN] == true }
    ///       else { m_value == PREV(m_value) && [RETURN] == false };
    bool setValue(const int &_value);

    /// \brief Get the floating point value stored in the operand (if present and the stored type is indeed a floating point).
    /// \return true if the value can be successfully retrieved, false otherwise.
    /// \post if (isDefined() && m_type == InputType::DOUBLE) { _value == m_value && [RETURN] == true }
    ///       else { [RETURN] == false };
    bool getValue(double &_value) const;

    /// \brief Set the value of a floating point input (exploiting function overloading).
    /// \param _value Floating point value to set.
    /// \return true if the value could be set, false otherwise (type mismatch).
    /// \post if (m_type == InputType::DOUBLE) { m_value == _value && [RETURN] == true }
    ///       else { m_value == PREV(m_value) && [RETURN] == false };
    bool setValue(const double &_value);


private:
    /// \brief Mark the input as defined or undefined.
    bool m_isDefined;
    /// \brief Name of the input.
    std::string m_name;
    /// \brief Type of the input.
    InputType m_type;
    /// \brief Store the value in the most appropriate type.
    union m_value {
        double d;
        bool b;
        int i;
    };
};


/// \brief List of operation's inputs.
typedef std::vector<OperationInput> OperationInputList;


/// \ingroup TucuCore
/// \brief Abstract class representing a generic operation in Tucuxi.
class Operation
{
public:
    /// \brief Create an operation without specifying the required inputs (this has to be done later manually).
    Operation();

    /// \brief Create an operation already setting the list of required inputs.
    /// \param _requiredInputs List of required inputs.
    Operation(const OperationInputList &_requiredInputs);

    /// \brief Add a required input to the list of inputs required by the operation.
    /// \param _requiredInput Input to add to the list.
    void addRequiredInput(const OperationInput &_requiredInput);

    /// \brief Check if the given input list satisfies operation's requirements.
    /// \param _inputList List of inputs that have to be checked.
    /// \post if ((FOREACH(input: _inputs, required: m_requiredInputs) { input.isDefined() && input.getType() == required.getType() }) { [RETURN] == true }
    ///       else { [RETURN] == false };
    virtual bool check(const OperationInputList &_inputs) const;

    /// \brief Evaluate the operation on the given inputs.
    /// \param _inputList List of inputs that have to be used by the operation.
    /// \param _result Result of the operation.
    /// \return true if the operation could be performed, false otherwise.
    /// \post if (check(_inputs)) { _result == [OPERATION_RESULT] && [RETURN] == true }
    ///       else { [RETURN] == false };
    virtual bool evaluate(const OperationInputList &_inputs, double &_result) const = 0;

    /// \brief Return the list of required input operands.
    /// This list can be filled by the caller to have all the values ready for evaluation.
    /// \return Vector containing a list of the operands required for the operation.
    virtual OperationInputList getInputs() const;


protected:
    /// \brief List of required inputs.
    OperationInputList m_requiredInputs;
};


/// \brief List of operations, along with the preference level.
typedef std::vector<std::pair<Operation, unsigned int>> OperationList;


/// \ingroup TucuCore
/// \brief Base class for all hardcoded operations.
/// An hardcoded operation is an operation that has an hardcoded evaluate method.
class HardcodedOperation : public Operation
{
public:
    /// \brief Evaluate the operation on the given inputs.
    /// \param _inputList List of inputs that have to be used by the operation.
    /// \param _result Result of the operation.
    /// \return true if the operation could be performed, false otherwise.
    /// \post if (check(_inputs)) { _result == [OPERATION_RESULT] && [RETURN] == true }
    ///       else { [RETURN] == false };
    virtual bool evaluate(const OperationInputList &_inputs, double &_result) const;
};


/// \ingroup TucuCore
/// \brief Special operation that dynamically selects, from a list of operations, the one to perform based on the available inputs.
class DynamicOperation : public Operation
{
public:
    /// \brief Add an operation to the list of operations to choose from.
    bool addOperation(const Operation &_operation, const unsigned int _preferenceLevel);

    /// \brief Check if the given input list satisfies the requirements of at least one of the operations stored.
    /// \param _inputList List of inputs that have to be checked.
    /// \post if (ANY(operation: m_operations) { operation.check(_inputs) } { [RETURN] == true }
    ///       else { [RETURN] == false };
    virtual bool check(const OperationInputList &_inputs) const;

    /// \brief Evaluate an operation on the given inputs, choosing the one that suits best.
    /// The parameter match is taken as a first match measure, using the preference level as a second discrimination
    /// criterion.
    /// \warning If multiple operations have the same input set, the function returns the result of the first operation
    ///          in the list accepting those parameters (for a given preference level).
    /// \param _inputList List of inputs that have to be used by the operation.
    /// \param _result Result of the operation.
    /// \return true if the operation could be performed, false otherwise.
    /// \post bestMatch = { operation IN m_operations | operation.check(_inputs == true) &&
    ///                                                 PREFERENCE(operation) <= { PREFERENCE({op2 IN m_operations | op2.check(_inputs == true) && op2 != operation })} };
    ///       if (EXISTS(bestMatch)) { _result == [RESULT(bestMatch)] && [RETURN] == true }
    ///       else { [RETURN] == false };
    virtual bool evaluate(const OperationInputList &_inputs, double &_result) const;

    /// \brief Return the list of *possibly* required input operands.
    /// This list contains *ALL* the operands that could be needed by *ALL* the stored operations -- only a subset might
    /// be needed for an operation to be performed. *NO* duplicates are present.
    /// \return Vector containing a list of the operands required by the whole set of stored operations.
    virtual OperationInputList getInputs() const;


protected:
    /// \brief List of operations among which to choose the one to perform.
    OperationList m_operations;
};


/// \ingroup TucuCore
/// \brief Operation that uses the JavaScript engine for computation.
class JSOperation : public Operation
{
public:
    /// \brief Build a JS operation from an expression and a set of required inputs.
    /// \param _expression Expression to evaluate.
    /// \param _requiredInputs List of required inputs.
    JSOperation(const std::string &_expression, const OperationInputList &_requiredInputs);

    /// \brief Evaluate the operation on the given inputs using the JSEngine.
    /// \param _inputList List of inputs that have to be used by the operation.
    /// \param _result Result of the operation.
    /// \return true if the operation could be performed, false otherwise.
    /// \post if (check(_inputs) && m_jsEngine.evaluate(m_expression) == true) { _result == [OPERATION_RESULT] && [RETURN] == true }
    ///       else { [RETURN] == false };
    virtual bool evaluate(const OperationInputList &_inputs, double &_result) const;
    /// \todo To retrieve the result, an additional variable has to be introduced in the expression!


protected:
    /// \brief JavaScript expression representing the operation to perform.
    std::string m_expression;
    /// \brief JavaScript engine.
    JSEngine m_jsEngine;
};

}
}

#endif // TUCUXI_OPERATION_H
