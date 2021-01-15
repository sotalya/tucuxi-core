#include "unit.h"

#include <iostream>

#include "tucucommon/loggerhelper.h"


namespace Tucuxi {
namespace Common {

bool TucuUnit::isEmpty() const {
    return m_unitString == "";
}


std::string TucuUnit::toString() const {
    return m_unitString;
}

const std::vector<std::pair<std::string, std::string> > getTolerateUnit()
{
    static const std::vector<std::pair<std::string, std::string> > toleratePairUnit =
    {
        {"day","d"},
        {"days", "d"},
        {"week", "w"},
        {"weeks", "w"},
        {"months", "month"},
        {"year", "y"},
        {"years", "y"}
    };

    return toleratePairUnit;
}

bool UnitManager::isUnitTolerated(std::string& _unitString) {

    auto toleratePairUnit = getTolerateUnit();

    for(const auto unit : toleratePairUnit){
        if(_unitString == unit.first){
            _unitString = unit.second;
            return true;
        }
    }
    return false;
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
            UnitType::MolarMass,
            {
                {"g/mol", 1.0},
                {"g/umol", 1000000.0},
                {"kg/mol", 1000.0},
                {"kg/umol", 1000000000.0}
            }
        },
        {
            UnitType::Concentration,
            {
                {"g/l", 1.0},
                {"mg/l", 0.001},
                {"ug/l", 0.000001},
                {"g/ml", 1000},
                {"mg/ml", 1.0},
                {"ug/ml", 0.001}

            }
        },
        {
            UnitType::ConcentrationTime,
            {
                {"ug*h/l", 1.0},
                {"mg*h/l", 1000.0},
                {"h*mg/l", 1000.0},
                {"h*ug/l", 1.0},
                {"h*g/l", 1000000.0},
                {"g*h/l", 1000000.0},
                {"mg*min/l", 1000.0 / 60.0},
                {"min*mg/l", 1000.0 / 60.0},
                {"g*min/l", 1000000.0 / 60.0},
                {"min*g/l", 1000000.0 / 60.0},
                {"ug*min/l", 1.0 / 60.0},
                {"min*ug/l", 1.0 / 60.0},

                {"ug*h/ml", 1000.0},
                {"mg*h/ml", 1000000.0},
                {"h*mg/ml", 1000000.0},
                {"h*ug/ml", 1000.0},
                {"h*g/ml", 1000000000.0},
                {"g*h/ml", 1000000000.0},
                {"mg*min/ml", 1000000.0 / 60.0},
                {"min*mg/ml", 1000000.0 / 60.0},
                {"g*min/ml", 1000000000.0 / 60.0},
                {"min*g/ml", 1000000000.0 / 60.0},
                {"ug*min/ml", 1000.0 / 60.0 },
                {"min*ug/ml", 1000.0 / 60.0}
            }
        },
        {
            UnitType::MoleConcentration,
            {
                {"mol/l", 1.0},
                {"mmol/l", 0.001},
                {"umol/l", 0.000001},
                {"µmol/l", 0.000001},
                {"µmol/L", 0.000001},
                {"mol/ml", 1000},
                {"mmol/ml", 1.0},
                {"umol/ml", 0.001},
                {"µmol/ml", 0.001}
            }
        },
        {
            UnitType::FlowRate,
            {
                {"ml/min", 1.0},
                {"l/min", 1000.0},
                {"ml/h", 1.0 / 60.0},
                {"l/h", 1000.0 / 60.0}
            }
        },
        {
            UnitType::Temperature,
            {
                {"celsius", 1.0}
            }
        },
        {
            UnitType::Time,
            {
                {"min", 1.0},
                {"s", 1 / 60.0},
                {"h", 60.0},
                {"d", 24.0 * 60.0},
                {"w", 7.0 * 24.0 * 60.0},
                {"month", 30.0 * 24.0 * 60.0},
                {"y", 365.0 * 24.0 * 60.0}
            }
        },
        {
            UnitType::Length,
            {
                {"m", 1.0},
                {"cm", 0.01},
                {"dm", 0.1},
                {"mm", 0.001},
                {"ft", 0.3048},
                {"in", 0.0254}
            }
        },
        {
            UnitType::NoUnit,
            {
                {"-", 1.0},
                {"", 1.0}
            }
        }
    };

    return sm_conversionMap;
}

void UnitManager::logConversionError(const TucuUnit &_initialUnit, const TucuUnit &_finalUnit)
{
    static Tucuxi::Common::LoggerHelper logHelper;
    logHelper.error("Error in unit conversion. No known conversion from {} to {}", _initialUnit.toString(), _finalUnit.toString());
}

double UnitManager::convertToUnit(double _value, TucuUnit _initialUnit, TucuUnit _finalUnit)
{
    std::string initialKey = _initialUnit.toString();
    std::string finalKey = _finalUnit.toString();
    for(const auto &map : getConversionMap())
    {
        if ((map.second.count(initialKey) != 0) && (map.second.count(finalKey) != 0))
        {
            return _value * map.second.at(initialKey) / map.second.at(finalKey);
        }
    }

    logConversionError(_initialUnit, _finalUnit);
    throw std::invalid_argument("Error in unit conversion");
}


bool UnitManager::isCompatible(const TucuUnit& _unit1, const TucuUnit& _unit2)
{
    std::string unitString1 = _unit1.toString();
    std::string unitString2 = _unit2.toString();
    for(const auto &map : getConversionMap())
    {
        if ((map.second.count(unitString1) != 0) && (map.second.count(unitString2) != 0))
        {
            return true;
        }
    }

    return false;
}

bool UnitManager::isKnown(const TucuUnit& _unit)
{
    std::string key = _unit.toString();
    for(const auto &map : getConversionMap()){
        if (map.second.count(key) != 0){
            return true;
        }
    }
    return false;
}



} // namespace Common
} // namespace Tucuxi
