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


#ifndef TUCUXI_CORE_RKMICHAELISMENTENTWOCOMP_H
#define TUCUXI_CORE_RKMICHAELISMENTENTWOCOMP_H

#include "tucucore/intakeintervalcalculatorrk4.h"

namespace Tucuxi {
namespace Core {


enum class RkMichaelisMentenTwoCompCompartments : int
{
    First = 0,
    Second
};

/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment extravascular algorithm
/// \sa IntakeIntervalCalculator
class RkMichaelisMentenTwoComp : public IntakeIntervalCalculatorRK4Base<3, RkMichaelisMentenTwoComp>
{

public:
    /// \brief Constructor
    RkMichaelisMentenTwoComp();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

    inline void derive(double _t, const Compartments_t& _c, Compartments_t& _dcdt)
    {
        // This function _dcdt[0] needs to be checked. I used the one of ADAPT5-User-Guide, but used
        // c[0] = x1/Vc, as Tucuxi does not allow to calculate an output easily (could be implemented in the future
        //
        // Actually it considers VMax to be amout/time
        // _dcdt[0] = (m_Ka * _c[2] + m_K21 * _c[1] - m_K12 * _c[0] * m_V - m_Vmax * _c[0] / (m_Km + _c[0])) / m_V;

        // This version considers VMax to be concentration/time
        _dcdt[0] = m_Ka * _c[2] + m_K21 * _c[1] - m_K12 * _c[0] - m_Vmax * _c[0] / (m_Km + _c[0]);
        _dcdt[1] = m_K12 * _c[0] - m_K21 * _c[1];
        _dcdt[2] = -m_Ka * _c[2];

        if (m_isInfusion) {
            if (_t <= m_TinfLow) {
                _dcdt[0] += m_infusionRate;
            }
        }
    }

    inline void deriveAtPotentialInfusionStop(double _t, const Compartments_t& _c, Compartments_t& _dcdt)
    {
        // This version considers VMax to be concentration/time
        _dcdt[0] = m_Ka * _c[2] + m_K21 * _c[1] - m_K12 * _c[0] - m_Vmax * _c[0] / (m_Km + _c[0]);
        _dcdt[1] = m_K12 * _c[0] - m_K21 * _c[1];
        _dcdt[2] = -m_Ka * _c[2];

        if (m_isInfusion) {
            if (_t < m_TinfHigh) {
                _dcdt[0] += m_infusionRate;
            }
        }
    }

    inline void addFixedValue(double _t, Compartments_t& _concentrations)
    {
        if (m_isWithLag) {
            if ((!m_delivered) && (_t >= m_Tlag)) {
                _concentrations[2] += m_D / m_V1 * m_F;
                m_delivered = true;
            }
        }
    }

protected:
    Value m_D{NAN};  /// Quantity of drug
    Value m_F{1.0};  /// bioavailability
    Value m_Ka{0.0}; /// Absorption rate constant
    Value m_V1{NAN}; /// Volume of the compartment
    Value m_Km{NAN};
    Value m_Vmax{NAN};
    Value m_K12{NAN};
    Value m_K21{NAN};
    Value m_Tinf{NAN};
    Value m_TinfLow{NAN};
    Value m_TinfHigh{NAN};
    Value m_Tlag{NAN};
    Value m_infusionRate{0};
    bool m_delivered{false};
    bool m_isInfusion{false};
    bool m_isWithLag{false};

private:
    typedef RkMichaelisMentenTwoCompCompartments Compartments;
};



/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment extravascular algorithm
/// \sa IntakeIntervalCalculator
class RkMichaelisMentenTwoCompExtraMicro : public RkMichaelisMentenTwoComp
{
    INTAKEINTERVALCALCULATOR_UTILS(RkMichaelisMentenTwoCompExtraMicro)
public:
    /// \brief Constructor
    RkMichaelisMentenTwoCompExtraMicro();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;

    void initConcentrations(const Residuals& _inResiduals, MultiCompConcentration& _concentrations) override
    {
        _concentrations[0] = _inResiduals[0];
        _concentrations[1] = _inResiduals[1];
        _concentrations[2] = _inResiduals[2] + m_D / m_V1 * m_F;
    }
};


/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment extravascular algorithm
/// \sa IntakeIntervalCalculator
class RkMichaelisMentenTwoCompExtraLagMicro : public RkMichaelisMentenTwoComp
{
    INTAKEINTERVALCALCULATOR_UTILS(RkMichaelisMentenTwoCompExtraLagMicro)
public:
    /// \brief Constructor
    RkMichaelisMentenTwoCompExtraLagMicro();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;

    void initConcentrations(const Residuals& _inResiduals, MultiCompConcentration& _concentrations) override
    {
        _concentrations[0] = _inResiduals[0];
        _concentrations[1] = _inResiduals[1];
        _concentrations[2] = _inResiduals[2];
        // Do not forget to reinitialize the flag for delivery of the drug
        m_delivered = false;
    }
};


/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment extravascular algorithm
/// \sa IntakeIntervalCalculator
class RkMichaelisMentenTwoCompBolusMicro : public RkMichaelisMentenTwoComp
{
    INTAKEINTERVALCALCULATOR_UTILS(RkMichaelisMentenTwoCompBolusMicro)
public:
    /// \brief Constructor
    RkMichaelisMentenTwoCompBolusMicro();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;

    void initConcentrations(const Residuals& _inResiduals, MultiCompConcentration& _concentrations) override
    {
        _concentrations[0] = _inResiduals[0] + m_D / m_V1;
        _concentrations[1] = _inResiduals[1];
        _concentrations[2] = _inResiduals[2];
    }
};

/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment extravascular algorithm
/// \sa IntakeIntervalCalculator
class RkMichaelisMentenTwoCompInfusionMicro : public RkMichaelisMentenTwoComp
{
    INTAKEINTERVALCALCULATOR_UTILS(RkMichaelisMentenTwoCompInfusionMicro)
public:
    /// \brief Constructor
    RkMichaelisMentenTwoCompInfusionMicro();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;

    void initConcentrations(const Residuals& _inResiduals, MultiCompConcentration& _concentrations) override
    {
        m_infusionRate = m_D / m_V1 / m_Tinf;
        _concentrations[0] = _inResiduals[0];
        _concentrations[1] = _inResiduals[1];
        _concentrations[2] = _inResiduals[2];
    }
};



/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment extravascular algorithm
/// \sa IntakeIntervalCalculator
class RkMichaelisMentenTwoCompExtraMacro : public RkMichaelisMentenTwoCompExtraMicro
{
    INTAKEINTERVALCALCULATOR_UTILS(RkMichaelisMentenTwoCompExtraMacro)
public:
    /// \brief Constructor
    RkMichaelisMentenTwoCompExtraMacro();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;
};


/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment extravascular algorithm
/// \sa IntakeIntervalCalculator
class RkMichaelisMentenTwoCompExtraLagMacro : public RkMichaelisMentenTwoCompExtraLagMicro
{
    INTAKEINTERVALCALCULATOR_UTILS(RkMichaelisMentenTwoCompExtraLagMacro)
public:
    /// \brief Constructor
    RkMichaelisMentenTwoCompExtraLagMacro();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;
};


/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment extravascular algorithm
/// \sa IntakeIntervalCalculator
class RkMichaelisMentenTwoCompBolusMacro : public RkMichaelisMentenTwoCompBolusMicro
{
    INTAKEINTERVALCALCULATOR_UTILS(RkMichaelisMentenTwoCompBolusMacro)
public:
    /// \brief Constructor
    RkMichaelisMentenTwoCompBolusMacro();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;
};

/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment extravascular algorithm
/// \sa IntakeIntervalCalculator
class RkMichaelisMentenTwoCompInfusionMacro : public RkMichaelisMentenTwoCompInfusionMicro
{
    INTAKEINTERVALCALCULATOR_UTILS(RkMichaelisMentenTwoCompInfusionMacro)
public:
    /// \brief Constructor
    RkMichaelisMentenTwoCompInfusionMacro();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;
};

} // namespace Core
} // namespace Tucuxi


#endif // TUCUXI_CORE_RKMICHAELISMENTENTWOCOMP_H
