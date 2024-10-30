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


#include "rkmichaelismentenonecomp.h"

#include "tucucore/intakeevent.h"

namespace Tucuxi {
namespace Core {


RkMichaelisMentenOneComp::RkMichaelisMentenOneComp()
    : IntakeIntervalCalculatorRK4Base<2, RkMichaelisMentenOneComp>(std::make_unique<PertinentTimesCalculatorStandard>())
{
}


RkMichaelisMentenOneCompExtra::RkMichaelisMentenOneCompExtra() {}

std::vector<std::string> RkMichaelisMentenOneCompExtra::getParametersId()
{
    return {"V", "Km", "Vmax", "F", "Ka"};
}

bool RkMichaelisMentenOneCompExtra::checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkCondition(_parameters.size() >= 5, "The number of parameters should be equal to 5.")) {
        return false;
    }

    m_D = _intakeEvent.getDose();
    m_V = _parameters.getValue(ParameterId::V);
    m_Km = _parameters.getValue(ParameterId::Km);
    m_Vmax = _parameters.getValue(ParameterId::Vmax);
    m_F = _parameters.getValue(ParameterId::F);
    m_Ka = _parameters.getValue(ParameterId::Ka);
    m_nbPoints = _intakeEvent.getNbPoints();
    m_Int = (_intakeEvent.getInterval()).toHours();

    // check the inputs
    bool bOK = true;
    bOK &= checkPositiveValue(m_D, "The dose");
    bOK &= checkStrictlyPositiveValue(m_V, "The volume");
    bOK &= checkStrictlyPositiveValue(m_F, "The bioavailability");
    bOK &= checkStrictlyPositiveValue(m_Km, "The Michaelis Menten constant");
    bOK &= checkStrictlyPositiveValue(m_Vmax, "VMax");
    bOK &= checkStrictlyPositiveValue(m_Ka, "The absorption rate");
    bOK &= checkCondition(m_nbPoints > 0, "The number of points is zero or negative.");
    bOK &= checkCondition(m_Int > 0, "The interval time is negative.");

    return bOK;
}


RkMichaelisMentenOneCompBolus::RkMichaelisMentenOneCompBolus() {}

std::vector<std::string> RkMichaelisMentenOneCompBolus::getParametersId()
{
    return {"V", "Km", "Vmax"};
}

bool RkMichaelisMentenOneCompBolus::checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkCondition(_parameters.size() >= 3, "The number of parameters should be equal to 3.")) {
        return false;
    }

    m_D = _intakeEvent.getDose();
    m_V = _parameters.getValue(ParameterId::V);
    m_Km = _parameters.getValue(ParameterId::Km);
    m_Vmax = _parameters.getValue(ParameterId::Vmax);
    m_F = _parameters.getOptionalValue(ParameterId::F, 1.0);
    m_Ka = 0.0; // _parameters.getValue(ParameterId::Ka);
    m_nbPoints = _intakeEvent.getNbPoints();
    m_Int = (_intakeEvent.getInterval()).toHours();

    // check the inputs
    bool bOK = true;
    bOK &= checkPositiveValue(m_D, "The dose");
    bOK &= checkStrictlyPositiveValue(m_V, "The volume");
    bOK &= checkStrictlyPositiveValue(m_F, "The bioavailability");
    bOK &= checkStrictlyPositiveValue(m_Km, "The Michaelis Menten constant");
    bOK &= checkStrictlyPositiveValue(m_Vmax, "VMax");
    bOK &= checkPositiveValue(m_Ka, "The absorption rate");
    bOK &= checkCondition(m_nbPoints > 0, "The number of points is zero or negative.");
    bOK &= checkCondition(m_Int > 0, "The interval time is negative.");

    return bOK;
}


RkMichaelisMentenOneCompInfusion::RkMichaelisMentenOneCompInfusion()
{
    m_isInfusion = true;
}

std::vector<std::string> RkMichaelisMentenOneCompInfusion::getParametersId()
{
    return {"V", "Km", "Vmax"};
}

bool RkMichaelisMentenOneCompInfusion::checkInputs(
        const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkCondition(_parameters.size() >= 3, "The number of parameters should be equal to 3.")) {
        return false;
    }

    m_Tinf = (_intakeEvent.getInfusionTime()).toHours();
    m_nonDifferentiableTime = m_Tinf;

    const double eps = 0.001;
    m_TinfLow = m_Tinf - eps;
    m_TinfHigh = m_Tinf + eps;

    m_D = _intakeEvent.getDose();
    m_V = _parameters.getValue(ParameterId::V);
    m_Km = _parameters.getValue(ParameterId::Km);
    m_Vmax = _parameters.getValue(ParameterId::Vmax);
    m_F = _parameters.getOptionalValue(ParameterId::F, 1.0);
    m_Ka = _parameters.getOptionalValue(ParameterId::Ka, 0.0);
    m_nbPoints = _intakeEvent.getNbPoints();
    m_Int = (_intakeEvent.getInterval()).toHours();

    // check the inputs
    bool bOK = true;
    bOK &= checkPositiveValue(m_D, "The dose");
    bOK &= checkStrictlyPositiveValue(m_V, "The volume");
    bOK &= checkStrictlyPositiveValue(m_F, "The bioavailability");
    bOK &= checkStrictlyPositiveValue(m_Km, "The Michaelis Menten constant");
    bOK &= checkStrictlyPositiveValue(m_Vmax, "VMax");
    bOK &= checkPositiveValue(m_Ka, "The absorption rate");
    bOK &= checkCondition(m_nbPoints > 0, "The number of points is zero or negative.");
    bOK &= checkCondition(m_Int > 0, "The interval time is negative.");

    return bOK;
}




} // namespace Core
} // namespace Tucuxi
