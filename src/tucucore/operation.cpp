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


#include "tucucore/operation.h"

#include "tucucommon/general.h"
#include "tucucommon/loggerhelper.h"

#include "TinyJS.h"

namespace Tucuxi {
namespace Core {

OperationInput::OperationInput(const std::string& _name, const InputType& _type)
    : m_name{_name}, m_type{_type}, m_isDefined{false}
{
}


OperationInput::OperationInput(const std::string& _name, const bool& _value)
    : m_name{_name}, m_type{InputType::BOOL}, m_isDefined{true}
{
    m_value.b = _value;
}


OperationInput::OperationInput(const std::string& _name, const int& _value)
    : m_name{_name}, m_type{InputType::INTEGER}, m_isDefined{true}
{
    m_value.i = _value;
}


OperationInput::OperationInput(const std::string& _name, const double& _value)
    : m_name{_name}, m_type{InputType::DOUBLE}, m_isDefined{true}
{
    m_value.d = _value;
}


bool OperationInput::operator==(const OperationInput& _rhs) const
{
    return (this->m_name == _rhs.m_name && this->m_isDefined == _rhs.m_isDefined
            && (!this->m_isDefined
                || ((this->m_type == InputType::BOOL && this->m_value.b == _rhs.m_value.b)
                    || (this->m_type == InputType::INTEGER && this->m_value.i == _rhs.m_value.i)
                    || (this->m_type == InputType::DOUBLE
                        && fabs(this->m_value.d - _rhs.m_value.d) < std::numeric_limits<double>::min())))
            && this->m_type == _rhs.m_type);
}


bool OperationInput::operator!=(const OperationInput& _rhs) const
{
    return !(*this == _rhs);
}


bool OperationInput::isDefined() const
{
    return m_isDefined;
}


std::string OperationInput::getName() const
{
    return m_name;
}


InputType OperationInput::getType() const
{
    return m_type;
}


bool OperationInput::getValue(bool& _value) const
{
    if (m_type == InputType::BOOL && m_isDefined) {
        _value = m_value.b;
        return true;
    }
    return false;
}


bool OperationInput::setValue(const bool& _value)
{
    if (m_type == InputType::BOOL) {
        m_value.b = _value;
        m_isDefined = true;
        return true;
    }
    return false;
}


bool OperationInput::getValue(int& _value) const
{
    if (m_type == InputType::INTEGER && m_isDefined) {
        _value = m_value.i;
        return true;
    }
    return false;
}


bool OperationInput::setValue(const int& _value)
{
    if (m_type == InputType::INTEGER) {
        m_value.i = _value;
        m_isDefined = true;
        return true;
    }
    if (m_type == InputType::BOOL) {
        m_value.b = _value != 0;
        m_isDefined = true;
        return true;
    }
    return false;
}


bool OperationInput::getValue(double& _value) const
{
    if (m_type == InputType::DOUBLE && m_isDefined) {
        _value = m_value.d;
        return true;
    }
    return false;
}


bool OperationInput::setValue(const double& _value)
{
    if (m_type == InputType::DOUBLE) {
        m_value.d = _value;
        m_isDefined = true;
        return true;
    }
    if (m_type == InputType::INTEGER) {
        m_value.i = static_cast<int>(_value);
        m_isDefined = true;
        return true;
    }
    if (m_type == InputType::BOOL) {
        m_value.b = static_cast<int>(_value) != 0;
        m_isDefined = true;
        return true;
    }
    return false;
}

std::string Operation::sm_errorMessage; // NOLINT(readability-identifier-naming)

std::string Operation::getLastErrorMessage() const
{
    return sm_errorMessage;
}

Operation::Operation(const OperationInputList& _requiredInputs) : m_requiredInputs{_requiredInputs} {}


bool Operation::check(const OperationInputList& _inputs) const
{
    if (_inputs.size() < m_requiredInputs.size()) {
        // Early-stop if the number of given inputs is below the required one
        return false;
    }
    // Reject inputs with multiple definitions (perform the check on a copy to avoid side effects).
    OperationInputList givenInputs = _inputs;
    std::sort(givenInputs.begin(), givenInputs.end(), [](const OperationInput& _a, const OperationInput& _b) {
        return _a.getName() < _b.getName();
    });
    if (std::adjacent_find(
                givenInputs.begin(),
                givenInputs.end(),
                [](const OperationInput& _a, const OperationInput& _b) { return _a.getName() == _b.getName(); })
        != givenInputs.end()) {
        return false;
    }

    // Check that all the inputs are there and they are valid:
    // For each required input, scan the list for a valid input with the same name and type.
    for (const auto& reqIn : m_requiredInputs) {
        if (!checkInputIsDefined(_inputs, reqIn.getName(), reqIn.getType())) {
            return false;
        }
    }
    return true;
}


OperationInputList Operation::getInputs() const
{
    return m_requiredInputs;
}


bool checkInputIsDefined(const OperationInputList& _inputs, const std::string& _inputName, const InputType& _type)
{
    OperationInputIt it = findInputInList(_inputs, _inputName, _type);
    if (it != _inputs.end()) {
        return it->isDefined();
    }
    return false;
}


bool checkInputIsPresent(const OperationInputList& _inputs, const std::string& _inputName, const InputType& _type)
{
    return findInputInList(_inputs, _inputName, _type) != _inputs.end();
}


OperationInputIt findInputInList(const OperationInputList& _inputs, const std::string& _inputName)
{
    OperationInputIt it =
            std::find_if(_inputs.begin(), _inputs.end(), [&_inputName](const OperationInput& _in) -> bool {
                return _inputName == _in.getName();
            });
    return it;
}


OperationInputIt findInputInList(
        const OperationInputList& _inputs, const std::string& _inputName, const InputType& _type)
{
    OperationInputIt it =
            std::find_if(_inputs.begin(), _inputs.end(), [&_inputName, &_type](const OperationInput& _in) -> bool {
                return _inputName == _in.getName() &&
                       // Check if the types are similar
                       //
                       // The following has to be checked to see if we tolerate such integer-double equivalency
                       ((_type == _in.getType()) ||
                        // Or double compatible with integer
                        //(_type == InputType::DOUBLE && _in.getType() == InputType::INTEGER) ||
                        // Or integer compatible with double
                        (_type == InputType::INTEGER && _in.getType() == InputType::DOUBLE));
            });
    return it;
}


bool getInputValue(const OperationInputList& _inputs, const std::string& _inputName, bool& _value)
{
    OperationInputIt it = findInputInList(_inputs, _inputName, InputType::BOOL);
    if (it != _inputs.end()) {
        return it->getValue(_value);
    }
    return false;
}


bool getInputValue(const OperationInputList& _inputs, const std::string& _inputName, int& _value)
{
    OperationInputIt it = findInputInList(_inputs, _inputName, InputType::INTEGER);
    if (it != _inputs.end()) {
        return it->getValue(_value);
    }
    return false;
}


bool getInputValue(const OperationInputList& _inputs, const std::string& _inputName, double& _value)
{
    OperationInputIt it = findInputInList(_inputs, _inputName, InputType::DOUBLE);
    if (it != _inputs.end()) {
        return it->getValue(_value);
    }
    return false;
}


HardcodedOperation::HardcodedOperation() : m_filledInputs{false} {}


bool HardcodedOperation::evaluate(const OperationInputList& _inputs, double& _result)
{
    if (!m_filledInputs) {
        fillRequiredInputs();
        m_filledInputs = true;
    }
    if (!check(_inputs)) {
        return false;
    }
    return compute(_inputs, _result);
}


JSOperation::JSOperation(const std::string& _expression, const OperationInputList& _requiredInputs)
    : Operation(_requiredInputs)
{
    // Put the code in a function, and call the function to store the result
    // of the evaluation in the 'result' variable
    m_expression = "function calc() {\n" + _expression + "\n}\n result = calc();";
}

JSOperation::JSOperation(const OperationInputList& _requiredInputs) : Operation(_requiredInputs) {}

JSExpression::JSExpression(const std::string& _expression, const OperationInputList& _requiredInputs)
    : JSOperation(_requiredInputs)
{
    // Append the prefix that will store the result of the evaluation in the 'result' variable
    m_expression = "result = " + _expression + ";";
}


std::unique_ptr<Operation> JSOperation::clone() const
{
    return std::make_unique<JSOperation>(*this);
}


// Add the variable according to the given data type
#define ADD_VAR_CASE(CASE_VAR, DATA_TYPE)                             \
    case CASE_VAR: {                                                  \
        DATA_TYPE value;                                              \
        if (inVar.getValue(value) == false) {                         \
            return false;                                             \
        }                                                             \
        jsEngine.setVariable(inVar.getName(), std::to_string(value)); \
    } break;


bool JSOperation::evaluate(const OperationInputList& _inputs, double& _result)
{
    /// \warning The JS engine does not return an error if variables are missing -- it will silently assume that they
    ///          are zeroes and happily perform the computation. This could go horribly bad if no precautions are taken,
    ///          therefore we validate hereby the inputs to ensure that everything is in order.
    if (!check(_inputs)) {
        //Tucuxi::Common::LoggerHelper logger;
        //logger.error("Missing inputs for a JSOperation : {}", m_expression);
        return false;
    }


    // The next lines could generate an exception.
    // If the jsEngine.evaluate() goes wrong, we intercept the exception and
    // simply return false.
    // Also the cast of result can go wrong.
    try {

        JSEngine jsEngine;
        // Push the inputs

        for (const auto& inVar : _inputs) {
            switch (inVar.getType()) {
                ADD_VAR_CASE(InputType::BOOL, bool);
                ADD_VAR_CASE(InputType::INTEGER, int);
                ADD_VAR_CASE(InputType::DOUBLE, double);
            default:
                return false;
            }
        }

        if (!jsEngine.evaluate(m_expression)) {
            // Tucuxi::Common::LoggerHelper logger;
            // logger.error("Could not evaluate the JSOperation : {}", m_expression);
            return false;
        }

        std::string resAsString;

        if (!jsEngine.getVariable("result", resAsString)) {
            //Tucuxi::Common::LoggerHelper logger;
            //logger.error("Could not get the result of the JSOperation : {}", m_expression);
            return false;
        }

        _result = std::stod(resAsString);
    }
    catch (const CScriptException* e) {
        sm_errorMessage = e->text;
        delete e;
        //        Tucuxi::Common::LoggerHelper logger;
        //        logger.error("Error with the execution of the JSOperation : {}\n\n{}", m_expression, e->text);
        return false;
    }
    catch (...) {
        //        Tucuxi::Common::LoggerHelper logger;
        //        logger.error("Error with the execution of the JSOperation : {}", m_expression);
        return false;
    }
    return true;
}

bool JSOperation::checkOperation(const OperationInputList& _inputs)
{

    /// \warning The JS engine does not return an error if variables are missing -- it will silently assume that they
    ///          are zeroes and happily perform the computation. This could go horribly bad if no precautions are taken,
    ///          therefore we validate hereby the inputs to ensure that everything is in order.
    if (!check(_inputs)) {
        //Tucuxi::Common::LoggerHelper logger;
        //logger.error("Missing inputs for a JSOperation : {}", m_expression);
        return false;
    }


    // The next lines could generate an exception.
    // If the jsEngine.evaluate() goes wrong, we intercept the exception and
    // simply return false.
    // Also the cast of result can go wrong.
    try {

        JSEngine jsEngine;
        // Push the inputs

        for (const auto& inVar : _inputs) {
            switch (inVar.getType()) {
                ADD_VAR_CASE(InputType::BOOL, bool);
                ADD_VAR_CASE(InputType::INTEGER, int);
                ADD_VAR_CASE(InputType::DOUBLE, double);
            default:
                return false;
            }
        }

        // We get back to the initial expression
        //        m_expression = "function calc() {\n" + _expression + "\n}\n result = calc();";
        std::string exp = m_expression.substr(18);
        exp = exp.substr(0, exp.size() - 20);
        size_t pos = exp.rfind("return ");
        if (pos != std::string::npos) {
            exp = exp.substr(0, pos);
        }
        else {
            sm_errorMessage = "Missing a return statement at the end of the script.";
            return false;
        }

        if (!jsEngine.evaluate(exp)) {
            // Tucuxi::Common::LoggerHelper logger;
            // logger.error("Could not evaluate the JSOperation : {}", m_expression);
            return false;
        }

        std::string resAsString;
        /*
        if(!jsEngine.getVariable("result", resAsString)) {
            //Tucuxi::Common::LoggerHelper logger;
            //logger.error("Could not get the result of the JSOperation : {}", m_expression);
            return false;
        }
        _result = std::stod(resAsString);
*/
    }
    catch (const CScriptException* e) {
        sm_errorMessage = e->text;
        delete e;
        //        Tucuxi::Common::LoggerHelper logger;
        //        logger.error("Error with the execution of the JSOperation : {}\n\n{}", m_expression, e->text);
        return false;
    }
    catch (...) {
        //        Tucuxi::Common::LoggerHelper logger;
        //        logger.error("Error with the execution of the JSOperation : {}", m_expression);
        return false;
    }


    try {

        JSEngine jsEngine;
        // Push the inputs

        for (const auto& inVar : _inputs) {
            switch (inVar.getType()) {
                ADD_VAR_CASE(InputType::BOOL, bool);
                ADD_VAR_CASE(InputType::INTEGER, int);
                ADD_VAR_CASE(InputType::DOUBLE, double);
            default:
                return false;
            }
        }

        // We get back to the initial expression
        //        m_expression = "function calc() {\n" + _expression + "\n}\n result = calc();";
        std::string exp = m_expression;

        if (!jsEngine.evaluate(exp)) {
            // Tucuxi::Common::LoggerHelper logger;
            // logger.error("Could not evaluate the JSOperation : {}", m_expression);
            return false;
        }

        std::string resAsString;
        /*
        if(!jsEngine.getVariable("result", resAsString)) {
            //Tucuxi::Common::LoggerHelper logger;
            //logger.error("Could not get the result of the JSOperation : {}", m_expression);
            return false;
        }
        _result = std::stod(resAsString);
*/
    }
    catch (const CScriptException* e) {
        sm_errorMessage = e->text;
        delete e;
        //        Tucuxi::Common::LoggerHelper logger;
        //        logger.error("Error with the execution of the JSOperation : {}\n\n{}", m_expression, e->text);
        return false;
    }
    catch (...) {
        //        Tucuxi::Common::LoggerHelper logger;
        //        logger.error("Error with the execution of the JSOperation : {}", m_expression);
        return false;
    }


    return true;
}


DynamicOperation::DynamicOperation(const DynamicOperation& _other) : Operation(_other)
{
    for (auto&& op : _other.m_operations) {
        m_operations.push_back(std::make_pair(op.first->clone(), op.second));
    }
}


std::unique_ptr<Operation> DynamicOperation::clone() const
{
    return std::make_unique<DynamicOperation>(*this);
}


bool DynamicOperation::addOperation(const Operation& _operation, unsigned int _preferenceLevel)
{
    // Check that inputs do not conflict with exiting ones (that is, same name but different type).
    OperationInputList alreadyPresentInputs = getInputs();
    OperationInputList newInputs = _operation.getInputs();

    for (const auto& newIn : newInputs) {
        OperationInputIt it = findInputInList(alreadyPresentInputs, newIn.getName());
        if (it != alreadyPresentInputs.end()) {
            if (it->getType() != newIn.getType()) {
                return false;
            }
        }
    }

    m_operations.push_back(std::make_pair(_operation.clone(), _preferenceLevel));
    return true;
}


bool DynamicOperation::check(const OperationInputList& _inputs) const
{
    for (auto&& op : m_operations) {
        if (op.first->check(_inputs)) {
            return true;
        }
    }
    return false;
}


bool DynamicOperation::evaluate(const OperationInputList& _inputs, double& _result)
{
    // Initialize values that will be used in the search for the best matching operation
    int idxBest = -1;
    int nInputsBest = -1;
    int prefLevelBest = -1;

    // Find the 'best' operation to perform
    for (size_t i = 0; i < m_operations.size(); ++i) {
        // Check that the given operation can run with the inputs passed as parameter
        if (m_operations[i].first->check(_inputs)) {
            const int nInputs = static_cast<int>(m_operations[i].first->getInputs().size());
            const int prefLevel = static_cast<int>(m_operations[i].second);

            if (nInputs > nInputsBest || (nInputs == nInputsBest && prefLevel < prefLevelBest)) {
                // We have found an operation with either more matching inputs or the same number but with higher
                // preferability
                idxBest = static_cast<int>(i);
                nInputsBest = nInputs;
                prefLevelBest = prefLevel;
            }
        }
    }
    if (idxBest < 0) {
        // Could not find a suitable operation
        return false;
    }

    return m_operations[static_cast<size_t>(idxBest)].first->evaluate(_inputs, _result);
}


OperationInputList DynamicOperation::getInputs() const
{
    OperationInputList ret;
    for (auto&& op : m_operations) {
        OperationInputList tmp = op.first->getInputs();
        for (const auto& input : tmp) {
            // Push missing inputs, skipping duplicates
            if (!checkInputIsPresent(ret, input.getName(), input.getType())) {
                ret.push_back(input);
            }
        }
    }
    return ret;
}


} // namespace Core
} // namespace Tucuxi
