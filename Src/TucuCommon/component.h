
#ifndef TUCUXI_TUCUCOMMON_COMPONENT_H
#define TUCUXI_TUCUCOMMON_COMPONENT_H

#include <string>
#include <map>

#include "interface.h"

namespace Tucuxi {
namespace Common {

class Component
{
protected:
	template <typename T> 
	void registerInterface(T*_interface)						// Called by component classes to create the map of interface pointers
	{
		std::string name = typeid(_interface).name();
		m_interfaces[name] = _interface;
	}

	Interface* getInterface(const std::string &_name)			// Actual implementation of switching from one interface to the other
	{
		if (m_interfaces.end() != m_interfaces.find(_name)) {
			return m_interfaces[_name];
		}
		return nullptr;
	}

private:
	std::map<std::string, Interface*> m_interfaces;				// Our list of correctly initialized interface pointers
};

}
}

#endif // TUCUXI_TUCUCOMMON_COMPONENT_H
