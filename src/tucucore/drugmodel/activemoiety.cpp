//@@license@@

#include <utility>

#include "activemoiety.h"

namespace Tucuxi {
namespace Core {


ActiveMoiety::ActiveMoiety() : m_id("") {}

ActiveMoiety::ActiveMoiety(
        ActiveMoietyId _id, TucuUnit _unit, std::vector<AnalyteId> _analyteIds, std::unique_ptr<Operation> _formula)
    : m_id(std::move(_id)), m_unit(std::move(_unit)), m_analyteIds(std::move(_analyteIds)),
      m_formula(std::move(_formula))
{
}

} // namespace Core
} // namespace Tucuxi
