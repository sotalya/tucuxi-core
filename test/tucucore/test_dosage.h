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
        std::unique_ptr<Tucuxi::Core::Dosage> dosage(Tucuxi::Core::LastingDose());
        std::cout << "Basic dosage test succeeds" << std::endl;
        fructose_assert(1 == 1);
    }


    void advanced(const std::string& _testName)
    {
        std::cout << _testName << std::endl;
        std::unique_ptr<Tucuxi::Core::Dosage> dosage(Tucuxi::Core::LastingDose());
        std::cout << "Advanced dosage test fails" << std::endl;
        fructose_assert(0 == 1);
    }

};

#endif // TEST_DOSAGE_H
