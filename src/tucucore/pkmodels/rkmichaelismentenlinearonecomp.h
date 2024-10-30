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


#ifndef TUCUXI_CORE_RKMICHAELISMENTENLINEARONECOMP_H
#define TUCUXI_CORE_RKMICHAELISMENTENLINEARONECOMP_H

#include "tucucore/intakeintervalcalculatorrk4.h"

namespace Tucuxi {
namespace Core {


enum class RkMichaelisMentenLinearOneCompCompartments : int
{
    First = 0,
    Second
};

/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment extravascular algorithm
/// \sa IntakeIntervalCalculator
class RkMichaelisMentenLinearOneComp : public IntakeIntervalCalculatorRK4Base<2, RkMichaelisMentenLinearOneComp>
{

public:
    /// \brief Constructor
    RkMichaelisMentenLinearOneComp();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

    inline void derive(double _t, const Compartments_t& _c, Compartments_t& _dcdt)
    {
        // This version considers VMax to be concentration/time
        _dcdt[0] = m_Ka * _c[1] - m_Ke * _c[0] - m_Vmax * _c[0] / (m_Km + _c[0]);
        _dcdt[1] = -m_Ka * _c[1];

        if (m_isInfusion) {
            if (_t < m_TinfLow) {
                _dcdt[0] += m_infusionRate;
            }
        }
    }


    inline void deriveAtPotentialInfusionStop(double _t, const Compartments_t& _c, Compartments_t& _dcdt)
    {
        // This version considers VMax to be concentration/time
        _dcdt[0] = m_Ka * _c[1] - m_Ke * _c[0] - m_Vmax * _c[0] / (m_Km + _c[0]);
        _dcdt[1] = -m_Ka * _c[1];

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
                _concentrations[1] += m_D / m_V * m_F;
                m_delivered = true;
            }
        }
    }

protected:
    Value m_D{NAN};  /// Quantity of drug
    Value m_F{1.0};  /// bioavailability
    Value m_Ka{0.0}; /// Absorption rate constant
    Value m_V{NAN};  /// Volume of the compartment
    Value m_Ke{NAN};
    Value m_Km{NAN};
    Value m_Vmax{NAN};
    Value m_Tinf{NAN};
    Value m_TinfLow{NAN};
    Value m_TinfHigh{NAN};
    Value m_Tlag{NAN};
    Value m_infusionRate{0};
    bool m_delivered{false};
    bool m_isInfusion{false};
    bool m_isWithLag{false};

private:
    typedef RkMichaelisMentenLinearOneCompCompartments Compartments;
};



/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment extravascular algorithm
/// \sa IntakeIntervalCalculator
class RkMichaelisMentenLinearOneCompExtraMicro : public RkMichaelisMentenLinearOneComp
{
    INTAKEINTERVALCALCULATOR_UTILS(RkMichaelisMentenLinearOneCompExtraMicro)
public:
    /// \brief Constructor
    RkMichaelisMentenLinearOneCompExtraMicro();

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
class RkMichaelisMentenLinearOneCompExtraLagMicro : public RkMichaelisMentenLinearOneComp
{
    INTAKEINTERVALCALCULATOR_UTILS(RkMichaelisMentenLinearOneCompExtraLagMicro)
public:
    /// \brief Constructor
    RkMichaelisMentenLinearOneCompExtraLagMicro();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;

    void initConcentrations(const Residuals& _inResiduals, MultiCompConcentration& _concentrations) override
    {
        _concentrations[0] = _inResiduals[0];
        _concentrations[1] = _inResiduals[1];
        // Do not forget to reinitialize the flag for delivery of the drug
        m_delivered = false;
    }
};


/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment extravascular algorithm
/// \sa IntakeIntervalCalculator
class RkMichaelisMentenLinearOneCompBolusMicro : public RkMichaelisMentenLinearOneComp
{
    INTAKEINTERVALCALCULATOR_UTILS(RkMichaelisMentenLinearOneCompBolusMicro)
public:
    /// \brief Constructor
    RkMichaelisMentenLinearOneCompBolusMicro();

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
class RkMichaelisMentenLinearOneCompInfusionMicro : public RkMichaelisMentenLinearOneComp
{
    INTAKEINTERVALCALCULATOR_UTILS(RkMichaelisMentenLinearOneCompInfusionMicro)
public:
    /// \brief Constructor
    RkMichaelisMentenLinearOneCompInfusionMicro();

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



/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment extravascular algorithm
/// \sa IntakeIntervalCalculator
class RkMichaelisMentenLinearOneCompExtraMacro : public RkMichaelisMentenLinearOneCompExtraMicro
{
    INTAKEINTERVALCALCULATOR_UTILS(RkMichaelisMentenLinearOneCompExtraMacro)
public:
    /// \brief Constructor
    RkMichaelisMentenLinearOneCompExtraMacro();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;
};


/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment extravascular algorithm
/// \sa IntakeIntervalCalculator
class RkMichaelisMentenLinearOneCompExtraLagMacro : public RkMichaelisMentenLinearOneCompExtraLagMicro
{
    INTAKEINTERVALCALCULATOR_UTILS(RkMichaelisMentenLinearOneCompExtraLagMacro)
public:
    /// \brief Constructor
    RkMichaelisMentenLinearOneCompExtraLagMacro();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;
};


/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment extravascular algorithm
/// \sa IntakeIntervalCalculator
class RkMichaelisMentenLinearOneCompBolusMacro : public RkMichaelisMentenLinearOneCompBolusMicro
{
    INTAKEINTERVALCALCULATOR_UTILS(RkMichaelisMentenLinearOneCompBolusMacro)
public:
    /// \brief Constructor
    RkMichaelisMentenLinearOneCompBolusMacro();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;
};

/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment extravascular algorithm
/// \sa IntakeIntervalCalculator
class RkMichaelisMentenLinearOneCompInfusionMacro : public RkMichaelisMentenLinearOneCompInfusionMicro
{
    INTAKEINTERVALCALCULATOR_UTILS(RkMichaelisMentenLinearOneCompInfusionMacro)
public:
    /// \brief Constructor
    RkMichaelisMentenLinearOneCompInfusionMacro();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;
};

} // namespace Core
} // namespace Tucuxi


#endif // TUCUXI_CORE_RKMICHAELISMENTELINEARNONECOMP_H
