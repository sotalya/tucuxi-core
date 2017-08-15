#ifndef HARDCODEDOPERATION_H
#define HARDCODEDOPERATION_H

#include <algorithm>
#include <cmath>
#include <iostream>
#include <memory>

#include "tucucore/operation.h"

using namespace Tucuxi::Core;

namespace Tucuxi {
namespace Core {

#define HARDCODED_OPERATION(NAME)                                                   \
class NAME : public HardcodedOperation                                              \
{                                                                                   \
public:                                                                             \
    virtual std::unique_ptr<Operation> clone() const                                \
    {                                                                               \
        return std::unique_ptr<Operation>(new NAME(*this));                         \
    }                                                                               \
protected:                                                                          \
    virtual bool compute(const OperationInputList &_inputs, double &_result) const; \
    virtual void fillRequiredInputs();                                              \
};

/// \ingroup TucuCore
/// \brief Ideal Body Weight computation.
HARDCODED_OPERATION(IdealBodyWeight);

/// \ingroup TucuCore
/// \brief Body Surface Area computation.
HARDCODED_OPERATION(BodySurfaceArea);

/// \ingroup TucuCore
/// \brief Compute the eGFR value using the Cockcroft-Gault general equation.
HARDCODED_OPERATION(eGFR_CockcroftGaultGeneral);

/// \ingroup TucuCore
/// \brief Compute the eGFR value using the Cockcroft-Gault equation based on IBW.
HARDCODED_OPERATION(eGFR_CockcroftGaultIBW);

/// \ingroup TucuCore
/// \brief Compute the eGFR value using the Cockcroft-Gault equation based on the adjusted body weight.
HARDCODED_OPERATION(eGFR_CockcroftGaultAdjIBW);

/// \ingroup TucuCore
/// \brief Compute the **GFR** value using the MDRD approach.
HARDCODED_OPERATION(GFR_MDRD);

/// \ingroup TucuCore
/// \brief Compute the **GFR** value using the Chronic Kidney Disease Epidemiology Collaboration method.
HARDCODED_OPERATION(GFR_CKD_EPI);

/// \ingroup TucuCore
/// \brief Compute the eGFR value using the Schwartz (pediatric) method.
HARDCODED_OPERATION(eGFR_Schwartz);

/// \ingroup TucuCore
/// \brief Compute the **GFR** value using the Jelliffe method.
HARDCODED_OPERATION(GFR_Jelliffe);

/// \ingroup TucuCore
/// \brief Compute the eGFR value using the Salazar-Corcoran method (obese patients).
HARDCODED_OPERATION(eGFR_SalazarCorcoran);

}
}

#endif // HARDCODEDOPERATION_H
