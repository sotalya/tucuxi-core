/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_TUCUCOMMON_XMLATTRIBUTE_H
#define TUCUXI_TUCUCOMMON_XMLATTRIBUTE_H

#include <string>

namespace rapidxml {
    template<class Ch> class xml_attribute;
}

namespace Tucuxi {
namespace Common {

class XmlNode;

/// \ingroup TucuCommon
/// \brief 
/// \sa XmlNode, XmlDocument
class XmlAttribute
{
public:
    XmlAttribute();
    XmlAttribute(const XmlAttribute& _node);
    ~XmlAttribute();

    XmlNode getNode();
    std::string getName() const;
    void setName(const std::string& _name);
    std::string getValue() const;
    void setValue(const std::string& _value);

    XmlAttribute& operator=(const XmlAttribute& _other);
    bool operator==(const XmlAttribute& _other) const;
    bool operator!=(const XmlAttribute& _other) const;

private:    
    XmlAttribute(rapidxml::xml_attribute<char>* _pAttribute);
    
    XmlAttribute next() const;

    char* allocateString(const std::string& _string);

private:
    rapidxml::xml_attribute<char>* m_pAttribute;

    friend class XmlDocument;
    friend class XmlNode;    
    friend class XmlIterator<XmlAttribute>;
};

}
}

#endif // TUCUXI_TUCUCOMMON_XMLATTRIBUTE_H
