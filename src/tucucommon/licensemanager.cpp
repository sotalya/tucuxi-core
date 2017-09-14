/*
 * Copyright (C) 2017 Tucuxi SA
 */

#include <iostream>
#include <fstream>

#include "tucucommon/loggerhelper.h"
#include "tucucommon/licensemanager.h"
#include "tucucommon/datetime.h"
#include "tucucommon/cryptohelper.h"

namespace Tucuxi {
namespace Common {

// Symmetric key between Tucuxi and Server
const std::string LicenseManager::m_key = "86685E7AA62844102FC7FAD5D6DDF46C9CA7777BF4E0153FDF5F86463EAC0D75";

LicenseError LicenseManager::checkLicenseFile(std::string _filename)
{
    LoggerHelper logger;

    // Read content of license file
    std::ifstream license(_filename);

    if (!license.is_open()) {
        logger.error("License file not found.");
        return LicenseError::MISSING_LICENSE_FILE;
    }

    std::string content;
    std::getline(license, content);

    license.close();

    // Check license file
    LicenseError res = checklicense(content);

    if (res != LicenseError::VALID_LICENSE) {
        return res;
    }

    // Update last used date in license file
    res = rewriteLicense(content, _filename);

    if (res == LicenseError::INSTALLATION_SUCCESSFUL) {
        return LicenseError::VALID_LICENSE;
    }
    else {
        return res;
    }
}

LicenseError LicenseManager::checklicense(std::string _license)
{
    LoggerHelper logger;

    // Decrypt content of licence file
    if(!CryptoHelper::decrypt(LicenseManager::m_key, _license, &_license)) {
        logger.error("Cannot decrypt licence file.");
        return LicenseError::ERROR_CRYPTO;
    }

    // Read license : license:0:ABCDABCDABCDABCDABCDABCD:yyyy/mm/dd:yyyy/mm/dd
    // Read license : key_word:type:fingerprint:end validity date:last used date
    std::size_t field1 = _license.find(":");
    std::size_t field2 = _license.find(":", field1 + 1);
    std::size_t field3 = _license.find(":", field2 + 1);
    std::size_t field4 = _license.find(":", field3 + 1);

    // Check the number of fields in license
    if(field1 == std::string::npos ||
            field2 == std::string::npos ||
            field3 == std::string::npos ||
            field4 == std::string::npos) {
        logger.error("License is invalid.");
        return LicenseError::INVALID_LICENSE;
    }

    // Check 1st field == license
    if(_license.substr(0, field1) != "license") {
        logger.error("License is invalid.");
        return LicenseError::INVALID_LICENSE;
    }

    try {
        // Check 2th field == type machine id
        MachineIdType type = static_cast<MachineIdType>(std::stoi(_license.substr(field1 + 1, field2 - field1 - 1)));

        if(type == MachineIdType::UNDEFINED) {
            logger.error("License is invalid.");
            return LicenseError::INVALID_LICENSE;
        }

        // Get hash of fingerprint from machine
        std::string hash;
        if(!CryptoHelper::hash(SystemInfo::retrieveFingerPrint(type), &hash)) {
            logger.error("Cannot encrypt licence file.");
            return LicenseError::ERROR_CRYPTO;
        }

        // Check 3th field == fingerprint valid
        if(hash != _license.substr(field2 + 1, field3 - field2 - 1)) {
            logger.error("License is invalid.");
            return LicenseError::INVALID_LICENSE;
        }
    }
    catch (...) {
        logger.error("License is invalid.");
        return LicenseError::INVALID_LICENSE;
    }

    try {
        // Get date of today
        std::string strToday = std::to_string(DateTime().year()) +
                std::to_string(DateTime().month()) +
                std::to_string(DateTime().day());

        int today = std::stoi(strToday);

        // Check 4th field == end validity date and check 5th field == last used date
        int endValidity = std::stoi(_license.substr(field3 + 1, field4 - field3 - 1));  // >= today
        int lastUsed = std::stoi(_license.substr(field4 + 1, _license.length()));       // <= today

        if(endValidity >= today && lastUsed <= today) {
            return LicenseError::VALID_LICENSE;
        }
        else {
            logger.error("License is invalid.");
            return LicenseError::INVALID_LICENSE;
        }
    }
    catch (...) {
        logger.error("License is invalid.");
        return LicenseError::INVALID_LICENSE;
    }
}

LicenseError LicenseManager::rewriteLicense(std::string _license, std::string _filename)
{
    LoggerHelper logger;

    // Decrypt content of licence file
    if(!CryptoHelper::decrypt(LicenseManager::m_key, _license, &_license)) {
        logger.error("Cannot decrypt licence file.");
        return LicenseError::ERROR_CRYPTO;
    }

    // Read license : key_word:type:fingerprint:end validity date:last used date

    // Get last fiedl (last used date)
    std::size_t pos = _license.find_last_of(":");
    if(pos == std::string::npos) {
        logger.error("License is invalid.");
        return LicenseError::INVALID_LICENSE;
    }

    // Update date
    std::string lastUsed = ":";

    lastUsed += std::to_string(DateTime().year()) +
            std::to_string(DateTime().month()) +
            std::to_string(DateTime().day());

    _license.replace(pos, _license.length(), lastUsed);

    // Encrypt content of licence file
    if(!CryptoHelper::encrypt(LicenseManager::m_key, _license, &_license)) {
        logger.error("Cannot encrypt licence file.");
        return LicenseError::ERROR_CRYPTO;
    }

    // Write result in license file
    std::ofstream file(_filename);

    if (!file.is_open()) {
        logger.error("Cannot decrypt licence file.");
        return LicenseError::MISSING_LICENSE_FILE;
    }

    file << _license;
    file.close();

    return LicenseError::INSTALLATION_SUCCESSFUL;
}

LicenseError LicenseManager::installLicense(std::string _request, std::string _filename)
{
    LoggerHelper logger;

    // Check license before installation
    LicenseError res = checklicense(_request);
    if(res != LicenseError::VALID_LICENSE) {
        return res;
    }

    // Write result in license file
    std::ofstream file(_filename);

    if (!file.is_open()) {
        logger.error("License file not found.");
        return LicenseError::MISSING_LICENSE_FILE;
    }

    file << _request;
    file.close();

    return LicenseError::INSTALLATION_SUCCESSFUL;
}

LicenseError LicenseManager::generateRequestString(std::string* request)
{
    LoggerHelper logger;

    // Retrieve fingerprint ID from machine
    MachineId idfromMachine;

    for (int i = int(MachineIdType::CPU); i != int(MachineIdType::UNDEFINED); i++) {

        idfromMachine.m_fingerprint = SystemInfo::retrieveFingerPrint(static_cast<MachineIdType>(i));

        if(!idfromMachine.m_fingerprint.empty()) {
            idfromMachine.m_type = static_cast<MachineIdType>(i);
            break;
        }
    }

    // Check if a valid ID is found
    if(idfromMachine.m_type == MachineIdType::UNDEFINED) {
        logger.error("No machine id found.");
        return LicenseError::NO_MACHINE_ID_FOUND;
    }

    // Get hash from fingerprint
    std::string iDhash;
    if(!CryptoHelper::hash(idfromMachine.m_fingerprint, &iDhash)) {
        logger.error("Cannot generate hash from fingerprint.");
        return LicenseError::ERROR_CRYPTO;
    }

    // Get date of today
    DateTime today;

    // Build request : request:type of machine id:id:Date of request:version app
    *request = "request";
    *request += ":";
    *request += std::to_string(int(idfromMachine.m_type));
    *request += ":";
    *request += iDhash;
    *request += ":";
    *request += std::to_string(today.year()) + "-" + std::to_string(today.month()) + "-" + std::to_string(today.day());
    *request += ":";
    *request += "1.0";

    // Encrypt content of licence file
    if(!CryptoHelper::encrypt(LicenseManager::m_key, *request, request)) {
        logger.error("Cannot encrypt license file.");
        return LicenseError::ERROR_CRYPTO;
    }

    return LicenseError::REQUEST_SUCCESSFUL;
}


}
}
