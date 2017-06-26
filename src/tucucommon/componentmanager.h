#ifndef TUCUXI_TUCUCOMMON_COMPONENTMANAGER_H
#define TUCUXI_TUCUCOMMON_COMPONENTMANAGER_H

#include <typeinfo>
#include <string>
#include <map>

#include "tucucommon/interface.h"
#include "tucucommon/component.h"

namespace Tucuxi {
namespace Common {

typedef Interface* (*TComponentFactory)();

class ComponentManager
{
public:
	static ComponentManager* getInstance();

public:
	void registerComponentFactory(const std::string &_name, TComponentFactory _pFactory);	
	void registerComponent(const std::string &_name, Interface *_pComponent);
	void unregisterComponent(const std::string &_name);

	template <typename T> T* createComponent(const std::string &_factoryName)
	{
		if (m_factories.end() != m_factories.find(_factoryName))
		{
			Interface *cmp = m_factories[_factoryName]();
			if (cmp != nullptr) {
				return cmp->getInterface<T>();
			}
		}
		return nullptr;
	}

	template <typename T> T* getComponent(const std::string &_name)
	{
		std::string interfaceName = typeid(T*).name();
		if (m_components.end() != m_components.find(_name))
		{
			Interface* itf = m_components[_name];
			if (itf != nullptr) {
				return itf->getInterface<T>();
			}
		}
		return nullptr;
	}

private:
	ComponentManager();

private:
	std::map<std::string, TComponentFactory> m_factories;
	std::map<std::string, Interface*> m_components;
};

}
}

#endif // TUCUXI_TUCUCOMMON_COMPONENTMANAGER_H