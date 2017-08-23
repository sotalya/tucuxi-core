/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_THREECOMPARTMENTINFUSION_H
#define TUCUXI_CORE_THREECOMPARTMENTINFUSION_H

#include "tucucore/intakeintervalcalculator.h"

namespace Tucuxi {
namespace Core {

enum class ThreeCompartmentIntraLogarithms : int { Alpha, Beta, Gamma };

/// \ingroup TucuCore
/// \brief Intake interval calculator for the three compartment infusion algorithm
/// \sa IntakeIntervalCalculator
class ThreeCompartmentInfusionMicro : public IntakeIntervalCalculatorBase<3, ThreeCompartmentIntraLogarithms>
{
    INTAKEINTERVALCALCULATOR_UTILS(ThreeCompartmentInfusionMicro)
public:
    /// \brief Constructor
    ThreeCompartmentInfusionMicro();

    typedef ThreeCompartmentIntraLogarithms Logarithms;

protected:
    virtual bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;
    virtual void computeLogarithms(Eigen::VectorXd& _times) override;
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
    Value m_Tinf; /// Infusion time (hours)
    Value m_Int; /// Interval (hours)
    int m_NbPoints; /// number measure points during interval

private:

};

inline void ThreeCompartmentInfusionMicro::compute(const Residuals& _inResiduals, Eigen::VectorXd&
_concentrations1, Value& _concentrations2, Value& _concentrations3)
{
    Value deltaD = (m_D / m_V1) / m_Tinf; 
    Value alphaTinf = std::exp(-m_Alpha* m_Tinf);
    Value betaTinf = std::exp(-m_Beta * m_Tinf);
    Value gammaTinf = std::exp(-m_Gamma * m_Tinf);

    Value A = (1 / m_V1) * (m_K21 - m_Alpha) * (m_K31 - m_Alpha) / (m_Alpha - m_Beta) / (m_Alpha - m_Gamma);
    Value B = (1 / m_V1) * (m_K21 - m_Beta) * (m_K31 - m_Beta) / (m_Beta - m_Alpha) / (m_Beta - m_Gamma);
    Value C = (1 / m_V1) * (m_K21 - m_Gamma) * (m_K31 - m_Gamma) / (m_Gamma - m_Beta) / (m_Gamma - m_Alpha);
    Value A2 = m_K12 / (m_K21 - m_Alpha) * A;
    Value B2 = m_K12 / (m_K21 - m_Beta) * B;
    Value C2 = m_K12 / (m_K21 - m_Gamma) * C;
    Value A3 = m_K13 / (m_K31 - m_Alpha) * A;
    Value B3 = m_K13 / (m_K31 - m_Beta) * B;
    Value C3 = m_K13 / (m_K31 - m_Gamma) * C;

    int logSize = (logs(Logarithms::Alpha)).size();

    for (int t = 0; t < logSize; ++t) 
    {
       // Compare the point is outside of infusion time or not
       if ((t * (m_Int/logSize)) < m_Tinf)
       {
            _concentrations1(t) = 
	        deltaD 
	        * (A/m_Alpha * (1 - logs(Logarithms::Alpha)(t)) 
		    + B/m_Beta * (1 - logs(Logarithms::Beta)(t)) 
	            + C/m_Gamma * (1 - logs(Logarithms::Gamma)(t)));
       } 
       else 
       {
            _concentrations1(t) = 
	        deltaD 
	        * (A/m_Alpha * (1 - alphaTinf) * logs(Logarithms::Alpha)(t) / alphaTinf 
	            + B/m_Beta * (1 - betaTinf) * logs(Logarithms::Beta)(t) / betaTinf 
	            + C/m_Gamma * (1 - gammaTinf) *    logs(Logarithms::Gamma)(t) / gammaTinf);
       }
    }

    // Do NOT use m_NbPoints because in case of single calculation "m_NbPoints = 0"
    _concentrations2 =
        deltaD * 
        (A2/m_Alpha * (1 - alphaTinf) * logs(Logarithms::Alpha)(logSize - 1) / alphaTinf 
	    + B2/m_Beta * (1 - betaTinf) * logs(Logarithms::Beta)(logSize - 1) / betaTinf 
            + C2/m_Gamma * (1 - gammaTinf) * logs(Logarithms::Gamma)(logSize - 1) / gammaTinf);

    _concentrations3 = 
        deltaD * 
        (A3/m_Alpha * (1 - alphaTinf) * logs(Logarithms::Alpha)(logSize - 1) / alphaTinf 
	    + B3/m_Beta * (1 - betaTinf) * logs(Logarithms::Beta)(logSize - 1) / betaTinf 
            + C3/m_Gamma * (1 - gammaTinf) * logs(Logarithms::Gamma)(logSize - 1) / gammaTinf);

}

class ThreeCompartmentInfusionMacro : public ThreeCompartmentInfusionMicro
{
    INTAKEINTERVALCALCULATOR_UTILS(ThreeCompartmentInfusionMacro)
public:
    ThreeCompartmentInfusionMacro();

protected:
    virtual bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;

};

}
}

#endif // TUCUXI_CORE_THREECOMPARTMENTINFUSION_H
