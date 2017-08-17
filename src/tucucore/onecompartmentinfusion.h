/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_MATH_ONECOMPARTMENTINFUSION_H
#define TUCUXI_MATH_ONECOMPARTMENTINFUSION_H

#include "tucucore/intakeintervalcalculator.h"

namespace Tucuxi {
namespace Core {

/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment extravascular algorithm
/// \sa IntakeIntervalCalculator
class OneCompartmentInfusion : public IntakeIntervalCalculator
{
public:
    /// \brief Constructor
    OneCompartmentInfusion();

protected:
    virtual bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterList& _parameters) override;
    virtual void prepareComputations(const IntakeEvent& _intakeEvent, const ParameterList& _parameters) override;
    virtual void computeLogarithms(const IntakeEvent& _intakeEvent, const ParameterList& _parameters, Eigen::VectorXd& _times) override;
    virtual bool computeConcentrations(const Residuals& _inResiduals, Concentrations& _concentrations, Residuals& _outResiduals) override;
    virtual bool computeConcentration(const Value& _atTime, const Residuals& _inResiduals, Concentrations& _concentrations, Residuals& _outResiduals) override;
    void compute(const Residuals& _inResiduals, const int _forcesize, Eigen::VectorXd& _concentrations);

private:
    Value m_D;	/// Quantity of drug
    Value m_Cl;	/// Clearance
    Value m_V;  /// Volume of the compartment
    Value m_Ke; /// Elimination constant rate = Cl/V where Cl is the clearance and V is the volume of the compartment
    Value m_Tinf; /// Infusion time (hours)
    Value m_Int; /// Interval (hours)
    int m_NbPoints; /// number measure points during interval
};

inline void OneCompartmentInfusion::compute(const Residuals& _inResiduals, const int _forcesize, Eigen::VectorXd& _concentrations)
{
    Concentration part1 = m_D / (m_Tinf * m_Cl);

    // Calcaulate concentrations
    _concentrations = Eigen::VectorXd::Constant(m_precomputedLogarithms["Ke"].size(), _inResiduals[0]);
    _concentrations = _concentrations.cwiseProduct(m_precomputedLogarithms["Ke"]);

    if(_forcesize != 0) {
	_concentrations.head(_forcesize) =
		_concentrations.head(_forcesize)
		+ part1 * (1.0 - m_precomputedLogarithms["Ke"].head(_forcesize).array()).matrix();
    }
    
    int therest = static_cast<int>(_concentrations.size() - _forcesize);
    _concentrations.tail(therest) =
        _concentrations.tail(therest)
	+ part1 * (exp(m_Ke * m_Tinf) - 1) * m_precomputedLogarithms["Ke"].tail(therest);
}

}
}

#endif // TUCUXI_MATH_ONECOMPARTMENTINFUSION_H
