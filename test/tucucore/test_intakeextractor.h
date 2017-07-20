/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TEST_INTAKEEXTRACTOR_H
#define TEST_INTAKEEXTRACTOR_H


#include "fructose/fructose.h"

#include "tucucore/intakeextractor.h"

struct TestIntakeExtractor : public fructose::test_base<TestIntakeExtractor>
{

    TestIntakeExtractor()
    {
    }

    void basic(const std::string& _testName)
    {
        std::cout << _testName << std::endl;
        Tucuxi::Core::Dosage *dosage;
//        dosage = new Tucuxi::Core::LastingDose();
        std::cout << "Dosage test succeeds" << std::endl;
    }

    ///
    /// \brief weekWithoutWeekEnd
    /// \param _testName
    /// Scenarios where a treatment is given every working day, but not on the
    /// week-end.
    void weekWithoutWeekEnd(const std::string& _testName)
    {

    }

    ///
    /// \brief weekWithWeekEnd
    /// \param _testName
    /// Scenarios where a treatment is given every working day with a specific
    /// pattern, and with another one during the week-end
    void weekWithWeekEnd(const std::string& _testName)
    {

    }



 };

#endif // TEST_INTAKEEXTRACTOR_H
