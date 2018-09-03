#include "validdose.h"



namespace Tucuxi {
namespace Core {



ValidDoses::~ValidDoses()
{

}

ValidDoses::ValidDoses(Unit _unit, std::unique_ptr<PopulationValue> _defaultDose) :
    ValidValues (_unit, std::move(_defaultDose))
{

}


void ValidDoses::setAnalyteIds(std::vector<std::string> _analyteIds)
{
    m_analyteIds = _analyteIds;
}

const std::vector<std::string>& ValidDoses::getAnalyteIds() const
{
    return m_analyteIds;
}



}
}
