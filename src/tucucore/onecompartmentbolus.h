/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_MATH_ONECOMPARTMENTBOLUS_H
#define TUCUXI_MATH_ONECOMPARTMENTBOLUS_H

#include "tucucore/intakeintervalcalculator.h"

namespace Tucuxi {
namespace Core {

/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment bolus algorithm
/// \sa IntakeIntervalCalculator
class OneCompartmentBolus : public IntakeIntervalCalculator
{
public:
    /// \brief Constructor
    OneCompartmentBolus();

protected:
    virtual bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterList& _parameters) override;
    virtual void prepareComputations(const IntakeEvent& _intakeEvent, const ParameterList& _parameters) override;
    virtual void computeLogarithms(const IntakeEvent& _intakeEvent, const ParameterList& _parameters, Eigen::VectorXd& _times) override;
    virtual bool computeConcentrations(const Residuals& _inResiduals, Concentrations& _concentrations, Residuals& _outResiduals) override;
    virtual bool computeConcentration(const int64& _atTime, const Residuals& _inResiduals, Concentrations& _concentrations, Residuals& _outResiduals) override;
    Eigen::VectorXd compute(const Residuals& _inResiduals);

private:
    Value m_D;	/// Quantity of drug
    Value m_V;	///	Volume of the compartment
    Value m_Ke; /// Elimination constant rate = Cl/V where Cl is the clearance and V is the volume of the compartment
    int m_NbPoints; /// number measure points during interval
};

inline Eigen::VectorXd OneCompartmentBolus::compute(const Residuals& _inResiduals)
{
    return (m_D / m_V + _inResiduals[0]) * m_precomputedLogarithms["Ke"];
}

}
}

#endif // TUCUXI_MATH_ONECOMPARTMENTBOLUS_H
