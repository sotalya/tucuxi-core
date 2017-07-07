
#include <iostream>
#include <string>

#include "tucucommon/utils.h"
#include "tucucore/dosage.h"

#include "test_dosage.h"

int main(int argc, char** argv) 
{
    // Get application folder
    std::string appFolder = Tucuxi::Common::Utils::getAppFolder(argv);

    TestDosage dosageTests;
    dosageTests.add_test("Dosage-basic", &TestDosage::basic);
    int res = dosageTests.run(argc, argv);
    if (res != 0) {
        printf("Dosage test failed");
        exit(1);
    }

    return 0;
}
