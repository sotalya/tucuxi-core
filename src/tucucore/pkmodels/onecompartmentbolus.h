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


#ifndef TUCUXI_CORE_ONECOMPARTMENTBOLUS_H
#define TUCUXI_CORE_ONECOMPARTMENTBOLUS_H

#include "tucucore/intakeintervalcalculatoranalytical.h"

namespace Tucuxi {
namespace Core {

enum class OneCompartmentBolusExponentials : int
{
    Ke
};

enum class OneCompartmentBolusCompartments : int
{
    First
};

/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment bolus algorithm
/// \sa IntakeIntervalCalculator
class OneCompartmentBolusMicro : public IntakeIntervalCalculatorBase<1, OneCompartmentBolusExponentials>
{
    INTAKEINTERVALCALCULATOR_UTILS(OneCompartmentBolusMicro)
public:
    /// \brief Constructor
    OneCompartmentBolusMicro();

    typedef OneCompartmentBolusExponentials Exponentials;


    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

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

    void compute(const Residuals& _inResiduals, Eigen::VectorXd& _concentrations);

    Value m_D{NAN}; /// Quantity of drug
    Value m_V{NAN}; /// Volume of the compartment
    Value m_Ke{
            NAN}; /// Elimination constant rate = Cl/V where Cl is the clearance and V is the volume of the compartment
    Eigen::Index m_nbPoints{0}; /// Number measure points during interval
    Value m_Int{NAN};           /// Interval (hours)

private:
    typedef OneCompartmentBolusCompartments Compartments;
};

inline void OneCompartmentBolusMicro::compute(const Residuals& _inResiduals, Eigen::VectorXd& _concentrations)
{
    _concentrations = (m_D / m_V + _inResiduals[0]) * exponentials(Exponentials::Ke);
}

class OneCompartmentBolusMacro : public OneCompartmentBolusMicro
{
    INTAKEINTERVALCALCULATOR_UTILS(OneCompartmentBolusMacro)
public:
    OneCompartmentBolusMacro();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;
};


} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_ONECOMPARTMENTBOLUS_H
