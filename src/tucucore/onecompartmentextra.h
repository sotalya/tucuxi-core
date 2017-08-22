/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_ONECOMPARTMENTEXTRA_H
#define TUCUXI_CORE_ONECOMPARTMENTEXTRA_H

#include "tucucore/intakeintervalcalculator.h"

namespace Tucuxi {
namespace Core {

enum class EOneCompartmentExtraLogarithms : int { Ke, Ka };

/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment extravascular algorithm
/// \sa IntakeIntervalCalculator
class OneCompartmentExtraMicro : public IntakeIntervalCalculatorBase<EOneCompartmentExtraLogarithms>
{
    INTAKEINTERVALCALCULATOR_UTILS(OneCompartmentExtraMicro)
public:
    /// \brief Constructor
    OneCompartmentExtraMicro();

    typedef EOneCompartmentExtraLogarithms Logarithms;

protected:
    virtual bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;
    virtual void computeLogarithms(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters, Eigen::VectorXd& _times) override;
    virtual bool computeConcentrations(const Residuals& _inResiduals, Concentrations& _concentrations, Residuals& _outResiduals) override;
    virtual bool computeConcentration(const Value& _atTime, const Residuals& _inResiduals, Concentrations& _concentrations, Residuals& _outResiduals) override;
    void compute(const Residuals& _inResiduals, Eigen::VectorXd& _concentrations1, Eigen::VectorXd& _concentrations2);

    Value m_D;	/// Quantity of drug
    Value m_F;  /// Biodisponibility
    Value m_Ka; /// Absorption rate constant
    Value m_V;  /// Volume of the compartment
    Value m_Ke; /// Elimination constant rate = Cl/V where Cl is the clearance and V is the volume of the compartment
    int m_NbPoints; /// Number measure points during interval
    Value m_Int; /// Interval time (Hours)

private:
};

inline void OneCompartmentExtraMicro::compute(const Residuals& _inResiduals, Eigen::VectorXd& _concentrations1, Eigen::VectorXd& _concentrations2)
{
    Concentration resid1 = _inResiduals[0];
    Concentration resid2 = _inResiduals[1] + m_F * m_D / m_V;
    Concentration part2 = m_Ka * resid2 / (-m_Ka + m_Ke);

    _concentrations1 = 
        logs(Logarithms::Ke) * resid1
	+ (logs(Logarithms::Ka) - logs(Logarithms::Ke)) * part2;
    _concentrations2 = resid2 * logs(Logarithms::Ka);

    // In ezechiel, the equation of cencenrations2 for single points was different.
    // After the test, if the result is strange, 
    // try to use following equation for calculation of single points
    #if 0
    // a.cwiseQuotient(b): element wise division of Matrix
    Eigen::VectorXd concentrations2 = 
        (m_F * m_D * logs(Logarithms::Ka)).
	cwiseQuotient((m_V * (1*Eigen::VectorXd::Ones(logs(Logarithms::Ka).size()) -
        logs(Logarithms::Ka))));
    #endif
}

class OneCompartmentExtraMacro : public OneCompartmentExtraMicro
{
    INTAKEINTERVALCALCULATOR_UTILS(OneCompartmentExtraMacro)
public:
    OneCompartmentExtraMacro();

protected:
    virtual bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;

};

}
}

#endif // TUCUXI_CORE_ONECOMPARTMENTEXTRA_H
