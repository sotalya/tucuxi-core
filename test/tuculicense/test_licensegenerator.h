/*
* Copyright (C) 2017 Tucuxi SA
*/

#include <string>
#include <iostream>
#include <ctime>

#include "fructose/fructose.h"

#include "tucucommon/cryptohelper.h"
#include "tucucommon/licensemanager.h"
#include "tuculicense/licensegenerator.h"

struct TestLicenseGenerator : public fructose::test_base<TestLicenseGenerator>
{
    std::string m_path;
    std::string m_key;

    TestLicenseGenerator(std::string& _path)
    {
        // Path
        m_path = _path;

        // Key
        m_key = "86685E7AA62844102FC7FAD5D6DDF46C9CA7777BF4E0153FDF5F86463EAC0D75";
    }


    void basic(const std::string& _testName)
    {
        std::cout << _testName << std::endl;

        // Make sure to remove delete licence file.
        const std::string& fileName = Tucuxi::Common::Utils::strFormat("%s/%s", m_path.c_str(), "license.txt");
        std::remove(fileName.c_str());

        // Detect missing license file
        int res = Tucuxi::Common::LicenseManager::checkLicenseFile(fileName);
        fructose_assert(res == int(Tucuxi::Common::LicenseError::MISSING_LICENSE_FILE));

        // Generate a request to get a new license file
        std::string request;
        res = Tucuxi::Common::LicenseManager::generateRequestString(&request);
        fructose_assert(res == int(Tucuxi::Common::LicenseError::REQUEST_SUCCESSFUL));

        Tucuxi::License::Request plainRequest;
        res = Tucuxi::License::LicenseGenerator::decryptRequest(request, &plainRequest);

        fructose_assert(res == int(Tucuxi::License::RequestError::REQUEST_SUCCESSFUL));

        std::cout << "License : "
                  << plainRequest.m_keyWord << ":"
                  << plainRequest.m_type << ":"
                  << plainRequest.m_fingerprint << ":"
                  << plainRequest.m_date.year() << "-"
                  << plainRequest.m_date.month() << "-"
                  << plainRequest.m_date.day() << ":"
                  << plainRequest.m_version
                  << std::endl;

        std::string license = Tucuxi::License::LicenseGenerator::generateLicense(plainRequest);

        std::string test;
        if(!Tucuxi::Common::CryptoHelper::decrypt(m_key, license, &test))  {
            fructose_fail("Error encrypt failed.");
        }

         std::cout << "Test : " << test << std::endl;

        // Install a new license file
        res = Tucuxi::Common::LicenseManager::installLicense(license, fileName);
        fructose_assert(res == int(Tucuxi::Common::LicenseError::INSTALLATION_SUCCESSFUL));

        // Check license file
        res = Tucuxi::Common::LicenseManager::checkLicenseFile(fileName);
        fructose_assert(res == int(Tucuxi::Common::LicenseError::VALID_LICENSE));

        // Check license file
        res = Tucuxi::Common::LicenseManager::checkLicenseFile(fileName);
        fructose_assert(res == int(Tucuxi::Common::LicenseError::VALID_LICENSE));

    }

};
