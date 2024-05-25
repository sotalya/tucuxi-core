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

#include <date/date.h>
#include <gtest/gtest.h>

#include "tucucore/targetextractor.h"

using namespace Tucuxi::Common::Utils;
using namespace Tucuxi::Core;

class TestTargetExtractor
{
public:
    static Value get_m_valueMin(TargetEvent* targetResult)
    {
        return targetResult->m_valueMin;
    };

    static Value get_m_valueMax(TargetEvent* targetResult)
    {
        return targetResult->m_valueMax;
    };

    static Value get_m_valueBest(TargetEvent* targetResult)
    {
        return targetResult->m_valueBest;
    };
};

TEST(Core_TestTargetExtractor, AprioriValues)
{
    TargetExtractor extractor;


    CovariateSeries covariates;
    TargetDefinitions targetDefinitions;
    Targets targets;
    DateTime start = DateTime::now();
    DateTime end = DateTime::now();
    TargetExtractionOption extractionOption;
    TargetSeries series;

    ComputingStatus result;

    {
        extractionOption = TargetExtractionOption::AprioriValues;

        result = extractor.extract(
                ActiveMoietyId("NoId"),
                covariates,
                targetDefinitions,
                targets,
                start,
                end,
                TucuUnit("ug/l"),
                extractionOption,
                series);

        // Not yet implemented, so extraction error
        ASSERT_EQ(result, ComputingStatus::TargetExtractionError);
    }
}

TEST(Core_TestTargetExtractor, PopulationValues)
{
    TargetExtractor extractor;


    CovariateSeries covariates;
    TargetDefinitions targetDefinitions;
    Targets targets;
    DateTime start = DateTime::now();
    DateTime end = DateTime::now();
    TargetExtractionOption extractionOption;
    TargetSeries series;


    ComputingStatus result;

    {
        // Test with only population values

        // Add targets
        auto target = std::make_unique<TargetDefinition>(
                TargetType::Residual,
                TucuUnit("ug/l"),
                ActiveMoietyId("imatinib"),
                std::make_unique<SubTargetDefinition>("cMin", 750.0, nullptr),
                std::make_unique<SubTargetDefinition>("cMax", 1500.0, nullptr),
                std::make_unique<SubTargetDefinition>("cBest", 1000.0, nullptr),
                std::make_unique<SubTargetDefinition>("mic", 2.0, nullptr),
                std::make_unique<SubTargetDefinition>("tMin", 1000.0, nullptr),
                std::make_unique<SubTargetDefinition>("tMax", 1200.0, nullptr),
                std::make_unique<SubTargetDefinition>("tBest", 1100.0, nullptr),
                std::make_unique<SubTargetDefinition>("toxicity", 10000.0, nullptr),
                std::make_unique<SubTargetDefinition>("inefficacy", 1.0, nullptr));


        targetDefinitions.push_back(std::move(target));

        extractionOption = TargetExtractionOption::PopulationValues;
        result = extractor.extract(
                ActiveMoietyId("imatinib"),
                covariates,
                targetDefinitions,
                targets,
                start,
                end,
                TucuUnit("ug/l"),
                extractionOption,
                series);
        ASSERT_EQ(result, ComputingStatus::Ok);

        ASSERT_EQ(series.size(), static_cast<size_t>(1));
        {
            TargetEvent targetResult = series[0];

            ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_valueMin(&targetResult), 750.0);
            ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_valueMax(&targetResult), 1500.0);
            ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_valueBest(&targetResult), 1000.0);
            // The MIC is not defined in a target event Residual
            //ASSERT_DOUBLE_EQ( targetResult.m_mic, 2.0);
            //ASSERT_DOUBLE_EQ( targetResult.m_tMin, 1000.0);
            //ASSERT_DOUBLE_EQ( targetResult.m_tMax, 1200.0);
            //ASSERT_DOUBLE_EQ( targetResult.m_tBest, 1100.0);
            //ASSERT_DOUBLE_EQ( targetResult.m_toxicity, 1100.0);
            //ASSERT_DOUBLE_EQ( targetResult.m_tBest, 1100.0);
        }
    }
}

TEST(Core_TestTargetExtractor, IndividualTargets)
{
    TargetExtractor extractor;


    CovariateSeries covariates;
    TargetDefinitions targetDefinitions;
    Targets targets;
    DateTime start = DateTime::now();
    DateTime end = DateTime::now();
    TargetExtractionOption extractionOption;
    TargetSeries series;

    ComputingStatus result;


    {
        // Test with individual values

        // Add targets
        auto target = std::make_unique<TargetDefinition>(
                TargetType::Residual,
                TucuUnit("mg/l"),
                ActiveMoietyId("imatinib"),
                std::make_unique<SubTargetDefinition>("cMin", 750.0, nullptr),
                std::make_unique<SubTargetDefinition>("cMax", 1500.0, nullptr),
                std::make_unique<SubTargetDefinition>("cBest", 1000.0, nullptr),
                std::make_unique<SubTargetDefinition>("mic", 2.0, nullptr),
                std::make_unique<SubTargetDefinition>("tMin", 1000.0, nullptr),
                std::make_unique<SubTargetDefinition>("tMax", 1200.0, nullptr),
                std::make_unique<SubTargetDefinition>("tBest", 1100.0, nullptr),
                std::make_unique<SubTargetDefinition>("toxicity", 10000.0, nullptr),
                std::make_unique<SubTargetDefinition>("inefficacy", 1.0, nullptr));


        targetDefinitions.push_back(std::move(target));


        auto patientTarget = std::make_unique<Target>(
                ActiveMoietyId("imatinib"), TargetType::Residual, TucuUnit("mg/l"), 50.0, 100.0, 150.0, 2.0, 200.0);

        targets.push_back(std::move(patientTarget));

        extractionOption = TargetExtractionOption::IndividualTargets;
        result = extractor.extract(
                ActiveMoietyId("imatinib"),
                covariates,
                targetDefinitions,
                targets,
                start,
                end,
                TucuUnit("ug/l"),
                extractionOption,
                series);
        ASSERT_EQ(result, ComputingStatus::Ok);

        ASSERT_EQ(series.size(), static_cast<size_t>(1));
        {
            TargetEvent targetResult = series[0];

            // Here the values are x1000 the target values, as the units in the definition are mg/l. The final
            // target unit is in ug/l
            ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_valueMin(&targetResult), 50000.0);
            ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_valueMax(&targetResult), 150000.0);
            ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_valueBest(&targetResult), 100000.0);
            //ASSERT_DOUBLE_EQ( targetResult.m_mic, 2000.0);
            //ASSERT_DOUBLE_EQ( targetResult.m_tMin, 1000.0);
            //ASSERT_DOUBLE_EQ( targetResult.m_tMax, 1200.0);
            //ASSERT_DOUBLE_EQ( targetResult.m_tBest, 1100.0);
            //ASSERT_DOUBLE_EQ( targetResult.m_toxicity, 1100.0);
            //ASSERT_DOUBLE_EQ( targetResult.m_tBest, 1100.0);
        }
    }
}

TEST(Core_TestTargetExtractor, IndividualTargetsNoDefinition)
{
    TargetExtractor extractor;


    CovariateSeries covariates;
    TargetDefinitions targetDefinitions;
    Targets targets;
    DateTime start = DateTime::now();
    DateTime end = DateTime::now();
    TargetExtractionOption extractionOption;
    TargetSeries series;

    ComputingStatus result;


    {
        // Test with individual values

        // Add targets

        auto patientTarget = std::make_unique<Target>(
                ActiveMoietyId("imatinib"), TargetType::Residual, TucuUnit("mg/l"), 50.0, 100.0, 150.0, 2.0, 200.0);

        targets.push_back(std::move(patientTarget));

        extractionOption = TargetExtractionOption::DefinitionIfNoIndividualTarget;
        result = extractor.extract(
                ActiveMoietyId("imatinib"),
                covariates,
                targetDefinitions,
                targets,
                start,
                end,
                TucuUnit("ug/l"),
                extractionOption,
                series);
        ASSERT_EQ(result, ComputingStatus::Ok);

        ASSERT_EQ(series.size(), static_cast<size_t>(1));
        {
            TargetEvent targetResult = series[0];

            // Here the values are x1000 the target values, as the units in the definition are mg/l. The final
            // target unit is in ug/l
            ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_valueMin(&targetResult), 50000.0);
            ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_valueMax(&targetResult), 150000.0);
            ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_valueBest(&targetResult), 100000.0);
            //ASSERT_DOUBLE_EQ( targetResult.m_mic, 2000.0);
            //ASSERT_DOUBLE_EQ( targetResult.m_tMin, 1000.0);
            //ASSERT_DOUBLE_EQ( targetResult.m_tMax, 1200.0);
            //ASSERT_DOUBLE_EQ( targetResult.m_tBest, 1100.0);
            //ASSERT_DOUBLE_EQ( targetResult.m_toxicity, 1100.0);
            //ASSERT_DOUBLE_EQ( targetResult.m_tBest, 1100.0);
        }
    }
}
