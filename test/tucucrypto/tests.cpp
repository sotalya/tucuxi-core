//@@license@@

#include <iostream>
#include <string>

#include "tucucommon/utils.h"

#include "test_cryptohelper.h"
#include "test_licensemanager.h"

#define PRINT_SUCCESS(x) (std::cout << "\033[1;32m" << x << "\033[0m" << std::endl)
#define PRINT_FAIL(x) (std::cerr << "\033[1;31m" << x << "\033[0m" << std::endl)

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
        PRINT_FAIL("License Manager test failed");
    }
    else {
        PRINT_SUCCESS("License Manager test succeeded");
    }

    TestCryptoHelper cryptoHelperTests(appFolder);
    cryptoHelperTests.add_test("CryptoHelper-basic", &TestCryptoHelper::basic);
    res = cryptoHelperTests.run(argc, argv);
    tot_res |= res;
    if (res != 0) {
        PRINT_FAIL("Crypto Helper test failed");
    }
    else {
        PRINT_SUCCESS("Crypto Helper test succeeded");
    }

    return tot_res;
}
