/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_MATH_TWOCOMPARTMENTINFUSION_H
#define TUCUXI_MATH_TWOCOMPARTMENTINFUSION_H

#include "tucucore/intakeintervalcalculator.h"

namespace Tucuxi {
namespace Core {

/// \ingroup TucuCore
/// \brief Intake interval calculator for the two compartment infusion algorithm
/// \sa IntakeIntervalCalculator
class TwoCompartmentInfusion : public IntakeIntervalCalculator
{
public:
    /// \brief Constructor
    TwoCompartmentInfusion();

protected:
    virtual bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterList& _parameters) override;
    virtual void prepareComputations(const IntakeEvent& _intakeEvent, const ParameterList& _parameters) override;
    virtual void computeLogarithms(const IntakeEvent& _intakeEvent, const ParameterList& _parameters, Eigen::VectorXd& _times) override;
    virtual bool computeConcentrations(const Residuals& _inResiduals, Concentrations& _concentrations, Residuals& _outResiduals) override;
    virtual bool computeConcentration(const Value& _atTime, const Residuals& _inResiduals, Concentrations& _concentrations, Residuals& _outResiduals) override;
    void compute(const Residuals& _inResiduals, const int _forcesize, Eigen::VectorXd& _concentrations1, Eigen::VectorXd& _concentrations2);


private:
    Value m_D;	/// Quantity of drug
    Value m_Cl;	/// Clearance
    Value m_Q;	/// ???
    Value m_V1;	/// Volume of the compartment 1
    Value m_V2;	/// Volume of the compartment 2
    Value m_Ke; /// Elimination constant rate = Cl/V1 where Cl is the clearance and V1 is the volume of the compartment 1
    Value m_K12; /// Q/V1
    Value m_K21; /// Q/V2
    Value m_SumK; /// Ke+K12+K21
    Value m_RootK; /// sqrt(sumK*sumK - 4*K21*Ke)
    Value m_Divider; /// sqrt(sumK*sumK - 4*K21*Ke)
    Value m_Alpha; /// (sumK + root)/2
    Value m_Beta; /// (sumK - root)/2
    Value m_Tinf; /// Infusion time (Hours)
    Value m_Int; /// Interval (Hours)
    int m_NbPoints; /// number measure points during interval
};

inline void TwoCompartmentInfusion::compute(const Residuals& _inResiduals, const int _forcesize, Eigen::VectorXd& _concentrations1, Eigen::VectorXd& _concentrations2)
{
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
    _concentrations1 = ((A * alphaLogV) + (B * betaLogV)) / (2 * m_RootK);
    _concentrations2 = ((A2 * alphaLogV) + (BB2 * betaLogV)) / (2 * m_RootK);
    
    // During infusion
    if (_forcesize != 0) {
	Eigen::ArrayXd p1p1 = (2 * deltaD * m_K21 * betaInfLogV.head(_forcesize)).array();
	Eigen::ArrayXd p1p2 = AInf * (betaLogV.head(_forcesize) - betaInf2LogV.head(_forcesize));
	Eigen::ArrayXd p1p3 = 
	    (BInf 
	    * (rootLogV.head(_forcesize).cwiseQuotient(alphaInfLogV.head(_forcesize)) 
	    - alphaLogV.head(_forcesize).cwiseQuotient(betaInfLogV.head(_forcesize)))).array();
	Eigen::ArrayXd p2p1 = (2 * deltaD * m_K12 * betaInfLogV.head(_forcesize)).array();
	Eigen::ArrayXd p2p2 = 
	    betaLogV.head(_forcesize) 
	    * (-m_SumK - m_RootK) 
	    + betaInf2LogV.head(_forcesize) * (m_SumK + m_RootK)
	    + rootLogV.head(_forcesize).cwiseQuotient(alphaInfLogV.head(_forcesize)) * (m_SumK - m_RootK) 
	    + alphaLogV.head(_forcesize).cwiseQuotient(betaInfLogV.head(_forcesize)) * (-m_SumK + m_RootK);
	
	_concentrations1.head(_forcesize) += ((p1p1 * (p1p2 + p1p3)) / m_Divider).matrix();
	_concentrations2.head(_forcesize) += ((p2p1 * p2p2) / m_Divider).matrix();
    }

    // After infusion
    int therest = static_cast<int>(_concentrations1.size() - _forcesize);
    _concentrations1.tail(therest) += (APostInf * alphaLogV.head(therest) + BPostInf * betaLogV.head(therest)) / (2 * m_RootK);

    BB2 = 2 * m_K12 * residInf1 + (m_K12 - m_K21 + m_Ke + m_RootK)*residInf2;
    A2 = -2 * m_K12 * residInf1 + (-m_K12 + m_K21 - m_Ke + m_RootK)*residInf2;

    _concentrations2.tail(therest) += (A2 * alphaLogV.head(therest) + BB2 * betaLogV.head(therest)) / (2 * m_RootK);
}

}
}

#endif // TUCUXI_MATH_TWOCOMPARTMENTINFUSION_H
