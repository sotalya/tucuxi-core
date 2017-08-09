/*
* Copyright (C) 2017 Tucuxi SA
*/

#include <Eigen/Dense>

#include "tucucore/twocompartmentintra.h"

namespace Tucuxi {
namespace Core {

TwoCompartmentIntra::TwoCompartmentIntra()
{
}

bool TwoCompartmentIntra::checkInputs(const IntakeEvent& _intakeEvent, const Parameters& _parameters)
{
/*
    PRECOND(parameters.size() >= 4, SHOULDNTGONEGATIVE, "The number of parameters should be equal to 2.")
*/
    m_D = _intakeEvent.getDose() * 1000;
    m_Cl = _parameters[0].getValue();
    m_Q = _parameters[1].getValue();
    m_V1 = _parameters[2].getValue();
    m_V2 = _parameters[3].getValue();
    m_Ke = m_Cl / m_V1;
    m_K12 = m_Q / m_V1;
    m_K21 = m_Q / m_V2;
    m_SumK = m_Ke + m_K12 + m_K21;
    m_RootK = std::sqrt((m_SumK * m_SumK) - (4 * m_K21 * m_Ke));
    m_Divider = m_RootK * (-m_SumK + m_RootK) * (m_SumK + m_RootK);
    m_Alpha = (m_SumK + m_RootK)/2;
    m_Beta = (m_SumK - m_RootK)/2;
    m_Tinf = (_intakeEvent.getInfusionTime()).toMilliseconds();
    m_Int = (_intakeEvent.getInterval()).toMilliseconds();
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
    PRECONDCONT(m_Divider != 0.0, DIVIDEBYZERO, "Divide by zero.");
*/

    return true;
}


void TwoCompartmentIntra::prepareComputations(const IntakeEvent& _intakeEvent, const Parameters& _parameters)
{
}


void TwoCompartmentIntra::computeLogarithms(const IntakeEvent& _intakeEvent, const Parameters& _parameters, Eigen::VectorXd& _times)
{
    m_precomputedLogarithms["Alpha"] = (-m_Alpha * _times).array().exp();
    m_precomputedLogarithms["Beta"] = (-m_Beta * _times).array().exp();
    m_precomputedLogarithms["AlphaInf"] = (m_Alpha * _times).array().exp();
    m_precomputedLogarithms["BetaInf"] = (m_Beta * _times).array().exp();
    m_precomputedLogarithms["BetaInf2"] = (-2 * m_Beta * _times).array().exp();
    m_precomputedLogarithms["Root"] = (m_RootK * _times).array().exp();
}


bool TwoCompartmentIntra::computeConcentrations(const Residuals& _inResiduals, Concentrations& _concentrations, Residuals& _outResiduals)
{
    int forcesize = std::max(0.0, std::min(ceil(m_Tinf/m_Int * m_NbPoints), ceil(m_NbPoints)));
    int therest;
    Eigen::VectorXd& alphaLogV = m_precomputedLogarithms["Alpha"]; 
    Eigen::VectorXd& betaLogV = m_precomputedLogarithms["Beta"]; 
    Eigen::VectorXd& alphaInfLogV = m_precomputedLogarithms["AlphaInf"]; 
    Eigen::VectorXd& betaInfLogV = m_precomputedLogarithms["BetaInf"]; 
    Eigen::VectorXd& betaInf2LogV = m_precomputedLogarithms["BetaInf2"]; 
    Eigen::VectorXd& rootLogV = m_precomputedLogarithms["Root"]; 

    Concentration resid1 = _inResiduals[0];
    Concentration resid2 = _inResiduals[1];

    Value deltaD = (m_D / m_V1) / m_Tinf; 

    Concentration residInf1 =
        (2 * deltaD * std::exp(m_Beta * m_Tinf) * m_K21
        * (std::exp(-m_Beta * m_Tinf) * (-m_K12 - m_K21 + m_Ke - m_RootK) 
            + std::exp(-2 * m_Beta * m_Tinf) * (m_K12 + m_K21 - m_Ke + m_RootK)
            + std::exp(m_RootK*m_Tinf - m_Alpha*m_Tinf) * (m_K12 + m_K21 - m_Ke - m_RootK)
            + std::exp(-m_Alpha*m_Tinf - m_Beta*m_Tinf) * (-m_K12 - m_K21 + m_Ke + m_RootK))
	) / m_Divider;
    Concentration residInf2 = 
        (2 * deltaD * std::exp(m_Beta * m_Tinf) * m_K12
	* (std::exp(-m_Beta * m_Tinf) * (-m_SumK - m_RootK)
	    + std::exp(-2 * m_Beta * m_Tinf) * (m_SumK + m_RootK)
            + std::exp(m_RootK * m_Tinf - m_Alpha * m_Tinf) * (m_SumK - m_RootK)
            + std::exp(-m_Alpha * m_Tinf - m_Beta * m_Tinf) * (-m_SumK + m_RootK))
        ) / m_Divider;

    Value A = ((m_K12 - m_K21 + m_Ke + m_RootK) * resid1) - (2 * m_K21 * resid2);
    Value B = ((-m_K12 + m_K21 - m_Ke + m_RootK) * resid1) + (2 * m_K21 * resid2);
    Value A2 = (-2 * m_K12 * resid1) + ((-m_K12 + m_K21 - m_Ke + m_RootK) * resid2);
    Value BB2 = (2 * m_K12 * resid1) + ((m_K12 - m_K21 + m_Ke + m_RootK) * resid2);
    Value AInf = -m_K12 - m_K21 + m_Ke - m_RootK;
    Value BInf = m_K12 + m_K21 - m_Ke - m_RootK;
    Value BPostInf = (-m_K12 + m_K21 - m_Ke + m_RootK)*residInf1 + 2*m_K21*residInf2;
    Value APostInf = (m_K12 - m_K21 + m_Ke + m_RootK)*residInf1 - 2*m_K21*residInf2;

    // Calculate concentrations for comp1 and comp2
    Eigen::VectorXd concentrations1 = 
	((A * alphaLogV) + (B * betaLogV)) / (2 * m_RootK);
    Eigen::VectorXd concentrations2 = 
	((A2 * alphaLogV) + (BB2 * betaLogV)) / (2 * m_RootK);

    // During infusion
    Eigen::ArrayXd p1p1 = (2 * deltaD * m_K21 * betaInfLogV.head(forcesize)).array();
    Eigen::ArrayXd p1p2 = AInf * (betaLogV.head(forcesize) - betaInf2LogV.head(forcesize));
    Eigen::ArrayXd p1p3 = 
	(BInf 
	* (rootLogV.head(forcesize).cwiseQuotient(alphaInfLogV.head(forcesize)) 
	- alphaLogV.head(forcesize).cwiseQuotient(betaInfLogV.head(forcesize)))).array();
    Eigen::ArrayXd p2p1 = (2 * deltaD * m_K12 * betaInfLogV.head(forcesize)).array();
    Eigen::ArrayXd p2p2 = 
	betaLogV.head(forcesize) 
	* (-m_SumK - m_RootK) 
	+ betaInf2LogV.head(forcesize) * (m_SumK + m_RootK)
        + rootLogV.head(forcesize).cwiseQuotient(alphaInfLogV.head(forcesize)) * (m_SumK - m_RootK) 
	+ alphaLogV.head(forcesize).cwiseQuotient(betaInfLogV.head(forcesize)) * (-m_SumK + m_RootK);

    concentrations1.head(forcesize) = 
        concentrations1.head(forcesize) + ((p1p1 * (p1p2 + p1p3)) / m_Divider).matrix();
    concentrations2.head(forcesize) = 
        concentrations2.head(forcesize) + ((p2p1 * p2p2) / m_Divider).matrix();

    // After infusion
    therest = concentrations1.size() - forcesize;
    concentrations1.tail(therest) = 
        concentrations1.tail(therest) 
	+ (APostInf * alphaLogV.head(therest) + BPostInf * betaLogV.head(therest)) / (2 * m_RootK);

    BB2 = 2 * m_K12 * residInf1 + (m_K12 - m_K21 + m_Ke + m_RootK)*residInf2;
    A2 = -2 * m_K12 * residInf1 + (-m_K12 + m_K21 - m_Ke + m_RootK)*residInf2;
    concentrations2.tail(therest) += 
        (A2 * alphaLogV.head(therest) + BB2 * betaLogV.head(therest)) / (2 * m_RootK);

    // return concentrations of comp1 and comp2
    _outResiduals.push_back(concentrations1[m_NbPoints - 1]);
    _outResiduals.push_back(concentrations2[m_NbPoints - 1]);
    //POSTCONDCONT(concentrations1[concentrations.size() - 1] >= 0, SHOULDNTGONEGATIVE, "The concentration is negative.")
    //POSTCONDCONT(concentrations2[concentrations.size() - 1] >= 0, SHOULDNTGONEGATIVE, "The concentration is negative.")

    _concentrations.assign(concentrations1.data(), concentrations1.data() + concentrations1.size());	

    return true;
}

}
}

