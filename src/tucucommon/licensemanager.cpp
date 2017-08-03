/*
 * Copyright (C) 2017 Tucuxi SA
 */

#include <iostream>
#include <fstream>

#include "tucucommon/licensemanager.h"
#include "tucucommon/datetime.h"
#include "tucucommon/cryptohelper.h"

namespace Tucuxi {
namespace Common {

const std::string LicenseManager::m_key = "86685E7AA62844102FC7FAD5D6DDF46C9CA7777BF4E0153FDF5F86463EAC0D75";

int LicenseManager::checkLicenseFile(std::string _filename)
{
    // Read content of license file
    std::string content;
    std::ifstream license(_filename);

    if (!license.is_open()) {
        return LicenseError::MISSING_LICENSE;
    }

    std::getline(license, content);
    license.close();

    return checklicense(content);
}

int LicenseManager::installLicense(std::string _request, std::string _filename)
{
    if(checklicense(_request) == LicenseError::INVALID_LICENSE) {
        return LicenseError::INVALID_LICENSE;
    }

    std::ofstream license(_filename);

    if (!license.is_open()) {
        return CANNOT_CREATE_LICENSE;
    }

    license << _request;

    license.close();

    return 0;
}

int LicenseManager::generateRequestString(std::string* request)
{
    // Retrieve ID from machine
    MachineId idfromMachine;

    if(retrieveMachineID(&idfromMachine) < 0)
        return NO_MACHINE_ID_FOUND;

    std::string iDhash;
    if(!CryptoHelper::hash(idfromMachine.m_fingerprint, &iDhash))
        return ERROR_CRYPTO;

    DateTime today;

    // Build request : Type of id : ID : Date of request : version app
    *request = "request";
    *request += ":";
    *request += std::to_string(idfromMachine.m_type);
    *request += ":";
    *request += iDhash;
    *request += ":";
    *request += std::to_string(today.year());
    *request += "/";
    *request += std::to_string(today.month());
    *request += "/";
    *request += std::to_string(today.day());
    *request += ":";
    *request += "1.0";

    if(!CryptoHelper::encrypt(LicenseManager::m_key, *request, request))
        return ERROR_CRYPTO;

    return 0;
}

int LicenseManager::retrieveMachineID(MachineId* _machineId) {

    // Create machine id
    std::string fingerprint;

    for (int i = CPU; i != ERROR; i++) {

        fingerprint = SystemInfo::retrieveFingerPrint(static_cast<MachineIdType>(i));

        if(!fingerprint.empty()) {
            _machineId->m_type = static_cast<MachineIdType>(i);
            _machineId->m_fingerprint = fingerprint;
            return 0;
        }
    }

    return NO_MACHINE_ID_FOUND;
}

int LicenseManager::checklicense(std::string _license)
{
    // license:0:ABCDABCDABCDABCDABCDABCD:yyyy/mm/dd:yyyy/mm/dd

    std::size_t field1 = _license.find(":");
    std::size_t field2 = _license.find(":", field1);
    std::size_t field3 = _license.find(":", field2);
    std::size_t field4 = _license.find(":", field3);
    std::size_t field5 = _license.find(":", field4);

    if(field1 == std::string::npos ||
            field2 == std::string::npos ||
            field3 == std::string::npos ||
            field4 == std::string::npos ||
            field5 == std::string::npos) {

        return LicenseError::MISSING_FIELD;
    }

    // Check first field == license
    if(_license.substr(0, field1) != "license") {
        return INVALID_LICENSE;
    }

    // Check second and third field == type machine id and id
    try {
        MachineIdType type = static_cast<MachineIdType>(std::stoi(_license.substr(field1, field2)));

        if(type == MachineIdType::ERROR) {
            return LicenseError::INVALID_FIELD;
        }

        if(SystemInfo::retrieveFingerPrint(type) != _license.substr(field2, field3)) {
            return LicenseError::INVALID_FIELD;
        }
    }
    catch (...) {
        return LicenseError::INVALID_FIELD;
    }

//    DateTime lastUse(_license.substr(field3, field4)); // <= today

//    DateTime endValidity(_license.substr(field4, field5)); // >= today

    return VALID_LICENSE;
}

}
}
