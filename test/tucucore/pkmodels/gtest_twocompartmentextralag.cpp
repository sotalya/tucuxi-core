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


#include <memory>

#include <gtest/gtest.h>

#include "tucucommon/duration.h"

#include "tucucore/drugmodel/formulationandroute.h"
#include "tucucore/intakeevent.h"
#include "tucucore/pkmodels/twocompartmentextralag.h"

#include "../gtest_core.h"

using namespace Tucuxi::Core;
using namespace std::chrono_literals;

const unsigned int maxResidualSize = 3;

void testFirstDose(
        const Tucuxi::Core::ParameterSetEvent& _parameters,
        double _dose,
        Tucuxi::Core::AbsorptionModel _route,
        std::chrono::hours _interval,
        std::chrono::seconds _infusionTime,
        CycleSize _nbPoints)
{
    Tucuxi::Core::ComputingStatus res;
    TwoCompartmentExtraLagMacro macroCalculator;

    DateTime now = DateTime::now();
    Tucuxi::Common::Duration offsetTime = 0s;
    Tucuxi::Common::Duration interval = _interval;
    Tucuxi::Common::Duration infusionTime = _infusionTime;

    unsigned int residualSize = (macroCalculator.getResidualSize() == macroCalculator.getResidualSize())
                                        ? macroCalculator.getResidualSize()
                                        : maxResidualSize;
    bool isAll = false;

    MultiCompConcentrations concentrations;
    concentrations.resize(residualSize);

    std::vector<Tucuxi::Core::Concentrations> concentrationsInterval2;
    concentrationsInterval2.resize(residualSize);

    MultiCompConcentrations concentrations2;
    concentrations2.resize(residualSize);

    std::vector<Tucuxi::Core::Concentrations> concentrations2Interval2;
    concentrations2Interval2.resize(residualSize);

    Tucuxi::Core::TimeOffsets times;
    Tucuxi::Core::IntakeEvent intakeEvent(
            now,
            offsetTime,
            _dose,
            TucuUnit("mg"),
            interval,
            Tucuxi::Core::FormulationAndRoute(),
            infusionTime,
            _nbPoints);

    // Start with an empty residual
    const Tucuxi::Core::Residuals firstInResidual(residualSize, 0);
    Tucuxi::Core::Residuals outMicroMultiResiduals(residualSize);
    Tucuxi::Core::Residuals outMicroSingleResiduals(residualSize);
    Tucuxi::Core::Residuals outMacroMultiResiduals(residualSize);
    Tucuxi::Core::Residuals outMacroMultiResiduals2(residualSize);
    Tucuxi::Core::Residuals outMacroSingleResiduals(residualSize);
    Tucuxi::Core::Residuals outMacroSingleResiduals2(residualSize);

    // Calculation of Macro Class
    res = macroCalculator.calculateIntakePoints(
            concentrations, times, intakeEvent, _parameters, firstInResidual, isAll, outMicroMultiResiduals, true);

#if GTEST_VERBOSE
    std::cout << "[Micro Class Calculation]" << std::endl;
    for (unsigned int i = 0; i < residualSize; i++) {
        std::cout << "Multiple Out residual[" << i << "] = " << outMicroMultiResiduals[i] << std::endl;
    }
#endif

    ASSERT_EQ(res, Tucuxi::Core::ComputingStatus::Ok);
}

/// \brief Test the residual calculation of extravascular. Compares single point vs multiple points
/// \param _testName Test name.
/// A calculator is instanciated, and residuals are computed with both
///     calculateIntakePoints and calculateIntakeSinglePoint.
/// The residuals are then compared and shall be identical.
TEST(Core_TestTwoCompartmentExtraLag, twoCompExtraLagSingleDose)
{
    // parameter for macro class
    Tucuxi::Core::ParameterDefinitions macroParameterDefs;
    macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "CL", 15.106, Tucuxi::Core::ParameterVariabilityType::None));
    macroParameterDefs.push_back(
            std::make_unique<Tucuxi::Core::ParameterDefinition>("Q", 20, Tucuxi::Core::ParameterVariabilityType::None));
    macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "V2", 342, Tucuxi::Core::ParameterVariabilityType::None));
    macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "V1", 340, Tucuxi::Core::ParameterVariabilityType::None));
    macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "Ka", 0.609, Tucuxi::Core::ParameterVariabilityType::None));
    macroParameterDefs.push_back(
            std::make_unique<Tucuxi::Core::ParameterDefinition>("F", 1, Tucuxi::Core::ParameterVariabilityType::None));
    macroParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "Tlag", 1, Tucuxi::Core::ParameterVariabilityType::None));
    Tucuxi::Core::ParameterSetEvent macroParameters(DateTime::now(), macroParameterDefs);

    testFirstDose(macroParameters, 400.0, Tucuxi::Core::AbsorptionModel::Extravascular, 12h, 0s, CYCLE_SIZE);
}
