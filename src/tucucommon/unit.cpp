#include "unit.h"

#include <iostream>

#include "tucucommon/loggerhelper.h"


namespace Tucuxi {
namespace Common {

bool Unit::isTime() const
{
    return ((m_unitString == "d") || (m_unitString == "h") || (m_unitString == "m") || (m_unitString == "s"));
}

bool Unit::isEmpty() const {
    return m_unitString == "";
}


std::string Unit::toString() const {
    return m_unitString;
}

double UnitManager::translationFactor(Unit _initialUnit, Unit _finalUnit)
{
    if (_initialUnit == _finalUnit)
    {
        return 1.0;
    }

    static const std::map<std::string, double> factorMap = {
        {"h-m", 60.0},
        {"s-m", 1.0 / 60.0},

        {"m-h", 1.0 / 60.0},
        {"s-h", 1.0 / 3600.0},

        {"m-s", 60.0},
        {"h-s", 3600.0},

        {"", 1},

        {"g-kg", 0.001},
        {"mg-kg", 0.000001},

        {"mg-g", 0.001},
        {"kg-g", 1000.0},
        {"ug-g", 0.000001},

        {"g-mg", 1000.0},
        {"kg-mg", 1000000},
        {"ug-mg", 0.001},

        {"g-ug", 1000000},
        {"mg-ug", 1000},

        {"mg/l-ug/l", 1000.0},
        {"mg/ml-ug/l", 1000000},
        {"ug/ml-ug/l", 1000},

        {"ug/l-mg/l", 0.001},
        {"mg/ml-mg/l", 1000},
        {"ug/ml-mg/l", 1},

        {"ug/ml-mg/ml", 0.001},
        {"ug/l-mg/ml", 0.000001},
        {"mg/l-mg/ml", 1000},

        {"ug/l-ug/ml", 0.001},
        {"mg/ml-ug/ml", 0.001},
        {"mg/l-ug/ml", 1},

        {"mg*h/l-ug*h/l", 1000.0},
        {"h*mg/l-ug*h/l", 1000.0},
        {"h*ug/l-ug*h/l", 1},

        {"ug*h/l-mg*h/l", 0.001},
        {"h*ug/l-mg*h/l", 0.001},
        {"h*mg/l-mg*h/l", 1},

        {"mol/l-umol/l", 1000000},
        {"mmol/l-umol/l", 0.001},

        {"umol/l-mol/l", 0.000001},
        {"mmol/l-mol/l", 0.001},

        {"mol/l-mmol/l", 0.001},
        {"umol/l-mmol/l", 1000},

    };

    std::string key = _initialUnit.toString() + "-" + _finalUnit.toString();
    if (factorMap.count(key) == 0) {
        Tucuxi::Common::LoggerHelper logHelper;
        logHelper.error("Error in unit conversion. No known conversion from {} to {}", _initialUnit.toString(), _finalUnit.toString());
        return 0.0;
    }

    return factorMap.at(key);
}

double UnitManager::translateToUnit(double _value, Unit _initialUnit, Unit _finalUnit)
{
    double a = _value * translationFactor(_initialUnit, _finalUnit);
    return _value * translationFactor(_initialUnit, _finalUnit);
}

bool UnitManager::isKnown(const Unit& _unit)
{
    static const std::vector<Unit> units = {
        Unit(""),
        Unit("-"),
        Unit("h"),
        Unit("m"),
        Unit("s"),
        Unit("kg"),
        Unit("g"),
        Unit("mg"),
        Unit("ug"),
        Unit("mg/l"),
        Unit("mg/ml"),
        Unit("ug/l"),
        Unit("ug/ml"),
        Unit("mg*h/l"),
        Unit("h*mg/l"),
        Unit("ug*h/l"),
        Unit("h*ug/l"),
        Unit("mol/l"),
        Unit("mmol/l"),
        Unit("umol/l")
    };


    return false;
}



} // namespace Common
} // namespace Tucuxi
