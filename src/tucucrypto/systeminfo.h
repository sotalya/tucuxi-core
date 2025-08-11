/*
 * Tucuxi - Tucuxi-core library and command line tool.
 * This code allows to perform prediction of drug concentration in blood
 * and to propose dosage adaptations.
 * It has been developed by HEIG-VD, in close collaboration with CHUV.
 * Copyright (C) 2023 HEIG-VD, maintained by Yann Thoma  <yann.thoma@heig-vd.ch>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */


#ifndef TUCUXI_TUCUCOMMON_SYSTEMINFO_H
#define TUCUXI_TUCUCOMMON_SYSTEMINFO_H

/// \defgroup TucuCommon Tucuxi common library
/// \brief Common classes and components for Tucuxi applications
/// This module defines a set of basic and application independent classes and components for
/// use in other Tucuxi's libraries and applications.

#include <string>

namespace Tucuxi {
namespace Common {

enum class MachineIdType
{
    CPU,
    MOTHERBOARD,
    DISK,
    MAC,
    BIOS,
    PRODUCT,
    NAME,
    UNDEFINED
};

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
    /// \return Id, In case of error : the returned string is empty.
    static std::string retrieveFingerPrint(MachineIdType _machineIdType);

private:
    /// \brief Retrieve a unique id of CPU.
    /// \return Id, In case of error : the returned string is empty.
    static std::string retrieveCpu();

    /// \brief Retrieve a unique id of Motherboard.
    /// \return Id, In case of error : the returned string is empty.
    static std::string retrieveMotherboard();

    /// \brief Retrieve a unique id of Disks.
    /// \return Id, In case of error : the returned string is empty.
    static std::string retrieveDisk();

    /// \brief Retrieve Mac Address.
    /// \return Id, In case of error : the returned string is empty.
    static std::string retrieveMacAddress();

    /// \brief Retrieve some infos of BIOS.
    /// \return Id, In case of error : the returned string is empty.
    static std::string retrieveBios();

    /// \brief Retrieve some infos of product.
    /// \return Id, In case of error : the returned string is empty.
    static std::string retrieveProduct();

    /// \brief Retrieve the name of machine.
    /// \return Id, In case of error : the returned string is empty.
    static std::string retrieveName();

#ifndef _WIN32
    static std::string readDMIfile(std::string _filename);
#endif
};

} // namespace Common
} // namespace Tucuxi

#endif // TUCUXI_TUCUCOMMON_SYSTEMINFO_H
