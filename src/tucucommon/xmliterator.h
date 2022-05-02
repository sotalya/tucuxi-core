//@@license@@

#ifndef TUCUXI_TUCUCOMMON_XMLITERATOR_H
#define TUCUXI_TUCUCOMMON_XMLITERATOR_H

#include <string>

#include "xmlnode.h"
#include "xmltypes.h"

namespace Tucuxi {
namespace Common {

class XmlNode;

/// \ingroup TucuCommon
/// \brief Iterator base class to iterate nodes and attributes
/// \sa XmlDocument, XmlNode, XmlAttribute
template<class T>
class XmlIterator
{
public:
    /// \brief Let this class act as a pointer
    T* operator->()
    {
        return &m_current;
    }

    /// \brief Move to the next item
    /// \return The iterator
    XmlIterator operator++(int)
    {
        next();
        return *this;
    }

    /// \brief Move to the next item
    /// \return The iterator
    XmlIterator& operator++()
    {
        next();
        return *this;
    }

    /// \brief Check if both iterators reference the same item
    /// \param The other iterator
    /// \return True if both iterator reference the same item
    bool operator==(const XmlIterator& _other)
    {
        return m_current == _other.m_current;
    }

    /// \brief Check if both iterators reference the same item
    /// \param The other iterator
    /// \return False if both iterator reference the same item
    bool operator!=(const XmlIterator& _other)
    {
        return m_current != _other.m_current;
    }

    /// \brief Gives an "empty" iterator. Useful to check the end of an iteration.
    /// \return An "empty" iterator
    static XmlIterator<T> none()
    {
        return XmlIterator<T>(T());
    }

protected:
    /// \brief Constuctor used by XmlNode to build new iterators
    /// \param The first item of the collection.
    XmlIterator(T _first, const std::string& _name = "")
        : m_current(_first), m_name(_name), m_type(EXmlNodeType::Undefined)
    {
        // Look for the next valid item in the specified sub group
        if (m_current.isValid() && !isInSubgroup()) {
            next();
        }
    }

    XmlIterator(T _first, EXmlNodeType _type) : m_current(_first), m_name(""), m_type(_type)
    {
        // Look for the next valid item in the specified sub group
        if (m_current.isValid() && !isInSubgroup()) {
            next();
        }
    }

private:
    /// \brief Overridable method to move to the next item
    void next()
    {
        // Get the next valid item in the specified group
        m_current = m_current.next();
        while (m_current.isValid() && !isInSubgroup()) {
            m_current = m_current.next();
        }
    }

    /// \brief Check if the current item should be filtered out
    /// \return True if the current item should be returned by the iterator
    bool isInSubgroup()
    {
        return true; // No filter by default
    }

protected:
    T m_current;         /// The current node or attribute
    std::string m_name;  /// The name of nodes to return
    EXmlNodeType m_type; /// The type of nodes to return
    friend class XmlNode;
};


/// \brief Partial specialization to handle filtering nodes
/// \return True if the current item should be returned by the iterator
template<>
inline bool XmlIterator<XmlNode>::isInSubgroup()
{
    if (!m_name.empty() && m_name != m_current.getName()) {
        return false;
    }
    if (m_type != EXmlNodeType::Undefined && m_type != m_current.getType()) {
        return false;
    }
    return true;
}

} // namespace Common
} // namespace Tucuxi

#endif // TUCUXI_TUCUCOMMON_XMLITERATOR_H
