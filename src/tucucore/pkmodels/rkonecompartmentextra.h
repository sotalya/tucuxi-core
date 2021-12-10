/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_RK4ONECOMPARTMENTEXTRA_H
#define TUCUXI_CORE_RK4ONECOMPARTMENTEXTRA_H

#include "tucucore/intakeintervalcalculatorrk4.h"

namespace Tucuxi {
namespace Core {

enum class RK4OneCompartmentExtraCompartments : int
{
    First = 0,
    Second
};

/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment extravascular algorithm
/// \sa IntakeIntervalCalculator
class RK4OneCompartmentExtraMicro : public IntakeIntervalCalculatorRK4Base<2, RK4OneCompartmentExtraMicro>
{
    INTAKEINTERVALCALCULATOR_UTILS(RK4OneCompartmentExtraMicro)
public:
    /// \brief Constructor
    RK4OneCompartmentExtraMicro();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

    inline void derive(double _t, const std::vector<double>& _c, std::vector<double>& _dcdt)
    {
        FINAL_UNUSED_PARAMETER(_t);
        _dcdt[0] = m_Ka * _c[1] - m_Ke * _c[0];
        _dcdt[1] = -m_Ka * _c[1];
    }

    inline void addFixedValue(double _t, std::vector<double>& _concentrations)
    {
        FINAL_UNUSED_PARAMETER(_t);
        FINAL_UNUSED_PARAMETER(_concentrations);
    }

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;

    void initConcentrations(const Residuals& _inResiduals, std::vector<double>& _concentrations) override
    {
        _concentrations[0] = _inResiduals[0];
        _concentrations[1] = _inResiduals[1] + m_D / m_V;
    }

    Value m_D;  /// Quantity of drug
    Value m_F;  /// bioavailability
    Value m_Ka; /// Absorption rate constant
    Value m_V;  /// Volume of the compartment
    Value m_Ke; /// Elimination constant rate = Cl/V where Cl is the clearance and V is the volume of the compartment


private:
    typedef RK4OneCompartmentExtraCompartments Compartments;
};



class RK4OneCompartmentExtraMacro : public RK4OneCompartmentExtraMicro
{
    INTAKEINTERVALCALCULATOR_UTILS(RK4OneCompartmentExtraMacro)
public:
    RK4OneCompartmentExtraMacro();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;
};

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_RK4ONECOMPARTMENTEXTRA_H
