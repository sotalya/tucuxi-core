//@@license@@

#ifndef TEST_OPERATIONCOLLECTION_H
#define TEST_OPERATIONCOLLECTION_H

#include <iostream>
#include <memory>

#include "tucucore/hardcodedoperation.h"
#include "tucucore/operation.h"

#include "fructose/fructose.h"

using namespace Tucuxi::Core;

/// \brief Get an operation from a collection and check its type.
/// \param _TYPE Type of the operation to get.
#define TEST_GET_OPERATION(_TYPE)                                                     \
    do {                                                                              \
        std::shared_ptr<Operation> operation = collection.getOperationFromId(#_TYPE); \
        fructose_assert(operation != nullptr);                                        \
        fructose_assert(dynamic_cast<_TYPE*>(operation.get()) != nullptr);            \
    } while (0);


struct TestOperationCollection : public fructose::test_base<TestOperationCollection>
{
    TestOperationCollection() {}

    /// \brief Test: Add the default hardcoded operations and get them back.
    void testOperationCollection(const std::string& /* _testName */)
    {
        // The operation
        OperationCollection collection;

        // We populate it
        collection.populate();

        // Check that we get nullptr when asking for a non existing operation
        {
            std::shared_ptr<Operation> operation;
            operation = collection.getOperationFromId("nonExisting");
            fructose_assert(operation == nullptr);
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
};

#endif // TEST_OPERATIONCOLLECTION_H
