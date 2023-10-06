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


#include "xmlattribute.h"

#include "rapidxml.hpp"
#include "xmlnode.h"

namespace Tucuxi {
namespace Common {

XmlAttribute::XmlAttribute() : m_pAttribute(nullptr) {}


XmlAttribute::XmlAttribute(const XmlAttribute& _node) : m_pAttribute(_node.m_pAttribute) {}


XmlAttribute::XmlAttribute(rapidxml::xml_attribute<>* _pAttribute)
{
    m_pAttribute = _pAttribute;
}


XmlNode XmlAttribute::getNode()
{
    rapidxml::xml_node<>* pNode = nullptr;
    if (m_pAttribute != nullptr) {
        pNode = m_pAttribute->parent();
    }
    return XmlNode(pNode);
}


std::string XmlAttribute::getName() const
{
    if (m_pAttribute != nullptr) {
        return m_pAttribute->name();
    }
    return "";
}


bool XmlAttribute::setName(const std::string& _name)
{
    char* pstr = allocateString(_name);
    if (pstr != nullptr) {
        m_pAttribute->name(pstr);
        return true;
    }
    return false;
}


std::string XmlAttribute::getValue() const
{
    if (m_pAttribute != nullptr) {
        return m_pAttribute->value();
    }
    return "";
}


bool XmlAttribute::setValue(const std::string& _value)
{
    char* pstr = allocateString(_value);
    if (pstr != nullptr) {
        m_pAttribute->value(pstr);
        return true;
    }
    return false;
}


bool XmlAttribute::isValid() const
{
    return (m_pAttribute != nullptr);
}


XmlAttribute& XmlAttribute::operator=(const XmlAttribute& _other)
{
    m_pAttribute = _other.m_pAttribute;
    return *this;
}


bool XmlAttribute::operator==(const XmlAttribute& _other) const
{
    return m_pAttribute == _other.m_pAttribute;
}


bool XmlAttribute::operator!=(const XmlAttribute& _other) const
{
    return m_pAttribute != _other.m_pAttribute;
}


XmlAttribute XmlAttribute::next() const
{
    rapidxml::xml_attribute<>* pAttribute = nullptr;
    if (m_pAttribute != nullptr) {
        pAttribute = m_pAttribute->next_attribute();
    }
    return XmlAttribute(pAttribute);
}


char* XmlAttribute::allocateString(const std::string& _string)
{
    if (m_pAttribute != nullptr) {
        try {
            return m_pAttribute->document()->allocate_string(_string.c_str());
        }
        catch (std::bad_alloc&) {
        }
    }
    return nullptr;
}

} // namespace Common
} // namespace Tucuxi
