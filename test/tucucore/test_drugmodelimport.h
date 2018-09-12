#ifndef TEST_DRUGMODELIMPORT_H
#define TEST_DRUGMODELIMPORT_H

#include "fructose/fructose.h"

#include "tucucore/drugmodel/drugmodel.h"
#include "tucucore/drugmodelimport.h"
#include "drugmodels/buildimatinib.h"

using namespace Tucuxi::Core;



struct TestDrugModelImport : public fructose::test_base<TestDrugModelImport>
{
    TestDrugModelImport() { }

    DrugModel *buildImatinib()
    {
        BuildImatinib builder;
        return builder.buildDrugModel();
    }

    void testImatinib(const std::string& /* _testName */)
    {
        DrugModel* imatinib;
        DrugModelImport *importer = new DrugModelImport();

        DrugModelImport::Result result = importer->import(imatinib, "/home/ythoma/docs/ezechiel/git/dev/src/newdrugs/last/ch.heig-vd.ezechiel.example.xml");

        fructose_assert( result == DrugModelImport::Result::Ok);

        fructose_assert(imatinib != nullptr);

        delete importer;
    }

};

#endif // TEST_DRUGMODELIMPORT_H
