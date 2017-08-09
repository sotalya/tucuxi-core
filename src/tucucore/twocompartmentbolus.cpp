/*
* Copyright (C) 2017 Tucuxi SA
*/

#include <Eigen/Dense>

#include "tucucore/twocompartmentbolus.h"

namespace Tucuxi {
namespace Core {

TwoCompartmentBolus::TwoCompartmentBolus()
{
}

bool TwoCompartmentBolus::checkInputs(const IntakeEvent& _intakeEvent, const Parameters& _parameters)
{
    bool bOK = true;

    if(!checkValue(_parameters.size() >= 4, "The number of parameters should be equal to 4."))
	    return false;
    
    m_D = _intakeEvent.getDose() * 1000;
    m_Cl = _parameters[0].getValue();
    m_Q = _parameters[1].getValue();
    m_V1 = _parameters[2].getValue();
    m_V2 = _parameters[3].getValue();
    m_Ke = m_Cl / m_V1;
    m_K12 = m_Q / m_V1;
    m_K21 = m_Q / m_V2;
    m_NbPoints = _intakeEvent.getNumberPoints();

    bOK &= checkValue(m_D >= 0, "The dose is negative.");
    bOK &= checkValue(!std::isnan(m_D), "The dose is NaN.");
    bOK &= checkValue(!std::isinf(m_D), "The dose is Inf.");
    bOK &= checkValue(m_Cl > 0, "The clearance is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_Cl), "The CL is NaN.");
    bOK &= checkValue(!std::isinf(m_Cl), "The CL is Inf.");
    bOK &= checkValue(m_Q > 0, "The Q is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_Q), "The Q is NaN.");
    bOK &= checkValue(!std::isinf(m_Q), "The Q is Inf.");
    bOK &= checkValue(m_V1 > 0, "The volume1 is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_V1), "The V1 is NaN.");
    bOK &= checkValue(!std::isinf(m_V1), "The V1 is Inf.");
    bOK &= checkValue(m_V2 > 0, "The volume2 is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_V2), "The V2 is NaN.");
    bOK &= checkValue(!std::isinf(m_V2), "The V2 is Inf.");

    Value sumK = m_Ke + m_K12 + m_K21;
    m_RootK = std::sqrt((sumK * sumK) - (4 * m_K21 * m_Ke));
    m_Alpha = (sumK + m_RootK)/2;
    m_Beta = (sumK - m_RootK)/2;

    return bOK;
}


void TwoCompartmentBolus::prepareComputations(const IntakeEvent& _intakeEvent, const Parameters& _parameters)
{
}


void TwoCompartmentBolus::computeLogarithms(const IntakeEvent& _intakeEvent, const Parameters& _parameters, Eigen::VectorXd& _times)
{
    m_precomputedLogarithms["Alpha"] = (-m_Alpha * _times).array().exp();
    m_precomputedLogarithms["Beta"] = (-m_Beta * _times).array().exp();
}


bool TwoCompartmentBolus::computeConcentrations(const Residuals& _inResiduals, Concentrations& _concentrations, Residuals& _outResiduals)
{
    bool bOK = true;

    Concentration resid1 = _inResiduals[0] + (m_D/m_V1);
    Concentration resid2 = _inResiduals[1];

    Value A = ((m_K12 - m_K21 + m_Ke + m_RootK) * resid1) - (2 * m_K21 * resid2);
    Value B = ((-m_K12 + m_K21 - m_Ke + m_RootK) * resid1) + (2 * m_K21 * resid2);
    Value A2 = (-2 * m_K12 * resid1) + ((-m_K12 + m_K21 - m_Ke + m_RootK) * resid2);
    Value BB2 = (2 * m_K12 * resid1) + ((m_K12 - m_K21 + m_Ke + m_RootK) * resid2);

    // Calculate concentrations for comp1 and comp2
    Eigen::VectorXd concentrations1 = 
        ((A * m_precomputedLogarithms["Alpha"]) + (B * m_precomputedLogarithms["Beta"])) / (2 * m_RootK);
    Eigen::VectorXd concentrations2 = 
        ((A2 * m_precomputedLogarithms["Alpha"]) + (BB2 * m_precomputedLogarithms["Beta"])) / (2 * m_RootK);

    // return concentrations of comp1 and comp2
    _outResiduals.push_back(concentrations1[m_NbPoints - 1]);
    _outResiduals.push_back(concentrations2[m_NbPoints - 1]);

    _concentrations.assign(concentrations1.data(), concentrations1.data() + concentrations1.size());	

    bOK &= checkValue(_outResiduals[0] >= 0, "The concentration1 is negative.");
    bOK &= checkValue(_outResiduals[1] >= 0, "The concentration2 is negative.");

    return bOK;
}

}
}

