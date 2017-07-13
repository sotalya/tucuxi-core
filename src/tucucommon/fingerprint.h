/*
 * Copyright (C) 2017 Tucuxi SA
 */

#ifndef TUCUXI_TUCUCOMMON_FINGERPRINT_H
#define TUCUXI_TUCUCOMMON_FINGERPRINT_H

#include <string.h>

namespace Tucuxi {
namespace Common {

enum MachineIdType {CPU, MOTHERBOARD, BIOS, PRODUCT, NETWORK, NAME};

class FingerPrint
{
public:
    static std::string retrieveFingerPrint(MachineIdType _machineIdType);
    static std::string retrieveCpu();
    static std::string retrieveBios();
    static std::string retrieveMotherboard();
    static std::string retrieveProduct();
    static std::string retrieveNetwork();
    static std::string retrieveName();

private:
    static std::string readDMIfile(std::string _filename);
};

}
}

#endif // TUCUXI_TUCUCOMMON_FINGERPRINT_H
