/*
 * Copyright (C) 2017 Tucuxi SA
 */

#ifndef TUCUXI_TUCUCOMMON_SYSTEMINFO_H
#define TUCUXI_TUCUCOMMON_SYSTEMINFO_H

/// \defgroup TucuCommon Tucuxi common library
/// \brief Common classes and components for Tucuxi applications
/// This module defines a set of basic and application independent classes and components for
/// use in other Tucuxi's libraries and applications.

#include <string.h>
#include <string>

namespace Tucuxi {
namespace Common {

enum class MachineIdType {CPU, MOTHERBOARD, DISK, MAC, BIOS, PRODUCT, NAME, UNDEFINED};

/// \ingroup SystemInfo
/// \brief This class provides a unique id that identifies a machine.
///
/// CPU, MOTHERBOARD, DISK or MAC are strong ids. On the other hand, BIOS, PRODUCT or NAME may not be unique.
///
/// Sometimes it is not possible to find a particular id like CPU or MOTHERBOARD.
///
class SystemInfo
{
public:
    /// \brief Retrieve a unique id of Machine.
    /// @return Id, In case of error : the returned string is empty.
    static std::string retrieveFingerPrint(MachineIdType _machineIdType);

private:
    /// \brief Retrieve a unique id of CPU.
    /// @return Id, In case of error : the returned string is empty.
    static std::string retrieveCpu();

    /// \brief Retrieve a unique id of Motherboard.
    /// @return Id, In case of error : the returned string is empty.
    static std::string retrieveMotherboard();

    /// \brief Retrieve a unique id of Disks.
    /// @return Id, In case of error : the returned string is empty.
    static std::string retrieveDisk();

    /// \brief Retrieve Mac Address.
    /// @return Id, In case of error : the returned string is empty.
    static std::string retrieveMacAddress();

    /// \brief Retrieve some infos of BIOS.
    /// @return Id, In case of error : the returned string is empty.
    static std::string retrieveBios();

    /// \brief Retrieve some infos of product.
    /// @return Id, In case of error : the returned string is empty.
    static std::string retrieveProduct();

    /// \brief Retrieve the name of machine.
    /// @return Id, In case of error : the returned string is empty.
    static std::string retrieveName();

#ifndef _WIN32
    static std::string readDMIfile(std::string _filename);
#endif

};

}
}

#endif // TUCUXI_TUCUCOMMON_SYSTEMINFO_H
