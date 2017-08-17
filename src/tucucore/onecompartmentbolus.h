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
class OneCompartmentBolusMicro : public IntakeIntervalCalculator
{
public:
    /// \brief Constructor
    OneCompartmentBolusMicro();

protected:
    virtual bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterList& _parameters) override;
    virtual void prepareComputations(const IntakeEvent& _intakeEvent, const ParameterList& _parameters) override;
    virtual void computeLogarithms(const IntakeEvent& _intakeEvent, const ParameterList& _parameters, Eigen::VectorXd& _times) override;
    virtual bool computeConcentrations(const Residuals& _inResiduals, Concentrations& _concentrations, Residuals& _outResiduals) override;
    virtual bool computeConcentration(const int64& _atTime, const Residuals& _inResiduals, Concentrations& _concentrations, Residuals& _outResiduals) override;
    void compute(const Residuals& _inResiduals, Eigen::VectorXd& _concentrations);

protected:
    Value m_D;	/// Quantity of drug
    Value m_V;	/// Volume of the compartment
    Value m_Ke; /// Elimination constant rate = Cl/V where Cl is the clearance and V is the volume of the compartment
    int m_NbPoints; /// Number measure points during interval
    int64 m_Int; /// Interval (hours)
};

inline void OneCompartmentBolusMicro::compute(const Residuals& _inResiduals, Eigen::VectorXd& _concentrations)
{
    _concentrations = (m_D / m_V + _inResiduals[0]) * m_precomputedLogarithms["Ke"];
}

class OneCompartmentBolusMacro : public OneCompartmentBolusMicro
{
public:
    OneCompartmentBolusMacro();

protected:
    virtual bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterList& _parameters) override;

};

}
}

#endif // TUCUXI_MATH_ONECOMPARTMENTBOLUS_H
