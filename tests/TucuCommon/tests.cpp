
#include "test_componentmanager.h"
#include "test_logger.h"

int main(int argc, char** argv) 
{
	TestComponentManager componentManagerTests;
	componentManagerTests.add_test("CreateComponent", &TestComponentManager::createComponent);
	int res = componentManagerTests.run(argc, argv);
	if (res != 0) {
		return res;
	}

	TestLogger loggerTests;
	loggerTests.add_test("Logger-basic", &TestLogger::basic);
	loggerTests.add_test("Logger-class", &TestLogger::logMyClass);	
	return loggerTests.run(argc, argv);
}
