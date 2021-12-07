/*
* Copyright (C) 2017 Tucuxi SA
*/

#include <Eigen/Dense>
#include <Eigen/LU>

#include "tucucore/pkmodels/rktwocompartmentextralag.h"

#include "tucucore/intakeevent.h"

namespace Tucuxi {
namespace Core {

RK4TwoCompartmentExtraLagMicro::RK4TwoCompartmentExtraLagMicro()
    : IntakeIntervalCalculatorRK4Base<3, RK4TwoCompartmentExtraLagMicro>(new PertinentTimesCalculatorStandard())
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


RK4TwoCompartmentExtraLagMacro::RK4TwoCompartmentExtraLagMacro() : RK4TwoCompartmentExtraLagMicro() {}


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
