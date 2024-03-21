//@@license@@

#include "rkmichaelismentenlineartwocomp.h"

#include "tucucore/intakeevent.h"

namespace Tucuxi {
namespace Core {


RkMichaelisMentenLinearTwoComp::RkMichaelisMentenLinearTwoComp()
    : IntakeIntervalCalculatorRK4Base<3, RkMichaelisMentenLinearTwoComp>(
            std::make_unique<PertinentTimesCalculatorStandard>())
{
    m_Tinf = 0.0;
    m_Tlag = 0.0;
}


RkMichaelisMentenLinearTwoCompExtraMicro::RkMichaelisMentenLinearTwoCompExtraMicro() : RkMichaelisMentenLinearTwoComp()
{
    m_delivered = true;
    m_isInfusion = false;
    m_isWithLag = false;
}

std::vector<std::string> RkMichaelisMentenLinearTwoCompExtraMicro::getParametersId()
{
    return {"V1", "Km", "Vmax", "Ke", "K12", "K21", "F", "Ka"};
}

bool RkMichaelisMentenLinearTwoCompExtraMicro::checkInputs(
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


RkMichaelisMentenLinearTwoCompExtraLagMicro::RkMichaelisMentenLinearTwoCompExtraLagMicro()
    : RkMichaelisMentenLinearTwoComp()
{
    m_delivered = false;
    m_isInfusion = false;
    m_isWithLag = true;
}

std::vector<std::string> RkMichaelisMentenLinearTwoCompExtraLagMicro::getParametersId()
{
    return {"V1", "Km", "Vmax", "Ke", "K12", "K21", "F", "Ka", "Tlag"};
}

bool RkMichaelisMentenLinearTwoCompExtraLagMicro::checkInputs(
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


RkMichaelisMentenLinearTwoCompBolusMicro::RkMichaelisMentenLinearTwoCompBolusMicro() : RkMichaelisMentenLinearTwoComp()
{
    m_delivered = true;
    m_isInfusion = false;
    m_isWithLag = false;
    m_Ka = 0.0;
    m_Tlag = 0.0;
    m_F = 0.0;
}

std::vector<std::string> RkMichaelisMentenLinearTwoCompBolusMicro::getParametersId()
{
    return {"V1", "Km", "Vmax", "Ke", "K12", "K21"};
}

bool RkMichaelisMentenLinearTwoCompBolusMicro::checkInputs(
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


RkMichaelisMentenLinearTwoCompInfusionMicro::RkMichaelisMentenLinearTwoCompInfusionMicro()
    : RkMichaelisMentenLinearTwoComp()
{
    m_delivered = true;
    m_isInfusion = true;
    m_isWithLag = false;
}

std::vector<std::string> RkMichaelisMentenLinearTwoCompInfusionMicro::getParametersId()
{
    return {"V1", "Km", "Vmax", "Ke", "K12", "K21"};
}

bool RkMichaelisMentenLinearTwoCompInfusionMicro::checkInputs(
        const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkCondition(_parameters.size() >= 6, "The number of parameters should be equal to 6.")) {
        return false;
    }

    m_Tinf = (_intakeEvent.getInfusionTime()).toHours();

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






















RkMichaelisMentenLinearTwoCompExtraMacro::RkMichaelisMentenLinearTwoCompExtraMacro() = default;



std::vector<std::string> RkMichaelisMentenLinearTwoCompExtraMacro::getParametersId()
{
    return {"V1", "Km", "Vmax", "CL", "V2", "Q", "F", "Ka"};
}

bool RkMichaelisMentenLinearTwoCompExtraMacro::checkInputs(
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


RkMichaelisMentenLinearTwoCompExtraLagMacro::RkMichaelisMentenLinearTwoCompExtraLagMacro() = default;

std::vector<std::string> RkMichaelisMentenLinearTwoCompExtraLagMacro::getParametersId()
{
    return {"V1", "Km", "Vmax", "CL", "V2", "Q", "F", "Ka", "Tlag"};
}

bool RkMichaelisMentenLinearTwoCompExtraLagMacro::checkInputs(
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


RkMichaelisMentenLinearTwoCompBolusMacro::RkMichaelisMentenLinearTwoCompBolusMacro() = default;

std::vector<std::string> RkMichaelisMentenLinearTwoCompBolusMacro::getParametersId()
{
    return {"V1", "Km", "Vmax", "CL", "V2", "Q"};
}

bool RkMichaelisMentenLinearTwoCompBolusMacro::checkInputs(
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
















RkMichaelisMentenLinearTwoCompInfusionMacro::RkMichaelisMentenLinearTwoCompInfusionMacro() = default;

std::vector<std::string> RkMichaelisMentenLinearTwoCompInfusionMacro::getParametersId()
{
    return {"V1", "Km", "Vmax", "CL", "V2", "Q"};
}

bool RkMichaelisMentenLinearTwoCompInfusionMacro::checkInputs(
        const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkCondition(_parameters.size() >= 6, "The number of parameters should be equal to 6.")) {
        return false;
    }

    m_Tinf = (_intakeEvent.getInfusionTime()).toHours();

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
