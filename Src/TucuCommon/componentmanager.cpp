
#include "componentmanager.h"

namespace Tucuxi {
namespace Common {

ComponentManager::ComponentManager()
{
}

ComponentManager* ComponentManager::getInstance()
{
	static ComponentManager mgr;
	return &mgr;
}

void ComponentManager::registerComponentFactory(const std::string &_name, TComponentFactory _pFactory)
{
	m_factories[_name] = _pFactory;
}

void ComponentManager::registerComponent(const std::string &_name, Interface *_pComponent)
{
	m_components[_name] = _pComponent;
}

void ComponentManager::unregisterComponent(const std::string &_name)
{
	m_components.erase(_name);
}

}
}
