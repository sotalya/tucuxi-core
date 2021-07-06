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

std::vector<std::string> OneCompartmentInfusionMicro::getParametersId()
{
    return {"Ke", "V"};
}


bool OneCompartmentInfusionMicro::checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkCondition(_parameters.size() >= 2, "The number of parameters should be equal to 2.")) {
        return false;
    }

    m_D = _intakeEvent.getDose() * 1000;
    m_Ke = _parameters.getValue(ParameterId::Ke);
    m_V = _parameters.getValue(ParameterId::V);
    m_Tinf = (_intakeEvent.getInfusionTime()).toHours();
    m_Int = (_intakeEvent.getInterval()).toHours();
    m_nbPoints = static_cast<Eigen::Index>(_intakeEvent.getNbPoints());

#ifdef DEBUG
    Tucuxi::Common::LoggerHelper logHelper;

    logHelper.debug("<<Input Values>>");
    logHelper.debug("m_D: {}", m_D);
    logHelper.debug("m_V: {}", m_V);
    logHelper.debug("m_Ke: {}", m_Ke);
    logHelper.debug("m_Tinf: {}", m_Tinf);
    logHelper.debug("m_Int: {}", m_Int);
    logHelper.debug("m_nbPoints: {}", m_nbPoints);
#endif

    bool bOK = checkPositiveValue(m_D, "The dose");
    bOK &= checkStrictlyPositiveValue(m_V, "The volume");
    bOK &= checkStrictlyPositiveValue(m_Ke, "The clearance");
    bOK &= checkCondition(m_Tinf >= 0, "The infusion time is negative.");
    bOK &= checkCondition(m_nbPoints >= 0, "The number of points is zero or negative.");
    bOK &= checkCondition(m_Int > 0, "The interval time is negative.");

    return bOK;
}


void OneCompartmentInfusionMicro::computeExponentials(Eigen::VectorXd& _times)
{
    setExponentials(Exponentials::Ke, (-m_Ke * _times).array().exp());
}


bool OneCompartmentInfusionMicro::computeConcentrations(const Residuals& _inResiduals, bool _isAll, std::vector<Concentrations>& _concentrations, Residuals& _outResiduals)
{
    Eigen::VectorXd concentrations;
    size_t firstCompartment = static_cast<size_t>(Compartments::First);
    int forcesize;
    if (m_nbPoints == 2) {
        forcesize = static_cast<int>(std::min(ceil(m_Tinf/m_Int * static_cast<double>(m_nbPoints)), ceil(static_cast<double>(m_nbPoints))));
    }
    else {
        forcesize = std::min(static_cast<int>(m_nbPoints), std::max(2, static_cast<int>((m_Tinf / m_Int) * static_cast<double>(m_nbPoints))));
    }

    // Calculate concentrations
    compute(_inResiduals, forcesize, concentrations);

    // Return final Residual
    _outResiduals[firstCompartment] = concentrations[m_nbPoints - 1];

    // Return concentraions of first compartment
    _concentrations[firstCompartment].assign(concentrations.data(), concentrations.data() + concentrations.size());	
    // Only one compartment is existed.
    TMP_UNUSED_PARAMETER(_isAll);

    return checkCondition(_outResiduals[firstCompartment] >= 0, "The concentration is negative.");
}

bool OneCompartmentInfusionMicro::computeConcentration(const Value& _atTime, const Residuals& _inResiduals, bool _isAll, std::vector<Concentrations>& _concentrations, Residuals& _outResiduals)
{
    Eigen::VectorXd concentrations;
    size_t firstCompartment = static_cast<size_t>(Compartments::First);
    Eigen::Index atTime = static_cast<Eigen::Index>(SingleConcentrations::AtTime);
    Eigen::Index atEndInterval = static_cast<Eigen::Index>(SingleConcentrations::AtEndInterval);

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
    if (m_Int == 0.0) {
        concentrations[atEndInterval] = 0;
    }

    // Return final residual (computation with m_Int (interval))
    _outResiduals[firstCompartment] = concentrations[atEndInterval];
    
    return checkCondition(_outResiduals[firstCompartment] >= 0, "The concentration is negative.");
}

OneCompartmentInfusionMacro::OneCompartmentInfusionMacro() : OneCompartmentInfusionMicro()
{
}

std::vector<std::string> OneCompartmentInfusionMacro::getParametersId()
{
    return {"CL", "V"};
}

bool OneCompartmentInfusionMacro::checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkCondition(_parameters.size() >= 2, "The number of parameters should be equal to 2.")) {
        return false;
    }

    m_D = _intakeEvent.getDose() * 1000;
    double cl = _parameters.getValue(ParameterId::CL);
    m_V = _parameters.getValue(ParameterId::V);
    m_Ke = cl / m_V;
    m_Tinf = (_intakeEvent.getInfusionTime()).toHours();
    m_Int = (_intakeEvent.getInterval()).toHours();
    m_nbPoints = static_cast<Eigen::Index>(_intakeEvent.getNbPoints());

#ifdef DEBUG
    Tucuxi::Common::LoggerHelper logHelper;

    logHelper.debug("<<Input Values>>");
    logHelper.debug("m_D: {}", m_D);
    logHelper.debug("m_V: {}", m_V);
    logHelper.debug("cl: {}", cl);
    logHelper.debug("m_Ke: {}", m_Ke);
    logHelper.debug("m_Tinf: {}", m_Tinf);
    logHelper.debug("m_Int: {}", m_Int);
    logHelper.debug("m_nbPoints: {}", m_nbPoints);
#endif

    bool bOK = checkPositiveValue(m_D, "The dose");
    bOK &= checkStrictlyPositiveValue(m_V, "The volume");
    bOK &= checkStrictlyPositiveValue(cl, "The clearance");
    bOK &= checkCondition(m_Tinf >= 0, "The infusion time is negative.");
    bOK &= checkCondition(m_nbPoints >= 0, "The number of points is zero or negative.");
    bOK &= checkCondition(m_Int > 0, "The interval time is negative.");


    return bOK;
}


}
}
