/*
* Copyright (C) 2017 Tucuxi SA
*/

#include <Eigen/Dense>
#include <math.h>
#include <algorithm>

#include "tucucommon/loggerhelper.h"
#include "tucucore/pkmodels/onecompartmentinfusion.h"
#include "tucucore/intakeevent.h"

namespace Tucuxi {
namespace Core {

#if 0
#define DEBUG
#endif

OneCompartmentInfusionMicro::OneCompartmentInfusionMicro() : IntakeIntervalCalculatorBase<1, OneCompartmentInfusionExponentials> (new PertinentTimesCalculatorInfusion())
{
}

bool OneCompartmentInfusionMicro::checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkValue(_parameters.size() >= 2, "The number of parameters should be equal to 2.")) {
        return false;
    }

    m_D = _intakeEvent.getDose() * 1000;
    m_Ke = _parameters.getValue(ParameterId::Ke);
    m_V = _parameters.getValue(ParameterId::V);
    m_Tinf = (_intakeEvent.getInfusionTime()).toHours();
    m_Int = (_intakeEvent.getInterval()).toHours();
    m_NbPoints = _intakeEvent.getNbPoints();

#ifdef DEBUG
    Tucuxi::Common::LoggerHelper logHelper;

    logHelper.debug("<<Input Values>>");
    logHelper.debug("m_D: {}", m_D);
    logHelper.debug("m_V: {}", m_V);
    logHelper.debug("m_Ke: {}", m_Ke);
    logHelper.debug("m_Tinf: {}", m_Tinf);
    logHelper.debug("m_Int: {}", m_Int);
    logHelper.debug("m_NbPoints: {}", m_NbPoints);
#endif

    bool bOK = checkValue(m_D >= 0, "The dose is negative.");
    bOK &= checkValue(!std::isnan(m_D), "The dose is NaN.");
    bOK &= checkValue(!std::isinf(m_D), "The dose is Inf.");
    bOK &= checkValue(m_V > 0, "The volume is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_V), "The volume is NaN.");
    bOK &= checkValue(!std::isinf(m_V), "The volume is Inf.");
    bOK &= checkValue(m_Ke > 0, "The clearance is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_Ke), "The CL is NaN.");
    bOK &= checkValue(!std::isinf(m_Ke), "The CL is Inf.");
    bOK &= checkValue(m_Tinf >= 0, "The infusion time is zero or negative.");
    bOK &= checkValue(m_Int > 0, "The interval time is negative.");

    return bOK;
}


void OneCompartmentInfusionMicro::computeExponentials(Eigen::VectorXd& _times)
{
    setExponentials(Exponentials::Ke, (-m_Ke * _times).array().exp());
}


bool OneCompartmentInfusionMicro::computeConcentrations(const Residuals& _inResiduals, bool _isAll, std::vector<Concentrations>& _concentrations, Residuals& _outResiduals)
{
    Eigen::VectorXd concentrations;
    int firstCompartment = static_cast<int>(Compartments::First);
    int forcesize;
    if (m_NbPoints == 2) {
        forcesize = static_cast<int>(std::min(ceil(m_Tinf/m_Int * m_NbPoints), ceil(m_NbPoints)));
    }
    else {
        forcesize = std::min(m_NbPoints, std::max(2, static_cast<int>((m_Tinf / m_Int) * static_cast<double>(m_NbPoints))));
    }

    // Calculate concentrations
    compute(_inResiduals, forcesize, concentrations);

    // Return final Residual
    _outResiduals[firstCompartment] = concentrations[m_NbPoints - 1];

    // Return concentraions of first compartment
    _concentrations[firstCompartment].assign(concentrations.data(), concentrations.data() + concentrations.size());	
    // Only one compartment is existed.
    TMP_UNUSED_PARAMETER(_isAll);

    return checkValue(_outResiduals[firstCompartment] >= 0, "The concentration is negative.");
}

bool OneCompartmentInfusionMicro::computeConcentration(const Value& _atTime, const Residuals& _inResiduals, bool _isAll, std::vector<Concentrations>& _concentrations, Residuals& _outResiduals)
{
    Eigen::VectorXd concentrations;
    int firstCompartment = static_cast<int>(Compartments::First);
    int atTime = static_cast<int>(SingleConcentrations::AtTime);
    int atEndInterval = static_cast<int>(SingleConcentrations::AtEndInterval);

    int forcesize = 0;

    if (_atTime < m_Tinf) {
        forcesize = 1;
    }

    // Calculate concentrations
    compute(_inResiduals, forcesize, concentrations);

    // return concentrations of first compartment 
    // (computation with atTime (current time))
    _concentrations[firstCompartment].push_back(concentrations[atTime]);
    // Only one compartment is existed.
    TMP_UNUSED_PARAMETER(_isAll);

    // interval=0 means that it is the last cycle, so final residual = 0
    if (m_Int == 0) {
        concentrations[atEndInterval] = 0;
    }

    // Return final residual (computation with m_Int (interval))
    _outResiduals[firstCompartment] = concentrations[atEndInterval];
    
    return checkValue(_outResiduals[firstCompartment] >= 0, "The concentration is negative.");
}

OneCompartmentInfusionMacro::OneCompartmentInfusionMacro() : OneCompartmentInfusionMicro()
{
}

bool OneCompartmentInfusionMacro::checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkValue(_parameters.size() >= 2, "The number of parameters should be equal to 2.")) {
        return false;
    }

    m_D = _intakeEvent.getDose() * 1000;
    double cl = _parameters.getValue(ParameterId::CL);
    m_V = _parameters.getValue(ParameterId::V);
    m_Ke = cl / m_V;
    m_Tinf = (_intakeEvent.getInfusionTime()).toHours();
    m_Int = (_intakeEvent.getInterval()).toHours();
    m_NbPoints = _intakeEvent.getNbPoints();

#ifdef DEBUG
    Tucuxi::Common::LoggerHelper logHelper;

    logHelper.debug("<<Input Values>>");
    logHelper.debug("m_D: {}", m_D);
    logHelper.debug("m_V: {}", m_V);
    logHelper.debug("cl: {}", cl);
    logHelper.debug("m_Ke: {}", m_Ke);
    logHelper.debug("m_Tinf: {}", m_Tinf);
    logHelper.debug("m_Int: {}", m_Int);
    logHelper.debug("m_NbPoints: {}", m_NbPoints);
#endif

    bool bOK = checkValue(m_D >= 0, "The dose is negative.");
    bOK &= checkValue(!std::isnan(m_D), "The dose is NaN.");
    bOK &= checkValue(!std::isinf(m_D), "The dose is Inf.");
    bOK &= checkValue(m_V > 0, "The volume is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_V), "The volume is NaN.");
    bOK &= checkValue(!std::isinf(m_V), "The volume is Inf.");
    bOK &= checkValue(cl > 0, "The clearance is not greater than zero.");
    bOK &= checkValue(!std::isnan(cl), "The clearance is NaN.");
    bOK &= checkValue(!std::isinf(cl), "The clearance is Inf.");
    bOK &= checkValue(m_Tinf >= 0, "The infusion time is zero or negative.");
    bOK &= checkValue(m_Int > 0, "The interval time is negative.");

    return bOK;
}


}
}
