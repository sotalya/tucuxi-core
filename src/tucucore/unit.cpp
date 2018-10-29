#include "unit.h"

#include <iostream>

namespace Tucuxi {
namespace Core {

Value translateToUnit(Value _value, Unit _initialUnit, Unit _finalUnit)
{
    if (_initialUnit == _finalUnit) {
        return _value;
    }

    static std::map<std::string, Value> factorMap = {
        {"mg/l-ug/l", 1000.0},
        {"ug/l-mg/l", 0.0001}
    };

    std::string key = _initialUnit.toString() + "-" + _finalUnit.toString();
    if (factorMap.count(key) == 0) {
        std::cout << "Error in unit conversion. No known conversion from " <<
                     _initialUnit.toString() << " to "  << _finalUnit.toString() << std::endl;
        return 0.0;
    }

    return _value * factorMap[key];


}




} // namespace Core
} // namespace Tucuxi
