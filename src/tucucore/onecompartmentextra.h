/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_MATH_ONECOMPARTMENTEXTRA_H
#define TUCUXI_MATH_ONECOMPARTMENTEXTRA_H

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
    virtual bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterList& _parameters) override;
    virtual void prepareComputations(const IntakeEvent& _intakeEvent, const ParameterList& _parameters) override;
    virtual void computeLogarithms(const IntakeEvent& _intakeEvent, const ParameterList& _parameters, Eigen::VectorXd& _times) override;
    virtual bool computeConcentrations(const Residuals& _inResiduals, Concentrations& _concentrations, Residuals& _outResiduals) override;
    virtual bool computeConcentration(const int64& _atTime, const Residuals& _inResiduals, Concentrations& _concentrations, Residuals& _outResiduals) override;
    void compute(const Residuals& _inResiduals, Eigen::VectorXd& _concentrations1, Eigen::VectorXd& _concentrations2);


private:
    Value m_D;	/// Quantity of drug
    Value m_Cl;	/// Clearance
    Value m_F;  /// Biodisponibility
    Value m_Ka; /// Absorption rate constant
    Value m_V;  /// Volume of the compartment
    Value m_Ke; /// Elimination constant rate = Cl/V where Cl is the clearance and V is the volume of the compartment
    int m_NbPoints; /// number measure points during interval
};

inline void OneCompartmentExtra::compute(const Residuals& _inResiduals, Eigen::VectorXd& _concentrations1, Eigen::VectorXd& _concentrations2)
{
    Concentration resid1 = _inResiduals[0];
    Concentration resid2 = _inResiduals[1] + m_F * m_D / m_V;
    Concentration part2 = m_Ka * resid2 / (-m_Ka + m_Ke);

    _concentrations1 = 
        m_precomputedLogarithms["Ke"] * resid1 
	+ (m_precomputedLogarithms["Ka"] - m_precomputedLogarithms["Ke"]) * part2;
    _concentrations2 = resid2 * m_precomputedLogarithms["Ka"];

    // In ezechiel, the equation of cencenrations2 for single points was different.
    // After the test, if the result is strange, 
    // try to use following equation for calculation of single points
    #if 0
    // a.cwiseQuotient(b): element wise division of Matrix
    Eigen::VectorXd concentrations2 = 
        (m_F * m_D * m_precomputedLogarithms["Ka"]).
	cwiseQuotient((m_V * (1*Eigen::VectorXd::Ones(m_precomputedLogarithms["Ka"].size()) -
	    m_precomputedLogarithms["Ka"])));
    #endif
}

}
}

#endif // TUCUXI_MATH_ONECOMPARTMENTEXTRA_H
