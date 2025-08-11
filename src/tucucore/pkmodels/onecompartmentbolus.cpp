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


#include <Eigen/Dense>

#include "tucucore/pkmodels/onecompartmentbolus.h"

#include "tucucommon/loggerhelper.h"

#include "tucucore/intakeevent.h"

namespace Tucuxi {
namespace Core {

OneCompartmentBolusMicro::OneCompartmentBolusMicro()
    : IntakeIntervalCalculatorBase<1, OneCompartmentBolusExponentials>(
            std::make_unique<PertinentTimesCalculatorStandard>())
{
}

std::vector<std::string> OneCompartmentBolusMicro::getParametersId()
{
    return {"Ke", "V"};
}

bool OneCompartmentBolusMicro::checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkCondition(_parameters.size() >= 2, "The number of parameters should be equal to 2.")) {
        return false;
    }

    m_D = _intakeEvent.getDose();
    m_V = _parameters.getValue(ParameterId::V);
    m_Ke = _parameters.getValue(ParameterId::Ke);
    m_nbPoints = static_cast<Eigen::Index>(_intakeEvent.getNbPoints());
    m_Int = (_intakeEvent.getInterval()).toHours();

#ifdef TUCU_INTAKECALCULATOR_VERBOSE
    Tucuxi::Common::LoggerHelper logHelper;

    logHelper.debug("<<Input Values>>");
    logHelper.debug("m_D: {}", m_D);
    logHelper.debug("m_V: {}", m_V);
    logHelper.debug("m_Ke: {}", m_Ke);
    logHelper.debug("m_nbPoints: {}", m_nbPoints);
    logHelper.debug("m_Int: {}", m_Int);
#endif // TUCU_INTAKECALCULATOR_VERBOSE

    // check the inputs
    bool bOK = checkPositiveValue(m_D, "The dose");
    bOK &= checkStrictlyPositiveValue(m_V, "The volume");
    bOK &= checkStrictlyPositiveValue(m_Ke, "Ke");
    bOK &= checkCondition(m_nbPoints > 0, "The number of points is zero or negative.");
    bOK &= checkCondition(m_Int > 0, "The interval time is negative.");

    return bOK;
}


void OneCompartmentBolusMicro::computeExponentials(Eigen::VectorXd& _times)
{
    setExponentials(Exponentials::Ke, (-m_Ke * _times).array().exp());
}


bool OneCompartmentBolusMicro::computeConcentrations(
        const Residuals& _inResiduals, bool _isAll, MultiCompConcentrations& _concentrations, Residuals& _outResiduals)
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

bool OneCompartmentBolusMicro::computeConcentration(
        const Value& _atTime,
        const Residuals& _inResiduals,
        bool _isAll,
        MultiCompConcentrations& _concentrations,
        Residuals& _outResiduals)
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
    if (m_Int == 0) {
        concentrations[atEndInterval] = 0;
    }

    // Return final residual (computation with m_Int (interval))
    _outResiduals[firstCompartment] = concentrations[atEndInterval];

    return checkCondition(_outResiduals[firstCompartment] >= 0, "The concentration is negative.");
}

OneCompartmentBolusMacro::OneCompartmentBolusMacro() {}


std::vector<std::string> OneCompartmentBolusMacro::getParametersId()
{
    return {"CL", "V"};
}

bool OneCompartmentBolusMacro::checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkCondition(_parameters.size() >= 2, "The number of parameters should be equal to 2.")) {
        return false;
    }

    m_D = _intakeEvent.getDose();
    m_V = _parameters.getValue(ParameterId::V);
    Value cl = _parameters.getValue(ParameterId::CL);
    m_Ke = cl / m_V;
    m_nbPoints = static_cast<Eigen::Index>(_intakeEvent.getNbPoints());
    m_Int = (_intakeEvent.getInterval()).toHours();

#ifdef TUCU_INTAKECALCULATOR_VERBOSE
    Tucuxi::Common::LoggerHelper logHelper;

    logHelper.debug("<<Input Values>>");
    logHelper.debug("m_D: {}", m_D);
    logHelper.debug("m_V: {}", m_V);
    logHelper.debug("cl: {}", cl);
    logHelper.debug("m_Ke: {}", m_Ke);
    logHelper.debug("m_nbPoints: {}", m_nbPoints);
    logHelper.debug("m_Int: {}", m_Int);
#endif // TUCU_INTAKECALCULATOR_VERBOSE

    // check the inputs
    bool bOK = checkPositiveValue(m_D, "The dose");
    bOK &= checkStrictlyPositiveValue(m_V, "The volume");
    bOK &= checkStrictlyPositiveValue(cl, "The clearance");
    bOK &= checkCondition(m_nbPoints > 0, "The number of points is zero or negative.");
    bOK &= checkCondition(m_Int > 0, "The interval time is negative.");

    return bOK;
}



} // namespace Core
} // namespace Tucuxi
