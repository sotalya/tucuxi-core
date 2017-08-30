/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TEST_OPERABLEGRAPHMANAGER_H
#define TEST_OPERABLEGRAPHMANAGER_H

#include <iostream>
#include <memory>

#include "fructose/fructose.h"

#include "tucucore/operablegraphmanager.h"

using namespace Tucuxi::Core;


class TestOperable : public Operable
{

};


struct TestOperableGraphManager : public fructose::test_base<TestOperableGraphManager>
{
    TestOperableGraphManager() { }

    void testGraph(const std::string& _testName)
    {
        UNUSED_PARAMETER(_testName);
    }
};

#endif // TEST_OPERABLEGRAPHMANAGER_H
