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


#ifndef TUCUXI_CORE_ONECOMPARTMENTEXTRA_H
#define TUCUXI_CORE_ONECOMPARTMENTEXTRA_H

#include "tucucore/intakeintervalcalculatoranalytical.h"

namespace Tucuxi {
namespace Core {

enum class OneCompartmentExtraExponentials : int
{
    Ke = 0,
    Ka
};

enum class OneCompartmentExtraCompartments : int
{
    First = 0,
    Second
};

/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment extravascular algorithm
/// \sa IntakeIntervalCalculator
class OneCompartmentExtraMicro : public IntakeIntervalCalculatorBase<2, OneCompartmentExtraExponentials>
{
    INTAKEINTERVALCALCULATOR_UTILS(OneCompartmentExtraMicro)
public:
    /// \brief Constructor
    OneCompartmentExtraMicro();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

    typedef OneCompartmentExtraExponentials Exponentials;

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

    void compute(const Residuals& _inResiduals, Eigen::VectorXd& _concentrations1, Eigen::VectorXd& _concentrations2);

    Value m_D{NAN};  /// Quantity of drug
    Value m_F{NAN};  /// bioavailability
    Value m_Ka{NAN}; /// Absorption rate constant
    Value m_V{NAN};  /// Volume of the compartment
    Value m_Ke{
            NAN}; /// Elimination constant rate = Cl/V where Cl is the clearance and V is the volume of the compartment
    Value m_Int{NAN};           /// Interval time (Hours)
    Eigen::Index m_nbPoints{0}; /// Number measure points during interval

private:
    typedef OneCompartmentExtraCompartments Compartments;
};

inline void OneCompartmentExtraMicro::compute(
        const Residuals& _inResiduals, Eigen::VectorXd& _concentrations1, Eigen::VectorXd& _concentrations2)
{
    Concentration resid1 = _inResiduals[0];
    Concentration resid2 = _inResiduals[1] + m_F * m_D / m_V;
    Concentration part2 = m_Ka * resid2 / (-m_Ka + m_Ke);

    _concentrations1 = exponentials(Exponentials::Ke) * resid1
                       + (exponentials(Exponentials::Ka) - exponentials(Exponentials::Ke)) * part2;
    _concentrations2 = resid2 * exponentials(Exponentials::Ka);
}

class OneCompartmentExtraMacro : public OneCompartmentExtraMicro
{
    INTAKEINTERVALCALCULATOR_UTILS(OneCompartmentExtraMacro)
public:
    OneCompartmentExtraMacro();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;
};

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_ONECOMPARTMENTEXTRA_H
