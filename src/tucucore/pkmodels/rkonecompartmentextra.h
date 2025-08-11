/*
 * Tucuxi - Tucuxi-core library and command line tool.
 * This code allows to perform prediction of drug concentration in blood
 * and to propose dosage adaptations.
 * It has been developed by HEIG-VD, in close collaboration with CHUV.
 * Copyright (C) 2023 HEIG-VD, maintained by Yann Thoma  <yann.thoma@heig-vd.ch>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
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

    inline void derive(double _t, const Compartments_t& _c, Compartments_t& _dcdt)
    {
        FINAL_UNUSED_PARAMETER(_t);
        _dcdt[0] = m_Ka * _c[1] - m_Ke * _c[0];
        _dcdt[1] = -m_Ka * _c[1];
    }

    inline void addFixedValue(double _t, Compartments_t& _concentrations)
    {
        FINAL_UNUSED_PARAMETER(_t);
        FINAL_UNUSED_PARAMETER(_concentrations);
    }

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;

    void initConcentrations(const Residuals& _inResiduals, MultiCompConcentration& _concentrations) override
    {
        _concentrations[0] = _inResiduals[0];
        _concentrations[1] = _inResiduals[1] + m_D / m_V;
    }

    Value m_D{NAN};  /// Quantity of drug
    Value m_F{NAN};  /// bioavailability
    Value m_Ka{NAN}; /// Absorption rate constant
    Value m_V{NAN};  /// Volume of the compartment
    Value m_Ke{
            NAN}; /// Elimination constant rate = Cl/V where Cl is the clearance and V is the volume of the compartment


private:
    typedef RK4OneCompartmentExtraCompartments Compartments;
};



class RK4OneCompartmentExtraMacro : public RK4OneCompartmentExtraMicro
{
    INTAKEINTERVALCALCULATOR_UTILS(RK4OneCompartmentExtraMacro)
public:
    RK4OneCompartmentExtraMacro() = default;

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;
};

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_RK4ONECOMPARTMENTEXTRA_H
