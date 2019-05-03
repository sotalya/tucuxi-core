/*
* Copyright (C) 2017 Tucuxi SA
*/



#include <Eigen/Dense>
#include <Eigen/LU>

#include "rk4twocompartmenterlang4.h"
#include "tucucore/intakeevent.h"

namespace Tucuxi {
namespace Core {


RK4TwoCompartmentErlang4Micro::RK4TwoCompartmentErlang4Micro() : IntakeIntervalCalculatorRK4Base<7, RK4TwoCompartmentErlang4Micro> (new PertinentTimesCalculatorStandard())
{
}

bool RK4TwoCompartmentErlang4Micro::checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkValue(_parameters.size() >= 6, "The number of parameters should be equal to 6.")) {
        return false;
    }

    m_D = _intakeEvent.getDose() * 1000;
    m_V1 = _parameters.getValue(ParameterId::V1);
    m_Ktr = _parameters.getValue(ParameterId::Ktr);
    m_Ke = _parameters.getValue(ParameterId::Ke);
    m_K12 = _parameters.getValue(ParameterId::K12);
    m_K21 = _parameters.getValue(ParameterId::K21);
    m_F = _parameters.getValue(ParameterId::F);

    m_nbPoints = _intakeEvent.getNbPoints();
    m_Int = (_intakeEvent.getInterval()).toHours();

    // check the inputs
    bool bOK = checkValue(m_D >= 0, "The dose is negative.");
    bOK &= checkValue(!std::isnan(m_D), "The dose is NaN.");
    bOK &= checkValue(!std::isinf(m_D), "The dose is Inf.");
    bOK &= checkValue(m_V1 > 0, "The volume is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_V1), "The m_V is NaN.");
    bOK &= checkValue(!std::isinf(m_V1), "The _V is Inf.");
    bOK &= checkValue(m_F > 0, "The volume is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_F), "The F is NaN.");
    bOK &= checkValue(!std::isinf(m_F), "The F is Inf.");
    bOK &= checkValue(m_Ke > 0, "The clearance is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_Ke), "The CL is NaN.");
    bOK &= checkValue(!std::isinf(m_Ke), "The CL is Inf.");
    bOK &= checkValue(m_Ktr > 0, "The m_Ktr is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_Ktr), "The m_Ktr is NaN.");
    bOK &= checkValue(!std::isinf(m_Ktr), "The m_Ktr is Inf.");
    bOK &= checkValue(m_K12 > 0, "The m_K12 is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_K12), "The m_K12 is NaN.");
    bOK &= checkValue(!std::isinf(m_K12), "The m_K12 is Inf.");
    bOK &= checkValue(m_K21 > 0, "The m_K21 is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_K21), "The m_K21 is NaN.");
    bOK &= checkValue(!std::isinf(m_K21), "The m_K21 is Inf.");
    bOK &= checkValue(m_nbPoints >= 0, "The number of points is zero or negative.");
    bOK &= checkValue(m_Int > 0, "The interval time is negative.");

    return bOK;
}


RK4TwoCompartmentErlang4Macro::RK4TwoCompartmentErlang4Macro() : RK4TwoCompartmentErlang4Micro()
{
}

bool RK4TwoCompartmentErlang4Macro::checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkValue(_parameters.size() >= 6, "The number of parameters should be equal to 6.")) {
        return false;
    }


    m_D = _intakeEvent.getDose() * 1000;
    m_V1 = _parameters.getValue(ParameterId::V1);
    Value v2 = _parameters.getValue(ParameterId::V2);
    m_Ktr = _parameters.getValue(ParameterId::Ktr);
    Value cl = _parameters.getValue(ParameterId::CL);
    Value q = _parameters.getValue(ParameterId::Q);
    m_F = _parameters.getValue(ParameterId::F);

    m_K12 = q / m_V1;
    m_K21 = q / v2;
    m_Ke = cl / m_V1;

    m_nbPoints = _intakeEvent.getNbPoints();
    m_Int = (_intakeEvent.getInterval()).toHours();

    // check the inputs
    bool bOK = checkValue(m_D >= 0, "The dose is negative.");
    bOK &= checkValue(!std::isnan(m_D), "The dose is NaN.");
    bOK &= checkValue(!std::isinf(m_D), "The dose is Inf.");
    bOK &= checkValue(m_V1 > 0, "The volume is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_V1), "The m_V is NaN.");
    bOK &= checkValue(!std::isinf(m_V1), "The _V is Inf.");
    bOK &= checkValue(m_F > 0, "The volume is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_F), "The F is NaN.");
    bOK &= checkValue(!std::isinf(m_F), "The F is Inf.");
    bOK &= checkValue(m_Ke > 0, "The clearance is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_Ke), "The CL is NaN.");
    bOK &= checkValue(!std::isinf(m_Ke), "The CL is Inf.");
    bOK &= checkValue(m_Ktr > 0, "The m_Ktr is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_Ktr), "The m_Ktr is NaN.");
    bOK &= checkValue(!std::isinf(m_Ktr), "The m_Ktr is Inf.");
    bOK &= checkValue(m_K12 > 0, "The m_K12 is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_K12), "The m_K12 is NaN.");
    bOK &= checkValue(!std::isinf(m_K12), "The m_K12 is Inf.");
    bOK &= checkValue(m_K21 > 0, "The m_K21 is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_K21), "The m_K21 is NaN.");
    bOK &= checkValue(!std::isinf(m_K21), "The m_K21 is Inf.");
    bOK &= checkValue(m_nbPoints >= 0, "The number of points is zero or negative.");
    bOK &= checkValue(m_Int > 0, "The interval time is negative.");

    return bOK;
}

}
}

