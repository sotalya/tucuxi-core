/*
 * Copyright (C) 2017 Tucuxi SA
 */

#ifndef TUCUXI_TUCUCOMMON_LICENSECHECKER_H
#define TUCUXI_TUCUCOMMON_LICENSECHECKER_H

#include <string.h>

#include "tucucommon/systeminfo.h"

namespace Tucuxi {
namespace Common {

enum LicenseError {
    REQUEST_SUCCESSFUL = 0,
    INSTALLATION_SUCCESSFUL = 0,
    INVALID_LICENSE = 0,
    VALID_LICENSE = 1,
    MISSING_LICENSE = -1,
    CANNOT_CREATE_LICENSE = -2,
    MISSING_FIELD = -3,
    INVALID_FIELD = -4,
    NO_MACHINE_ID_FOUND = -5,
    ERROR_CRYPTO = -6
};

struct MachineId {
    std::string m_fingerprint;
    MachineIdType m_type;
};

class LicenseManager
{
public:
    static int checkLicenseFile(std::string _filename);
    static int installLicense(std::string _license, std::string _filename);
    static int generateRequestString(std::string* _request);

private:
    static int retrieveMachineID(MachineId* _machineId);
    static int checklicense(std::string _request);

private:
    static const std::string m_key;
};

}
}

#endif // TUCUXI_TUCUCOMMON_LICENSECHECKER_H
