#ifndef TEST_SAMPLEEXTRACTOR_H
#define TEST_SAMPLEEXTRACTOR_H


#include <chrono>
#include <iostream>
#include <memory>

#include <date/date.h>

#include "fructose/fructose.h"

#include "tucucore/sampleextractor.h"
#include "tucucore/operation.h"

#include "tucucore/drugmodel/covariatedefinition.h"
#include "tucucore/drugtreatment/patientcovariate.h"

#include "tucucommon/loggerhelper.h"
#include "tucucommon/basetypes.h"
#include "tucucommon/timeofday.h"

#include "testutils.h"

using namespace Tucuxi::Common::Utils;
using namespace Tucuxi::Core;


struct TestSampleExtractor : public fructose::test_base<TestSampleExtractor>
{
    TestSampleExtractor() { }

    void testStandard(const std::string& /* _testName */)
    {
        {
            // Test with a single sample
            SampleExtractor extractor;

            Samples samples;
            DateTime start = DATE_TIME_NO_VAR(2018, 01, 01, 8, 00, 00);
            DateTime end = DATE_TIME_NO_VAR(2018, 01, 10, 8, 00, 00);
            SampleSeries series;

            AnalyteId analyteId("theAnalyte");

            samples.push_back(std::make_unique<Sample>(DATE_TIME_NO_VAR(2018, 01, 02, 8, 00, 00), analyteId, 12.0, TucuUnit("ug/l")));

            ComputingStatus result = extractor.extract(samples, start, end, TucuUnit("ug/l"), series);

            fructose_assert(result == ComputingStatus::Ok);
            fructose_assert_eq(series.size(), size_t{1});
            fructose_assert_eq(series[0].getValue(), 12.0);
            fructose_assert_eq(series[0].getEventTime(), DATE_TIME_NO_VAR(2018, 01, 02, 8, 00, 00));
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
            samples.push_back(std::make_unique<Sample>(DATE_TIME_NO_VAR(2017, 01, 02, 8, 00, 00), analyteId, 12.0, TucuUnit("ug/l")));
            samples.push_back(std::make_unique<Sample>(DATE_TIME_NO_VAR(2018, 01, 02, 8, 00, 00), analyteId, 10.0, TucuUnit("ug/l")));
            samples.push_back(std::make_unique<Sample>(DATE_TIME_NO_VAR(2018, 01, 03, 8, 00, 00), analyteId, 14.0, TucuUnit("mg/l")));
            samples.push_back(std::make_unique<Sample>(DATE_TIME_NO_VAR(2018, 01, 12, 8, 00, 00), analyteId, 12.0, TucuUnit("ug/l")));

            ComputingStatus result = extractor.extract(samples, start, end, TucuUnit("ug/l"), series);

            fructose_assert(result == ComputingStatus::Ok);
            fructose_assert_eq(series.size(), size_t{2});
            fructose_assert_double_eq(series[0].getValue(), 10.0);
            fructose_assert_eq(series[0].getEventTime(), DATE_TIME_NO_VAR(2018, 01, 02, 8, 00, 00));
            // Here we check the unit conversion
            fructose_assert_double_eq(series[1].getValue(), 14000.0);
            fructose_assert_eq(series[1].getEventTime(), DATE_TIME_NO_VAR(2018, 01, 03, 8, 00, 00));

            {
                // Test with another final unit
                SampleSeries mgSeries;
                ComputingStatus result = extractor.extract(samples, start, end, TucuUnit("mg/l"), mgSeries);
                fructose_assert(result == ComputingStatus::Ok);
                fructose_assert_eq(mgSeries.size(), size_t{2});
                fructose_assert_double_eq(mgSeries[0].getValue(), 0.01);
                fructose_assert_eq(mgSeries[0].getEventTime(), DATE_TIME_NO_VAR(2018, 01, 02, 8, 00, 00));
                // Here we check the unit conversion
                fructose_assert_double_eq(mgSeries[1].getValue(), 14.0);
                fructose_assert_eq(mgSeries[1].getEventTime(), DATE_TIME_NO_VAR(2018, 01, 03, 8, 00, 00));
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

            // First disable the logger
            Tucuxi::Common::LoggerHelper logHelper;
            logHelper.disable();

            samples.push_back(std::make_unique<Sample>(DATE_TIME_NO_VAR(2018, 01, 02, 8, 00, 00), analyteId, 12.0, TucuUnit("test")));

            ComputingStatus result = extractor.extract(samples, start, end, TucuUnit("ug/l"), series);

            fructose_assert(result == ComputingStatus::SampleExtractionError);
            fructose_assert_eq(series.size(), size_t{0});

            Samples samples2;

            samples2.push_back(std::make_unique<Sample>(DATE_TIME_NO_VAR(2018, 01, 02, 8, 00, 00), analyteId, 10.0, TucuUnit("km/l")));

            result = extractor.extract(samples2, start, end, TucuUnit("ug/l"), series);

            fructose_assert(result == ComputingStatus::SampleExtractionError);
            fructose_assert_eq(series.size(), size_t{0});

            Samples samples3;

            samples3.push_back(std::make_unique<Sample>(DATE_TIME_NO_VAR(2018, 01, 02, 8, 00, 00), analyteId, 14.0, TucuUnit("5")));

            result = extractor.extract(samples3, start, end, TucuUnit("ug/l"), series);

            fructose_assert(result == ComputingStatus::SampleExtractionError);
            fructose_assert_eq(series.size(), size_t{0});

            // Enable the logger
            logHelper.enable();
        }
    }

};


#endif // TEST_SAMPLEEXTRACTOR_H
