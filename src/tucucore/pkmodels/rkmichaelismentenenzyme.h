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


#ifndef TUCUXI_CORE_RKMICHAELISMENTENENZYME_H
#define TUCUXI_CORE_RKMICHAELISMENTENENZYME_H

#include "tucucore/intakeintervalcalculatorrk4.h"

namespace Tucuxi {
namespace Core {


enum class RkMichaelisMentenEnzymeCompartments : int
{
    First = 0,
    Second,
    Enzyme
};

class CalculationException : public std::exception
{
    const char* what() const throw() override
    {
        return "Computation value NaN in RkMichaelisMentenEnzyme calculator";
    }
};

/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment extravascular algorithm
/// \sa IntakeIntervalCalculator
class RkMichaelisMentenEnzyme : public IntakeIntervalCalculatorRK4Base<3, RkMichaelisMentenEnzyme>
{

public:
    /// \brief Constructor
    RkMichaelisMentenEnzyme();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

    inline void derive(double _t, const Compartments_t& _c, Compartments_t& _dcdt)
    {
        //auto c0 = _c[0] / 1000.0 * m_V;
        //auto cp = c0 / m_V;
        // We get concentration 0 in mg/l instead of ug/l
        //        auto cp = _c[0] / 1000.0;

        // Store local variables to avoid multiple access to the same memory
        auto c0 = _c[0];
        auto c1 = _c[1];
        auto c2 = _c[2];
        auto kaC1 = m_Ka * c1;
        auto den0 = 1 / (m_Km + c0);
        auto ktt = m_ktr * _t;
        auto den2 = 1 / (m_ECmid + c0);

        // Compute the derivatives with simplified expressions
        _dcdt[0] = kaC1 - m_Vmax * c0 * m_AllmCL * c2 * den0; // / m_V;
        if (m_MTT == 0.0) {
            _dcdt[1] = -kaC1;
        }
        else {
            _dcdt[1] = -kaC1 + fastExpApproximation(std::fma(m_NN, std::log(ktt), m_cumul - ktt));
        }
        _dcdt[2] = m_Kenz * (1 - c2 + (c0 * m_Emax * den2));

        // Here we modify the derivative on the central compartment, in order
        // to consider it in ug/l     (1000.0 for ug, and /m_V for concentration)
        //_dcdt[0] *= 1000.0 / m_V;
        _dcdt[0] *= 1.0 / m_V;
        if (std::isnan(_dcdt[0])) {
            throw CalculationException();
        }
        if (std::isnan(_dcdt[1])) {
            throw CalculationException();
        }
        if (std::isnan(_dcdt[2])) {
            throw CalculationException();
        }
        //assert(!std::isnan(_dcdt[0]));
        //assert(!std::isnan(_dcdt[1]));
        //assert(!std::isnan(_dcdt[2]));
    }


    inline void addFixedValue(double _t, Compartments_t& _concentrations)
    {
        FINAL_UNUSED_PARAMETER(_t);
        FINAL_UNUSED_PARAMETER(_concentrations);
    }

    // Fast exp approximation
    inline double fastExpApproximation(double _x)
    {
        constexpr double log2e = 1.4426950408889634; // log2(e)
        return std::exp2(_x * log2e);
    }

protected:
    Value m_D{NAN};  /// Quantity of drug
    Value m_F{NAN};  /// bioavailability
    Value m_Ka{NAN}; /// Absorption rate constant
    Value m_V{NAN};  /// Volume of the compartment
    Value m_Km{NAN};
    Value m_Vmax{NAN};
    Value m_AllmCL{NAN}; /// Allometric clearance
    Value m_Tinf{NAN};
    Value m_TinfLow{NAN};
    Value m_TinfHigh{NAN};

    Value m_Kenz{NAN};
    Value m_Emax{NAN};
    Value m_ECmid{NAN};
    Value m_EDmid{NAN};
    Value m_DoseMid{NAN};
    Value m_Fmax{NAN};
    Value m_NN{NAN};
    Value m_MTT{NAN};

    Value m_cumul{NAN};
    Value m_ktr{NAN};

private:
    typedef RkMichaelisMentenEnzymeCompartments Compartments;
};



/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment extravascular algorithm
/// \sa IntakeIntervalCalculator
class RkMichaelisMentenEnzymeExtra : public RkMichaelisMentenEnzyme
{
    INTAKEINTERVALCALCULATOR_UTILS(RkMichaelisMentenEnzymeExtra)
public:
    /// \brief Constructor
    RkMichaelisMentenEnzymeExtra();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;

    void initConcentrations(const Residuals& _inResiduals, MultiCompConcentration& _concentrations) override
    {
        _concentrations[0] = _inResiduals[0];
        _concentrations[1] = _inResiduals[1];
        if (m_D > 0.0) {
            if (_inResiduals[2] == 0.0) {
                // First dose, we need an induction value
                _concentrations[2] = 1.0;
            }
            else {
                // Following dose
                //_concentrations[2] = 0.0;
                _concentrations[2] = _inResiduals[2];
            }
        }
        else {
            _concentrations[2] = _inResiduals[2];
        }
        // TODO : Check if this induction should be set to 1 everytime,
        //        or only for the first dose
    }
};

/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment extravascular algorithm
/// \sa IntakeIntervalCalculator
class RkMichaelisMentenEnzymeBolus : public RkMichaelisMentenEnzyme
{
    INTAKEINTERVALCALCULATOR_UTILS(RkMichaelisMentenEnzymeBolus)
public:
    /// \brief Constructor
    RkMichaelisMentenEnzymeBolus();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;

    void initConcentrations(const Residuals& _inResiduals, MultiCompConcentration& _concentrations) override
    {
        _concentrations[0] = _inResiduals[0] + m_D * m_F / m_V;
        _concentrations[1] = _inResiduals[1];
        if (m_D > 0.0) {
            _concentrations[2] = _inResiduals[2] + 1.0;
        }
        else {
            _concentrations[2] = _inResiduals[2];
        }
    }
};

/*
/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment extravascular algorithm
/// \sa IntakeIntervalCalculator
class RkMichaelisMentenEnzymeInfusion : public RkMichaelisMentenEnzyme
{
    INTAKEINTERVALCALCULATOR_UTILS(RkMichaelisMentenEnzymeInfusion)
public:
    /// \brief Constructor
    RkMichaelisMentenEnzymeInfusion();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;

    void initConcentrations (const Residuals& _inResiduals, Compartments_t& _concentrations) override
    {
        _concentrations[0] = _inResiduals[0];
        _concentrations[1] = _inResiduals[1];

    }

};
*/


} // namespace Core
} // namespace Tucuxi


#endif // TUCUXI_CORE_RKMICHAELISMENTENENZYME_H
