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

#ifndef TUCUXI_CORE_RKMICHAELISMENTENAIEMAX_H
#define TUCUXI_CORE_RKMICHAELISMENTENAIEMAX_H

#include "tucucore/intakeintervalcalculatorrk4.h"

namespace Tucuxi {
namespace Core {


enum class RkMichaelisMentenOneCompAiEmaxCompartments : int
{
    First = 0,
    Second
};

/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment extravascular
///        algorithm
/// \sa IntakeIntervalCalculator
class RkMichaelisMentenOneCompAiEmax : public IntakeIntervalCalculatorRK4Base<2, RkMichaelisMentenOneCompAiEmax>
{
    INTAKEINTERVALCALCULATOR_UTILS(RkMichaelisMentenOneCompAiEmax)
public:
    /// \brief Constructor
    RkMichaelisMentenOneCompAiEmax();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

    inline void derive(double _t, const Compartments_t& _c, Compartments_t& _dcdt)
    {
        double const adj_t = _t - m_Tfs;
        double const cc = _c[1] / m_V;
        double const AI = 1 + (m_Emax * adj_t) / (m_T50 + adj_t);
        double const ClAI = ((m_Vmax * cc) / (m_Km + cc)) * AI;
        double const ka_c0 = m_Ka * _c[0];

        _dcdt[0] = -ka_c0;
        _dcdt[1] = ka_c0 - ClAI;
    }

    inline void addFixedValue(double _t, Compartments_t& _concentrations)
    {
        FINAL_UNUSED_PARAMETER(_t);
        FINAL_UNUSED_PARAMETER(_concentrations);
    }

    void initConcentrations(const Residuals& _inResiduals, MultiCompConcentration& _concentrations) override
    {
        _concentrations[0] = _inResiduals[1] / m_V;
        _concentrations[1] = _concentrations[0];
        // Do not forget to reinitialize the flag for delivery of the drug
        m_delivered = false;
    }

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;

    Value m_D{NAN};    /// Drug quantity
    Value m_F{0.0};    /// Bioavailability
    Value m_V{NAN};    /// Volume of the compartment
    Value m_Vmax{NAN}; /// Maximum elimination rate
    Value m_Km{NAN};   /// Concentration of half-maximal elimination rate
    Value m_Ka{0.0};   /// Absorption rate constant
    Value m_Emax{NAN}; /// Maximum effect parameter
    Value m_T50{NAN};  /// Time at which the effect reaches half of its maximum
    Value m_Tfs{0.0};  /// Time from treatment start

    bool m_delivered{false};
    bool m_isWithLag{false};

private:
    typedef RkMichaelisMentenOneCompAiEmaxCompartments Compartments;
};


/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment extravascular
///        algorithm
/// \sa IntakeIntervalCalculator
class RkMichaelisMentenOneCompAiEmaxLag : public RkMichaelisMentenOneCompAiEmax
{
    INTAKEINTERVALCALCULATOR_UTILS(RkMichaelisMentenOneCompAiEmaxLag)
public:
    /// \brief Constructor
    RkMichaelisMentenOneCompAiEmaxLag()
    {
        m_delivered = false;
        m_isWithLag = true;
    }

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

    inline void addFixedValue(double _t, Compartments_t& _concentrations)
    {
        if (m_isWithLag) {
            if ((!m_delivered) && (_t >= m_Tlag)) {
                _concentrations[1] += m_D / m_V * m_F;
                m_delivered = true;
            }
        }
    }


protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;

    Value m_Tlag{0.0}; /// Lag time before absorption begins
};


} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_RKMICHAELISMENTENAIEMAX_H
