/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "tucucore/covariateevent.h"


namespace Tucuxi {
namespace Core {

bool CovariateEvent::evaluate(const OperableGraphManager &_graphMgr)
{
    Operation &op = getOperation();

    // Collect inputs
    OperationInputList inputs = getInputs();

    for (auto &input : inputs) {
        double val;
        bool rc;

        rc = _graphMgr.getValue(input.getName(), val);
        if (!rc) {
            return false;
        }

        rc = input.setValue(val);
        if (!rc) {
            return false;
        }

    }

    return op.evaluate(inputs, m_value);
}

} // namespace Core
} // namespace Tucuxi
