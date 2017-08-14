/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_TWOCOMPARTMENTEXTRA_H
#define TUCUXI_CORE_TWOCOMPARTMENTEXTRA_H

#include "tucucore/intakeintervalcalculator.h"

namespace Tucuxi {
namespace Core {

/// \ingroup TucuCore
/// \brief Intake interval calculator for the two compartment extra algorithm
/// \sa IntakeIntervalCalculator
class TwoCompartmentExtra : public IntakeIntervalCalculator
{
public:
    /// \brief Constructor
    TwoCompartmentExtra();

protected:
    virtual bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;
    virtual void computeLogarithms(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters, Eigen::VectorXd& _times) override;
    virtual bool computeConcentrations(const Residuals& _inResiduals, Concentrations& _concentrations, Residuals& _outResiduals) override;
    virtual bool computeConcentration(const int64& _atTime, const Residuals& _inResiduals, Concentrations& _concentrations, Residuals& _outResiduals) override;

private:
    Value m_D;	/// Quantity of drug
    Value m_Cl;	/// Clearance
    Value m_F;  /// Biodisponibility
    Value m_Ka; /// Absorption rate constant
    Value m_Q;	/// ???
    Value m_V1;	/// Volume of the compartment 1
    Value m_V2;	/// Volume of the compartment 2
    Value m_Ke; /// Elimination constant rate = Cl/V1 where Cl is the clearance and V1 is the volume of the compartment 1
    Value m_K12; /// Q/V1
    Value m_K21; /// Q/V2
    Value m_RootK; /// sqrt(sumK*sumK - 4*K21*Ke)
    Value m_Alpha; /// (sumK + root)/2
    Value m_Beta; /// (sumK - root)/2
    int m_NbPoints; /// number measure points during interval
};

}
}

#endif // TUCUXI_CORE_TWOCOMPARTMENTEXTRA_H
