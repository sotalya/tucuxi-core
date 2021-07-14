/*
* Copyright (C) 2017 Tucuxi SA
*/

#include <Eigen/Dense>

#include "tucucommon/loggerhelper.h"
#include "tucucore/pkmodels/twocompartmentbolus.h"
#include "tucucore/intakeevent.h"

namespace Tucuxi {
namespace Core {

#if 0
#define DEBUG
#endif

TwoCompartmentBolusMicro::TwoCompartmentBolusMicro() : IntakeIntervalCalculatorBase<2, TwoCompartmentBolusExponentials> (new PertinentTimesCalculatorStandard())
{
}

std::vector<std::string> TwoCompartmentBolusMicro::getParametersId()
{
    return {"Ke", "V1", "K12", "K21"};
}


bool TwoCompartmentBolusMicro::checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkCondition(_parameters.size() >= 4, "The number of parameters should be equal to 4.")) {
        return false;
    }
    
    m_D = _intakeEvent.getDose() * 1000;
    m_V1 = _parameters.getValue(ParameterId::V1);
    m_Ke = _parameters.getValue(ParameterId::Ke);
    m_K12 = _parameters.getValue(ParameterId::K12);
    m_K21 = _parameters.getValue(ParameterId::K21);
    m_nbPoints = static_cast<Eigen::Index>(_intakeEvent.getNbPoints());
    m_Int = (_intakeEvent.getInterval()).toHours();

    Value sumK = m_Ke + m_K12 + m_K21;
    m_RootK = std::sqrt((sumK * sumK) - (4 * m_K21 * m_Ke));
    m_Alpha = (sumK + m_RootK)/2;
    m_Beta = (sumK - m_RootK)/2;

    // check the inputs
    bool bOK = checkPositiveValue(m_D, "The dose");
    bOK &= checkStrictlyPositiveValue(m_Ke, "Ke");
    bOK &= checkStrictlyPositiveValue(m_K12, "K12");
    bOK &= checkStrictlyPositiveValue(m_K21, "K21");
    bOK &= checkPositiveValue(m_Alpha, "Alpha");
    bOK &= checkPositiveValue(m_Beta, "Beta");
    bOK &= checkCondition(m_nbPoints > 0, "The number of points is zero or negative.");
    bOK &= checkCondition(m_Int > 0, "The interval time is negative.");


#ifdef DEBUG
    Tucuxi::Common::LoggerHelper logHelper;

    logHelper.debug("<<Input Values>>");
    logHelper.debug("m_D: {}", m_D);
    logHelper.debug("m_V1: {}", m_V1);
    logHelper.debug("m_Ke: {}", m_Ke);
    logHelper.debug("m_K12: {}", m_K12);
    logHelper.debug("m_K21: {}", m_K21);
    logHelper.debug("m_nbPoints: {}", m_nbPoints);
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


bool TwoCompartmentBolusMicro::computeConcentrations(const Residuals& _inResiduals, bool _isAll, std::vector<Concentrations>& _concentrations, Residuals& _outResiduals)
{
    Eigen::VectorXd concentrations1, concentrations2;
    size_t firstCompartment = static_cast<size_t>(Compartments::First);
    size_t secondCompartment = static_cast<size_t>(Compartments::Second);

    // Calculate concentrations for comp1 and comp2
    compute(_inResiduals, concentrations1, concentrations2);

    // return residuals of comp1 and comp2
    _outResiduals[firstCompartment] = concentrations1[m_nbPoints - 1];
    _outResiduals[secondCompartment] = concentrations2[m_nbPoints - 1];

    // return concentration of comp1
    _concentrations[firstCompartment].assign(concentrations1.data(), concentrations1.data() + concentrations1.size());	
    // Return concentrations of other compartments
    if (_isAll == true) {
	_concentrations[secondCompartment].assign(concentrations2.data(), concentrations2.data() + concentrations2.size());	
    }


    bool bOK = checkCondition(_outResiduals[firstCompartment] >= 0, "The concentration1 is negative.");
    bOK &= checkCondition(_outResiduals[secondCompartment] >= 0, "The concentration2 is negative.");

    return bOK;
}


bool TwoCompartmentBolusMicro::computeConcentration(const Value& _atTime, const Residuals& _inResiduals, bool _isAll, std::vector<Concentrations>& _concentrations, Residuals& _outResiduals)
{
    TMP_UNUSED_PARAMETER(_atTime);
    Eigen::VectorXd concentrations1, concentrations2;
    size_t firstCompartment = static_cast<size_t>(Compartments::First);
    size_t secondCompartment = static_cast<size_t>(Compartments::Second);
    Eigen::Index atTime = static_cast<Eigen::Index>(SingleConcentrations::AtTime);
    Eigen::Index atEndInterval = static_cast<Eigen::Index>(SingleConcentrations::AtEndInterval);

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

    bool bOK = checkCondition(_outResiduals[firstCompartment] >= 0, "The concentration1 is negative.");
    bOK &= checkCondition(_outResiduals[secondCompartment] >= 0, "The concentration2 is negative.");

    return bOK;
}

TwoCompartmentBolusMacro::TwoCompartmentBolusMacro() : TwoCompartmentBolusMicro()
{
}


std::vector<std::string> TwoCompartmentBolusMacro::getParametersId()
{
    return {"CL", "V1", "Q", "V2"};
}

bool TwoCompartmentBolusMacro::checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if(!checkCondition(_parameters.size() >= 4, "The number of parameters should be equal to 4.")) {
        return false;
    }
    
    m_D = _intakeEvent.getDose() * 1000;
    Value cl = _parameters.getValue(ParameterId::CL); 
    Value q = _parameters.getValue(ParameterId::Q); 
    m_V1 = _parameters.getValue(ParameterId::V1); 
    Value v2 = _parameters.getValue(ParameterId::V2); 
    m_Ke = cl / m_V1;
    m_K12 = q / m_V1;
    m_K21 = q / v2;
    m_nbPoints = static_cast<Eigen::Index>(_intakeEvent.getNbPoints());
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
    logHelper.debug("m_nbPoints: {}", m_nbPoints);
    logHelper.debug("m_Int: {}", m_Int);
#endif

    // check the inputs
    bool bOK = checkPositiveValue(m_D, "The dose");
    bOK &= checkStrictlyPositiveValue(cl, "The clearance");
    bOK &= checkStrictlyPositiveValue(q, "Q");
    bOK &= checkStrictlyPositiveValue(m_V1, "V1");
    bOK &= checkStrictlyPositiveValue(v2, "V2");
    bOK &= checkPositiveValue(m_Alpha, "Alpha");
    bOK &= checkPositiveValue(m_Beta, "Beta");
    bOK &= checkCondition(m_nbPoints > 0, "The number of points is zero or negative.");
    bOK &= checkCondition(m_Int > 0, "The interval time is negative.");

    return bOK;
}


}
}

