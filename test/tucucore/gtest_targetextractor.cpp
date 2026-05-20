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

#include "mocklogger.h"

using namespace Tucuxi::Common::Utils;
using namespace Tucuxi::Core;

class TestTargetExtractor
{
public:
    static Value get_m_valueMin(TargetEvent* _targetResult)
    {
        return _targetResult->m_valueMin;
    };

    static Value get_m_valueMax(TargetEvent* _targetResult)
    {
        return _targetResult->m_valueMax;
    };

    static Value get_m_valueBest(TargetEvent* _targetResult)
    {
        return _targetResult->m_valueBest;
    };

    static TargetType get_m_targetType(TargetEvent* _targetResult)
    {
        return _targetResult->m_targetType;
    };

    static TucuUnit get_m_unit(TargetEvent* _targetResult)
    {
        return _targetResult->m_unit;
    };

    static TucuUnit get_m_finalUnit(TargetEvent* _targetResult)
    {
        return _targetResult->m_finalUnit;
    };

    static Value get_m_mic(TargetEvent* _targetResult)
    {
        return _targetResult->m_mic;
    };

    static TucuUnit get_m_micUnit(TargetEvent* _targetResult)
    {
        return _targetResult->m_micUnit;
    };

    static Tucuxi::Common::Duration get_m_tMin(TargetEvent* _targetResult)
    {
        return _targetResult->m_tMin;
    };

    static Tucuxi::Common::Duration get_m_tBest(TargetEvent* _targetResult)
    {
        return _targetResult->m_tBest;
    };

    static Tucuxi::Common::Duration get_m_tMax(TargetEvent* _targetResult)
    {
        return _targetResult->m_tMax;
    };
};

class TestableTargetExtractor : public TargetExtractor
{
public:
    using TargetExtractor::targetEventFromTarget;
    using TargetExtractor::targetEventFromTargetDefinition;
};

namespace {

std::unique_ptr<TargetDefinition> makeTargetDefinition(
        TargetType _type,
        const TucuUnit& _unit,
        Value _cMin,
        Value _cBest,
        Value _cMax,
        Value _mic,
        const TucuUnit& _micUnit,
        Value _tMin,
        Value _tBest,
        Value _tMax,
        const TucuUnit& _timeUnit)
{
    return std::make_unique<TargetDefinition>(
            _type,
            _unit,
            ActiveMoietyId("imatinib"),
            std::make_unique<SubTargetDefinition>("cMin", _cMin, nullptr),
            std::make_unique<SubTargetDefinition>("cMax", _cMax, nullptr),
            std::make_unique<SubTargetDefinition>("cBest", _cBest, nullptr),
            std::make_unique<SubTargetDefinition>("mic", _mic, nullptr),
            std::make_unique<SubTargetDefinition>("tMin", _tMin, nullptr),
            std::make_unique<SubTargetDefinition>("tMax", _tMax, nullptr),
            std::make_unique<SubTargetDefinition>("tBest", _tBest, nullptr),
            std::make_unique<SubTargetDefinition>("toxicity", 10000.0, nullptr),
            std::make_unique<SubTargetDefinition>("inefficacy", 1.0, nullptr),
            _micUnit,
            _timeUnit);
}

} // namespace

TEST(Core_TestTargetExtractor, TargetEventFromTargetUnknownType)
{
    Tucuxi::Common::ScopedMockLogger mockLogger;

    TestableTargetExtractor extractor;
    Target target(
            ActiveMoietyId("imatinib"),
            TargetType::UnknownTarget,
            TucuUnit("mg/l"),
            Value(1.0),
            Value(2.0),
            Value(3.0),
            Value(4.0),
            TucuUnit("ug/l"),
            Tucuxi::Common::Duration(std::chrono::minutes(10)),
            Tucuxi::Common::Duration(std::chrono::minutes(20)),
            Tucuxi::Common::Duration(std::chrono::minutes(30)));

    TargetEvent targetEvent = extractor.targetEventFromTarget(&target, TucuUnit("ug/l"));

    ASSERT_EQ(TestTargetExtractor::get_m_targetType(&targetEvent), TargetType::UnknownTarget);
    ASSERT_EQ(TestTargetExtractor::get_m_unit(&targetEvent), TucuUnit(""));
    ASSERT_EQ(TestTargetExtractor::get_m_finalUnit(&targetEvent), TucuUnit("mg/l"));
    ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_valueMin(&targetEvent), 1.0);
    ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_valueBest(&targetEvent), 2.0);
    ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_valueMax(&targetEvent), 3.0);
    ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_mic(&targetEvent), 4.0);
    ASSERT_EQ(TestTargetExtractor::get_m_micUnit(&targetEvent), TucuUnit("ug/l"));
    EXPECT_TRUE(mockLogger.hasEntry(Tucuxi::Common::LogLevel::Error, "unkown type"));
}

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

TEST(Core_TestTargetExtractor, IndividualTargetsIfDefinitionExists)
{
    TargetExtractor extractor;


    CovariateSeries covariates;
    DateTime start = DateTime::now();
    DateTime end = DateTime::now();
    TargetExtractionOption extractionOption = TargetExtractionOption::IndividualTargetsIfDefinitionExists;
    ComputingStatus result;

    // Success path
    {
        TargetDefinitions targetDefinitions;
        Targets targets;
        TargetSeries series;

        targetDefinitions.push_back(makeTargetDefinition(
                TargetType::Residual,
                TucuUnit("mg/l"),
                750.0,
                1000.0,
                1500.0,
                2.0,
                TucuUnit("mg/l"),
                1000.0,
                1100.0,
                1200.0,
                TucuUnit("min")));

        auto patientTarget = std::make_unique<Target>(
                ActiveMoietyId("imatinib"), TargetType::Residual, TucuUnit("mg/l"), 50.0, 100.0, 150.0, 2.0, 200.0);
        targets.push_back(std::move(patientTarget));

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

        TargetEvent targetResult = series[0];
        ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_valueMin(&targetResult), 50000.0);
        ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_valueMax(&targetResult), 150000.0);
        ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_valueBest(&targetResult), 100000.0);
    }

    // Error path
    {
        TargetDefinitions targetDefinitions;
        Targets targets;
        TargetSeries series;

        auto patientTarget = std::make_unique<Target>(
                ActiveMoietyId("imatinib"), TargetType::Residual, TucuUnit("mg/l"), 50.0, 100.0, 150.0, 2.0, 200.0);
        targets.push_back(std::move(patientTarget));

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
        ASSERT_EQ(result, ComputingStatus::TargetExtractionError);
        ASSERT_TRUE(series.empty());
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

TEST(Core_TestTargetExtractor, TargetEventFromTargetDefinitionResidualAndMean)
{
    TestableTargetExtractor extractor;

    for (const auto targetType : {TargetType::Residual, TargetType::Mean}) {
        auto targetDefinition = makeTargetDefinition(
                targetType, TucuUnit("mg/l"), 1.0, 2.0, 3.0, 4.0, TucuUnit("mg/l"), 1.0, 2.0, 3.0, TucuUnit("h"));

        TargetEvent targetEvent = extractor.targetEventFromTargetDefinition(targetDefinition.get(), TucuUnit("ug/l"));

        ASSERT_EQ(TestTargetExtractor::get_m_targetType(&targetEvent), targetType);
        ASSERT_EQ(TestTargetExtractor::get_m_unit(&targetEvent), TucuUnit("ug/l"));
        ASSERT_EQ(TestTargetExtractor::get_m_finalUnit(&targetEvent), TucuUnit("mg/l"));
        ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_valueMin(&targetEvent), 1000.0);
        ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_valueBest(&targetEvent), 2000.0);
        ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_valueMax(&targetEvent), 3000.0);
    }
}

TEST(Core_TestTargetExtractor, TargetEventFromTargetDefinitionPeak)
{
    TestableTargetExtractor extractor;
    auto targetDefinition = makeTargetDefinition(
            TargetType::Peak, TucuUnit("mg/l"), 1.0, 2.0, 3.0, 4.0, TucuUnit("mg/l"), 1.0, 2.0, 3.0, TucuUnit("h"));

    TargetEvent targetEvent = extractor.targetEventFromTargetDefinition(targetDefinition.get(), TucuUnit("ug/l"));

    ASSERT_EQ(TestTargetExtractor::get_m_targetType(&targetEvent), TargetType::Peak);
    ASSERT_EQ(TestTargetExtractor::get_m_unit(&targetEvent), TucuUnit("ug/l"));
    ASSERT_EQ(TestTargetExtractor::get_m_finalUnit(&targetEvent), TucuUnit("mg/l"));
    ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_valueMin(&targetEvent), 1000.0);
    ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_valueBest(&targetEvent), 2000.0);
    ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_valueMax(&targetEvent), 3000.0);
    ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_tMin(&targetEvent).toMinutes(), 60.0);
    ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_tBest(&targetEvent).toMinutes(), 120.0);
    ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_tMax(&targetEvent).toMinutes(), 180.0);
}

TEST(Core_TestTargetExtractor, TargetEventFromTargetDefinitionAucFamily)
{
    TestableTargetExtractor extractor;

    for (const auto targetType : {TargetType::Auc, TargetType::Auc24, TargetType::CumulativeAuc}) {
        auto targetDefinition = makeTargetDefinition(
                targetType, TucuUnit("mg*h/l"), 1.0, 2.0, 3.0, 4.0, TucuUnit("mg*h/l"), 1.0, 2.0, 3.0, TucuUnit("h"));

        TargetEvent targetEvent = extractor.targetEventFromTargetDefinition(targetDefinition.get(), TucuUnit("ug/l"));

        ASSERT_EQ(TestTargetExtractor::get_m_targetType(&targetEvent), targetType);
        ASSERT_EQ(TestTargetExtractor::get_m_unit(&targetEvent), TucuUnit("ug*h/l"));
        ASSERT_EQ(TestTargetExtractor::get_m_finalUnit(&targetEvent), TucuUnit("mg*h/l"));
        ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_valueMin(&targetEvent), 1000.0);
        ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_valueBest(&targetEvent), 2000.0);
        ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_valueMax(&targetEvent), 3000.0);
        ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_tMin(&targetEvent).toMinutes(), 60.0);
        ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_tBest(&targetEvent).toMinutes(), 120.0);
        ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_tMax(&targetEvent).toMinutes(), 180.0);
    }
}

TEST(Core_TestTargetExtractor, DISABLED_TargetEventFromTargetDefinitionAucOverMicFamily)
{
    TestableTargetExtractor extractor;

    for (const auto targetType : {TargetType::AucOverMic, TargetType::Auc24OverMic}) {
        auto targetDefinition = makeTargetDefinition(
                targetType, TucuUnit("mg*h/l"), 1.0, 2.0, 3.0, 4.0, TucuUnit("mg/l"), 1.0, 2.0, 3.0, TucuUnit("h"));

        TargetEvent targetEvent = extractor.targetEventFromTargetDefinition(targetDefinition.get(), TucuUnit("ug/l"));

        ASSERT_EQ(TestTargetExtractor::get_m_targetType(&targetEvent), targetType);
        ASSERT_EQ(TestTargetExtractor::get_m_unit(&targetEvent), TucuUnit("ug*h/l"));
        ASSERT_EQ(TestTargetExtractor::get_m_finalUnit(&targetEvent), TucuUnit("mg*h/l"));
        ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_valueMin(&targetEvent), 1000.0);
        ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_valueBest(&targetEvent), 2000.0);
        ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_valueMax(&targetEvent), 3000.0);
    }
}

TEST(Core_TestTargetExtractor, TargetEventFromTargetDefinitionConcentrationThrows)
{
    TestableTargetExtractor extractor;

    auto targetDefinition = makeTargetDefinition(
            TargetType::Auc, TucuUnit("mg*h/l"), 1.0, 2.0, 3.0, 4.0, TucuUnit("mg*h/l"), 1.0, 2.0, 3.0, TucuUnit("h"));

    EXPECT_THROW(
            extractor.targetEventFromTargetDefinition(targetDefinition.get(), TucuUnit("ug*h/l")),
            std::invalid_argument);
}

TEST(Core_TestTargetExtractor, TargetEventFromTargetAucDividedByMicFamily)
{
    TestableTargetExtractor extractor;


    for (const auto targetType :
         {TargetType::AucDividedByMic, TargetType::Auc24DividedByMic, TargetType::TimeOverMic}) {
        auto targetDefinition = makeTargetDefinition(
                targetType, TucuUnit("min"), 60.0, 90.0, 120.0, 4.0, TucuUnit("mg/l"), 1.0, 2.0, 3.0, TucuUnit("h"));

        TargetEvent targetEvent = extractor.targetEventFromTargetDefinition(targetDefinition.get(), TucuUnit("ug/l"));

        ASSERT_EQ(TestTargetExtractor::get_m_targetType(&targetEvent), targetType);
        ASSERT_EQ(TestTargetExtractor::get_m_unit(&targetEvent), TucuUnit("h"));
        ASSERT_EQ(TestTargetExtractor::get_m_finalUnit(&targetEvent), TucuUnit("min"));
        ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_valueMin(&targetEvent), 1.0);
        ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_valueBest(&targetEvent), 1.5);
        ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_valueMax(&targetEvent), 2.0);
    }
}

TEST(Core_TestTargetExtractor, TargetEventFromTargetPeakDividedByMicFamily)
{
    TestableTargetExtractor extractor;

    for (const auto targetType :
         {TargetType::PeakDividedByMic, TargetType::ResidualDividedByMic, TargetType::FractionTimeOverMic}) {
        Target target(
                ActiveMoietyId("imatinib"),
                targetType,
                TucuUnit(""),
                Value(1.0),
                Value(2.0),
                Value(3.0),
                Value(4.0),
                TucuUnit("mg/l"),
                Tucuxi::Common::Duration(std::chrono::minutes(60)),
                Tucuxi::Common::Duration(std::chrono::minutes(120)),
                Tucuxi::Common::Duration(std::chrono::minutes(180)));

        TargetEvent targetEvent = extractor.targetEventFromTarget(&target, TucuUnit("ug/l"));

        ASSERT_EQ(TestTargetExtractor::get_m_targetType(&targetEvent), targetType);
        ASSERT_EQ(TestTargetExtractor::get_m_unit(&targetEvent), TucuUnit(""));
        ASSERT_EQ(TestTargetExtractor::get_m_finalUnit(&targetEvent), TucuUnit(""));
        ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_valueMin(&targetEvent), 1.0);
        ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_valueBest(&targetEvent), 2.0);
        ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_valueMax(&targetEvent), 3.0);
        ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_mic(&targetEvent), 4000.0);
        // ASSERT_EQ(TestTargetExtractor::get_m_micUnit(&targetEvent), TucuUnit("ug/l"));
        ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_tMin(&targetEvent).toMinutes(), 60.0);
        ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_tBest(&targetEvent).toMinutes(), 120.0);
        ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_tMax(&targetEvent).toMinutes(), 180.0);
    }
}

TEST(Core_TestTargetExtractor, TargetEventFromTargetDefinitionPeakDividedByMicFamily)
{
    TestableTargetExtractor extractor;

    for (const auto targetType :
         {TargetType::PeakDividedByMic, TargetType::ResidualDividedByMic, TargetType::FractionTimeOverMic}) {
        auto targetDefinition = makeTargetDefinition(
                targetType, TucuUnit(""), 1.0, 2.0, 3.0, 4.0, TucuUnit("mg/l"), 1.0, 2.0, 3.0, TucuUnit("h"));

        TargetEvent targetEvent = extractor.targetEventFromTargetDefinition(targetDefinition.get(), TucuUnit("ug/l"));

        ASSERT_EQ(TestTargetExtractor::get_m_targetType(&targetEvent), targetType);
        ASSERT_EQ(TestTargetExtractor::get_m_unit(&targetEvent), TucuUnit(""));
        ASSERT_EQ(TestTargetExtractor::get_m_finalUnit(&targetEvent), TucuUnit(""));
        ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_valueMin(&targetEvent), 1.0);
        ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_valueBest(&targetEvent), 2.0);
        ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_valueMax(&targetEvent), 3.0);
        ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_mic(&targetEvent), 4000.0);
        ASSERT_EQ(TestTargetExtractor::get_m_micUnit(&targetEvent), TucuUnit("ug/l"));
        ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_tMin(&targetEvent).toMinutes(), 60.0);
        ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_tBest(&targetEvent).toMinutes(), 120.0);
        ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_tMax(&targetEvent).toMinutes(), 180.0);
    }
}

TEST(Core_TestTargetExtractor, TargetEventFromTargetDefinitionUnknownType)
{
    Tucuxi::Common::ScopedMockLogger mockLogger;

    TestableTargetExtractor extractor;
    auto targetDefinition = makeTargetDefinition(
            TargetType::UnknownTarget,
            TucuUnit("mg/l"),
            11.0,
            12.0,
            13.0,
            4.0,
            TucuUnit("mg/l"),
            1.0,
            2.0,
            3.0,
            TucuUnit("h"));

    TargetEvent targetEvent = extractor.targetEventFromTargetDefinition(targetDefinition.get(), TucuUnit("ug/l"));

    ASSERT_EQ(TestTargetExtractor::get_m_targetType(&targetEvent), TargetType::UnknownTarget);
    ASSERT_EQ(TestTargetExtractor::get_m_unit(&targetEvent), TucuUnit(""));
    ASSERT_EQ(TestTargetExtractor::get_m_finalUnit(&targetEvent), TucuUnit("mg/l"));
    ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_valueMin(&targetEvent), 11.0);
    ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_valueBest(&targetEvent), 12.0);
    ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_valueMax(&targetEvent), 13.0);
    ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_tMin(&targetEvent).toMinutes(), 60.0);
    ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_tBest(&targetEvent).toMinutes(), 120.0);
    ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_tMax(&targetEvent).toMinutes(), 180.0);
    EXPECT_TRUE(mockLogger.hasEntry(Tucuxi::Common::LogLevel::Error, "unkown type"));
}

TEST(Core_TestTargetExtractor, TargetEventFromTargetDefinitionDefaultCaseWithInvalidEnum)
{
    Tucuxi::Common::ScopedMockLogger mockLogger;

    TestableTargetExtractor extractor;
    auto invalidTargetType = static_cast<TargetType>(999);
    auto targetDefinition = makeTargetDefinition(
            invalidTargetType, TucuUnit("mg/l"), 21.0, 22.0, 23.0, 4.0, TucuUnit("mg/l"), 1.0, 2.0, 3.0, TucuUnit("h"));

    TargetEvent targetEvent = extractor.targetEventFromTargetDefinition(targetDefinition.get(), TucuUnit("ug/l"));

    ASSERT_EQ(TestTargetExtractor::get_m_targetType(&targetEvent), invalidTargetType);
    ASSERT_EQ(TestTargetExtractor::get_m_unit(&targetEvent), TucuUnit(""));
    ASSERT_EQ(TestTargetExtractor::get_m_finalUnit(&targetEvent), TucuUnit("mg/l"));
    ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_valueMin(&targetEvent), 21.0);
    ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_valueBest(&targetEvent), 22.0);
    ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_valueMax(&targetEvent), 23.0);
    EXPECT_TRUE(mockLogger.hasEntry(Tucuxi::Common::LogLevel::Error, "unkown type"));
}

TEST(Core_TestTargetExtractor, IndividualTargetsIfDefinitionExistsAndDefinitionIfNoIndividualTarget)
{
    TargetExtractor extractor;

    CovariateSeries covariates;
    TargetDefinitions targetDefinitions;
    Targets targets;
    DateTime start = DateTime::now();
    DateTime end = DateTime::now();
    TargetSeries series;

    // Definition for Residual — a matching patient target will be provided (branch 1).
    targetDefinitions.push_back(makeTargetDefinition(
            TargetType::Residual,
            TucuUnit("mg/l"),
            750.0,
            1000.0,
            1500.0,
            2.0,
            TucuUnit("mg/l"),
            1000.0,
            1100.0,
            1200.0,
            TucuUnit("min")));

    // Definition for Peak — no patient target provided, so it falls back to the definition (branch 2).
    targetDefinitions.push_back(makeTargetDefinition(
            TargetType::Peak,
            TucuUnit("mg/l"),
            100.0,
            200.0,
            300.0,
            2.0,
            TucuUnit("mg/l"),
            60.0,
            120.0,
            180.0,
            TucuUnit("min")));

    // Patient target only for Residual (branch 1).
    auto patientTarget = std::make_unique<Target>(
            ActiveMoietyId("imatinib"), TargetType::Residual, TucuUnit("mg/l"), 50.0, 100.0, 150.0, 2.0, 200.0);
    targets.push_back(std::move(patientTarget));

    ComputingStatus result = extractor.extract(
            ActiveMoietyId("imatinib"),
            covariates,
            targetDefinitions,
            targets,
            start,
            end,
            TucuUnit("ug/l"),
            TargetExtractionOption::IndividualTargetsIfDefinitionExistsAndDefinitionIfNoIndividualTarget,
            series);

    ASSERT_EQ(result, ComputingStatus::Ok);
    ASSERT_EQ(series.size(), static_cast<size_t>(2));

    // First event: built from the individual Residual target — values come from the patient target (mg/l → ug/l).
    TargetEvent residualEvent = series[0];
    ASSERT_EQ(TestTargetExtractor::get_m_targetType(&residualEvent), TargetType::Residual);
    ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_valueMin(&residualEvent), 50000.0);
    ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_valueBest(&residualEvent), 100000.0);
    ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_valueMax(&residualEvent), 150000.0);

    // Second event: built from the Peak definition (no patient target) — values come from the definition.
    TargetEvent peakEvent = series[1];
    ASSERT_EQ(TestTargetExtractor::get_m_targetType(&peakEvent), TargetType::Peak);
    ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_valueMin(&peakEvent), 100000.0);
    ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_valueBest(&peakEvent), 200000.0);
    ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_valueMax(&peakEvent), 300000.0);
    ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_tMin(&peakEvent).toMinutes(), 60.0);
    ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_tBest(&peakEvent).toMinutes(), 120.0);
    ASSERT_DOUBLE_EQ(TestTargetExtractor::get_m_tMax(&peakEvent).toMinutes(), 180.0);
}
