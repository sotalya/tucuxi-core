/*
* Copyright (C) 2018 Tucuxi SA
*/

#include <Eigen/Dense>
#include <Eigen/LU>

#include "tucucore/pkmodels/rkonecompartmentgammaextra.h"
#include "tucucore/intakeevent.h"

namespace Tucuxi {
namespace Core {

RK4OneCompartmentGammaExtraMicro::RK4OneCompartmentGammaExtraMicro() : IntakeIntervalCalculatorRK4Base<1, RK4OneCompartmentGammaExtraMicro> (new PertinentTimesCalculatorStandard())
{
}

bool RK4OneCompartmentGammaExtraMicro::checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkValue(_parameters.size() >= 5, "The number of parameters should be equal to 5.")) {
        return false;
    }

    m_D = _intakeEvent.getDose() * 1000;
    m_V = _parameters.getValue(ParameterId::V);
    m_Ke = _parameters.getValue(ParameterId::Ke);
    m_F = _parameters.getValue(ParameterId::F);
    m_nbPoints = _intakeEvent.getNbPoints();
    m_Int = (_intakeEvent.getInterval()).toHours();
    m_a = _parameters.getValue(ParameterId::a);
    m_b = _parameters.getValue(ParameterId::b);

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
    bOK &= checkValue(m_nbPoints >= 0, "The number of points is zero or negative.");
    bOK &= checkValue(m_Int > 0, "The interval time is negative.");

    return bOK;
}


RK4OneCompartmentGammaExtraMacro::RK4OneCompartmentGammaExtraMacro() : RK4OneCompartmentGammaExtraMicro()
{
}

bool RK4OneCompartmentGammaExtraMacro::checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkValue(_parameters.size() >= 5, "The number of parameters should be equal to 5.")) {
        return false;
    }

    m_D = _intakeEvent.getDose() * 1000;
    m_V = _parameters.getValue(ParameterId::V);
    Value cl = _parameters.getValue(ParameterId::CL); // clearance
    m_F = _parameters.getValue(ParameterId::F);
    m_Ke = cl / m_V;
    m_nbPoints = _intakeEvent.getNbPoints();
    m_Int = (_intakeEvent.getInterval()).toHours();
    m_a = _parameters.getValue(ParameterId::a);
    m_b = _parameters.getValue(ParameterId::b);

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
    bOK &= checkValue(cl > 0, "The clearance is not greater than zero.");
    bOK &= checkValue(!std::isnan(cl), "The clearance is NaN.");
    bOK &= checkValue(!std::isinf(cl), "The clearance is Inf.");
    bOK &= checkValue(m_nbPoints >= 0, "The number of points is zero or negative.");
    bOK &= checkValue(m_Int > 0, "The interval time is negative.");

    return bOK;
}

}
}


