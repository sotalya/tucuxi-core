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

#include "test_cryptohelper.h"
#include "test_licensemanager.h"

int main(int argc, char** argv)
{
    // Get application folder
    std::string appFolder = Tucuxi::Common::Utils::getAppFolder(argv);

    int res;

    TestLicenseManager licenseManagerTests(appFolder);
    licenseManagerTests.add_test("LicenseManager-basic", &TestLicenseManager::basic);
    licenseManagerTests.add_test("LicenseManager-installNewLicense", &TestLicenseManager::installNewLicense);
    licenseManagerTests.add_test("LicenseManager-checkInvalidLicense", &TestLicenseManager::checkInvalidLicense);
    res = licenseManagerTests.run(argc, argv);
    if (res != 0) {
        printf("License Manager test failed\n");
        exit(1);
    }

    TestCryptoHelper cryptoHelperTests(appFolder);
    cryptoHelperTests.add_test("CryptoHelper-basic", &TestCryptoHelper::basic);
    res = cryptoHelperTests.run(argc, argv);
    if (res != 0) {
        printf("Crypto Helper test failed\n");
        exit(1);
    }

    return 0;
}
