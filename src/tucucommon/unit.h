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


#ifndef TUCUXI_COMMON_UNIT_H
#define TUCUXI_COMMON_UNIT_H

#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace Tucuxi {
namespace Common {

///
/// \brief The Unit class
///
/// This class is used to represent any unit used by a quantity.
/// It is very flexible in the sense that any string can be used to represent
/// a unit. However, converting a unit to another one requires the use of
/// standard known units.
///
/// The known units are:
///
/// TODO: List the known units
///
class TucuUnit
{
public:
    ///
    /// \brief Constructs an empty unit
    ///
    TucuUnit() {}

    ///
    /// \brief Construct a unit from a string
    /// \param _unitString The string representing the unit
    ///
    /// There is no requirements on the unit string. However some tests
    /// can be performed later on depending on a specific type of unit required
    /// at a specific place in code.
    ///
    TucuUnit(const std::string& _unitString) : m_unitString(_unitString) {}

    ///
    /// \brief Compares two units for equality
    /// \param _rhs The unit to compare
    /// \return true if the units are equal, false else
    ///
    inline bool operator==(const TucuUnit& _rhs) const
    {
        return this->m_unitString == _rhs.m_unitString;
    }

    ///
    /// \brief Converts the unit to a string
    /// \return The string corresponding to the unit
    ///
    std::string toString() const;

    ///
    /// \brief Indicates if the unit is actually an empty unit
    /// \return true if there is no unit, false else
    ///
    bool isEmpty() const;


protected:
    /// A unit is simply represented as a string
    std::string m_unitString;
};

///
/// \brief The UnitManager class
///
/// This class is meant to manage know units and conversions.
///
/// Currently all conversions are hardcoded, but a unit manager could be designed
/// to load this information from a file, to be more flexible. That would allow
/// a user to use its own units in case covariates expose unkown units.
///
class UnitManager
{
public:
    enum class UnitType
    {
        Weight = 0,
        Concentration,
        MoleConcentration,
        Time,
        ConcentrationTime,
        Length,
        MolarMass,
        FlowRate,
        Temperature,
        NoUnit,
        Undefined
    };


    ///
    /// \brief Converts a specific unit type to another unit of the same type
    /// \param _value
    /// \param _initialUnit
    /// \param _finalUnit
    /// \return converted value
    ///
    template<UnitType unitType>
    static double convertToUnit(double _value, const TucuUnit& _initialUnit, const TucuUnit& _finalUnit)
    {
        const auto conversionMap = getConversionMap().at(unitType);

        std::string initialKey = _initialUnit.toString();
        std::string finalKey = _finalUnit.toString();

        if ((conversionMap.count(initialKey) == 0) || (conversionMap.count(finalKey) == 0)) {
            logConversionError(_initialUnit, _finalUnit);
            throw std::invalid_argument("Error in unit conversion");
        }

        return _value / conversionMap.at(finalKey) * conversionMap.at(initialKey);
    }
    /*
    ///
    /// \brief Converts a specific unit type to another unit of the same type on a vector
    /// \param _value
    /// \param _initialUnit
    /// \param _finalUnit
    /// \return converted vector
    ///
    template<UnitType unitType>
    static std::vector<double> convertToUnit(
            const std::vector<double>& _value, const TucuUnit& _initialUnit, const TucuUnit& _finalUnit)
    {
        std::vector<double> result(_value.size());
        const auto conversionMap = getConversionMap().at(unitType);

        std::string initialKey = _initialUnit.toString();
        std::string finalKey = _finalUnit.toString();

        if ((conversionMap.count(initialKey) == 0) || (conversionMap.count(finalKey) == 0)) {
            logConversionError(_initialUnit, _finalUnit);
            throw std::invalid_argument("Error in unit conversion");
        }

        double factor = 1.0 / conversionMap.at(finalKey) * conversionMap.at(initialKey);

        for (size_t i = 0; i < _value.size(); i++) {
            result[i] = _value[i] * factor;
        }

        return result;
    }
*/

    ///
    /// \brief Converts a specific unit type to another unit of the same type on a vector
    /// \param _value
    /// \param _initialUnit
    /// \param _finalUnit
    /// \return converted vector
    ///
    template<UnitType unitType, typename TVector>
    static TVector convertToUnit(const TVector& _value, const TucuUnit& _initialUnit, const TucuUnit& _finalUnit)
    {
        TVector result(_value.size());
        const auto conversionMap = getConversionMap().at(unitType);

        std::string initialKey = _initialUnit.toString();
        std::string finalKey = _finalUnit.toString();

        if ((conversionMap.count(initialKey) == 0) || (conversionMap.count(finalKey) == 0)) {
            logConversionError(_initialUnit, _finalUnit);
            throw std::invalid_argument("Error in unit conversion");
        }

        double factor = 1.0 / conversionMap.at(finalKey) * conversionMap.at(initialKey);

        for (size_t i = 0; i < _value.size(); i++) {
            result[i] = _value[i] * factor;
        }

        return result;
    }

    ///
    /// \brief Converts a specific unit type to another unit of the same type on a vector
    /// \param _value
    /// \param _initialUnit
    /// \param _finalUnit
    ///
    template<UnitType unitType, typename TVector>
    static void updateAndConvertToUnit(TVector& _value, const TucuUnit& _initialUnit, const TucuUnit& _finalUnit)
    {
        const auto conversionMap = getConversionMap().at(unitType);

        std::string initialKey = _initialUnit.toString();
        std::string finalKey = _finalUnit.toString();

        if ((conversionMap.count(initialKey) == 0) || (conversionMap.count(finalKey) == 0)) {
            logConversionError(_initialUnit, _finalUnit);
            throw std::invalid_argument("Error in unit conversion");
        }

        double factor = 1.0 / conversionMap.at(finalKey) * conversionMap.at(initialKey);

        for (size_t i = 0; i < _value.size(); i++) {
            _value[i] = _value[i] * factor;
        }
    }

    /*
     *
    ///
    /// \brief Converts a specific unit type to another unit of the same type on a vector
    /// \param _value
    /// \param _initialUnit
    /// \param _finalUnit
    ///
    template<UnitType unitType>
    static void updateAndConvertToUnit(
            std::vector<double>& _value, const TucuUnit& _initialUnit, const TucuUnit& _finalUnit)
    {
        const auto conversionMap = getConversionMap().at(unitType);

    std::string initialKey = _initialUnit.toString();
    std::string finalKey = _finalUnit.toString();

    if ((conversionMap.count(initialKey) == 0) || (conversionMap.count(finalKey) == 0)) {
        logConversionError(_initialUnit, _finalUnit);
        throw std::invalid_argument("Error in unit conversion");
    }

    double factor = 1.0 / conversionMap.at(finalKey) * conversionMap.at(initialKey);

    for (size_t i = 0; i < _value.size(); i++) {
        _value[i] = _value[i] * factor;
    }
}*/

    template<UnitType unitType>
    static bool isOfType(const TucuUnit& _unit)
    {
        const auto conversionMap = getConversionMap().at(unitType);

        auto found = conversionMap.find(_unit.toString());

        return (found != conversionMap.end());
    }

    ///
    /// \brief Converts a non-specified unit to another
    /// \param _value
    /// \param _initialUnit
    /// \param _finalUnit
    /// \return converted value
    ///
    static double convertToUnit(double _value, const TucuUnit& _initialUnit, const TucuUnit& _finalUnit);

    static void logConversionError(const TucuUnit& _initialUnit, const TucuUnit& _finalUnit);

    ///
    /// \brief Indicates if this unit is a known one
    /// \param _unit The unit to be tested
    /// \return true if the unit is known, false else
    ///
    /// This function allows to check whether a unit can be used in a conversion.
    /// For instance, concentrations, doses, AUC and times have known units.
    ///
    static bool isKnown(const TucuUnit& _unit);

    ///
    /// \brief Checks if a unit is compatible with another one
    /// \param _unit1 The first unit to compare
    /// \param _unit2 The second unit to compare
    /// \return true if the units are compatible, false else
    ///
    /// For instance, Unit("ug/l") is compatible with Unit("mg/l"),
    /// while Unit("ug/l") is not compatible with Unit("ug").
    ///
    static bool isCompatible(const TucuUnit& _unit1, const TucuUnit& _unit2);

    ///
    /// \brief Checks if a unit is tolerate during import
    /// and convert it to a known one for Tucuxi
    /// \param _unitString The unit string extracted from query
    /// \return true if the units are compatible, false else
    ///
    static bool isUnitTolerated(std::string& _unitString);

    ///
    /// \brief Gets the weight corresponding to a concentration unit
    /// \param _unit The concentration unit
    /// \return A weight unit
    ///
    /// Examples:
    /// -   ug/l    -> ug
    /// -   g/l     -> g
    /// -   ug/ml   -> ug
    ///
    static TucuUnit getWeightFromConcentration(const TucuUnit& _unit);

private:
    static const std::map<UnitManager::UnitType, std::map<std::string, double>>& getConversionMap();
};


} // namespace Common
} // namespace Tucuxi


#endif // TUCUXI_COMMON_UNIT_H
