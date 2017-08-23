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
//    INTAKEINTERVALCALCULATOR_UTILS(ThreeCompartmentInfusionMicro)
public:
    /// \brief Constructor
    ThreeCompartmentInfusionMicro();

    typedef ThreeCompartmentIntraLogarithms Logarithms;

protected:
    virtual bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;
    virtual void computeLogarithms(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters, Eigen::VectorXd& _times) override;
    virtual bool computeConcentrations(const Residuals& _inResiduals, Concentrations& _concentrations, Residuals& _outResiduals) override;

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

class ThreeCompartmentInfusionMacro : public ThreeCompartmentInfusionMicro
{
//    INTAKEINTERVALCALCULATOR_UTILS(ThreeCompartmentInfusionMacro)
public:
    ThreeCompartmentInfusionMacro();

protected:
    virtual bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;

};

}
}

#endif // TUCUXI_CORE_THREECOMPARTMENTINFUSION_H
