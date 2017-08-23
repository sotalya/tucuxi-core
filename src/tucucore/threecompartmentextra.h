/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_THREECOMPARTMENTEXTRA_H
#define TUCUXI_CORE_THREECOMPARTMENTEXTRA_H

#include "tucucore/intakeintervalcalculator.h"

namespace Tucuxi {
namespace Core {

enum class ThreeCompartmentExtraLogarithms : int { Alpha, Beta, Gamma, Ka };

/// \ingroup TucuCore
/// \brief Intake interval calculator for the three compartment extra algorithm
/// \sa IntakeIntervalCalculator
class ThreeCompartmentExtraMicro : public IntakeIntervalCalculatorBase<ThreeCompartmentExtraLogarithms>
{
//    INTAKEINTERVALCALCULATOR_UTILS(ThreeCompartmentExtraMicro)
public:
    /// \brief Constructor
    ThreeCompartmentExtraMicro();

    typedef ThreeCompartmentExtraLogarithms Logarithms;

protected:
    virtual bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;
    virtual void computeLogarithms(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters, Eigen::VectorXd& _times) override;
    virtual bool computeConcentrations(const Residuals& _inResiduals, Concentrations& _concentrations, Residuals& _outResiduals) override;

    Value m_D;	/// Quantity of drug
    Value m_F;	/// ???
    Value m_V1;	/// Volume of the compartment 1
    Value m_Ka; /// Absorption rate constant
    Value m_Ke; /// Elimination constant rate = Cl/V1 where Cl is the clearance and V1 is the volume of the compartment 1
    Value m_K12; /// Q/V1
    Value m_K21; /// Q/V2
    Value m_K13; /// Q/V1
    Value m_K31; /// Q/V2
    Value m_Alpha;
    Value m_Beta;
    Value m_Gamma;
    int m_NbPoints; /// number measure points during interval

private:

};

class ThreeCompartmentExtraMacro : public ThreeCompartmentExtraMicro
{
//    INTAKEINTERVALCALCULATOR_UTILS(ThreeCompartmentExtraMacro)
public:
    ThreeCompartmentExtraMacro();

protected:
    virtual bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;

};

}
}

#endif // TUCUXI_CORE_THREECOMPARTMENTEXTRA_H
