//@@lisence@@

#include <iomanip>
#include <iostream>
#include <sstream>

#ifdef _WIN32

#include <Windows.h>
#include <iphlpapi.h>
#include <stdlib.h>

#else

#include <fstream>
#include <unistd.h> // get mac address

#include <net/if.h>      // get mac address
#include <netinet/in.h>  // get mac address
#include <sys/ioctl.h>   // get mac address
#include <sys/utsname.h> // get name

#ifdef MACOS
#include <ifaddrs.h>

#include <net/if_dl.h>
#include <sys/socket.h>
#include <sys/types.h>
#endif

#endif

#include "tucucommon/utils.h"

#include "tucucrypto/systeminfo.h"

namespace Tucuxi {
namespace Common {

std::string SystemInfo::retrieveFingerPrint(MachineIdType _machineIdType)
{
    try {
        switch (_machineIdType) {
        case MachineIdType::CPU:
            return retrieveCpu();
        case MachineIdType::MOTHERBOARD:
            return retrieveMotherboard();
        case MachineIdType::DISK:
            return retrieveDisk();
        case MachineIdType::MAC:
            return retrieveMacAddress();
        case MachineIdType::BIOS:
            return retrieveBios();
        case MachineIdType::PRODUCT:
            return retrieveProduct();
        case MachineIdType::NAME:
            return retrieveName();
        default:
            break;
        };
    }
    catch (const std::exception& e) {
        std::cout << "Error in CryptoHelper::hash: " << e.what() << std::endl;
    }
    return "";
}

std::string SystemInfo::retrieveCpu()
{
#ifdef _WIN32
    return std::string();
#else
    // Retrieve the processor serial number
    unsigned int eax = 3, ebx = 0, ecx = 0, edx = 0;

    // clang-format off
    asm volatile("cpuid"
                 : "=a" (eax),
                 "=b" (ebx),
                 "=c" (ecx),
                 "=d" (edx)
                 : "0" (eax), "2" (ecx));
    // clang-format on

    if (edx == 0 && ecx == 0) {
        return std::string();
    }
    else {
        std::stringstream ss;
        ss << std::uppercase << "0x" << std::hex << edx << ecx;
        return ss.str();
    }
#endif
}

std::string SystemInfo::retrieveMotherboard()
{
#ifdef _WIN32
    return std::string();
#else
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
#endif
}

std::string SystemInfo::retrieveDisk()
{
#ifdef _WIN32
    std::stringstream ss_volume;

    // Volume ID
    char volumeName[MAX_PATH + 1] = {0};
    char fileSystemName[MAX_PATH + 1] = {0};
    DWORD serialNumber = 0;
    DWORD maxComponentLen = 0;
    DWORD fileSystemFlags = 0;

    if (GetVolumeInformationA(
                "C:\\",
                volumeName,
                sizeof(volumeName),
                &serialNumber,
                &maxComponentLen,
                &fileSystemFlags,
                fileSystemName,
                sizeof(fileSystemName))
        == TRUE) {
        ss_volume << volumeName << serialNumber << fileSystemName << maxComponentLen << fileSystemFlags;
    }

    return ss_volume.str();
#else
    return std::string();
#endif
}

std::string SystemInfo::retrieveMacAddress()
{
#ifdef _WIN32
    std::stringstream ss_mac;

    // Address MAC
    IP_ADAPTER_INFO AdapterInfo[16];
    DWORD dwBufLen = sizeof(AdapterInfo);
    GetAdaptersInfo(AdapterInfo, &dwBufLen);
    IP_ADAPTER_INFO* pAdapterInfo = AdapterInfo;

    FIXED_INFO AdapterFixedInfo[16];
    DWORD dwLen = sizeof(AdapterFixedInfo);
    GetNetworkParams(AdapterFixedInfo, &dwLen);

    do {
        std::string temp = pAdapterInfo->Description;

        if (temp.find("VirtualBox") == std::string::npos) {
            std::string macaddr = Utils::strFormat(
                    "%02x%02x%02x%02x%02x%02x",
                    pAdapterInfo->Address[0],
                    pAdapterInfo->Address[1],
                    pAdapterInfo->Address[2],
                    pAdapterInfo->Address[3],
                    pAdapterInfo->Address[4],
                    pAdapterInfo->Address[5]);
            ss_mac << macaddr;
        }

        pAdapterInfo = pAdapterInfo->Next;

    } while (pAdapterInfo);

    return ss_mac.str();

#elif defined MACOS
    struct ifaddrs *ifap, *ifaptr;
    unsigned char* ptr;
    if (getifaddrs(&ifap) == 0) {
        for (ifaptr = ifap; ifaptr != NULL; ifaptr = (ifaptr)->ifa_next) {
            if (ifaptr->ifa_addr->sa_family == AF_LINK && !strcmp(ifaptr->ifa_name, "en0")) {
                unsigned char* mac_address = ( unsigned char* )LLADDR(( struct sockaddr_dl* )(ifaptr)->ifa_addr);
                std::stringstream ss;
                ss << std::hex << std::setfill('0');
                ss << std::setw(2) << static_cast<unsigned>(mac_address[0]);
                ss << std::setw(2) << static_cast<unsigned>(mac_address[1]);
                ss << std::setw(2) << static_cast<unsigned>(mac_address[2]);
                ss << std::setw(2) << static_cast<unsigned>(mac_address[3]);
                ss << std::setw(2) << static_cast<unsigned>(mac_address[4]);
                ss << std::setw(2) << static_cast<unsigned>(mac_address[5]);
                freeifaddrs(ifap);
                return ss.str();
            }
        }
    }
    else {
        return std::string();
    }
#else
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

            if ((ifr.ifr_flags & IFF_LOOPBACK) == 0) { // don't count loopback

                if (ioctl(sock, SIOCGIFHWADDR, &ifr) == 0) {

                    unsigned char macAddress[6];
                    memcpy(macAddress, ifr.ifr_hwaddr.sa_data, 6);

                    std::stringstream ss;
                    ss << std::hex << std::setfill('0');
                    ss << std::setw(2) << static_cast<unsigned>(macAddress[0]);
                    ss << std::setw(2) << static_cast<unsigned>(macAddress[1]);
                    ss << std::setw(2) << static_cast<unsigned>(macAddress[2]);
                    ss << std::setw(2) << static_cast<unsigned>(macAddress[3]);
                    ss << std::setw(2) << static_cast<unsigned>(macAddress[4]);
                    ss << std::setw(2) << static_cast<unsigned>(macAddress[5]);
                    return ss.str();
                }
            }
        }
    }

    return std::string();
#endif
}

std::string SystemInfo::retrieveBios()
{
#ifdef _WIN32
    return "";
#else
    // Retrieve the infos of bios (vendor-date-version)
    std::stringstream ss;
    ss << readDMIfile(std::string("/sys/class/dmi/id/bios_date"));
    ss << readDMIfile(std::string("/sys/class/dmi/id/bios_version"));
    ss << readDMIfile(std::string("/sys/class/dmi/id/bios_vendor"));
    return ss.str();
#endif
}

std::string SystemInfo::retrieveProduct()
{
#ifdef _WIN32
    std::stringstream ss_hw;

    // Hardware Info
    SYSTEM_INFO siSysInfo;
    GetSystemInfo(&siSysInfo);

    // clang-format off
    ss_hw << "HW:" << siSysInfo.dwOemId <<
             siSysInfo.dwNumberOfProcessors <<
             siSysInfo.dwPageSize <<
             siSysInfo.dwProcessorType <<
             siSysInfo.lpMinimumApplicationAddress <<
             siSysInfo.lpMaximumApplicationAddress <<
             siSysInfo.dwActiveProcessorMask;
    // clang-format on

    return ss_hw.str();

#else
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
#endif
}

std::string SystemInfo::retrieveName()
{
#ifdef _WIN32
    std::stringstream ss_name;

    TCHAR computerName[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD size = sizeof(computerName) / sizeof(computerName[0]);
    GetComputerName(computerName, &size);

    for (size_t i = 0; i < size; i++) {
        ss_name << computerName[i];
    }

    return ss_name.str();
#else
    // Retrieve the infos of the machine
    struct utsname unameData;
    uname(&unameData);

    // Name of the hardware type the system is running on.
    // Name of this node on the network.
    std::stringstream ss;
    ss << unameData.nodename << unameData.machine;

    return ss.str();
#endif
}

#ifndef _WIN32
std::string SystemInfo::readDMIfile(std::string _filename)
{
    std::string content;
    std::ifstream file(_filename.c_str());

    if (file.is_open()) {
        std::getline(file, content);
    }

    file.close();

    // Some content of dmi file can be fill with space character.
    for (size_t i = 0; i < content.size(); i++) {
        if (isblank(content[i]) == 0) {
            return content;
        }
    }

    return std::string();
}
#endif

} // namespace Common
} // namespace Tucuxi
