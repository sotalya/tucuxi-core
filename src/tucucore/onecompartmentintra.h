/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_ONECOMPARTMENTINTRA_H
#define TUCUXI_CORE_ONECOMPARTMENTINTRA_H

#include "tucucore/intakeintervalcalculator.h"

namespace Tucuxi {
namespace Core {

/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment extravascular algorithm
/// \sa IntakeIntervalCalculator
class OneCompartmentIntra : public IntakeIntervalCalculator
{
public:
    /// \brief Constructor
    OneCompartmentIntra();

protected:
    virtual bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;
    virtual void computeLogarithms(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters, Eigen::VectorXd& _times) override;
    virtual bool computeConcentrations(const Residuals& _inResiduals, Concentrations& _concentrations, Residuals& _outResiduals) override;
    virtual bool computeConcentration(const int64& _atTime, const Residuals& _inResiduals, Concentrations& _concentrations, Residuals& _outResiduals) override;

private:
    Value m_D;	/// Quantity of drug
    Value m_Cl;	/// Clearance
    Value m_V;  /// Volume of the compartment
    Value m_Ke; /// Elimination constant rate = Cl/V where Cl is the clearance and V is the volume of the compartment
    int64 m_Tinf; /// Infusion time (milliseconds)
    int64 m_Int; /// Interval (milliseconds)
    int m_NbPoints; /// number measure points during interval
};

}
}

#endif // TUCUXI_CORE_ONECOMPARTMENTINTRA_H
