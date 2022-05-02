//@@license@@s

#ifndef TEST_DRUGMODELS_H
#define TEST_DRUGMODELS_H


#include "tucucore/drugmodel/drugmodel.h"

#include "drugmodels/buildimatinib.h"
#include "fructose/fructose.h"

using namespace Tucuxi::Core;



struct TestDrugModels : public fructose::test_base<TestDrugModels>
{
    TestDrugModels() {}

    std::unique_ptr<DrugModel> buildImatinib()
    {
        BuildImatinib builder;
        return builder.buildDrugModel();
    }

    void testImatinib(const std::string& /* _testName */)
    {
        auto imatinib = buildImatinib();

        fructose_assert(imatinib != nullptr);
    }
};

#endif // TEST_DRUGMODELS_H
