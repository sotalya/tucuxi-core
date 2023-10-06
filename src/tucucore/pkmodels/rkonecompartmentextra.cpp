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


#include <Eigen/Dense>
#include <Eigen/LU>

#include "tucucore/pkmodels/rkonecompartmentextra.h"

#include "tucucore/intakeevent.h"

namespace Tucuxi {
namespace Core {

RK4OneCompartmentExtraMicro::RK4OneCompartmentExtraMicro()
    : IntakeIntervalCalculatorRK4Base<2, RK4OneCompartmentExtraMicro>(
            std::make_unique<PertinentTimesCalculatorStandard>())
{
}

bool RK4OneCompartmentExtraMicro::checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkCondition(_parameters.size() >= 4, "The number of parameters should be equal to 4.")) {
        return false;
    }

    m_D = _intakeEvent.getDose();
    m_V = _parameters.getValue(ParameterId::V);
    m_Ke = _parameters.getValue(ParameterId::Ke);
    m_Ka = _parameters.getValue(ParameterId::Ka);
    m_F = _parameters.getValue(ParameterId::F);
    m_nbPoints = _intakeEvent.getNbPoints();
    m_Int = (_intakeEvent.getInterval()).toHours();

    // check the inputs
    bool bOK = true;
    bOK &= checkPositiveValue(m_D, "The dose");
    bOK &= checkStrictlyPositiveValue(m_V, "The volume");
    bOK &= checkStrictlyPositiveValue(m_F, "The bioavailability");
    bOK &= checkStrictlyPositiveValue(m_Ke, "The absorption constant");
    bOK &= checkStrictlyPositiveValue(m_Ka, "The clearance");
    bOK &= checkCondition(m_nbPoints > 0, "The number of points is zero or negative.");
    bOK &= checkCondition(m_Int > 0, "The interval time is negative.");

    return bOK;
}


RK4OneCompartmentExtraMacro::RK4OneCompartmentExtraMacro() : RK4OneCompartmentExtraMicro() {}

bool RK4OneCompartmentExtraMacro::checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkCondition(_parameters.size() >= 4, "The number of parameters should be equal to 4.")) {
        return false;
    }

    m_D = _intakeEvent.getDose();
    m_V = _parameters.getValue(ParameterId::V);
    Value cl = _parameters.getValue(ParameterId::CL); // clearance
    m_Ka = _parameters.getValue(ParameterId::Ka);
    m_F = _parameters.getValue(ParameterId::F);
    m_Ke = cl / m_V;
    m_nbPoints = _intakeEvent.getNbPoints();
    m_Int = (_intakeEvent.getInterval()).toHours();

    // check the inputs
    bool bOK = true;
    bOK &= checkPositiveValue(m_D, "The dose");
    bOK &= checkStrictlyPositiveValue(m_V, "The volume");
    bOK &= checkStrictlyPositiveValue(m_F, "The bioavailability");
    bOK &= checkStrictlyPositiveValue(m_Ke, "The absorption constant");
    bOK &= checkStrictlyPositiveValue(m_Ka, "The clearance");
    bOK &= checkCondition(m_nbPoints > 0, "The number of points is zero or negative.");
    bOK &= checkCondition(m_Int > 0, "The interval time is negative.");

    return bOK;
}

} // namespace Core
} // namespace Tucuxi
