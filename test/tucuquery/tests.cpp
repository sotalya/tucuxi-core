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
