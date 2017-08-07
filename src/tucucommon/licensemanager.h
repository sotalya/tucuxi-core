/*
 * Copyright (C) 2017 Tucuxi SA
 */

#ifndef TUCUXI_TUCUCOMMON_LICENSECHECKER_H
#define TUCUXI_TUCUCOMMON_LICENSECHECKER_H

#include <string.h>

#include "tucucommon/systeminfo.h"

namespace Tucuxi {
namespace Common {

enum class LicenseError {
    VALID_LICENSE = 1,
    INVALID_LICENSE = 0,
    REQUEST_SUCCESSFUL = 0,
    INSTALLATION_SUCCESSFUL = 0,
    MISSING_LICENSE_FILE = -1,
    ERROR_CRYPTO = -2,
    NO_MACHINE_ID_FOUND = -3
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
    static int checklicense(std::string _request);
    static int rewriteLicense(std::string _license, std::string _filename);

private:
    static const std::string m_key;
};

}
}

#endif // TUCUXI_TUCUCOMMON_LICENSECHECKER_H
