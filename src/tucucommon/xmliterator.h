/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_TUCUCOMMON_XMLITERATOR_H
#define TUCUXI_TUCUCOMMON_XMLITERATOR_H

namespace Tucuxi {
namespace Common {

template<class T>
class XmlIterator
{
public:
    T* operator->() { return &m_current; }
    XmlIterator operator++(int) { m_current = m_current.next(); return *this; }
    XmlIterator& operator++() { m_current = m_current.next(); return *this; }
    bool operator==(const XmlIterator& _other) { return m_current == _other.m_current; }
    bool operator!=(const XmlIterator& _other) { return m_current != _other.m_current; }

    static XmlIterator<T> none() { return XmlIterator<T>(T()); }

private:
    XmlIterator(T _firstNode) { m_current = _firstNode; }

private:
    T m_current;

    friend class XmlNode;
};

}
}

#endif // TUCUXI_TUCUCOMMON_XMLITERATOR_H