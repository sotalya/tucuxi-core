//@@license@@

#include <iostream>
#include <string>

#include "tucucommon/utils.h"

#include "test_dosageimportexport.h"
#include "test_query1comp.h"

#define PRINT_SUCCESS(x)    (std::cout << "\033[1;32m" << x << "\033[0m" << std::endl)
#define PRINT_FAIL(x)    (std::cerr << "\033[1;31m" << x << "\033[0m" << std::endl)

int main(int argc, char** argv)
{
    // Get application folder
    std::string appFolder = Tucuxi::Common::Utils::getAppFolder(argv);

    int res = 0;

    TestDosageImportExport dosageImportExportTests;
    dosageImportExportTests.add_test("testall", &TestDosageImportExport::testall);
    res = dosageImportExportTests.run(argc, argv);
    if (res != 0) {
        PRINT_FAIL("Dosage import export test failed");
    }
    else {
        PRINT_SUCCESS("Dosage import export test succeeded");
    }

    TestQuery1Comp query1CompTests;
    query1CompTests.add_test("testsingle", &TestQuery1Comp::testSingle);
    res |= query1CompTests.run(argc, argv);
    if (res != 0) {
        PRINT_FAIL("Query 1 comp test failed");
    }
    else {
        PRINT_SUCCESS("Query 1 comp test succeeded");
    }

    return 0;
}
