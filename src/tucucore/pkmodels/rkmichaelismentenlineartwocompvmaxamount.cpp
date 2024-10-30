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


#include "rkmichaelismentenlineartwocompvmaxamount.h"

#include "tucucore/intakeevent.h"

namespace Tucuxi {
namespace Core {


RkMichaelisMentenLinearTwoCompVmaxAmount::RkMichaelisMentenLinearTwoCompVmaxAmount()
    : IntakeIntervalCalculatorRK4Base<3, RkMichaelisMentenLinearTwoCompVmaxAmount>(
            std::make_unique<PertinentTimesCalculatorStandard>())
{
    m_Tinf = 0.0;
    m_Tlag = 0.0;
}


RkMichaelisMentenLinearTwoCompVmaxAmountExtraMicro::RkMichaelisMentenLinearTwoCompVmaxAmountExtraMicro()
{
    m_delivered = true;
    m_isWithLag = false;
}

std::vector<std::string> RkMichaelisMentenLinearTwoCompVmaxAmountExtraMicro::getParametersId()
{
    return {"V1", "Km", "Vmax", "Ke", "K12", "K21", "F", "Ka"};
}

bool RkMichaelisMentenLinearTwoCompVmaxAmountExtraMicro::checkInputs(
        const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkCondition(_parameters.size() >= 8, "The number of parameters should be equal to 8.")) {
        return false;
    }

    m_D = _intakeEvent.getDose();
    m_V1 = _parameters.getValue(ParameterId::V1);
    m_Km = _parameters.getValue(ParameterId::Km);
    m_Vmax = _parameters.getValue(ParameterId::Vmax);
    m_Ke = _parameters.getValue(ParameterId::Ke);
    m_F = _parameters.getValue(ParameterId::F);
    m_Ka = _parameters.getValue(ParameterId::Ka);
    m_K12 = _parameters.getValue(ParameterId::K12);
    m_K21 = _parameters.getValue(ParameterId::K21);
    m_nbPoints = _intakeEvent.getNbPoints();
    m_Int = (_intakeEvent.getInterval()).toHours();

    // check the inputs
    bool bOK = true;
    bOK &= checkPositiveValue(m_D, "The dose");
    bOK &= checkStrictlyPositiveValue(m_V1, "The volume");
    bOK &= checkStrictlyPositiveValue(m_F, "The bioavailability");
    bOK &= checkStrictlyPositiveValue(m_Km, "The Michaelis Menten constant");
    bOK &= checkStrictlyPositiveValue(m_Vmax, "VMax");
    bOK &= checkPositiveValue(m_Ke, "Ke");
    bOK &= checkStrictlyPositiveValue(m_Ka, "The absorption rate");
    bOK &= checkPositiveValue(m_Tlag, "The lag time");
    bOK &= checkPositiveValue(m_K12, "K12");
    bOK &= checkPositiveValue(m_K21, "K21");
    bOK &= checkCondition(m_nbPoints > 0, "The number of points is zero or negative.");
    bOK &= checkCondition(m_Int > 0, "The interval time is negative.");

    return bOK;
}


RkMichaelisMentenLinearTwoCompVmaxAmountExtraLagMicro::RkMichaelisMentenLinearTwoCompVmaxAmountExtraLagMicro()
{
    m_delivered = false;
    m_isWithLag = true;
}

std::vector<std::string> RkMichaelisMentenLinearTwoCompVmaxAmountExtraLagMicro::getParametersId()
{
    return {"V1", "Km", "Vmax", "Ke", "K12", "K21", "F", "Ka", "Tlag"};
}

bool RkMichaelisMentenLinearTwoCompVmaxAmountExtraLagMicro::checkInputs(
        const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkCondition(_parameters.size() >= 9, "The number of parameters should be equal to 9.")) {
        return false;
    }

    m_D = _intakeEvent.getDose();
    m_V1 = _parameters.getValue(ParameterId::V1);
    m_Km = _parameters.getValue(ParameterId::Km);
    m_Vmax = _parameters.getValue(ParameterId::Vmax);
    m_Ke = _parameters.getValue(ParameterId::Ke);
    m_F = _parameters.getValue(ParameterId::F);
    m_Ka = _parameters.getValue(ParameterId::Ka);
    m_K12 = _parameters.getValue(ParameterId::K12);
    m_K21 = _parameters.getValue(ParameterId::K21);
    m_Tlag = _parameters.getValue(ParameterId::Tlag);
    m_nbPoints = _intakeEvent.getNbPoints();
    m_Int = (_intakeEvent.getInterval()).toHours();

    // check the inputs
    bool bOK = true;
    bOK &= checkPositiveValue(m_D, "The dose");
    bOK &= checkStrictlyPositiveValue(m_V1, "The volume");
    bOK &= checkStrictlyPositiveValue(m_F, "The bioavailability");
    bOK &= checkStrictlyPositiveValue(m_Km, "The Michaelis Menten constant");
    bOK &= checkStrictlyPositiveValue(m_Vmax, "VMax");
    bOK &= checkPositiveValue(m_Ke, "Ke");
    bOK &= checkStrictlyPositiveValue(m_Ka, "The absorption rate");
    bOK &= checkPositiveValue(m_Tlag, "The lag time");
    bOK &= checkPositiveValue(m_K12, "K12");
    bOK &= checkPositiveValue(m_K21, "K21");
    bOK &= checkCondition(m_nbPoints > 0, "The number of points is zero or negative.");
    bOK &= checkCondition(m_Int > 0, "The interval time is negative.");

    return bOK;
}


RkMichaelisMentenLinearTwoCompVmaxAmountBolusMicro::RkMichaelisMentenLinearTwoCompVmaxAmountBolusMicro()
{
    m_delivered = true;
    m_isWithLag = false;
    m_Ka = 0.0;
    m_Tlag = 0.0;
    m_F = 0.0;
}

std::vector<std::string> RkMichaelisMentenLinearTwoCompVmaxAmountBolusMicro::getParametersId()
{
    return {"V1", "Km", "Vmax", "Ke", "K12", "K21"};
}

bool RkMichaelisMentenLinearTwoCompVmaxAmountBolusMicro::checkInputs(
        const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkCondition(_parameters.size() >= 6, "The number of parameters should be equal to 6.")) {
        return false;
    }

    m_D = _intakeEvent.getDose();
    m_V1 = _parameters.getValue(ParameterId::V1);
    m_Km = _parameters.getValue(ParameterId::Km);
    m_Vmax = _parameters.getValue(ParameterId::Vmax);
    m_Ke = _parameters.getValue(ParameterId::Ke);
    m_F = _parameters.getOptionalValue(ParameterId::F);
    m_Ka = _parameters.getOptionalValue(ParameterId::Ka);
    m_K12 = _parameters.getValue(ParameterId::K12);
    m_K21 = _parameters.getValue(ParameterId::K21);
    m_nbPoints = _intakeEvent.getNbPoints();
    m_Int = (_intakeEvent.getInterval()).toHours();

    // check the inputs
    bool bOK = true;
    bOK &= checkPositiveValue(m_D, "The dose");
    bOK &= checkStrictlyPositiveValue(m_V1, "The volume");
    bOK &= checkPositiveValue(m_F, "The bioavailability");
    bOK &= checkStrictlyPositiveValue(m_Km, "The Michaelis Menten constant");
    bOK &= checkStrictlyPositiveValue(m_Vmax, "VMax");
    bOK &= checkPositiveValue(m_Ke, "Ke");
    bOK &= checkPositiveValue(m_Ka, "The absorption rate");
    bOK &= checkPositiveValue(m_Tlag, "The lag time");
    bOK &= checkPositiveValue(m_K12, "K12");
    bOK &= checkPositiveValue(m_K21, "K21");
    bOK &= checkCondition(m_nbPoints > 0, "The number of points is zero or negative.");
    bOK &= checkCondition(m_Int > 0, "The interval time is negative.");

    return bOK;
}


RkMichaelisMentenLinearTwoCompVmaxAmountInfusionMicro::RkMichaelisMentenLinearTwoCompVmaxAmountInfusionMicro()
{
    m_delivered = true;
    m_isInfusion = true;
    m_isWithLag = false;
}

std::vector<std::string> RkMichaelisMentenLinearTwoCompVmaxAmountInfusionMicro::getParametersId()
{
    return {"V1", "Km", "Vmax", "Ke", "K12", "K21"};
}

bool RkMichaelisMentenLinearTwoCompVmaxAmountInfusionMicro::checkInputs(
        const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkCondition(_parameters.size() >= 6, "The number of parameters should be equal to 6.")) {
        return false;
    }

    m_Tinf = (_intakeEvent.getInfusionTime()).toHours();
    m_nonDifferentiableTime = m_Tinf;

    const double eps = 0.001;
    m_TinfLow = m_Tinf - eps;
    m_TinfHigh = m_Tinf + eps;

    m_D = _intakeEvent.getDose();
    m_V1 = _parameters.getValue(ParameterId::V1);
    m_Km = _parameters.getValue(ParameterId::Km);
    m_Vmax = _parameters.getValue(ParameterId::Vmax);
    m_Ke = _parameters.getValue(ParameterId::Ke);
    m_F = _parameters.getValue(ParameterId::F);
    m_Ka = _parameters.getValue(ParameterId::Ka);
    m_K12 = _parameters.getValue(ParameterId::K12);
    m_K21 = _parameters.getValue(ParameterId::K21);
    m_nbPoints = _intakeEvent.getNbPoints();
    m_Int = (_intakeEvent.getInterval()).toHours();

    // check the inputs
    bool bOK = true;
    bOK &= checkPositiveValue(m_D, "The dose");
    bOK &= checkStrictlyPositiveValue(m_V1, "The volume");
    bOK &= checkPositiveValue(m_F, "The bioavailability");
    bOK &= checkStrictlyPositiveValue(m_Km, "The Michaelis Menten constant");
    bOK &= checkStrictlyPositiveValue(m_Vmax, "VMax");
    bOK &= checkPositiveValue(m_Ke, "Ke");
    bOK &= checkPositiveValue(m_Ka, "The absorption rate");
    bOK &= checkPositiveValue(m_Tlag, "The lag time");
    bOK &= checkPositiveValue(m_K12, "K12");
    bOK &= checkPositiveValue(m_K21, "K21");
    bOK &= checkCondition(m_nbPoints > 0, "The number of points is zero or negative.");
    bOK &= checkCondition(m_Int > 0, "The interval time is negative.");

    return bOK;
}






















RkMichaelisMentenLinearTwoCompVmaxAmountExtraMacro::RkMichaelisMentenLinearTwoCompVmaxAmountExtraMacro() = default;



std::vector<std::string> RkMichaelisMentenLinearTwoCompVmaxAmountExtraMacro::getParametersId()
{
    return {"V1", "Km", "Vmax", "CL", "V2", "Q", "F", "Ka"};
}

bool RkMichaelisMentenLinearTwoCompVmaxAmountExtraMacro::checkInputs(
        const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkCondition(_parameters.size() >= 8, "The number of parameters should be equal to 8.")) {
        return false;
    }

    m_D = _intakeEvent.getDose();
    m_V1 = _parameters.getValue(ParameterId::V1);
    m_Km = _parameters.getValue(ParameterId::Km);
    m_Vmax = _parameters.getValue(ParameterId::Vmax);
    m_F = _parameters.getValue(ParameterId::F);
    m_Ka = _parameters.getValue(ParameterId::Ka);
    m_nbPoints = _intakeEvent.getNbPoints();
    m_Int = (_intakeEvent.getInterval()).toHours();
    auto v2 = _parameters.getValue(ParameterId::V2);
    auto q = _parameters.getValue(ParameterId::Q);
    auto cl = _parameters.getValue(ParameterId::CL);
    m_Ke = cl / m_V1;
    m_K12 = q / m_V1;
    m_K21 = q / v2;

    // check the inputs
    bool bOK = true;
    bOK &= checkPositiveValue(m_D, "The dose");
    bOK &= checkStrictlyPositiveValue(m_V1, "The volume");
    bOK &= checkStrictlyPositiveValue(m_F, "The bioavailability");
    bOK &= checkStrictlyPositiveValue(m_Km, "The Michaelis Menten constant");
    bOK &= checkStrictlyPositiveValue(m_Vmax, "VMax");
    bOK &= checkPositiveValue(m_Ke, "Ke");
    bOK &= checkStrictlyPositiveValue(m_Ka, "The absorption rate");
    bOK &= checkPositiveValue(m_Tlag, "The lag time");
    bOK &= checkPositiveValue(m_K12, "K12");
    bOK &= checkPositiveValue(m_K21, "K21");
    bOK &= checkCondition(m_nbPoints > 0, "The number of points is zero or negative.");
    bOK &= checkCondition(m_Int > 0, "The interval time is negative.");

    return bOK;
}


RkMichaelisMentenLinearTwoCompVmaxAmountExtraLagMacro::RkMichaelisMentenLinearTwoCompVmaxAmountExtraLagMacro() =
        default;

std::vector<std::string> RkMichaelisMentenLinearTwoCompVmaxAmountExtraLagMacro::getParametersId()
{
    return {"V1", "Km", "Vmax", "CL", "V2", "Q", "F", "Ka", "Tlag"};
}

bool RkMichaelisMentenLinearTwoCompVmaxAmountExtraLagMacro::checkInputs(
        const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkCondition(_parameters.size() >= 9, "The number of parameters should be equal to 9.")) {
        return false;
    }

    m_D = _intakeEvent.getDose();
    m_V1 = _parameters.getValue(ParameterId::V1);
    m_Km = _parameters.getValue(ParameterId::Km);
    m_Vmax = _parameters.getValue(ParameterId::Vmax);
    m_F = _parameters.getValue(ParameterId::F);
    m_Ka = _parameters.getValue(ParameterId::Ka);
    m_Tlag = _parameters.getValue(ParameterId::Tlag);
    m_nbPoints = _intakeEvent.getNbPoints();
    m_Int = (_intakeEvent.getInterval()).toHours();
    auto v2 = _parameters.getValue(ParameterId::V2);
    auto q = _parameters.getValue(ParameterId::Q);
    auto cl = _parameters.getValue(ParameterId::CL);
    m_Ke = cl / m_V1;
    m_K12 = q / m_V1;
    m_K21 = q / v2;

    // check the inputs
    bool bOK = true;
    bOK &= checkPositiveValue(m_D, "The dose");
    bOK &= checkStrictlyPositiveValue(m_V1, "The volume");
    bOK &= checkStrictlyPositiveValue(m_F, "The bioavailability");
    bOK &= checkStrictlyPositiveValue(m_Km, "The Michaelis Menten constant");
    bOK &= checkStrictlyPositiveValue(m_Vmax, "VMax");
    bOK &= checkPositiveValue(m_Ke, "Ke");
    bOK &= checkStrictlyPositiveValue(m_Ka, "The absorption rate");
    bOK &= checkPositiveValue(m_Tlag, "The lag time");
    bOK &= checkPositiveValue(m_K12, "K12");
    bOK &= checkPositiveValue(m_K21, "K21");
    bOK &= checkCondition(m_nbPoints > 0, "The number of points is zero or negative.");
    bOK &= checkCondition(m_Int > 0, "The interval time is negative.");

    return bOK;
}


RkMichaelisMentenLinearTwoCompVmaxAmountBolusMacro::RkMichaelisMentenLinearTwoCompVmaxAmountBolusMacro() = default;

std::vector<std::string> RkMichaelisMentenLinearTwoCompVmaxAmountBolusMacro::getParametersId()
{
    return {"V1", "Km", "Vmax", "CL", "V2", "Q"};
}

bool RkMichaelisMentenLinearTwoCompVmaxAmountBolusMacro::checkInputs(
        const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkCondition(_parameters.size() >= 6, "The number of parameters should be equal to 6.")) {
        return false;
    }

    m_D = _intakeEvent.getDose();
    m_V1 = _parameters.getValue(ParameterId::V1);
    m_Km = _parameters.getValue(ParameterId::Km);
    m_Vmax = _parameters.getValue(ParameterId::Vmax);
    m_nbPoints = _intakeEvent.getNbPoints();
    m_Int = (_intakeEvent.getInterval()).toHours();
    auto v2 = _parameters.getValue(ParameterId::V2);
    auto q = _parameters.getValue(ParameterId::Q);
    auto cl = _parameters.getValue(ParameterId::CL);
    m_Ke = cl / m_V1;
    m_K12 = q / m_V1;
    m_K21 = q / v2;

    // check the inputs
    bool bOK = true;
    bOK &= checkPositiveValue(m_D, "The dose");
    bOK &= checkStrictlyPositiveValue(m_V1, "The volume");
    bOK &= checkPositiveValue(m_F, "The bioavailability");
    bOK &= checkStrictlyPositiveValue(m_Km, "The Michaelis Menten constant");
    bOK &= checkStrictlyPositiveValue(m_Vmax, "VMax");
    bOK &= checkPositiveValue(m_Ke, "Ke");
    bOK &= checkPositiveValue(m_Ka, "The absorption rate");
    bOK &= checkPositiveValue(m_Tlag, "The lag time");
    bOK &= checkPositiveValue(m_K12, "K12");
    bOK &= checkPositiveValue(m_K21, "K21");
    bOK &= checkCondition(m_nbPoints > 0, "The number of points is zero or negative.");
    bOK &= checkCondition(m_Int > 0, "The interval time is negative.");

    return bOK;
}
















RkMichaelisMentenLinearTwoCompVmaxAmountInfusionMacro::RkMichaelisMentenLinearTwoCompVmaxAmountInfusionMacro() =
        default;

std::vector<std::string> RkMichaelisMentenLinearTwoCompVmaxAmountInfusionMacro::getParametersId()
{
    return {"V1", "Km", "Vmax", "CL", "V2", "Q"};
}

bool RkMichaelisMentenLinearTwoCompVmaxAmountInfusionMacro::checkInputs(
        const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkCondition(_parameters.size() >= 6, "The number of parameters should be equal to 6.")) {
        return false;
    }

    m_Tinf = (_intakeEvent.getInfusionTime()).toHours();
    m_nonDifferentiableTime = m_Tinf;

    const double eps = 0.001;
    m_TinfLow = m_Tinf - eps;
    m_TinfHigh = m_Tinf + eps;

    m_D = _intakeEvent.getDose();
    m_V1 = _parameters.getValue(ParameterId::V1);
    m_Km = _parameters.getValue(ParameterId::Km);
    m_Vmax = _parameters.getValue(ParameterId::Vmax);
    m_nbPoints = _intakeEvent.getNbPoints();
    m_Int = (_intakeEvent.getInterval()).toHours();
    auto v2 = _parameters.getValue(ParameterId::V2);
    auto q = _parameters.getValue(ParameterId::Q);
    auto cl = _parameters.getValue(ParameterId::CL);
    m_Ke = cl / m_V1;
    m_K12 = q / m_V1;
    m_K21 = q / v2;

    // check the inputs
    bool bOK = true;
    bOK &= checkPositiveValue(m_D, "The dose");
    bOK &= checkStrictlyPositiveValue(m_V1, "The volume");
    bOK &= checkPositiveValue(m_F, "The bioavailability");
    bOK &= checkStrictlyPositiveValue(m_Km, "The Michaelis Menten constant");
    bOK &= checkStrictlyPositiveValue(m_Vmax, "VMax");
    bOK &= checkPositiveValue(m_Ke, "Ke");
    bOK &= checkPositiveValue(m_Ka, "The absorption rate");
    bOK &= checkPositiveValue(m_Tlag, "The lag time");
    bOK &= checkPositiveValue(m_K12, "K12");
    bOK &= checkPositiveValue(m_K21, "K21");
    bOK &= checkCondition(m_nbPoints > 0, "The number of points is zero or negative.");
    bOK &= checkCondition(m_Int > 0, "The interval time is negative.");

    return bOK;
}







} // namespace Core
} // namespace Tucuxi
