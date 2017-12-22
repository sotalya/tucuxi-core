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
    m_V1 = _parameters.getValue(ParameterId::V1);
    m_Ke = _parameters.getValue(ParameterId::Ke);
    m_K12 = _parameters.getValue(ParameterId::K12);
    m_K21 = _parameters.getValue(ParameterId::K21);
    m_NbPoints = _intakeEvent.getNbPoints();
    m_Int = (_intakeEvent.getInterval()).toHours();

    Value sumK = m_Ke + m_K12 + m_K21;
    m_RootK = std::sqrt((sumK * sumK) - (4 * m_K21 * m_Ke));
    m_Alpha = (sumK + m_RootK)/2;
    m_Beta = (sumK - m_RootK)/2;

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
    bOK &= checkValue(m_NbPoints >= 0, "The number of points is negative.");
    bOK &= checkValue(m_Int > 0, "The interval time is not greater than zero.");
    bOK &= checkValue(m_Alpha >= 0, "Alpha is negative.");
    bOK &= checkValue(m_Beta >= 0, "Beta is negative.");

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
    logHelper.debug("m_Alpha: {}", m_Alpha);
    logHelper.debug("m_Beta: {}", m_Beta);
#endif

    return bOK;
}


void TwoCompartmentBolusMicro::computeExponentials(Eigen::VectorXd& _times) 
{
    setExponentials(Exponentials::Alpha, (-m_Alpha * _times).array().exp());
    setExponentials(Exponentials::Beta, (-m_Beta * _times).array().exp());
}


bool TwoCompartmentBolusMicro::computeConcentrations(const Residuals& _inResiduals, const bool _isAll, std::vector<Concentrations>& _concentrations, Residuals& _outResiduals)
{
    Eigen::VectorXd concentrations1, concentrations2;
    int firstCompartment = static_cast<int>(Compartments::First);
    int secondCompartment = static_cast<int>(Compartments::Second);

    // Calculate concentrations for comp1 and comp2
    compute(_inResiduals, concentrations1, concentrations2);

    // return residuals of comp1 and comp2
    _outResiduals[firstCompartment] = concentrations1[m_NbPoints - 1];
    _outResiduals[secondCompartment] = concentrations2[m_NbPoints - 1];

    // return concentration of comp1
    _concentrations[firstCompartment].assign(concentrations1.data(), concentrations1.data() + concentrations1.size());	
    // Return concentrations of other compartments
    if (_isAll == true) {
	_concentrations[secondCompartment].assign(concentrations2.data(), concentrations2.data() + concentrations2.size());	
    }


    bool bOK = checkValue(_outResiduals[firstCompartment] >= 0, "The concentration1 is negative.");
    bOK &= checkValue(_outResiduals[secondCompartment] >= 0, "The concentration2 is negative.");

    return bOK;
}


bool TwoCompartmentBolusMicro::computeConcentration(const Value& _atTime, const Residuals& _inResiduals, const bool _isAll, std::vector<Concentrations>& _concentrations, Residuals& _outResiduals)
{
    Eigen::VectorXd concentrations1, concentrations2;
    int firstCompartment = static_cast<int>(Compartments::First);
    int secondCompartment = static_cast<int>(Compartments::Second);
    int atTime = static_cast<int>(SingleConcentrations::AtTime);
    int atEndInterval = static_cast<int>(SingleConcentrations::AtEndInterval);

    // Calculate concentrations for comp1 and comp2
    compute(_inResiduals, concentrations1, concentrations2);

    // return concentraions (computation with atTime (current time))
    _concentrations[firstCompartment].push_back(concentrations1[atTime]);
    if (_isAll == true) {
	_concentrations[secondCompartment].push_back(concentrations2[atTime]);
    }

    // interval=0 means that it is the last cycle, so final residual = 0
    if (m_Int == 0) {
        concentrations1[atEndInterval] = 0;
        concentrations2[atEndInterval] = 0;
    }

    // return final residual (computation with m_Int (interval))
    _outResiduals[firstCompartment] = concentrations1[atEndInterval];
    _outResiduals[secondCompartment] = concentrations2[atEndInterval];

    bool bOK = checkValue(_outResiduals[firstCompartment] >= 0, "The concentration1 is negative.");
    bOK &= checkValue(_outResiduals[secondCompartment] >= 0, "The concentration2 is negative.");

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
    Value cl = _parameters.getValue(ParameterId::Cl); 
    Value q = _parameters.getValue(ParameterId::Q); 
    m_V1 = _parameters.getValue(ParameterId::V1); 
    Value v2 = _parameters.getValue(ParameterId::V2); 
    m_Ke = cl / m_V1;
    m_K12 = q / m_V1;
    m_K21 = q / v2;
    m_NbPoints = _intakeEvent.getNbPoints();
    m_Int = (_intakeEvent.getInterval()).toHours();

    Value sumK = m_Ke + m_K12 + m_K21;
    m_RootK = std::sqrt((sumK * sumK) - (4 * m_K21 * m_Ke));
    m_Alpha = (sumK + m_RootK)/2;
    m_Beta = (sumK - m_RootK)/2;

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
    bOK &= checkValue(m_Int > 0, "The interval time is not greater than zero.");
    bOK &= checkValue(m_Alpha >= 0, "Alpha is negative.");
    bOK &= checkValue(m_Beta >= 0, "Beta is negative.");

    return bOK;
}


}
}

