/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_ONECOMPARTMENTINFUSION_H
#define TUCUXI_CORE_ONECOMPARTMENTINFUSION_H

#include "tucucore/intakeintervalcalculator.h"

namespace Tucuxi {
namespace Core {

enum class EOneCompartmentIntraLogarithms : int { Ke };

/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment extravascular algorithm
/// \sa IntakeIntervalCalculator
class OneCompartmentInfusionMicro : public IntakeIntervalCalculatorBase<EOneCompartmentIntraLogarithms>
{
public:
    /// \brief Constructor
    OneCompartmentInfusionMicro();

    typedef EOneCompartmentIntraLogarithms Logarithms;

protected:
    virtual bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;
    virtual void computeLogarithms(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters, Eigen::VectorXd& _times) override;
    virtual bool computeConcentrations(const Residuals& _inResiduals, Concentrations& _concentrations, Residuals& _outResiduals) override;
    virtual bool computeConcentration(const Value& _atTime, const Residuals& _inResiduals, Concentrations& _concentrations, Residuals& _outResiduals) override;
    void compute(const Residuals& _inResiduals, const int _forcesize, Eigen::VectorXd& _concentrations);

    Value m_D;	/// Quantity of drug
    Value m_Cl;	/// Clearance
    Value m_V;  /// Volume of the compartment
    Value m_Ke; /// Elimination constant rate = Cl/V where Cl is the clearance and V is the volume of the compartment
    Value m_Tinf; /// Infusion time (hours)
    Value m_Int; /// Interval (hours)
    int m_NbPoints; /// number measure points during interval

private:
};

inline void OneCompartmentInfusionMicro::compute(const Residuals& _inResiduals, const int _forcesize, Eigen::VectorXd& _concentrations)
{
//    Concentration part1 = m_D / (m_Tinf * m_Cl);
    Concentration part1 = m_D / (m_Tinf * m_Ke * m_V);

    // Calcaulate concentrations
    _concentrations = Eigen::VectorXd::Constant(logs(Logarithms::Ke).size(), _inResiduals[0]);
    _concentrations = _concentrations.cwiseProduct(logs(Logarithms::Ke));

    if(_forcesize != 0) {
	_concentrations.head(_forcesize) =
		_concentrations.head(_forcesize)
		+ part1 * (1.0 - logs(Logarithms::Ke).head(_forcesize).array()).matrix();
    }
    
    int therest = static_cast<int>(_concentrations.size() - _forcesize);
    _concentrations.tail(therest) =
        _concentrations.tail(therest)
	+ part1 * (exp(m_Ke * m_Tinf) - 1) * logs(Logarithms::Ke).tail(therest);
}

class OneCompartmentInfusionMacro : public OneCompartmentInfusionMicro
{
public:
    OneCompartmentInfusionMacro();

protected:
    virtual bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;

};


}
}

#endif // TUCUXI_CORE_ONECOMPARTMENTINFUSION_H
