#ifndef TUCUXI_CORE_OVERLOADEVALUATOR_H
#define TUCUXI_CORE_OVERLOADEVALUATOR_H

#include "tucucore/dosage.h"
#include "tucucore/computingservice/computingrequest.h"

namespace Tucuxi {
namespace Core {


class OverloadEvaluator
{
public:
    OverloadEvaluator();

    bool isAcceptable(IntakeSeries &_intakeSeries,
                      const ComputingTrait *_trait);
};

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_OVERLOADEVALUATOR_H
