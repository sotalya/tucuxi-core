/*
 * Copyright (C) 2017 Tucuxi SA
 */

#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <sstream>

// Use for getSerial
//#include <sys/types.h>
//#include <dirent.h>
//#include <errno.h>
#include <vector>
#include <fstream>

// Use for getMacAdress()
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>
#include <netinet/in.h>

// Use for getName()
#include <sys/utsname.h>

#include "tucucommon/fingerprint.h"

//#ifdef _WIN32
//// Windows
//#else
//// Linux
//#endif

namespace Tucuxi {
namespace Common {

std::string FingerPrint::retrieveFingerPrint(MachineIdType _machineIdType)
{
    switch(_machineIdType) {
        case CPU:
            return retrieveCpu();
        case BIOS:
            return retrieveBios();
        case MOTHERBOARD:
            return retrieveMotherboard();
        case PRODUCT:
            return retrieveProduct();
        case NETWORK:
            return retrieveNetwork();
        case NAME:
            return retrieveName();
    };

    return std::string();
}

std::string FingerPrint::retrieveCpu()
{
    // Retrieve the processor serial number
    unsigned int eax = 3, ebx = 0, ecx = 0, edx = 0;

    asm volatile("cpuid"
                 : "=a" (eax),
                 "=b" (ebx),
                 "=c" (ecx),
                 "=d" (edx)
                 : "0" (eax), "2" (ecx));

    if (edx == 0 && ecx == 0) {
        return std::string();
    }
    else {
        std::stringstream ss;
        ss << std::uppercase << "0x" << std::hex << edx << ecx;
        return ss.str();
    }
}

std::string FingerPrint::readDMIfile(std::string _filename)
{
    std::string content;
    std::ifstream file (_filename.c_str());

    if (file.is_open()) {
        std::getline (file, content);
    }

    file.close();

    // Some content of dmi file can be fill with space character.
    for(size_t i=0; i < content.size(); i++) {
        if(!isblank(content[i])) {
            return content;
        }
    }

    return std::string();
}

std::string FingerPrint::retrieveBios()
{
    // Retrieve the infos of bios (vendor-date-version)
    std::stringstream ss;
    ss << readDMIfile(std::string("/sys/class/dmi/id/bios_date"));
    ss << readDMIfile(std::string("/sys/class/dmi/id/bios_version"));
    ss << readDMIfile(std::string("/sys/class/dmi/id/bios_vendor"));
    return ss.str();
}

std::string FingerPrint::retrieveMotherboard()
{
    // Retrieve the infos of motherboard (name-vendor-version-serial number)
    std::stringstream ss;

    // Check root privileges
    if (getuid() == 0 || geteuid() == 0) {
        //ss << readDMIfile(std::string("/sys/class/dmi/id/board_name"));
        //ss << readDMIfile(std::string("/sys/class/dmi/id/board_vendor"));
        //ss << readDMIfile(std::string("/sys/class/dmi/id/board_version"));

        // Need root privileges
        ss << readDMIfile(std::string("/sys/class/dmi/id/board_serial"));
    }

    return ss.str();
}

std::string FingerPrint::retrieveProduct()
{
    // Retrieve the infos of product (version-serial-uuid)
    std::stringstream ss;

    // Check root privileges
    if (getuid() == 0 || geteuid() == 0) {
        //ss << readDMIfile(std::string("/sys/class/dmi/id/product_serial"));
        //ss << readDMIfile(std::string("/sys/class/dmi/id/product_version"));

        // Need root privileges
        ss << readDMIfile(std::string("/sys/class/dmi/id/product_uuid"));
    }

    return ss.str();
}

std::string FingerPrint::retrieveNetwork()
{
    // Retrieve the mac address of interfaces
    struct ifreq ifr;
    struct ifconf ifc;
    char buf[1024];

    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);

    if (sock == -1) {
        return std::string();
    }

    ifc.ifc_len = sizeof(buf);
    ifc.ifc_buf = buf;

    if (ioctl(sock, SIOCGIFCONF, &ifc) == -1) {
        return std::string();
    }

    struct ifreq* it = ifc.ifc_req;
    const struct ifreq* const end = it + (ifc.ifc_len / sizeof(struct ifreq));

    for (; it != end; ++it) {

        strcpy(ifr.ifr_name, it->ifr_name);

        if (ioctl(sock, SIOCGIFFLAGS, &ifr) == 0) {

            if (!(ifr.ifr_flags & IFF_LOOPBACK)) { // don't count loopback

                if (ioctl(sock, SIOCGIFHWADDR, &ifr) == 0) {

                    unsigned char mac_address[6];
                    memcpy(mac_address, ifr.ifr_hwaddr.sa_data, 6);

                    std::stringstream ss;
                    ss << std::hex << std::setfill('0');
                    ss << std::setw(2) << static_cast<unsigned>(mac_address[0]);
                    ss << ":";
                    ss << std::setw(2) << static_cast<unsigned>(mac_address[1]);
                    ss << ":";
                    ss << std::setw(2) << static_cast<unsigned>(mac_address[2]);
                    ss << ":";
                    ss << std::setw(2) << static_cast<unsigned>(mac_address[3]);
                    ss << ":";
                    ss << std::setw(2) << static_cast<unsigned>(mac_address[4]);
                    ss << ":";
                    ss << std::setw(2) << static_cast<unsigned>(mac_address[5]);
                    return ss.str();
                }
            }
        }
    }

    return std::string();
}

std::string FingerPrint::retrieveName()
{
    // Retrieve the infos of the machine
    struct utsname unameData;
    uname(&unameData);

    // Name of the hardware type the system is running on.
    // Name of this node on the network.
    std::stringstream ss;
    ss << unameData.nodename << unameData.machine;

    return ss.str();
}

}
}
