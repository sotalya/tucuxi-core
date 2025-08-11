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
    StandaloneSTLIterator(iteratorType& _first, iteratorType& _last) : m_first(_first), m_last(_last)
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

} // namespace Common
} // namespace Tucuxi

#endif // TUCUXI_TUCUCOMMON_ITERATOR_H
