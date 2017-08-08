/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_MATH_THREECOMPARTMENTEXTRA_H
#define TUCUXI_MATH_THREECOMPARTMENTEXTRA_H

#include "tucucore/intakeintervalcalculator.h"

namespace Tucuxi {
namespace Core {

/// \ingroup TucuCore
/// \brief Intake interval calculator for the three compartment extra algorithm
/// \sa IntakeIntervalCalculator
class ThreeCompartmentExtra : public IntakeIntervalCalculator
{
public:
    /// \brief Constructor
    ThreeCompartmentExtra();

protected:
    virtual bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterList& _parameters) override;
    virtual void prepareComputations(const IntakeEvent& _intakeEvent, const ParameterList& _parameters) override;
    virtual void computeLogarithms(const IntakeEvent& _intakeEvent, const ParameterList& _parameters, Eigen::VectorXd& _times) override;
    virtual void computeConcentrations(const Residuals& _inResiduals, Concentrations& _concentrations, Residuals& _outResiduals) override;

private:
    Value m_D;	/// Quantity of drug
    Value m_Cl;	/// Clearance
    Value m_F;	/// ???
    Value m_Q1;	/// ???
    Value m_Q2;	/// ???
    Value m_V1;	/// Volume of the compartment 1
    Value m_V2;	/// Volume of the compartment 2
    Value m_Ka; /// Absorption rate constant
    Value m_Ke; /// Elimination constant rate = Cl/V1 where Cl is the clearance and V1 is the volume of the compartment 1
    Value m_K12; /// Q/V1
    Value m_K21; /// Q/V2
    Value m_K13; /// Q/V1
    Value m_K31; /// Q/V2
    Value m_Alpha;
    Value m_Beta;
    Value m_Gamma;
    int m_NbPoints; /// number measure points during interval
};

}
}

#endif // TUCUXI_MATH_THREECOMPARTMENTEXTRA_H