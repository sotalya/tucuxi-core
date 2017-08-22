/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_TUCUCOMMON_COMPONENT_H
#define TUCUXI_TUCUCOMMON_COMPONENT_H

#include <string>
#include <map>
#include <typeinfo>

#include "tucucommon/interface.h"

namespace Tucuxi {
namespace Common {

/// \ingroup TucuCommon
/// \brief Base class helping the implementation of Tucuxi's components.
///
/// This is a helper class focusing on making the implementation of components easier. Mainly it manages a map of 
/// interface pointers. Since users of components can only access components via their interfaces, using dynamic_cast
/// to switch interface is not possible has neither the compiler nor the runtime can understand the relation between a
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
    template <typename T> void registerInterface(T* _interface)
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
    Interface* getInterfaceImpl(const std::string &_name)
    {
        if (m_interfaces.end() != m_interfaces.find(_name)) {
            return m_interfaces[_name];
        }
        return nullptr;
    }

private:
    std::map<std::string, Interface*> m_interfaces;     /// The list of correctly initialized interface pointers
};

}
}

#endif // TUCUXI_TUCUCOMMON_COMPONENT_H
