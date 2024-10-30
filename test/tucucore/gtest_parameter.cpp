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


#include <gtest/gtest.h>

#include "tucucore/drugmodel/parameterdefinition.h"
#include "tucucore/parameter.h"

using namespace Tucuxi::Core;

TEST(Core_TestParameter, ApplyEta)
{
    {
        // Test Proportional variability
        ParameterDefinition pDef(
                "pid", 10.0, std::make_unique<ParameterVariability>(ParameterVariabilityType::Proportional, 1.0));
        Parameter p(pDef, 5.0);
        bool valid = p.applyEta(2.0);
        ASSERT_TRUE(valid);
        ASSERT_EQ(p.getValue(), 15.0);
    }
    {
        // Test Exponential variability
        ParameterDefinition pDef(
                "pid", 10.0, std::make_unique<ParameterVariability>(ParameterVariabilityType::Exponential, 1.0));
        Parameter p(pDef, 5.0);
        bool valid = p.applyEta(2.0);
        ASSERT_TRUE(valid);
        ASSERT_EQ(p.getValue(), 5.0 * std::exp(2.0));
    }
    {
        // Test LogNormal variability
        ParameterDefinition pDef(
                "pid", 10.0, std::make_unique<ParameterVariability>(ParameterVariabilityType::LogNormal, 1.0));
        Parameter p(pDef, 5.0);
        bool valid = p.applyEta(2.0);
        ASSERT_TRUE(valid);
        ASSERT_EQ(p.getValue(), 5.0 * std::exp(2.0));
    }
    {
        // Test Normal variability
        ParameterDefinition pDef(
                "pid", 10.0, std::make_unique<ParameterVariability>(ParameterVariabilityType::Normal, 1.0));
        Parameter p(pDef, 5.0);
        bool valid = p.applyEta(2.0);
        ASSERT_TRUE(valid);
        ASSERT_EQ(p.getValue(), 7.0);
    }
    {
        // Test logit variability
        ParameterDefinition pDef(
                "pid", 0.5, std::make_unique<ParameterVariability>(ParameterVariabilityType::Logit, 1.0));
        Parameter p(pDef, 0.6);
        bool valid = p.applyEta(2.0);
        ASSERT_TRUE(valid);
        ASSERT_EQ(p.getValue(), 1.0 / (1.0 + std::exp(-(std::log(0.6 / (1.0 - 0.6)) + 2.0))));
    }
    {
        // Test logit variability with wrong parameter value

        Tucuxi::Common::LoggerHelper logHelper;
        logHelper.disable();
        ParameterDefinition pDef(
                "pid", 0.5, std::make_unique<ParameterVariability>(ParameterVariabilityType::Logit, 1.0));
        Parameter p(pDef, 1.6);
        bool valid = p.applyEta(2.0);
        ASSERT_FALSE(valid);
        logHelper.enable();
    }
}

TEST(Core_TestParameter, GetTime)
{
    ParameterSetSeries series;

    Tucuxi::Core::ParameterDefinitions parameterDefs;
    Tucuxi::Core::ParameterDefinition def("F", 100, Tucuxi::Core::ParameterVariabilityType::None);
    Tucuxi::Core::ParameterSetEvent parameters0(
            DateTime::fromDurationSinceEpoch(Tucuxi::Common::Duration(std::chrono::minutes(200))), parameterDefs);
    parameters0.addParameterEvent(def, 200);
    Tucuxi::Core::ParameterSetEvent parameters1(
            DateTime::fromDurationSinceEpoch(Tucuxi::Common::Duration(std::chrono::minutes(300))), parameterDefs);
    parameters1.addParameterEvent(def, 300);
    Tucuxi::Core::ParameterSetEvent parameters2(
            DateTime::fromDurationSinceEpoch(Tucuxi::Common::Duration(std::chrono::minutes(400))), parameterDefs);
    parameters2.addParameterEvent(def, 400);
    Tucuxi::Core::ParameterSetSeries parametersSeries;
    parametersSeries.addParameterSetEvent(parameters0);
    parametersSeries.addParameterSetEvent(parameters1);
    parametersSeries.addParameterSetEvent(parameters2);
    {
        auto ps = parametersSeries.getAtTime(
                DateTime::fromDurationSinceEpoch(Tucuxi::Common::Duration(std::chrono::minutes(200))));
        for (auto p = ps->begin(); p < ps->end(); p++) {
            ASSERT_EQ(ps->getValue(ParameterId::F), 200);
        }
    }
    {
        auto ps = parametersSeries.getAtTime(
                DateTime::fromDurationSinceEpoch(Tucuxi::Common::Duration(std::chrono::minutes(250))));
        for (auto p = ps->begin(); p < ps->end(); p++) {
            ASSERT_EQ(ps->getValue(ParameterId::F), 200);
        }
    }
    {
        auto ps = parametersSeries.getAtTime(
                DateTime::fromDurationSinceEpoch(Tucuxi::Common::Duration(std::chrono::minutes(300))));
        for (auto p = ps->begin(); p < ps->end(); p++) {
            ASSERT_EQ(ps->getValue(ParameterId::F), 300);
        }
    }
    {
        auto ps = parametersSeries.getAtTime(
                DateTime::fromDurationSinceEpoch(Tucuxi::Common::Duration(std::chrono::minutes(350))));
        for (auto p = ps->begin(); p < ps->end(); p++) {
            ASSERT_EQ(ps->getValue(ParameterId::F), 300);
        }
    }
    {
        auto ps = parametersSeries.getAtTime(
                DateTime::fromDurationSinceEpoch(Tucuxi::Common::Duration(std::chrono::minutes(400))));
        for (auto p = ps->begin(); p < ps->end(); p++) {
            ASSERT_EQ(ps->getValue(ParameterId::F), 400);
        }
    }
}
