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
    // Calculate the total number of points to be calculated
    int nbPoints = 0;
    for (auto intake : _intakeSeries) {
        nbPoints += intake.getNbPoints();
    }

    if (dynamic_cast<const ComputingTraitPercentiles *>(_trait) != nullptr)
    {
        // If percentiles, then it is more critical

        // Arbitrary 100000 points. Could be changed later on or depend on
        // the system
        if (nbPoints > 100000) {
            return false;
        }
    }
    return true;
}

} // namespace Core
} // namespace Tucuxi
