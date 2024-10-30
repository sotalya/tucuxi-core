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

#include "tucucore/pkmodels/threecompartmentinfusion.h"

#include "tucucommon/loggerhelper.h"

#include "tucucore/intakeevent.h"

namespace Tucuxi {
namespace Core {

#if 0
#define DEBUG
#endif

ThreeCompartmentInfusionMicro::ThreeCompartmentInfusionMicro()
    : IntakeIntervalCalculatorBase<3, ThreeCompartmentInfusionExponentials>(
            std::make_unique<PertinentTimesCalculatorInfusion>())
{
}

std::vector<std::string> ThreeCompartmentInfusionMicro::getParametersId()
{
    return {"Ke", "V1", "K12", "K21", "K13", "K31"};
}

bool ThreeCompartmentInfusionMicro::checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkCondition(_parameters.size() >= 6, "The number of parameters should be equal to 6.")) {
        return false;
    }

    m_D = _intakeEvent.getDose();
    m_F = _parameters.getValue(ParameterId::F);
    m_V1 = _parameters.getValue(ParameterId::V1);
    m_Ke = _parameters.getValue(ParameterId::Ke);
    m_K12 = _parameters.getValue(ParameterId::K12);
    m_K21 = _parameters.getValue(ParameterId::K21);
    m_K13 = _parameters.getValue(ParameterId::K13);
    m_K31 = _parameters.getValue(ParameterId::K31);
    m_Tinf = _intakeEvent.getInfusionTime().toHours();
    m_Int = _intakeEvent.getInterval().toHours();
    m_nbPoints = static_cast<Eigen::Index>(_intakeEvent.getNbPoints());

    Value a0 = m_Ke * m_K21 * m_K31;
    Value a1 = m_Ke * m_K31 + m_K21 * m_K31 + m_K21 * m_K13 + m_Ke * m_K21 + m_K31 * m_K12;
    Value a2 = m_Ke + m_K12 + m_K13 + m_K21 + m_K31;
    Value p = a1 - std::pow(a2, 2) / 3;
    Value q = 2 * std::pow(a2, 3) / 27 - a1 * a2 / 3 + a0;
    Value r1 = std::sqrt(-(std::pow(p, 3) / 27));
    Value r2 = 2 * std::pow(r1, 1 / 3);
    Value phi = std::acos(-q / (2 * r1)) / 3;

    m_Alpha = -(std::cos(phi) * r2 - a2 / 3);
    m_Beta = -(std::cos(phi + 2 * 3.1428 / 3) * r2 - a2 / 3);
    m_Gamma = -(std::cos(phi + 4 * 3.1428 / 3) * r2 - a2 / 3);

#ifdef DEBUG
    Tucuxi::Common::LoggerHelper logHelper;

    logHelper.debug("<<Input Values>>");
    logHelper.debug("m_D: {}", m_D);
    logHelper.debug("m_F: {}", m_F);
    logHelper.debug("m_V1: {}", m_V1);
    logHelper.debug("m_Ke: {}", m_Ke);
    logHelper.debug("m_K12: {}", m_K12);
    logHelper.debug("m_K21: {}", m_K21);
    logHelper.debug("m_K13: {}", m_K13);
    logHelper.debug("m_K31: {}", m_K31);
    logHelper.debug("m_Tinf: {}", m_Tinf);
    logHelper.debug("m_nbPoints: {}", m_nbPoints);
    logHelper.debug("m_Int: {}", m_Int);
#endif

    // check the inputs
    bool bOK = checkPositiveValue(m_D, "The dose");
    bOK &= checkStrictlyPositiveValue(m_F, "F");
    bOK &= checkStrictlyPositiveValue(m_Ke, "Ke");
    bOK &= checkStrictlyPositiveValue(m_K12, "K12");
    bOK &= checkStrictlyPositiveValue(m_K21, "K21");
    bOK &= checkStrictlyPositiveValue(m_K13, "K13");
    bOK &= checkStrictlyPositiveValue(m_K31, "K31");
    bOK &= checkPositiveValue(m_Alpha, "Alpha");
    bOK &= checkPositiveValue(m_Beta, "Beta");
    bOK &= checkPositiveValue(m_Gamma, "Gamma");
    bOK &= checkPositiveValue(m_Tinf, "The infusion time");
    bOK &= checkCondition(m_nbPoints > 0, "The number of points is zero or negative.");
    bOK &= checkCondition(m_Int > 0, "The interval time is negative.");



    return bOK;
}

void ThreeCompartmentInfusionMicro::computeExponentials(Eigen::VectorXd& _times)
{
    setExponentials(Exponentials::Alpha, (-m_Alpha * _times).array().exp());
    setExponentials(Exponentials::Beta, (-m_Beta * _times).array().exp());
    setExponentials(Exponentials::Gamma, (-m_Gamma * _times).array().exp());
}

bool ThreeCompartmentInfusionMicro::computeConcentrations(
        const Residuals& _inResiduals, bool _isAll, MultiCompConcentrations& _concentrations, Residuals& _outResiduals)
{
    Eigen::VectorXd concentrations1(m_nbPoints);
    Value concentrations2;
    Value concentrations3;
    size_t firstCompartment = static_cast<size_t>(Compartments::First);
    size_t secondCompartment = static_cast<size_t>(Compartments::Second);
    size_t thirdCompartment = static_cast<size_t>(Compartments::Third);
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
    TMP_UNUSED_PARAMETER(_inResiduals);

    // Calculate concentrations for comp1 and comp2
    compute(forcesize, concentrations1, concentrations2, concentrations3);

    // return concentrations of comp1, comp2 and comp3
    _outResiduals[firstCompartment] = concentrations1[m_nbPoints - 1];
    _outResiduals[secondCompartment] = concentrations2;
    _outResiduals[thirdCompartment] = concentrations3;

    _concentrations[firstCompartment].assign(concentrations1.data(), concentrations1.data() + concentrations1.size());
    // TODO: add calcuation concentrations of second and third compartment and condtions
    TMP_UNUSED_PARAMETER(_isAll);

    bool bOK = checkCondition(_outResiduals[firstCompartment] >= 0, "The concentration1 is negative.");
    bOK &= checkCondition(_outResiduals[secondCompartment] >= 0, "The concentration2 is negative.");
    bOK &= checkCondition(_outResiduals[thirdCompartment] >= 0, "The concentration3 is negative.");

    return bOK;
}

bool ThreeCompartmentInfusionMicro::computeConcentration(
        const Value& _atTime,
        const Residuals& _inResiduals,
        bool _isAll,
        MultiCompConcentrations& _concentrations,
        Residuals& _outResiduals)
{
    Eigen::VectorXd concentrations1(2);
    Value concentrations2;
    Value concentrations3;
    size_t firstCompartment = static_cast<size_t>(Compartments::First);
    size_t secondCompartment = static_cast<size_t>(Compartments::Second);
    size_t thirdCompartment = static_cast<size_t>(Compartments::Third);
    Eigen::Index atTime = static_cast<Eigen::Index>(SingleConcentrations::AtTime);
    Eigen::Index atEndInterval = static_cast<Eigen::Index>(SingleConcentrations::AtEndInterval);

    int forcesize = 0;

    TMP_UNUSED_PARAMETER(_inResiduals);

    if (_atTime <= m_Tinf) {
        forcesize = 1;
    }

    // Calculate concentrations for comp1 and comp2
    compute(forcesize, concentrations1, concentrations2, concentrations3);

    // return concentraions (computation with atTime (current time))
    _concentrations[firstCompartment].push_back(concentrations1[atTime]);
    // TODO: add calcuation concentrations of second and third compartment and condtions
    TMP_UNUSED_PARAMETER(_isAll);

    // interval=0 means that it is the last cycle, so final residual = 0
    if (m_Int == 0) {
        concentrations1[atEndInterval] = 0;
        concentrations2 = 0;
        concentrations3 = 0;
    }

    // return final residual (computation with m_Int (interval))
    _outResiduals[firstCompartment] = concentrations1[atEndInterval];
    _outResiduals[secondCompartment] = concentrations2;
    _outResiduals[thirdCompartment] = concentrations3;

    bool bOK = checkCondition(_outResiduals[firstCompartment] >= 0, "The concentration1 is negative.");
    bOK &= checkCondition(_outResiduals[secondCompartment] >= 0, "The concentration2 is negative.");
    bOK &= checkCondition(_outResiduals[thirdCompartment] >= 0, "The concentration3 is negative.");

    return bOK;
}

ThreeCompartmentInfusionMacro::ThreeCompartmentInfusionMacro() = default;

std::vector<std::string> ThreeCompartmentInfusionMacro::getParametersId()
{
    return {"CL", "V1", "Q", "V2", "Q1", "Q2"};
}


bool ThreeCompartmentInfusionMacro::checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkCondition(_parameters.size() >= 6, "The number of parameters should be equal to 6.")) {
        return false;
    }

    m_D = _intakeEvent.getDose();
    Value cl = _parameters.getValue(ParameterId::CL);
    m_F = _parameters.getValue(ParameterId::F);
    Value q3 = _parameters.getValue(ParameterId::Q3);
    Value q2 = _parameters.getValue(ParameterId::Q2);
    m_V1 = _parameters.getValue(ParameterId::V1);
    Value v2 = _parameters.getValue(ParameterId::V2);
    Value v3 = _parameters.getValue(ParameterId::V3);
    m_Ke = cl / m_V1;
    m_K12 = q2 / m_V1;
    m_K21 = q2 / v2;
    m_K13 = q3 / m_V1;
    m_K31 = q3 / v3;
    m_Tinf = _intakeEvent.getInfusionTime().toHours();
    m_Int = _intakeEvent.getInterval().toHours();
    m_nbPoints = static_cast<Eigen::Index>(_intakeEvent.getNbPoints());

    Value a0 = m_Ke * m_K21 * m_K31;
    Value a1 = m_Ke * m_K31 + m_K21 * m_K31 + m_K21 * m_K13 + m_Ke * m_K21 + m_K31 * m_K12;
    Value a2 = m_Ke + m_K12 + m_K13 + m_K21 + m_K31;
    Value p = a1 - std::pow(a2, 2) / 3;
    Value q = 2 * std::pow(a2, 3) / 27 - a1 * a2 / 3 + a0;
    Value r1 = std::sqrt(-(std::pow(p, 3) / 27));
    Value r2 = 2 * std::pow(r1, 1 / 3);
    Value phi = std::acos(-q / (2 * r1)) / 3;

    m_Alpha = -(std::cos(phi) * r2 - a2 / 3);
    m_Beta = -(std::cos(phi + 2 * 3.1428 / 3) * r2 - a2 / 3);
    m_Gamma = -(std::cos(phi + 4 * 3.1428 / 3) * r2 - a2 / 3);

    // check the inputs
    bool bOK = checkPositiveValue(m_D, "The dose");
    bOK &= checkStrictlyPositiveValue(m_F, "F");
    bOK &= checkStrictlyPositiveValue(cl, "The clearance");
    bOK &= checkStrictlyPositiveValue(q, "Q");
    bOK &= checkStrictlyPositiveValue(q3, "Q3");
    bOK &= checkStrictlyPositiveValue(q2, "Q2");
    bOK &= checkStrictlyPositiveValue(m_V1, "V1");
    bOK &= checkStrictlyPositiveValue(v2, "V2");
    bOK &= checkPositiveValue(m_Alpha, "Alpha");
    bOK &= checkPositiveValue(m_Beta, "Beta");
    bOK &= checkPositiveValue(m_Gamma, "Gamma");
    bOK &= checkPositiveValue(m_Tinf, "The infusion time");
    bOK &= checkCondition(m_nbPoints > 0, "The number of points is zero or negative.");
    bOK &= checkCondition(m_Int > 0, "The interval time is negative.");

    return bOK;
}

} // namespace Core
} // namespace Tucuxi
