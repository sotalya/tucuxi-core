//@@license@@

#include "validdose.h"



namespace Tucuxi {
namespace Core {



ValidDoses::~ValidDoses() = default;

ValidDoses::ValidDoses(TucuUnit _unit, std::unique_ptr<PopulationValue> _defaultDose)
    : ValidValues(_unit, std::move(_defaultDose))
{
}

} // namespace Core
} // namespace Tucuxi
