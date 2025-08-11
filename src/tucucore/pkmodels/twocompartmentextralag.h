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


#ifndef TUCUXI_CORE_TWOCOMPARTMENTEXTRALAG_H
#define TUCUXI_CORE_TWOCOMPARTMENTEXTRALAG_H

#include "tucucore/intakeintervalcalculatoranalytical.h"

namespace Tucuxi {
namespace Core {

enum class TwoCompartmentExtraLagExponentials : int
{
    Alpha,
    Beta,
    Ka,
    AlphaPost,
    BetaPost,
    KaPost
};

enum class TwoCompartmentExtraLagCompartments : int
{
    First,
    Second,
    Third
};

/// \ingroup TucuCore
/// \brief Intake interval calculator for the two compartment extra algorithm
/// \sa IntakeIntervalCalculator
class TwoCompartmentExtraLagMicro : public IntakeIntervalCalculatorBase<3, TwoCompartmentExtraLagExponentials>
{
    INTAKEINTERVALCALCULATOR_UTILS(TwoCompartmentExtraLagMicro)
public:
    /// \brief Constructor
    TwoCompartmentExtraLagMicro();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

    typedef TwoCompartmentExtraLagExponentials Exponentials;

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

    bool compute(
            const Residuals& _inResiduals,
            Eigen::VectorXd& _concentrations1,
            Eigen::VectorXd& _concentrations2,
            Eigen::VectorXd& _concentrations3);

    Value m_D{NAN};  /// Quantity of drug
    Value m_F{NAN};  /// bioavailability
    Value m_V1{NAN}; /// Volume1
    Value m_Ka{NAN}; /// Absorption rate constant
    Value m_Ke{
            NAN}; /// Elimination constant rate = Cl/V1 where Cl is the clearance and V1 is the volume of the compartment 1
    Value m_K12{NAN};            /// Q/V1
    Value m_K21{NAN};            /// Q/V2
    Value m_Tlag{NAN};           /// Lag time (in hours)
    Value m_RootK{NAN};          /// sqrt(sumK*sumK - 4*K21*Ke)
    Value m_Alpha{NAN};          /// (sumK + root)/2
    Value m_Beta{NAN};           /// (sumK - root)/2
    Value m_Int{NAN};            /// Interval (hours)
    Eigen::Index m_nbPoints{0};  /// number measure points during interval
    Eigen::Index m_nbPoints0{0}; /// Number of points during lag time
    Eigen::Index m_nbPoints1{0}; /// Number of points after lag time

private:
    typedef TwoCompartmentExtraLagCompartments Compartments;
};



class TwoCompartmentExtraLagMacro : public TwoCompartmentExtraLagMicro
{
    INTAKEINTERVALCALCULATOR_UTILS(TwoCompartmentExtraLagMacro)
public:
    TwoCompartmentExtraLagMacro();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;
};



class TwoCompartmentExtraLagMacroRatios : public TwoCompartmentExtraLagMacro
{
    INTAKEINTERVALCALCULATOR_UTILS(TwoCompartmentExtraLagMacroRatios)
public:
    TwoCompartmentExtraLagMacroRatios() = default;

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;
};

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_TWOCOMPARTMENTEXTRALAG_H
