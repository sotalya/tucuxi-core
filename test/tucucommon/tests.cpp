/*
* Copyright (C) 2017 Tucuxi SA
*/

#include <iostream>
#include <string>

#include "tucucommon/utils.h"

#include "test_componentmanager.h"
#include "test_licensechecker.h"
#include "test_logger.h"
#include "test_scriptengine.h"
#include "test_datetime.h"
#include "test_xml.h"

int main(int argc, char** argv) 
{
    // Get application folder
    std::string appFolder = Tucuxi::Common::Utils::getAppFolder(argv);

    TestComponentManager componentManagerTests;
    componentManagerTests.add_test("CreateComponent", &TestComponentManager::createComponent);
    int res = componentManagerTests.run(argc, argv);
    if (res != 0) {
        printf("ComponentManager test failed");
        exit(1);
    }

    TestLogger loggerTests(appFolder);
    loggerTests.add_test("Logger-crashes", &TestLogger::crashes);
    loggerTests.add_test("Logger-basic", &TestLogger::basic);
    res = loggerTests.run(argc, argv);
    if (res != 0) {
        printf("Logger test failed");
        exit(1);
    }

    TestLicenseChecker licenseCheckerTests(appFolder);
    licenseCheckerTests.add_test("licenseChecker-basic", &TestLicenseChecker::basic);
    licenseCheckerTests.add_test("licenseChecker-getNewLicense", &TestLicenseChecker::getNewLicense);
    licenseCheckerTests.add_test("licenseChecker-checkValidLicense", &TestLicenseChecker::checkValidLicense);
    licenseCheckerTests.add_test("licenseChecker-checkInvalidLicense", &TestLicenseChecker::checkInvalidLicense);
    res = licenseCheckerTests.run(argc, argv);
    if (res != 0) {
        printf("LicenseChecker test failed");
        exit(1);
    }

    TestScriptEngine scriptEngineTests;
    scriptEngineTests.add_test("ScriptEngine-basic", &TestScriptEngine::basic);
    res = scriptEngineTests.run(argc, argv);
    if (res != 0) {
        printf("Script engine test failed");
        exit(1);
    }

    TestDateTime dateTimeTests;
    dateTimeTests.add_test("DateTime-datetime", &TestDateTime::datetime);
    dateTimeTests.add_test("DateTime-timeofday", &TestDateTime::timeofday);
    dateTimeTests.add_test("DateTime-duration", &TestDateTime::duration);
    res = dateTimeTests.run(argc, argv);
    if (res != 0) {
        printf("Date and time test failed");
        exit(1);
    }

    TestXml xmlTests;
    xmlTests.add_test("XML-read", &TestXml::read);
    xmlTests.add_test("XML-write", &TestXml::write);
    res = xmlTests.run(argc, argv);
    if (res != 0) {
        printf("XML test failed");
        exit(1);
    }

    return 0;
}
