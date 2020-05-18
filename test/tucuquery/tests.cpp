/*
* Copyright (C) 2017 Tucuxi SA
*/

#include <iostream>
#include <string>

#include "test_dosageimportexport.h"

int main(int argc, char** argv) 
{
    // Get application folder
    std::string appFolder = Tucuxi::Common::Utils::getAppFolder(argv);

    int res;

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

    return 0;
}
