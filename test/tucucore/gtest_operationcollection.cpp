//@@license@@

#include <memory>

#include <gtest/gtest.h>

#include "tucucore/hardcodedoperation.h"
#include "tucucore/operation.h"

using namespace Tucuxi::Core;

/// \brief Get an operation from a collection and check its type.
/// \param _TYPE Type of the operation to get.
#define TEST_GET_OPERATION(_TYPE)                                                         \
do {                                                                                      \
            std::shared_ptr<Operation> operation = collection.getOperationFromId(#_TYPE); \
            ASSERT_TRUE(operation != nullptr);                                            \
            ASSERT_TRUE(dynamic_cast<_TYPE*>(operation.get()) != nullptr);                \
} while (0);

TEST (Core_TestOperationCollection, OperationCollection){
    // The operation
    OperationCollection collection;

    // We populate it
    collection.populate();

    // Check that we get nullptr when asking for a non existing operation
    {
        std::shared_ptr<Operation> operation;
        operation = collection.getOperationFromId("nonExisting");
        ASSERT_TRUE(operation == nullptr);
    }


    // Then check that every hardcoded operation is in the collection
    TEST_GET_OPERATION(IdealBodyWeight);
    TEST_GET_OPERATION(BodySurfaceArea);
    TEST_GET_OPERATION(eGFR_CockcroftGaultGeneral);
    TEST_GET_OPERATION(eGFR_CockcroftGaultIBW);
    TEST_GET_OPERATION(eGFR_CockcroftGaultAdjIBW);
    TEST_GET_OPERATION(GFR_MDRD);
    TEST_GET_OPERATION(GFR_CKD_EPI);
    TEST_GET_OPERATION(eGFR_Schwartz);
    TEST_GET_OPERATION(GFR_Jelliffe);
    TEST_GET_OPERATION(eGFR_SalazarCorcoran);
}
