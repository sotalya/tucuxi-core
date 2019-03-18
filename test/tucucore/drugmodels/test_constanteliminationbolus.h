/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TEST_CONSTANTELIMINATIONBOLUS_H
#define TEST_CONSTANTELIMINATIONBOLUS_H

#include <chrono>
#include <iostream>
#include <memory>

#include <date/date.h>

#include "fructose/fructose.h"

#include "tucucore/drugmodelimport.h"
#include "buildconstantelimination.h"

using namespace Tucuxi::Core;


struct TestConstantEliminationBolus : public fructose::test_base<TestConstantEliminationBolus>
{
    TestConstantEliminationBolus() { }

    /// \brief Check that objects are correctly constructed by the constructor.
    void testConstantElimination(const std::string& /* _testName */)
    {

        BuildConstantElimination builder;
        DrugModel *drugModel = builder.buildDrugModel();

        fructose_assert(drugModel != nullptr);

        fructose_assert(drugModel->checkInvariants());

    }
};



#endif // TEST_CONSTANTELIMINATIONBOLUS_H
