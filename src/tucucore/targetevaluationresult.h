#ifndef TARGETEVALUATIONRESULT_H
#define TARGETEVALUATIONRESULT_H

#include "tucucore/drugmodel/targetdefinition.h"
#include "tucucore/definitions.h"

namespace Tucuxi {
namespace Core {

///
/// \brief The TargetEvaluationResult class
/// This class is meant to embed information about the evaluation of a specific target.
/// It is composed of the type of target, the calculated value, and the score.
///
class TargetEvaluationResult
{
public:
    explicit TargetEvaluationResult() : m_targetType(TargetType::UnknownTarget){}
    explicit TargetEvaluationResult(TargetType _targetType, double _score, Value _value);

    TargetType getTargetType() const { return m_targetType;}

    double getScore() const { return m_score;}

    Value getValue() const { return m_value;}

protected:

    //! Type of target, as there should be only a single one of each type it is sufficient to discriminate
    TargetType m_targetType;

    //! Score of the target, calculated by the TargetEvaluator
    double m_score;

    //! Target value calculated by the TargetEvaluator
    Value m_value;
};


} // namespace Core
} // namespace Tucuxi

#endif // TARGETEVALUATIONRESULT_H
