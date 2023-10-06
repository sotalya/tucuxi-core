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


#include "jsengine.h"

#include "TinyJS.h"
#include "TinyJS_Functions.h"
#include "TinyJS_MathFunctions.h"
#include "utils.h"

namespace Tucuxi {
namespace Common {


JSEngine::JSEngine()
{
    reset();
}

JSEngine::~JSEngine() = default;


template<>
bool JSEngine::getVariable<std::string>(const std::string& _name, std::string& _value)
{
    if (m_pEngine != nullptr) {
        CScriptVar* pVar = m_pEngine->getScriptVariable(_name);
        if (pVar != nullptr) {
            _value = pVar->getString();
            return true;
        }
    }
    return false;
}

template<>
bool JSEngine::getVariable<int>(const std::string& _name, int& _value)
{
    std::string value;
    if (getVariable(_name, value)) {
        try {
            _value = std::stoi(value);
            return true;
        }
        catch (...) {
        }
    }
    return false;
}

template<>
bool JSEngine::getVariable<double>(const std::string& _name, double& _value)
{
    std::string value;
    if (getVariable(_name, value)) {
        try {
            _value = std::stod(value);
            return true;
        }
        catch (...) {
        }
    }
    return false;
}


template<>
bool JSEngine::getVariable<bool>(const std::string& _name, bool& _value)
{
    std::string value;
    if (getVariable(_name, value)) {
        _value = (value == "1");
    }
    return false;
}


bool JSEngine::setVariable(const std::string& _name, const std::string& _expression)
{
    if (m_pEngine != nullptr) {
        m_pEngine->evaluate(Tucuxi::Common::Utils::strFormat("%s=%s", _name.c_str(), _expression.c_str()));
        return true;
    }
    return false;
}


bool JSEngine::evaluate(const std::string& _code)
{
    if (m_pEngine != nullptr) {
        m_pEngine->execute(_code);
        return true;
    }
    return false;
}


void JSEngine::reset()
{
    m_pEngine = std::make_unique<CTinyJS>();
    registerFunctions(m_pEngine.get());
    registerMathFunctions(m_pEngine.get());
}

} // namespace Common
} // namespace Tucuxi
