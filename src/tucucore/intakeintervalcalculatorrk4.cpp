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


#include "intakeintervalcalculatorrk4.h"

#include "intakeevent.h"

namespace Tucuxi {
namespace Core {

IntakeIntervalCalculatorRK4::~IntakeIntervalCalculatorRK4() = default;


ComputingStatus IntakeIntervalCalculatorRK4::calculateIntakePoints(
        MultiCompConcentrations& _concentrations,
        TimeOffsets& _times,
        const IntakeEvent& _intakeEvent,
        const ParameterSetEvent& _parameters,
        const Residuals& _inResiduals,
        bool _isAll,
        Residuals& _outResiduals,
        const bool _isDensityConstant)
{
    if (m_firstCalculation) {
        m_firstCalculation = false;
        m_lastThreadId = std::this_thread::get_id();
    }
    else {
        if (m_lastThreadId != std::this_thread::get_id()) {
            // Somthing strange. It should not be used by another thread
            // Maybe raise an error here
        }
    }
    TMP_UNUSED_PARAMETER(_isDensityConstant);
    if (!checkInputs(_intakeEvent, _parameters)) {
        return ComputingStatus::BadParameters;
    }

    CHECK_CALCULATEINTAKEPOINTS_INPUTS;

    // Create our serie of times
    CycleSize nbPoints = _intakeEvent.getNbPoints();

    Eigen::VectorXd times(nbPoints);
    m_pertinentTimesCalculator->calculateTimes(_intakeEvent, static_cast<Eigen::Index>(nbPoints), times);

    if (!computeConcentrations(times, _inResiduals, _isAll, _concentrations, _outResiduals)) {
        return ComputingStatus::BadConcentration;
    }

    times = times.array() + _intakeEvent.getOffsetTime().toHours();
    _times.assign(times.data(), times.data() + times.size());

    return ComputingStatus::Ok;
}


ComputingStatus IntakeIntervalCalculatorRK4::calculateIntakeSinglePoint(
        MultiCompConcentrations& _concentrations,
        const IntakeEvent& _intakeEvent,
        const ParameterSetEvent& _parameters,
        const Residuals& _inResiduals,
        const Value& _atTime,
        bool _isAll,
        Residuals& _outResiduals)
{
    if (m_firstCalculation) {
        m_firstCalculation = false;
        m_lastThreadId = std::this_thread::get_id();
    }
    else {
        if (m_lastThreadId != std::this_thread::get_id()) {
            // Something strange. It should not be used by another thread
            // Maybe raise an error here
        }
    }

    if (!checkInputs(_intakeEvent, _parameters)) {
        return ComputingStatus::BadParameters;
    }

    CHECK_CALCULATEINTAKESINGLEPOINT_INPUTS;

    // To reuse interface of computeExponentials with multiple points, remaine time as a vector.
    Eigen::VectorXd times(2);
    times << static_cast<double>(_atTime), static_cast<double>(_intakeEvent.getInterval().toHours());

    if (!computeConcentration(_atTime, _inResiduals, _isAll, _concentrations, _outResiduals)) {
        return ComputingStatus::BadConcentration;
    }

    return ComputingStatus::Ok;
}



} // namespace Core
} // namespace Tucuxi
