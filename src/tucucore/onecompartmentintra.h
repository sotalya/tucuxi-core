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
    void compute(const Residuals& _inResiduals, const int _forcesize, Eigen::VectorXd& _concentrations);

private:
    Value m_D;	/// Quantity of drug
    Value m_Cl;	/// Clearance
    Value m_V;  /// Volume of the compartment
    Value m_Ke; /// Elimination constant rate = Cl/V where Cl is the clearance and V is the volume of the compartment
    int64 m_Tinf; /// Infusion time (milliseconds)
    int64 m_Int; /// Interval (milliseconds)
    int m_NbPoints; /// number measure points during interval
};

inline void OneCompartmentIntra::compute(const Residuals& _inResiduals, const int _forcesize, Eigen::VectorXd& _concentrations)
{
    Concentration part1 = m_D / (m_Tinf * m_Cl);

    // Calcaulate concentrations
    Eigen::VectorXd concentrations = Eigen::VectorXd::Constant(m_NbPoints, _inResiduals[0]);
    concentrations = concentrations.cwiseProduct(m_precomputedLogarithms["Ke"]);

    concentrations.head(_forcesize) = 
        concentrations.head(_forcesize) 
	+ part1 * (1.0 - m_precomputedLogarithms["Ke"].head(_forcesize).array()).matrix();
    
    int therest = static_cast<int>(concentrations.size() - _forcesize);
    concentrations.tail(therest) = 
        concentrations.tail(therest) 
	+ part1 * (exp(m_Ke * m_Tinf) - 1) * m_precomputedLogarithms["Ke"].tail(therest);
}

}
}

#endif // TUCUXI_CORE_ONECOMPARTMENTINTRA_H
