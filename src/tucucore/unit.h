#ifndef TUCUXI_CORE_UNIT_H
#define TUCUXI_CORE_UNIT_H

#include <string>

#include "definitions.h"

namespace Tucuxi {
namespace Core {

class Unit
{
public:
    Unit() {}
    Unit(std::string _unitString) { m_unitString = _unitString; }

    inline bool operator==(const Unit& rhs) const { return this->m_unitString == rhs.m_unitString; }

    std::string toString() const { return m_unitString;}

    bool isTime() const { return ((m_unitString == "d") || (m_unitString == "h") || (m_unitString == "m") || (m_unitString == "s"));}

protected:
    std::string m_unitString;

};

Value translateToUnit(Value _value, Unit _initialUnit, Unit _finalUnit);


} // namespace Core
} // namespace Tucuxi


#endif // TUCUXI_CORE_UNIT_H
