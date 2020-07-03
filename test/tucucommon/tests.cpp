/*
* Copyright (C) 2017 Tucuxi SA
*/

#include <iostream>
#include <string>

#include "tucucommon/utils.h"

#include "test_componentmanager.h"
#include "test_logger.h"
#include "test_scriptengine.h"
#include "test_datetime.h"
#include "test_xml.h"
#include "test_iterator.h"
#include "test_xmlimporter.h"
#include "test_unit.h"

int main(int argc, char** argv) 
{
    // Get application folder
    std::string appFolder = Tucuxi::Common::Utils::getAppFolder(argv);

    int res;

    TestDateTime dateTimeTests;
    dateTimeTests.add_test("DateTime-datetime", &TestDateTime::datetime);
    dateTimeTests.add_test("DateTime-timeofday", &TestDateTime::timeofday);
    dateTimeTests.add_test("DateTime-duration", &TestDateTime::duration);
    res = dateTimeTests.run(argc, argv);
    if (res != 0) {
        printf("Date and time test failed\n");
        exit(1);
    }

    TestComponentManager componentManagerTests;
    componentManagerTests.add_test("CreateComponent", &TestComponentManager::createComponent);
    res = componentManagerTests.run(argc, argv);
    if (res != 0) {
        printf("ComponentManager test failed\n");
        exit(1);
    }

    TestLogger loggerTests(appFolder);
    loggerTests.add_test("Logger-crashes", &TestLogger::crashes);
    loggerTests.add_test("Logger-basic", &TestLogger::basic);
    res = loggerTests.run(argc, argv);
    if (res != 0) {
        printf("Logger test failed\n");
        exit(1);
    }

    TestScriptEngine scriptEngineTests;
    scriptEngineTests.add_test("ScriptEngine-basic", &TestScriptEngine::basic);
    res = scriptEngineTests.run(argc, argv);
    if (res != 0) {
        printf("Script engine test failed\n");
        exit(1);
    }

    TestXml xmlTests(appFolder);
    xmlTests.add_test("XML-read", &TestXml::read);
    xmlTests.add_test("XML-write", &TestXml::write);
    xmlTests.add_test("XML-files", &TestXml::files);
    res = xmlTests.run(argc, argv);
    if (res != 0) {
        printf("XML test failed\n");
        exit(1);
    }

    TestIterator iteratorTests;
    iteratorTests.add_test("Iterators", &TestIterator::basic);
    res = iteratorTests.run(argc, argv);
    if (res != 0) {
        printf("Iterator test failed\n");
        exit(1);
    }

    TestXmlImporter xmlImporterTests;
    xmlImporterTests.add_test("XmlImporter", &TestXmlImporter::xmlImporter);
    res = xmlImporterTests.run(argc, argv);
    if (res != 0) {
        printf("xmlImporter test failed\n");
        exit(1);
    }

    TestUnit unitTests;
    unitTests.add_test("Unit", &TestUnit::unit);
    res = unitTests.run(argc, argv);
    if (res != 0) {
        printf("unit test failed\n");
        exit(1);
    }


    Tucuxi::Common::LoggerHelper::beforeExit();

    return 0;
}
