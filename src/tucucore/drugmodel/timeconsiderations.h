#ifndef TIMECONSIDERATIONS_H
#define TIMECONSIDERATIONS_H

#include <memory>
#include "tucucore/drugdefinitions.h"

namespace Tucuxi {
namespace Core {

class HalfLife : public PopulationValue
{
public:
    HalfLife(const std::string _id, Value _value, Unit _unit, double _multiplier, Operation *_operation = nullptr)
        : PopulationValue(_id, _value, _operation),
          m_multiplier(_multiplier), m_unit(_unit)
    {}

    double getMultiplier() const { return m_multiplier;}
    Unit getUnit() const { return m_unit;}

private:
    double m_multiplier;

    Unit m_unit;
};

class OutdatedMeasure : public PopulationValue
{

private:
    Unit m_unit;

};

class TimeConsiderations
{
public:
    TimeConsiderations();

    void setHalfLife(std::unique_ptr<HalfLife> _halfLife) { m_halfLife = std::move(_halfLife);}

    const HalfLife & getHalfLife() const { return *m_halfLife;}



private:

    std::unique_ptr<HalfLife> m_halfLife;
    std::unique_ptr<OutdatedMeasure> m_outdatedMeasure;
};

} // namespace Core
} // namespace Tucuxi

#endif // TIMECONSIDERATIONS_H
