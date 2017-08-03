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

bool TwoCompartmentIntra::checkInputs(const IntakeEvent& _intakeEvent, const ParameterList& _parameters)
{
/*
    PRECOND(parameters.size() >= 2, SHOULDNTGONEGATIVE, "The number of parameters should be equal to 2.")
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
    m_Root = std::sqrt((m_SumK * m_SumK) - (4 * m_K21 * m_Ke));
    m_Divider = m_Root * (-m_SumK + m_Root) * (m_SumK + m_Root);
    m_Alpha = (m_SumK + m_Root)/2;
    m_Beta = (m_SumK - m_Root)/2;
    m_Tinf = (_intakeEvent.getInfusionTime()).toMilliseconds();
    m_Int = (_intakeEvent.getInterval()).toMilliseconds();
    m_DeltaD = (m_D / m_V1) / m_Tinf; 
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


void TwoCompartmentIntra::prepareComputations(const IntakeEvent& _intakeEvent, const ParameterList& _parameters)
{
}


void TwoCompartmentIntra::computeLogarithms(const IntakeEvent& _intakeEvent, const ParameterList& _parameters, Eigen::VectorXd& _times)
{
    m_precomputedLogarithms["Ke"] = (-m_Ke * _times).array().exp();
    m_precomputedLogarithms["Alpha"] = (-m_Alpha * _times).array().exp();
    m_precomputedLogarithms["Beta"] = (-m_Beta * _times).array().exp();
    m_precomputedLogarithms["AlphaInf"] = (m_Alpha * _times).array().exp();
    m_precomputedLogarithms["BetaInf"] = (m_Beta * _times).array().exp();
    m_precomputedLogarithms["BetaInf2"] = (-2 * m_Beta * _times).array().exp();
    m_precomputedLogarithms["Root"] = (m_Root * _times).array().exp();
}


void TwoCompartmentIntra::computeConcentrations(const Residuals& _inResiduals, Concentrations& _concentrations, Residuals& _outResiduals)
{
    size_t ke_size = m_precomputedLogarithms["Ke"].size();
    int forcesize = std::min(ceil(m_Tinf/m_Int * ke_size), ceil(ke_size));
    int therest;
    Eigen::VectorXd alphaLogV = m_precomputedLogarithms["Alpha"]; 
    Eigen::VectorXd betaLogV = m_precomputedLogarithms["Beta"]; 
    Eigen::VectorXd alphaInfLogV = m_precomputedLogarithms["AlphaInf"]; 
    Eigen::VectorXd betaInfLogV = m_precomputedLogarithms["BetaInf"]; 
    Eigen::VectorXd betaInf2LogV = m_precomputedLogarithms["BetaInf2"]; 
    Eigen::VectorXd rootLogV = m_precomputedLogarithms["Root"]; 

    Concentration resid1 = _inResiduals[0];
    Concentration resid2 = _inResiduals[1];

    Concentration residInf1 =
	(2 * m_DeltaD * std::exp(m_Beta * m_Tinf) * m_K21
	* (std::exp(-m_Beta * m_Tinf) * (-m_K12 - m_K21 + m_Ke - m_Root) 
	+ std::exp(-2 * m_Beta * m_Tinf) * (m_K12 + m_K21 - m_Ke + m_Root)
	+ std::exp(m_Root*m_Tinf - m_Alpha*m_Tinf) * (m_K12 + m_K21 - m_Ke - m_Root)
	+ std::exp(-m_Alpha*m_Tinf - m_Beta*m_Tinf) * (-m_K12 - m_K21 + m_Ke + m_Root))
	) / m_Divider;
    Concentration residInf2 = 
	(2 * m_DeltaD * std::exp(m_Beta * m_Tinf) * m_K12
	* (std::exp(-m_Beta * m_Tinf) * (-m_SumK - m_Root)
	+ std::exp(-2 * m_Beta * m_Tinf) * (m_SumK + m_Root)
	+ std::exp(m_Root * m_Tinf - m_Alpha * m_Tinf) * (m_SumK - m_Root)
	+ std::exp(-m_Alpha * m_Tinf - m_Beta * m_Tinf) * (-m_SumK + m_Root))
        ) / m_Divider;

    Concentration A = ((m_K12 - m_K21 + m_Ke + m_Root) * resid1) - (2 * m_K21 * resid2);
    Concentration B = ((-m_K12 + m_K21 - m_Ke + m_Root) * resid1) + (2 * m_K21 * resid2);
    Concentration A2 = (-2 * m_K12 * resid1) + ((-m_K12 + m_K21 - m_Ke + m_Root) * resid2);
    Concentration BB2 = (2 * m_K12 * resid1) + ((m_K12 - m_K21 + m_Ke + m_Root) * resid2);
    Concentration AInf = -m_K12 - m_K21 + m_Ke - m_Root;
    Concentration BInf = m_K12 + m_K21 - m_Ke - m_Root;
    Concentration BPostInf = (-m_K12 + m_K21 - m_Ke + m_Root)*residInf1 + 2*m_K21*residInf2;
    Concentration APostInf = (m_K12 - m_K21 + m_Ke + m_Root)*residInf1 - 2*m_K21*residInf2;

    // Calculate concentrations for comp1 and comp2
    Eigen::VectorXd concentrations1 = 
	((A * alphaLogV) + (B * betaLogV)) / (2 * m_Root);
    Eigen::VectorXd concentrations2 = 
	((A2 * alphaLogV) + (BB2 * betaLogV)) / (2 * m_Root);

    // During infusion
    Eigen::ArrayXd p1p1 = (2 * m_DeltaD * m_K21 * betaInfLogV.head(forcesize)).array();
    Eigen::ArrayXd p1p2 = AInf * (betaLogV.head(forcesize) - betaInf2LogV.head(forcesize));
    Eigen::ArrayXd p1p3 = 
	(BInf 
	* (rootLogV.head(forcesize).cwiseQuotient(alphaInfLogV.head(forcesize)) 
	- alphaLogV.head(forcesize).cwiseQuotient(betaInfLogV.head(forcesize)))).array();
    Eigen::ArrayXd p2p1 = (2 * m_DeltaD * m_K12 * betaInfLogV.head(forcesize)).array();
    Eigen::ArrayXd p2p2 = 
	betaLogV.head(forcesize) 
	* (-m_SumK - m_Root) 
	+ betaInf2LogV.head(forcesize) * (m_SumK + m_Root)
        + rootLogV.head(forcesize).cwiseQuotient(alphaInfLogV.head(forcesize)) * (m_SumK - m_Root) 
	+ alphaLogV.head(forcesize).cwiseQuotient(betaInfLogV.head(forcesize)) * (-m_SumK + m_Root);

    concentrations1.head(forcesize) = 
	concentrations1.head(forcesize) + ((p1p1 * (p1p2 + p1p3)) / m_Divider).matrix();
    concentrations2.head(forcesize) = 
	concentrations2.head(forcesize) + ((p2p1 * p2p2) / m_Divider).matrix();

    // After infusion
    therest = concentrations1.size() - forcesize;
    concentrations1.tail(therest) = 
	concentrations1.tail(therest) 
	+ (APostInf * alphaLogV.head(therest) + BPostInf * betaLogV.head(therest)) / (2 * m_Root);

    BB2 = 2 * m_K12 * residInf1 + (m_K12 - m_K21 + m_Ke + m_Root)*residInf2;
    A2 = -2 * m_K12 * residInf1 + (-m_K12 + m_K21 - m_Ke + m_Root)*residInf2;
    concentrations2.tail(therest) += 
	(A2 * alphaLogV.head(therest) + BB2 * betaLogV.head(therest)) / (2 * m_Root);

    // return concentrations of comp1 and comp2
    _outResiduals.push_back(concentrations1[concentrations1.size() - 1]);
    //POSTCONDCONT(concentrations1[concentrations.size() - 1] >= 0, SHOULDNTGONEGATIVE, "The concentration is negative.")
    _outResiduals.push_back(concentrations2[concentrations2.size() - 1]);
    //POSTCONDCONT(concentrations2[concentrations.size() - 1] >= 0, SHOULDNTGONEGATIVE, "The concentration is negative.")
    _concentrations.assign(concentrations1.data(), concentrations1.data() + concentrations1.size());	
}

}
}
