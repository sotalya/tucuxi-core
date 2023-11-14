//@@license@@

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
    int tot_res = 0;

    TestLicenseManager licenseManagerTests(appFolder);
    licenseManagerTests.add_test("LicenseManager-basic", &TestLicenseManager::basic);
    licenseManagerTests.add_test("LicenseManager-installNewLicense", &TestLicenseManager::installNewLicense);
    licenseManagerTests.add_test("LicenseManager-checkInvalidLicense", &TestLicenseManager::checkInvalidLicense);
    res = licenseManagerTests.run(argc, argv);
    tot_res |= res;
    if (res != 0) {
        std::cerr << "\033[1;31mLicense Manager test failed\033[0m\n";
    }
    else {
        std::cout << "\033[1;32mLicense Manager test succeeded\033[0m\n";
    }

    TestCryptoHelper cryptoHelperTests(appFolder);
    cryptoHelperTests.add_test("CryptoHelper-basic", &TestCryptoHelper::basic);
    res = cryptoHelperTests.run(argc, argv);
    tot_res |= res;
    if (res != 0) {
        std::cerr << "\033[1;31mCrypto Helper test failed\033[0m\n";
    }
    else {
        std::cout << "\033[1;32mCrypto Helper test succeeded\033[0m\n";
    }

    return tot_res;
}
