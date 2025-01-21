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

#include "rkmichaelismentenonecompaiemax.h"

#include "tucucore/intakeevent.h"

namespace Tucuxi {
namespace Core {


std::vector<std::string> RkMichaelisMentenOneCompAiEmax::getParametersId()
{
    return {"V", "Vmax", "F", "Km", "Ka", "Emax", "t50", "tfs"};
}


std::vector<std::string> RkMichaelisMentenOneCompAiEmaxLag::getParametersId()
{
    return {"V", "Vmax", "F", "Km", "Ka", "Emax", "t50", "tfs", "Tlag"};
}


RkMichaelisMentenOneCompAiEmax::RkMichaelisMentenOneCompAiEmax()
    : IntakeIntervalCalculatorRK4Base<2, RkMichaelisMentenOneCompAiEmax>(
            std::make_unique<PertinentTimesCalculatorStandard>())
{
}


bool RkMichaelisMentenOneCompAiEmax::checkInputs(
        const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkCondition(_parameters.size() >= 8,
                        "The number of parameters should be equal to 8.")) {
        return false;
    }

    bool bOK = true;

    // Implicit parameters.
    m_D = _intakeEvent.getDose();
    m_nbPoints = _intakeEvent.getNbPoints();
    m_Int = (_intakeEvent.getInterval()).toHours();

    bOK &= checkPositiveValue(m_D, "The dose");

    bOK &= checkCondition(m_nbPoints > 0,
                          "The number of points is zero or negative.");
    bOK &= checkCondition(m_Int > 0, "The interval time is negative.");


    // Explicit parameters.
    m_V = _parameters.getValue(ParameterId::V);
    m_Vmax = _parameters.getValue(ParameterId::Vmax);
    m_F = _parameters.getValue(ParameterId::F);
    m_Km = _parameters.getValue(ParameterId::Km);
    m_Ka = _parameters.getValue(ParameterId::Ka);
    m_Emax = _parameters.getValue(ParameterId::Emax);
    m_t50 = _parameters.getValue(ParameterId::t50);
    m_tfs = _parameters.getValue(ParameterId::tfs);

    bOK &= checkStrictlyPositiveValue(m_V, "The compartment volume");
    bOK &= checkStrictlyPositiveValue(m_Vmax, "The maximum elimination rate");
    bOK &= checkStrictlyPositiveValue(m_F, "The bioavailability");
    bOK &= checkStrictlyPositiveValue(m_Km, "The Michaelis Menten constant");
    bOK &= checkStrictlyPositiveValue(m_Ka, "The absorption rate");
    bOK &= checkStrictlyPositiveValue(m_Emax, "The maximum effect parameter");
    bOK &= checkStrictlyPositiveValue(m_t50,
                                      "The time at which the effect reaches " \
                                      "half of its maximum");
    bOK &= checkStrictlyPositiveValue(m_tfs,
                                      "The time from start");

    return bOK;
}


bool RkMichaelisMentenOneCompAiEmaxLag::checkInputs(
        const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkCondition(_parameters.size() >= 9,
                        "The number of parameters should be equal to 9.")) {
        return false;
    }

    bool bOK = true;

    m_Tlag = _parameters.getValue(ParameterId::Tlag);

    bOK &= RkMichaelisMentenOneCompAiEmax::checkInputs(_intakeEvent,
                                                       _parameters);
    bOK &= checkPositiveValue(m_Tlag, "The lag time");

    return bOK;
}


} // namespace Core
} // namespace Tucuxi
