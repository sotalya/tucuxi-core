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


#ifndef TUCUXI_CORE_TARGETEVALUATIONRESULT_H
#define TUCUXI_CORE_TARGETEVALUATIONRESULT_H

#include "tucucore/definitions.h"
#include "tucucore/drugmodel/targetdefinition.h"
#include "tucucore/targetevent.h"

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
    //TargetEvaluationResult(TargetEvaluationResult&) = default;

    explicit TargetEvaluationResult() : m_targetType(TargetType::UnknownTarget) {}
    explicit TargetEvaluationResult(TargetType _targetType, double _score, Value _value, TucuUnit _unit);


    TargetType getTargetType() const
    {
        return m_targetType;
    }

    double getScore() const
    {
        return m_score;
    }

    Value getValue() const
    {
        return m_value;
    }

    TucuUnit getUnit() const
    {
        return m_unit;
    }

    void setTarget(TargetEvent _target)
    {
        m_target = std::move(_target);
    }

    const TargetEvent& getTarget() const
    {
        return m_target;
    }

protected:
    //! Type of target, as there should be only a single one of each type it is sufficient to discriminate
    TargetType m_targetType;

    //! Score of the target, calculated by the TargetEvaluator
    double m_score{0.0};

    //! Target value calculated by the TargetEvaluator
    Value m_value{0.0};

    //! Unit of the target
    TucuUnit m_unit{};

    //! The target used for this evaluation
    TargetEvent m_target;
};


} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_TARGETEVALUATIONRESULT_H
