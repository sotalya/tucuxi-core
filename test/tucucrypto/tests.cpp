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
