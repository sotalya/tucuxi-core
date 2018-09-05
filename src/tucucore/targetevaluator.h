#ifndef TUCUXI_CORE_TARGETEVALUATOR_H
#define TUCUXI_CORE_TARGETEVALUATOR_H

// We need this include because IntakeSeries is a typedef, and as such can not be forward declared
#include "tucucore/dosage.h"

namespace Tucuxi {
namespace Core {

class ConcentrationPrediction;
class Target;
class TargetEvaluationResult;

class TargetEvaluator
{
public:

    enum class Result {
        Ok,
        EvaluationError
    };

    TargetEvaluator();

    Result evaluate(
            const ConcentrationPrediction& _prediction,
            const Tucuxi::Core::IntakeSeries &_intakeSeries,
            const Target& _target,
            TargetEvaluationResult & _result);
};

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_TARGETEVALUATOR_H
