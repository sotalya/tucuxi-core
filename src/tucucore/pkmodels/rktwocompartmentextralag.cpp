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

#include "tucucore/pkmodels/rktwocompartmentextralag.h"

#include "tucucore/intakeevent.h"

namespace Tucuxi {
namespace Core {

RK4TwoCompartmentExtraLagMicro::RK4TwoCompartmentExtraLagMicro()
    : IntakeIntervalCalculatorRK4Base<3, RK4TwoCompartmentExtraLagMicro>(
            std::make_unique<PertinentTimesCalculatorStandard>())
{
}

std::vector<std::string> RK4TwoCompartmentExtraLagMicro::getParametersId()
{
    return {"Ke", "V1", "K12", "K21", "Ka", "F", "Tlag"};
}

bool RK4TwoCompartmentExtraLagMicro::checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkCondition(_parameters.size() >= 5, "The number of parameters should be equal to 5.")) {
        return false;
    }

    m_D = _intakeEvent.getDose();
    m_V1 = _parameters.getValue(ParameterId::V1);
    m_Ke = _parameters.getValue(ParameterId::Ke);
    m_K12 = _parameters.getValue(ParameterId::K12);
    m_K21 = _parameters.getValue(ParameterId::K21);
    m_Ka = _parameters.getValue(ParameterId::Ka);
    m_F = _parameters.getValue(ParameterId::F);
    m_Tlag = _parameters.getValue(ParameterId::Tlag);

    m_nbPoints = static_cast<Eigen::Index>(_intakeEvent.getNbPoints());
    m_Int = (_intakeEvent.getInterval()).toHours();

    if (m_Tlag < 0.0) {
        m_Tlag = 0.0;
    }

    // check the inputs
    bool bOK = checkPositiveValue(m_D, "The dose");
    bOK &= checkStrictlyPositiveValue(m_Ka, "Ka");
    bOK &= checkStrictlyPositiveValue(m_F, "F");
    bOK &= checkStrictlyPositiveValue(m_V1, "V1");
    bOK &= checkStrictlyPositiveValue(m_Ke, "Ke");
    bOK &= checkPositiveValue(m_Tlag, "The lag time");
    bOK &= checkStrictlyPositiveValue(m_K12, "K12");
    bOK &= checkStrictlyPositiveValue(m_K21, "K21");
    bOK &= checkCondition(m_nbPoints > 0, "The number of points is zero or negative.");
    bOK &= checkCondition(m_Int > 0, "The interval time is negative.");

    return bOK;
}


RK4TwoCompartmentExtraLagMacro::RK4TwoCompartmentExtraLagMacro() {}


std::vector<std::string> RK4TwoCompartmentExtraLagMacro::getParametersId()
{
    return {"CL", "Q", "V1", "V2", "Ka", "F", "Tlag"};
}

bool RK4TwoCompartmentExtraLagMacro::checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkCondition(_parameters.size() >= 5, "The number of parameters should be equal to 5.")) {
        return false;
    }

    m_D = _intakeEvent.getDose();
    Value cl = _parameters.getValue(ParameterId::CL);
    Value q = _parameters.getValue(ParameterId::Q);
    Value v2 = _parameters.getValue(ParameterId::V2);
    m_V1 = _parameters.getValue(ParameterId::V1);
    m_Ka = _parameters.getValue(ParameterId::Ka);
    m_F = _parameters.getValue(ParameterId::F);
    m_Tlag = _parameters.getValue(ParameterId::Tlag);
    m_Ke = cl / m_V1;
    m_K12 = q / m_V1;
    m_K21 = q / v2;
    m_nbPoints = static_cast<Eigen::Index>(_intakeEvent.getNbPoints());
    m_Int = (_intakeEvent.getInterval()).toHours();

    if (m_Tlag < 0.0) {
        m_Tlag = 0.0;
    }

    // check the inputs
    bool bOK = checkPositiveValue(m_D, "The dose");
    bOK &= checkStrictlyPositiveValue(m_Ka, "Ka");
    bOK &= checkStrictlyPositiveValue(m_F, "F");
    bOK &= checkStrictlyPositiveValue(cl, "The clearance");
    bOK &= checkStrictlyPositiveValue(q, "Q");
    bOK &= checkStrictlyPositiveValue(m_V1, "V1");
    bOK &= checkStrictlyPositiveValue(v2, "V2");
    bOK &= checkCondition(m_nbPoints > 0, "The number of points is zero or negative.");
    bOK &= checkCondition(m_Int > 0, "The interval time is negative.");


    return bOK;
}

} // namespace Core
} // namespace Tucuxi
