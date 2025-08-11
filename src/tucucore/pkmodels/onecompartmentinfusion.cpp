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


#include <algorithm>
#include <math.h>

#include <Eigen/Dense>

#include "tucucore/pkmodels/onecompartmentinfusion.h"

#include "tucucommon/loggerhelper.h"

#include "tucucore/intakeevent.h"

namespace Tucuxi {
namespace Core {

OneCompartmentInfusionMicro::OneCompartmentInfusionMicro()
    : IntakeIntervalCalculatorBase<1, OneCompartmentInfusionExponentials>(
            std::make_unique<PertinentTimesCalculatorInfusion>())
{
}

std::vector<std::string> OneCompartmentInfusionMicro::getParametersId()
{
    return {"Ke", "V"};
}


bool OneCompartmentInfusionMicro::checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkCondition(_parameters.size() >= 2, "The number of parameters should be equal to 2.")) {
        return false;
    }

    m_D = _intakeEvent.getDose();
    m_Ke = _parameters.getValue(ParameterId::Ke);
    m_V = _parameters.getValue(ParameterId::V);
    m_Tinf = (_intakeEvent.getInfusionTime()).toHours();
    m_Int = (_intakeEvent.getInterval()).toHours();
    m_nbPoints = static_cast<Eigen::Index>(_intakeEvent.getNbPoints());

#ifdef TUCU_INTAKECALCULATOR_VERBOSE
    Tucuxi::Common::LoggerHelper logHelper;

    logHelper.debug("<<Input Values>>");
    logHelper.debug("m_D: {}", m_D);
    logHelper.debug("m_V: {}", m_V);
    logHelper.debug("m_Ke: {}", m_Ke);
    logHelper.debug("m_Tinf: {}", m_Tinf);
    logHelper.debug("m_Int: {}", m_Int);
    logHelper.debug("m_nbPoints: {}", m_nbPoints);
#endif // TUCU_INTAKECALCULATOR_VERBOSE

    bool bOK = checkPositiveValue(m_D, "The dose");
    bOK &= checkStrictlyPositiveValue(m_V, "The volume");
    bOK &= checkStrictlyPositiveValue(m_Ke, "The clearance");
    bOK &= checkCondition(m_Tinf >= 0, "The infusion time is negative.");
    bOK &= checkCondition(m_nbPoints > 0, "The number of points is zero or negative.");
    bOK &= checkCondition(m_Int > 0, "The interval time is negative.");

    return bOK;
}


void OneCompartmentInfusionMicro::computeExponentials(Eigen::VectorXd& _times)
{
    setExponentials(Exponentials::Ke, (-m_Ke * _times).array().exp());

    // We need to determine if the post infusion part needs to be calculated or not
    if (_times[_times.size() - 1] <= m_Tinf) {
        m_timeMaxHigherThanTinf = false;
    }
}


bool OneCompartmentInfusionMicro::computeConcentrations(
        const Residuals& _inResiduals, bool _isAll, MultiCompConcentrations& _concentrations, Residuals& _outResiduals)
{
    Eigen::VectorXd concentrations;
    size_t firstCompartment = static_cast<size_t>(Compartments::First);
    int forcesize;
    if (m_nbPoints == 2) {
        forcesize = static_cast<int>(std::min(
                ceil(m_Tinf / m_Int * static_cast<double>(m_nbPoints)), ceil(static_cast<double>(m_nbPoints))));
    }
    else {
        forcesize = std::min(
                static_cast<int>(m_nbPoints),
                std::max(2, static_cast<int>((m_Tinf / m_Int) * static_cast<double>(m_nbPoints))));
    }

    // Calculate concentrations
    compute(_inResiduals, forcesize, concentrations);

    // Return final Residual
    _outResiduals[firstCompartment] = concentrations[m_nbPoints - 1];

    // Return concentraions of first compartment
    _concentrations[firstCompartment].assign(concentrations.data(), concentrations.data() + concentrations.size());
    // Only one compartment is existed.
    TMP_UNUSED_PARAMETER(_isAll);

    return checkCondition(_outResiduals[firstCompartment] >= 0, "The concentration is negative.");
}

bool OneCompartmentInfusionMicro::computeConcentration(
        const Value& _atTime,
        const Residuals& _inResiduals,
        bool _isAll,
        MultiCompConcentrations& _concentrations,
        Residuals& _outResiduals)
{
    Eigen::VectorXd concentrations;
    size_t firstCompartment = static_cast<size_t>(Compartments::First);
    Eigen::Index atTime = static_cast<Eigen::Index>(SingleConcentrations::AtTime);
    Eigen::Index atEndInterval = static_cast<Eigen::Index>(SingleConcentrations::AtEndInterval);

    int forcesize = 0;

    if (_atTime <= m_Tinf) {
        if (m_timeMaxHigherThanTinf) {
            forcesize = 1;
        }
        else {
            forcesize = 2;
        }
    }

    // Calculate concentrations
    compute(_inResiduals, forcesize, concentrations);

    // return concentrations of first compartment
    // (computation with atTime (current time))
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

OneCompartmentInfusionMacro::OneCompartmentInfusionMacro() {}

std::vector<std::string> OneCompartmentInfusionMacro::getParametersId()
{
    return {"CL", "V"};
}

bool OneCompartmentInfusionMacro::checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkCondition(_parameters.size() >= 2, "The number of parameters should be equal to 2.")) {
        return false;
    }

    m_D = _intakeEvent.getDose();
    double cl = _parameters.getValue(ParameterId::CL);
    m_V = _parameters.getValue(ParameterId::V);
    m_Ke = cl / m_V;
    m_Tinf = (_intakeEvent.getInfusionTime()).toHours();
    m_Int = (_intakeEvent.getInterval()).toHours();
    m_nbPoints = static_cast<Eigen::Index>(_intakeEvent.getNbPoints());

#ifdef DEBUG
    Tucuxi::Common::LoggerHelper logHelper;

    logHelper.debug("<<Input Values>>");
    logHelper.debug("m_D: {}", m_D);
    logHelper.debug("m_V: {}", m_V);
    logHelper.debug("cl: {}", cl);
    logHelper.debug("m_Ke: {}", m_Ke);
    logHelper.debug("m_Tinf: {}", m_Tinf);
    logHelper.debug("m_Int: {}", m_Int);
    logHelper.debug("m_nbPoints: {}", m_nbPoints);
#endif

    bool bOK = checkPositiveValue(m_D, "The dose");
    bOK &= checkStrictlyPositiveValue(m_V, "The volume");
    bOK &= checkStrictlyPositiveValue(cl, "The clearance");
    bOK &= checkCondition(m_Tinf >= 0, "The infusion time is negative.");
    bOK &= checkCondition(m_nbPoints > 0, "The number of points is zero or negative.");
    bOK &= checkCondition(m_Int > 0, "The interval time is negative.");


    return bOK;
}


} // namespace Core
} // namespace Tucuxi
