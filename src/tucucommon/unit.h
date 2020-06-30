#ifndef TUCUXI_COMMON_UNIT_H
#define TUCUXI_COMMON_UNIT_H

#include <string>

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
class Unit
{
public:

    ///
    /// \brief Constructs an empty unit
    ///
    Unit() {}

    ///
    /// \brief Construct a unit from a string
    /// \param _unitString The string representing the unit
    ///
    /// There is no requirements on the unit string. However some tests
    /// can be performed later on depending on a specific type of unit required
    /// at a specific place in code.
    ///
    Unit(std::string _unitString) : m_unitString(_unitString){}

    ///
    /// \brief Compares two units for equality
    /// \param _rhs The unit to compare
    /// \return true if the units are equal, false else
    ///
    inline bool operator==(const Unit& _rhs) const { return this->m_unitString == _rhs.m_unitString; }

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

    ///
    /// \brief Indicates if the unit corresponds to a time unit
    /// \return true if it is a time unit, false else
    ///
    /// A time is expressed in hours or any of its derivatives
    ///
    bool isTime() const;

    ///
    /// \brief Indicates if the unit corresponds to a concentration
    /// \return true if it is a concentration unit, false else
    ///
    /// A concentration is expressed in grams per liter or any of its derivatives
    ///
    bool isConcentration() const;

    ///
    /// \brief Indicates if the unit corresponds to an area under curve
    /// \return true if it is an area under curve, false else
    ///
    /// An area under curve is expressed in grams*hours/liters or any of its derivatives
    ///
    bool isAuc() const;

    ///
    /// \brief Indicates if the unit corresponds to a dose unit
    /// \return true if it is a dose unit, false else
    ///
    /// A dose is expressed in grams or any of its derivatives
    ///
    bool isDose() const;

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

    ///
    /// \brief Indicates if this unit is a known one
    /// \param _unit The unit to be tested
    /// \return true if the unit is known, false else
    ///
    /// This function allows to check whether a unit can be used in a conversion.
    /// For instance, concentrations, doses, AUC and times have known units.
    ///
    static bool isKnown(const Unit& _unit);

    ///
    /// \brief Checks if a unit is compatible with another one
    /// \param _unit1 The first unit to compare
    /// \param _unit2 The second unit to compare
    /// \return true if the units are compatible, false else
    ///
    /// For instance, Unit("ug/l") is compatible with Unit("mg/l"),
    /// while Unit("ug/l") is not compatible with Unit("ug").
    ///
    static bool isCompatible(const Unit& _unit1, const Unit& _unit2);

    ///
    /// \brief Translates a value from one unit to a compatible unit
    /// \param _value The value to convert
    /// \param _initialUnit The initial unit
    /// \param _finalUnit The final unit
    /// \return The value represented in the final unit
    ///
    /// If the units are not compatible, then the returned value is 0.0.
    ///
    static double translateToUnit(double _value, Unit _initialUnit, Unit _finalUnit);

    ///
    /// \brief Translation factor from one unit to another compatible one
    /// \param _initialUnit The initial unit
    /// \param _finalUnit The final unit
    /// \return The factor by which a value in the initial unit should be multiplied to get a value in the final unit
    ///
    /// If the units are not compatible, then the returned value is 0.0.
    ///
    static double translationFactor(Unit _initialUnit, Unit _finalUnit);

};


} // namespace Common
} // namespace Tucuxi


#endif // TUCUXI_COMMON_UNIT_H
