//@@license@@

#include <iostream>
#include <string>

#include "tucucommon/utils.h"

#define PRINT_SUCCESS(x)    (std::cout << "\033[1;32m" << x << "\033[0m" << std::endl)
#define PRINT_FAIL(x)    (std::cerr << "\033[1;31m" << x << "\033[0m" << std::endl)

#if defined(test_datetime) || !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "test_datetime.h"
#endif
#if defined(test_componentmanager) || !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "test_componentmanager.h"
#endif
#if defined(test_logger) || !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "test_logger.h"
#endif
#if defined(test_scriptengine) || !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "test_scriptengine.h"
#endif
#if defined(test_xml) || !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "test_xml.h"
#endif
#if defined(test_iterator) || !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "test_iterator.h"
#endif
#if defined(test_xmlimporter) || !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "test_xmlimporter.h"
#endif
#if defined(test_unit) || !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "test_unit.h"
#endif

int main(int argc, char** argv)
{
    // Get application folder
    std::string appFolder = Tucuxi::Common::Utils::getAppFolder(argv);

    int res;

#if defined(test_datetime) || !defined(DO_NOT_COMPILE_ALL_TESTS)
    TestDateTime dateTimeTests;

    dateTimeTests.add_test("DateTime-datetime", &TestDateTime::datetime);
    dateTimeTests.add_test("DateTime-timeofday", &TestDateTime::timeofday);
    dateTimeTests.add_test("DateTime-duration", &TestDateTime::duration);
    res = dateTimeTests.run(argc, argv);
    if (res != 0) {
        PRINT_FAIL("Date and time test failed");
    }
    else {
        PRINT_SUCCESS("Date and time test succeeded");
    }
#endif

#if defined(test_componentmanager) || !defined(DO_NOT_COMPILE_ALL_TESTS)
    TestComponentManager componentManagerTests;

    componentManagerTests.add_test("CreateComponent", &TestComponentManager::createComponent);
    res = componentManagerTests.run(argc, argv);
    if (res != 0) {
        PRINT_FAIL("ComponentManager test failed");
    }
    else {
        PRINT_SUCCESS("ComponentManager test succeeded");
    }
#endif

#if defined(test_logger) || !defined(DO_NOT_COMPILE_ALL_TESTS)
    TestLogger loggerTests(appFolder);

    loggerTests.add_test("Logger-crashes", &TestLogger::crashes);
    loggerTests.add_test("Logger-basic", &TestLogger::basic);
    res = loggerTests.run(argc, argv);
    if (res != 0) {
        PRINT_FAIL("Logger test failed");
    }
    else {
        PRINT_SUCCESS("Logger test succeeded");
    }

    Tucuxi::Common::LoggerHelper::beforeExit();
#endif

#if defined(test_scriptengine) || !defined(DO_NOT_COMPILE_ALL_TESTS)
    TestScriptEngine scriptEngineTests;

    scriptEngineTests.add_test("ScriptEngine-basic", &TestScriptEngine::basic);
    res = scriptEngineTests.run(argc, argv);
    if (res != 0) {
        PRINT_FAIL("Script engine test failed");
    }
    else {
        PRINT_SUCCESS("Script engine test succeeded");
    }
#endif

#if defined(test_xml) || !defined(DO_NOT_COMPILE_ALL_TESTS)
    TestXml xmlTests(appFolder);

    xmlTests.add_test("XML-read", &TestXml::read);
    xmlTests.add_test("XML-write", &TestXml::write);
    xmlTests.add_test("XML-files", &TestXml::files);
    res = xmlTests.run(argc, argv);
    if (res != 0) {
        PRINT_FAIL("XML test failed");
    }
    else {
        PRINT_SUCCESS("XML test succeeded");
    }
#endif

#if defined(test_iterator) || !defined(DO_NOT_COMPILE_ALL_TESTS)
    TestIterator iteratorTests;

    iteratorTests.add_test("Iterators", &TestIterator::basic);
    res = iteratorTests.run(argc, argv);
    if (res != 0) {
        PRINT_FAIL("Iterator test failed");
    }
    else {
        PRINT_SUCCESS("Iterator test succeeded");
    }
#endif

#if defined(test_xmlimporter) || !defined(DO_NOT_COMPILE_ALL_TESTS)
    TestXmlImporter xmlImporterTests;

    xmlImporterTests.add_test("XmlImporter", &TestXmlImporter::xmlImporter);
    res = xmlImporterTests.run(argc, argv);
    if (res != 0) {
        PRINT_FAIL("xmlImporter test failed");
    }
    else {
        PRINT_SUCCESS("xmlImporter test succeeded");
    }
#endif

#if defined(test_unit) || !defined(DO_NOT_COMPILE_ALL_TESTS)
    TestUnit unitTests;

    unitTests.add_test("Unit", &TestUnit::unit);
    res = unitTests.run(argc, argv);
    if (res != 0) {
        PRINT_FAIL("unit test failed");
    }
    else {
        PRINT_SUCCESS("unit test succeeded");
    }
#endif

    return 0;
}
