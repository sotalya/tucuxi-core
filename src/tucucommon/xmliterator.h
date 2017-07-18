/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_TUCUCOMMON_XMLITERATOR_H
#define TUCUXI_TUCUCOMMON_XMLITERATOR_H

namespace Tucuxi {
namespace Common {

/// \ingroup TucuCommon
/// \brief Iterator class to iterate nodes and attributes
/// \sa XmlDocument, XmlNode, XmlAttribute
template<class T>
class XmlIterator
{
public:
    /// \brief Let this class act as a pointer
    T* operator->() { return &m_current; }
    
    /// \brief Move to the next item
    /// @return The iterator
    XmlIterator operator++(int) { m_current = m_current.next(); return *this; }
    
    /// \brief Move to the next item
    /// @return The iterator
    XmlIterator& operator++() { m_current = m_current.next(); return *this; }
    
    /// \brief Check if both iterators reference the same item
    /// @param The other iterator
    /// @return True if both iterator reference the same item
    bool operator==(const XmlIterator& _other) { return m_current == _other.m_current; }
    
    /// \brief Check if both iterators reference the same item
    /// @param The other iterator
    /// @return False if both iterator reference the same item
    bool operator!=(const XmlIterator& _other) { return m_current != _other.m_current; }

    /// \brief Gives an "empty" iterator. Useful to check the end of an iteration.
    /// @return An "empty" iterator
    static XmlIterator<T> none() { return XmlIterator<T>(T()); }

private:
    /// \brief Constuctor used by XmlNode to build new iterators
    /// @param The first item of the collection.
    XmlIterator(T _first) { m_current = _first; }

private:
    T m_current;    /// The current node or attribute

    friend class XmlNode;
};

}
}

#endif // TUCUXI_TUCUCOMMON_XMLITERATOR_H