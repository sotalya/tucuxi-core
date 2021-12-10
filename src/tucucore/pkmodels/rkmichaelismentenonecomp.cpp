#include "rkmichaelismentenonecomp.h"

#include "tucucore/intakeevent.h"

namespace Tucuxi {
namespace Core {


RkMichaelisMentenOneComp::RkMichaelisMentenOneComp()
    : IntakeIntervalCalculatorRK4Base<2, RkMichaelisMentenOneComp>(new PertinentTimesCalculatorStandard())
{
}


RkMichaelisMentenOneCompExtra::RkMichaelisMentenOneCompExtra() : RkMichaelisMentenOneComp() {}

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


RkMichaelisMentenOneCompBolus::RkMichaelisMentenOneCompBolus() : RkMichaelisMentenOneComp() {}

std::vector<std::string> RkMichaelisMentenOneCompBolus::getParametersId()
{
    return {"V", "Km", "Vmax", "F"};
}

bool RkMichaelisMentenOneCompBolus::checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkCondition(_parameters.size() >= 4, "The number of parameters should be equal to 4.")) {
        return false;
    }

    m_D = _intakeEvent.getDose();
    m_V = _parameters.getValue(ParameterId::V);
    m_Km = _parameters.getValue(ParameterId::Km);
    m_Vmax = _parameters.getValue(ParameterId::Vmax);
    m_F = _parameters.getValue(ParameterId::F);
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


RkMichaelisMentenOneCompInfusion::RkMichaelisMentenOneCompInfusion() : RkMichaelisMentenOneComp() {}

std::vector<std::string> RkMichaelisMentenOneCompInfusion::getParametersId()
{
    return {"V", "Km", "Vmax", "F"};
}

bool RkMichaelisMentenOneCompInfusion::checkInputs(
        const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkCondition(_parameters.size() >= 4, "The number of parameters should be equal to 4.")) {
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




} // namespace Core
} // namespace Tucuxi
