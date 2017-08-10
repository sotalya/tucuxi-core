/*
* Copyright (C) 2017 Tucuxi SA
*/

#include <Eigen/Dense>
#include <math.h>
#include <algorithm>

#include "tucucore/onecompartmentintra.h"

namespace Tucuxi {
namespace Core {

OneCompartmentIntra::OneCompartmentIntra()
{
}

bool OneCompartmentIntra::checkInputs(const IntakeEvent& _intakeEvent, const ParameterList& _parameters)
{
    bool bOK = true;

    if(!checkValue(_parameters.size() >= 4, "The number of parameters should be equal to 4."))
	    return false;
    
    m_D = _intakeEvent.getDose() * 1000;
    m_Cl = _parameters[0].getValue();
    m_V = _parameters[1].getValue();
    m_Ke = m_Cl / m_V;
    m_Tinf = (_intakeEvent.getInfusionTime()).toMilliseconds();
    m_Int = (_intakeEvent.getInterval()).toMilliseconds();
    m_NbPoints = _intakeEvent.getNumberPoints();

    bOK &= checkValue(m_D >= 0, "The dose is negative.");
    bOK &= checkValue(!std::isnan(m_D), "The dose is NaN.");
    bOK &= checkValue(!std::isinf(m_D), "The dose is Inf.");
    bOK &= checkValue(m_V > 0, "The volume is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_V), "The volume is NaN.");
    bOK &= checkValue(!std::isinf(m_V), "The volume is Inf.");
    bOK &= checkValue(m_Ke > 0, "The clearance is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_Ke), "The CL is NaN.");
    bOK &= checkValue(!std::isinf(m_Ke), "The CL is Inf.");
    bOK &= checkValue(m_Tinf >= 0, "The infusion time is zero or negative.");
    bOK &= checkValue(m_Int >= 0, "The interval time is zero or negative.");

    return bOK;
}


void OneCompartmentIntra::prepareComputations(const IntakeEvent& _intakeEvent, const ParameterList& _parameters)
{
}


void OneCompartmentIntra::computeLogarithms(const IntakeEvent& _intakeEvent, const ParameterList& _parameters, Eigen::VectorXd& _times)
{
    m_precomputedLogarithms["Ke"] = (-m_Ke * _times).array().exp();
}


bool OneCompartmentIntra::computeConcentrations(const Residuals& _inResiduals, Concentrations& _concentrations, Residuals& _outResiduals)
{
    Concentration part1 = m_D/(m_Tinf*m_Cl);
    size_t ke_size = m_precomputedLogarithms["Ke"].size();
    int forcesize = static_cast<int>(std::min(ceil(m_Tinf/m_Int * ke_size), ceil(ke_size)));
    int therest;

    // Calcaulate concentrations
    Eigen::VectorXd concentrations = Eigen::VectorXd::Constant(ke_size, _inResiduals[0]);
    concentrations = concentrations.cwiseProduct(m_precomputedLogarithms["Ke"]);

    concentrations.head(forcesize) = 
        concentrations.head(forcesize) 
	+ part1 * (1.0 - m_precomputedLogarithms["Ke"].head(forcesize).array()).matrix();
    
    therest = static_cast<int>(concentrations.size() - forcesize);
    concentrations.tail(therest) = 
        concentrations.tail(therest) 
	+ part1 * (exp(m_Ke * m_Tinf) - 1) * m_precomputedLogarithms["Ke"].tail(therest);

    // Set the new residual
    _outResiduals.push_back(concentrations[m_NbPoints - 1]);
    _concentrations.assign(concentrations.data(), concentrations.data() + concentrations.size());	

    return checkValue(_outResiduals[0] >= 0, "The concentration is negative.");
}

bool OneCompartmentIntra::computeConcentration(const int64& _atTime, const Residuals& _inResiduals, Concentrations& _concentrations, Residuals& _outResiduals)
{
    Concentration part1 = m_D/(m_Tinf*m_Cl);
    size_t ke_size = m_precomputedLogarithms["Ke"].size();

    // Calcaulate concentrations
    Eigen::VectorXd concentrations = Eigen::VectorXd::Constant(ke_size, _inResiduals[0]);
    concentrations = concentrations.cwiseProduct(m_precomputedLogarithms["Ke"]);

    int therest = 2;

    if(_atTime < m_Tinf)
    {
	concentrations.head(1) = 
	    concentrations.head(1) + part1 * (1.0 - m_precomputedLogarithms["Ke"].head(1).array()).matrix();
	therest = 1;
    }
    concentrations.tail(therest) = 
        concentrations.tail(therest) 
	+ part1 * (exp(m_Ke * m_Tinf) - 1) * m_precomputedLogarithms["Ke"].tail(therest);

    // return concentraions (computation with atTime (current time))
    _concentrations.push_back(concentrations[0]);

    // return final residual (computation with m_Int (interval))
    _outResiduals.push_back(concentrations[1]);
    
    return checkValue(_outResiduals[0] >= 0, "The concentration is negative.");
}

}
}
