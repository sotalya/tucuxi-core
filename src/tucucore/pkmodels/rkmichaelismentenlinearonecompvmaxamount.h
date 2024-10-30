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


#ifndef TUCUXI_CORE_RKMICHAELISMENTENLINEARONECOMPVMAXAMOUNT_H
#define TUCUXI_CORE_RKMICHAELISMENTENLINEARONECOMPVMAXAMOUNT_H

#include "tucucore/intakeintervalcalculatorrk4.h"

namespace Tucuxi {
namespace Core {


enum class RkMichaelisMentenLinearOneCompVmaxAmountCompartments : int
{
    First = 0,
    Second
};

/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment extravascular algorithm
/// \sa IntakeIntervalCalculator
class RkMichaelisMentenLinearOneCompVmaxAmount :
    public IntakeIntervalCalculatorRK4Base<2, RkMichaelisMentenLinearOneCompVmaxAmount>
{

public:
    /// \brief Constructor
    RkMichaelisMentenLinearOneCompVmaxAmount();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

    inline void derive(double _t, const Compartments_t& _c, Compartments_t& _dcdt)
    {
        FINAL_UNUSED_PARAMETER(_t);

        // This version considers VMax to be concentration/time
        _dcdt[0] = m_Ka * _c[1] - m_Ke * _c[0] - m_Vmax * _c[0] / m_V / (m_Km + _c[0]);
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
        _dcdt[0] = m_Ka * _c[1] - m_Ke * _c[0] - m_Vmax * _c[0] / m_V / (m_Km + _c[0]);
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
    typedef RkMichaelisMentenLinearOneCompVmaxAmountCompartments Compartments;
};



/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment extravascular algorithm
/// \sa IntakeIntervalCalculator
class RkMichaelisMentenLinearOneCompVmaxAmountExtraMicro : public RkMichaelisMentenLinearOneCompVmaxAmount
{
    INTAKEINTERVALCALCULATOR_UTILS(RkMichaelisMentenLinearOneCompVmaxAmountExtraMicro)
public:
    /// \brief Constructor
    RkMichaelisMentenLinearOneCompVmaxAmountExtraMicro();

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
class RkMichaelisMentenLinearOneCompVmaxAmountExtraLagMicro : public RkMichaelisMentenLinearOneCompVmaxAmount
{
    INTAKEINTERVALCALCULATOR_UTILS(RkMichaelisMentenLinearOneCompVmaxAmountExtraLagMicro)
public:
    /// \brief Constructor
    RkMichaelisMentenLinearOneCompVmaxAmountExtraLagMicro();

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
class RkMichaelisMentenLinearOneCompVmaxAmountBolusMicro : public RkMichaelisMentenLinearOneCompVmaxAmount
{
    INTAKEINTERVALCALCULATOR_UTILS(RkMichaelisMentenLinearOneCompVmaxAmountBolusMicro)
public:
    /// \brief Constructor
    RkMichaelisMentenLinearOneCompVmaxAmountBolusMicro();

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
class RkMichaelisMentenLinearOneCompVmaxAmountInfusionMicro : public RkMichaelisMentenLinearOneCompVmaxAmount
{
    INTAKEINTERVALCALCULATOR_UTILS(RkMichaelisMentenLinearOneCompVmaxAmountInfusionMicro)
public:
    /// \brief Constructor
    RkMichaelisMentenLinearOneCompVmaxAmountInfusionMicro();

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
class RkMichaelisMentenLinearOneCompVmaxAmountExtraMacro : public RkMichaelisMentenLinearOneCompVmaxAmountExtraMicro
{
    INTAKEINTERVALCALCULATOR_UTILS(RkMichaelisMentenLinearOneCompVmaxAmountExtraMacro)
public:
    /// \brief Constructor
    RkMichaelisMentenLinearOneCompVmaxAmountExtraMacro();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;
};


/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment extravascular algorithm
/// \sa IntakeIntervalCalculator
class RkMichaelisMentenLinearOneCompVmaxAmountExtraLagMacro :
    public RkMichaelisMentenLinearOneCompVmaxAmountExtraLagMicro
{
    INTAKEINTERVALCALCULATOR_UTILS(RkMichaelisMentenLinearOneCompVmaxAmountExtraLagMacro)
public:
    /// \brief Constructor
    RkMichaelisMentenLinearOneCompVmaxAmountExtraLagMacro();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;
};


/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment extravascular algorithm
/// \sa IntakeIntervalCalculator
class RkMichaelisMentenLinearOneCompVmaxAmountBolusMacro : public RkMichaelisMentenLinearOneCompVmaxAmountBolusMicro
{
    INTAKEINTERVALCALCULATOR_UTILS(RkMichaelisMentenLinearOneCompVmaxAmountBolusMacro)
public:
    /// \brief Constructor
    RkMichaelisMentenLinearOneCompVmaxAmountBolusMacro();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;
};

/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment extravascular algorithm
/// \sa IntakeIntervalCalculator
class RkMichaelisMentenLinearOneCompVmaxAmountInfusionMacro :
    public RkMichaelisMentenLinearOneCompVmaxAmountInfusionMicro
{
    INTAKEINTERVALCALCULATOR_UTILS(RkMichaelisMentenLinearOneCompVmaxAmountInfusionMacro)
public:
    /// \brief Constructor
    RkMichaelisMentenLinearOneCompVmaxAmountInfusionMacro();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;
};

} // namespace Core
} // namespace Tucuxi


#endif // TUCUXI_CORE_RKMICHAELISMENTELINEARNONECOMPVMAXAMOUNT_H
