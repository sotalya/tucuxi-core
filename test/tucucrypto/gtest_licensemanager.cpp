//@@license@@

#include <iostream>
#include <fstream>
#include <string>

#include <gtest/gtest.h>

#include "tucucommon/datetime.h"

#include "tucucommon/utils.h"

#include "tucucrypto/cryptohelper.h"
#include "tucucrypto/licensemanager.h"
#include "tucucrypto/systeminfo.h"

extern std::string appFolder;
using namespace Tucuxi::Common;


std::string m_path;

std::string m_key;

std::string m_fingerprint; // Fingerprint
MachineIdType m_type;      // Type of fingerprint
std::string m_licenses[6]; // Licenses

static void testLicenseManager(std::string& _path)
{
    // Path
    m_path = _path;

    // Key
    m_key = "86685E7AA62844102FC7FAD5D6DDF46C9CA7777BF4E0153FDF5F86463EAC0D75";

    // Build Fingerprint
    for (int i = int(MachineIdType::CPU); i != int(MachineIdType::UNDEFINED); i++) {
        m_fingerprint = SystemInfo::retrieveFingerPrint(static_cast<MachineIdType>(i));

        if (!m_fingerprint.empty()) {
            m_type = static_cast<MachineIdType>(i);
            break;
        }
    }

    // Get hash from fingerprint
    CryptoHelper::hash(m_fingerprint, m_fingerprint);

    // Build Date
    DateTime dateToday = DateTime::now();

    char today[9];
    char endLicense1[9];
    char endLicense2[9];
    sprintf(today, "%i%02i%02i", dateToday.year(), dateToday.month(), dateToday.day());
    sprintf(endLicense1, "%i%02i%02i", dateToday.year() + 1, dateToday.month(), dateToday.day());
    sprintf(endLicense2, "%i%02i%02i", dateToday.year() - 1, dateToday.month(), dateToday.day());

    // Valid licens
    m_licenses[0] = "license:";
    m_licenses[0] += std::to_string(int(m_type));
    m_licenses[0] += ":";
    m_licenses[0] += m_fingerprint;
    m_licenses[0] += ":";
    m_licenses[0] += endLicense1;
    m_licenses[0] += ":";
    m_licenses[0] += today;

    if (!Tucuxi::Common::CryptoHelper::encrypt(m_key, m_licenses[0], m_licenses[0])) {
        testing::AssertionFailure() << "Error encrypt failed.";
    }

    // Wrong license with fake keyword
    m_licenses[1] = "random:";
    m_licenses[1] += std::to_string(int(m_type));
    m_licenses[1] += ":";
    m_licenses[1] += m_fingerprint;
    m_licenses[1] += ":";
    m_licenses[1] += endLicense1;
    m_licenses[1] += ":";
    m_licenses[1] += today;

    if (!Tucuxi::Common::CryptoHelper::encrypt(m_key, m_licenses[1], m_licenses[1])) {
        testing::AssertionFailure() << "Error encrypt failed.";
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

    if (!Tucuxi::Common::CryptoHelper::encrypt(m_key, m_licenses[2], m_licenses[2])) {
        testing::AssertionFailure() << "Error encrypt failed.";
    }

    // Wrong license with no fingerprint
    m_licenses[3] = "license:";
    m_licenses[3] += std::to_string(int(m_type));
    m_licenses[3] += ":";
    m_licenses[3] += "";
    m_licenses[3] += ":";
    m_licenses[3] += endLicense1;
    m_licenses[3] += ":";
    m_licenses[3] += today;

    if (!Tucuxi::Common::CryptoHelper::encrypt(m_key, m_licenses[3], m_licenses[3])) {
        testing::AssertionFailure() << "Error encrypt failed.";
    }

    // Fake license (date change by user)
    m_licenses[4] = "license:";
    m_licenses[4] += std::to_string(int(m_type));
    m_licenses[4] += ":";
    m_licenses[4] += m_fingerprint;
    m_licenses[4] += ":";
    m_licenses[4] += endLicense2;
    m_licenses[4] += ":";
    m_licenses[4] += endLicense1;

    if (!Tucuxi::Common::CryptoHelper::encrypt(m_key, m_licenses[4], m_licenses[4])) {
        testing::AssertionFailure() << "Error encrypt failed.";
    }

    // Expired license
    m_licenses[5] = "license:";
    m_licenses[5] += std::to_string(int(m_type));
    m_licenses[5] += ":";
    m_licenses[5] += m_fingerprint;
    m_licenses[5] += ":";
    m_licenses[5] += endLicense2;
    m_licenses[5] += ":";
    m_licenses[5] += endLicense2;

    if (!Tucuxi::Common::CryptoHelper::encrypt(m_key, m_licenses[5], m_licenses[5])) {
        testing::AssertionFailure() << "Error encrypt failed.";
    }
}

TEST (Crypto_TestLicenseManager, Basic){
    std::string id;
    testLicenseManager(appFolder);

    id = Tucuxi::Common::SystemInfo::retrieveFingerPrint(Tucuxi::Common::MachineIdType::CPU);
    std::cout << "CPU : " << id << std::endl;

    id = Tucuxi::Common::SystemInfo::retrieveFingerPrint(Tucuxi::Common::MachineIdType::MOTHERBOARD);
    std::cout << "MOTHERBOARD : " << id << std::endl;

    id = Tucuxi::Common::SystemInfo::retrieveFingerPrint(Tucuxi::Common::MachineIdType::DISK);
    std::cout << "DISK : " << id << std::endl;

    id = Tucuxi::Common::SystemInfo::retrieveFingerPrint(Tucuxi::Common::MachineIdType::BIOS);
    std::cout << "BIOS : " << id << std::endl;

    id = Tucuxi::Common::SystemInfo::retrieveFingerPrint(Tucuxi::Common::MachineIdType::PRODUCT);
    std::cout << "PRODUCT : " << id << std::endl;

    id = Tucuxi::Common::SystemInfo::retrieveFingerPrint(Tucuxi::Common::MachineIdType::MAC);
    std::cout << "MAC: " << id << std::endl;

    id = Tucuxi::Common::SystemInfo::retrieveFingerPrint(Tucuxi::Common::MachineIdType::NAME);
    std::cout << "NAME : " << id << std::endl;

    id = Tucuxi::Common::SystemInfo::retrieveFingerPrint(Tucuxi::Common::MachineIdType::UNDEFINED);
    std::cout << "UNDEFINED : " << id << std::endl;

    MachineIdType idType = MachineIdType::UNDEFINED;
    std::string hashedFingerprint;
    for (int i = int(MachineIdType::CPU); i != int(MachineIdType::UNDEFINED); i++) {
        hashedFingerprint = SystemInfo::retrieveFingerPrint(static_cast<MachineIdType>(i));

        if (!hashedFingerprint.empty()) {
            idType = static_cast<MachineIdType>(i);
            break;
        }
    }

    ASSERT_TRUE(idType != MachineIdType::UNDEFINED);
}

TEST (Crypto_TestLicenseManager, InstallNewLicense){
    // Make sure to remove delete licence file.
    testLicenseManager(appFolder);
    const std::string& fileName = Tucuxi::Common::Utils::strFormat("%s/%s", m_path.c_str(), "license.txt");
    std::remove(fileName.c_str());

    // Detect missing license file
    LicenseError res = Tucuxi::Common::LicenseManager::checkLicenseFile(fileName);
    ASSERT_EQ(res, Tucuxi::Common::LicenseError::MISSING_LICENSE_FILE);

    // Generate a request to get a new license file
    std::string request;
    res = Tucuxi::Common::LicenseManager::generateRequestString(request, "1.0");
    ASSERT_EQ(res, Tucuxi::Common::LicenseError::REQUEST_SUCCESSFUL);
    std::cout << "Request string: " << request << std::endl;

    std::string decryptedRequest;
    if (!Tucuxi::Common::CryptoHelper::decrypt(m_key, m_licenses[0], decryptedRequest)) {
        testing::AssertionFailure() << "Error encrypt failed.";
    }
    std::cout << "Decrypted request: " << decryptedRequest << std::endl;

    // Install a new license file
    res = Tucuxi::Common::LicenseManager::installLicense(m_licenses[0], fileName);
    ASSERT_EQ(res, Tucuxi::Common::LicenseError::INSTALLATION_SUCCESSFUL);

    // Check license file
    res = Tucuxi::Common::LicenseManager::checkLicenseFile(fileName);
    ASSERT_EQ(res, Tucuxi::Common::LicenseError::VALID_LICENSE);

    // Check license file
    res = Tucuxi::Common::LicenseManager::checkLicenseFile(fileName);
    ASSERT_EQ(res, Tucuxi::Common::LicenseError::VALID_LICENSE);
}

TEST (Crypto_TestLicenseManager, CheckInvalidLicense){
    LicenseError res = LicenseError::INVALID_LICENSE;
    testLicenseManager(appFolder);

    for (int i = 1; i < 6; i++) {

        std::string test;
        if (!Tucuxi::Common::CryptoHelper::decrypt(m_key, m_licenses[i], test)) {
            testing::AssertionFailure() << "Error encrypt failed.";
        }

        std::cout << "Test " << i << " : " << test << std::endl;

        // Make sure to remove delete licence file.
        const std::string& fileName = Tucuxi::Common::Utils::strFormat("%s/%s", m_path.c_str(), "license.txt");
        std::remove(fileName.c_str());

        // Install a invalid license file (wrong type)
        Tucuxi::Common::LicenseManager::installLicense(m_licenses[i], fileName);
        ASSERT_EQ(res, Tucuxi::Common::LicenseError::INVALID_LICENSE);

        // Write result in license file
        std::ofstream file(fileName);
        if (!file.is_open()) {
            testing::AssertionFailure() << "Cannot write license file.";
        }
        file << m_licenses[i];
        file.close();

        // Check license file
        res = Tucuxi::Common::LicenseManager::checkLicenseFile(fileName);
        ASSERT_EQ(res, Tucuxi::Common::LicenseError::INVALID_LICENSE);

        // Check license file
        res = Tucuxi::Common::LicenseManager::checkLicenseFile(fileName);
        ASSERT_EQ(res, Tucuxi::Common::LicenseError::INVALID_LICENSE);
    }
}
