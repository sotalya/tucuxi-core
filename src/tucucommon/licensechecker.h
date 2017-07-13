/*
 * Copyright (C) 2017 Tucuxi SA
 */

#ifndef TUCUXI_TUCUCOMMON_LICENSECHECKER_H
#define TUCUXI_TUCUCOMMON_LICENSECHECKER_H

#include <string.h>
#include "tucucommon/fingerprint.h"

namespace Tucuxi {
namespace Common {

struct MachineId {
    std::string m_fingerprint;
    MachineIdType m_type;
};

class LicenseChecker
{
public:
    static int evalLicenseFile(std::string _filename, MachineId* _machineId);
    static int writeLicenseFile(std::string _filename, MachineId* _machineId);
    static int retrieveMachineID(MachineId* _machineId);
    static int readLicenseFile(std::string _filename, MachineId* _machineId);

//private:
//    static std::string public_key;  // server
//    static std::string private_key; // tucuxi
};

}
}

#endif // TUCUXI_TUCUCOMMON_LICENSECHECKER_H
