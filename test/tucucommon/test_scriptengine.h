/*
* Copyright (C) 2017 Tucuxi SA
*/

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
        int value;
        engine.getVariable("a", value);
        fructose_assert(value == 68);

        engine.evaluate("b = a-10");
        engine.getVariable("b", value);
        fructose_assert(value == 58); 

        engine.setVariable("c", "12");
        engine.evaluate("d = b+c");
        engine.getVariable("d", value);
        fructose_assert(value == 70);

        engine.evaluate("e = Math.pow(3, 2)");
        double dblValue;
        engine.getVariable("e", dblValue);
        fructose_assert(dblValue == 9.0);

        engine.evaluate("str = 'Tcho les' + ' topiots'");
        std::string strValue;
        engine.getVariable("str", strValue);
        fructose_assert(strValue == "Tcho les topiots");

        engine.evaluate("f = 2 == 3");
        bool bValue;
        engine.getVariable("f", bValue);
        fructose_assert(!bValue);

        engine.evaluate("f = 3 == 3");
        engine.getVariable("f", bValue);
        fructose_assert(bValue);
    }
 };
