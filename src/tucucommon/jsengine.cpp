/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "TinyJS.h"

#include "jsengine.h"
#include "utils.h"

namespace Tucuxi {
namespace Common {


JSEngine::JSEngine() 
    : m_pEngine(nullptr)
{    
    reset();   
}


bool JSEngine::getVariable(const std::string& _name, std::string& _value)
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
        m_pEngine->evaluate(_code);
        return true;
    }
    return false;
}


void JSEngine::reset()
{
    if (m_pEngine != nullptr) {
        delete m_pEngine;
    }
    m_pEngine = new CTinyJS();
}

}
}


