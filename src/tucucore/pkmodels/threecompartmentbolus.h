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


#ifndef TUCUXI_CORE_THREECOMPARTMENTBOLUS_H
#define TUCUXI_CORE_THREECOMPARTMENTBOLUS_H

#include "tucucore/intakeintervalcalculatoranalytical.h"

namespace Tucuxi {
namespace Core {

enum class ThreeCompartmentBolusExponentials : int
{
    Alpha,
    Beta,
    Gamma
};

enum class ThreeCompartmentBolusCompartments : int
{
    First,
    Second,
    Third
};

/// \ingroup TucuCore
/// \brief Intake interval calculator for the three compartment bolus algorithm
/// \sa IntakeIntervalCalculator
class ThreeCompartmentBolusMicro : public IntakeIntervalCalculatorBase<3, ThreeCompartmentBolusExponentials>
{
    INTAKEINTERVALCALCULATOR_UTILS(ThreeCompartmentBolusMicro)
public:
    /// \brief Constructor
    ThreeCompartmentBolusMicro();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

    typedef ThreeCompartmentBolusExponentials Exponentials;

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;

    void computeExponentials(Eigen::VectorXd& _times) override;

    bool computeConcentrations(
            const Residuals& _inResiduals,
            bool _isAll,
            MultiCompConcentrations& _concentrations,
            Residuals& _outResiduals) override;

    bool computeConcentration(
            const Value& _atTime,
            const Residuals& _inResiduals,
            bool _isAll,
            MultiCompConcentrations& _concentrations,
            Residuals& _outResiduals) override;

    void compute(
            const Residuals& _inResiduals,
            Eigen::VectorXd& _concentrations1,
            Value& _concentrations2,
            Value& _concentrations3);

    Value m_D{NAN};  /// Quantity of drug
    Value m_F{NAN};  /// ???
    Value m_V1{NAN}; /// Volume of the compartment 1
    Value m_Ke{
            NAN}; /// Elimination constant rate = Cl/V1 where Cl is the clearance and V1 is the volume of the compartment 1
    Value m_K12{NAN}; /// Q/V1
    Value m_K21{NAN}; /// Q/V2
    Value m_K13{NAN}; /// Q/V1
    Value m_K31{NAN}; /// Q/V2
    Value m_Alpha{NAN};
    Value m_Beta{NAN};
    Value m_Gamma{NAN};
    Eigen::Index m_nbPoints{0}; /// number measure points during interval
    Value m_Int{NAN};           /// Interval (hours)

private:
    typedef ThreeCompartmentBolusCompartments Compartments;
};

inline void ThreeCompartmentBolusMicro::compute(
        const Residuals& _inResiduals,
        Eigen::VectorXd& _concentrations1,
        Value& _concentrations2,
        Value& _concentrations3)
{
    Concentration resid1 = _inResiduals[0] + m_F * m_D / m_V1;
    Concentration resid2 = _inResiduals[1];
    Concentration resid3 = _inResiduals[2];

    Value A = (1 / m_V1) * (m_K21 - m_Alpha) * (m_K31 - m_Alpha) / (m_Alpha - m_Beta)
              / (m_Alpha - m_Gamma); // NOLINT(readability-identifier-naming)
    Value B = (1 / m_V1) * (m_K21 - m_Beta) * (m_K31 - m_Beta) / (m_Beta - m_Alpha)
              / (m_Beta - m_Gamma); // NOLINT(readability-identifier-naming)
    Value C = (1 / m_V1) * (m_K21 - m_Gamma) * (m_K31 - m_Gamma) / (m_Gamma - m_Beta)
              / (m_Gamma - m_Alpha);          // NOLINT(readability-identifier-naming)
    Value A2 = m_K12 / (m_K21 - m_Alpha) * A; // NOLINT(readability-identifier-naming)
    Value B2 = m_K12 / (m_K21 - m_Beta) * B;  // NOLINT(readability-identifier-naming)
    Value C2 = m_K12 / (m_K21 - m_Gamma) * C; // NOLINT(readability-identifier-naming)
    Value A3 = m_K13 / (m_K31 - m_Alpha) * A; // NOLINT(readability-identifier-naming)
    Value B3 = m_K13 / (m_K31 - m_Beta) * B;  // NOLINT(readability-identifier-naming)
    Value C3 = m_K13 / (m_K31 - m_Gamma) * C; // NOLINT(readability-identifier-naming)

    // Calculate concentrations for comp1, comp2 and comp3
    _concentrations1 = resid1
                       * (B * exponentials(Exponentials::Beta) + A * exponentials(Exponentials::Alpha)
                          + C * exponentials(Exponentials::Gamma));

    // Do NOT use m_nbPoints because in case of single calculation "m_nbPoints = 0"
    _concentrations2 =
            resid2
            + resid1
                      * (B2 * exponentials(Exponentials::Beta)((exponentials(Exponentials::Beta)).size() - 1)
                         + A2 * exponentials(Exponentials::Alpha)((exponentials(Exponentials::Alpha)).size() - 1)
                         + C2 * exponentials(Exponentials::Gamma)((exponentials(Exponentials::Gamma)).size() - 1));

    _concentrations3 =
            resid3
            + resid1
                      * (B3 * exponentials(Exponentials::Beta)((exponentials(Exponentials::Beta)).size() - 1)
                         + A3 * exponentials(Exponentials::Alpha)((exponentials(Exponentials::Alpha)).size() - 1)
                         + C3 * exponentials(Exponentials::Gamma)((exponentials(Exponentials::Gamma)).size() - 1));
}

class ThreeCompartmentBolusMacro : public ThreeCompartmentBolusMicro
{
    INTAKEINTERVALCALCULATOR_UTILS(ThreeCompartmentBolusMacro)
public:
    ThreeCompartmentBolusMacro();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;
};

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_THREECOMPARTMENTBOLUS_H
