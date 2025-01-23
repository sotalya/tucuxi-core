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


#ifndef LINEARBOLUS_H
#define LINEARBOLUS_H

#include "tucucore/intakeevent.h"
#include "tucucore/intakeintervalcalculatoranalytical.h"

namespace Tucuxi {
namespace Core {

enum class ConstantEliminationBolusExponentials : int
{
    P
};
enum class ConstantEliminationBolusCompartments : int
{
    First
};

/// \ingroup TucuCore
/// \brief Intake interval calculator for tests. It is a bolus with a linear slope
///
/// 4 parameters:
///     - A
///     - S
///     - R
///     - M
/// The equation is the following:
/// C(t) = max(0.0 , (D + residual * R) * ( 1 - t * S) * M + A)
/// It allows to test other components with a simple equation:
///     - Easy to calculate AUC
///     - Easy to calculate residual
///     - Easy to calculate percentiles depending on variability of A
///
/// When using R=0, S=0 and A=0, the concentration is simply the dose
///
class ConstantEliminationBolus : public IntakeIntervalCalculatorBase<1, ConstantEliminationBolusExponentials>
{
    INTAKEINTERVALCALCULATOR_UTILS(ConstantEliminationBolus)
public:
    /// \brief Constructor
    ConstantEliminationBolus()
        : IntakeIntervalCalculatorBase<1, ConstantEliminationBolusExponentials>(
                std::make_unique<PertinentTimesCalculatorStandard>())
    {
    }

    typedef ConstantEliminationBolusExponentials Exponentials;


    static std::vector<std::string> getParametersId()
    {
        return {"TestA", "TestS", "TestR", "TestM"};
    }

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override
    {

        if (!checkCondition(_parameters.size() >= 4, "The number of parameters should be equal to 4.")) {
            return false;
        }

        m_D = _intakeEvent.getDose();
        m_S = _parameters.getValue(ParameterId::TestS);
        m_A = _parameters.getValue(ParameterId::TestA);
        m_R = _parameters.getValue(ParameterId::TestR);
        m_M = _parameters.getValue(ParameterId::TestM);
        m_nbPoints = static_cast<Eigen::Index>(_intakeEvent.getNbPoints());
        m_Int = static_cast<int>((_intakeEvent.getInterval()).toHours());

#ifdef TUCU_INTAKECALCULATOR_VERBOSE
        Tucuxi::Common::LoggerHelper logHelper;

        logHelper.debug("<<Input Values>>");
        logHelper.debug("m_S: {}", m_S);
        logHelper.debug("m_A: {}", m_A);
        logHelper.debug("m_nbPoints: {}", m_nbPoints);
        logHelper.debug("m_Int: {}", m_Int);
#endif // TUCU_INTAKECALCULATOR_VERBOSE

        // check the inputs
        bool bOK = true;
        bOK &= checkPositiveValue(m_S, "S");
        bOK &= checkValidValue(m_A, "A");

        return bOK;
    }

    void computeExponentials(Eigen::VectorXd& _times) override
    {
        Eigen::VectorXd val = _times;
        auto s = val.size();
        for (int i = 0; i < s; i++) {
            val[i] = (1 - _times[i] * m_S) * m_M;
        }
        setExponentials(Exponentials::P, val.array());
    }

    bool computeConcentrations(
            const Residuals& _inResiduals,
            bool _isAll,
            MultiCompConcentrations& _concentrations,
            Residuals& _outResiduals) override
    {
        Eigen::VectorXd concentrations;
        size_t firstCompartment = static_cast<size_t>(Compartments::First);


        // Compute concentrations
        compute(_inResiduals, concentrations);

        // Return finla residual
        _outResiduals[firstCompartment] = concentrations[m_nbPoints - 1];

        // Return concentraions of first compartment
        _concentrations[firstCompartment].assign(concentrations.data(), concentrations.data() + concentrations.size());
        // Only one compartment is existed.
        TMP_UNUSED_PARAMETER(_isAll);

        return checkCondition(_outResiduals[firstCompartment] >= 0, "The concentration is negative.");
    }

    bool computeConcentration(
            const Value& _atTime,
            const Residuals& _inResiduals,
            bool _isAll,
            MultiCompConcentrations& _concentrations,
            Residuals& _outResiduals) override
    {
        TMP_UNUSED_PARAMETER(_atTime);

        Eigen::VectorXd concentrations;
        size_t firstCompartment = static_cast<size_t>(Compartments::First);
        Eigen::Index atTime = static_cast<Eigen::Index>(SingleConcentrations::AtTime);
        Eigen::Index atEndInterval = static_cast<Eigen::Index>(SingleConcentrations::AtEndInterval);

        // Compute concentrations
        compute(_inResiduals, concentrations);

        // Return concentrations (computation with atTime (current time))
        _concentrations[firstCompartment].push_back(concentrations[atTime]);
        // Only one compartment is existed.
        TMP_UNUSED_PARAMETER(_isAll);

        // interval=0 means that it is the last cycle, so final residual = 0
        if (m_Int == 0.0) {
            concentrations[atEndInterval] = 0;
        }

        // Return final residual (computation with m_Int (interval))
        _outResiduals[firstCompartment] = concentrations[atEndInterval];

        return checkCondition(_outResiduals[firstCompartment] >= 0, "The concentration is negative.");
    }


    void compute(const Residuals& _inResiduals, Eigen::VectorXd& _concentrations);

    Value m_D;               /// Quantity of drug
    Value m_S;               /// Slope of elimination
    Value m_M;               /// Multiplicative factor of the concentration
    Value m_A;               /// Additional value to concentration
    Value m_R;               /// Multiplier for the residual
    Eigen::Index m_nbPoints; /// Number measure points during interval
    Value m_Int;             /// Interval (hours)

private:
    typedef ConstantEliminationBolusCompartments Compartments;
};

inline void ConstantEliminationBolus::compute(const Residuals& _inResiduals, Eigen::VectorXd& _concentrations)
{
    _concentrations = (m_D + m_R * _inResiduals[0]) * exponentials(Exponentials::P);
    for (int i = 0; i < _concentrations.size(); i++) {
        _concentrations[i] += m_A;
        if (_concentrations[i] < 0.0) {
            _concentrations[i] = 0;
        }
    }
}

} // namespace Core
} // namespace Tucuxi


#endif // LINEARBOLUS_H
