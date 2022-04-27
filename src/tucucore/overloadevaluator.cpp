//@@lisence@@

#include <sstream>

#include "overloadevaluator.h"

#include "tucucore/computingservice/computingtrait.h"

namespace Tucuxi {
namespace Core {

OverloadEvaluator::OverloadEvaluator() : OverloadEvaluator(10000, 2000, 10000)
{
    // Arbitrary numbers here.
}

OverloadEvaluator::OverloadEvaluator(int _nbPredictionPoints, int _nbPercentilesPoints, int _nbDosagePossibilities)
    : m_nbPredictionPoints(_nbPredictionPoints), m_nbPercentilePoints(_nbPercentilesPoints),
      m_nbDosagePossibilities(_nbDosagePossibilities)
{
}

std::string OverloadEvaluator::getErrorMessage() const
{
    return m_errorMessage;
}

void OverloadEvaluator::setValues(int _nbPredictionPoints, int _nbPercentilesPoints, int _nbDosagePossibilities)
{
    m_nbPredictionPoints = _nbPredictionPoints;
    m_nbPercentilePoints = _nbPercentilesPoints;
    m_nbDosagePossibilities = _nbDosagePossibilities;
}

bool OverloadEvaluator::isAcceptable(IntakeSeries& _intakeSeries, const ComputingTrait* _trait)
{
    // Calculate the total number of points to be calculated
    CycleSize nbPoints = 0;
    for (const auto& intake : _intakeSeries) {
        nbPoints += intake.getNbPoints();
    }

    if (dynamic_cast<const ComputingTraitConcentration*>(_trait) != nullptr) {
        if (nbPoints > m_nbPredictionPoints) {
            std::stringstream ss;
            ss << "Asking for " << nbPoints << " points, but the maximum allowed is " << m_nbPredictionPoints;
            m_errorMessage = ss.str();
            return false;
        }
    }
    else if (dynamic_cast<const ComputingTraitPercentiles*>(_trait) != nullptr) {
        if (nbPoints > m_nbPercentilePoints) {
            std::stringstream ss;
            ss << "Asking for " << nbPoints << " points for percentiles, but the maximum allowed is "
               << m_nbPercentilePoints;
            m_errorMessage = ss.str();
            return false;
        }
    }
    else if (dynamic_cast<const ComputingTraitAdjustment*>(_trait) != nullptr) {
        // Not implemented yet, actually depends on the drug model that could be
        // statically checked
    }
    return true;
}

OverloadEvaluator* SingleOverloadEvaluator::getInstance()
{

    static std::unique_ptr<OverloadEvaluator> overloadEvaluator;
    if (overloadEvaluator == nullptr) {
        overloadEvaluator = std::make_unique<OverloadEvaluator>();
    }
    return overloadEvaluator.get();
}


} // namespace Core
} // namespace Tucuxi
