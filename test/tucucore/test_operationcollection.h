/* 
 * Tucuxi - Tucuxi-core library and command line tool. 
 * This code allows to perform prediction of drug concentration in blood 
 * and to propose dosage adaptations.
 * It has been developed by HEIG-VD, in close collaboration with CHUV. 
 * Copyright (C) 2023 HEIG-VD, maintained by Yann Thoma  <yann.thoma@heig-vd.ch>
 * 
 * This program is free software: you can redistribute it and/or modify 
 * it under the terms of the GNU Affero General Public License as 
 * published by the Free Software Foundation, either version 3 of the 
 * License, or any later version. 
 * 
 * This program is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU Affero General Public License for more details. 
 * 
 * You should have received a copy of the GNU Affero General Public License 
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */


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
