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

#include "tucucore/dosage.h"
#include "tucucore/intakeextractor.h"
#include "tucucore/pkmodels/onecompartmentbolus.h"
#include "tucucore/pkmodels/onecompartmentextra.h"
#include "tucucore/pkmodels/onecompartmentinfusion.h"
// #include "tucucore/pkmodels/threecompartmentbolus.h"
#include "tucucore/pkmodels/rkthreecompartment.h"
#include "tucucore/pkmodels/threecompartmentextra.h"
#include "tucucore/pkmodels/threecompartmentinfusion.h"
#include "tucucore/pkmodels/twocompartmentbolus.h"
#include "tucucore/pkmodels/twocompartmentextra.h"
#include "tucucore/pkmodels/twocompartmentinfusion.h"

#include "gtest_core.h"
#include "pkmodels/constanteliminationbolus.h"

using namespace Tucuxi::Core;

TEST(Core_TestConcentrationCalculator, ConstantEliminationBolus)
{
    Tucuxi::Core::ParameterDefinitions parameterDefs;
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestA", 0.0, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestR", 0.0, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestS", 0.0, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestM", 1.0, Tucuxi::Core::ParameterVariabilityType::None));
    Tucuxi::Core::ParameterSetEvent parameters(DateTime::now(), parameterDefs);
    Tucuxi::Core::ParameterSetSeries parametersSeries;
    parametersSeries.addParameterSetEvent(parameters);

    testCalculator<Tucuxi::Core::ConstantEliminationBolus>(
            parametersSeries, 400.0, Tucuxi::Core::AbsorptionModel::Extravascular, 12h, 0s, CYCLE_SIZE);
}

TEST(Core_TestConcentrationCalculator, oneCompBolus)
{
    Tucuxi::Core::ParameterDefinitions parameterDefs;
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "V", 347, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "Ke", 22.97, Tucuxi::Core::ParameterVariabilityType::None));
    Tucuxi::Core::ParameterSetEvent parameters(DateTime::now(), parameterDefs);
    Tucuxi::Core::ParameterSetSeries parametersSeries;
    parametersSeries.addParameterSetEvent(parameters);

    testCalculator<Tucuxi::Core::OneCompartmentBolusMicro>(
            parametersSeries, 400.0, Tucuxi::Core::AbsorptionModel::Intravascular, 12h, 0s, CYCLE_SIZE);
}

TEST(Core_TestConcentrationCalculator, oneCompExtra)
{
    Tucuxi::Core::ParameterDefinitions parameterDefs;
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "V", 347, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "Ke", 0.0435331, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "Ka", 0.609, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(
            std::make_unique<Tucuxi::Core::ParameterDefinition>("F", 1, Tucuxi::Core::ParameterVariabilityType::None));
    Tucuxi::Core::ParameterSetEvent parameters(DateTime::now(), parameterDefs);
    Tucuxi::Core::ParameterSetSeries parametersSeries;
    parametersSeries.addParameterSetEvent(parameters);

    testCalculator<Tucuxi::Core::OneCompartmentExtraMicro>(
            parametersSeries, 400.0, Tucuxi::Core::AbsorptionModel::Extravascular, 12h, 0s, CYCLE_SIZE);
}

TEST(Core_TestConcentrationCalculator, oneCompInfusion)
{
    Tucuxi::Core::ParameterDefinitions parameterDefs;
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "V", 347, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "Ke", 22.97, Tucuxi::Core::ParameterVariabilityType::None));
    Tucuxi::Core::ParameterSetEvent parameters(DateTime::now(), parameterDefs);
    Tucuxi::Core::ParameterSetSeries parametersSeries;
    parametersSeries.addParameterSetEvent(parameters);

    testCalculator<Tucuxi::Core::OneCompartmentInfusionMicro>(
            parametersSeries, 400.0, Tucuxi::Core::AbsorptionModel::Infusion, 12h, 1h, CYCLE_SIZE);
}

TEST(Core_TestConcentrationCalculator, twoCompBolus)
{
    Tucuxi::Core::ParameterDefinitions parameterDefs;
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "V1", 340, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "Ke", 0.0444294, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "K12", 0.0588235, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "K21", 0.0584795, Tucuxi::Core::ParameterVariabilityType::None));
    Tucuxi::Core::ParameterSetEvent parameters(DateTime::now(), parameterDefs);
    Tucuxi::Core::ParameterSetSeries parametersSeries;
    parametersSeries.addParameterSetEvent(parameters);

    testCalculator<Tucuxi::Core::TwoCompartmentBolusMicro>(
            parametersSeries, 400.0, Tucuxi::Core::AbsorptionModel::Intravascular, 12h, 0s, CYCLE_SIZE);
}

TEST(Core_TestConcentrationCalculator, twoCompExtra)
{
    Tucuxi::Core::ParameterDefinitions parameterDefs;
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "V1", 340, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "Ke", 0.0444294, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "K12", 0.0588235, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "K21", 0.0584795, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "Ka", 0.609, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(
            std::make_unique<Tucuxi::Core::ParameterDefinition>("F", 1, Tucuxi::Core::ParameterVariabilityType::None));
    Tucuxi::Core::ParameterSetEvent parameters(DateTime::now(), parameterDefs);
    Tucuxi::Core::ParameterSetSeries parametersSeries;
    parametersSeries.addParameterSetEvent(parameters);

    testCalculator<Tucuxi::Core::TwoCompartmentExtraMicro>(
            parametersSeries, 400.0, Tucuxi::Core::AbsorptionModel::Extravascular, 12h, 0s, CYCLE_SIZE);
}

TEST(Core_TestConcentrationCalculator, twoCompInfusion)
{
    Tucuxi::Core::ParameterDefinitions parameterDefs;
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "V1", 340, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "Ke", 0.0444294, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "K12", 0.0588235, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "K21", 0.0584795, Tucuxi::Core::ParameterVariabilityType::None));
    Tucuxi::Core::ParameterSetEvent parameters(DateTime::now(), parameterDefs);
    Tucuxi::Core::ParameterSetSeries parametersSeries;
    parametersSeries.addParameterSetEvent(parameters);

    testCalculator<Tucuxi::Core::TwoCompartmentInfusionMicro>(
            parametersSeries, 400.0, Tucuxi::Core::AbsorptionModel::Infusion, 12h, 1h, CYCLE_SIZE);
}

// TODO Active following test after fixing input parameters
TEST(Core_TestConcentrationCalculator, threeCompBolus)
{
    Tucuxi::Core::ParameterDefinitions parameterDefs;
    //parameterDefs.push_back(
    //        std::make_unique<Tucuxi::Core::ParameterDefinition>("F", 2, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "V1", 340, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "Ke", 0.0444294, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "K12", 0.0588235, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "K21", 0.0584795, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "K13", 0.0882353, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "K31", 0.0877193, Tucuxi::Core::ParameterVariabilityType::None));
    Tucuxi::Core::ParameterSetEvent parameters(DateTime::now(), parameterDefs);
    Tucuxi::Core::ParameterSetSeries parametersSeries;
    parametersSeries.addParameterSetEvent(parameters);

    testCalculator<Tucuxi::Core::RkThreeCompartmentBolusMicro>(
            parametersSeries, 400.0, Tucuxi::Core::AbsorptionModel::Intravascular, 12h, 0s, CYCLE_SIZE);
}

TEST(Core_TestConcentrationCalculator, threeCompExtra)
{
    Tucuxi::Core::ParameterDefinitions parameterDefs;
    parameterDefs.push_back(
            std::make_unique<Tucuxi::Core::ParameterDefinition>("F", 2, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "V1", 340, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "Ka", 0.609, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "Ke", 0.0444294, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "K12", 0.0588235, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "K21", 0.0584795, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "K13", 0.0882353, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "K31", 0.0877193, Tucuxi::Core::ParameterVariabilityType::None));
    Tucuxi::Core::ParameterSetEvent parameters(DateTime::now(), parameterDefs);
    Tucuxi::Core::ParameterSetSeries parametersSeries;
    parametersSeries.addParameterSetEvent(parameters);

    testCalculator<Tucuxi::Core::RkThreeCompartmentExtraMicro>(
            parametersSeries, 400.0, Tucuxi::Core::AbsorptionModel::Extravascular, 12h, 0s, CYCLE_SIZE);
}

TEST(Core_TestConcentrationCalculator, threeCompInfusion)
{
    Tucuxi::Core::ParameterDefinitions parameterDefs;
    parameterDefs.push_back(
            std::make_unique<Tucuxi::Core::ParameterDefinition>("F", 2, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "V1", 340, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "Ke", 0.0444294, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "K12", 0.0588235, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "K21", 0.0584795, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "K13", 0.0882353, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "K31", 0.0877193, Tucuxi::Core::ParameterVariabilityType::None));
    Tucuxi::Core::ParameterSetEvent parameters(DateTime::now(), parameterDefs);
    Tucuxi::Core::ParameterSetSeries parametersSeries;
    parametersSeries.addParameterSetEvent(parameters);

    testCalculator<Tucuxi::Core::RkThreeCompartmentInfusionMicro>(
            parametersSeries, 400.0, Tucuxi::Core::AbsorptionModel::Infusion, 12h, 1h, CYCLE_SIZE);
}
