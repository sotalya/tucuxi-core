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
