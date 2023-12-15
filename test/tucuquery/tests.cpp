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


#include <iostream>
#include <string>

#include "tucucommon/utils.h"

#include "test_dosageimportexport.h"
#include "test_query1comp.h"

#define PRINT_SUCCESS(x) (std::cout << "\033[1;32m" << x << "\033[0m" << std::endl)
#define PRINT_FAIL(x) (std::cerr << "\033[1;31m" << x << "\033[0m" << std::endl)

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
