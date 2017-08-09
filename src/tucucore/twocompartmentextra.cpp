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

bool TwoCompartmentExtra::checkInputs(const IntakeEvent& _intakeEvent, const ParameterList& _parameters)
{
/*
    PRECOND(parameters.size() >= 4, SHOULDNTGONEGATIVE, "The number of parameters should be equal to 2.")
*/
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
/*
    PRECONDCONT(m_D >= 0, SHOULDNTGONEGATIVE, "The dose is negative.")
    PRECONDCONT(!qIsNaN(m_D), NOTANUMBER, "The dose is NaN.")
    PRECONDCONT(!qIsInf(m_D), ISINFINITY, "The dose is Inf.")
    PRECONDCONT(m_Cl > 0, SHOULDNTGONEGATIVE, "The clearance is not greater than zero.")
    PRECONDCONT(!qIsNaN(m_Cl), NOTANUMBER, "The CL is NaN.")
    PRECONDCONT(!qIsInf(m_Cl), ISINFINITY, "The CL is Inf.")
    PRECONDCONT(m_V1 > 0, SHOULDNTGONEGATIVE, "The volume1 is not greater than zero.")
    PRECONDCONT(!qIsNaN(m_V1), NOTANUMBER, "The V1 is NaN.")
    PRECONDCONT(!qIsInf(m_V1), ISINFINITY, "The V1 is Inf.")
    PRECONDCONT(m_V2 > 0, SHOULDNTGONEGATIVE, "The volume2 is not greater than zero.")
    PRECONDCONT(!qIsNaN(m_V2), NOTANUMBER, "The V2 is NaN.")
    PRECONDCONT(!qIsInf(m_V2), ISINFINITY, "The V2 is Inf.")
    PRECONDCONT(m_Ka > 0, SHOULDNTGONEGATIVE, "The Ka is not greater than zero.")
    PRECONDCONT(!qIsNaN(m_Ka), NOTANUMBER, "The Ka is NaN.")
    PRECONDCONT(!qIsInf(m_Ka), ISINFINITY, "The Ka is Inf.")
    PRECONDCONT(m_F > 0, SHOULDNTGONEGATIVE, "The F is not greater than zero.")
    PRECONDCONT(!qIsNaN(m_F), NOTANUMBER, "The F is NaN.")
    PRECONDCONT(!qIsInf(m_F), ISINFINITY, "The F is Inf.")
*/
    return true;
}


void TwoCompartmentExtra::prepareComputations(const IntakeEvent& _intakeEvent, const ParameterList& _parameters)
{
}


void TwoCompartmentExtra::computeLogarithms(const IntakeEvent& _intakeEvent, const ParameterList& _parameters, Eigen::VectorXd& _times)
{
    m_precomputedLogarithms["Ka"] = (-m_Ka * _times).array().exp();
    m_precomputedLogarithms["Alpha"] = (-m_Alpha * _times).array().exp();
    m_precomputedLogarithms["Beta"] = (-m_Beta * _times).array().exp();
}


bool TwoCompartmentExtra::computeConcentrations(const Residuals& _inResiduals, Concentrations& _concentrations, Residuals& _outResiduals)
{
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

    //PRECONDCONT(divider != 0.0, DIVIDEBYZERO, "Dividing by zero.");


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
    
    //PRECONDCONT(divider != 0.0, DIVIDEBYZERO, "Dividing by zero.");

    // Calculate concentrations of compartment 2
    Value concentrations2 = 
        2 * (B * m_precomputedLogarithms["Beta"](m_precomputedLogarithms["Beta"].size() - 1) 
        + A * m_precomputedLogarithms["Alpha"](m_precomputedLogarithms["Alpha"].size() - 1) 
        + C * m_precomputedLogarithms["Ka"](m_precomputedLogarithms["Ka"].size() - 1)) / divider;

    // Return concentrations of comp1, comp2 and comp3
    _outResiduals.push_back(concentrations1[m_NbPoints - 1]);
    _outResiduals.push_back(concentrations2);
    _outResiduals.push_back(m_precomputedLogarithms["Ka"](m_precomputedLogarithms["Ka"].size() - 1) * resid3);
    //POSTCONDCONT(concentrations1[concentrations.size() - 1] >= 0, SHOULDNTGONEGATIVE, "The concentration is negative.")
    //POSTCONDCONT(concentrations2 >= 0, SHOULDNTGONEGATIVE, "The concentration is negative.")
    //POSTCONDCONT( >= 0, SHOULDNTGONEGATIVE, "The concentration is negative.")

    _concentrations.assign(concentrations1.data(), concentrations1.data() + concentrations1.size());	

    return true;
}

}
}

