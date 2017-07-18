/*
* Copyright (C) 2017 Tucuxi SA
*/

#include <iostream>
#include <sstream>
#include <regex>

#include "rapidxml.hpp"
#include "rapidxml_print.hpp"

#include "xmldocument.h"
#include "xmlnode.h"
#include "xmlattribute.h"

namespace Tucuxi {
namespace Common {


XmlDocument::XmlDocument()
    : m_pDocument(nullptr)
{
}


XmlDocument::~XmlDocument()
{
    if (m_pDocument != nullptr) {
        delete m_pDocument;
    }
}


bool XmlDocument::isValid() const
{
    return (m_pDocument != nullptr);
}


bool XmlDocument::open(const std::string& _fileName)
{
    return false;
}


bool XmlDocument::save(const std::string& _fileName)
{
    return false;
}


bool XmlDocument::fromString(const std::string& _xml)
{
    m_rawXml = _xml;
    if (createDocument()) {
        try {
            m_pDocument->parse<0>(&m_rawXml[0]);
            return true;
        }
        catch (rapidxml::parse_error) {
            reset();
        }
    }
    return false;
}


bool XmlDocument::toString(std::string& _xml, bool _prettyPrint)
{
    if (m_pDocument != nullptr) {        
        rapidxml::print(std::back_inserter(_xml), *m_pDocument, _prettyPrint ? 0 : rapidxml::print_no_indenting);
        _xml = std::regex_replace(_xml, std::regex("\""), "'");
        return true;
    }
    return false;
}


XmlNode XmlDocument::getRoot() const
{
    rapidxml::xml_node<>* pRoot = nullptr;
    if (m_pDocument != nullptr) {
        pRoot = m_pDocument->first_node();
    }
    return XmlNode(pRoot);
}


bool XmlDocument::setRoot(const XmlNode& _root)
{
    if (_root.isValid()) {
        if (isValid()) {
            if (m_pDocument->first_node() != 0) {
                m_pDocument->remove_first_node();
            }
            m_pDocument->append_node(_root.m_pNode);
            return true;
        }
    }
    return false;
}


XmlNode XmlDocument::createNode(EXmlNodeType _type, const std::string& _name, const std::string& _value)
{
    if (m_pDocument == nullptr) {
        createDocument();
    }

    rapidxml::xml_node<>* pNode = nullptr;
    if (m_pDocument != nullptr && _type != EXmlNodeType::Undefined) {
        rapidxml::node_type type = rapidxml::node_type::node_document;
        switch (_type) {
            case EXmlNodeType::Document:    type = rapidxml::node_type::node_document; break;
            case EXmlNodeType::Element:     type = rapidxml::node_type::node_element; break;
            case EXmlNodeType::Data:        type = rapidxml::node_type::node_data; break;
            case EXmlNodeType::CData:       type = rapidxml::node_type::node_cdata; break;
            case EXmlNodeType::Comment:     type = rapidxml::node_type::node_comment; break;
            case EXmlNodeType::Declaration: type = rapidxml::node_type::node_declaration; break;
            case EXmlNodeType::Doctype:     type = rapidxml::node_type::node_doctype; break;
            case EXmlNodeType::Pi:          type = rapidxml::node_type::node_pi; break;            
            case EXmlNodeType::Undefined:   break;
        }
        const char* name = _name.empty() ? 0 : m_pDocument->allocate_string(_name.c_str());
        const char* value = _value.empty() ? 0 : m_pDocument->allocate_string(_value.c_str());
        try {
            pNode = m_pDocument->allocate_node(type, name, value);
        }
        catch (std::bad_alloc ex) {
            pNode = nullptr;
        }
    }
    return XmlNode(pNode);
}


XmlAttribute XmlDocument::createAttribute(const std::string& _name, const std::string& _value)
{
    if (m_pDocument == nullptr) {
        createDocument();
    }

    rapidxml::xml_attribute<>* pAttribute = nullptr;
    if (m_pDocument != nullptr) {
        const char* name = _name.empty() ? 0 : m_pDocument->allocate_string(_name.c_str());
        const char* value = _value.empty() ? 0 : m_pDocument->allocate_string(_value.c_str());
        try {
            pAttribute = m_pDocument->allocate_attribute(name, value);
        }
        catch (std::bad_alloc ex) {
            pAttribute = nullptr;
        }
    }
    return XmlAttribute(pAttribute);
}

bool XmlDocument::createDocument()
{
    reset();
    m_pDocument = new rapidxml::xml_document<>();   
    return (m_pDocument != nullptr);
}

void XmlDocument::reset()
{
    if (m_pDocument != nullptr) {
        delete m_pDocument;
        m_rawXml = "";
    }
}

}
}