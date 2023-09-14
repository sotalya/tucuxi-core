//@@license@@

#include <iostream>
#include <string>

#include "tucucommon/utils.h"

#include "test_dosageimportexport.h"
#include "test_query1comp.h"

int main(int argc, char** argv)
{
    // Get application folder
    std::string appFolder = Tucuxi::Common::Utils::getAppFolder(argv);

    int res = 0;

    TestDosageImportExport dosageImportExportTests;
    dosageImportExportTests.add_test("testall", &TestDosageImportExport::testall);
    res = dosageImportExportTests.run(argc, argv);
    if (res != 0) {
        printf("Dosage import export test failed\n");
        exit(1);
    }
    else {
        printf("Dosage import export test succeeded\n");
    }

    TestQuery1Comp query1CompTests;
    query1CompTests.add_test("testsingle", &TestQuery1Comp::testSingle);
    res |= query1CompTests.run(argc, argv);
    if (res != 0) {
        printf("Query 1 comp test failed\n");
        exit(1);
    }
    else {
        printf("Query 1 comp test succeeded\n");
    }

    return 0;
}
