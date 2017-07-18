/*
 * Copyright (C) 2017 Tucuxi SA
 */

#ifndef TUCUXI_TUCUCOMMON_LICENSECHECKER_H
#define TUCUXI_TUCUCOMMON_LICENSECHECKER_H

#include <string.h>
#include "tucucommon/systeminfo.h"

namespace Tucuxi {
namespace Common {

struct MachineId {
    std::string m_fingerprint;
    MachineIdType m_type;
};

class LicenseManager
{
public:
    LicenseManager();
    int checklicense(std::string _filename);
    int installLicense(std::string _filename);
    int generateRequestString(std::string _filename);

private:
    //    static std::string public_key;  // server
    //    static std::string private_key; // tucuxi

    // System info
    MachineId m_machineId;
    // Date m_today;
};

}
}

#endif // TUCUXI_TUCUCOMMON_LICENSECHECKER_H
