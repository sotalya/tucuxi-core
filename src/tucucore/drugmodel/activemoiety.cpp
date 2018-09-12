#include "activemoiety.h"

namespace Tucuxi {
namespace Core {


ActiveMoiety::ActiveMoiety()
{

}

ActiveMoiety::ActiveMoiety(
        std::string _id,
        Unit _unit,
        std::vector<std::string> _analyteIds,
        std::unique_ptr<Operation> _formula)
    : m_id(_id), m_unit(_unit), m_analyteIds(_analyteIds), m_formula(std::move(_formula))
{

}

}
}
