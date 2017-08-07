
#include <string>
#include <iostream>

#include "fructose/fructose.h"
//#include "date/date.h"

#include "tucucommon/systeminfo.h"
#include "tucucommon/cryptohelper.h"
#include "tucucommon/licensemanager.h"
#include "tucucommon/datetime.h"

//using namespace std::chrono_literals;
//using namespace date;
using namespace Tucuxi::Common;

#define DEBUG_MSG

struct TestLicenseManager : public fructose::test_base<TestLicenseManager>
{
    std::string m_path;

    std::string m_fingerprint;  // Fingerprint
    MachineIdType m_type;       // Type of fingerprint
    std::string m_licenses[6];  // Licenses

    TestLicenseManager(std::string& _path)
    {
        // Path
        m_path = _path;

        // Build Fingerprint
        for (int i = CPU; i != ERROR; i++) {
            m_fingerprint = SystemInfo::retrieveFingerPrint(static_cast<MachineIdType>(i));

            if(!m_fingerprint.empty()) {
                m_type = static_cast<MachineIdType>(i);
                break;
            }
        }

        // Get hash from fingerprint
        CryptoHelper::hash(m_fingerprint, &m_fingerprint);

        // Build Date
        DateTime dateToday;
        DateTime dateEndLicense1 = dateToday; // + DateTime("0-01-0","%Y-%b-%ds");
        DateTime dateEndLicense2 = dateToday; // - DateTime("0-01-0","%Y-%b-%ds");

        std::string today = std::to_string(dateToday.year());
        today += "-";
        today += std::to_string(dateToday.month());
        today += "-";
        today += std::to_string(dateToday.day());

        std::string endLicense1 = std::to_string(dateEndLicense1.year()+1);
        endLicense1 += "-";
        endLicense1 += std::to_string(dateEndLicense1.month());
        endLicense1 += "-";
        endLicense1 += std::to_string(dateEndLicense1.day());

        std::string endLicense2 = std::to_string(dateEndLicense2.year()-1);
        endLicense2 += "-";
        endLicense2 += std::to_string(dateEndLicense2.month());
        endLicense2 += "-";
        endLicense2 += std::to_string(dateEndLicense2.day());

        // Valid licens
        m_licenses[0] = "license:";
        m_licenses[0] += std::to_string(m_type);
        m_licenses[0] += ":";
        m_licenses[0] += m_fingerprint;
        m_licenses[0] += ":";
        m_licenses[0] += endLicense1;
        m_licenses[0] += ":";
        m_licenses[0] += today;

        if(!Tucuxi::Common::CryptoHelper::encrypt("86685E7AA62844102FC7FAD5D6DDF46C9CA7777BF4E0153FDF5F86463EAC0D75",
                                  m_licenses[0],
                                  &m_licenses[0]))  {
            fructose_fail("Error encrypt failed.");
        }

        // Wrong license with fake keyword
        m_licenses[1] = "random:";
        m_licenses[1] += std::to_string(m_type);
        m_licenses[1] += ":";
        m_licenses[1] += m_fingerprint;
        m_licenses[1] += ":";
        m_licenses[1] += endLicense1;
        m_licenses[1] += ":";
        m_licenses[1] += today;

        if(!Tucuxi::Common::CryptoHelper::encrypt("86685E7AA62844102FC7FAD5D6DDF46C9CA7777BF4E0153FDF5F86463EAC0D75",
                                  m_licenses[1],
                                  &m_licenses[1]))  {
            fructose_fail("Error encrypt failed.");
        }

        // Wrong license with wrong type of fingerprint
        m_licenses[2] = "license:";
        m_licenses[2] += std::to_string(10);
        m_licenses[2] += ":";
        m_licenses[2] += m_fingerprint;
        m_licenses[2] += ":";
        m_licenses[2] += endLicense1;
        m_licenses[2] += ":";
        m_licenses[2] += today;

        if(!Tucuxi::Common::CryptoHelper::encrypt("86685E7AA62844102FC7FAD5D6DDF46C9CA7777BF4E0153FDF5F86463EAC0D75",
                                  m_licenses[2],
                                  &m_licenses[2]))  {
            fructose_fail("Error encrypt failed.");
        }

        // Wrong license with no fingerprint
        m_licenses[3] = "license:";
        m_licenses[3] += std::to_string(m_type);
        m_licenses[3] += ":";
        m_licenses[3] += "";
        m_licenses[3] += ":";
        m_licenses[3] += endLicense1;
        m_licenses[3] += ":";
        m_licenses[3] += today;

        if(!Tucuxi::Common::CryptoHelper::encrypt("86685E7AA62844102FC7FAD5D6DDF46C9CA7777BF4E0153FDF5F86463EAC0D75",
                                  m_licenses[3],
                                  &m_licenses[3]))  {
            fructose_fail("Error encrypt failed.");
        }

        // Fake license (date change by user)
        m_licenses[4] = "license:";
        m_licenses[4] += std::to_string(m_type);
        m_licenses[4] += ":";
        m_licenses[4] += m_fingerprint;
        m_licenses[4] += ":";
        m_licenses[4] += endLicense2;
        m_licenses[4] += ":";
        m_licenses[4] += endLicense1;

        if(!Tucuxi::Common::CryptoHelper::encrypt("86685E7AA62844102FC7FAD5D6DDF46C9CA7777BF4E0153FDF5F86463EAC0D75",
                                  m_licenses[4],
                                  &m_licenses[4]))  {
            fructose_fail("Error encrypt failed.");
        }

        // Expired license
        m_licenses[5] = "license:";
        m_licenses[5] += std::to_string(m_type);
        m_licenses[5] += ":";
        m_licenses[5] += m_fingerprint;
        m_licenses[5] += ":";
        m_licenses[5] += endLicense2;
        m_licenses[5] += ":";
        m_licenses[5] += today;

        if(!Tucuxi::Common::CryptoHelper::encrypt("86685E7AA62844102FC7FAD5D6DDF46C9CA7777BF4E0153FDF5F86463EAC0D75",
                                  m_licenses[5],
                                  &m_licenses[5]))  {
            fructose_fail("Error encrypt failed.");
        }
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

        MachineId idfromMachine;
        for (int i = CPU; i != ERROR; i++) {
            idfromMachine.m_fingerprint = SystemInfo::retrieveFingerPrint(static_cast<MachineIdType>(i));

            if(!idfromMachine.m_fingerprint.empty()) {
                idfromMachine.m_type = static_cast<MachineIdType>(i);
                break;
            }
        }

        fructose_assert(idfromMachine.m_type != ERROR);
    }

    void installNewLicense(const std::string& _testName)
    {
        std::cout << _testName << std::endl;

        // Make sure to remove delete licence file.
        const std::string& fileName = Tucuxi::Common::Utils::strFormat("%s/%s", m_path.c_str(), "license.txt");
        std::remove(fileName.c_str());

        // Detect missing license file
        int res = Tucuxi::Common::LicenseManager::checkLicenseFile(fileName);
        fructose_assert(res == Tucuxi::Common::LicenseError::MISSING_LICENSE_FILE);

        // Generate a request to get a new license file
        std::string request;
        res = Tucuxi::Common::LicenseManager::generateRequestString(&request);
        fructose_assert(res == Tucuxi::Common::LicenseError::REQUEST_SUCCESSFUL);

        std::string test;
        if(!Tucuxi::Common::CryptoHelper::decrypt("86685E7AA62844102FC7FAD5D6DDF46C9CA7777BF4E0153FDF5F86463EAC0D75",
                                  m_licenses[0],
                                  &test))  {
            fructose_fail("Error encrypt failed.");
        }

         std::cout << "Test : " << test << std::endl;

        // Install a new license file
        res = Tucuxi::Common::LicenseManager::installLicense(m_licenses[0], fileName);
        fructose_assert(res == Tucuxi::Common::LicenseError::INSTALLATION_SUCCESSFUL);

        // Check license file
        res = Tucuxi::Common::LicenseManager::checkLicenseFile(fileName);
        fructose_assert(res == Tucuxi::Common::LicenseError::VALID_LICENSE);

        // Check license file
        res = Tucuxi::Common::LicenseManager::checkLicenseFile(fileName);
        fructose_assert(res == Tucuxi::Common::LicenseError::VALID_LICENSE);
    }

    void checkInvalidLicense(const std::string& _testName)
    {
        std::cout << _testName << std::endl;

        int res = 0;

        for(int i=1; i < 4; i++) {

            std::string test;
            if(!Tucuxi::Common::CryptoHelper::decrypt("86685E7AA62844102FC7FAD5D6DDF46C9CA7777BF4E0153FDF5F86463EAC0D75",
                                      m_licenses[i],
                                      &test))  {
                fructose_fail("Error encrypt failed.");
            }

            std::cout << "Test " << i << " : " << test << std::endl;

            // Make sure to remove delete licence file.
            const std::string& fileName = Tucuxi::Common::Utils::strFormat("%s/%s", m_path.c_str(), "license.txt");
            std::remove(fileName.c_str());

            // Install a invalid license file (wrong type)
            Tucuxi::Common::LicenseManager::installLicense(m_licenses[i], fileName);
            fructose_assert(res == Tucuxi::Common::LicenseError::INVALID_LICENSE);

            // Write result in license file
            std::ofstream file(fileName);
            if (!file.is_open()) {
                fructose_fail("Cannot write license file.");
            }
            file << m_licenses[i];
            file.close();

            // Check license file
            res = Tucuxi::Common::LicenseManager::checkLicenseFile(fileName);
            fructose_assert(res == Tucuxi::Common::LicenseError::INVALID_LICENSE);

            // Check license file
            res = Tucuxi::Common::LicenseManager::checkLicenseFile(fileName);
            fructose_assert(res == Tucuxi::Common::LicenseError::INVALID_LICENSE);
        }
    }
};
