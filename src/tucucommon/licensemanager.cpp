/*
 * Copyright (C) 2017 Tucuxi SA
 */

#include <iostream>
#include <fstream>

#include "tucucommon/licensemanager.h"

namespace Tucuxi {
namespace Common {

//LicenseChecker::public_key = std::string("1235");
//LicenseChecker::private_key = std::string("1235");

LicenseManager::LicenseManager() {

    // Create machine id
    std::string fingerprint;

    for (int i = CPU; i != ERROR; i++)
    {
        fingerprint = SystemInfo::retrieveFingerPrint(static_cast<MachineIdType>(i));

        if(!fingerprint.empty())
        {
            m_machineId.m_type = static_cast<MachineIdType>(i);
            m_machineId.m_fingerprint = fingerprint;
        }
    }
}

int LicenseManager::checklicense(std::string _filename)
{
    MachineId idfromLicense;

    // Read content of license file
    std::string content;
    std::ifstream license(_filename);

    if (!license.is_open()) {
        // Error : File not found.
        return -1;
    }

    std::getline(license, content);

    license.close();

    // Read the first field : MachineId type
    std::size_t field1 = content.find(":");
    if(field1 == std::string::npos) {
        // Error : Missing field.
        return -2;
    }
    idfromLicense.m_type = static_cast<MachineIdType>(std::stoi(content.substr(0, field1)));

    // Read the second field : MachineId fingerprint
    std::size_t field2 = content.find(":", field1);
    if (field2 == std::string::npos) {
        // Error : Missing field.
        return -2;
    }
    idfromLicense.m_fingerprint = std::stoi(content.substr(field1, field2));

    // Read the third field : Date of creation
    // std::size_t field3 = content.find(":", field2);
    // if (field2 == std::string::npos) {
    //    // Error : Missing field.
    //    return -1;
    // }
    // Date date = ..;

    // Read the fourth field : Duration of validity
    // std::size_t field4 = content.find(":", field3);
    // if (field3 == std::string::npos) {
    //    // Error : Missing field.
    //     return -1;
    // }
    // Date duration = ..;

    // Check the license
    if(idfromLicense.m_type == m_machineId.m_type &&
            idfromLicense.m_fingerprint == m_machineId.m_fingerprint /* &&
            m_today < date + duration */) {

        // License is valide
        return 1;
    }
    else {
        // License is not valide
        return 0;
    }

}

int LicenseManager::installLicense(std::string _filename)
{
    std::ofstream license(_filename);

    if (!license.is_open()) {
        // Error : Cannot create lisence file.
        return -1;
    }

    license << m_machineId.m_type;
    license << ":";
    license << m_machineId.m_fingerprint;
    license << ":";
    //license << m_today;
    //license << ":";
    //license << duration;

    license.close();

    return 0;
}

int generateRequestString(std::string* request)
{
    return 0;
}

}
}
