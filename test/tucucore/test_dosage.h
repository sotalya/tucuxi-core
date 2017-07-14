/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TEST_DOSAGE_H
#define TEST_DOSAGE_H

#include "fructose/fructose.h"

#include "tucucore/dosage.h"

struct TestDosage : public fructose::test_base<TestDosage>
{

    TestDosage()
    {
    }

    void basic(const std::string& _testName)
    {
        std::cout << _testName << std::endl;
        Tucuxi::Core::Dosage *dosage;
        dosage = new Tucuxi::Core::SingleDose();
        std::cout << "Dosage test succeeds" << std::endl;
    }

 };

#endif // TEST_DOSAGE_H
