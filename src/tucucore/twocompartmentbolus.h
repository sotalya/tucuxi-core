/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_MATH_TWOCOMPARTMENTBOLUS_H
#define TUCUXI_MATH_TWOCOMPARTMENTBOLUS_H

#include "tucucore/intakeintervalcalculator.h"

namespace Tucuxi {
namespace Core {

/// \ingroup TucuCore
/// \brief Intake interval calculator for the two compartment bolus algorithm
/// \sa IntakeIntervalCalculator
class TwoCompartmentBolus : public IntakeIntervalCalculator
{
public:
    /// \brief Constructor
    TwoCompartmentBolus();

protected:
    virtual bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterList& _parameters) override;
    virtual void prepareComputations(const IntakeEvent& _intakeEvent, const ParameterList& _parameters) override;
    virtual void computeLogarithms(const IntakeEvent& _intakeEvent, const ParameterList& _parameters, Eigen::VectorXd& _times) override;
    virtual bool computeConcentrations(const Residuals& _inResiduals, Concentrations& _concentrations, Residuals& _outResiduals) override;
    virtual bool computeConcentration(const Value& _atTime, const Residuals& _inResiduals, Concentrations& _concentrations, Residuals& _outResiduals) override;
    void compute(const Residuals& _inResiduals, Eigen::VectorXd& _concentrations1, Eigen::VectorXd& _concentrations2);

private:
    Value m_D;	/// Quantity of drug
    Value m_Cl;	/// Clearance
    Value m_Q;	/// ???
    Value m_V1;	/// Volume of the compartment 1
    Value m_V2;	/// Volume of the compartment 2
    Value m_Ke; /// Elimination constant rate = Cl/V1 where Cl is the clearance and V1 is the volume of the compartment 1
    Value m_K12; /// Q/V1
    Value m_K21; /// Q/V2
    Value m_RootK; /// sqrt(sumK*sumK - 4*K21*Ke)
    Value m_Alpha; /// (sumK + root)/2
    Value m_Beta; /// (sumK - root)/2
    int m_NbPoints; /// Number measure points during interval
    Value m_Int; /// Interval (hours)
};

inline void TwoCompartmentBolus::compute(const Residuals& _inResiduals, Eigen::VectorXd& _concentrations1, Eigen::VectorXd& _concentrations2)
{
    Concentration resid1 = _inResiduals[0] + (m_D/m_V1);
    Concentration resid2 = _inResiduals[1];

    Value A = ((m_K12 - m_K21 + m_Ke + m_RootK) * resid1) - (2 * m_K21 * resid2);
    Value B = ((-m_K12 + m_K21 - m_Ke + m_RootK) * resid1) + (2 * m_K21 * resid2);
    Value A2 = (-2 * m_K12 * resid1) + ((-m_K12 + m_K21 - m_Ke + m_RootK) * resid2);
    Value BB2 = (2 * m_K12 * resid1) + ((m_K12 - m_K21 + m_Ke + m_RootK) * resid2);

    // Calculate concentrations for comp1 and comp2
    _concentrations1 = 
        ((A * m_precomputedLogarithms["Alpha"]) + (B * m_precomputedLogarithms["Beta"])) / (2 * m_RootK);
    _concentrations2 = 
        ((A2 * m_precomputedLogarithms["Alpha"]) + (BB2 * m_precomputedLogarithms["Beta"])) / (2 * m_RootK);
}

}
}

#endif // TUCUXI_MATH_TWOCOMPARTMENTBOLUS_H
