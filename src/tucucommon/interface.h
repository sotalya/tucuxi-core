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


#ifndef TUCUXI_TUCUCOMMON_INTERFACE_H
#define TUCUXI_TUCUCOMMON_INTERFACE_H

#include <string>
#include <typeinfo>

namespace Tucuxi {
namespace Common {

/// \ingroup TucuCommon
/// \brief Abstract class for all interfaces.
/// \sa Component, ComponentManager
class Interface
{
public:
    /// \brief The method to use to switch between interface of a given component.
    /// \param T The type of the desired interface
    /// \return A pointer to the specified interface
    template<class T>
    T* getInterface()
    {
        std::string name = typeid(T*).name();
        Interface* itf = getInterface(name);
        if (itf != nullptr) {
            // Note that the actual "dynamic_cast" is done by components when calling Component::registerInterface !!!
            return static_cast<T*>(itf);
        }
        return nullptr;
    }

protected:
    /// \brief Abstract method allowing the templated version of getInterface to access other interfaces of the same component.
    /// \param _name The name of the interface as returned by typeid.
    /// \return A pointer to the specified interface
    virtual Interface* getInterface(const std::string& _name) = 0;
};

} // namespace Common
} // namespace Tucuxi

#endif // TUCUXI_TUCUCOMMON_INTERFACE_H
