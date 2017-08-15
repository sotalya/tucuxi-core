/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_ONECOMPARTMENTBOLUS_H
#define TUCUXI_CORE_ONECOMPARTMENTBOLUS_H

#include "tucucore/intakeintervalcalculator.h"

namespace Tucuxi {
namespace Core {

enum class EOneCompartmentBolusLogarithms : int { Ke };

/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment bolus algorithm
/// \sa IntakeIntervalCalculator
class OneCompartmentBolus : public IntakeIntervalCalculatorBase<EOneCompartmentBolusLogarithms>
{
public:
    /// \brief Constructor
    OneCompartmentBolus();

    typedef EOneCompartmentBolusLogarithms Logarithms;

protected:
    virtual bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;
    virtual void computeLogarithms(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters, Eigen::VectorXd& _times) override;
    virtual bool computeConcentrations(const Residuals& _inResiduals, Concentrations& _concentrations, Residuals& _outResiduals) override;
    virtual bool computeConcentration(const int64& _atTime, const Residuals& _inResiduals, Concentrations& _concentrations, Residuals& _outResiduals) override;
    void compute(const Residuals& _inResiduals, Eigen::VectorXd& _concentrations);

private:
    Value m_D;	/// Quantity of drug
    Value m_V;	/// Volume of the compartment
    Value m_Ke; /// Elimination constant rate = Cl/V where Cl is the clearance and V is the volume of the compartment
    int m_NbPoints; /// Number measure points during interval
    int m_Int; /// Interval time
};

inline void OneCompartmentBolus::compute(const Residuals& _inResiduals, Eigen::VectorXd& _concentrations)
{    
    _concentrations = (m_D / m_V + _inResiduals[0]) * logs(Logarithms::Ke);
}

}
}

#endif // TUCUXI_CORE_ONECOMPARTMENTBOLUS_H
