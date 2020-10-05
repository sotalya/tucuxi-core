#ifndef TUCUXI_COMMON_UNIT_H
#define TUCUXI_COMMON_UNIT_H

#include <string>
#include <map>

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
    TucuUnit(std::string _unitString) : m_unitString(_unitString){}

    ///
    /// \brief Compares two units for equality
    /// \param _rhs The unit to compare
    /// \return true if the units are equal, false else
    ///
    inline bool operator==(const TucuUnit& _rhs) const { return this->m_unitString == _rhs.m_unitString; }

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

    enum class UnitType{
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
    static double convertToUnit(double _value, TucuUnit _initialUnit, TucuUnit _finalUnit)
    {
        const auto conversionMap = getConversionMap().at(unitType);

        std::string initialKey = _initialUnit.toString();
        std::string finalKey = _finalUnit.toString();

        if ((conversionMap.count(initialKey) == 0) || (conversionMap.count(finalKey) == 0))
        {
            logConversionError(_initialUnit, _finalUnit);
            throw std::invalid_argument("Error in unit conversion");
        }

        return _value * conversionMap.at(initialKey) / conversionMap.at(finalKey);
    }

    template<UnitType unitType>
    static bool isOfType(TucuUnit _unit)
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
    static double convertToUnit(double _value, TucuUnit _initialUnit, TucuUnit _finalUnit);

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

private:

    static const std::map<UnitManager::UnitType, std::map<std::string, double>>& getConversionMap();

};


} // namespace Common
} // namespace Tucuxi


#endif // TUCUXI_COMMON_UNIT_H
