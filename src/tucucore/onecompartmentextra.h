/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_ONECOMPARTMENTEXTRA_H
#define TUCUXI_CORE_ONECOMPARTMENTEXTRA_H

#include "tucucore/intakeintervalcalculator.h"

namespace Tucuxi {
namespace Core {

/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment extravascular algorithm
/// \sa IntakeIntervalCalculator
class OneCompartmentExtra : public IntakeIntervalCalculator
{
public:
    /// \brief Constructor
    OneCompartmentExtra();

protected:
    virtual bool checkInputs(const IntakeEvent& _intakeEvent, const Parameters& _parameters) override;
    virtual void prepareComputations(const IntakeEvent& _intakeEvent, const Parameters& _parameters) override;
    virtual void computeLogarithms(const IntakeEvent& _intakeEvent, const Parameters& _parameters, Eigen::VectorXd& _times) override;
    virtual bool computeConcentrations(const Residuals& _inResiduals, Concentrations& _concentrations, Residuals& _outResiduals) override;

private:
    Value m_D;	/// Quantity of drug
    Value m_Cl;	/// Clearance
    Value m_F;  /// Biodisponibility
    Value m_Ka; /// Absorption rate constant
    Value m_V;  /// Volume of the compartment
    Value m_Ke; /// Elimination constant rate = Cl/V where Cl is the clearance and V is the volume of the compartment
    int m_NbPoints; /// number measure points during interval
};

}
}

#endif // TUCUXI_CORE_ONECOMPARTMENTEXTRA_H
