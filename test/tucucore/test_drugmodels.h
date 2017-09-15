/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TEST_DRUGMODELS_H
#define TEST_DRUGMODELS_H


#include "fructose/fructose.h"

#include "tucucore/drugmodel/drugmodel.h"
#include "drugmodels/buildimatinib.h"

using namespace Tucuxi::Core;



struct TestDrugModels : public fructose::test_base<TestDrugModels>
{
    TestDrugModels() { }

    DrugModel *buildImatinib()
    {
        BuildImatinib builder;
        return builder.buildDrugModel();
    }

    void testImatinib(const std::string& /* _testName */)
    {
        DrugModel* imatinib;
        imatinib = buildImatinib();

        fructose_assert(imatinib != nullptr);
    }

};

#endif // TEST_DRUGMODELS_H
