/*
* Copyright (C) 2017 Tucuxi SA
*/

#include <iostream>
#include <string>

#include "tucucommon/utils.h"

#include "test_componentmanager.h"
#include "test_licensemanager.h"
#include "test_logger.h"
#include "test_cryptohelper.h"
#include "test_scriptengine.h"
#include "test_datetime.h"
#include "test_xml.h"
#include "test_iterator.h"

int main(int argc, char** argv) 
{
    // Get application folder
    std::string appFolder = Tucuxi::Common::Utils::getAppFolder(argv);

    TestComponentManager componentManagerTests;
    componentManagerTests.add_test("CreateComponent", &TestComponentManager::createComponent);
    int res = componentManagerTests.run(argc, argv);
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

    TestScriptEngine scriptEngineTests;
    scriptEngineTests.add_test("ScriptEngine-basic", &TestScriptEngine::basic);
    res = scriptEngineTests.run(argc, argv);
    if (res != 0) {
        printf("Script engine test failed\n");
        exit(1);
    }

    TestDateTime dateTimeTests;
    dateTimeTests.add_test("DateTime-datetime", &TestDateTime::datetime);
    dateTimeTests.add_test("DateTime-timeofday", &TestDateTime::timeofday);
    dateTimeTests.add_test("DateTime-duration", &TestDateTime::duration);
    res = dateTimeTests.run(argc, argv);
    if (res != 0) {
        printf("Date and time test failed\n");
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

    Tucuxi::Common::LoggerHelper::beforeExit();

    return 0;
}
