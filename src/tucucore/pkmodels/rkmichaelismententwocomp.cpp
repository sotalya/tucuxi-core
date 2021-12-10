#include "rkmichaelismententwocomp.h"

#include "tucucore/intakeevent.h"

namespace Tucuxi {
namespace Core {


RkMichaelisMentenTwoComp::RkMichaelisMentenTwoComp()
    : IntakeIntervalCalculatorRK4Base<3, RkMichaelisMentenTwoComp>(new PertinentTimesCalculatorStandard())
{
    m_Tinf = 0.0;
    m_Tlag = 0.0;
}


RkMichaelisMentenTwoCompExtraMicro::RkMichaelisMentenTwoCompExtraMicro() : RkMichaelisMentenTwoComp()
{
    m_delivered = true;
    m_isInfusion = false;
    m_isWithLag = false;
}

std::vector<std::string> RkMichaelisMentenTwoCompExtraMicro::getParametersId()
{
    return {"V1", "Km", "Vmax", "K12", "K21", "F", "Ka"};
}

bool RkMichaelisMentenTwoCompExtraMicro::checkInputs(
        const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkCondition(_parameters.size() >= 7, "The number of parameters should be equal to 7.")) {
        return false;
    }

    m_D = _intakeEvent.getDose();
    m_V1 = _parameters.getValue(ParameterId::V1);
    m_Km = _parameters.getValue(ParameterId::Km);
    m_Vmax = _parameters.getValue(ParameterId::Vmax);
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
    bOK &= checkStrictlyPositiveValue(m_Ka, "The absorption rate");
    bOK &= checkPositiveValue(m_Tlag, "The lag time");
    bOK &= checkPositiveValue(m_K12, "K12");
    bOK &= checkPositiveValue(m_K21, "K21");
    bOK &= checkCondition(m_nbPoints > 0, "The number of points is zero or negative.");
    bOK &= checkCondition(m_Int > 0, "The interval time is negative.");

    return bOK;
}


RkMichaelisMentenTwoCompExtraLagMicro::RkMichaelisMentenTwoCompExtraLagMicro() : RkMichaelisMentenTwoComp()
{
    m_delivered = false;
    m_isInfusion = false;
    m_isWithLag = true;
}

std::vector<std::string> RkMichaelisMentenTwoCompExtraLagMicro::getParametersId()
{
    return {"V1", "Km", "Vmax", "K12", "K21", "F", "Ka", "Tlag"};
}

bool RkMichaelisMentenTwoCompExtraLagMicro::checkInputs(
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
    bOK &= checkStrictlyPositiveValue(m_Ka, "The absorption rate");
    bOK &= checkPositiveValue(m_Tlag, "The lag time");
    bOK &= checkPositiveValue(m_K12, "K12");
    bOK &= checkPositiveValue(m_K21, "K21");
    bOK &= checkCondition(m_nbPoints > 0, "The number of points is zero or negative.");
    bOK &= checkCondition(m_Int > 0, "The interval time is negative.");

    return bOK;
}


RkMichaelisMentenTwoCompBolusMicro::RkMichaelisMentenTwoCompBolusMicro() : RkMichaelisMentenTwoComp()
{
    m_delivered = true;
    m_isInfusion = false;
    m_isWithLag = false;
    m_Ka = 0.0;
    m_Tlag = 0.0;
    m_F = 0.0;
}

std::vector<std::string> RkMichaelisMentenTwoCompBolusMicro::getParametersId()
{
    return {"V1", "Km", "Vmax", "K12", "K21"};
}

bool RkMichaelisMentenTwoCompBolusMicro::checkInputs(
        const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkCondition(_parameters.size() >= 5, "The number of parameters should be equal to 5.")) {
        return false;
    }

    m_D = _intakeEvent.getDose();
    m_V1 = _parameters.getValue(ParameterId::V1);
    m_Km = _parameters.getValue(ParameterId::Km);
    m_Vmax = _parameters.getValue(ParameterId::Vmax);
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
    bOK &= checkPositiveValue(m_Ka, "The absorption rate");
    bOK &= checkPositiveValue(m_Tlag, "The lag time");
    bOK &= checkPositiveValue(m_K12, "K12");
    bOK &= checkPositiveValue(m_K21, "K21");
    bOK &= checkCondition(m_nbPoints > 0, "The number of points is zero or negative.");
    bOK &= checkCondition(m_Int > 0, "The interval time is negative.");

    return bOK;
}


RkMichaelisMentenTwoCompInfusionMicro::RkMichaelisMentenTwoCompInfusionMicro() : RkMichaelisMentenTwoComp()
{
    m_delivered = true;
    m_isInfusion = true;
    m_isWithLag = false;
}

std::vector<std::string> RkMichaelisMentenTwoCompInfusionMicro::getParametersId()
{
    return {"V1", "Km", "Vmax", "K12", "K21"};
}

bool RkMichaelisMentenTwoCompInfusionMicro::checkInputs(
        const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkCondition(_parameters.size() >= 5, "The number of parameters should be equal to 5.")) {
        return false;
    }

    m_Tinf = (_intakeEvent.getInfusionTime()).toHours();

    m_D = _intakeEvent.getDose();
    m_V1 = _parameters.getValue(ParameterId::V1);
    m_Km = _parameters.getValue(ParameterId::Km);
    m_Vmax = _parameters.getValue(ParameterId::Vmax);
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
    bOK &= checkPositiveValue(m_Ka, "The absorption rate");
    bOK &= checkPositiveValue(m_Tlag, "The lag time");
    bOK &= checkPositiveValue(m_K12, "K12");
    bOK &= checkPositiveValue(m_K21, "K21");
    bOK &= checkCondition(m_nbPoints > 0, "The number of points is zero or negative.");
    bOK &= checkCondition(m_Int > 0, "The interval time is negative.");

    return bOK;
}






















RkMichaelisMentenTwoCompExtraMacro::RkMichaelisMentenTwoCompExtraMacro() {}



std::vector<std::string> RkMichaelisMentenTwoCompExtraMacro::getParametersId()
{
    return {"V1", "Km", "Vmax", "V2", "Q", "F", "Ka"};
}

bool RkMichaelisMentenTwoCompExtraMacro::checkInputs(
        const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkCondition(_parameters.size() >= 7, "The number of parameters should be equal to 7.")) {
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
    m_K12 = q / m_V1;
    m_K21 = q / v2;

    // check the inputs
    bool bOK = true;
    bOK &= checkPositiveValue(m_D, "The dose");
    bOK &= checkStrictlyPositiveValue(m_V1, "The volume");
    bOK &= checkStrictlyPositiveValue(m_F, "The bioavailability");
    bOK &= checkStrictlyPositiveValue(m_Km, "The Michaelis Menten constant");
    bOK &= checkStrictlyPositiveValue(m_Vmax, "VMax");
    bOK &= checkStrictlyPositiveValue(m_Ka, "The absorption rate");
    bOK &= checkPositiveValue(m_Tlag, "The lag time");
    bOK &= checkPositiveValue(m_K12, "K12");
    bOK &= checkPositiveValue(m_K21, "K21");
    bOK &= checkCondition(m_nbPoints > 0, "The number of points is zero or negative.");
    bOK &= checkCondition(m_Int > 0, "The interval time is negative.");

    return bOK;
}


RkMichaelisMentenTwoCompExtraLagMacro::RkMichaelisMentenTwoCompExtraLagMacro() {}

std::vector<std::string> RkMichaelisMentenTwoCompExtraLagMacro::getParametersId()
{
    return {"V1", "Km", "Vmax", "V2", "Q", "F", "Ka", "Tlag"};
}

bool RkMichaelisMentenTwoCompExtraLagMacro::checkInputs(
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
    m_Tlag = _parameters.getValue(ParameterId::Tlag);
    m_nbPoints = _intakeEvent.getNbPoints();
    m_Int = (_intakeEvent.getInterval()).toHours();
    auto v2 = _parameters.getValue(ParameterId::V2);
    auto q = _parameters.getValue(ParameterId::Q);
    m_K12 = q / m_V1;
    m_K21 = q / v2;

    // check the inputs
    bool bOK = true;
    bOK &= checkPositiveValue(m_D, "The dose");
    bOK &= checkStrictlyPositiveValue(m_V1, "The volume");
    bOK &= checkStrictlyPositiveValue(m_F, "The bioavailability");
    bOK &= checkStrictlyPositiveValue(m_Km, "The Michaelis Menten constant");
    bOK &= checkStrictlyPositiveValue(m_Vmax, "VMax");
    bOK &= checkStrictlyPositiveValue(m_Ka, "The absorption rate");
    bOK &= checkPositiveValue(m_Tlag, "The lag time");
    bOK &= checkPositiveValue(m_K12, "K12");
    bOK &= checkPositiveValue(m_K21, "K21");
    bOK &= checkCondition(m_nbPoints > 0, "The number of points is zero or negative.");
    bOK &= checkCondition(m_Int > 0, "The interval time is negative.");

    return bOK;
}


RkMichaelisMentenTwoCompBolusMacro::RkMichaelisMentenTwoCompBolusMacro() {}

std::vector<std::string> RkMichaelisMentenTwoCompBolusMacro::getParametersId()
{
    return {"V1", "Km", "Vmax", "V2", "Q"};
}

bool RkMichaelisMentenTwoCompBolusMacro::checkInputs(
        const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkCondition(_parameters.size() >= 5, "The number of parameters should be equal to 5.")) {
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
    m_K12 = q / m_V1;
    m_K21 = q / v2;

    // check the inputs
    bool bOK = true;
    bOK &= checkPositiveValue(m_D, "The dose");
    bOK &= checkStrictlyPositiveValue(m_V1, "The volume");
    bOK &= checkPositiveValue(m_F, "The bioavailability");
    bOK &= checkStrictlyPositiveValue(m_Km, "The Michaelis Menten constant");
    bOK &= checkStrictlyPositiveValue(m_Vmax, "VMax");
    bOK &= checkPositiveValue(m_Ka, "The absorption rate");
    bOK &= checkPositiveValue(m_Tlag, "The lag time");
    bOK &= checkPositiveValue(m_K12, "K12");
    bOK &= checkPositiveValue(m_K21, "K21");
    bOK &= checkCondition(m_nbPoints > 0, "The number of points is zero or negative.");
    bOK &= checkCondition(m_Int > 0, "The interval time is negative.");

    return bOK;
}
















RkMichaelisMentenTwoCompInfusionMacro::RkMichaelisMentenTwoCompInfusionMacro() {}

std::vector<std::string> RkMichaelisMentenTwoCompInfusionMacro::getParametersId()
{
    return {"V1", "Km", "Vmax", "V2", "Q"};
}

bool RkMichaelisMentenTwoCompInfusionMacro::checkInputs(
        const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkCondition(_parameters.size() >= 5, "The number of parameters should be equal to 5.")) {
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
    m_K12 = q / m_V1;
    m_K21 = q / v2;

    // check the inputs
    bool bOK = true;
    bOK &= checkPositiveValue(m_D, "The dose");
    bOK &= checkStrictlyPositiveValue(m_V1, "The volume");
    bOK &= checkPositiveValue(m_F, "The bioavailability");
    bOK &= checkStrictlyPositiveValue(m_Km, "The Michaelis Menten constant");
    bOK &= checkStrictlyPositiveValue(m_Vmax, "VMax");
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
