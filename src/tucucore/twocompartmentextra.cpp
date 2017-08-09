/*
* Copyright (C) 2017 Tucuxi SA
*/

#include <Eigen/Dense>

#include "tucucore/twocompartmentextra.h"

namespace Tucuxi {
namespace Core {

TwoCompartmentExtra::TwoCompartmentExtra()
{
}

bool TwoCompartmentExtra::checkInputs(const IntakeEvent& _intakeEvent, const Parameters& _parameters)
{
    bool bOK = true;

    if(!checkValue(_parameters.size() >= 4, "The number of parameters should be equal to 4."))
	    return false;
    
    m_D = _intakeEvent.getDose() * 1000;
    m_Cl = _parameters[0].getValue();
    m_F = _parameters[1].getValue();
    m_Ka = _parameters[2].getValue();
    m_Q = _parameters[3].getValue();
    m_V1 = _parameters[4].getValue();
    m_V2 = _parameters[5].getValue();
    m_Ke = m_Cl / m_V1;
    m_K12 = m_Q / m_V1;
    m_K21 = m_Q / m_V2;
    Value sumK = m_Ke + m_K12 + m_K21;
    m_RootK = std::sqrt((sumK * sumK) - (4 * m_K21 * m_Ke));
    m_Alpha = (sumK + m_RootK)/2;
    m_Beta = (sumK - m_RootK)/2;
    m_NbPoints = _intakeEvent.getNumberPoints();

    bOK &= checkValue(m_D >= 0, "The dose is negative.");
    bOK &= checkValue(!std::isnan(m_D), "The dose is NaN.");
    bOK &= checkValue(!std::isinf(m_D), "The dose is Inf.");
    bOK &= checkValue(m_Cl > 0, "The clearance is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_Cl), "The CL is NaN.");
    bOK &= checkValue(!std::isinf(m_Cl), "The CL is Inf.");
    bOK &= checkValue(m_V1 > 0, "The volume1 is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_V1), "The V1 is NaN.");
    bOK &= checkValue(!std::isinf(m_V1), "The V1 is Inf.");
    bOK &= checkValue(m_V2 > 0, "The volume2 is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_V2), "The V2 is NaN.");
    bOK &= checkValue(!std::isinf(m_V2), "The V2 is Inf.");
    bOK &= checkValue(m_Ka > 0, "The Ka is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_Ka), "The Ka is NaN.");
    bOK &= checkValue(!std::isinf(m_Ka), "The Ka is Inf.");
    bOK &= checkValue(m_F > 0, "The F is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_F), "The F is NaN.");
    bOK &= checkValue(!std::isinf(m_F), "The F is Inf.");

    return true;
}


void TwoCompartmentExtra::prepareComputations(const IntakeEvent& _intakeEvent, const Parameters& _parameters)
{
}


void TwoCompartmentExtra::computeLogarithms(const IntakeEvent& _intakeEvent, const Parameters& _parameters, Eigen::VectorXd& _times)
{
    m_precomputedLogarithms["Ka"] = (-m_Ka * _times).array().exp();
    m_precomputedLogarithms["Alpha"] = (-m_Alpha * _times).array().exp();
    m_precomputedLogarithms["Beta"] = (-m_Beta * _times).array().exp();
}


bool TwoCompartmentExtra::computeConcentrations(const Residuals& _inResiduals, Concentrations& _concentrations, Residuals& _outResiduals)
{
    bool bOK = true;

    Value A, B, C, divider;
    Concentration resid1 = _inResiduals[0];
    Concentration resid2 = _inResiduals[1];
    Concentration resid3 = _inResiduals[2] + (m_F * m_D / m_V1);
    Value sumResid13 = resid1 + resid3;
    Value sumK12K21 = m_K12 + m_K21;
    Value sumK21Ke = m_K21 + m_Ke;
    Value diffK21Ka = m_K21 - m_Ka;
    Value diffK21Ke = m_K21 - m_Ke;
    Value powDiffK21Ke = std::pow(diffK21Ke, 2);

    // For compartment1, calculate A, B, C and divider
    A = 
        std::pow(m_K12, 3) * m_Ka * resid1 
        + diffK21Ka * (powDiffK21Ke * (-m_Ke * resid1 + m_Ka * sumResid13) 
            + ((m_Ka - m_Ke) * (m_Ke * resid1 - m_K21 * (resid1 + 2 * resid2)) 
            + m_Ka * (-m_K21 + m_Ke) * resid3) * m_RootK) 
        + std::pow(m_K12, 2) * (m_K21 * (-m_Ke * resid1 + m_Ka * (3 * resid1 + resid3))
            + m_Ka * (-m_Ka * sumResid13 + resid1 * (3 * m_Ke + m_RootK))) 
        + m_K12 * (std::pow(m_K21, 2) * (3 * m_Ka * resid1 - 2 * m_Ke * resid1 + 2 * m_Ka * resid3) 
            - m_K21 * (2 * std::pow(m_Ka, 2) * sumResid13 - 2 * m_Ka * m_Ke * sumResid13 
	        + m_Ka * (2 * resid2 + resid3) * m_RootK + m_Ke * resid1 * (2 * m_Ke + m_RootK)) 
            - m_Ka * (m_Ka * sumResid13 * (2 * m_Ke + m_RootK) 
	        - m_Ke * resid1 * (3 * m_Ke + 2 * m_RootK)));

    B = 
        std::pow(m_K12, 3) * m_Ka * resid1 
        + diffK21Ka * (powDiffK21Ke * (-m_Ke * resid1 + m_Ka * sumResid13) 
            + ((m_Ka - m_Ke) * (-m_Ke * resid1 + m_K21 * (resid1 + 2 * resid2)) 
            + m_Ka * diffK21Ke * resid3) * m_RootK) 
        + std::pow(m_K12, 2) * (m_K21 * (-m_Ke * resid1 + m_Ka * (3 * resid1 + resid3)) 
            - m_Ka * (m_Ka * sumResid13 + resid1 * (-3 * m_Ke + m_RootK))) 
        + m_K12 * (std::pow(m_K21, 2) * (3 * m_Ka * resid1 - 2 * m_Ke * resid1 + 2 * m_Ka * resid3) 
            + m_Ka * (m_Ke * resid1 * (3 * m_Ke - 2 * m_RootK) 
	        - m_Ka * sumResid13 * (2 * m_Ke - m_RootK)) 
            + m_K21 * (-2 * std::pow(m_Ka, 2) * sumResid13 + 2 * m_Ka * m_Ke * sumResid13 
	        + m_Ka * (2 * resid2 + resid3) * m_RootK + m_Ke * resid1 * (-2 * m_Ke + m_RootK)));

    C = 
	- 2 * diffK21Ka * m_Ka * (std::pow(m_K12, 2) + powDiffK21Ke + 2 * m_K12 * sumK21Ke) * resid3;

    divider = 
	std::pow((sumK12K21 - 2 * m_Ka + m_Ke) * m_RootK,  2) 
	- std::pow(std::pow(m_K12,2) + powDiffK21Ke + 2 * m_K12 * sumK21Ke, 2);

    if(!checkValue(divider != 0.0, "Dividing by zero."))
	    return false;

    // Calculate concentrations of compartment 1
    Eigen::VectorXd concentrations1 = 
	-2 * (B * m_precomputedLogarithms["Beta"] 
		+ A * m_precomputedLogarithms["Alpha"] + C * m_precomputedLogarithms["Ka"]) / divider;

    // For compartment1, calculate A, B, C and divider
    A = 
        -(std::pow(m_K12, 2) + powDiffK21Ke + 2 * m_K12 * sumK21Ke) 
        * (diffK21Ka * (m_Ka - m_Ke) * resid2 + m_K12 * m_Ka * (resid2 + resid3)) 
        + ((m_K12 * m_Ka + diffK21Ka * (m_Ka -m_Ke)) * (2 * m_K12 * resid1 + (m_K12 - diffK21Ke) * resid2) 
	    + m_K12 * m_Ka * (sumK12K21 - 2 * m_Ka + m_Ke) * resid3) * m_RootK;
    
    B = 
        -(std::pow(m_K12, 2) + powDiffK21Ke + 2 * m_K12 * sumK21Ke) 
        * (diffK21Ka * (m_Ka - m_Ke) * resid2 + m_K12 * m_Ka * (resid2 + resid3)) 
        - ((m_K12 * m_Ka + diffK21Ka * (m_Ka - m_Ke)) * (2 * m_K12 * resid1 + (m_K12 - diffK21Ke) * resid2) 
            + m_K12 * m_Ka * (sumK12K21 - 2 * m_Ka + m_Ke) * resid3) * m_RootK;
    
    C = 
        2 * m_K12 * m_Ka * (std::pow(m_K12, 2) + powDiffK21Ke + 2 * m_K12 * sumK21Ke) *resid3;
    
    divider = 
        -std::pow(std::pow(m_K12, 2) + powDiffK21Ke + 2 * m_K12 * sumK21Ke, 2) 
        + std::pow(sumK12K21 - 2 * m_Ka + m_Ke, 2) * std::pow(m_RootK, 2);
    
    if(!checkValue(divider != 0.0, "Dividing by zero."))
	    return false;

    // Calculate concentrations of compartment 2 and 3
    Value concentrations2 = 
        2 * (B * m_precomputedLogarithms["Beta"](m_precomputedLogarithms["Beta"].size() - 1) 
        + A * m_precomputedLogarithms["Alpha"](m_precomputedLogarithms["Alpha"].size() - 1) 
        + C * m_precomputedLogarithms["Ka"](m_precomputedLogarithms["Ka"].size() - 1)) / divider;
    Value concentrations3 = 
	m_precomputedLogarithms["Ka"](m_precomputedLogarithms["Ka"].size() - 1) * resid3;

    // Return concentrations of comp1, comp2 and comp3
    _outResiduals.push_back(concentrations1[m_NbPoints - 1]);
    _outResiduals.push_back(concentrations2);
    _outResiduals.push_back(concentrations3);

    _concentrations.assign(concentrations1.data(), concentrations1.data() + concentrations1.size());	

    bOK &= checkValue(_outResiduals[0] >= 0, "The concentration is negative.");
    bOK &= checkValue(_outResiduals[1] >= 0, "The concentration is negative.");
    bOK &= checkValue(_outResiduals[2] >= 0, "The concentration is negative.");

    return bOK;
}

}
}

