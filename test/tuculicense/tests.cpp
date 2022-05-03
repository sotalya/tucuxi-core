//@@license@@

#include <iostream>
#include <string>

#include "tucucommon/utils.h"

#include "test_licensegenerator.h"

int main(int argc, char** argv)
{
    // Get application folder
    std::string appFolder = Tucuxi::Common::Utils::getAppFolder(argv);

    int res = 0;

    TestLicenseGenerator licenseGeneratorTests(appFolder);
    licenseGeneratorTests.add_test("licenseGenerator-basic", &TestLicenseGenerator::basic);
    res = licenseGeneratorTests.run(argc, argv);
    if (res != 0) {
        printf("License Generator test failed\n");
        exit(1);
    }

    return 0;
}
