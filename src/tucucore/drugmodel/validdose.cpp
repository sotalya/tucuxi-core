#include "validdose.h"



namespace Tucuxi {
namespace Core {

ValidDoses::ValidDoses(Unit _unit, MultiAnalyteDose _defaultDose) :
    m_unit(_unit), m_defaultDose(_defaultDose)
{

}

Unit ValidDoses::getUnit() const
{
    return m_unit;
}

MultiAnalyteDose ValidDoses::getDefaultDose() const
{
    return m_defaultDose;
}


const std::vector<std::string>& ValidDoses::getAnalyteIds() const
{
    return m_analyteIds;
}

void ValidDoses::setAnalyteIds(std::vector<std::string> _analyteIds)
{
    m_analyteIds = _analyteIds;
}

AnyDoses::AnyDoses(Unit _unit, MultiAnalyteDose _defaultDose, MultiAnalyteDose _from, MultiAnalyteDose _to, MultiAnalyteDose _step) :
    ValidDoses(_unit, _defaultDose), m_from(_from), m_to(_to), m_step(_step)
{

}

std::vector<MultiAnalyteDose> AnyDoses::getDoses() const
{
    MultiAnalyteDose currentDose = m_from;
    std::vector<MultiAnalyteDose> result;

    int nbAnalytes = m_from.size();

    while (currentDose <= m_to) {
        result.push_back(currentDose);
        for(int i = 0; i < nbAnalytes; i++) {
            currentDose[i] += m_step[i];
        }
    }
    return result;
}




SpecificDoses::SpecificDoses(Unit _unit, MultiAnalyteDose _defaultDose) :
    ValidDoses(_unit, _defaultDose)
{

}

void SpecificDoses::addDose(MultiAnalyteDose _dose)
{
    m_doses.push_back(_dose);
}

std::vector<MultiAnalyteDose> SpecificDoses::getDoses() const
{
    return m_doses;
}

}
}
