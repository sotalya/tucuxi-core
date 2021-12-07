/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_TUCUCOMMON_XMLDOCUMENT_H
#define TUCUXI_TUCUCOMMON_XMLDOCUMENT_H

#include <memory>
#include <string>

namespace rapidxml {
template<class Ch>
class xml_document; // NOLINT(readability-identifier-naming)
} // namespace rapidxml

namespace Tucuxi {
namespace Common {

class XmlNode;
class XmlAttribute;
enum class EXmlNodeType;

/// \ingroup TucuCommon
/// \brief Models an XML document.
/// This class allows reading and writing XML data from/to a string or a file.
/// \sa XmlNode, XmlAttribute
class XmlDocument
{
public:
    /// \brief Default constructor to create an empty document
    XmlDocument();

    /// \brief Copy constructor is not supported
    XmlDocument(const XmlDocument& _document) = delete;

    /// \brief Destructor that will free all memory used by the document
    ~XmlDocument();

    /// \brief Check if the document is valid
    /// \return True if the document is valid
    bool isValid() const;

    /// \brief Load and parse XML data from the specified file
    /// \param _fileName The location of the XML file
    bool open(const std::string& _fileName);

    /// \brief Save XML data into the specified file
    /// \param _fileName The location of the XML file
    bool save(const std::string& _fileName);

    /// \brief Parse XML data from the specified string
    /// \param _xml The string containing XML data
    bool fromString(const std::string& _xml);

    /// \brief Save XML data into the specified string
    /// \param _xml The string where XML data should be written
    /// \param _prettyPrint Make the output pretty (line breaks, indenting, ...)
    bool toString(std::string& _xml, bool _prettyPrint = false);

    /// \brief Returns the root element of the document
    /// \return The rool element
    XmlNode getRoot() const;

    /// \brief Set the root element of the document
    /// \param The new root element
    /// \return True if ok.
    bool setRoot(const XmlNode& _root);

    /// \brief Create a new node
    /// \param _type The type of node to be created
    /// \param _name The name of the node
    /// \param _value The value of the node
    /// \return The newly created node or nullptr in case of problem
    XmlNode createNode(EXmlNodeType _type, const std::string& _name = "", const std::string& _value = "");

    /// \brief Create a new attribute
    /// \param _name The name of the attribute
    /// \param _value The value of the attribute
    XmlAttribute createAttribute(const std::string& _name, const std::string& _value = "");

private:
    /// \brief Delete the current document
    void reset();

    /// \brief Create a new document
    /// \return True if the document was created
    bool createDocument();

    std::string m_rawXml; /// The raw XML data as recieved by fromString() or open().
    std::unique_ptr<rapidxml::xml_document<char> > m_pDocument; /// A pointer to the actual "rapidxml" document object
};

} // namespace Common
} // namespace Tucuxi

#endif // TUCUXI_TUCUCOMMON_XMLDOCUMENT_H
