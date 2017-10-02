/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TEST_COVARIATEEXTRACTOR_H
#define TEST_COVARIATEEXTRACTOR_H

#include <chrono>
#include <iostream>
#include <memory>

#include <date/date.h>

#include "fructose/fructose.h"

#include "tucucore/covariateextractor.h"
#include "tucucore/operation.h"

#include "tucucore/drugmodel/covariatedefinition.h"
#include "tucucore/drugtreatment/patientcovariate.h"

#include "tucucommon/basetypes.h"

using namespace Tucuxi::Core;

#define DATE_TIME_NO_VAR(YY, MM, DD, hh, mm, ss)                                                            \
    Tucuxi::Common::DateTime(date::year_month_day(date::year(YY), date::month(MM), date::day(DD)),          \
                             Tucuxi::Common::TimeOfDay(Tucuxi::Common::Duration(std::chrono::hours(hh),     \
                                                                                std::chrono::minutes(mm),   \
                                                                                std::chrono::seconds(ss))))

#define DATE_TIME_VAR(varName, YY, MM, DD, hh, mm, ss)                                                              \
    Tucuxi::Common::DateTime varName(date::year_month_day(date::year(YY), date::month(MM), date::day(DD)),          \
                                     Tucuxi::Common::TimeOfDay(Tucuxi::Common::Duration(std::chrono::hours(hh),     \
                                                                                        std::chrono::minutes(mm),   \
                                                                                        std::chrono::seconds(ss))))

bool covariateEventIsPresent(const std::string &_id,
                             const DateTime &_date,
                             const Value &_value,
                             const CovariateSeries &_series)
{
    for (const auto &covEl : _series) {
        if (covEl.getId() == _id && fabs(covEl.getValue() - _value) < 1e-4 && covEl.getEventTime() == _date) {
            return true;
        }
    }
    return false;
}

void printCovariateSeries(const CovariateSeries &_series)
{
    std::cerr << "--------- " << _series.size() << " ----------\n";
    for (const auto &covEl : _series) {
        std::cerr << covEl.getId() << " @ "
                  << covEl.getEventTime().day() << "."
                  << covEl.getEventTime().month() << "."
                  << covEl.getEventTime().year() << " "
                  << covEl.getEventTime().hour() << "h"
                  << covEl.getEventTime().minute() << "m"
                  << covEl.getEventTime().second() << "s"
                  << " = " << covEl.getValue() << "\n";
    }
    std::cerr << "==============================================\n";
}

struct TestCovariateExtractor : public fructose::test_base<TestCovariateExtractor>
{

    TestCovariateExtractor() { }

    /// \brief Test covariate extraction using the example presented in the specs.
    void testCovariateExtraction(const std::string& /* _testName */)
    {
        std::unique_ptr<CovariateDefinition> gist(new CovariateDefinition("Gist", 0, nullptr, CovariateType::Standard, DataType::Bool));
        gist->setInterpolationType(InterpolationType::Direct);

        std::unique_ptr<CovariateDefinition> weight(new CovariateDefinition("Weight", 3.5, nullptr));
        weight->setRefreshPeriod(Tucuxi::Common::days(1));
        weight->setInterpolationType(InterpolationType::Linear);
        weight->setUnit(Unit("kg"));

        std::unique_ptr<CovariateDefinition> isMale(new CovariateDefinition("IsMale", 1, nullptr, CovariateType::Standard, DataType::Bool));

        /// \todo Change the plain-old-pointer below when unique_ptr will be in place!
        Operation *opSpecial =
                new JSOperation("Weight * 0.5 + IsMale * 15",
        { OperationInput("Weight", InputType::DOUBLE), OperationInput("IsMale", InputType::DOUBLE)});
        std::unique_ptr<CovariateDefinition> special(new CovariateDefinition("Special", 17.0, opSpecial));

        CovariateDefinitions cDefinitions;
        cDefinitions.push_back(std::move(gist));
        cDefinitions.push_back(std::move(weight));
        cDefinitions.push_back(std::move(isMale));
        cDefinitions.push_back(std::move(special));

        const DATE_TIME_VAR(startDate, 2017, 8, 12, 8, 0, 0);
        const DATE_TIME_VAR(endDate,   2017, 8, 17, 8, 0, 0);

        // -- Patient covariates #1 --

        // gist == true @ 13.08.2017, 12h32.
        std::unique_ptr<PatientCovariate> patient_gist_1(new PatientCovariate("Gist", "1", DataType::Bool, Unit(),
                                                                              DATE_TIME_NO_VAR(2017, 8, 13, 12, 32, 0)));
        // gist == false @ 13.08.2017, 14h32.
        std::unique_ptr<PatientCovariate> patient_gist_2(new PatientCovariate("Gist", "0", DataType::Bool, Unit(),
                                                                               DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
        PatientVariates pVariates1;
        pVariates1.push_back(std::move(patient_gist_1));
        pVariates1.push_back(std::move(patient_gist_2));

        // Test 1: call the extractor with the two patient covariates above.
        // We expect the first one to be "back-propagated" to the beginning of the interval.

        std::cerr << "******************** T1 ******************** \n";

        CovariateExtractor extractor;
        CovariateSeries series;
        int rc;
        rc = extractor.extract(cDefinitions, pVariates1, startDate, endDate, series);

        printCovariateSeries(series);

        fructose_assert(series.size() == 5);
        fructose_assert(rc == 0);
        // First measure propagated back to the start of the interval.
        fructose_assert(covariateEventIsPresent("Gist",
                                                DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0),
                                                1,
                                                series));
        // Second measure at the moment it is performed.
        fructose_assert(covariateEventIsPresent("Gist",
                                                DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0),
                                                0,
                                                series));
        // Remaining values propagated back to the start of the interval.
        fructose_assert(covariateEventIsPresent("Weight",
                                                DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0),
                                                3.5,
                                                series));
        fructose_assert(covariateEventIsPresent("IsMale",
                                                DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0),
                                                1,
                                                series));
        // This should be computed from IsMale and Weight.
        fructose_assert(covariateEventIsPresent("Special",
                                                DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0),
                                                16.75,
                                                series));

        /// \todo test1 with first event before start interval -> must start with one!
        /// \todo test1 with second event also before start interval -> must start with zero!

        // Test 2: call the extractor with no patient covariates.

        std::cerr << "******************** T2 ******************** \n";

        PatientVariates pVariates2;
        series.clear();
        rc = extractor.extract(cDefinitions, pVariates2, startDate, endDate, series);

        printCovariateSeries(series);

        fructose_assert(series.size() == 4);
        fructose_assert(rc == 0);
        // All events should have the initial default value.
        fructose_assert(covariateEventIsPresent("Gist",
                                                DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0),
                                                0,
                                                series));
        fructose_assert(covariateEventIsPresent("Weight",
                                                DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0),
                                                3.5,
                                                series));
        fructose_assert(covariateEventIsPresent("IsMale",
                                                DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0),
                                                1,
                                                series));
        fructose_assert(covariateEventIsPresent("Special",
                                                DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0),
                                                16.75,
                                                series));

        // Test 3: add gist and model weight and special changes.

        std::cerr << "******************** T3 ******************** \n";

        // gist == true @ 13.08.2017, 12h32.
        std::unique_ptr<PatientCovariate> patient_gist_3(new PatientCovariate("Gist", "1", DataType::Bool, Unit("kg"),
                                                                              DATE_TIME_NO_VAR(2017, 8, 13, 12, 32, 0)));

        // weight = 3.8 @ 13.08.2017, 9h00.
        std::unique_ptr<PatientCovariate> patient_weight_1(new PatientCovariate("Weight", "3.8", DataType::Double, Unit("kg"),
                                                                                DATE_TIME_NO_VAR(2017, 8, 13, 9, 0, 0)));
        // weight = 4.05 @ 15.08.2017, 21h00.
        std::unique_ptr<PatientCovariate> patient_weight_2(new PatientCovariate("Weight", "4.05", DataType::Double, Unit("kg"),
                                                                                DATE_TIME_NO_VAR(2017, 8, 15, 21, 0, 0)));
        // weight = 4.25 @ 16.08.2017, 21h00.
        std::unique_ptr<PatientCovariate> patient_weight_3(new PatientCovariate("Weight", "4.25", DataType::Double, Unit("kg"),
                                                                                DATE_TIME_NO_VAR(2017, 8, 16, 21, 0, 0)));
        PatientVariates pVariates3;
        pVariates3.push_back(std::move(patient_gist_3));
        pVariates3.push_back(std::move(patient_weight_1));
        pVariates3.push_back(std::move(patient_weight_2));
        pVariates3.push_back(std::move(patient_weight_3));

        series.clear();
        rc = extractor.extract(cDefinitions, pVariates3, startDate, endDate, series);

        printCovariateSeries(series);

        fructose_assert(series.size() == 12);
        fructose_assert(rc == 0);

        fructose_assert(covariateEventIsPresent("Gist",
                                                DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0),
                                                1,
                                                series));
        fructose_assert(covariateEventIsPresent("IsMale",
                                                DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0),
                                                1,
                                                series));
        // Since the first measure is past the beginning of the interval, we expect a value
        // at the beginning of the interval (the refresh period would have put this value one
        // day before, but that could have been weird).
        fructose_assert(covariateEventIsPresent("Weight",
                                                DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0),
                                                3.8,
                                                series));
        fructose_assert(covariateEventIsPresent("Weight",
                                                DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0),
                                                3.89583,
                                                series));
        fructose_assert(covariateEventIsPresent("Weight",
                                                DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0),
                                                3.99583,
                                                series));
        fructose_assert(covariateEventIsPresent("Weight",
                                                DATE_TIME_NO_VAR(2017, 8, 16, 8, 0, 0),
                                                4.14167,
                                                series));
        fructose_assert(covariateEventIsPresent("Weight",
                                                DATE_TIME_NO_VAR(2017, 8, 17, 8, 0, 0),
                                                4.25,
                                                series));
        // Computed value at the beginning of the interval.
        fructose_assert(covariateEventIsPresent("Special",
                                                DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0),
                                                16.9,
                                                series));
        fructose_assert(covariateEventIsPresent("Special",
                                                DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0),
                                                16.9479,
                                                series));
        fructose_assert(covariateEventIsPresent("Special",
                                                DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0),
                                                16.9979,
                                                series));
        fructose_assert(covariateEventIsPresent("Special",
                                                DATE_TIME_NO_VAR(2017, 8, 16, 8, 0, 0),
                                                17.0708,
                                                series));
        fructose_assert(covariateEventIsPresent("Special",
                                                DATE_TIME_NO_VAR(2017, 8, 17, 8, 0, 0),
                                                17.125,
                                                series));
    }

};

#endif // TEST_COVARIATEEXTRACTOR_H
