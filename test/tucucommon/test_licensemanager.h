
#include <string>
#include <iostream>


#include "fructose/fructose.h"

#include "tucucommon/systeminfo.h"
#include "tucucommon/cryptohelper.h"
#include "tucucommon/licensemanager.h"
//#include "tucucommon/duration.h"
#include "tucucommon/datetime.h"


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

    void installNewLicense(const std::string& _testName)
    {
        std::cout << _testName << std::endl;

        // Make sure to remove delete licence file.
        const std::string& fileName = Tucuxi::Common::Utils::strFormat("%s/%s", m_path.c_str(), "license.txt");
        std::remove(fileName.c_str());

        // Detect missing license file
        int res = Tucuxi::Common::LicenseManager::checkLicenseFile(fileName);

        fructose_assert(res == Tucuxi::Common::LicenseError::MISSING_LICENSE);

        // Generate a request to get a new license file
        std::string request;
        res = Tucuxi::Common::LicenseManager::generateRequestString(&request);
        fructose_assert(res == 0);

        std::cout << "Request string : " << request << std::endl;

        // Fictif license with a infini duration
        std::string license = simuleNewLicense(request);

        std::cout << "License string : " << license << std::endl;

        // Install a new license file
        res = Tucuxi::Common::LicenseManager::installLicense(license, fileName);
        fructose_assert(res == 0);
    }

    std::string simuleNewLicense(std::string _request) {

        Tucuxi::Common::CryptoHelper::decrypt("86685E7AA62844102FC7FAD5D6DDF46C9CA7777BF4E0153FDF5F86463EAC0D75",
                                              _request,
                                              &_request);

        std::cout << "Plain request string : " << _request << std::endl;

        std::size_t field1 = _request.find_first_of(":");
        std::size_t field2 = _request.find_last_of(":");

         if(field1 == std::string::npos || field2 == std::string::npos) {
            fructose_fail();
        }

        // Replace request by license :
        _request = _request.replace(0, field1, "license");

        // Remove version of tucuxi
        _request = _request.substr(0, field2);

        // Add end date of license
        Tucuxi::Common::DateTime endValidty = Tucuxi::Common::DateTime();
        //Tucuxi::Common::DateTime endValidty = Tucuxi::Common::DateTime() + Tucuxi::Common::Duration(date::days(60));

        _request += ":";
        _request += std::to_string(endValidty.year() + 1); // Valid for 1 year
        _request += "/";
        _request += std::to_string(endValidty.month());
        _request += "/";
        _request += std::to_string(endValidty.day());

        std::cout << "Plain license string : " << _request << std::endl;

        std::string license;
        if(!Tucuxi::Common::CryptoHelper::encrypt("86685E7AA62844102FC7FAD5D6DDF46C9CA7777BF4E0153FDF5F86463EAC0D75",
                                  _request,
                                  &license))  {
            fructose_fail();
        }

        return license;
    }

    void checkValidLicense(const std::string& _testName)
    {
        std::cout << _testName << std::endl;

//        // Make sure to remove delete licence file.
//        const std::string& fileName = Tucuxi::Common::Utils::strFormat("%s/%s", m_path.c_str(), "license.txt");
//        std::remove(fileName.c_str());

//        // Install a new license file with a infini duration
//        std::string response = "";
//        Tucuxi::Common::LicenseManager::installLicense(response, fileName);

//        // Check license file
//        Tucuxi::Common::LicenseManager::checkLicenseFile(fileName);
    }

    void checkInvalidLicense(const std::string& _testName)
    {
        std::cout << _testName << std::endl;

//        std::string licenses[6];

//        licenses[0] = ""; // Invalid type
//        licenses[1] = ""; // Wrong type
//        licenses[2] = ""; // Invalid ID
//        licenses[3] = ""; // invalid Date
//        licenses[4] = ""; // invalid duration
//        licenses[5] = ""; // expired license

//        for(int i=0; i < 6; i++) {

//            // Make sure to remove delete licence file.
//            const std::string& fileName = Tucuxi::Common::Utils::strFormat("%s/%s", m_path.c_str(), "license.txt");
//            std::remove(fileName.c_str());

//            // Install a invalid license file (wrong type)
//            std::string response = "";
//            Tucuxi::Common::LicenseManager::installLicense(licenses[i], fileName);

//            // Check license file
//            Tucuxi::Common::LicenseManager::checkLicenseFile(fileName);
//        }
    }
};
