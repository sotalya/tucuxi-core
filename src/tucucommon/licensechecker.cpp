/*
 * Copyright (C) 2017 Tucuxi SA
 */

#include <iostream>
#include <fstream>

#include "tucucommon/licensechecker.h"

namespace Tucuxi {
namespace Common {

//LicenseChecker::public_key = std::string("1235");
//LicenseChecker::private_key = std::string("1235");

int LicenseChecker::evalLicenseFile(std::string _filename, MachineId* _machineId)
{
    MachineId machineIdfromLicense;

    if(readLicenseFile(_filename, &machineIdfromLicense) == 0) {

        if(machineIdfromLicense.m_type == _machineId->m_type &&
                machineIdfromLicense.m_fingerprint == _machineId->m_fingerprint)
        {
            if(true/*machineIdfromLicense.date < _machineId.date + duration*/)
            {
                return 1;
            }
            else
            {
                return 2;
            }
        }
        else
        {
            return 3;
        }
    }

    return -1;
}

int LicenseChecker::writeLicenseFile(std::string _filename, MachineId* _machineId)
{
    std::ofstream license(_filename);

    if (license.is_open())
    {
        license << _machineId->m_type;
        license << ":";
        license << _machineId->m_fingerprint;
        license << ":";
        //Date

        license.close();
        return 0;
    }

   license.close();
   return -1;
}

int LicenseChecker::retrieveMachineID(MachineId* _machineId)
{
    // Create machine id
    std::string fingerprint;

    for (int i = CPU; i != NAME; i++)
    {
        fingerprint = FingerPrint::retrieveFingerPrint(static_cast<MachineIdType>(i));

        if(!fingerprint.empty())
        {
            _machineId->m_type = static_cast<MachineIdType>(i);
            _machineId->m_fingerprint = fingerprint;
            return 0;
        }
    }

    return -1;
}

int LicenseChecker::readLicenseFile(std::string _filename, MachineId* _machineId)
{
    // Read content of license file
    std::string content;
    std::ifstream license(_filename);

    if (license.is_open()) {
        std::getline(license, content);
    }
    else {
        return -1;
    }

    license.close();

    std::size_t field1 = content.find(":");
    if(field1 == std::string::npos) {
        return -1;
    }

    _machineId->m_type = static_cast<MachineIdType>(std::stoi(content.substr(0, field1)));

    std::size_t field2 = content.find(":", field1);
    if (field2 == std::string::npos) {
        return -1;
    }

    _machineId->m_fingerprint = std::stoi(content.substr(field1, field2));

    return 0;
}


}
}
