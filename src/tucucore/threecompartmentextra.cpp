/*
* Copyright (C) 2017 Tucuxi SA
*/

#include <Eigen/Dense>

#include "tucucommon/loggerhelper.h"
#include "tucucore/threecompartmentextra.h"
#include "tucucore/intakeevent.h"

namespace Tucuxi {
namespace Core {

#if 0
#define DEBUG
#endif

ThreeCompartmentExtraMicro::ThreeCompartmentExtraMicro()
{
}

bool ThreeCompartmentExtraMicro::checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if(!checkValue(_parameters.size() >= 6, "The number of parameters should be equal to 6."))
	    return false;
    
    Value a0, a1, a2, p, q, r1, r2, phi;

    m_D = _intakeEvent.getDose() * 1000;
    m_F = _parameters.getValue(0);
    m_V1 = _parameters.getValue(1);
    m_Ka = _parameters.getValue(2);
    m_Ke = _parameters.getValue(3);
    m_K12 =_parameters.getValue(4);
    m_K21 =_parameters.getValue(5);
    m_K13 =_parameters.getValue(6);
    m_K31 =_parameters.getValue(7);
    m_NbPoints = _intakeEvent.getNbPoints();
    m_Int = (_intakeEvent.getInterval()).toHours();

#ifdef DEBUG
    Tucuxi::Common::LoggerHelper logHelper;

    logHelper.debug("<<Input Values>>");
    logHelper.debug("m_D: {}", m_D);
    logHelper.debug("m_F: {}", m_F);
    logHelper.debug("m_V1: {}", m_V1);
    logHelper.debug("m_Ke: {}", m_Ke);
    logHelper.debug("m_Ka: {}", m_Ka);
    logHelper.debug("m_K12: {}", m_K12);
    logHelper.debug("m_K21: {}", m_K21);
    logHelper.debug("m_K13: {}", m_K13);
    logHelper.debug("m_K31: {}", m_K31);
    logHelper.debug("m_NbPoints: {}", m_NbPoints);
    logHelper.debug("m_Int: {}", m_Int);
#endif

    bool bOK = checkValue(m_D >= 0, "The dose is negative.");
    bOK &= checkValue(!std::isnan(m_D), "The dose is NaN.");
    bOK &= checkValue(!std::isinf(m_D), "The dose is Inf.");
    bOK &= checkValue(!std::isnan(m_F), "The F is NaN.");
    bOK &= checkValue(!std::isinf(m_F), "The F is Inf.");
    bOK &= checkValue(m_V1 > 0, "The volume1 is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_V1), "The V1 is NaN.");
    bOK &= checkValue(!std::isinf(m_V1), "The V1 is Inf.");
    bOK &= checkValue(m_Ka > 0, "The Ka is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_Ka), "The Ka is NaN.");
    bOK &= checkValue(!std::isinf(m_Ka), "The Ka is Inf.");
    bOK &= checkValue(m_Ke > 0, "The Ke is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_Ke), "The Ke is NaN.");
    bOK &= checkValue(!std::isinf(m_Ke), "The Ke is Inf.");
    bOK &= checkValue(m_K12 > 0, "The K12 is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_K12), "The K12 is NaN.");
    bOK &= checkValue(!std::isinf(m_K12), "The K12 is Inf.");
    bOK &= checkValue(m_K21 > 0, "The K21 is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_K21), "The K21 is NaN.");
    bOK &= checkValue(!std::isinf(m_K21), "The K21 is Inf.");
    bOK &= checkValue(m_K13 > 0, "The K13 is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_K13), "The K13 is NaN.");
    bOK &= checkValue(!std::isinf(m_K13), "The K13 is Inf.");
    bOK &= checkValue(m_K31 > 0, "The K31 is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_K31), "The K31 is NaN.");
    bOK &= checkValue(!std::isinf(m_K31), "The K31 is Inf.");
    bOK &= checkValue(m_NbPoints >= 0, "The number of points is zero or negative.");
    bOK &= checkValue( m_Int > 0, "The interval time is negative.");

    a0 = m_Ke * m_K21 * m_K31;
    a1 = m_Ke * m_K31 + m_K21 * m_K31 + m_K21 * m_K13 + m_Ke * m_K21 + m_K31 * m_K12;
    a2 = m_Ke + m_K12 + m_K13 + m_K21 + m_K31;
    p = a1 - std::pow(a2,2) / 3;
    q = 2 * std::pow(a2,3) / 27 - a1 * a2 / 3 + a0;
    r1 = std::sqrt(-(std::pow(p,3) / 27));
    r2 = 2 * std::pow(r1,1/3);
    phi = std::acos(- q / (2 * r1)) / 3;

    m_Alpha = - (std::cos(phi) * r2 - a2 / 3);
    m_Beta = - (std::cos(phi + 2 * 3.1428 / 3) * r2 - a2/3);
    m_Gamma = - (std::cos(phi + 4 * 3.1428/3) * r2 - a2/3);

    return bOK;
}

void ThreeCompartmentExtraMicro::computeExponentials(Eigen::VectorXd& _times) 
{
    setExponentials(Exponentials::Alpha, (-m_Alpha * _times).array().exp());
    setExponentials(Exponentials::Beta, (-m_Beta * _times).array().exp());
    setExponentials(Exponentials::Gamma, (-m_Gamma * _times).array().exp());
    setExponentials(Exponentials::Ka, (-m_Ka * _times).array().exp());
}


bool ThreeCompartmentExtraMicro::computeConcentrations(const Residuals& _inResiduals, Concentrations& _concentrations, Residuals& _outResiduals)
{
    Eigen::VectorXd concentrations1;
    Value concentrations2, concentrations3;

    // Calculate concentrations for comp1 and comp2
    compute(_inResiduals, concentrations1, concentrations2, concentrations3);

    // return residuals of comp1, comp2 and comp3
    _outResiduals.push_back(concentrations1[m_NbPoints - 1]);
    _outResiduals.push_back(concentrations2);
    _outResiduals.push_back(concentrations3);

    // return concentration
    _concentrations.assign(concentrations1.data(), concentrations1.data() + concentrations1.size());	

    bool bOK = checkValue(_outResiduals[0] >= 0, "The concentration1 is negative.");
    bOK &= checkValue(_outResiduals[1] >= 0, "The concentration2 is negative.");
    bOK &= checkValue(_outResiduals[2] >= 0, "The concentration3 is negative.");

    return bOK;
}

bool ThreeCompartmentExtraMicro::computeConcentration(const Value& _atTime, const Residuals& _inResiduals, Concentrations& _concentrations, Residuals& _outResiduals)
{
    Eigen::VectorXd concentrations1;
    Value concentrations2, concentrations3;

    // Calculate concentrations for comp1 and comp2
    compute(_inResiduals, concentrations1, concentrations2, concentrations3);

    // return concentraions (computation with atTime (current time))
    _concentrations.push_back(concentrations1[0]);

    // interval=0 means that it is the last cycle, so final residual = 0
    if (m_Int == 0) {
        concentrations1[1] = 0;
        concentrations2 = 0;
        concentrations3 = 0;
    }

    // return final residual (computation with m_Int (interval))
    _outResiduals.push_back(concentrations1[1]);
    _outResiduals.push_back(concentrations2);
    _outResiduals.push_back(concentrations3);

    bool bOK = checkValue(_outResiduals[0] >= 0, "The concentration1 is negative.");
    bOK &= checkValue(_outResiduals[1] >= 0, "The concentration2 is negative.");
    bOK &= checkValue(_outResiduals[2] >= 0, "The concentration3 is negative.");

    return bOK;
}

ThreeCompartmentExtraMacro::ThreeCompartmentExtraMacro()
{
}

bool ThreeCompartmentExtraMacro::checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if(!checkValue(_parameters.size() >= 6, "The number of parameters should be equal to 6."))
	    return false;
    
    Value a0, a1, a2, p, q, r1, r2, phi;

    m_D = _intakeEvent.getDose() * 1000;
    Value cl = _parameters.getValue(0);
    m_F = _parameters.getValue(1);
    Value q1 = _parameters.getValue(2);
    Value q2 = _parameters.getValue(3);
    m_V1 = _parameters.getValue(4);
    Value v2 = _parameters.getValue(5);
    m_Ka = _parameters.getValue(6);
    m_Ke = cl / m_V1;
    m_K12 = q1 / m_V1;
    m_K21 = q1 / v2;
    m_K13 = q2 / m_V1;
    m_K31 = q2 / v2;
    m_NbPoints = _intakeEvent.getNbPoints();
    m_Int = (_intakeEvent.getInterval()).toHours();

#ifdef DEBUG
    Tucuxi::Common::LoggerHelper logHelper;

    logHelper.debug("<<Input Values>>");
    logHelper.debug("m_D: {}", m_D);
    logHelper.debug("cl: {}", cl);
    logHelper.debug("m_F: {}", m_F);
    logHelper.debug("q1: {}", q1);
    logHelper.debug("q2: {}", q2);
    logHelper.debug("m_V1: {}", m_V1);
    logHelper.debug("v2: {}", v2);
    logHelper.debug("m_Ka: {}", m_Ka);
    logHelper.debug("m_Ke: {}", m_Ke);
    logHelper.debug("m_K12: {}", m_K12);
    logHelper.debug("m_K21: {}", m_K21);
    logHelper.debug("m_K13: {}", m_K13);
    logHelper.debug("m_K31: {}", m_K31);
    logHelper.debug("m_NbPoints: {}", m_NbPoints);
    logHelper.debug("m_Int: {}", m_Int);
#endif

    bool bOK = checkValue(m_D >= 0, "The dose is negative.");
    bOK &= checkValue(!std::isnan(m_D), "The dose is NaN.");
    bOK &= checkValue(!std::isinf(m_D), "The dose is Inf.");
    bOK &= checkValue(cl > 0, "The clearance is not greater than zero.");
    bOK &= checkValue(!std::isnan(cl), "The CL is NaN.");
    bOK &= checkValue(!std::isinf(cl), "The CL is Inf.");
    bOK &= checkValue(m_F > 0, "The F is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_F), "The F is NaN.");
    bOK &= checkValue(!std::isinf(m_F), "The F is Inf.");
    bOK &= checkValue(q1 > 0, "The Q1 is not greater than zero.");
    bOK &= checkValue(!std::isnan(q1), "The Q1 is NaN.");
    bOK &= checkValue(!std::isinf(q1), "The Q1 is Inf.");
    bOK &= checkValue(q2 > 0, "The Q2 is not greater than zero.");
    bOK &= checkValue(!std::isnan(q2), "The Q2 is NaN.");
    bOK &= checkValue(!std::isinf(q2), "The Q2 is Inf.");
    bOK &= checkValue(m_V1 > 0, "The volume1 is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_V1), "The V1 is NaN.");
    bOK &= checkValue(!std::isinf(m_V1), "The V1 is Inf.");
    bOK &= checkValue(v2 > 0, "The volume2 is not greater than zero.");
    bOK &= checkValue(!std::isnan(v2), "The V2 is NaN.");
    bOK &= checkValue(!std::isinf(v2), "The V2 is Inf.");

    a0 = m_Ke * m_K21 * m_K31;
    a1 = m_Ke * m_K31 + m_K21 * m_K31 + m_K21 * m_K13 + m_Ke * m_K21 + m_K31 * m_K12;
    a2 = m_Ke + m_K12 + m_K13 + m_K21 + m_K31;
    p = a1 - std::pow(a2,2) / 3;
    q = 2 * std::pow(a2,3) / 27 - a1 * a2 / 3 + a0;
    r1 = std::sqrt(-(std::pow(p,3) / 27));
    r2 = 2 * std::pow(r1,1/3);
    phi = std::acos(- q / (2 * r1)) / 3;

    m_Alpha = - (std::cos(phi) * r2 - a2 / 3);
    m_Beta = - (std::cos(phi + 2 * 3.1428 / 3) * r2 - a2/3);
    m_Gamma = - (std::cos(phi + 4 * 3.1428/3) * r2 - a2/3);

    return bOK;
}

}
}

