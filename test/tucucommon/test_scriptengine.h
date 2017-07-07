
#include <sstream>
#include <string>
#include <iostream>
#include <stdio.h>
#include <cstdio>
#include <regex>

#include "fructose/fructose.h"

#include "tucucommon/jsengine.h"

struct TestScriptEngine : public fructose::test_base<TestScriptEngine>
{
    void basic(const std::string& _testName)
    {
        std::cout << _testName << std::endl;

        Tucuxi::Common::JSEngine engine;
        engine.evaluate("a = 34*2");
        std::string value;
        engine.getVariable("a", value);
        fructose_assert(value == "68");

        engine.evaluate("b = a-10");
        engine.getVariable("b", value);
        fructose_assert(value == "58"); 

        engine.setVariable("c", "12");
        engine.evaluate("d = b+c");
        engine.getVariable("d", value);
        fructose_assert(value == "70");
    }
 };
