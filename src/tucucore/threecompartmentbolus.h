/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_THREECOMPARTMENTBOLUS_H
#define TUCUXI_CORE_THREECOMPARTMENTBOLUS_H

#include "tucucore/intakeintervalcalculator.h"

namespace Tucuxi {
namespace Core {

enum class ThreeCompartmentBolusLogarithms : int { Alpha, Beta, Gamma };

/// \ingroup TucuCore
/// \brief Intake interval calculator for the three compartment bolus algorithm
/// \sa IntakeIntervalCalculator
class ThreeCompartmentBolusMicro : public IntakeIntervalCalculatorBase<ThreeCompartmentBolusLogarithms>
{
    INTAKEINTERVALCALCULATOR_UTILS(ThreeCompartmentBolusMicro)
public:
    /// \brief Constructor
    ThreeCompartmentBolusMicro();

    typedef ThreeCompartmentBolusLogarithms Logarithms;

protected:
    virtual bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;
    virtual void computeLogarithms(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters, Eigen::VectorXd& _times) override;
    virtual bool computeConcentrations(const Residuals& _inResiduals, Concentrations& _concentrations, Residuals& _outResiduals) override;
    virtual bool computeConcentration(const Value& _atTime, const Residuals& _inResiduals, Concentrations& _concentrations, Residuals& _outResiduals) override;
    void compute(const Residuals& _inResiduals, Eigen::VectorXd& _concentrations1, Value& _concentrations2, Value& _concentrations3);

    Value m_D;	/// Quantity of drug
    Value m_F;	/// ???
    Value m_V1;	/// Volume of the compartment 1
    Value m_Ke; /// Elimination constant rate = Cl/V1 where Cl is the clearance and V1 is the volume of the compartment 1
    Value m_K12; /// Q/V1
    Value m_K21; /// Q/V2
    Value m_K13; /// Q/V1
    Value m_K31; /// Q/V2
    Value m_Alpha;
    Value m_Beta;
    Value m_Gamma;
    int m_NbPoints; /// number measure points during interval
    Value m_Int; /// Interval (hours)

private:

};

inline void ThreeCompartmentBolusMicro::compute(const Residuals& _inResiduals, Eigen::VectorXd&
_concentrations1, Value& _concentrations2, Value& _concentrations3)
{
    Concentration resid1 = _inResiduals[0] + m_F*m_D/m_V1;
    Concentration resid2 = _inResiduals[1];
    Concentration resid3 = _inResiduals[2];

    Value A = (1 / m_V1) * (m_K21 - m_Alpha) * (m_K31 - m_Alpha) / (m_Alpha - m_Beta) / (m_Alpha - m_Gamma);
    Value B = (1 / m_V1) * (m_K21 - m_Beta) * (m_K31 - m_Beta) / (m_Beta - m_Alpha) / (m_Beta - m_Gamma);
    Value C = (1 / m_V1) * (m_K21 - m_Gamma) * (m_K31 - m_Gamma) / (m_Gamma - m_Beta) / (m_Gamma - m_Alpha);
    Value A2 = m_K12 / (m_K21 - m_Alpha) * A;
    Value B2 = m_K12 / (m_K21 - m_Beta) * B;
    Value C2 = m_K12 / (m_K21 - m_Gamma) * C;
    Value A3 = m_K13 / (m_K31 - m_Alpha) * A;
    Value B3 = m_K13 / (m_K31 - m_Beta) * B;
    Value C3 = m_K13 / (m_K31 - m_Gamma) * C;

    // Calculate concentrations for comp1, comp2 and comp3
    _concentrations1 = 
        resid1 * (B * logs(Logarithms::Beta) 
	    + A * logs(Logarithms::Alpha)
	    + C * logs(Logarithms::Gamma));

    // don't use m_NbPoints because in case of single calculation "m_NbPoints = 0"
    _concentrations2 = 
        resid2 + resid1 * (B2 * logs(Logarithms::Beta)((logs(Logarithms::Beta)).size() - 1) 
            + A2 * logs(Logarithms::Alpha)((logs(Logarithms::Alpha)).size() - 1)
            + C2 * logs(Logarithms::Gamma)((logs(Logarithms::Gamma)).size() - 1));

    _concentrations3 = 
        resid3 + resid1 * (B3 * logs(Logarithms::Beta)((logs(Logarithms::Beta)).size() - 1) 
            + A3 * logs(Logarithms::Alpha)((logs(Logarithms::Alpha)).size() - 1) 
            + C3 * logs(Logarithms::Gamma)((logs(Logarithms::Gamma)).size() - 1));

}

class ThreeCompartmentBolusMacro : public ThreeCompartmentBolusMicro
{
    INTAKEINTERVALCALCULATOR_UTILS(ThreeCompartmentBolusMacro)
public:
    ThreeCompartmentBolusMacro();

protected:
    virtual bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;

};

}
}

#endif // TUCUXI_CORE_THREECOMPARTMENTBOLUS_H
