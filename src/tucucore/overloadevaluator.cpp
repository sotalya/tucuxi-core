#include "overloadevaluator.h"

#include "tucucore/computingservice/computingtrait.h"

namespace Tucuxi {
namespace Core {

OverloadEvaluator::OverloadEvaluator()
{

}

bool OverloadEvaluator::isAcceptable(IntakeSeries &_intakeSeries,
                                     const ComputingTrait *_trait)
{
    if (dynamic_cast<const ComputingTraitPercentiles *>(_trait))
    {

    }
    return true;
}

} // namespace Core
} // namespace Tucuxi
