#include "unit.h"

#include <iostream>

#include "tucucommon/loggerhelper.h"


namespace Tucuxi {
namespace Core {


double translationFactor(Unit _initialUnit, Unit _finalUnit)
{
    if (_initialUnit == _finalUnit)
    {
        return 1.0;
    }

    static const std::map<std::string, Value> factorMap = {
        {"h-m", 60.0},
        {"m-h", 1.0 / 60.0},
        {"m-s", 60.0},
        {"s-m", 1.0 / 60.0},
        {"", 1},
        {"kg-g", 1000.0},
        {"g-kg", 0.001},
        {"g-mg", 1000.0},
        {"mg-g", 0.001},
        {"mg/l-ug/l", 1000.0},
        {"ug/l-mg/l", 0.001},
        {"mg*h/l-ug*h/l", 1000.0},
        {"h*mg/l-ug*h/l", 1000.0},
        {"ug*h/l-mg*h/l", 0.001}
    };

    std::string key = _initialUnit.toString() + "-" + _finalUnit.toString();
    if (factorMap.count(key) == 0) {
        Tucuxi::Common::LoggerHelper logHelper;
        logHelper.error("Error in unit conversion. No known conversion from {} to {}", _initialUnit.toString(), _finalUnit.toString());
        return 0.0;
    }

    return factorMap.at(key);
}

Value translateToUnit(Value _value, Unit _initialUnit, Unit _finalUnit)
{
    return _value * translationFactor(_initialUnit, _finalUnit);
}




} // namespace Core
} // namespace Tucuxi
