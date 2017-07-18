/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "rapidxml.hpp"

#include "xmlnode.h"
#include "xmlattribute.h"

namespace Tucuxi {
namespace Common {

XmlNode::XmlNode()
    : m_pNode(nullptr)
{
}


XmlNode::XmlNode(const XmlNode& _other)
    : m_pNode(_other.m_pNode)
{
}


XmlNode::XmlNode(rapidxml::xml_node<>* _pNode) 
    : m_pNode(_pNode)
{    
}


XmlNode::~XmlNode()
{
}


std::string XmlNode::getName() const
{
    if (m_pNode != nullptr) {
        return m_pNode->name();
    }
    return "";
}


bool XmlNode::setName(const std::string& _name) 
{
    char *pstr = allocateString(_name);
    if (pstr != nullptr) {
        m_pNode->name(pstr);
        return true;
    }
    return false;
}


std::string XmlNode::getValue() const
{
    if (m_pNode != nullptr) {
        return m_pNode->value();
    }
    return "";
}


bool XmlNode::setValue(const std::string& _value)
{
    char *pstr = allocateString(_value);
    if (pstr != nullptr) {
        m_pNode->name(pstr);
        return true;
    }
    return false;
}


EXmlNodeType XmlNode::getType() const
{
    if (m_pNode != nullptr) {
        switch (m_pNode->type()) {
            case rapidxml::node_document:    return EXmlNodeType::Document;
            case rapidxml::node_element:     return EXmlNodeType::Element;
            case rapidxml::node_data:        return EXmlNodeType::Data;
            case rapidxml::node_cdata:       return EXmlNodeType::CData;
            case rapidxml::node_comment:     return EXmlNodeType::Comment;
            case rapidxml::node_declaration: return EXmlNodeType::Declaration;
            case rapidxml::node_doctype:     return EXmlNodeType::Doctype;
            case rapidxml::node_pi:          return EXmlNodeType::Pi;
            default:                         break;
        }
    }
    return EXmlNodeType::Undefined;
}


XmlAttribute XmlNode::getAttribute(const std::string& _name) const
{
    rapidxml::xml_attribute<> *pAttr = nullptr;
    if (m_pNode != nullptr) {
        pAttr = m_pNode->first_attribute(_name.c_str());
    }
    return XmlAttribute(pAttr);
}


XmlAttributeIterator XmlNode::getAttributes() const
{
    rapidxml::xml_attribute<>* pFirst = nullptr;
    if (m_pNode != nullptr) {
        pFirst = m_pNode->first_attribute();
    }
    return XmlAttributeIterator(XmlAttribute(pFirst));
}


XmlNodeIterator XmlNode::getChildren() const
{
    rapidxml::xml_node<>* pFirst = nullptr;
    if (m_pNode != nullptr) {
        pFirst = m_pNode->first_node();
    }
    return XmlNodeIterator(XmlNode(pFirst));
}

/*
XmlNodeIterator getChildren(EXmlNodeType) const
{
}


XmlNodeIterator getChildren(const std::string& _name) const
{
}
*/

XmlNode XmlNode::getParent() const
{
    rapidxml::xml_node<>* pParent = nullptr;
    if (m_pNode != nullptr) {
        pParent = m_pNode->parent();
    }
    return XmlNode(pParent);
}


bool XmlNode::addChild(XmlNode _child)
{
    if (m_pNode != nullptr && _child.isValid()) {
        m_pNode->append_node(_child.m_pNode);
        return true;
    }
    return false;
}


bool XmlNode::addAttribute(XmlAttribute _attribute)
{
    if (m_pNode != nullptr && _attribute.isValid()) {
        m_pNode->append_attribute(_attribute.m_pAttribute);
        return true;
    }
    return false;
}


bool XmlNode::isValid() const
{
    return (m_pNode != nullptr);
}


XmlNode& XmlNode::operator=(const XmlNode& _other)
{
    m_pNode = _other.m_pNode;
    return *this;
}


bool XmlNode::operator==(const XmlNode& _other) const
{
    return m_pNode == _other.m_pNode;
}


bool XmlNode::operator!=(const XmlNode& _other) const
{
    return m_pNode != _other.m_pNode;
}


XmlNode XmlNode::next() const
{
    rapidxml::xml_node<>* pSibling = nullptr;
    if (m_pNode != nullptr) {
        pSibling = m_pNode->next_sibling();
    }
    return XmlNode(pSibling);
}


char* XmlNode::allocateString(const std::string& _string)
{
    if (m_pNode != nullptr) {
        try {
            return m_pNode->document()->allocate_string(_string.c_str());
        }
        catch (std::bad_alloc) {
        }
    }
    return nullptr;
}

}
}