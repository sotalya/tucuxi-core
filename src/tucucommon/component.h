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


#ifndef TUCUXI_TUCUCOMMON_COMPONENT_H
#define TUCUXI_TUCUCOMMON_COMPONENT_H

#include <map>
#include <string>
#include <typeinfo>

#include "tucucommon/interface.h"

namespace Tucuxi {
namespace Common {

/// \ingroup TucuCommon
/// \brief Base class helping the implementation of Tucuxi's components.
///
/// This is a helper class focusing on making the implementation of components easier. Mainly it manages a map of
/// interface pointers. Since users of components can only access components via their interfaces, using dynamic_cast
/// to switch interface is not possible as neither the compiler nor the runtime can understand the relation between a
/// component and its interfaces. As a result, switching from one interface is only possible by using
/// Interface::getInterface. The actual implementation of getInterface is provided by the component itself via
/// dynamic_cast. In order to simplify this code, Component offers methods registerInterface and getInterfaceImpl.
///
/// \code
///	class TestComponent : public Tucuxi::Common::Component, Interface1, Interface2
///	{
///	    ...
///	protected:
///	    virtual Tucuxi::Common::Interface* getInterface(const std::string &_name)
///	    {
///	        return Tucuxi::Common::Component::getInterfaceImpl(_name);
///	    }
///	private:
///	    TestComponent() {
///	        registerInterface(dynamic_cast<Interface1*>(this));
///	        registerInterface(dynamic_cast<Interface2*>(this));
///	    }
///	};
/// \endcode
///
/// \sa Interface, ComponentManager
class Component
{
protected:
    /// \brief Allows a component to store the correct pointer to the given interface.
    /// \param _interface The interface. The calling component should use "dynamic_cast" to pass the correct pointer.
    /// \return None
    template<typename T>
    void registerInterface(T* _interface)
    {
        std::string name = typeid(_interface).name();
        m_interfaces[name] = _interface;
    }

    /// \brief Helper method to simplify the implementation of Interface::getInterface.
    /// The name to pass to this function is the name of the interface as expressed by the typeid function. Note that
    /// this is actually transparent to users if they take advantage of the templated version of Interface::getInterface.
    /// \code
    /// Interface1 i1 = ...;
    /// Interface2 i2 = i1.getInterface<Interface2>();   // Calling getInterface with the correct name is done automatically.
    /// \endcode
    /// \param _name The name of the interface to retrieve.
    /// \return None
    Interface* getInterfaceImpl(const std::string& _name)
    {
        if (m_interfaces.end() != m_interfaces.find(_name)) {
            return m_interfaces[_name];
        }
        return nullptr;
    }

private:
    std::map<std::string, Interface*> m_interfaces; /// The list of correctly initialized interface pointers
};

} // namespace Common
} // namespace Tucuxi

#endif // TUCUXI_TUCUCOMMON_COMPONENT_H
