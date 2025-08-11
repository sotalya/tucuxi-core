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


#ifndef TUCUXI_CORE_RKMICHAELISMENTENONECOMP_H
#define TUCUXI_CORE_RKMICHAELISMENTENONECOMP_H

#include "tucucore/intakeintervalcalculatorrk4.h"

namespace Tucuxi {
namespace Core {


enum class RkMichaelisMentenOneCompCompartments : int
{
    First = 0,
    Second
};

/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment extravascular algorithm
/// \sa IntakeIntervalCalculator
class RkMichaelisMentenOneComp : public IntakeIntervalCalculatorRK4Base<2, RkMichaelisMentenOneComp>
{

public:
    /// \brief Constructor
    RkMichaelisMentenOneComp();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

    inline void derive(double _t, const Compartments_t& _c, Compartments_t& _dcdt)
    {
        _dcdt[0] = m_Ka * _c[1] - m_Vmax * _c[0] / (m_Km + _c[0]);
        _dcdt[1] = -m_Ka * _c[1];

        if (m_isInfusion) {
            if (_t < m_TinfLow) {
                _dcdt[0] += m_infusionRate;
            }
        }
    }


    inline void deriveAtPotentialInfusionStop(double _t, const Compartments_t& _c, Compartments_t& _dcdt)
    {
        _dcdt[0] = m_Ka * _c[1] - m_Vmax * _c[0] / (m_Km + _c[0]);
        _dcdt[1] = -m_Ka * _c[1];

        if (m_isInfusion) {
            if (_t < m_TinfHigh) {
                _dcdt[0] += m_infusionRate;
            }
        }
    }

    inline void addFixedValue(double _t, Compartments_t& _concentrations)
    {
        FINAL_UNUSED_PARAMETER(_t);
        FINAL_UNUSED_PARAMETER(_concentrations);
    }

protected:
    Value m_D{NAN};  /// Quantity of drug
    Value m_F{NAN};  /// bioavailability
    Value m_Ka{NAN}; /// Absorption rate constant
    Value m_V{NAN};  /// Volume of the compartment
    Value m_Km{NAN};
    Value m_Vmax{NAN};
    Value m_Tinf{NAN};
    Value m_TinfLow{NAN};
    Value m_TinfHigh{NAN};
    Value m_infusionRate{0};
    bool m_isInfusion{false};

private:
    typedef RkMichaelisMentenOneCompCompartments Compartments;
};



/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment extravascular algorithm
/// \sa IntakeIntervalCalculator
class RkMichaelisMentenOneCompExtra : public RkMichaelisMentenOneComp
{
    INTAKEINTERVALCALCULATOR_UTILS(RkMichaelisMentenOneCompExtra)
public:
    /// \brief Constructor
    RkMichaelisMentenOneCompExtra();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;

    void initConcentrations(const Residuals& _inResiduals, MultiCompConcentration& _concentrations) override
    {
        _concentrations[0] = _inResiduals[0];
        _concentrations[1] = _inResiduals[1] + m_D / m_V * m_F;
    }
};

/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment extravascular algorithm
/// \sa IntakeIntervalCalculator
class RkMichaelisMentenOneCompBolus : public RkMichaelisMentenOneComp
{
    INTAKEINTERVALCALCULATOR_UTILS(RkMichaelisMentenOneCompBolus)
public:
    /// \brief Constructor
    RkMichaelisMentenOneCompBolus();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;

    void initConcentrations(const Residuals& _inResiduals, MultiCompConcentration& _concentrations) override
    {
        _concentrations[0] = _inResiduals[0] + m_D / m_V;
        _concentrations[1] = _inResiduals[1];
    }
};

/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment extravascular algorithm
/// \sa IntakeIntervalCalculator
class RkMichaelisMentenOneCompInfusion : public RkMichaelisMentenOneComp
{
    INTAKEINTERVALCALCULATOR_UTILS(RkMichaelisMentenOneCompInfusion)
public:
    /// \brief Constructor
    RkMichaelisMentenOneCompInfusion();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;

    void initConcentrations(const Residuals& _inResiduals, MultiCompConcentration& _concentrations) override
    {
        m_infusionRate = m_D / m_V / m_Tinf;
        _concentrations[0] = _inResiduals[0];
        _concentrations[1] = _inResiduals[1];
    }
};



} // namespace Core
} // namespace Tucuxi


#endif // TUCUXI_CORE_RKMICHAELISMENTENONECOMP_H
