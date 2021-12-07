/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "tucucommon/componentmanager.h"

namespace Tucuxi {
namespace Common {

ComponentManager::ComponentManager() = default;

ComponentManager* ComponentManager::getInstance()
{
    static ComponentManager mgr;
    return &mgr;
}


void ComponentManager::registerComponentFactory(const std::string& _name, TComponentFactory _pFactory)
{
    m_factories[_name] = _pFactory;
}


void ComponentManager::registerComponent(const std::string& _name, Interface* _pComponent)
{
    if (_pComponent != nullptr) {
        m_components[_name] = _pComponent;
    }
}


void ComponentManager::unregisterComponent(const std::string& _name)
{
    m_components.erase(_name);
}

} // namespace Common
} // namespace Tucuxi
