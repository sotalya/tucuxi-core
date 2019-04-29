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

            std::string analyteId = "theAnalyte";

            samples.push_back(std::make_unique<Sample>(DATE_TIME_NO_VAR(2018, 01, 02, 8, 00, 00), analyteId, 12.0, Unit("ug/l")));

            ComputingResult result = extractor.extract(samples, start, end, series);

            fructose_assert(result == ComputingResult::Ok);
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

            std::string analyteId = "theAnalyte";

            // One sample is in mg/l. The result should be multiplied by 1000
            samples.push_back(std::make_unique<Sample>(DATE_TIME_NO_VAR(2017, 01, 02, 8, 00, 00), analyteId, 12.0, Unit("ug/l")));
            samples.push_back(std::make_unique<Sample>(DATE_TIME_NO_VAR(2018, 01, 02, 8, 00, 00), analyteId, 10.0, Unit("ug/l")));
            samples.push_back(std::make_unique<Sample>(DATE_TIME_NO_VAR(2018, 01, 03, 8, 00, 00), analyteId, 14.0, Unit("mg/l")));
            samples.push_back(std::make_unique<Sample>(DATE_TIME_NO_VAR(2018, 01, 12, 8, 00, 00), analyteId, 12.0, Unit("ug/l")));

            ComputingResult result = extractor.extract(samples, start, end, series);

            fructose_assert(result == ComputingResult::Ok);
            fructose_assert_eq(series.size(), size_t{2});
            fructose_assert_eq(series[0].getValue(), 10.0);
            fructose_assert_eq(series[0].getEventTime(), DATE_TIME_NO_VAR(2018, 01, 02, 8, 00, 00));
            // Here we check the unit conversion
            fructose_assert_eq(series[1].getValue(), 14000.0);
            fructose_assert_eq(series[1].getEventTime(), DATE_TIME_NO_VAR(2018, 01, 03, 8, 00, 00));
        }
    }

};


#endif // TEST_SAMPLEEXTRACTOR_H
