/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_TUCUCOMMON_XMLDOCUMENT_H
#define TUCUXI_TUCUCOMMON_XMLDOCUMENT_H

#include <string>

namespace rapidxml {
    template<class Ch> class xml_document;
}

namespace Tucuxi {
namespace Common {

class XmlNode;
class XmlAttribute;
enum class EXmlNodeType;

/// \ingroup TucuCommon
/// \brief 
/// \sa XmlNode, XmlAttribute
class XmlDocument
{
public:
    XmlDocument();
    XmlDocument(const XmlDocument& _document);
    ~XmlDocument();

    bool open(const std::string& _fileName);
    bool save(const std::string& _fileName);

    bool fromString(const std::string& _xml);
    bool toString(std::string& _xml);

    XmlNode getRoot();
    void setRoot(const XmlNode& _root);

    XmlNode createNode(EXmlNodeType _type, const std::string& _name = "", const std::string& _value = "");
    XmlAttribute createAttribute(const std::string& _name, const std::string& _value = "");

private:
    bool createDocument();

private:
    std::string m_rawXml;
    rapidxml::xml_document<char>* m_pDocument;
};

}
}

#endif // TUCUXI_TUCUCOMMON_XMLDOCUMENT_H
