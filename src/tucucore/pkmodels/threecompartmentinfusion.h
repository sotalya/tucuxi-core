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


#ifndef TUCUXI_CORE_THREECOMPARTMENTINFUSION_H
#define TUCUXI_CORE_THREECOMPARTMENTINFUSION_H

#include "tucucore/intakeintervalcalculatoranalytical.h"

namespace Tucuxi {
namespace Core {

enum class ThreeCompartmentInfusionExponentials : int
{
    Alpha,
    Beta,
    Gamma
};

enum class ThreeCompartmentInfusionCompartments : int
{
    First,
    Second,
    Third
};

/// \ingroup TucuCore
/// \brief Intake interval calculator for the three compartment infusion algorithm
/// \sa IntakeIntervalCalculator
class ThreeCompartmentInfusionMicro : public IntakeIntervalCalculatorBase<3, ThreeCompartmentInfusionExponentials>
{
    INTAKEINTERVALCALCULATOR_UTILS(ThreeCompartmentInfusionMicro)
public:
    /// \brief Constructor
    ThreeCompartmentInfusionMicro();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

    typedef ThreeCompartmentInfusionExponentials Exponentials;

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

    void compute(int _forceSize, Eigen::VectorXd& _concentrations1, Value& _concentrations2, Value& _concentrations3);

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
    Value m_Tinf{NAN};          /// Infusion time (hours)
    Value m_Int{NAN};           /// Interval (hours)
    Eigen::Index m_nbPoints{0}; /// number measure points during interval

private:
    typedef ThreeCompartmentInfusionCompartments Compartments;
};

inline void ThreeCompartmentInfusionMicro::compute(
        int _forceSize, Eigen::VectorXd& _concentrations1, Value& _concentrations2, Value& _concentrations3)
{
    const Eigen::VectorXd& alphaLogV = exponentials(Exponentials::Alpha);
    const Eigen::VectorXd& betaLogV = exponentials(Exponentials::Beta);
    const Eigen::VectorXd& gammaLogV = exponentials(Exponentials::Gamma);

    Value deltaD = (m_D / m_V1) / m_Tinf;
    Value alphaTinf = std::exp(-m_Alpha * m_Tinf);
    Value betaTinf = std::exp(-m_Beta * m_Tinf);
    Value gammaTinf = std::exp(-m_Gamma * m_Tinf);

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

    if (_forceSize != 0) {
        _concentrations1.head(_forceSize) =
                deltaD
                * (A / m_Alpha * (1 * Eigen::VectorXd::Ones(_forceSize) - alphaLogV.head(_forceSize))
                   + B / m_Beta * (1 * Eigen::VectorXd::Ones(_forceSize) - betaLogV.head(_forceSize))
                   + C / m_Gamma * (1 * Eigen::VectorXd::Ones(_forceSize) - gammaLogV.head(_forceSize)));
    }

    int therest = static_cast<int>(alphaLogV.size() - _forceSize);

    _concentrations1.tail(therest) = deltaD
                                     * (A / m_Alpha * (1 - alphaTinf) * alphaLogV.tail(therest) / alphaTinf
                                        + B / m_Beta * (1 - betaTinf) * betaLogV.tail(therest) / betaTinf
                                        + C / m_Gamma * (1 - gammaTinf) * gammaLogV.tail(therest) / gammaTinf);

    // Do NOT use m_nbPoints because in case of single calculation "m_nbPoints = 0"
    _concentrations2 = deltaD
                       * (A2 / m_Alpha * (1 - alphaTinf) * alphaLogV(alphaLogV.size() - 1) / alphaTinf
                          + B2 / m_Beta * (1 - betaTinf) * betaLogV(betaLogV.size() - 1) / betaTinf
                          + C2 / m_Gamma * (1 - gammaTinf) * gammaLogV(gammaLogV.size() - 1) / gammaTinf);

    _concentrations3 = deltaD
                       * (A3 / m_Alpha * (1 - alphaTinf) * alphaLogV(alphaLogV.size() - 1) / alphaTinf
                          + B3 / m_Beta * (1 - betaTinf) * betaLogV(betaLogV.size() - 1) / betaTinf
                          + C3 / m_Gamma * (1 - gammaTinf) * gammaLogV(gammaLogV.size() - 1) / gammaTinf);
}

class ThreeCompartmentInfusionMacro : public ThreeCompartmentInfusionMicro
{
    INTAKEINTERVALCALCULATOR_UTILS(ThreeCompartmentInfusionMacro)
public:
    ThreeCompartmentInfusionMacro();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;
};

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_THREECOMPARTMENTINFUSION_H
