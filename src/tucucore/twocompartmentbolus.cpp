/*
* Copyright (C) 2017 Tucuxi SA
*/

#include <Eigen/Dense>

#include "tucucommon/loggerhelper.h"
#include "tucucore/twocompartmentbolus.h"
#include "tucucore/intakeevent.h"

namespace Tucuxi {
namespace Core {

#if 0
#define DEBUG
#endif

TwoCompartmentBolusMicro::TwoCompartmentBolusMicro()
{
}

bool TwoCompartmentBolusMicro::checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if(!checkValue(_parameters.size() >= 4, "The number of parameters should be equal to 4."))
	    return false;
    
    m_D = _intakeEvent.getDose() * 1000;
    m_V1 = _parameters.getValue(0);
    m_Ke = _parameters.getValue(1);
    m_K12 = _parameters.getValue(2);
    m_K21 = _parameters.getValue(3);
    m_NbPoints = _intakeEvent.getNbPoints();
    m_Int = (_intakeEvent.getInterval()).toHours();

#ifdef DEBUG
    Tucuxi::Common::LoggerHelper logHelper;

    logHelper.debug("<<Input Values>>");
    logHelper.debug("m_D: {}", m_D);
    logHelper.debug("m_V1: {}", m_V1);
    logHelper.debug("m_Ke: {}", m_Ke);
    logHelper.debug("m_K12: {}", m_K12);
    logHelper.debug("m_K21: {}", m_K21);
    logHelper.debug("m_NbPoints: {}", m_NbPoints);
    logHelper.debug("m_Int: {}", m_Int);
#endif

    bool bOK = checkValue(m_D >= 0, "The dose is negative.");
    bOK &= checkValue(!std::isnan(m_D), "The dose is NaN.");
    bOK &= checkValue(!std::isinf(m_D), "The dose is Inf.");
    bOK &= checkValue(m_Ke > 0, "The Ke is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_Ke), "The Ke is NaN.");
    bOK &= checkValue(!std::isinf(m_Ke), "The Ke is Inf.");
    bOK &= checkValue(m_K12 > 0, "The K12 is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_K12), "The K12 is NaN.");
    bOK &= checkValue(!std::isinf(m_K12), "The K12 is Inf.");
    bOK &= checkValue(m_K21 > 0, "The K21 is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_K21), "The K21 is NaN.");
    bOK &= checkValue(!std::isinf(m_K21), "The K21 is Inf.");
    bOK &= checkValue(m_NbPoints >= 0, "The number of points is zero or negative.");
    bOK &= checkValue(m_Int > 0, "The interval time is negative.");

    Value sumK = m_Ke + m_K12 + m_K21;
    m_RootK = std::sqrt((sumK * sumK) - (4 * m_K21 * m_Ke));
    m_Alpha = (sumK + m_RootK)/2;
    m_Beta = (sumK - m_RootK)/2;

    return bOK;
}


void TwoCompartmentBolusMicro::computeExponentials(Eigen::VectorXd& _times) 
{
    setExponentials(Exponentials::Alpha, (-m_Alpha * _times).array().exp());
    setExponentials(Exponentials::Beta, (-m_Beta * _times).array().exp());
}


bool TwoCompartmentBolusMicro::computeConcentrations(const Residuals& _inResiduals, Concentrations& _concentrations, Residuals& _outResiduals)
{
    Eigen::VectorXd concentrations1, concentrations2;

    // Calculate concentrations for comp1 and comp2
    compute(_inResiduals, concentrations1, concentrations2);

    // return concentrations of comp1 and comp2
    _outResiduals.push_back(concentrations1[m_NbPoints - 1]);
    _outResiduals.push_back(concentrations2[m_NbPoints - 1]);

    _concentrations.assign(concentrations1.data(), concentrations1.data() + concentrations1.size());	

    bool bOK = checkValue(_outResiduals[0] >= 0, "The concentration1 is negative.");
    bOK &= checkValue(_outResiduals[1] >= 0, "The concentration2 is negative.");

    return bOK;
}


bool TwoCompartmentBolusMicro::computeConcentration(const Value& _atTime, const Residuals& _inResiduals, Concentrations& _concentrations, Residuals& _outResiduals)
{
    Eigen::VectorXd concentrations1, concentrations2;

    // Calculate concentrations for comp1 and comp2
    compute(_inResiduals, concentrations1, concentrations2);

    // return concentraions (computation with atTime (current time))
    _concentrations.push_back(concentrations1[0]);
    _concentrations.push_back(concentrations2[0]);

    // interval=0 means that it is the last cycle, so final residual = 0
    if (m_Int == 0) {
        concentrations1[1] = 0;
        concentrations2[1] = 0;
    }

    // return final residual (computation with m_Int (interval))
    _outResiduals.push_back(concentrations1[1]);
    _outResiduals.push_back(concentrations2[1]);

    bool bOK = checkValue(_outResiduals[0] >= 0, "The concentration1 is negative.");
    bOK &= checkValue(_outResiduals[1] >= 0, "The concentration2 is negative.");

    return bOK;
}

TwoCompartmentBolusMacro::TwoCompartmentBolusMacro() : TwoCompartmentBolusMicro()
{
}

bool TwoCompartmentBolusMacro::checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if(!checkValue(_parameters.size() >= 4, "The number of parameters should be equal to 4.")) {
        return false;
    }
    
    m_D = _intakeEvent.getDose() * 1000;
    Value cl = _parameters.getValue(0); /// clearance
    Value q = _parameters.getValue(1); /// speed between c1 and c2
    m_V1 = _parameters.getValue(2); /// volume of first compartment
    Value v2 = _parameters.getValue(3); /// volue of second ompartment
    m_Ke = cl / m_V1;
    m_K12 = q / m_V1;
    m_K21 = q / v2;
    m_NbPoints = _intakeEvent.getNbPoints();
    m_Int = (_intakeEvent.getInterval()).toHours();

#ifdef DEBUG
    Tucuxi::Common::LoggerHelper logHelper;

    logHelper.debug("<<Input Values>>");
    logHelper.debug("m_D: {}", m_D);
    logHelper.debug("cl: {}", cl);
    logHelper.debug("q: {}", q);
    logHelper.debug("m_V1: {}", m_V1);
    logHelper.debug("v2: {}", v2);
    logHelper.debug("m_Ke: {}", m_Ke);
    logHelper.debug("m_K12: {}", m_K12);
    logHelper.debug("m_K21: {}", m_K21);
    logHelper.debug("m_NbPoints: {}", m_NbPoints);
    logHelper.debug("m_Int: {}", m_Int);
#endif

    bool bOK = checkValue(m_D >= 0, "The dose is negative.");
    bOK &= checkValue(!std::isnan(m_D), "The dose is NaN.");
    bOK &= checkValue(!std::isinf(m_D), "The dose is Inf.");
    bOK &= checkValue(cl > 0, "The clearance is not greater than zero.");
    bOK &= checkValue(!std::isnan(cl), "The CL is NaN.");
    bOK &= checkValue(!std::isinf(cl), "The CL is Inf.");
    bOK &= checkValue(q > 0, "The Q is not greater than zero.");
    bOK &= checkValue(!std::isnan(q), "The Q is NaN.");
    bOK &= checkValue(!std::isinf(q), "The Q is Inf.");
    bOK &= checkValue(m_V1 > 0, "The volume1 is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_V1), "The m_V1 is NaN.");
    bOK &= checkValue(!std::isinf(m_V1), "The m_V1 is Inf.");
    bOK &= checkValue(v2 > 0, "The volume2 is not greater than zero.");
    bOK &= checkValue(!std::isnan(v2), "The V2 is NaN.");
    bOK &= checkValue(!std::isinf(v2), "The V2 is Inf.");
    bOK &= checkValue(m_NbPoints >= 0, "The number of points is zero or negative.");
    bOK &= checkValue(m_Int > 0, "The interval time is negative.");

    Value sumK = m_Ke + m_K12 + m_K21;
    m_RootK = std::sqrt((sumK * sumK) - (4 * m_K21 * m_Ke));
    m_Alpha = (sumK + m_RootK)/2;
    m_Beta = (sumK - m_RootK)/2;

    return bOK;
}


}
}

