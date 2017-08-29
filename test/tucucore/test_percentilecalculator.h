/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TEST_PERCENTILECALCULATOR_H
#define TEST_PERCENTILECALCULATOR_H

#include <iostream>
#include <memory>

#include "fructose/fructose.h"

#include "tucucommon/general.h"

#include "tucucore/dosage.h"
#include "tucucore/intakeextractor.h"
#include "tucucore/intakeintervalcalculator.h"
#include "tucucore/concentrationcalculator.h"
#include "tucucore/onecompartmentbolus.h"
#include "tucucore/onecompartmentinfusion.h"
#include "tucucore/onecompartmentextra.h"

struct TestPercentileCalculator : public fructose::test_base<TestPercentileCalculator>
{

    TestPercentileCalculator() { }

    void test1(const std::string& /* _testName */)
    {

    }


};

#endif // TEST_PERCENTILECALCULATOR_H
