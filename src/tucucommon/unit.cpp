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

const std::map<UnitManager::UnitType, std::map<std::string, double>>& UnitManager::getConversionMap()
{
    static const std::map<UnitType, std::map<std::string, double>> sm_conversionMap =
    {
        {
            UnitType::Weight,
            {
                {"kg", 1000.0},
                {"g", 1.0},
                {"mg", 0.001},
                {"ug", 0.000001}
            }
        },
        {
            UnitType::Concentration,
            {
                {"g/l", 1.0},
                {"mg/l", 1000.0},
                {"ug/l", 1000000.0}
            }
        }
    };

    return sm_conversionMap;
}

void UnitManager::logConversionError(const Unit &_initialUnit, const Unit &_finalUnit)
{
    Tucuxi::Common::LoggerHelper logHelper;
    logHelper.error("Error in unit conversion. No known conversion from {} to {}", _initialUnit.toString(), _finalUnit.toString());
}

double UnitManager::convertToUnit(double _value, Unit _initialUnit, Unit _finalUnit)
{
    // TODO : Iterate over the conversion map to see if we find the initial and final unit strings.
    // If yes and they are of the same type, then conversion is applied,
    // Else throw an std::invalid_argument
    return 1.0;

}

double UnitManager::translateConcentrationTimeFactor(Unit _initialUnit, Unit _finalUnit)
{
    static const std::map<std::string, double> factorMap = {

        {"mg*h/l-ug*h/l", 1000.0},
        {"h*mg/l-ug*h/l", 1000.0},

        {"h*ug/l-ug*h/l", 1.0},

        {"h*g/l-ug*h/l", 1000000.0},
        {"g*h/l-ug*h/l", 1000000.0},


        {"mg*min/l-ug*h/l", 1000.0 / 60.0},
        {"min*mg/l-ug*h/l", 1000.0 / 60.0},

        {"g*min/l-ug*h/l", 10000000.0 / 60.0},
        {"min*g/l-ug*h/l", 10000000.0 / 60.0},

        {"ug*min/l-ug*h/l", 1.0 / 60.0},
        {"min*ug/l-ug*h/l", 1.0 / 60.0},



        {"mg*h/ml-ug*h/l", 1000000.0},
        {"h*mg/ml-ug*h/l", 1000000.0},

        {"h*ug/ml-ug*h/l", 1000.0},

        {"h*g/ml-ug*h/l", 1000000000.0},
        {"g*h/ml-ug*h/l", 1000000000.0},


        {"mg*min/ml-ug*h/l", 1000000.0 / 60.0},
        {"min*mg/ml-ug*h/l", 1000000.0 / 60.0},

        {"g*min/ml-ug*h/l", 10000000000.0 / 60.0},
        {"min*g/ml-ug*h/l", 10000000000.0 / 60.0},

        {"ug*min/ml-ug*h/l", 1.0 / 60000.0},
        {"min*ug/ml-ug*h/l", 1.0 / 60000.0}
    };

    std::string key = _initialUnit.toString() + "-" + _finalUnit.toString();
    std::string reverseKey = _finalUnit.toString() + "-" + _initialUnit.toString();
    if (factorMap.count(key) == 0) {
        if (factorMap.count(reverseKey) == 0)
        {
            Tucuxi::Common::LoggerHelper logHelper;
            logHelper.error("Error in unit conversion. No known conversion from {} to {} or reverse", _initialUnit.toString(), _finalUnit.toString());
            return 0.0;
        }
        return 1.0 / factorMap.at(reverseKey);
    }


    return factorMap.at(key);
}

double UnitManager::translateWeightFactor(Unit _initialUnit, Unit _finalUnit)
{
    // The base unit is g
    static const std::map<std::string, double> factorMap = {

        {"kg", 1000.0},
        {"g", 1.0},
        {"mg", 0.001},
        {"ug", 0.000001}
    };

    convertToUnit(0.0, Unit(), Unit());

    std::string key = _initialUnit.toString();
    std::string reverseKey = _finalUnit.toString();
    if ((factorMap.count(key) == 0) || (factorMap.count(reverseKey) == 0))
    {
        Tucuxi::Common::LoggerHelper logHelper;
        logHelper.error("Error in unit conversion. No known conversion from {} to {} or reverse", _initialUnit.toString(), _finalUnit.toString());
        throw std::invalid_argument("Error in unit conversion");
    }

    return factorMap.at(key) / factorMap.at(reverseKey);
}


double UnitManager::translateConcentrationFactor(Unit _initialUnit, Unit _finalUnit)
{
    static const std::map<std::string, double> factorMap = {

        {"g/l-ug/l", 1000000.0},
        {"mg/l-ug/l", 1000.0},
        {"g/ml-ug/l", 1000000000.0},
        {"mg/ml-ug/l", 1000000.0},
        {"ug/ml-ug/l", 1000.0}
    };

    std::string key = _initialUnit.toString() + "-" + _finalUnit.toString();
    std::string reverseKey = _finalUnit.toString() + "-" + _initialUnit.toString();
    if (factorMap.count(key) == 0) {
        if (factorMap.count(reverseKey) == 0)
        {
            Tucuxi::Common::LoggerHelper logHelper;
            logHelper.error("Error in unit conversion. No known conversion from {} to {} or reverse", _initialUnit.toString(), _finalUnit.toString());
            return 0.0;
        }
        return 1.0 / factorMap.at(reverseKey);
    }


    return factorMap.at(key);
}

double UnitManager::translateMoleConcentrationFactor(Unit _initialUnit, Unit _finalUnit)
{
    static const std::map<std::string, double> factorMap = {

        {"mmol/l-mol/l", 0.001},
        {"umol/l-mol/l", 0.000001},
        {"mol/ml-mol/l", 1000},
        {"mmol/ml-mol/l", 1.0},
        {"umol/ml-mol/l", 0.001}
    };

    std::string key = _initialUnit.toString() + "-" + _finalUnit.toString();
    std::string reverseKey = _finalUnit.toString() + "-" + _initialUnit.toString();
    if (factorMap.count(key) == 0) {
        if (factorMap.count(reverseKey) == 0)
        {
            Tucuxi::Common::LoggerHelper logHelper;
            logHelper.error("Error in unit conversion. No known conversion from {} to {} or reverse", _initialUnit.toString(), _finalUnit.toString());
            return 0.0;
        }
        return 1.0 / factorMap.at(reverseKey);
    }


    return factorMap.at(key);
}

double UnitManager::translateTimeFactor(Unit _initialUnit, Unit _finalUnit)
{
    static const std::map<std::string, double> factorMap = {

        {"s-m", 1.0 / 60.0},
        {"h-m", 3600.0}
    };

    std::string key = _initialUnit.toString() + "-" + _finalUnit.toString();
    std::string reverseKey = _finalUnit.toString() + "-" + _initialUnit.toString();
    if (factorMap.count(key) == 0) {
        if (factorMap.count(reverseKey) == 0)
        {
            Tucuxi::Common::LoggerHelper logHelper;
            logHelper.error("Error in unit conversion. No known conversion from {} to {} or reverse", _initialUnit.toString(), _finalUnit.toString());
            return 0.0;
        }
        return 1.0 / factorMap.at(reverseKey);
    }


    return factorMap.at(key);
}

double UnitManager::translateHeightFactor(Unit _initialUnit, Unit _finalUnit)
{
    static const std::map<std::string, double> factorMap = {

        {"cm-m", 0.01},
        {"dm-m", 0.1},
        {"mm-m", 0.001},
        {"ft-m", 0.3048},
        {"in-m", 0.0254},

    };

    std::string key = _initialUnit.toString() + "-" + _finalUnit.toString();
    std::string reverseKey = _finalUnit.toString() + "-" + _initialUnit.toString();
    if (factorMap.count(key) == 0) {
        if (factorMap.count(reverseKey) == 0)
        {
            Tucuxi::Common::LoggerHelper logHelper;
            logHelper.error("Error in unit conversion. No known conversion from {} to {} or reverse", _initialUnit.toString(), _finalUnit.toString());
            return 0.0;
        }
        return 1.0 / factorMap.at(reverseKey);
    }


    return factorMap.at(key);
}

double UnitManager::translateEmptyFactor(Unit _initialUnit, Unit _finalUnit)
{
    static const std::map<std::string, double> factorMap = {

        {"-", 1.0}

    };

    std::string key = _initialUnit.toString() + "-" + _finalUnit.toString();
    std::string reverseKey = _finalUnit.toString() + "-" + _initialUnit.toString();
    if (factorMap.count(key) == 0) {
        if (factorMap.count(reverseKey) == 0)
        {
            Tucuxi::Common::LoggerHelper logHelper;
            logHelper.error("Error in unit conversion. No known conversion from {} to {} or reverse", _initialUnit.toString(), _finalUnit.toString());
            return 0.0;
        }
        return 1.0 / factorMap.at(reverseKey);
    }


    return factorMap.at(key);
}

double UnitManager::translationFactor(Unit _initialUnit, Unit _finalUnit)
{
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

double UnitManager::translateToUnit(double _value, Unit _initialUnit, Unit _finalUnit, UnitType _unitType)
{

    double tempValue;

    if (_initialUnit == _finalUnit){
        return _value;
    }

    switch (_unitType) {

    case UnitType::Time :
            tempValue = _value * translateTimeFactor(_initialUnit, _finalUnit);
        break;

    case UnitType::Height :
            tempValue = _value * translateHeightFactor(_initialUnit, _finalUnit);
        break;

    case UnitType::Weight :
            tempValue = _value * translateWeightFactor(_initialUnit, _finalUnit);
        break;

    case UnitType::Concentration :
            tempValue = _value * translateConcentrationFactor(_initialUnit, _finalUnit);
        break;

    case UnitType::ConcentrationTime :
            tempValue = _value * translateConcentrationTimeFactor(_initialUnit, _finalUnit);
        break;

    case UnitType::MoleConcentration :
            tempValue = _value * translateMoleConcentrationFactor(_initialUnit, _finalUnit);
        break;

    case UnitType::NoUnit :
            tempValue = _value * translateEmptyFactor(_initialUnit, _finalUnit);
        break;

    default:
            tempValue = _value * translationFactor(_initialUnit, _finalUnit);
        break;

    }

    return tempValue;
}

bool UnitManager::isKnown(const Unit& _unit)
{
    static const std::vector<Unit> units = {
        Unit(""),
        Unit("-"),
        Unit("y"),
        Unit("d"),
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
        Unit("g/l"),
        Unit("g/ml"),

        Unit("mg*h/l"),
        Unit("h*mg/l"),
        Unit("min*mg/l"),
        Unit("mg*min/l"),
        Unit("mg*h/ml"),
        Unit("h*mg/ml"),
        Unit("min*mg/ml"),
        Unit("mg*min/ml"),

        Unit("ug*h/l"),
        Unit("h*ug/l"),
        Unit("min*ug/l"),
        Unit("ug*min/l"),
        Unit("h*ug/ml"),
        Unit("ug*h/ml"),
        Unit("min*ug/ml"),
        Unit("ug*min/ml"),

        Unit("mol/l"),
        Unit("mmol/l"),
        Unit("umol/l"),
        Unit("mol/ml"),
        Unit("mmol/ml"),
        Unit("umol/ml"),

        Unit("g/mol")
    };

    for( const auto unit : units)
    {
        if(unit == _unit)
        {
            return true;
        }
    }

    return false;
}



} // namespace Common
} // namespace Tucuxi
