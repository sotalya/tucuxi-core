//@@license@@

#include <string>

#include <gtest/gtest.h>

#include "tucucommon/jsengine.h"


TEST(Common_TestScriptEngine, Basic)
{
    Tucuxi::Common::JSEngine engine;
    engine.evaluate("a = 34*2;");
    int value;
    engine.getVariable("a", value);
    ASSERT_EQ(value, 68);

    engine.evaluate("b = a-10;");
    engine.getVariable("b", value);
    ASSERT_EQ(value, 58);

    engine.setVariable("c", "12");
    engine.evaluate("d = b+c;");
    engine.getVariable("d", value);
    ASSERT_EQ(value, 70);

    engine.evaluate("e = Math.pow(3, 2);");
    double dblValue;
    engine.getVariable("e", dblValue);
    ASSERT_DOUBLE_EQ(dblValue, 9.0);

    engine.evaluate("str = 'Tcho les' + ' topiots';");
    std::string strValue;
    engine.getVariable("str", strValue);
    ASSERT_EQ(strValue, "Tcho les topiots");

    engine.evaluate("f = 2 == 3;");
    bool bValue;
    engine.getVariable("f", bValue);
    ASSERT_FALSE(bValue);

    engine.evaluate("f = 3 == 3;");
    engine.getVariable("f", bValue);
    ASSERT_TRUE(bValue);
}
