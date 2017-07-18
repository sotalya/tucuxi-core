
#include <string>
#include <iostream>


#include "fructose/fructose.h"

#include "tucucommon/systeminfo.h"
#include "tucucommon/licensemanager.h"

struct TestLicenseManager : public fructose::test_base<TestLicenseManager>
{
    std::string m_path;

    TestLicenseManager(std::string& _path)
    {
        m_path = _path;
    }

    void basic(const std::string& _testName)
    {
        std::cout << _testName << std::endl;

        std::string id;

        id = Tucuxi::Common::SystemInfo::retrieveFingerPrint(Tucuxi::Common::CPU);
        std::cout << "CPU : " << id << std::endl;

        id = Tucuxi::Common::SystemInfo::retrieveFingerPrint(Tucuxi::Common::MOTHERBOARD);
        std::cout << "MOTHERBOARD : " << id << std::endl;

        id = Tucuxi::Common::SystemInfo::retrieveFingerPrint(Tucuxi::Common::BIOS);
        std::cout << "BIOS : " << id << std::endl;

        id = Tucuxi::Common::SystemInfo::retrieveFingerPrint(Tucuxi::Common::PRODUCT);
        std::cout << "PRODUCT : " << id << std::endl;

        id = Tucuxi::Common::SystemInfo::retrieveFingerPrint(Tucuxi::Common::NETWORK);
        std::cout << "NETWORK : " << id << std::endl;

        id = Tucuxi::Common::SystemInfo::retrieveFingerPrint(Tucuxi::Common::NAME);
        std::cout << "NAME : " << id << std::endl;

        id = Tucuxi::Common::SystemInfo::retrieveFingerPrint(Tucuxi::Common::ERROR);
        std::cout << "Error : " << id << std::endl;
    }

    void checkValidLicense(const std::string& _testName)
    {
        std::cout << _testName << std::endl;
    }

    void checkInvalidLicense(const std::string& _testName)
    {
        std::cout << _testName << std::endl;
    }

    void installNewLicense(const std::string& _testName)
    {
        std::cout << _testName << std::endl;
    }

};
