#include "unit.h"

#include <iostream>

#include "tucucommon/loggerhelper.h"


namespace Tucuxi {
namespace Core {

Value translateToUnit(Value _value, Unit _initialUnit, Unit _finalUnit)
{
    if (_initialUnit == _finalUnit)
    {
        return _value;
    }

    static std::map<std::string, Value> factorMap = {
        {"h-m", 60.0},
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

    return _value * factorMap[key];


}




} // namespace Core
} // namespace Tucuxi
