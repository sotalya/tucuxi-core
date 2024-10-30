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


#ifndef TUCUXI_TUCUCOMMON_XMLATTRIBUTE_H
#define TUCUXI_TUCUCOMMON_XMLATTRIBUTE_H

#include <string>

namespace rapidxml {
template<class Ch>
class xml_attribute; // NOLINT(readability-identifier-naming)
} // namespace rapidxml

namespace Tucuxi {
namespace Common {

class XmlNode;
class XmlDocument;

template<class T>
class XmlIterator;



/// \ingroup TucuCommon
/// \brief Models an XML attribute
/// \sa XmlNode, XmlDocument
class XmlAttribute
{
public:
    /// \brief Default constructor
    XmlAttribute();

    /// \brief Copy constructor a new reference to the given attribute
    XmlAttribute(const XmlAttribute& _node);

    /// \brief Destructor
    ~XmlAttribute() = default;

    /// \brief Check if the attribute is valid
    /// \return True if the attribute is valid
    bool isValid() const;

    /// \brief Retrieve the associated node
    /// \return The associated node
    XmlNode getNode();

    /// \brief Retrieve the name of the attribute
    /// \return The name of the attribute
    std::string getName() const;

    /// \brief Change the name of the attribute
    /// \param _name The new name
    /// \return True if successful
    bool setName(const std::string& _name);

    /// \brief Retrieve the value of the attribute.
    /// \return The value of the attribute.
    std::string getValue() const;

    /// \brief Change the value of the attribute
    /// \param _value The new value
    /// \return True if successful
    bool setValue(const std::string& _value);

    /// \brief Share the reference with the specified node (this method copies the wrapper not the actual underlying xml attribute!)
    /// \param _other The attribute to share the reference with
    /// \return The attribute itself
    XmlAttribute& operator=(const XmlAttribute& _other);

    /// \brief Compare if two XmlAttribute instances share the same reference to the actual XML attribute.
    /// \return True if both objects are sharing the same reference.
    bool operator==(const XmlAttribute& _other) const;

    /// \brief Compare if two XmlAttribute instances share the same reference to the actual XML attribute.
    /// \return False if both objects are sharing the same reference.
    bool operator!=(const XmlAttribute& _other) const;

private:
    /// \brief Create a new node and initialize it with a pointer to a rapidxml attribute.
    XmlAttribute(rapidxml::xml_attribute<char>* _pAttribute);

    /// \brief Used by XmlAttributeIterator to move to the next sibling.
    /// \return The next sibling. The returned node will be invalid if there is no more sibling.
    XmlAttribute next() const;

    /// \brief Helper method asking the associated docuement to allocate a string.
    char* allocateString(const std::string& _string);

    rapidxml::xml_attribute<char>* m_pAttribute; /// The "wrapped" rapdixml attribute

    friend class XmlDocument;
    friend class XmlNode;
    friend class XmlIterator<XmlAttribute>;
};

} // namespace Common
} // namespace Tucuxi

#endif // TUCUXI_TUCUCOMMON_XMLATTRIBUTE_H
