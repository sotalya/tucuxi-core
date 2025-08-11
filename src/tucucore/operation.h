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


#ifndef TUCUXI_OPERATION_H
#define TUCUXI_OPERATION_H

#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include "tucucommon/jsengine.h"

#include "tucucore/definitions.h"

using Tucuxi::Common::JSEngine; // NOLINT(google-global-names-in-headers)

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
    OperationInput(std::string _name, const InputType& _type = InputType::DOUBLE);

    /// \brief Create an operand from its name and value.
    /// \param _name Name of the operand.
    /// \param _value Numerical value of the input.
    OperationInput(std::string _name, const bool& _value);

    /// \brief Create an operand from its name and value.
    /// \param _name Name of the operand.
    /// \param _value Numerical value of the input.
    OperationInput(std::string _name, const int& _value);

    /// \brief Create an operand from its name and value.
    /// \param _name Name of the operand.
    /// \param _value Numerical value of the input.
    OperationInput(std::string _name, const double& _value);

    /// \brief Default copy-construct an operation input.
    /// \param _other OperationInput used for copy-construction.
    OperationInput(const OperationInput& _other) = default;

    /// \brief Default destructor.
    virtual ~OperationInput() = default;

    /// \brief Default operator assignment function.
    /// \param _rhs Source operation input.
    OperationInput& operator=(const OperationInput& _rhs) = default;

    /// \brief Comparison (equality) operator.
    /// param _rhs OperationInput to compare to.
    bool operator==(const OperationInput& _rhs) const;

    /// \brief Comparison (difference) operator.
    /// param _rhs OperationInput to compare to.
    bool operator!=(const OperationInput& _rhs) const;

    /// \brief Return whether the input value is defined or not.
    /// \return true if the value is defined, false otherwise.
    bool isDefined() const;

    /// \brief Return the name of the input
    /// \return Name of the input.
    std::string getName() const;

    /// \brief Return the type of the input
    /// \return Type of the input.
    InputType getType() const;

    /// \brief Get the boolean value stored in the operand (if present and the stored type is indeed a boolean).
    /// \return true if the value can be successfully retrieved, false otherwise.
    /// \post if (isDefined() && m_type == InputType::BOOL) { _value == m_value && [RETURN] == true }
    ///       else { [RETURN] == false };
    bool getValue(bool& _value) const;

    /// \brief Set the value of a boolean input (exploiting function overloading).
    /// \param _value Boolean value to set.
    /// \return true if the value could be set, false otherwise (type mismatch).
    /// \post if (m_type == InputType::BOOL) { m_value == _value && [RETURN] == true }
    ///       else { m_value == PREV(m_value) && [RETURN] == false };
    bool setValue(const bool& _value);

    /// \brief Get the integer value stored in the operand (if present and the stored type is indeed an integer).
    /// \return true if the value can be successfully retrieved, false otherwise.
    /// \post if (isDefined() && m_type == InputType::INTEGER) { _value == m_value && [RETURN] == true }
    ///       else { [RETURN] == false };
    bool getValue(int& _value) const;

    /// \brief Set the value of an integer input (exploiting function overloading).
    /// \param _value Integer value to set.
    /// \return true if the value could be set, false otherwise (type mismatch).
    /// \post if (m_type == InputType::INTEGER) { m_value == _value && [RETURN] == true }
    ///       else { m_value == PREV(m_value) && [RETURN] == false };
    bool setValue(const int& _value);

    /// \brief Get the floating point value stored in the operand (if present and the stored type is indeed a floating point).
    /// \return true if the value can be successfully retrieved, false otherwise.
    /// \post if (isDefined() && m_type == InputType::DOUBLE) { _value == m_value && [RETURN] == true }
    ///       else { [RETURN] == false };
    bool getValue(double& _value) const;

    /// \brief Set the value of a floating point input (exploiting function overloading).
    /// \param _value Floating point value to set.
    /// \return true if the value could be set, false otherwise (type mismatch).
    /// \post if (m_type == InputType::DOUBLE) { m_value == _value && [RETURN] == true }
    ///       else { m_value == PREV(m_value) && [RETURN] == false };
    bool setValue(const double& _value);


private:
    /// \brief Name of the input.
    std::string m_name;
    /// \brief Type of the input.
    InputType m_type;
    /// \brief Mark the input as defined or undefined.
    bool m_isDefined;
    /// \brief Store the value in the most appropriate type.
    std::variant<double, bool, int> m_value;
};


/// \brief List of operation's inputs.
typedef std::vector<OperationInput> OperationInputList;

/// \brief Iteration pointing to an operation's input.
typedef std::vector<OperationInput>::const_iterator OperationInputIt;

/// \brief Helper function to check if an input is present in a list of inputs and its value is defined.
/// \param _inputs Input list to scan.
/// \param _inputName Sought input name.
/// \param _type Sought input type.
/// \return true if the sought input is present, false otherwise.
bool checkInputIsDefined(const OperationInputList& _inputs, const std::string& _inputName, const InputType& _type);

/// \brief Helper function to check if an input is present in a list of inputs.
/// \param _inputs Input list to scan.
/// \param _inputName Sought input name.
/// \param _type Sought input type.
/// \return true if the sought input is present, false otherwise.
bool checkInputIsPresent(const OperationInputList& _inputs, const std::string& _inputName, const InputType& _type);

/// \brief Helper function to check if an input is present in a list of inputs, considering the name alone.
/// \param _inputs Input list to scan.
/// \param _inputName Sought input name.
/// \return true if the sought input is present, false otherwise.
OperationInputIt findInputInList(const OperationInputList& _inputs, const std::string& _inputName);

/// \brief Helper function to check if an input is present in a list of inputs.
/// \param _inputs Input list to scan.
/// \param _inputName Sought input name.
/// \param _type Sought input type.
/// \return true if the sought input is present, false otherwise.
OperationInputIt findInputInList(
        const OperationInputList& _inputs, const std::string& _inputName, const InputType& _type);

/// \brief Helper function to ease input retrieval from an input list.
/// \param _inputs Input list to scan.
/// \param _inputName Sought input name.
/// \param _value Retrieved value
/// \return true if the value can be successfully retrieved, false otherwise.
/// \post if (inputIsDefined() && inputType == InputType::BOOLEAN) { _value == inputValue && [RETURN] == true }
///       else { [RETURN] == false };
bool getInputValue(const OperationInputList& _inputs, const std::string& _inputName, bool& _value);

/// \brief Helper function to ease input retrieval from an input list.
/// \param _inputs Input list to scan.
/// \param _inputName Sought input name.
/// \param _value Retrieved value
/// \return true if the value can be successfully retrieved, false otherwise.
/// \post if (inputIsDefined() && inputType == InputType::INTEGER) { _value == inputValue && [RETURN] == true }
///       else { [RETURN] == false };
bool getInputValue(const OperationInputList& _inputs, const std::string& _inputName, int& _value);

/// \brief Helper function to ease input retrieval from an input list.
/// \param _inputs Input list to scan.
/// \param _inputName Sought input name.
/// \param _value Retrieved value
/// \return true if the value can be successfully retrieved, false otherwise.
/// \post if (inputIsDefined() && inputType == InputType::DOUBLE) { _value == inputValue && [RETURN] == true }
///       else { [RETURN] == false };
bool getInputValue(const OperationInputList& _inputs, const std::string& _inputName, double& _value);


/// \ingroup TucuCore
/// \brief Abstract class representing a generic operation in Tucuxi.
class Operation
{
public:
    /// \brief Create an operation without specifying the required inputs (this has to be done later manually).
    Operation() = default;

    /// \brief Create an operation already setting the list of required inputs.
    /// \param _requiredInputs List of required inputs.
    Operation(OperationInputList _requiredInputs);

    /// \brief Default destructor.
    virtual ~Operation() = default;

    /// \brief Return a pointer to a clone of the correct subclass.
    /// \return Pointer to a new object of subclass' type.
    virtual std::unique_ptr<Operation> clone() const = 0;

    /// \brief Default operator assignment function.
    /// \param _rhs Source operation.
    Operation& operator=(const Operation& _rhs) = default;

    /// \brief Check if the given input list satisfies operation's requirements.
    /// \param _inputList List of inputs that have to be checked.
    /// \post if ((FOREACH(input: _inputs, required: m_requiredInputs) { input.isDefined() && input.getType() == required.getType() }) { [RETURN] == true }
    ///       else { [RETURN] == false };
    virtual bool check(const OperationInputList& _inputs) const;

    /// \brief Evaluate the operation on the given inputs.
    /// \warning No control on types is performed -- you can for instance divide a boolean by a double without the
    ///          system raising a warning.
    /// \param _inputList List of inputs that have to be used by the operation.
    /// \param _result Result of the operation.
    /// \return true if the operation could be performed, false otherwise.
    /// \post if (check(_inputs)) { _result == [OPERATION_RESULT] && [RETURN] == true }
    ///       else { [RETURN] == false };
    virtual bool evaluate(const OperationInputList& _inputs, double& _result) = 0;

    /// \brief Return the list of required input operands.
    /// This list can be filled by the caller to have all the values ready for evaluation.
    /// \return Vector containing a list of the operands required for the operation.
    virtual OperationInputList getInputs() const;

    /// \brief Returns the last error message
    /// \return The last error message
    /// This function can be used when an evaluate() function returns false.
    std::string getLastErrorMessage() const;

protected:
    /// \brief List of required inputs.
    OperationInputList m_requiredInputs;

    static std::string sm_errorMessage; // NOLINT(readability-identifier-naming)
};


/// \brief List of operations, along with the preference level.
typedef std::vector<std::pair<std::unique_ptr<Operation>, unsigned int>> OperationList;


/// \ingroup TucuCore
/// \brief Base class for all hardcoded operations.
/// An hardcoded operation is an operation that has an hardcoded evaluate method.
/// \note Hardcoded operations do not have a way to add required inputs dynamically, since all the operations are known
///       at compile time -- and therefore we expect the developer of the class to fill the list of requirements at
///       development time.
class HardcodedOperation : public Operation
{
public:
    /// \brief Initialize the flag marking required inputs as not yet entered.
    HardcodedOperation();

    /// \brief Default destructor.
    ~HardcodedOperation() override = default;

    /// \brief Clone function returning a pointer to the base class.
    /// \return Pointer to the base Operation class.
    std::unique_ptr<Operation> clone() const override = 0;

    /// \brief Evaluate the operation on the given inputs.
    /// \warning No control on types is performed -- you can for instance divide a boolean by a double without the
    ///          system raising a warning.
    /// \param _inputList List of inputs that have to be used by the operation.
    /// \param _result Result of the operation.
    /// \return true if the operation could be performed, false otherwise.
    /// \post if (check(_inputs) == true) { _result == [OPERATION_RESULT] && [RETURN] == true }
    ///       else { [RETURN] == false };
    bool evaluate(const OperationInputList& _inputs, double& _result) final;


protected:
    /// \brief Perform the desired computation on the given inputs.
    /// \param _inputs List of inputs that have to be used by the operation.
    /// \param _result Result of the operation.
    /// \return true if the operation could be performed, false otherwise.
    /// \pre check(_inputs) == true
    virtual bool compute(const OperationInputList& _inputs, double& _result) const = 0;

    /// \brief Force the user of the class to implement a function filling the vector of required inputs.
    virtual void fillRequiredInputs() = 0;

    /// \brief Flag marking required inputs as not yet entered.
    bool m_filledInputs;
};


/// \ingroup TucuCore
/// \brief Operation that uses the JavaScript engine for computation.
/// It to express complex computation and shall end up with a
/// return statement.
class JSOperation : public Operation
{
public:
    /// \brief Build a JS operation from an expression and a set of required inputs.
    /// \param _expression Expression to evaluate.
    /// \param _requiredInputs List of required inputs.
    JSOperation(const std::string& _expression, const OperationInputList& _requiredInputs);

    /// \brief Clone function returning a pointer to the base class.
    /// \return Pointer to the base Operation class.
    std::unique_ptr<Operation> clone() const override;

    /// \brief Evaluate the operation on the given inputs using the JSEngine.
    /// \warning No control on types is performed -- you can for instance divide a boolean by a double without the
    ///          system raising a warning.
    /// \param _inputList List of inputs that have to be used by the operation.
    /// \param _result Result of the operation.
    /// \return true if the operation could be performed, false otherwise.
    /// \post if (check(_inputs) && m_jsEngine.evaluate(m_expression) == true) { _result == [OPERATION_RESULT] && [RETURN] == true }
    ///       else { [RETURN] == false };
    bool evaluate(const OperationInputList& _inputs, double& _result) override;

    /// \brief Checks the operation on the given inputs using the JSEngine.
    /// \warning No control on types is performed -- you can for instance divide a boolean by a double without the
    ///          system raising a warning.
    /// \param _inputList List of inputs that have to be used by the operation.
    /// \return true if the operation could be performed, false otherwise.
    /// \post if (check(_inputs) && m_jsEngine.evaluate(m_expression) == true) { [RETURN] == true }
    ///       else { [RETURN] == false };
    /// The check slightly modify the operation, removing :
    /// "function calc() {\n" at the beggining,
    /// "\n}\n result = calc();" at the end,
    /// and also removing the last "return" statement
    ///
    bool checkOperation(const OperationInputList& _inputs);

    /// \brief Get the expression as a string
    /// \return The expression
    /// This function is meant to be used by the DrugModelChecker, not for any other evaluation
    std::string getExpression()
    {
        return m_expression;
    }

protected:
    /// \brief JavaScript expression representing the operation to perform.
    std::string m_expression;

    /// \brief Build a JS operation from a set of required inputs.
    /// \param _requiredInputs List of required inputs.
    /// Only used as super class constructor for JSExpression
    JSOperation(const OperationInputList& _requiredInputs);
};


/// \ingroup TucuCore
/// \brief Expression that uses the JavaScript engine for computation.
/// While JSOperation allow to express complex computation and shall end up with a
/// return statement, a JSExpression is only a simple expression, such as "a+b".
/// It shall not contain a return statement.
class JSExpression : public JSOperation
{
public:
    /// \brief Build a JS expression from an expression and a set of required inputs.
    /// \param _expression Expression to evaluate.
    /// \param _requiredInputs List of required inputs.
    JSExpression(const std::string& _expression, const OperationInputList& _requiredInputs);
};


/// \ingroup TucuCore
/// \brief Special operation that dynamically selects, from a list of operations, the one to perform based on the available inputs.
class DynamicOperation : public Operation
{
public:
    /// \brief Default Constructor.
    DynamicOperation() = default;

    /// \brief Copy-construct a dynamic operation.
    /// \param _other Operation used for copy-construction.
    DynamicOperation(const DynamicOperation& _other);

    /// \brief Default destructor.
    ~DynamicOperation() override = default;

    /// \brief Clone function returning a pointer to the base class.
    /// \return Pointer to the base Operation class.
    std::unique_ptr<Operation> clone() const override;

    /// \brief Default operator assignment function.
    /// \param _rhs Source operation.
    DynamicOperation& operator=(const DynamicOperation& _rhs) = default;

    /// \brief Add an operation to the list of operations to choose from.
    /// \param _operation Operation to add.
    /// \param _preferenceLevel Preference level of the operation (if another one with the same number of matching
    ///                         inputs is available)
    /// \return true if the operation was successfully added to the list, false otherwise.
    bool addOperation(const Operation& _operation, unsigned int _preferenceLevel);

    /// \brief Check if the given input list satisfies the requirements of at least one of the operations stored.
    /// \param _inputList List of inputs that have to be checked.
    /// \return true if the input list satisfies the requirements of at least one of the operations stored, false
    ///         otherwise.
    /// \post if (ANY(operation: m_operations) { operation.check(_inputs) } { [RETURN] == true }
    ///       else { [RETURN] == false };
    bool check(const OperationInputList& _inputs) const override;

    /// \brief Evaluate an operation on the given inputs, choosing the one that suits best.
    /// The parameter match is taken as a first match measure, using the preference level as a second discrimination
    /// criterion.
    /// \warning No control on types is performed -- you can for instance divide a boolean by a double without the
    ///          system raising a warning.
    /// \warning If multiple operations have the same input set, the function returns the result of the first operation
    ///          in the list accepting those parameters (for a given preference level).
    /// \param _inputList List of inputs that have to be used by the operation.
    /// \param _result Result of the operation.
    /// \return true if the operation could be performed, false otherwise.
    /// \post bestMatch = { operation IN m_operations | operation.check(_inputs == true) &&
    ///                                                 PREFERENCE(operation) <= { PREFERENCE({op2 IN m_operations | op2.check(_inputs == true) && op2 != operation })} };
    ///       if (EXISTS(bestMatch)) { _result == [RESULT(bestMatch)] && [RETURN] == true }
    ///       else { [RETURN] == false };
    bool evaluate(const OperationInputList& _inputs, double& _result) override;

    /// \brief Return the list of *possibly* required input operands.
    /// This list contains *ALL* the operands that could be needed by *ALL* the stored operations -- only a subset might
    /// be needed for an operation to be performed. *NO* duplicates are present.
    /// \return Vector containing a list of the operands required by the whole set of stored operations.
    OperationInputList getInputs() const override;


protected:
    /// \brief List of operations among which to choose the one to perform.
    OperationList m_operations;
};


} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_OPERATION_H
