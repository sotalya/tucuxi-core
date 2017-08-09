/*
 * Copyright (C) 2017 Tucuxi SA
 */

#ifndef TUCUXI_TUCUCOMMON_SYSTEMINFO_H
#define TUCUXI_TUCUCOMMON_SYSTEMINFO_H

#include <string.h>

namespace Tucuxi {
namespace Common {

enum class MachineIdType {CPU, MOTHERBOARD, DISK, MAC, BIOS, PRODUCT, NAME, UNDEFINED};

class SystemInfo
{
public:
    static std::string retrieveFingerPrint(MachineIdType _machineIdType);

private:
    static std::string retrieveCpu();
    static std::string retrieveMotherboard();
    static std::string retrieveDisk();
    static std::string retrieveMacAddress();
    static std::string retrieveBios();
    static std::string retrieveProduct();
    static std::string retrieveName();

#ifndef _WIN32
    static std::string readDMIfile(std::string _filename);
#endif

};

}
}

#endif // TUCUXI_TUCUCOMMON_SYSTEMINFO_H
