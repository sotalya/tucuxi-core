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

#include "tucucore/drugmodel/analyte.h"
#include "tucucore/sampleextractor.h"

#include "mocklogger.h"
#include "testutils.h"

using namespace Tucuxi::Common::Utils;
using namespace Tucuxi::Core;

namespace {

AnalyteSet makeAnalyteSet(std::initializer_list<const char*> _analyteIds)
{
    AnalyteSet analyteSet;
    for (const char* analyteId : _analyteIds) {
        auto analyte = std::make_unique<Analyte>();
        analyte->setAnalyteId(analyteId);
        analyteSet.addAnalyte(std::move(analyte));
    }
    return analyteSet;
}

} // namespace

TEST(Core_TestSampleExtractor, Standard)
{
    {
        // Test with a single sample
        SampleExtractor extractor;

        Samples samples;
        DateTime start = DATE_TIME_NO_VAR(2018, 01, 01, 8, 00, 00);
        DateTime end = DATE_TIME_NO_VAR(2018, 01, 10, 8, 00, 00);
        SampleSeries series;

        AnalyteId analyteId("theAnalyte");

        samples.push_back(
                std::make_unique<Sample>(DATE_TIME_NO_VAR(2018, 01, 02, 8, 00, 00), analyteId, 12.0, TucuUnit("ug/l")));

        ComputingStatus result = extractor.extract(samples, start, end, TucuUnit("ug/l"), series);

        ASSERT_EQ(result, ComputingStatus::Ok);
        ASSERT_EQ(series.size(), static_cast<size_t>(1));
        ASSERT_DOUBLE_EQ(series[0].getValue(), 12.0);
        ASSERT_EQ(series[0].getEventTime(), DATE_TIME_NO_VAR(2018, 01, 02, 8, 00, 00));
    }

    {
        // Test with 4 samples. The first and the last are not relevant, and the third does have mg/l units
        SampleExtractor extractor;

        Samples samples;
        DateTime start = DATE_TIME_NO_VAR(2018, 01, 01, 8, 00, 00);
        DateTime end = DATE_TIME_NO_VAR(2018, 01, 10, 8, 00, 00);
        SampleSeries series;

        AnalyteId analyteId("theAnalyte");

        // One sample is in mg/l. The result should be multiplied by 1000
        samples.push_back(
                std::make_unique<Sample>(DATE_TIME_NO_VAR(2017, 01, 02, 8, 00, 00), analyteId, 12.0, TucuUnit("ug/l")));
        samples.push_back(
                std::make_unique<Sample>(DATE_TIME_NO_VAR(2018, 01, 02, 8, 00, 00), analyteId, 10.0, TucuUnit("ug/l")));
        samples.push_back(
                std::make_unique<Sample>(DATE_TIME_NO_VAR(2018, 01, 03, 8, 00, 00), analyteId, 14.0, TucuUnit("mg/l")));
        samples.push_back(
                std::make_unique<Sample>(DATE_TIME_NO_VAR(2018, 01, 12, 8, 00, 00), analyteId, 12.0, TucuUnit("ug/l")));

        ComputingStatus result = extractor.extract(samples, start, end, TucuUnit("ug/l"), series);

        ASSERT_EQ(result, ComputingStatus::Ok);
        ASSERT_EQ(series.size(), static_cast<size_t>(2));
        ASSERT_DOUBLE_EQ(series[0].getValue(), 10.0);
        ASSERT_EQ(series[0].getEventTime(), DATE_TIME_NO_VAR(2018, 01, 02, 8, 00, 00));
        // Here we check the unit conversion
        ASSERT_DOUBLE_EQ(series[1].getValue(), 14000.0);
        ASSERT_EQ(series[1].getEventTime(), DATE_TIME_NO_VAR(2018, 01, 03, 8, 00, 00));

        {
            // Test with another final unit
            SampleSeries mgSeries;
            ComputingStatus result = extractor.extract(samples, start, end, TucuUnit("mg/l"), mgSeries);
            ASSERT_EQ(result, ComputingStatus::Ok);
            ASSERT_EQ(mgSeries.size(), static_cast<size_t>(2));
            ASSERT_DOUBLE_EQ(mgSeries[0].getValue(), 0.01);
            ASSERT_EQ(mgSeries[0].getEventTime(), DATE_TIME_NO_VAR(2018, 01, 02, 8, 00, 00));
            // Here we check the unit conversion
            ASSERT_DOUBLE_EQ(mgSeries[1].getValue(), 14.0);
            ASSERT_EQ(mgSeries[1].getEventTime(), DATE_TIME_NO_VAR(2018, 01, 03, 8, 00, 00));
        }
    }
    {
        // Test with 3 sample. All sample have bad units
        SampleExtractor extractor;

        Samples samples;
        DateTime start = DATE_TIME_NO_VAR(2018, 01, 01, 8, 00, 00);
        DateTime end = DATE_TIME_NO_VAR(2018, 01, 03, 8, 00, 00);
        SampleSeries series;

        AnalyteId analyteId("theAnalyte");

        // Use a mock logger to capture error messages instead of printing them
        Tucuxi::Common::ScopedMockLogger mockLogger;

        samples.push_back(
                std::make_unique<Sample>(DATE_TIME_NO_VAR(2018, 01, 02, 8, 00, 00), analyteId, 12.0, TucuUnit("test")));

        ComputingStatus result = extractor.extract(samples, start, end, TucuUnit("ug/l"), series);

        ASSERT_EQ(result, ComputingStatus::SampleExtractionError);
        ASSERT_EQ(series.size(), static_cast<size_t>(0));
        ASSERT_TRUE(mockLogger.hasEntry(Tucuxi::Common::LogLevel::Error, "Sample unit not handled"));

        mockLogger.clear();

        Samples samples2;

        samples2.push_back(
                std::make_unique<Sample>(DATE_TIME_NO_VAR(2018, 01, 02, 8, 00, 00), analyteId, 10.0, TucuUnit("km/l")));

        result = extractor.extract(samples2, start, end, TucuUnit("ug/l"), series);

        ASSERT_EQ(result, ComputingStatus::SampleExtractionError);
        ASSERT_EQ(series.size(), static_cast<size_t>(0));
        ASSERT_TRUE(mockLogger.hasEntry(Tucuxi::Common::LogLevel::Error, "Sample unit not handled"));

        mockLogger.clear();

        Samples samples3;

        samples3.push_back(
                std::make_unique<Sample>(DATE_TIME_NO_VAR(2018, 01, 02, 8, 00, 00), analyteId, 14.0, TucuUnit("5")));

        result = extractor.extract(samples3, start, end, TucuUnit("ug/l"), series);

        ASSERT_EQ(result, ComputingStatus::SampleExtractionError);
        ASSERT_EQ(series.size(), static_cast<size_t>(0));
        ASSERT_TRUE(mockLogger.hasEntry(Tucuxi::Common::LogLevel::Error, "Sample unit not handled"));
    }
}

TEST(Core_TestSampleExtractor, ExtractWithAnalyteSetInvalidUnitReturnsOkAndLogs)
{
    SampleExtractor extractor;
    Samples samples;
    DateTime start = DATE_TIME_NO_VAR(2018, 01, 01, 8, 00, 00);
    DateTime end = DATE_TIME_NO_VAR(2018, 01, 03, 8, 00, 00);
    SampleSeries series;

    AnalyteSet analyteSet = makeAnalyteSet({"theAnalyte"});

    samples.push_back(std::make_unique<Sample>(
            DATE_TIME_NO_VAR(2018, 01, 02, 8, 00, 00), AnalyteId("theAnalyte"), 12.0, TucuUnit("invalid")));

    Tucuxi::Common::ScopedMockLogger mockLogger;

    ComputingStatus result = extractor.extract(samples, &analyteSet, start, end, TucuUnit("ug/l"), series);

    ASSERT_EQ(result, ComputingStatus::Ok);
    ASSERT_EQ(series.size(), static_cast<size_t>(0));
    ASSERT_TRUE(mockLogger.hasEntry(Tucuxi::Common::LogLevel::Error, "Sample unit not handled"));
}

TEST(Core_TestSampleExtractor, ExtractWithAnalyteSetRejectsMultiAnalyteSamples)
{
    SampleExtractor extractor;
    Samples samples;
    DateTime start = DATE_TIME_NO_VAR(2018, 01, 01, 8, 00, 00);
    DateTime end = DATE_TIME_NO_VAR(2018, 01, 05, 8, 00, 00);
    SampleSeries series;

    AnalyteSet analyteSet = makeAnalyteSet({"analyteA", "analyteB"});

    samples.push_back(std::make_unique<Sample>(
            DATE_TIME_NO_VAR(2018, 01, 02, 8, 00, 00), AnalyteId("analyteA"), 10.0, TucuUnit("ug/l")));
    samples.push_back(std::make_unique<Sample>(
            DATE_TIME_NO_VAR(2018, 01, 03, 8, 00, 00), AnalyteId("analyteB"), 11.0, TucuUnit("ug/l")));

    Tucuxi::Common::ScopedMockLogger mockLogger;

    ComputingStatus result = extractor.extract(samples, &analyteSet, start, end, TucuUnit("ug/l"), series);

    ASSERT_EQ(result, ComputingStatus::SampleExtractionError);
    ASSERT_EQ(series.size(), static_cast<size_t>(2));
    ASSERT_TRUE(mockLogger.hasEntry(Tucuxi::Common::LogLevel::Error, "Multi-analytes measures are not yet supported"));
}

TEST(Core_TestSampleExtractor, ExtractRejectsMultiAnalyteSamples)
{
    SampleExtractor extractor;
    Samples samples;
    DateTime start = DATE_TIME_NO_VAR(2018, 01, 01, 8, 00, 00);
    DateTime end = DATE_TIME_NO_VAR(2018, 01, 05, 8, 00, 00);
    SampleSeries series;

    samples.push_back(std::make_unique<Sample>(
            DATE_TIME_NO_VAR(2018, 01, 02, 8, 00, 00), AnalyteId("analyteA"), 10.0, TucuUnit("ug/l")));
    samples.push_back(std::make_unique<Sample>(
            DATE_TIME_NO_VAR(2018, 01, 03, 8, 00, 00), AnalyteId("analyteB"), 11.0, TucuUnit("ug/l")));

    Tucuxi::Common::ScopedMockLogger mockLogger;

    ComputingStatus result = extractor.extract(samples, start, end, TucuUnit("ug/l"), series);

    ASSERT_EQ(result, ComputingStatus::SampleExtractionError);
    ASSERT_EQ(series.size(), static_cast<size_t>(2));
    ASSERT_TRUE(mockLogger.hasEntry(Tucuxi::Common::LogLevel::Error, "Multi-analytes measures are not yet supported"));
}
