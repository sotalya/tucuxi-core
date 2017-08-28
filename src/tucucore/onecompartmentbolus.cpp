/*
* Copyright (C) 2017 Tucuxi SA
*/

#include <Eigen/Dense>

#include "tucucommon/loggerhelper.h"
#include "tucucore/onecompartmentbolus.h"
#include "tucucore/intakeevent.h"

namespace Tucuxi {
namespace Core {

#if 0
#define DEBUG
#endif

OneCompartmentBolusMicro::OneCompartmentBolusMicro()
{
}

bool OneCompartmentBolusMicro::checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if(!checkValue(_parameters.size() >= 2, "The number of parameters should be equal to 2."))
	    return false;
    
    m_D = _intakeEvent.getDose() * 1000;
    m_V = _parameters.getValue(0);
    m_Ke = _parameters.getValue(1);
    m_NbPoints = _intakeEvent.getNbPoints();
    m_Int = static_cast<int>((_intakeEvent.getInterval()).toHours());

#ifdef DEBUG
    Tucuxi::Common::LoggerHelper logHelper;

    logHelper.debug("<<Input Values>>");
    logHelper.debug("m_D: {}", m_D);
    logHelper.debug("m_V: {}", m_V);
    logHelper.debug("m_Ke: {}", m_Ke);
    logHelper.debug("m_NbPoints: {}", m_NbPoints);
    logHelper.debug("m_Int: {}", m_Int);
#endif

    // check the inputs
    bool bOK = checkValue(m_D >= 0, "The dose is negative.");
    bOK &= checkValue(!std::isnan(m_D), "The dose is NaN.");
    bOK &= checkValue(!std::isinf(m_D), "The dose is Inf.");
    bOK &= checkValue(m_V > 0, "The volume is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_V), "The V is NaN.");
    bOK &= checkValue(!std::isinf(m_V), "The V is Inf.");
    bOK &= checkValue(m_Ke > 0, "The Ke is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_Ke), "The Ke is NaN.");
    bOK &= checkValue(!std::isinf(m_Ke), "The Ke is Inf.");
    bOK &= checkValue(m_NbPoints >= 0, "The number of points is zero or negative.");
    bOK &= checkValue(m_Int > 0, "The interval time is negative.");

    return bOK;
}


void OneCompartmentBolusMicro::computeExponentials(Eigen::VectorXd& _times)
{
    setExponentials(Exponentials::Ke, (-m_Ke * _times).array().exp());
}


bool OneCompartmentBolusMicro::computeConcentrations(const Residuals& _inResiduals, Concentrations& _concentrations, Residuals& _outResiduals)
{
    Eigen::VectorXd concentrations;

    // Compute concentrations
    compute(_inResiduals, concentrations);

    // Return concentraions nd finla residual
    _outResiduals.push_back(concentrations[m_NbPoints - 1]);
    _concentrations.assign(concentrations.data(), concentrations.data() + concentrations.size());
    
    return checkValue(_outResiduals[0] >= 0, "The concentration is negative.");
}

bool OneCompartmentBolusMicro::computeConcentration(const Value& _atTime, const Residuals& _inResiduals, Concentrations& _concentrations, Residuals& _outResiduals)
{
    Eigen::VectorXd concentrations;

    // Compute concentrations
    compute(_inResiduals, concentrations);

    // Return concentrations (computation with atTime (current time))
    _concentrations.push_back(concentrations[0]);
    
    // interval=0 means that it is the last cycle, so final residual = 0
    if (m_Int == 0) {
        concentrations[1] = 0;
    }

    // Return final residual (computation with m_Int (interval))
    _outResiduals.push_back(concentrations[1]);
    
    return checkValue(_outResiduals[0] >= 0, "The concentration is negative.");
}

OneCompartmentBolusMacro::OneCompartmentBolusMacro() : OneCompartmentBolusMicro()
{
}

bool OneCompartmentBolusMacro::checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if(!checkValue(_parameters.size() >= 2, "The number of parameters should be equal to 2."))
        return false;

    m_D = _intakeEvent.getDose() * 1000;
    m_V = _parameters.getValue(0);
    Value cl = _parameters.getValue(1);
    m_Ke = cl / m_V;
    m_NbPoints = _intakeEvent.getNbPoints();
    m_Int = (_intakeEvent.getInterval()).toHours();

#ifdef DEBUG
    Tucuxi::Common::LoggerHelper logHelper;

    logHelper.debug("<<Input Values>>");
    logHelper.debug("m_D: {}", m_D);
    logHelper.debug("m_V: {}", m_V);
    logHelper.debug("cl: {}", cl);
    logHelper.debug("m_Ke: {}", m_Ke);
    logHelper.debug("m_NbPoints: {}", m_NbPoints);
    logHelper.debug("m_Int: {}", m_Int);
#endif

    // check the inputs
    bool bOK = checkValue(m_D >= 0, "The dose is negative.");
    bOK &= checkValue(!std::isnan(m_D), "The dose is NaN.");
    bOK &= checkValue(!std::isinf(m_D), "The dose is Inf.");
    bOK &= checkValue(m_V > 0, "The volume is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_V), "The V is NaN.");
    bOK &= checkValue(!std::isinf(m_V), "The V is Inf.");
    bOK &= checkValue(cl > 0, "The clearance is not greater than zero.");
    bOK &= checkValue(!std::isnan(cl), "The Ke is NaN.");
    bOK &= checkValue(!std::isinf(cl), "The Ke is Inf.");
    bOK &= checkValue(m_NbPoints >= 0, "The number of points is zero or negative.");
    bOK &= checkValue(m_Int > 0, "The interval time is negative.");

    return bOK;
}



}
}
