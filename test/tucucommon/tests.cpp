
#include <iostream>
#include <string>

#include "test_componentmanager.h"
#include "test_logger.h"

int main(int argc, char** argv) 
{
    // Get path of executable
    std::string::size_type found = std::string(argv[0]).find_last_of("/\\");
    std::string path = std::string(argv[0]).substr(0,found);

    TestComponentManager componentManagerTests;
    componentManagerTests.add_test("CreateComponent", &TestComponentManager::createComponent);
    int res = componentManagerTests.run(argc, argv);
    if (res != 0) {
            return res;
    }

    TestLogger loggerTests(path);
    loggerTests.add_test("Logger-basic", &TestLogger::basic);
    loggerTests.add_test("Logger-class", &TestLogger::logMyClass);
    return loggerTests.run(argc, argv);
}
