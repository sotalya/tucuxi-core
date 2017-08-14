/*
* Copyright (C) 2017 Tucuxi SA
*/

#include <Eigen/Dense>

#include "tucucore/onecompartmentbolus.h"

namespace Tucuxi {
namespace Core {

OneCompartmentBolus::OneCompartmentBolus()
{
}

bool OneCompartmentBolus::checkInputs(const IntakeEvent& _intakeEvent, const ParameterList& _parameters)
{
    bool bOK = true;
    
    if(!checkValue(_parameters.size() >= 2, "The number of parameters should be equal to 2."))
	    return false;
    
    m_D = _intakeEvent.getDose() * 1000;
    m_V = _parameters[0].getValue();
    m_Ke = _parameters[1].getValue();
    m_NbPoints = _intakeEvent.getNumberPoints();

    // check the inputs
    bOK &= checkValue(m_D >= 0, "The dose is negative.");
    bOK &= checkValue(!std::isnan(m_D), "The dose is NaN.");
    bOK &= checkValue(!std::isinf(m_D), "The dose is Inf.");
    bOK &= checkValue(m_V > 0, "The volume is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_V), "The V is NaN.");
    bOK &= checkValue(!std::isinf(m_V), "The V is Inf.");
    bOK &= checkValue(m_Ke > 0, "The clearance is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_Ke), "The CL is NaN.");
    bOK &= checkValue(!std::isinf(m_Ke), "The CL is Inf.");
    bOK &= checkValue(m_NbPoints >= 0, "The number of points is zero or negative.");
    bOK &= checkValue((_intakeEvent.getInterval()).toMilliseconds() >= 0, "The interval time is zero or negative.");

    return bOK;
}


void OneCompartmentBolus::prepareComputations(const IntakeEvent& _intakeEvent, const ParameterList& _parameters)
{
}


void OneCompartmentBolus::computeLogarithms(const IntakeEvent& _intakeEvent, const ParameterList& _parameters, Eigen::VectorXd& _times)
{
    m_precomputedLogarithms["Ke"] = (-m_Ke * _times).array().exp();
}


bool OneCompartmentBolus::computeConcentrations(const Residuals& _inResiduals, Concentrations& _concentrations, Residuals& _outResiduals)
{
    Eigen::VectorXd concentrations = compute(_inResiduals);

    _outResiduals.push_back(concentrations[m_NbPoints - 1]);    
    _concentrations.assign(concentrations.data(), concentrations.data() + concentrations.size());	
    
    return checkValue(_outResiduals[0] >= 0, "The concentration is negative.");
}

bool OneCompartmentBolus::computeConcentration(const int64& _atTime, const Residuals& _inResiduals, Concentrations& _concentrations, Residuals& _outResiduals)
{
    Eigen::VectorXd concentrations = compute(_inResiduals);

    // return concentraions (computation with atTime (current time))
    _concentrations.push_back(concentrations[0]);

    // return final residual (computation with m_Int (interval))
    _outResiduals.push_back(concentrations[1]);
    
    return checkValue(_outResiduals[0] >= 0, "The concentration is negative.");
}

}
}
