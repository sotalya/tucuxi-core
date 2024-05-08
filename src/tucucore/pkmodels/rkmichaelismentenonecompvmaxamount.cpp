//@@license@@

#include "rkmichaelismentenonecompvmaxamount.h"

#include "tucucore/intakeevent.h"

namespace Tucuxi {
namespace Core {


RkMichaelisMentenOneCompVmaxAmount::RkMichaelisMentenOneCompVmaxAmount()
    : IntakeIntervalCalculatorRK4Base<2, RkMichaelisMentenOneCompVmaxAmount>(
            std::make_unique<PertinentTimesCalculatorStandard>())
{
}


RkMichaelisMentenOneCompVmaxAmountExtra::RkMichaelisMentenOneCompVmaxAmountExtra() {}

std::vector<std::string> RkMichaelisMentenOneCompVmaxAmountExtra::getParametersId()
{
    return {"V", "Km", "Vmax", "F", "Ka"};
}

bool RkMichaelisMentenOneCompVmaxAmountExtra::checkInputs(
        const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
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


RkMichaelisMentenOneCompVmaxAmountBolus::RkMichaelisMentenOneCompVmaxAmountBolus() {}

std::vector<std::string> RkMichaelisMentenOneCompVmaxAmountBolus::getParametersId()
{
    return {"V", "Km", "Vmax"};
}

bool RkMichaelisMentenOneCompVmaxAmountBolus::checkInputs(
        const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
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


RkMichaelisMentenOneCompVmaxAmountInfusion::RkMichaelisMentenOneCompVmaxAmountInfusion()
{
    m_isInfusion = true;
}

std::vector<std::string> RkMichaelisMentenOneCompVmaxAmountInfusion::getParametersId()
{
    return {"V", "Km", "Vmax"};
}

bool RkMichaelisMentenOneCompVmaxAmountInfusion::checkInputs(
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
