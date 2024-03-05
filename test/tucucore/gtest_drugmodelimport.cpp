//@@license@@

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
