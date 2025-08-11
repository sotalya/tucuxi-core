/*
 * Tucuxi - Tucuxi-core library and command line tool.
 * This code allows to perform prediction of drug concentration in blood
 * and to propose dosage adaptations.
 * It has been developed by HEIG-VD, in close collaboration with CHUV.
 * Copyright (C) 2023 HEIG-VD, maintained by Yann Thoma  <yann.thoma@heig-vd.ch>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */


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
