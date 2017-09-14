#include "validdose.h"



namespace Tucuxi {
namespace Core {

ValidDoses::ValidDoses(Unit _unit, Value _defaultDose) :
    m_unit(_unit), m_defaultDose(_defaultDose)
{

}

Unit ValidDoses::getUnit() const
{
    return m_unit;
}

Value ValidDoses::getDefaultDose() const
{
    return m_defaultDose;
}

AnyDoses::AnyDoses(Unit _unit, Value _defaultDose, Value _from, Value _to, Value _step) :
    ValidDoses(_unit, _defaultDose), m_from(_from), m_to(_to), m_step(_step)
{

}

std::vector<Value> AnyDoses::getDoses() const
{
    Value currentDose = m_from;
    std::vector<Value> result;

    while (currentDose <= m_to) {
        result.push_back(currentDose);
        currentDose += m_step;
    }
    return result;
}




SpecificDoses::SpecificDoses(Unit _unit, Value _defaultDose) :
    ValidDoses(_unit, _defaultDose)
{

}

std::vector<Value> SpecificDoses::getDoses() const
{
    return m_doses;
}

}
}
