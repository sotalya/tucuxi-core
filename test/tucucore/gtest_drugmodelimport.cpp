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


#include <gtest/gtest.h>

#include "tucucore/drugmodel/drugmodel.h"
#include "tucucore/drugmodelimport.h"

#include "gtest_drugmodelimport_xmlstrings.h"

using namespace Tucuxi::Core;

TEST(Core_TestDrugModelImport, DrugModelImportFake)
{
    std::unique_ptr<DrugModel> fake;
    auto importer = std::make_unique<DrugModelImport>();

    DrugModelImport::Status status = importer->importFromString(fake, drug1);
    //        DrugModelImport::Result result = importer->importFromFile(imatinib, "/home/ythoma/docs/ezechiel/git/dev/src/newdrugs/last/ch.heig-vd.ezechiel.example.xml");
    //        DrugModelImport::Result result = importer->importFromFile(imatinib, "/home/ythoma/docs/ezechiel/git/dev/src/drugs2/ch.tucuxi.busulfan_children.xml");

    ASSERT_EQ(status, DrugModelImport::Status::Ok);

    ASSERT_TRUE(fake != nullptr);
}

TEST(Core_TestDrugModelImport, DrugModelImportBusulfan1)
{
    std::unique_ptr<DrugModel> busulfan;
    auto importer = std::make_unique<DrugModelImport>();

    DrugModelImport::Status status = importer->importFromString(busulfan, busulfan1);

    ASSERT_EQ(status, DrugModelImport::Status::Ok);

    ASSERT_TRUE(busulfan != nullptr);
}

TEST(Core_TestDrugModelImport, DrugModelImportOperation)
{
    std::unique_ptr<Operation> operation;
    auto importer = std::make_unique<DrugModelImport>();

    DrugModelImport::Status status = importer->importOperationFromString(operation, operationXml);

    ASSERT_EQ(status, DrugModelImport::Status::Ok);

    ASSERT_TRUE(operation != nullptr);
}
