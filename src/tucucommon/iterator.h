/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_TUCUCOMMON_ITERATOR_H
#define TUCUXI_TUCUCOMMON_ITERATOR_H

#include <iterator>

namespace Tucuxi {
namespace Common {

template<class T>
class Iterator
{
public:
    virtual T operator*() = 0;
    virtual Iterator<T>& next() = 0;
    virtual bool isDone() const = 0;
    virtual void reset() = 0;
};

template<class iteratorType, class T>
class StandaloneSTLIterator : public Iterator<T>
{
public:
    StandaloneSTLIterator(iteratorType &_first, iteratorType &_last)
        : m_first(_first), m_last(_last)
    {
        reset();
    }
    T operator*() override
    {
        return *m_it;
    }
    Iterator<T>& next() override
    {
        m_it++;
        return *this;
    }
    bool isDone() const override
    {
        return (m_it == m_last);
    }
    void reset() override
    {
        m_it = m_first;
    }

private:
    iteratorType m_first;
    iteratorType m_last;
    iteratorType m_it;
};

}
}

#endif // TUCUXI_TUCUCOMMON_ITERATOR_H
