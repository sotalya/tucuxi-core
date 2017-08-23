/*
* Copyright (C) 2017 Tucuxi SA
*/

#include <Eigen/Dense>

#include "tucucore/twocompartmentextra.h"
#include "tucucore/intakeevent.h"

namespace Tucuxi {
namespace Core {

TwoCompartmentExtraMicro::TwoCompartmentExtraMicro()
{
}

bool TwoCompartmentExtraMicro::checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if(!checkValue(_parameters.size() >= 4, "The number of parameters should be equal to 4."))
	    return false;
    
    m_D = _intakeEvent.getDose() * 1000;
    m_V1 = _parameters.getValue(0);
    m_Ke = _parameters.getValue(1);
    m_K12 = _parameters.getValue(2);
    m_K21 = _parameters.getValue(3);
    m_Ka = _parameters.getValue(4);
    m_F = _parameters.getValue(5);

    Value sumK = m_Ke + m_K12 + m_K21;
    m_RootK = std::sqrt((sumK * sumK) - (4 * m_K21 * m_Ke));
    m_Alpha = (sumK + m_RootK)/2;
    m_Beta = (sumK - m_RootK)/2;
    m_NbPoints = _intakeEvent.getNbPoints();
    m_Int = (_intakeEvent.getInterval()).toHours();

    bool bOK = checkValue(m_D >= 0, "The dose is negative.");
    bOK &= checkValue(!std::isnan(m_D), "The dose is NaN.");
    bOK &= checkValue(!std::isinf(m_D), "The dose is Inf.");
    bOK &= checkValue(m_Ka > 0, "The m_Ka is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_Ka), "The m_Ka is NaN.");
    bOK &= checkValue(!std::isinf(m_Ka), "The m_Ka is Inf.");
    bOK &= checkValue(m_Ke > 0, "The m_Ke is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_Ke), "The m_Ke is NaN.");
    bOK &= checkValue(!std::isinf(m_Ke), "The m_Ke is Inf.");
    bOK &= checkValue(m_K12 > 0, "The K12 is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_K12), "The K12 is NaN.");
    bOK &= checkValue(!std::isinf(m_K12), "The K12 is Inf.");
    bOK &= checkValue(m_K21 > 0, "The K21 is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_K21), "The K21 is NaN.");
    bOK &= checkValue(!std::isinf(m_K21), "The K21 is Inf.");
    bOK &= checkValue(m_F > 0, "The F is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_F), "The F is NaN.");
    bOK &= checkValue(!std::isinf(m_F), "The F is Inf.");
    bOK &= checkValue(m_NbPoints >= 0, "The number of points is zero or negative.");
    bOK &= checkValue( m_Int > 0, "The interval time is negative.");

    return true;
}


void TwoCompartmentExtraMicro::computeLogarithms(Eigen::VectorXd& _times) 
{
    setLogs(Logarithms::Alpha, (-m_Alpha * _times).array().exp());
    setLogs(Logarithms::Beta, (-m_Beta * _times).array().exp());
    setLogs(Logarithms::Ka, (-m_Ka * _times).array().exp());
}


bool TwoCompartmentExtraMicro::computeConcentrations(const Residuals& _inResiduals, Concentrations& _concentrations, Residuals& _outResiduals)
{
    Eigen::VectorXd concentrations1;
    Value concentrations2, concentrations3;

    // Compute concentrations
    bool bOK = compute(_inResiduals, concentrations1, concentrations2, concentrations3);

    // Return concentrations of comp1, comp2 and comp3
    _outResiduals.push_back(concentrations1[m_NbPoints - 1]);
    _outResiduals.push_back(concentrations2);
    _outResiduals.push_back(concentrations3);

    _concentrations.assign(concentrations1.data(), concentrations1.data() + concentrations1.size());	

    bOK &= checkValue(_outResiduals[0] >= 0, "The concentration is negative.");
    bOK &= checkValue(_outResiduals[1] >= 0, "The concentration is negative.");
    bOK &= checkValue(_outResiduals[2] >= 0, "The concentration is negative.");

    return bOK;
}

bool TwoCompartmentExtraMicro::computeConcentration(const Value& _atTime, const Residuals& _inResiduals, Concentrations& _concentrations, Residuals& _outResiduals)
{
    Eigen::VectorXd concentrations1;
    Value concentrations2, concentrations3;

    // Compute concentrations
    bool bOK = compute(_inResiduals, concentrations1, concentrations2, concentrations3);

    // return concentraions (computation with atTime (current time))
    _concentrations.push_back(concentrations1[0]);

    // interval=0 means that it is the last cycle, so final residual = 0
    if (m_Int == 0) {
        concentrations1[1] = 0;
        concentrations2 = 0;
        concentrations3 = 0;
    }

    // Return final residual of comp1, comp2 and comp3 (computation with m_Int (interval))
    _outResiduals.push_back(concentrations1[1]);
    _outResiduals.push_back(concentrations2);
    _outResiduals.push_back(concentrations3);

    bOK &= checkValue(_outResiduals[0] >= 0, "The concentration is negative.");
    bOK &= checkValue(_outResiduals[1] >= 0, "The concentration is negative.");
    bOK &= checkValue(_outResiduals[2] >= 0, "The concentration is negative.");

    return bOK;
}

TwoCompartmentExtraMacro::TwoCompartmentExtraMacro() : TwoCompartmentExtraMicro()
{
}

bool TwoCompartmentExtraMacro::checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if(!checkValue(_parameters.size() >= 4, "The number of parameters should be equal to 4.")) {
        return false;
    }
    
    m_D = _intakeEvent.getDose() * 1000;
    Value cl = _parameters.getValue(0);
    Value q = _parameters.getValue(1);
    Value v2 = _parameters.getValue(2);
    m_V1 = _parameters.getValue(3);
    m_Ka = _parameters.getValue(4);
    m_F = _parameters.getValue(5);
    m_Ke = cl / m_V1;
    m_K12 = q / m_V1;
    m_K21 = q / v2;
    Value sumK = m_Ke + m_K12 + m_K21;
    m_RootK = std::sqrt((sumK * sumK) - (4 * m_K21 * m_Ke));
    m_Alpha = (sumK + m_RootK)/2;
    m_Beta = (sumK - m_RootK)/2;
    m_NbPoints = _intakeEvent.getNbPoints();
    m_Int = (_intakeEvent.getInterval()).toHours();

    bool bOK = checkValue(m_D >= 0, "The dose is negative.");
    bOK &= checkValue(!std::isnan(m_D), "The dose is NaN.");
    bOK &= checkValue(!std::isinf(m_D), "The dose is Inf.");
    bOK &= checkValue(cl > 0, "The clearance is not greater than zero.");
    bOK &= checkValue(!std::isnan(cl), "The CL is NaN.");
    bOK &= checkValue(!std::isinf(cl), "The CL is Inf.");
    bOK &= checkValue(m_V1 > 0, "The volume1 is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_V1), "The m_V1 is NaN.");
    bOK &= checkValue(!std::isinf(m_V1), "The m_V1 is Inf.");
    bOK &= checkValue(v2 > 0, "The volume2 is not greater than zero.");
    bOK &= checkValue(!std::isnan(v2), "The V2 is NaN.");
    bOK &= checkValue(!std::isinf(v2), "The V2 is Inf.");
    bOK &= checkValue(m_Ka > 0, "The Ka is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_Ka), "The Ka is NaN.");
    bOK &= checkValue(!std::isinf(m_Ka), "The Ka is Inf.");
    bOK &= checkValue(m_F > 0, "The F is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_F), "The F is NaN.");
    bOK &= checkValue(!std::isinf(m_F), "The F is Inf.");
    bOK &= checkValue(m_NbPoints >= 0, "The number of points is zero or negative.");
    bOK &= checkValue( m_Int > 0, "The interval time is negative.");

    return true;
}

}
}

