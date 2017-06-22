#ifndef TUCUXI_TUCUCOMMON_INTERFACE_H
#define TUCUXI_TUCUCOMMON_INTERFACE_H

#include <string>

namespace Tucuxi {
namespace Common {

class Interface
{
public:
	template <class T> T* getInterface() 
	 {
		std::string name = typeid(T*).name();
		Interface *itf = getInterface(name);
		if (itf != nullptr) {
			return static_cast<T*>(itf); // Note that the actual "dynamic_cast" is done by components when calling Component::registerInterface !!!
		}
		return nullptr;
	}

protected:
	virtual Interface* getInterface(const std::string &_name) = 0;
	friend class Component;
	friend class ComponentManager;
};

}
}

#endif // TUCUXI_TUCUCOMMON_INTERFACE_H