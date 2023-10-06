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


#ifndef TUCUXI_CORE_TARGETEVALUATOR_H
#define TUCUXI_CORE_TARGETEVALUATOR_H

// We need this include because IntakeSeries is a typedef, and as such can not be forward declared
#include "tucucore/computingservice/computingresponse.h"
#include "tucucore/computingservice/computingresult.h"
#include "tucucore/intakeevent.h"
#include "tucucore/targetevent.h"

namespace Tucuxi {
namespace Core {

class ConcentrationPrediction;
class Target;
class TargetEvaluationResult;

///
/// \brief The TargetEvaluator class is meant to evaluate a target on a prediction
///
/// It only offers a single evaluation function that is to be called when evaluating
/// candidates for a dosage adjustment.
///
class TargetEvaluator
{
public:
    TargetEvaluator();

    ///
    /// \brief evaluates a specific target on a prediction
    /// \param _prediction The calculated prediction
    /// \param _intakeSeries The intake series used for calculating the prediction
    /// \param _target The target to be evaluated
    /// \param _result The result of the evaluation
    /// \return Result::Ok if everything went well, Result::InvalidCandidate if the candidate is outside the boundaries, Result::EvaluationError if there is an internal with the evaluation
    ///
    ComputingStatus evaluate(
            const ConcentrationPrediction& _prediction,
            const Tucuxi::Core::IntakeSeries& _intakeSeries,
            const TargetEvent& _targetEvent,
            TargetEvaluationResult& _result);

    ///
    /// \brief aucOverMicCalculator
    /// \param _target : The target in evaluation
    /// \param _cycle : Cycle containing times
    /// \return double value corresponding to Area under Curve which is over the mic value
    ///
    double aucOverMicCalculator(const TargetEvent& _targetEvent, CycleData& _cycle);

protected:
    ///
    /// \brief Checks if the value is inside the boundaries
    /// \param _value Value to be checked
    /// \param _min Minimum value of the range
    /// \param _max Maximum value of the range
    /// \return true if the value is within [_min - delta, _max + delta], false else
    ///
    /// delta is a value defined within the function as 1e-07. It allows to tolerate
    /// some floating point operations roundings.
    ///
    bool isWithinBoundaries(Value _value, Value _min, Value _max);


    ///
    /// \brief Evaluates if a value is within the boundaries and update some values
    /// \param _value The value to be checked
    /// \param _target The target
    /// \param _ok Output value modified if the value is outside the boundaries
    /// \param _score Score of the value
    /// \param _outputValue Contains the value _value if _value is inside the boundaries
    ///
    /// _ok, _score and _outputValue are modified within the function.
    /// _ok will be false if _value is outside [_target.m_minValue, _target.m_maxValue]
    /// (see isWithinBoundaries() for details).
    /// _score is only modified if _ok is true and will contain the score calculation
    /// _outputValue is only modified if _ok is true and will contain _value
    ///
    void evaluateValue(Value _value, const TargetEvent& _targetEvent, bool& _ok, double& _score, double& _outputValue);
};

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_TARGETEVALUATOR_H
