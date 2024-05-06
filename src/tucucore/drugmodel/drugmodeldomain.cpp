//@@license@@

#include "drugmodeldomain.h"

#include "tucucore/operation.h"


namespace Tucuxi {
namespace Core {


DrugModelDomain::DrugModelDomain(std::unique_ptr<Constraint> _constraint)
{
    if (_constraint != nullptr) {
        m_constraints.push_back(std::move(_constraint));
    }
}

DrugModelDomain::~DrugModelDomain() = default;

} // namespace Core
} // namespace Tucuxi
