/*
 * Copyright (C) 2017 Tucuxi SA
 */

#include <iostream>
#include <fstream>
#include <ctime>

#include "tucucommon/licensemanager.h"
#include "tucucommon/datetime.h"
#include "tucucommon/cryptohelper.h"

namespace Tucuxi {
namespace Common {

const std::string LicenseManager::m_key = "86685E7AA62844102FC7FAD5D6DDF46C9CA7777BF4E0153FDF5F86463EAC0D75";

int LicenseManager::checkLicenseFile(std::string _filename)
{
    // Read content of license file
    std::ifstream license(_filename);

    if (!license.is_open()) {
        return MISSING_LICENSE_FILE;
    }

    std::string content;
    std::getline(license, content);

    license.close();

    // Check license file
    int res = checklicense(content);

    // Update date last used in file
    if(res == VALID_LICENSE) {
        rewriteLicense(content, _filename);
    }

    return res;
}

int LicenseManager::checklicense(std::string _license)
{
    if(!CryptoHelper::decrypt(LicenseManager::m_key, _license, &_license))
        return ERROR_CRYPTO;

    // Read license : license:0:ABCDABCDABCDABCDABCDABCD:yyyy/mm/dd:yyyy/mm/dd
    // Read license : key_word:type:fingerprint:end validity date:last used date
    std::size_t field1 = _license.find(":");
    std::size_t field2 = _license.find(":", field1 + 1);
    std::size_t field3 = _license.find(":", field2 + 1);
    std::size_t field4 = _license.find(":", field3 + 1);

    // Check missing fields in file
    if(field1 == std::string::npos ||
            field2 == std::string::npos ||
            field3 == std::string::npos ||
            field4 == std::string::npos) {

        std::cout << "Error : Missing fields in file." << std::endl;
        return LicenseError::INVALID_LICENSE;
    }

    // Check 1st field == license
    if(_license.substr(0, field1) != "license") {
        std::cout << "Error : license = " << _license.substr(0, field1) << std::endl;
        return INVALID_LICENSE;
    }

    // Check 2th field == type machine id and check 3th field == machine id
    try {
        MachineIdType type = static_cast<MachineIdType>(std::stoi(_license.substr(field1 + 1, field2 - field1 - 1)));
        if(type == MachineIdType::ERROR) {
            std::cout << "Error type machine id = ERROR." << std::endl;
            return LicenseError::INVALID_LICENSE;
        }

        std::string hash;
        if(!CryptoHelper::hash(SystemInfo::retrieveFingerPrint(type), &hash)) {
            return ERROR_CRYPTO;
        }

        if(hash != _license.substr(field2 + 1, field3 - field2 - 1)) {
            std::cout << "Error hash are differents." << std::endl;
            return LicenseError::INVALID_LICENSE;
        }
    }
    catch (...) {
        std::cout << "Error bad machine id type conversion." << std::endl;
        return LicenseError::INVALID_LICENSE;
    }

    // Check 4th field == end validity date and check 5th field == last used date    
    time_t rawtime;
    time (&rawtime);

    struct tm* tmToday = localtime (&rawtime);

    struct tm tmEndValidity; // >= today
    if (!strptime(_license.substr(field3 + 1, field4 - field3 - 1).c_str(), "%Y-%m-%d", &tmEndValidity)) {
        std::cout << "Error bad datetime conversion." << std::endl;
        return LicenseError::INVALID_LICENSE;
    }

    struct tm tmLastUsed; // <= today
    if (!strptime(_license.substr(field4 + 1, _license.length()).c_str(), "%Y-%m-%d", &tmLastUsed)) {
        std::cout << "Error bad datetime conversion." << std::endl;
        return LicenseError::INVALID_LICENSE;
    }

    int today = tmToday->tm_yday + tmToday->tm_year;
    int endValidity = tmEndValidity.tm_yday + tmEndValidity.tm_year;
    int lastUsed = tmLastUsed.tm_yday + tmLastUsed.tm_year;

    if(endValidity >= today && lastUsed <= today) {
        return VALID_LICENSE;
    }
    else {
        std::cout << "Error date false." << std::endl;
        return INVALID_LICENSE;
    }
}

int LicenseManager::rewriteLicense(std::string _license, std::string _filename)
{
    if(!CryptoHelper::decrypt(LicenseManager::m_key, _license, &_license))
        return ERROR_CRYPTO;

    // Build license : license:0:ABCDABCDABCDABCDABCDABCD:yyyy/mm/dd:yyyy/mm/dd
    // Read last field of last used date
    std::size_t pos = _license.find_last_of(":");
    if(pos == std::string::npos) {
        return LicenseError::INVALID_LICENSE;
    }

    // Update date
    DateTime today;
    std::string lastUsed = ":";
    lastUsed += std::to_string(today.year());
    lastUsed += "-";
    lastUsed += std::to_string(today.month());
    lastUsed += "-";
    lastUsed += std::to_string(today.day());

    _license.replace(pos, _license.length(), lastUsed);

    if(!CryptoHelper::encrypt(LicenseManager::m_key, _license, &_license))
        return ERROR_CRYPTO;

    // Write result in license file
    std::ofstream file(_filename);

    if (!file.is_open()) {
        return MISSING_LICENSE_FILE;
    }

    file << _license;
    file.close();

    return INSTALLATION_SUCCESSFUL;
}

int LicenseManager::installLicense(std::string _request, std::string _filename)
{
    // Check license before installation
    int res = checklicense(_request);
    if(res != LicenseError::VALID_LICENSE) {
        return res;
    }

    // Write result in license file
    std::ofstream file(_filename);

    if (!file.is_open()) {
        return MISSING_LICENSE_FILE;
    }

    file << _request;
    file.close();

    return INSTALLATION_SUCCESSFUL;
}

int LicenseManager::generateRequestString(std::string* request)
{
    // Retrieve ID from machine
    MachineId idfromMachine;
    for (int i = CPU; i != ERROR; i++) {
        idfromMachine.m_fingerprint = SystemInfo::retrieveFingerPrint(static_cast<MachineIdType>(i));

        if(!idfromMachine.m_fingerprint.empty()) {
            idfromMachine.m_type = static_cast<MachineIdType>(i);
            break;
        }
    }

    if(idfromMachine.m_type == ERROR) {
        return NO_MACHINE_ID_FOUND;
    }

    // Get hash from fingerprint
    std::string iDhash;
    if(!CryptoHelper::hash(idfromMachine.m_fingerprint, &iDhash))
        return ERROR_CRYPTO;

    // Get date of day
    DateTime today;

    // Build request : request:type of machine id:id:Date of request:version app
    *request = "request";
    *request += ":";
    *request += std::to_string(idfromMachine.m_type);
    *request += ":";
    *request += iDhash;
    *request += ":";
    *request += std::to_string(today.year());
    *request += "-";
    *request += std::to_string(today.month());
    *request += "-";
    *request += std::to_string(today.day());
    *request += ":";
    *request += "1.0";

    if(!CryptoHelper::encrypt(LicenseManager::m_key, *request, request))
        return ERROR_CRYPTO;

    return REQUEST_SUCCESSFUL;
}


}
}
