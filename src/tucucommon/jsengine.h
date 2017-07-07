/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_TUCUCOMMON_JSENGINE_H
#define TUCUXI_TUCUCOMMON_JSENGINE_H

#include <string>
#include <memory>

class CTinyJS;

namespace Tucuxi {
namespace Common {

class JSEngine
{
public:
    JSEngine();
    
    bool getVariable(const std::string& _name, std::string& _value);

    bool setVariable(const std::string& _name, const std::string& _expression);
    
    bool evaluate(const std::string& _code);

    void reset();

private:
    CTinyJS* m_pEngine;
};

}
}

#endif // TUCUXI_TUCUCOMMON_JSENGINE_H