/*
 * Copyright (C) 2017 Tucuxi SA
 */

#ifndef TUCUXI_TUCUCOMMON_FINGERPRINT_H
#define TUCUXI_TUCUCOMMON_FINGERPRINT_H

#include <string.h>

namespace Tucuxi {
namespace Common {

enum class MachineIdType {CPU, MOTHERBOARD, BIOS, PRODUCT, NETWORK, NAME, ERROR};

class SystemInfo
{
public:
    static std::string retrieveFingerPrint(MachineIdType _machineIdType);

private:
    static std::string retrieveCpu();
    static std::string retrieveBios();
    static std::string retrieveMotherboard();
    static std::string retrieveProduct();
    static std::string retrieveNetwork();
    static std::string retrieveName();

    static std::string readDMIfile(std::string _filename);
};

}
}

#endif // TUCUXI_TUCUCOMMON_FINGERPRINT_H
