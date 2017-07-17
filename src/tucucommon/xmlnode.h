/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_TUCUCOMMON_XMLNODE_H
#define TUCUXI_TUCUCOMMON_XMLNODE_H

#include <string>

#include "xmliterator.h"

namespace rapidxml {
    template<class Ch> class xml_node;
}

namespace Tucuxi {
namespace Common {

class XmlNode;
class XmlAttribute;

typedef class XmlIterator<XmlNode> XmlNodeIterator;
typedef class XmlIterator<XmlAttribute> XmlAttributeIterator;

enum class EXmlNodeType {
    Document,
    Element,
    Comment,
    Data,
    CData,
    Declaration,
    Doctype,       
    Pi,
    Undefined
};

/// \ingroup TucuCommon
/// \brief 
/// \sa XmlNode, XmlAttribute
class XmlNode
{
public: 
    XmlNode();
    XmlNode(const XmlNode& _other);
    ~XmlNode();

    std::string getName() const;
    void setName(const std::string& _name);
    std::string getValue() const;
    void setValue(const std::string& _value);
    EXmlNodeType getType() const;
    XmlAttribute getAttributes(const std::string& _name);
    XmlAttributeIterator getAttributes() const;
    XmlNodeIterator getChildren() const;
    XmlNode getParent() const;
    void addChild(XmlNode _child);
    void addAttribute(XmlAttribute _attribute);

    XmlNode& operator=(const XmlNode& _other);
    bool operator==(const XmlNode& _other) const;
    bool operator!=(const XmlNode& _other) const;

private:    
    XmlNode(rapidxml::xml_node<char>* _pNode);

    XmlNode next() const;

    char* allocateString(const std::string& _string);
    
private:
    rapidxml::xml_node<char> *m_pNode;

    friend class XmlDocument;
    friend class XmlAttribute;
    friend class XmlIterator<XmlNode>;
};

}
}

#endif // TUCUXI_TUCUCOMMON_XMLNODE_H
