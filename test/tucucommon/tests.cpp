
#include <iostream>
#include <string>

#include "tucucommon/utils.h"

#include "test_componentmanager.h"
#include "test_logger.h"

int main(int argc, char** argv) 
{
    // Get application folder
    std::string appFolder = Tucuxi::Common::Utils::getAppFolder(argv);

    TestComponentManager componentManagerTests;
    componentManagerTests.add_test("CreateComponent", &TestComponentManager::createComponent);
    int res = componentManagerTests.run(argc, argv);
    if (res != 0) {
        printf("ComponentManager test failed");
        return 1;
    }

    TestLogger loggerTests(appFolder);
    loggerTests.add_test("Logger-basic", &TestLogger::basic);
    loggerTests.add_test("Logger-crashes", &TestLogger::crashes);
    res = loggerTests.run(argc, argv);
    if (res != 0) {
        printf("Logger test failed");
        return 1;
    }

    return 0;
}
