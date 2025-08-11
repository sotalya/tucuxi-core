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


#ifndef TUCUXI_CORE_ONECOMPARTMENTINFUSION_H
#define TUCUXI_CORE_ONECOMPARTMENTINFUSION_H

#include "tucucore/intakeintervalcalculatoranalytical.h"

namespace Tucuxi {
namespace Core {

enum class OneCompartmentInfusionExponentials : int
{
    Ke
};

enum class OneCompartmentInfusionCompartments : int
{
    First
};

/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment extravascular algorithm
/// \sa IntakeIntervalCalculator
class OneCompartmentInfusionMicro : public IntakeIntervalCalculatorBase<1, OneCompartmentInfusionExponentials>
{
    INTAKEINTERVALCALCULATOR_UTILS(OneCompartmentInfusionMicro)
public:
    /// \brief Constructor
    OneCompartmentInfusionMicro();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

    typedef OneCompartmentInfusionExponentials Exponentials;

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

    void compute(const Residuals& _inResiduals, int _forceSize, Eigen::VectorXd& _concentrations);


    Value m_D{NAN};  /// Quantity of drug
    Value m_Cl{NAN}; /// Clearance
    Value m_V{NAN};  /// Volume of the compartment
    Value m_Ke{
            NAN}; /// Elimination constant rate = Cl/V where Cl is the clearance and V is the volume of the compartment
    Value m_Tinf{NAN};          /// Infusion time (hours)
    Value m_Int{NAN};           /// Interval (hours)
    Eigen::Index m_nbPoints{0}; /// number measure points during interval

    bool m_timeMaxHigherThanTinf{true};

private:
    typedef OneCompartmentInfusionCompartments Compartments;
};

inline void OneCompartmentInfusionMicro::compute(
        const Residuals& _inResiduals, int _forceSize, Eigen::VectorXd& _concentrations)
{
    //    Concentration part1 = m_D / (m_Tinf * m_Cl);
    Concentration part1 = m_D / (m_Tinf * m_Ke * m_V);

    // Calculate concentrations
    _concentrations = Eigen::VectorXd::Constant(exponentials(Exponentials::Ke).size(), _inResiduals[0]);
    _concentrations = _concentrations.cwiseProduct(exponentials(Exponentials::Ke));

    if (_forceSize != 0) {
        _concentrations.head(_forceSize) =
                _concentrations.head(_forceSize)
                + part1 * (1.0 - exponentials(Exponentials::Ke).head(_forceSize).array()).matrix();
    }

    int therest = static_cast<int>(_concentrations.size() - _forceSize);
    _concentrations.tail(therest) = _concentrations.tail(therest)
                                    + part1 * (exp(m_Ke * m_Tinf) - 1) * exponentials(Exponentials::Ke).tail(therest);
}

class OneCompartmentInfusionMacro : public OneCompartmentInfusionMicro
{
    INTAKEINTERVALCALCULATOR_UTILS(OneCompartmentInfusionMacro)
public:
    OneCompartmentInfusionMacro();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;
};


} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_ONECOMPARTMENTINFUSION_H
