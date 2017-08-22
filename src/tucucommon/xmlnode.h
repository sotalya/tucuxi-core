/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_TUCUCOMMON_XMLNODE_H
#define TUCUXI_TUCUCOMMON_XMLNODE_H

#include <string>

#include "componentmanager.h"

namespace rapidxml {
    template<class Ch> class xml_node;
}

namespace Tucuxi {
namespace Common {

class XmlNode;
class XmlAttribute;

template<class T> class XmlIterator;
typedef class XmlIterator<XmlNode> XmlNodeIterator;
typedef class XmlIterator<XmlAttribute> XmlAttributeIterator;

/// \ingroup TucuCommon
/// \brief Models an XML node
/// \sa XmlNode
enum class EXmlNodeType {
    Document,       /// A document node. Name and value are empty.
    Element,        /// An element node. Name contains element name. Value contains text of first data node.
    Comment,        /// A comment node. Name is empty. Value contains comment text.
    Data,           /// A data node. Name is empty. Value contains data text.
    CData,          /// A CDATA node. Name is empty. Value contains data text.
    Declaration,    /// A declaration node. Name and value are empty. Declaration parameters (version, encoding and standalone) are in node attributes.
    Doctype,        /// A DOCTYPE node. Name is empty. Value contains DOCTYPE text.
    Pi,             /// A "Processing Instruction" node. Name contains target. Value contains instructions.
    Undefined       /// Type not defined yet.
};


/// \ingroup TucuCommon
/// \brief Models an XML node.
/// This class is a simple wrapper around a rapidxml::xml_node pointer. 
/// Since it is so small and simple, all nodes and attributes are returned by value.
/// \sa XmlDocument, XmlAttribute, EXmlNodeType
class XmlNode
{
public: 
    /// \brief Default constructor to build an empty node
    XmlNode();

    /// \brief Copy constructor to create a new reference to the given node
    XmlNode(const XmlNode& _other);

    /// \brief Destructor
    ~XmlNode();
    
    /// \brief Check if the node is valid
    /// \return True if the node is valid
    bool isValid() const;
    
    /// \brief Retrieve the name of the node
    /// \return The name of the node or "" if invalid
    std::string getName() const;

    /// \brief Give a new name to the node
    /// \param _name The new name of the node
    /// \return True if successful
    bool setName(const std::string& _name);
    
    /// \brief Retrieve the value of the node
    /// \return The value of the node or "" if invalid
    std::string getValue() const;
        
    /// \brief Give a new value to the node
    /// \param _name The new value of the node
    /// \return True if successful
    bool setValue(const std::string& _value);
    
    /// \brief Retrieve the type of the node
    /// \return The type of the node
    EXmlNodeType getType() const;

    /// \brief Retrieve one attribute of the node
    /// \param The name of the attribute to look for
    /// \return The requested attribute. The returned object will be invalid in case no attribute matching the given string is found.
    XmlAttribute getAttribute(const std::string& _name) const;

    /// \brief Retrieve all attributes of the node
    /// \return An iterator to enumerate all attributes
    XmlAttributeIterator getAttributes() const;

    /// \brief Retrieve all children of the node
    /// \return An iterator to enumerate all children
    XmlNodeIterator getChildren() const;

    /// \brief Retrieve all children of the node matching the given type
    /// \param The type of element to look for
    /// \return An iterator to enumerate all children of the requested type
    XmlNodeIterator getChildren(EXmlNodeType) const;

    /// \brief Retrieve all children of the node matching the given name
    /// \param The name of element to look for
    /// \return An iterator to enumerate all children of the requested name
    XmlNodeIterator getChildren(const std::string& _name) const;

    /// \brief Retrieve the node's parent
    /// \return The node's parent
    XmlNode getParent() const;

    /// \brief Add a new child to the node
    /// \param _child The child to be added (see method XmlDocument::createNode)
    /// \return True if the node was added successfully
    bool addChild(XmlNode _child);
    
    /// \brief Add a new attribute to the node
    /// \param _attribute The attribute to be added (see method XmlDocument::createAttribute)
    /// \return True if the attribute was added successfully
    bool addAttribute(XmlAttribute _attribute);
    
    /// \brief Share the reference with the specified node (this method copies the wrapper not the actual underlying xml element!)
    /// \param _other The node to share the reference with
    /// \return The node itself
    XmlNode& operator=(const XmlNode& _other);

    /// \brief Compare if two XmlNode instances share the same reference to the actual XML element.
    /// \return True if both objects are sharing the same reference.
    bool operator==(const XmlNode& _other) const;

    /// \brief Compare if two XmlNode instances share the same reference to the actual XML element.
    /// \return False if both objects are sharing the same reference.
    bool operator!=(const XmlNode& _other) const;

private:
    /// \brief Create a new node and initialize it with a pointer to a rapidxml node.
    XmlNode(rapidxml::xml_node<char>* _pNode);
    
    /// \brief Used by XmlNodeIterator to move to the next sibling.
    /// \return The next sibling. The returned node will be invalid if there is no more sibling.
    XmlNode next() const;
    
    /// \brief Helper method asking the associated docuement to allocate a string.
    char* allocateString(const std::string& _string);
    
private:
    rapidxml::xml_node<char> *m_pNode;  /// The "wrapped" rapidxml node.
    
    friend class XmlDocument;
    friend class XmlAttribute;
    friend class XmlIterator<XmlNode>;
};

}
}

#endif // TUCUXI_TUCUCOMMON_XMLNODE_H
