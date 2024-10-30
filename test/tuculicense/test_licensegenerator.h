/* 
 * Tucuxi - Tucuxi-core library and command line tool. 
 * This code allows to perform prediction of drug concentration in blood 
 * and to propose dosage adaptations.
 * It has been developed by HEIG-VD, in close collaboration with CHUV. 
 * Copyright (C) 2023 HEIG-VD, maintained by Yann Thoma  <yann.thoma@heig-vd.ch>
 * 
 * This program is free software: you can redistribute it and/or modify 
 * it under the terms of the GNU Affero General Public License as 
 * published by the Free Software Foundation, either version 3 of the 
 * License, or any later version. 
 * 
 * This program is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU Affero General Public License for more details. 
 * 
 * You should have received a copy of the GNU Affero General Public License 
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */


#include <ctime>
#include <iostream>
#include <string>

#include "fructose/fructose.h"
#include "tucucrypto/cryptohelper.h"
#include "tucucrypto/licensemanager.h"
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
        Tucuxi::License::RequestError res2 = Tucuxi::License::LicenseGenerator::decryptRequest(request, plainRequest);

        fructose_assert(res2 == Tucuxi::License::RequestError::REQUEST_SUCCESSFUL);

        // clang-format off
        std::cout << "License : "
                  << plainRequest.m_keyWord << ":"
                  << plainRequest.m_type << ":"
                  << plainRequest.m_fingerprint << ":"
                  << plainRequest.m_date.year() << "-"
                  << plainRequest.m_date.month() << "-"
                  << plainRequest.m_date.day() << ":"
                  << plainRequest.m_version
                  << std::endl;
        // clang-format on

        std::string license = Tucuxi::License::LicenseGenerator::generateLicense(plainRequest);

        std::string test;
        if (!Tucuxi::Common::CryptoHelper::decrypt(m_key, license, &test)) {
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
