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
