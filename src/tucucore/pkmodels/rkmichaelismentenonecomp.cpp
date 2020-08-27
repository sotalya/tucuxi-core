#include "rkmichaelismentenonecomp.h"
#include "tucucore/intakeevent.h"

namespace Tucuxi {
namespace Core {


RkMichaelisMentenOneComp::RkMichaelisMentenOneComp() : IntakeIntervalCalculatorRK4Base<2, RkMichaelisMentenOneComp> (new PertinentTimesCalculatorStandard())
{
}


RkMichaelisMentenOneCompExtra::RkMichaelisMentenOneCompExtra() : RkMichaelisMentenOneComp()
{
}

std::vector<std::string> RkMichaelisMentenOneCompExtra::getParametersId()
{
    return {"V", "Km", "Vmax", "F", "Ka"};
}

bool RkMichaelisMentenOneCompExtra::checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkValue(_parameters.size() >= 5, "The number of parameters should be equal to 5.")) {
        return false;
    }

    m_D = _intakeEvent.getDose() * 1000;
    m_V = _parameters.getValue(ParameterId::V);
    m_Km = _parameters.getValue(ParameterId::Km);
    m_Vmax = _parameters.getValue(ParameterId::Vmax);
    m_F = _parameters.getValue(ParameterId::F);
    m_Ka = _parameters.getValue(ParameterId::Ka);
    m_nbPoints = _intakeEvent.getNbPoints();
    m_Int = (_intakeEvent.getInterval()).toHours();

    // check the inputs
    bool bOK = checkValue(m_D >= 0, "The dose is negative.");
    bOK &= checkValue(!std::isnan(m_D), "The dose is NaN.");
    bOK &= checkValue(!std::isinf(m_D), "The dose is Inf.");
    bOK &= checkValue(m_V > 0, "The volume is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_V), "The m_V is NaN.");
    bOK &= checkValue(!std::isinf(m_V), "The _V is Inf.");
    bOK &= checkValue(m_F > 0, "The volume is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_F), "The F is NaN.");
    bOK &= checkValue(!std::isinf(m_F), "The F is Inf.");
    bOK &= checkValue(m_Ke > 0, "The clearance is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_Ke), "The CL is NaN.");
    bOK &= checkValue(!std::isinf(m_Ke), "The CL is Inf.");
    bOK &= checkValue(m_Ka > 0, "The clearance is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_Ka), "The m_Ka is NaN.");
    bOK &= checkValue(!std::isinf(m_Ka), "The m_Ka is Inf.");
    bOK &= checkValue(m_nbPoints >= 0, "The number of points is zero or negative.");
    bOK &= checkValue(m_Int > 0, "The interval time is negative.");

    return bOK;
}


RkMichaelisMentenOneCompBolus::RkMichaelisMentenOneCompBolus() : RkMichaelisMentenOneComp()
{
}

std::vector<std::string> RkMichaelisMentenOneCompBolus::getParametersId()
{
    return {"V", "Km", "Vmax", "F"};
}

bool RkMichaelisMentenOneCompBolus::checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkValue(_parameters.size() >= 4, "The number of parameters should be equal to 4.")) {
        return false;
    }

    m_D = _intakeEvent.getDose() * 1000;
    m_V = _parameters.getValue(ParameterId::V);
    m_Km = _parameters.getValue(ParameterId::Km);
    m_Vmax = _parameters.getValue(ParameterId::Vmax);
    m_F = _parameters.getValue(ParameterId::F);
    m_Ka = _parameters.getValue(ParameterId::Ka);
    m_nbPoints = _intakeEvent.getNbPoints();
    m_Int = (_intakeEvent.getInterval()).toHours();

    // check the inputs
    bool bOK = checkValue(m_D >= 0, "The dose is negative.");
    bOK &= checkValue(!std::isnan(m_D), "The dose is NaN.");
    bOK &= checkValue(!std::isinf(m_D), "The dose is Inf.");
    bOK &= checkValue(m_V > 0, "The volume is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_V), "The m_V is NaN.");
    bOK &= checkValue(!std::isinf(m_V), "The _V is Inf.");
    bOK &= checkValue(m_F > 0, "The volume is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_F), "The F is NaN.");
    bOK &= checkValue(!std::isinf(m_F), "The F is Inf.");
    bOK &= checkValue(m_Ke > 0, "The clearance is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_Ke), "The CL is NaN.");
    bOK &= checkValue(!std::isinf(m_Ke), "The CL is Inf.");
    bOK &= checkValue(m_Ka > 0, "The clearance is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_Ka), "The m_Ka is NaN.");
    bOK &= checkValue(!std::isinf(m_Ka), "The m_Ka is Inf.");
    bOK &= checkValue(m_nbPoints >= 0, "The number of points is zero or negative.");
    bOK &= checkValue(m_Int > 0, "The interval time is negative.");

    return bOK;
}


RkMichaelisMentenOneCompInfusion::RkMichaelisMentenOneCompInfusion() : RkMichaelisMentenOneComp()
{
}

std::vector<std::string> RkMichaelisMentenOneCompInfusion::getParametersId()
{
    return {"V", "Km", "Vmax", "F"};
}

bool RkMichaelisMentenOneCompInfusion::checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkValue(_parameters.size() >= 4, "The number of parameters should be equal to 4.")) {
        return false;
    }

    m_D = _intakeEvent.getDose() * 1000;
    m_V = _parameters.getValue(ParameterId::V);
    m_Km = _parameters.getValue(ParameterId::Km);
    m_Vmax = _parameters.getValue(ParameterId::Vmax);
    m_F = _parameters.getValue(ParameterId::F);
    m_Ka = _parameters.getValue(ParameterId::Ka);
    m_nbPoints = _intakeEvent.getNbPoints();
    m_Int = (_intakeEvent.getInterval()).toHours();

    // check the inputs
    bool bOK = checkValue(m_D >= 0, "The dose is negative.");
    bOK &= checkValue(!std::isnan(m_D), "The dose is NaN.");
    bOK &= checkValue(!std::isinf(m_D), "The dose is Inf.");
    bOK &= checkValue(m_V > 0, "The volume is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_V), "The m_V is NaN.");
    bOK &= checkValue(!std::isinf(m_V), "The _V is Inf.");
    bOK &= checkValue(m_F > 0, "The volume is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_F), "The F is NaN.");
    bOK &= checkValue(!std::isinf(m_F), "The F is Inf.");
    bOK &= checkValue(m_Ke > 0, "The clearance is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_Ke), "The CL is NaN.");
    bOK &= checkValue(!std::isinf(m_Ke), "The CL is Inf.");
    bOK &= checkValue(m_Ka > 0, "The clearance is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_Ka), "The m_Ka is NaN.");
    bOK &= checkValue(!std::isinf(m_Ka), "The m_Ka is Inf.");
    bOK &= checkValue(m_nbPoints >= 0, "The number of points is zero or negative.");
    bOK &= checkValue(m_Int > 0, "The interval time is negative.");

    return bOK;
}




} // namespace Core
} // namespace Tucuxi
