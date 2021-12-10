/*
* Copyright (C) 2018 Tucuxi SA
*/

#ifndef TUCUXI_CORE_RK4ONECOMPARTMENTGAMMAEXTRA_H
#define TUCUXI_CORE_RK4ONECOMPARTMENTGAMMAEXTRA_H

#include "tucucore/intakeintervalcalculatorrk4.h"

namespace Tucuxi {
namespace Core {

enum class RK4OneCompartmentGammaExtraCompartments : int
{
    First = 0,
    Second
};

/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment extravascular algorithm
/// \sa IntakeIntervalCalculator
class RK4OneCompartmentGammaExtraMicro : public IntakeIntervalCalculatorRK4Base<1, RK4OneCompartmentGammaExtraMicro>
{
    INTAKEINTERVALCALCULATOR_UTILS(RK4OneCompartmentGammaExtraMicro)
public:
    /// \brief Constructor
    RK4OneCompartmentGammaExtraMicro();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();


    void initConcentrations(const Residuals& _inResiduals, std::vector<double>& _concentrations) override
    {
        _concentrations[0] = _inResiduals[0];
    }

    //    auto time = [&h] (const uint32_t index) -> const double {
    //        return h * index;
    //   };

    inline double probDensityAbsorptionTimes(const double& _t)
    {
        return pow(m_b, m_a) / std::tgamma(m_a) * pow(_t, m_a - 1) * exp(-m_b * _t);
    }

    inline void derive(double _t, const std::vector<double>& _c, std::vector<double>& _dcdt)
    {
        _dcdt[0] = m_F * m_D * probDensityAbsorptionTimes(_t) - m_Ke * _c[0];
    }

    inline void addFixedValue(double _t, std::vector<double>& _concentrations)
    {
        FINAL_UNUSED_PARAMETER(_t);
        FINAL_UNUSED_PARAMETER(_concentrations);
    }


protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;

    Value m_D;  /// Quantity of drug
    Value m_F;  /// bioavailability
    Value m_V;  /// Volume of the compartment
    Value m_Ke; /// Elimination constant rate = Cl/V where Cl is the clearance and V is the volume of the compartment
    Value m_a;  /// Gamma distribution parameter
    Value m_b;  /// Gamma distribution parameter

private:
    typedef RK4OneCompartmentGammaExtraCompartments Compartments;
};


class RK4OneCompartmentGammaExtraMacro : public RK4OneCompartmentGammaExtraMicro
{
    INTAKEINTERVALCALCULATOR_UTILS(RK4OneCompartmentGammaExtraMacro)
public:
    RK4OneCompartmentGammaExtraMacro();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;
};

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_RK4ONECOMPARTMENTEXTRA_H
