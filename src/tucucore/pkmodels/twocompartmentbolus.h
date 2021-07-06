/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_TWOCOMPARTMENTBOLUS_H
#define TUCUXI_CORE_TWOCOMPARTMENTBOLUS_H

#include "tucucore/intakeintervalcalculatoranalytical.h"

namespace Tucuxi {
namespace Core {

enum class TwoCompartmentBolusExponentials : int { Alpha, Beta };
enum class TwoCompartmentBolusCompartments : int { First, Second };

/// \ingroup TucuCore
/// \brief Intake interval calculator for the two compartment bolus algorithm
/// \sa IntakeIntervalCalculator
class TwoCompartmentBolusMicro : public IntakeIntervalCalculatorBase<2, TwoCompartmentBolusExponentials>
{
    INTAKEINTERVALCALCULATOR_UTILS(TwoCompartmentBolusMicro)
public:
    /// \brief Constructor
    TwoCompartmentBolusMicro();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

    typedef TwoCompartmentBolusExponentials Exponentials;

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;
    void computeExponentials(Eigen::VectorXd& _times) override;
    bool computeConcentrations(const Residuals& _inResiduals, bool _isAll, std::vector<Concentrations>& _concentrations, Residuals& _outResiduals) override;
    bool computeConcentration(const Value& _atTime, const Residuals& _inResiduals, bool _isAll, std::vector<Concentrations>& _concentrations, Residuals& _outResiduals) override;
    void compute(const Residuals& _inResiduals, Eigen::VectorXd& _concentrations1, Eigen::VectorXd& _concentrations2);

    Value m_D;	/// Quantity of drug
    Value m_V1; /// Volume
    Value m_Ke; /// Elimination constant rate = Cl/V1 where Cl is the clearance and V1 is the volume of the compartment 1
    Value m_K12; /// Q/V1
    Value m_K21; /// Q/V2
    Value m_RootK; /// sqrt(sumK*sumK - 4*K21*Ke)
    Value m_Alpha; /// (sumK + root)/2
    Value m_Beta; /// (sumK - root)/2
    Eigen::Index m_nbPoints; /// Number measure points during interval
    Value m_Int; /// Interval (hours)

private:
    typedef TwoCompartmentBolusCompartments Compartments;
};

inline void TwoCompartmentBolusMicro::compute(const Residuals& _inResiduals, Eigen::VectorXd& _concentrations1, Eigen::VectorXd& _concentrations2)
{
    Concentration resid1 = _inResiduals[0] + (m_D/m_V1);
    Concentration resid2 = _inResiduals[1];

    Value A = ((m_K12 - m_K21 + m_Ke + m_RootK) * resid1) - (2 * m_K21 * resid2); // NOLINT(readability-identifier-naming)
    Value B = ((-m_K12 + m_K21 - m_Ke + m_RootK) * resid1) + (2 * m_K21 * resid2); // NOLINT(readability-identifier-naming)
    Value A2 = (-2 * m_K12 * resid1) + ((-m_K12 + m_K21 - m_Ke + m_RootK) * resid2); // NOLINT(readability-identifier-naming)
    Value BB2 = (2 * m_K12 * resid1) + ((m_K12 - m_K21 + m_Ke + m_RootK) * resid2); // NOLINT(readability-identifier-naming)

    // Calculate concentrations for comp1 and comp2
    _concentrations1 = 
        ((A * exponentials(Exponentials::Alpha)) + (B * exponentials(Exponentials::Beta))) / (2 * m_RootK);
    _concentrations2 = 
        ((A2 * exponentials(Exponentials::Alpha)) + (BB2 * exponentials(Exponentials::Beta))) / (2 * m_RootK);
}

class TwoCompartmentBolusMacro : public TwoCompartmentBolusMicro
{
    INTAKEINTERVALCALCULATOR_UTILS(TwoCompartmentBolusMacro)
public:
    TwoCompartmentBolusMacro();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;
};

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_TWOCOMPARTMENTBOLUS_H
