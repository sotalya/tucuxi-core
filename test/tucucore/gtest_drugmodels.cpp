//@@license@@

#include <gtest/gtest.h>

#include "tucucore/drugmodel/drugmodel.h"

#include "drugmodels/buildimatinib.h"

using namespace Tucuxi::Core;

static std::unique_ptr<DrugModel> buildImatinib()
{
    BuildImatinib builder;
    return builder.buildDrugModel();
}

TEST (Core_TestDrugModels, Imatinib){
    auto imatinib = buildImatinib();

    ASSERT_TRUE(imatinib != nullptr);
}
