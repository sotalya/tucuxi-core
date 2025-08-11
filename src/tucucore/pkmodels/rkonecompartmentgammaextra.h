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


    void initConcentrations(const Residuals& _inResiduals, MultiCompConcentration& _concentrations) override
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

    inline void derive(double _t, const Compartments_t& _c, Compartments_t& _dcdt)
    {
        _dcdt[0] = m_F * m_D * probDensityAbsorptionTimes(_t) - m_Ke * _c[0];
    }

    inline void addFixedValue(double _t, Compartments_t& _concentrations)
    {
        FINAL_UNUSED_PARAMETER(_t);
        FINAL_UNUSED_PARAMETER(_concentrations);
    }


protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;

    Value m_D{NAN}; /// Quantity of drug
    Value m_F{NAN}; /// bioavailability
    Value m_V{NAN}; /// Volume of the compartment
    Value m_Ke{
            NAN}; /// Elimination constant rate = Cl/V where Cl is the clearance and V is the volume of the compartment
    Value m_a{NAN}; /// Gamma distribution parameter
    Value m_b{NAN}; /// Gamma distribution parameter

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
