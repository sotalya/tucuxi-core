/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_MATH_TWOCOMPARTMENTINTRA_H
#define TUCUXI_MATH_TWOCOMPARTMENTINTRA_H

#include "tucucore/intakeintervalcalculator.h"

namespace Tucuxi {
namespace Core {

/// \ingroup TucuCore
/// \brief Intake interval calculator for the two compartment intra algorithm
/// \sa IntakeIntervalCalculator
class TwoCompartmentIntra : public IntakeIntervalCalculator
{
public:
    /// \brief Constructor
    TwoCompartmentIntra();

protected:
    virtual bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterList& _parameters) override;
    virtual void prepareComputations(const IntakeEvent& _intakeEvent, const ParameterList& _parameters) override;
    virtual void computeLogarithms(const IntakeEvent& _intakeEvent, const ParameterList& _parameters, Eigen::VectorXd& _times) override;
    virtual void computeConcentrations(const Residuals& _inResiduals, Concentrations& _concentrations, Residuals& _outResiduals) override;

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
    Value m_DeltaD; /// (D/V)/tinf
    int64 m_Tinf; /// Infusion time (milliseconds)
    int64 m_Int; /// Interval (milliseconds)
};

}
}

#endif // TUCUXI_MATH_TWOCOMPARTMENTINTRA_H
