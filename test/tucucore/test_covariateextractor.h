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
#include "tucucommon/timeofday.h"

#include "testutils.h"

using namespace Tucuxi::Common::Utils;
using namespace Tucuxi::Core;

/// \brief Check whether a covariate event with a given name, date, and value is present in a series.
/// \param _id Name of the covariate to check.
/// \param _date Expected date of the covariate event.
/// \param _value Expected value of the covariate at the time of the event.
/// \param _series Vector of events in which the event is sought.
/// \return True if the event is present, false otherwise.
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


/// \brief Print the elements of a covariate series.
/// \param _series Series to print.
void printCovariateSeries(const CovariateSeries &_series)
{
    std::cerr << "--------- " << _series.size() << " ----------\n";
    for (const auto &covEl : _series) {
        std::cerr << covEl.getId() << " @ "
                  << covEl.getEventTime();
        std::cerr << " = " << covEl.getValue() << "\n";
    }
    std::cerr << "-----------------------------------------\n";
}


struct TestCovariateExtractor : public fructose::test_base<TestCovariateExtractor>
{
    TestCovariateExtractor() { }

    /// \brief Check that objects are correctly constructed by the constructor.
    void testCE_constructor(const std::string& /* _testName */)
    {
        // Even without covariates, no exception should be raised.
        {
            fructose_assert_no_exception(CovariateExtractor(CovariateDefinitions(), PatientVariates(),
                                                            DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0),
                                                            DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0)));
        }

        // Start date past end date.
        {
            fructose_assert_exception(CovariateExtractor(CovariateDefinitions(), PatientVariates(),
                                                         DATE_TIME_NO_VAR(2017, 8, 19, 14, 32, 0),
                                                         DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0)),
                                      std::runtime_error);
        }

        // Build a covariate extractor from a set of covariate definitions.
        {
            CovariateDefinitions cDefinitions;

            ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
            ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), kg, cDefinitions);
            ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);

            fructose_assert_no_exception(CovariateExtractor(cDefinitions, PatientVariates(),
                                                            DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                                                            DATE_TIME_NO_VAR(2017, 8, 19, 14, 32, 0)));
        }

        // Build a covariate extractor from a set of covariate definitions, where non-standard definitions are present.
        {
            CovariateDefinitions cDefinitions;
            PatientVariates pVariates;

            ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
            ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), kg, cDefinitions);
            ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);
            ADD_CDEF_NO_R(AgeDays, 20, AgeInDays, Double, Direct, cDefinitions);
            ADD_CDEF_NO_R(AgeMonths, 3, AgeInMonths, Double, Direct, cDefinitions);
            ADD_CDEF_NO_R(AgeYears, 15, AgeInYears, Double, Direct, cDefinitions);

            fructose_assert_no_exception(CovariateExtractor(cDefinitions, PatientVariates(),
                                                            DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                                                            DATE_TIME_NO_VAR(2017, 8, 19, 14, 32, 0)));

            CovariateExtractor extractor(cDefinitions, pVariates,
                                         DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                                         DATE_TIME_NO_VAR(2017, 8, 29, 14, 32, 0));
            fructose_assert(extractor.m_hasBirthDate == false);
            fructose_assert(extractor.m_pvValued.count("AgeDays") == 0);
            fructose_assert(extractor.m_pvValued.count("AgeMonths") == 0);
            fructose_assert(extractor.m_pvValued.count("AgeYears") == 0);
        }

        // Check that the definition of the age (expressed in days) is consistent (1).
        {
            CovariateDefinitions cDefinitions;
            PatientVariates pVariates;

            ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
            ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), kg, cDefinitions);
            ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);
            ADD_CDEF_NO_R(AgeDays, 20, AgeInDays, Double, Direct, cDefinitions);

            fructose_assert_no_exception(CovariateExtractor(cDefinitions, PatientVariates(),
                                                            DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                                                            DATE_TIME_NO_VAR(2017, 8, 19, 14, 32, 0)));

            CovariateExtractor extractor(cDefinitions, pVariates,
                                         DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                                         DATE_TIME_NO_VAR(2017, 8, 29, 14, 32, 0));
            fructose_assert(extractor.m_hasBirthDate == false);
            fructose_assert(extractor.m_pvValued.count("AgeDays") == 0);
            fructose_assert(extractor.m_pvValued.count("AgeMonths") == 0);
            fructose_assert(extractor.m_pvValued.count("AgeYears") == 0);
            fructose_assert(extractor.m_cdValued.count("AgeDays") == 1);
            fructose_assert(extractor.m_cdValued.count("AgeMonths") == 0);
            fructose_assert(extractor.m_cdValued.count("AgeYears") == 0);
            fructose_assert(extractor.m_initAgeInDays == 20);
            fructose_assert(extractor.m_initAgeInMonths == -1);
            fructose_assert(extractor.m_initAgeInYears == -1);
        }

        // Check that the definition of the age (expressed in days) is consistent (2).
        {
            CovariateDefinitions cDefinitions;
            PatientVariates pVariates;

            ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
            ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), kg, cDefinitions);
            ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);
            ADD_CDEF_NO_R(AgeDays, -20, AgeInDays, Double, Direct, cDefinitions);

            fructose_assert_exception(CovariateExtractor(cDefinitions, PatientVariates(),
                                                         DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                                                         DATE_TIME_NO_VAR(2017, 8, 19, 14, 32, 0)),
                                      std::runtime_error);
        }

        // Check that the definition of the age (expressed in days) is consistent (3).
        {
            CovariateDefinitions cDefinitions;
            PatientVariates pVariates;

            ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
            ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), kg, cDefinitions);
            ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);
            ADD_CDEF_NO_R(AgeDays, 20, AgeInDays, Double, Direct, cDefinitions);
            ADD_CDEF_NO_R(AgeDays2, 20, AgeInDays, Double, Direct, cDefinitions);

            fructose_assert_exception(CovariateExtractor(cDefinitions, PatientVariates(),
                                                         DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                                                         DATE_TIME_NO_VAR(2017, 8, 19, 14, 32, 0)),
                                      std::runtime_error);
        }

        // Check that the definition of the age (expressed in months) is consistent (1).
        {
            CovariateDefinitions cDefinitions;
            PatientVariates pVariates;

            ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
            ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), kg, cDefinitions);
            ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);
            ADD_CDEF_NO_R(AgeMonths, 20, AgeInMonths, Double, Direct, cDefinitions);

            fructose_assert_no_exception(CovariateExtractor(cDefinitions, PatientVariates(),
                                                            DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                                                            DATE_TIME_NO_VAR(2017, 8, 19, 14, 32, 0)));

            CovariateExtractor extractor(cDefinitions, pVariates,
                                         DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                                         DATE_TIME_NO_VAR(2017, 8, 29, 14, 32, 0));
            fructose_assert(extractor.m_hasBirthDate == false);
            fructose_assert(extractor.m_pvValued.count("AgeDays") == 0);
            fructose_assert(extractor.m_pvValued.count("AgeMonths") == 0);
            fructose_assert(extractor.m_pvValued.count("AgeYears") == 0);
            fructose_assert(extractor.m_cdValued.count("AgeDays") == 0);
            fructose_assert(extractor.m_cdValued.count("AgeMonths") == 1);
            fructose_assert(extractor.m_cdValued.count("AgeYears") == 0);
            fructose_assert(extractor.m_initAgeInDays == -1);
            fructose_assert(extractor.m_initAgeInMonths == 20);
            fructose_assert(extractor.m_initAgeInYears == -1);
        }

        // Check that the definition of the age (expressed in months) is consistent (2).
        {
            CovariateDefinitions cDefinitions;
            PatientVariates pVariates;

            ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
            ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), kg, cDefinitions);
            ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);
            ADD_CDEF_NO_R(AgeMonths, -20, AgeInMonths, Double, Direct, cDefinitions);

            fructose_assert_exception(CovariateExtractor(cDefinitions, PatientVariates(),
                                                         DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                                                         DATE_TIME_NO_VAR(2017, 8, 19, 14, 32, 0)),
                                      std::runtime_error);
        }

        // Check that the definition of the age (expressed in months) is consistent (3).
        {
            CovariateDefinitions cDefinitions;
            PatientVariates pVariates;

            ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
            ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), kg, cDefinitions);
            ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);
            ADD_CDEF_NO_R(AgeMonths, 20, AgeInMonths, Double, Direct, cDefinitions);
            ADD_CDEF_NO_R(AgeMonths2, 20, AgeInMonths, Double, Direct, cDefinitions);

            fructose_assert_exception(CovariateExtractor(cDefinitions, PatientVariates(),
                                                         DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                                                         DATE_TIME_NO_VAR(2017, 8, 19, 14, 32, 0)),
                                      std::runtime_error);
        }

        // Check that the definition of the age (expressed in years) is consistent (1).
        {
            CovariateDefinitions cDefinitions;
            PatientVariates pVariates;

            ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
            ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), kg, cDefinitions);
            ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);
            ADD_CDEF_NO_R(AgeYears, 20, AgeInYears, Double, Direct, cDefinitions);

            fructose_assert_no_exception(CovariateExtractor(cDefinitions, PatientVariates(),
                                                            DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                                                            DATE_TIME_NO_VAR(2017, 8, 19, 14, 32, 0)));

            CovariateExtractor extractor(cDefinitions, pVariates,
                                         DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                                         DATE_TIME_NO_VAR(2017, 8, 29, 14, 32, 0));
            fructose_assert(extractor.m_hasBirthDate == false);
            fructose_assert(extractor.m_pvValued.count("AgeDays") == 0);
            fructose_assert(extractor.m_pvValued.count("AgeMonths") == 0);
            fructose_assert(extractor.m_pvValued.count("AgeYears") == 0);
            fructose_assert(extractor.m_cdValued.count("AgeDays") == 0);
            fructose_assert(extractor.m_cdValued.count("AgeMonths") == 0);
            fructose_assert(extractor.m_cdValued.count("AgeYears") == 1);
            fructose_assert(extractor.m_initAgeInDays == -1);
            fructose_assert(extractor.m_initAgeInMonths == -1);
            fructose_assert(extractor.m_initAgeInYears == 20);
        }

        // Check that the definition of the age (expressed in years) is consistent (2).
        {
            CovariateDefinitions cDefinitions;
            PatientVariates pVariates;

            ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
            ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), kg, cDefinitions);
            ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);
            ADD_CDEF_NO_R(AgeYears, -20, AgeInYears, Double, Direct, cDefinitions);

            fructose_assert_exception(CovariateExtractor(cDefinitions, PatientVariates(),
                                                         DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                                                         DATE_TIME_NO_VAR(2017, 8, 19, 14, 32, 0)),
                                      std::runtime_error);
        }

        // Check that the definition of the age (expressed in years) is consistent (3).
        {
            CovariateDefinitions cDefinitions;
            PatientVariates pVariates;

            ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
            ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), kg, cDefinitions);
            ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);
            ADD_CDEF_NO_R(AgeYears, 20, AgeInYears, Double, Direct, cDefinitions);
            ADD_CDEF_NO_R(AgeYears2, 20, AgeInYears, Double, Direct, cDefinitions);

            fructose_assert_exception(CovariateExtractor(cDefinitions, PatientVariates(),
                                                         DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                                                         DATE_TIME_NO_VAR(2017, 8, 19, 14, 32, 0)),
                                      std::runtime_error);
        }

        // Build a covariate extractor from a set of covariate definitions and patient variates.
        {
            CovariateDefinitions cDefinitions;
            PatientVariates pVariates;

            ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
            ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), kg, cDefinitions);
            ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);
            ADD_PV_NO_UNIT(Gist, true, Bool, DATE_TIME_NO_VAR(2017, 8, 19, 12, 32, 0), pVariates);
            ADD_PV_NO_UNIT(Gist, false, Bool, DATE_TIME_NO_VAR(2017, 8, 19, 22, 32, 0), pVariates);

            fructose_assert_no_exception(CovariateExtractor(cDefinitions, pVariates,
                                                            DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                                                            DATE_TIME_NO_VAR(2017, 8, 29, 14, 32, 0)));
        }
/*
        // Add a birth date with the appropriate type.
        {
            CovariateDefinitions cDefinitions;
            PatientVariates pVariates;

            ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
            ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), kg, cDefinitions);
            ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);
            ADD_CDEF_NO_R_STR(CovariateExtractor::sm_birthDateCName, DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0), Standard,
                              Date, Direct, cDefinitions);

            ADD_PV_NO_UNIT(Gist, true, Bool, DATE_TIME_NO_VAR(2017, 8, 19, 12, 32, 0), pVariates);
            ADD_PV_NO_UNIT(Gist, false, Bool, DATE_TIME_NO_VAR(2017, 8, 19, 22, 32, 0), pVariates);

            fructose_assert_no_exception(CovariateExtractor(cDefinitions, pVariates,
                                                            DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                                                            DATE_TIME_NO_VAR(2017, 8, 29, 14, 32, 0)));

            CovariateExtractor extractor(cDefinitions, pVariates,
                                         DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                                         DATE_TIME_NO_VAR(2017, 8, 29, 14, 32, 0));
            fructose_assert(extractor.m_birthDate == DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0));
            fructose_assert(extractor.m_hasBirthDate == true);
            fructose_assert(extractor.m_pvValued.count(CovariateExtractor::sm_birthDateCName) == 0);
        }
*/
 /*
        // Check that a reasonable birth date is given.
        {
            CovariateDefinitions cDefinitions;
            PatientVariates pVariates;

            ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
            ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), kg, cDefinitions);
            ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);
            ADD_CDEF_NO_R_STR(CovariateExtractor::sm_birthDateCName, DATE_TIME_NO_VAR(2017, 8, 19, 14, 32, 0), Standard,
                              Date, Direct, cDefinitions);


            fructose_assert_exception(CovariateExtractor(cDefinitions, PatientVariates(),
                                                            DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                                                            DATE_TIME_NO_VAR(2017, 8, 29, 14, 32, 0)),
                                      std::runtime_error);
        }
*/
        // Add twice the same covariate definition (with different values).
        {
            CovariateDefinitions cDefinitions;

            ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
            ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), kg, cDefinitions);
            ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);
            ADD_CDEF_W_R_UNIT(Weight, 3500, Standard, Bool, Linear, Tucuxi::Common::days(2), mg, cDefinitions);

            fructose_assert_exception(CovariateExtractor(cDefinitions, PatientVariates(),
                                                         DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                                                         DATE_TIME_NO_VAR(2017, 8, 19, 14, 32, 0)),
                                      std::runtime_error);
        }
/*
        // Added a birth date, but not with date type.
        {
            CovariateDefinitions cDefinitions;

            ADD_CDEF_NO_R_STR(CovariateExtractor::sm_birthDateCName, false, Standard, Bool, Direct, cDefinitions);

            ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), kg, cDefinitions);
            ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);

            fructose_assert_exception(CovariateExtractor(cDefinitions, PatientVariates(),
                                                         DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                                                         DATE_TIME_NO_VAR(2017, 8, 19, 14, 32, 0)),
                                      std::runtime_error);
        }
*/

        // Try build a covariate extractor from a set of covariate definitions with a null pointer included.
        {
            CovariateDefinitions cDefinitions;

            ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
            ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), kg, cDefinitions);
            cDefinitions.push_back(nullptr);
            ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);

            fructose_assert_exception(CovariateExtractor(cDefinitions, PatientVariates(),
                                                         DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                                                         DATE_TIME_NO_VAR(2017, 8, 19, 14, 32, 0)),
                                      std::runtime_error);
        }

        // Try build a covariate extractor from a set of patient variates with a null pointer included.
        {
            CovariateDefinitions cDefinitions;
            PatientVariates pVariates;

            ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
            ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), kg, cDefinitions);
            ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);
            ADD_PV_NO_UNIT(Gist, true, Bool, DATE_TIME_NO_VAR(2017, 8, 19, 12, 32, 0), pVariates);
            pVariates.push_back(nullptr);
            ADD_PV_NO_UNIT(Gist, false, Bool, DATE_TIME_NO_VAR(2017, 8, 19, 22, 32, 0), pVariates);

            fructose_assert_exception(CovariateExtractor(cDefinitions, pVariates,
                                                         DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                                                         DATE_TIME_NO_VAR(2017, 8, 29, 14, 32, 0)),
                                      std::runtime_error);
        }

        // Sanity check on TimeDate/Duration/Utils functions that are used by the class.
        {
            DATE_TIME_VAR(t1, 2017, 8, 17, 14, 0, 0);
            DATE_TIME_VAR(t2, 2017, 8, 7, 4, 0, 0);
            DATE_TIME_VAR(t3, 2016, 7, 7, 4, 0, 0);
            DATE_TIME_VAR(t4, 1901, 12, 3, 4, 0, 0);
            DATE_TIME_VAR(t5, 2017, 7, 27, 4, 0, 0);

            Value v1 = stringToValue(varToString(t1), DataType::Date);
            fructose_assert(v1 == varToValue(t1));
            Value v2 = stringToValue(varToString(t2), DataType::Date);
            fructose_assert(v2 == varToValue(t2));

            fructose_assert(dateDiffInDays(t1, t2) == 10);
            fructose_assert(dateDiffInMonths(t1, t2) == 0);
            fructose_assert(dateDiffInYears(t1, t2) == 0);

            fructose_assert(dateDiffInDays(t1, t3) == 406);
            fructose_assert(dateDiffInMonths(t1, t3) == 13);
            fructose_assert(dateDiffInYears(t1, t3) == 1);

            fructose_assert(dateDiffInDays(t1, t4) == 42261);
            fructose_assert(dateDiffInMonths(t1, t4) == 1388);
            fructose_assert(dateDiffInYears(t1, t4) == 115);

            fructose_assert(dateDiffInDays(t1, t5) == 21);
            fructose_assert(dateDiffInMonths(t1, t5) == 0);
            fructose_assert(dateDiffInYears(t1, t5) == 0);
        }
    }


    /// \brief Test the collectRefreshIntervals helper function.
    /// We expect the function to gather the refresh intervals
    void testCE_collectRefreshIntervals(const std::string& /* _testName */)
    {
        // Sanity check on TimeDate/Duration functions that are used by collectRefreshIntervals().
        {
            DATE_TIME_VAR(t1, 2017, 8, 17, 14, 0, 0);

            DateTime tDayAfter = t1;
            tDayAfter.addDays(1);
            fructose_assert(tDayAfter == DATE_TIME_NO_VAR(2017, 8, 18, 14, 0, 0));

            DateTime t10DaysAfter = t1;
            t10DaysAfter.addDays(10);
            fructose_assert(t10DaysAfter == DATE_TIME_NO_VAR(2017, 8, 27, 14, 0, 0));

            DateTime t20DaysAfter = t1;
            t20DaysAfter.addDays(20);
            fructose_assert(t20DaysAfter == DATE_TIME_NO_VAR(2017, 9, 6, 14, 0, 0));

            DateTime tMonthAfter = t1;
            tMonthAfter.addMonths(1);
            fructose_assert(tMonthAfter == DATE_TIME_NO_VAR(2017, 9, 17, 14, 0, 0));

            DateTime t14MonthsAfter = t1;
            t14MonthsAfter.addMonths(14);
            fructose_assert(t14MonthsAfter == DATE_TIME_NO_VAR(2018, 10, 17, 14, 0, 0));

            DateTime tYearAfter = t1;
            tYearAfter.addYears(1);
            fructose_assert(tYearAfter == DATE_TIME_NO_VAR(2018, 8, 17, 14, 0, 0));

            DateTime t14YearsAfter = t1;
            t14YearsAfter.addYears(14);
            fructose_assert(t14YearsAfter == DATE_TIME_NO_VAR(2031, 8, 17, 14, 0, 0));

            // Now check conversions.
            fructose_assert(varToString(DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0)) == "2017-08-18 14:32:00");
            fructose_assert(ValueToDate(stringToValue(varToString(DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0)), DataType::Date)) == DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0));
        }

        // Check that refresh times for covariates with no patient variates are respected.
        {
            CovariateDefinitions cDefinitions;
            PatientVariates pVariates;

            ADD_CDEF_W_R(A1, 1.0, Standard, Double, Linear, Tucuxi::Common::days(1), cDefinitions);
            ADD_CDEF_W_R(B2, 2.0, Standard, Double, Linear, Tucuxi::Common::days(2), cDefinitions);
            ADD_CDEF_W_R(C3, 3.0, Standard, Double, Linear, Tucuxi::Common::days(3), cDefinitions);

            CovariateExtractor extractor(cDefinitions, pVariates,
                                         DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0),
                                         DATE_TIME_NO_VAR(2017, 8, 23, 14, 0, 0));
            extractor.sortPatientVariates();

            std::map<DateTime, std::vector<std::string>> refreshMap;
            extractor.collectRefreshIntervals(refreshMap);

            fructose_assert(refreshMap.size() == 7);
            // At each date we are supposed to have the three of them, since an update of one will have to trigger the
            // update of the others.
            for (unsigned int i = 17; i <= 23; ++i) {
                fructose_assert(refreshMap[DATE_TIME_NO_VAR(2017, 8, i, 14, 0, 0)].size() == 3);
                CHECK_REFRESH(A1, DATE_TIME_NO_VAR(2017, 8, i, 14, 0, 0), refreshMap);
                CHECK_REFRESH(B2, DATE_TIME_NO_VAR(2017, 8, i, 14, 0, 0), refreshMap);
                CHECK_REFRESH(C3, DATE_TIME_NO_VAR(2017, 8, i, 14, 0, 0), refreshMap);
            }
        }

        // Check that refresh times for covariates with no patient variates are respected.
        // Computed covariates are not included -- their value change as a function of other covariates.
        {
            CovariateDefinitions cDefinitions;
            PatientVariates pVariates;

            ADD_CDEF_W_R(A1, 1.0, Standard, Double, Linear, Tucuxi::Common::days(1), cDefinitions);
            ADD_CDEF_W_R(B2, 2.0, Standard, Double, Linear, Tucuxi::Common::days(2), cDefinitions);
            ADD_CDEF_W_R(C3, 3.0, Standard, Double, Linear, Tucuxi::Common::days(3), cDefinitions);

            ADD_EXPR3_CDEF(op1, "A1 + B2 + C3", "A1", "B2", "C3", cDefinitions);
            ADD_EXPR3_CDEF(op2, "A1 + B2 * C3", "A1", "B2", "C3", cDefinitions);

            CovariateExtractor extractor(cDefinitions, pVariates,
                                         DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0),
                                         DATE_TIME_NO_VAR(2017, 8, 23, 14, 0, 0));
            extractor.sortPatientVariates();

            std::map<DateTime, std::vector<std::string>> refreshMap;
            extractor.collectRefreshIntervals(refreshMap);

            fructose_assert(refreshMap.size() == 7);
            // At each date we are supposed to have the three of them, since an update of one will have to trigger the
            // update of the others.
            for (unsigned int i = 17; i <= 23; ++i) {
                fructose_assert(refreshMap[DATE_TIME_NO_VAR(2017, 8, i, 14, 0, 0)].size() == 3);
                CHECK_REFRESH(A1, DATE_TIME_NO_VAR(2017, 8, i, 14, 0, 0), refreshMap);
                CHECK_REFRESH(B2, DATE_TIME_NO_VAR(2017, 8, i, 14, 0, 0), refreshMap);
                CHECK_REFRESH(C3, DATE_TIME_NO_VAR(2017, 8, i, 14, 0, 0), refreshMap);
            }
        }

        // Check that refresh times for covariates with patient variates are respected.
        // The presence of PVs should not influence the refresh time (it just gives values for the interpolation).
        {
            CovariateDefinitions cDefinitions;
            PatientVariates pVariates;

            ADD_CDEF_W_R(A1, 1.0, Standard, Double, Linear, Tucuxi::Common::days(1), cDefinitions);
            ADD_CDEF_W_R(B2, 2.0, Standard, Double, Linear, Tucuxi::Common::days(2), cDefinitions);
            ADD_CDEF_W_R(C3, 3.0, Standard, Double, Linear, Tucuxi::Common::days(3), cDefinitions);

            ADD_PV_NO_UNIT(A1, 1.1, Double, DATE_TIME_NO_VAR(2017, 8, 19, 12, 32, 0), pVariates);
            ADD_PV_NO_UNIT(A1, 1.2, Double, DATE_TIME_NO_VAR(2017, 8, 21, 12, 32, 0), pVariates);
            ADD_PV_NO_UNIT(B2, 2.1, Double, DATE_TIME_NO_VAR(2017, 8, 21, 12, 32, 0), pVariates);

            CovariateExtractor extractor(cDefinitions, pVariates,
                                         DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0),
                                         DATE_TIME_NO_VAR(2017, 8, 23, 14, 0, 0));
            extractor.sortPatientVariates();

            std::map<DateTime, std::vector<std::string>> refreshMap;
            extractor.collectRefreshIntervals(refreshMap);

            fructose_assert(refreshMap.size() == 7);
            // At each date we are supposed to have the three of them, since an update of one will have to trigger the
            // update of the others.
            for (unsigned int i = 17; i <= 23; ++i) {
                fructose_assert(refreshMap[DATE_TIME_NO_VAR(2017, 8, i, 14, 0, 0)].size() == 3);
                CHECK_REFRESH(A1, DATE_TIME_NO_VAR(2017, 8, i, 14, 0, 0), refreshMap);
                CHECK_REFRESH(B2, DATE_TIME_NO_VAR(2017, 8, i, 14, 0, 0), refreshMap);
                CHECK_REFRESH(C3, DATE_TIME_NO_VAR(2017, 8, i, 14, 0, 0), refreshMap);
            }
        }

        // Check that refresh times for covariates with patient variates are respected.
        // The presence of PVs should not influence the refresh time (it just gives values for the interpolation).
        // However, if covariates do not have refresh times, then their value gets updated in the PV time instant and
        // therefore this time instant has to be included in the refresh intervals.
        {
            CovariateDefinitions cDefinitions;
            PatientVariates pVariates;

            ADD_CDEF_W_R(A1, 1.0, Standard, Double, Linear, Tucuxi::Common::days(1), cDefinitions);
            ADD_CDEF_W_R(B2, 2.0, Standard, Double, Linear, Tucuxi::Common::days(2), cDefinitions);
            ADD_CDEF_W_R(C3, 3.0, Standard, Double, Linear, Tucuxi::Common::days(3), cDefinitions);
            ADD_CDEF_NO_R(D4, 4.0, Standard, Double, Linear, cDefinitions);
            ADD_CDEF_NO_R(E5, 5.0, Standard, Double, Linear, cDefinitions);
            ADD_CDEF_NO_R(F6, 6.0, Standard, Double, Linear, cDefinitions);
            ADD_CDEF_NO_R(G7, 7.0, Standard, Double, Linear, cDefinitions);
            ADD_CDEF_NO_R(H8, 7.0, Standard, Double, Linear, cDefinitions);

            ADD_PV_NO_UNIT(A1, 1.1, Double, DATE_TIME_NO_VAR(2017, 8, 19, 12, 32, 0), pVariates);
            ADD_PV_NO_UNIT(A1, 1.2, Double, DATE_TIME_NO_VAR(2017, 8, 21, 12, 32, 0), pVariates);
            ADD_PV_NO_UNIT(B2, 2.1, Double, DATE_TIME_NO_VAR(2017, 8, 21, 12, 32, 0), pVariates);
            ADD_PV_NO_UNIT(D4, 4.2, Double, DATE_TIME_NO_VAR(2017, 8, 18, 14, 00, 0), pVariates);
            ADD_PV_NO_UNIT(D4, 4.2, Double, DATE_TIME_NO_VAR(2017, 8, 21, 12, 32, 0), pVariates);
            ADD_PV_NO_UNIT(D4, 4.3, Double, DATE_TIME_NO_VAR(2017, 8, 23, 12, 32, 0), pVariates);
            // This value should not be duplicated.
            ADD_PV_NO_UNIT(E5, 5.1, Double, DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0), pVariates);
            ADD_PV_NO_UNIT(E5, 5.2, Double, DATE_TIME_NO_VAR(2017, 8, 18, 14, 0, 0), pVariates);
            // F6 has no PVs -- it should take just the start value.
            // G7 has a single value -- it should take just the start value.
            ADD_PV_NO_UNIT(G7, 7.1, Double, DATE_TIME_NO_VAR(2017, 8, 18, 14, 0, 0), pVariates);
            // H8 has two values, but outside the interval -- it should take just the start value.
            ADD_PV_NO_UNIT(H8, 8.1, Double, DATE_TIME_NO_VAR(2017, 8, 16, 14, 0, 0), pVariates);
            ADD_PV_NO_UNIT(H8, 8.2, Double, DATE_TIME_NO_VAR(2017, 8, 26, 14, 0, 0), pVariates);

            CovariateExtractor extractor(cDefinitions, pVariates,
                                         DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0),
                                         DATE_TIME_NO_VAR(2017, 8, 23, 14, 0, 0));
            extractor.sortPatientVariates();

            std::map<DateTime, std::vector<std::string>> refreshMap;
            extractor.collectRefreshIntervals(refreshMap);

            fructose_assert(refreshMap.size() == 9);
            // At each date we are supposed to have the three of them, since an update of one will have to trigger the
            // update of the others.
            for (unsigned int i = 17; i <= 23; ++i) {
                fructose_assert(refreshMap[DATE_TIME_NO_VAR(2017, 8, i, 14, 0, 0)].size() == 8);
                CHECK_REFRESH(A1, DATE_TIME_NO_VAR(2017, 8, i, 14, 0, 0), refreshMap);
                CHECK_REFRESH(B2, DATE_TIME_NO_VAR(2017, 8, i, 14, 0, 0), refreshMap);
                CHECK_REFRESH(C3, DATE_TIME_NO_VAR(2017, 8, i, 14, 0, 0), refreshMap);
                CHECK_REFRESH(D4, DATE_TIME_NO_VAR(2017, 8, i, 14, 0, 0), refreshMap);
                CHECK_REFRESH(E5, DATE_TIME_NO_VAR(2017, 8, i, 14, 0, 0), refreshMap);
                CHECK_REFRESH(F6, DATE_TIME_NO_VAR(2017, 8, i, 14, 0, 0), refreshMap);
                CHECK_REFRESH(G7, DATE_TIME_NO_VAR(2017, 8, i, 14, 0, 0), refreshMap);
                CHECK_REFRESH(H8, DATE_TIME_NO_VAR(2017, 8, i, 14, 0, 0), refreshMap);
            }

            // 21.08.2017 @ 12h32
            fructose_assert(refreshMap[DATE_TIME_NO_VAR(2017, 8, 21, 12, 32, 0)].size() == 8);
            CHECK_REFRESH(A1, DATE_TIME_NO_VAR(2017, 8, 21, 12, 32, 0), refreshMap);
            CHECK_REFRESH(B2, DATE_TIME_NO_VAR(2017, 8, 21, 12, 32, 0), refreshMap);
            CHECK_REFRESH(C3, DATE_TIME_NO_VAR(2017, 8, 21, 12, 32, 0), refreshMap);
            CHECK_REFRESH(D4, DATE_TIME_NO_VAR(2017, 8, 21, 12, 32, 0), refreshMap);
            CHECK_REFRESH(E5, DATE_TIME_NO_VAR(2017, 8, 21, 12, 32, 0), refreshMap);
            CHECK_REFRESH(F6, DATE_TIME_NO_VAR(2017, 8, 21, 12, 32, 0), refreshMap);
            CHECK_REFRESH(G7, DATE_TIME_NO_VAR(2017, 8, 21, 12, 32, 0), refreshMap);
            CHECK_REFRESH(H8, DATE_TIME_NO_VAR(2017, 8, 21, 12, 32, 0), refreshMap);

            // 23.08.2017 @ 12h32
            fructose_assert(refreshMap[DATE_TIME_NO_VAR(2017, 8, 23, 12, 32, 0)].size() == 8);
            CHECK_REFRESH(A1, DATE_TIME_NO_VAR(2017, 8, 23, 12, 32, 0), refreshMap);
            CHECK_REFRESH(B2, DATE_TIME_NO_VAR(2017, 8, 23, 12, 32, 0), refreshMap);
            CHECK_REFRESH(C3, DATE_TIME_NO_VAR(2017, 8, 23, 12, 32, 0), refreshMap);
            CHECK_REFRESH(D4, DATE_TIME_NO_VAR(2017, 8, 23, 12, 32, 0), refreshMap);
            CHECK_REFRESH(E5, DATE_TIME_NO_VAR(2017, 8, 23, 12, 32, 0), refreshMap);
            CHECK_REFRESH(F6, DATE_TIME_NO_VAR(2017, 8, 23, 12, 32, 0), refreshMap);
            CHECK_REFRESH(G7, DATE_TIME_NO_VAR(2017, 8, 23, 12, 32, 0), refreshMap);
            CHECK_REFRESH(H8, DATE_TIME_NO_VAR(2017, 8, 23, 12, 32, 0), refreshMap);
        }

        // Check the refresh times for ages -- without birth date.
        {
            CovariateDefinitions cDefinitions;
            PatientVariates pVariates;

            ADD_CDEF_NO_R(AgeDays, 20, AgeInDays, Double, Direct, cDefinitions);
            ADD_CDEF_NO_R(AgeMonths, 3, AgeInMonths, Double, Direct, cDefinitions);
            ADD_CDEF_NO_R(AgeYears, 4, AgeInYears, Double, Direct, cDefinitions);

            CovariateExtractor extractor(cDefinitions, pVariates,
                                         DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0),
                                         DATE_TIME_NO_VAR(2018, 9, 23, 14, 0, 0));
            extractor.sortPatientVariates();

            std::map<DateTime, std::vector<std::string>> refreshMap;
            extractor.collectRefreshIntervals(refreshMap);

            fructose_assert(refreshMap.size() == 403);
            // 17.08.2017
            fructose_assert(refreshMap[DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0)].size() == 3);
            CHECK_REFRESH(AgeDays, DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0), refreshMap);
            CHECK_REFRESH(AgeMonths, DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0), refreshMap);
            CHECK_REFRESH(AgeYears, DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0), refreshMap);

            // 18.08-16.09.2017
            for (int i = 18; i <= 31; ++i) {
                fructose_assert(refreshMap[DATE_TIME_NO_VAR(2017, 8, i, 14, 0, 0)].size() == 1);
                CHECK_REFRESH(AgeDays, DATE_TIME_NO_VAR(2017, 8, i, 14, 0, 0), refreshMap);
            }
            for (int i = 1; i <= 16; ++i) {
                fructose_assert(refreshMap[DATE_TIME_NO_VAR(2017, 9, i, 14, 0, 0)].size() == 1);
                CHECK_REFRESH(AgeDays, DATE_TIME_NO_VAR(2017, 9, i, 14, 0, 0), refreshMap);
            }

            // 17.09.2017
            fructose_assert(refreshMap[DATE_TIME_NO_VAR(2017, 9, 17, 14, 0, 0)].size() == 2);
            CHECK_REFRESH(AgeDays, DATE_TIME_NO_VAR(2017, 9, 17, 14, 0, 0), refreshMap);
            CHECK_REFRESH(AgeMonths, DATE_TIME_NO_VAR(2017, 9, 17, 14, 0, 0), refreshMap);

            // 17.08.2018
            fructose_assert(refreshMap[DATE_TIME_NO_VAR(2018, 8, 17, 14, 0, 0)].size() == 3);
            CHECK_REFRESH(AgeDays, DATE_TIME_NO_VAR(2018, 8, 17, 14, 0, 0), refreshMap);
            CHECK_REFRESH(AgeMonths, DATE_TIME_NO_VAR(2018, 8, 17, 14, 0, 0), refreshMap);
            CHECK_REFRESH(AgeYears, DATE_TIME_NO_VAR(2018, 8, 17, 14, 0, 0), refreshMap);
        }

        // Check the refresh times for ages -- with birth date specified.
        {
            CovariateDefinitions cDefinitions;
            PatientVariates pVariates;

            ADD_CDEF_NO_R(AgeDays, 20, AgeInDays, Double, Direct, cDefinitions);
            ADD_CDEF_NO_R(AgeMonths, 3, AgeInMonths, Double, Direct, cDefinitions);
            ADD_CDEF_NO_R(AgeYears, 4, AgeInYears, Double, Direct, cDefinitions);

            //ADD_CDEF_NO_R_STR(CovariateExtractor::sm_birthDateCName, DATE_TIME_NO_VAR(1995, 12, 13, 14, 15, 0), Standard,
            //                  Date, Direct, cDefinitions);


            ADD_PV_NO_UNIT(birthdate, DATE_TIME_NO_VAR(1995, 12, 13, 14, 15, 0), Date, DATE_TIME_NO_VAR(1994, 12, 13, 14, 15, 0), pVariates);

            CovariateExtractor extractor(cDefinitions, pVariates,
                                         DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0),
                                         DATE_TIME_NO_VAR(2018, 9, 23, 14, 0, 0));
            extractor.sortPatientVariates();

            std::map<DateTime, std::vector<std::string>> refreshMap;
            extractor.collectRefreshIntervals(refreshMap);

            fructose_assert(refreshMap.size() == 403);
            // 17.08.2017 @ 14h00 (start)
            fructose_assert_eq(refreshMap[DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0)].size(), static_cast<size_t>(3));
            CHECK_REFRESH(AgeDays, DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0), refreshMap);
            CHECK_REFRESH(AgeMonths, DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0), refreshMap);
            CHECK_REFRESH(AgeYears, DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0), refreshMap);
            // 17.08.2017 @ 14h15
            fructose_assert_eq(refreshMap[DATE_TIME_NO_VAR(2017, 8, 17, 14, 15, 0)].size(), static_cast<size_t>(1));
            CHECK_REFRESH(AgeDays, DATE_TIME_NO_VAR(2017, 8, 17, 14, 15, 0), refreshMap);

            // 13.09.2017
            fructose_assert_eq(refreshMap[DATE_TIME_NO_VAR(2017, 9, 13, 14, 15, 0)].size(), static_cast<size_t>(2));
            CHECK_REFRESH(AgeDays, DATE_TIME_NO_VAR(2017, 9, 13, 14, 15, 0), refreshMap);
            CHECK_REFRESH(AgeMonths, DATE_TIME_NO_VAR(2017, 9, 13, 14, 15, 0), refreshMap);

            // 13.12.2017
            fructose_assert_eq(refreshMap.count(DATE_TIME_NO_VAR(2017, 12, 13, 14, 0, 0)), static_cast<size_t>(0));
            fructose_assert_eq(refreshMap[DATE_TIME_NO_VAR(2017, 12, 13, 14, 15, 0)].size(), static_cast<size_t>(3));
            CHECK_REFRESH(AgeDays, DATE_TIME_NO_VAR(2017, 12, 13, 14, 15, 0), refreshMap);
            CHECK_REFRESH(AgeMonths, DATE_TIME_NO_VAR(2017, 12, 13, 14, 15, 0), refreshMap);
            CHECK_REFRESH(AgeYears, DATE_TIME_NO_VAR(2017, 12, 13, 14, 15, 0), refreshMap);
        }
    }


    /// \brief Test the interpolateValues helper function.
    void testCE_interpolateValues(const std::string& /* _testName */)
    {
        CovariateExtractor extractor(CovariateDefinitions(), PatientVariates(),
                                     DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0),
                                     DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0));
        bool rc;
        Value valRes;

        // Violate precondition.
        rc = extractor.interpolateValues(0, DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0),
                                         1, DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0),
                                         DATE_TIME_NO_VAR(2017, 8, 15, 14, 32, 0),
                                         InterpolationType::Linear,
                                         valRes);
        fructose_assert(rc == false);

        // Interpolate in the middle.
        rc = extractor.interpolateValues(0, DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0),
                                         10, DATE_TIME_NO_VAR(2017, 8, 23, 14, 32, 0),
                                         DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                                         InterpolationType::Linear,
                                         valRes);
        fructose_assert(rc == true);
        fructose_assert(valRes == 5.0);

        // Extrapolate before.
        rc = extractor.interpolateValues(5, DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                                         10, DATE_TIME_NO_VAR(2017, 8, 23, 14, 32, 0),
                                         DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0),
                                         InterpolationType::Linear,
                                         valRes);
        fructose_assert(rc == true);
        fructose_assert(valRes == 0.0);

        // Extrapolate after.
        rc = extractor.interpolateValues(5, DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0),
                                         10, DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                                         DATE_TIME_NO_VAR(2017, 8, 23, 14, 32, 0),
                                         InterpolationType::Linear,
                                         valRes);
        fructose_assert(rc == true);
        fructose_assert(valRes == 15.0);

        // Test direct interpolation (before _date1).
        rc = extractor.interpolateValues(5, DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                                         10, DATE_TIME_NO_VAR(2017, 8, 19, 14, 32, 0),
                                         DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0),
                                         InterpolationType::Direct,
                                         valRes);
        fructose_assert(rc == true);
        fructose_assert(valRes == 5.0);

        // Test direct interpolation (between _date1 and _date2).
        rc = extractor.interpolateValues(5, DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                                         10, DATE_TIME_NO_VAR(2017, 8, 29, 14, 32, 0),
                                         DATE_TIME_NO_VAR(2017, 8, 19, 14, 32, 0),
                                         InterpolationType::Direct,
                                         valRes);
        fructose_assert(rc == true);
        fructose_assert(valRes == 5.0);

        // Test direct interpolation (after _date2).
        rc = extractor.interpolateValues(5, DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                                         10, DATE_TIME_NO_VAR(2017, 8, 19, 14, 32, 0),
                                         DATE_TIME_NO_VAR(2017, 8, 23, 14, 32, 0),
                                         InterpolationType::Direct,
                                         valRes);
        fructose_assert(rc == true);
        fructose_assert(valRes == 10.0);
    }


    /// \brief Check that patient variates are properly sorted and cleaned up.
    void testCE_sortPatientVariates(const std::string& /* _testName */)
    {
        // The weight measurement the 11.08 and the one at 16h30 on the 30.08 should disappear. The remaining ones
        // should be sorted.
        {
            CovariateDefinitions cDefinitions;
            PatientVariates pVariates;

            ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
            ADD_CDEF_W_R_UNIT(Weight, 6.5, Standard, Double, Linear, Tucuxi::Common::days(1), kg, cDefinitions);
            ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);
            ADD_PV_NO_UNIT(Weight, 6.3, Double, DATE_TIME_NO_VAR(2017, 8, 19, 22, 32, 0), pVariates);
            ADD_PV_NO_UNIT(Weight, 7.2, Double, DATE_TIME_NO_VAR(2017, 8, 19, 12, 32, 0), pVariates);
            ADD_PV_NO_UNIT(Weight, 7.4, Double, DATE_TIME_NO_VAR(2017, 8, 21, 12, 32, 0), pVariates);
            ADD_PV_NO_UNIT(Weight, 7.6, Double, DATE_TIME_NO_VAR(2017, 8, 12, 12, 32, 0), pVariates);
            ADD_PV_NO_UNIT(Weight, 7.9, Double, DATE_TIME_NO_VAR(2017, 8, 11, 12, 32, 0), pVariates);
            ADD_PV_NO_UNIT(Weight, 7.0, Double, DATE_TIME_NO_VAR(2017, 8, 30, 12, 32, 0), pVariates);
            ADD_PV_NO_UNIT(Weight, 7.8, Double, DATE_TIME_NO_VAR(2017, 8, 30, 16, 32, 0), pVariates);

            CovariateExtractor extractor(cDefinitions, pVariates,
                                         DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0),
                                         DATE_TIME_NO_VAR(2017, 8, 29, 14, 0, 0));
            extractor.sortPatientVariates();

            std::vector<std::unique_ptr<PatientCovariate>> res_pvVec;
            res_pvVec.push_back(std::unique_ptr<PatientCovariate>(new PatientCovariate("Weight", varToString(7.6), DataType::Double, Unit(), DATE_TIME_NO_VAR(2017, 8, 12, 12, 32, 0))));
            res_pvVec.push_back(std::unique_ptr<PatientCovariate>(new PatientCovariate("Weight", varToString(7.2), DataType::Double, Unit(), DATE_TIME_NO_VAR(2017, 8, 19, 12, 32, 0))));
            res_pvVec.push_back(std::unique_ptr<PatientCovariate>(new PatientCovariate("Weight", varToString(6.3), DataType::Double, Unit(), DATE_TIME_NO_VAR(2017, 8, 19, 22, 32, 0))));
            res_pvVec.push_back(std::unique_ptr<PatientCovariate>(new PatientCovariate("Weight", varToString(7.4), DataType::Double, Unit(), DATE_TIME_NO_VAR(2017, 8, 21, 12, 32, 0))));
            res_pvVec.push_back(std::unique_ptr<PatientCovariate>(new PatientCovariate("Weight", varToString(7.0), DataType::Double, Unit(), DATE_TIME_NO_VAR(2017, 8, 30, 12, 32, 0))));

            const auto &pvVals = extractor.m_pvValued.at("Weight");

            fructose_assert(pvVals.size() == res_pvVec.size());
            for (size_t i = 0; i < std::min(pvVals.size(), res_pvVec.size()); ++i) {
                fructose_assert(**(pvVals.at(i)) == *(res_pvVec.at(i)));
            }
        }

        // No patient variates -- the sort just has to be nice and smile.
        {
            CovariateDefinitions cDefinitions;
            PatientVariates pVariates;

            ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
            ADD_CDEF_W_R_UNIT(Weight, 6.5, Standard, Double, Linear, Tucuxi::Common::days(1), kg, cDefinitions);
            ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);

            CovariateExtractor extractor(cDefinitions, pVariates,
                                         DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0),
                                         DATE_TIME_NO_VAR(2017, 8, 29, 14, 0, 0));
            fructose_assert_no_exception(extractor.sortPatientVariates());
        }

        // We should keep all the measurements, properly sorted.
        {
            CovariateDefinitions cDefinitions;
            PatientVariates pVariates;

            ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
            ADD_CDEF_W_R_UNIT(Weight, 6.5, Standard, Double, Linear, Tucuxi::Common::days(1), kg, cDefinitions);
            ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);
            ADD_PV_NO_UNIT(Weight, 6.3, Double, DATE_TIME_NO_VAR(2017, 8, 19, 22, 32, 0), pVariates);
            ADD_PV_NO_UNIT(Weight, 7.2, Double, DATE_TIME_NO_VAR(2017, 8, 19, 12, 32, 0), pVariates);
            ADD_PV_NO_UNIT(Weight, 7.4, Double, DATE_TIME_NO_VAR(2017, 8, 21, 12, 32, 0), pVariates);
            ADD_PV_NO_UNIT(Weight, 7.6, Double, DATE_TIME_NO_VAR(2017, 8, 12, 12, 32, 0), pVariates);
            ADD_PV_NO_UNIT(Weight, 7.9, Double, DATE_TIME_NO_VAR(2017, 8, 11, 12, 32, 0), pVariates);
            ADD_PV_NO_UNIT(Weight, 7.0, Double, DATE_TIME_NO_VAR(2017, 8, 30, 12, 32, 0), pVariates);
            ADD_PV_NO_UNIT(Weight, 7.8, Double, DATE_TIME_NO_VAR(2017, 8, 30, 16, 32, 0), pVariates);

            CovariateExtractor extractor(cDefinitions, pVariates,
                                         DATE_TIME_NO_VAR(2017, 8, 12, 12, 0, 0),
                                         DATE_TIME_NO_VAR(2017, 8, 30, 14, 0, 0));
            extractor.sortPatientVariates();

            std::vector<std::unique_ptr<PatientCovariate>> res_pvVec;
            res_pvVec.push_back(std::unique_ptr<PatientCovariate>(new PatientCovariate("Weight", varToString(7.9), DataType::Double, Unit(), DATE_TIME_NO_VAR(2017, 8, 11, 12, 32, 0))));
            res_pvVec.push_back(std::unique_ptr<PatientCovariate>(new PatientCovariate("Weight", varToString(7.6), DataType::Double, Unit(), DATE_TIME_NO_VAR(2017, 8, 12, 12, 32, 0))));
            res_pvVec.push_back(std::unique_ptr<PatientCovariate>(new PatientCovariate("Weight", varToString(7.2), DataType::Double, Unit(), DATE_TIME_NO_VAR(2017, 8, 19, 12, 32, 0))));
            res_pvVec.push_back(std::unique_ptr<PatientCovariate>(new PatientCovariate("Weight", varToString(6.3), DataType::Double, Unit(), DATE_TIME_NO_VAR(2017, 8, 19, 22, 32, 0))));
            res_pvVec.push_back(std::unique_ptr<PatientCovariate>(new PatientCovariate("Weight", varToString(7.4), DataType::Double, Unit(), DATE_TIME_NO_VAR(2017, 8, 21, 12, 32, 0))));
            res_pvVec.push_back(std::unique_ptr<PatientCovariate>(new PatientCovariate("Weight", varToString(7.0), DataType::Double, Unit(), DATE_TIME_NO_VAR(2017, 8, 30, 12, 32, 0))));
            res_pvVec.push_back(std::unique_ptr<PatientCovariate>(new PatientCovariate("Weight", varToString(7.8), DataType::Double, Unit(), DATE_TIME_NO_VAR(2017, 8, 30, 16, 32, 0))));

            const auto &pvVals = extractor.m_pvValued.at("Weight");

            fructose_assert(pvVals.size() == res_pvVec.size());
            for (size_t i = 0; i < std::min(pvVals.size(), res_pvVec.size()); ++i) {
                fructose_assert(**(pvVals.at(i)) == *(res_pvVec.at(i)));
            }
        }

        // We should keep all the measurements, properly sorted.
        {
            CovariateDefinitions cDefinitions;
            PatientVariates pVariates;

            ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
            ADD_CDEF_W_R_UNIT(Weight, 6.5, Standard, Double, Linear, Tucuxi::Common::days(1), kg, cDefinitions);
            ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);
            ADD_PV_NO_UNIT(Weight, 6.3, Double, DATE_TIME_NO_VAR(2017, 8, 19, 22, 32, 0), pVariates);
            ADD_PV_NO_UNIT(Weight, 7.2, Double, DATE_TIME_NO_VAR(2017, 8, 19, 12, 32, 0), pVariates);
            ADD_PV_NO_UNIT(Weight, 7.4, Double, DATE_TIME_NO_VAR(2017, 8, 21, 12, 32, 0), pVariates);
            ADD_PV_NO_UNIT(Weight, 7.6, Double, DATE_TIME_NO_VAR(2017, 8, 12, 12, 32, 0), pVariates);
            ADD_PV_NO_UNIT(Weight, 7.9, Double, DATE_TIME_NO_VAR(2017, 8, 11, 12, 32, 0), pVariates);
            ADD_PV_NO_UNIT(Weight, 7.0, Double, DATE_TIME_NO_VAR(2017, 8, 30, 12, 32, 0), pVariates);
            ADD_PV_NO_UNIT(Weight, 7.8, Double, DATE_TIME_NO_VAR(2017, 8, 30, 16, 32, 0), pVariates);

            CovariateExtractor extractor(cDefinitions, pVariates,
                                         DATE_TIME_NO_VAR(2017, 8, 10, 12, 0, 0),
                                         DATE_TIME_NO_VAR(2017, 8, 31, 14, 0, 0));
            extractor.sortPatientVariates();

            std::vector<std::unique_ptr<PatientCovariate>> res_pvVec;
            res_pvVec.push_back(std::unique_ptr<PatientCovariate>(new PatientCovariate("Weight", varToString(7.9), DataType::Double, Unit(), DATE_TIME_NO_VAR(2017, 8, 11, 12, 32, 0))));
            res_pvVec.push_back(std::unique_ptr<PatientCovariate>(new PatientCovariate("Weight", varToString(7.6), DataType::Double, Unit(), DATE_TIME_NO_VAR(2017, 8, 12, 12, 32, 0))));
            res_pvVec.push_back(std::unique_ptr<PatientCovariate>(new PatientCovariate("Weight", varToString(7.2), DataType::Double, Unit(), DATE_TIME_NO_VAR(2017, 8, 19, 12, 32, 0))));
            res_pvVec.push_back(std::unique_ptr<PatientCovariate>(new PatientCovariate("Weight", varToString(6.3), DataType::Double, Unit(), DATE_TIME_NO_VAR(2017, 8, 19, 22, 32, 0))));
            res_pvVec.push_back(std::unique_ptr<PatientCovariate>(new PatientCovariate("Weight", varToString(7.4), DataType::Double, Unit(), DATE_TIME_NO_VAR(2017, 8, 21, 12, 32, 0))));
            res_pvVec.push_back(std::unique_ptr<PatientCovariate>(new PatientCovariate("Weight", varToString(7.0), DataType::Double, Unit(), DATE_TIME_NO_VAR(2017, 8, 30, 12, 32, 0))));
            res_pvVec.push_back(std::unique_ptr<PatientCovariate>(new PatientCovariate("Weight", varToString(7.8), DataType::Double, Unit(), DATE_TIME_NO_VAR(2017, 8, 30, 16, 32, 0))));

            const auto &pvVals = extractor.m_pvValued.at("Weight");

            fructose_assert(pvVals.size() == res_pvVec.size());
            for (size_t i = 0; i < std::min(pvVals.size(), res_pvVec.size()); ++i) {
                fructose_assert(**(pvVals.at(i)) == *(res_pvVec.at(i)));
            }
        }

        // One measure before m_start and one after m_end. We should keep both to interpolate.
        {
            CovariateDefinitions cDefinitions;
            PatientVariates pVariates;

            ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
            ADD_CDEF_W_R_UNIT(Weight, 6.5, Standard, Double, Linear, Tucuxi::Common::days(1), kg, cDefinitions);
            ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);
            ADD_PV_NO_UNIT(Weight, 6.3, Double, DATE_TIME_NO_VAR(2017, 8, 9, 22, 32, 0), pVariates);
            ADD_PV_NO_UNIT(Weight, 7.2, Double, DATE_TIME_NO_VAR(2017, 8, 12, 12, 32, 0), pVariates);

            CovariateExtractor extractor(cDefinitions, pVariates,
                                         DATE_TIME_NO_VAR(2017, 8, 10, 12, 0, 0),
                                         DATE_TIME_NO_VAR(2017, 8, 11, 14, 0, 0));
            extractor.sortPatientVariates();

            std::vector<std::unique_ptr<PatientCovariate>> res_pvVec;
            res_pvVec.push_back(std::unique_ptr<PatientCovariate>(new PatientCovariate("Weight", varToString(6.3), DataType::Double, Unit(), DATE_TIME_NO_VAR(2017, 8, 9, 22, 32, 0))));
            res_pvVec.push_back(std::unique_ptr<PatientCovariate>(new PatientCovariate("Weight", varToString(7.2), DataType::Double, Unit(), DATE_TIME_NO_VAR(2017, 8, 12, 12, 32, 0))));

            const auto &pvVals = extractor.m_pvValued.at("Weight");

            fructose_assert(pvVals.size() == res_pvVec.size());
            for (size_t i = 0; i < std::min(pvVals.size(), res_pvVec.size()); ++i) {
                fructose_assert(**(pvVals.at(i)) == *(res_pvVec.at(i)));
            }
        }

        // We should drop all measurements except those of the 19.08.
        {
            CovariateDefinitions cDefinitions;
            PatientVariates pVariates;

            ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
            ADD_CDEF_W_R_UNIT(Weight, 6.5, Standard, Double, Linear, Tucuxi::Common::days(1), kg, cDefinitions);
            ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);
            ADD_PV_NO_UNIT(Weight, 6.3, Double, DATE_TIME_NO_VAR(2017, 8, 19, 22, 32, 0), pVariates);
            ADD_PV_NO_UNIT(Weight, 7.2, Double, DATE_TIME_NO_VAR(2017, 8, 19, 12, 32, 0), pVariates);
            ADD_PV_NO_UNIT(Weight, 7.4, Double, DATE_TIME_NO_VAR(2017, 8, 21, 12, 32, 0), pVariates);
            ADD_PV_NO_UNIT(Weight, 7.6, Double, DATE_TIME_NO_VAR(2017, 8, 12, 12, 32, 0), pVariates);
            ADD_PV_NO_UNIT(Weight, 7.9, Double, DATE_TIME_NO_VAR(2017, 8, 11, 12, 32, 0), pVariates);
            ADD_PV_NO_UNIT(Weight, 7.0, Double, DATE_TIME_NO_VAR(2017, 8, 30, 12, 32, 0), pVariates);
            ADD_PV_NO_UNIT(Weight, 7.8, Double, DATE_TIME_NO_VAR(2017, 8, 30, 16, 32, 0), pVariates);

            CovariateExtractor extractor(cDefinitions, pVariates,
                                         DATE_TIME_NO_VAR(2017, 8, 19, 13, 0, 0),
                                         DATE_TIME_NO_VAR(2017, 8, 19, 17, 0, 0));
            extractor.sortPatientVariates();

            std::vector<std::unique_ptr<PatientCovariate>> res_pvVec;
            res_pvVec.push_back(std::unique_ptr<PatientCovariate>(new PatientCovariate("Weight", varToString(7.2), DataType::Double, Unit(), DATE_TIME_NO_VAR(2017, 8, 19, 12, 32, 0))));
            res_pvVec.push_back(std::unique_ptr<PatientCovariate>(new PatientCovariate("Weight", varToString(6.3), DataType::Double, Unit(), DATE_TIME_NO_VAR(2017, 8, 19, 22, 32, 0))));

            const auto &pvVals = extractor.m_pvValued.at("Weight");

            fructose_assert(pvVals.size() == res_pvVec.size());
            for (size_t i = 0; i < std::min(pvVals.size(), res_pvVec.size()); ++i) {
                fructose_assert(**(pvVals.at(i)) == *(res_pvVec.at(i)));
            }
        }
    }


    /// \brief Test covariate extraction using the test 1_0 presented in the specs.
    void testCovariateExtraction_test1_0(const std::string& /* _testName */)
    {
        CovariateDefinitions cDefinitions;

        ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), kg, cDefinitions);
        ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);

        ADD_EXPR2_CDEF(Special, "Weight * 0.5 + IsMale * 15", "Weight", "IsMale", cDefinitions);

        const DATE_TIME_VAR(startDate, 2017, 8, 12, 8, 0, 0);
        const DATE_TIME_VAR(endDate,   2017, 8, 17, 8, 0, 0);

        // -- Patient covariates #1 --
        PatientVariates pVariates;
        // gist == true @ 13.08.2017, 12h32.
        ADD_PV_NO_UNIT(Gist, true, Bool, DATE_TIME_NO_VAR(2017, 8, 13, 12, 32, 0), pVariates);
        // gist == false @ 13.08.2017, 14h32.
        ADD_PV_NO_UNIT(Gist, false, Bool, DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0), pVariates);

        // Test 1: call the extractor with the two patient covariates above.
        // We expect the first one to be "back-propagated" to the beginning of the interval.

        CovariateExtractor extractor(cDefinitions, pVariates, startDate, endDate);
        CovariateSeries series;
        CovariateExtractor::Result rc;
        rc = extractor.extract(series);

        //        printCovariateSeries(series);

        fructose_assert(series.size() == 5);
        fructose_assert(rc == CovariateExtractor::Result::Ok);
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

        PatientVariates pVariates2;
        CovariateExtractor extractor2(cDefinitions, pVariates2, startDate, endDate);
        series.clear();
        rc = extractor2.extract(series);

        //        printCovariateSeries(series);

        fructose_assert(series.size() == 4);
        fructose_assert(rc == CovariateExtractor::Result::Ok);
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

        // gist == true @ 13.08.2017, 12h32.
        std::unique_ptr<PatientCovariate> patient_gist_3(new PatientCovariate("Gist", varToString(true),
                                                                              DataType::Bool, Unit("kg"),
                                                                              DATE_TIME_NO_VAR(2017, 8, 13, 12, 32, 0)));

        // weight = 3.8 @ 13.08.2017, 9h00.
        std::unique_ptr<PatientCovariate> patient_weight_1(new PatientCovariate("Weight", varToString(3.8),
                                                                                DataType::Double, Unit("kg"),
                                                                                DATE_TIME_NO_VAR(2017, 8, 13, 9, 0, 0)));
        // weight = 4.05 @ 15.08.2017, 21h00.
        std::unique_ptr<PatientCovariate> patient_weight_2(new PatientCovariate("Weight", varToString(4.05),
                                                                                DataType::Double, Unit("kg"),
                                                                                DATE_TIME_NO_VAR(2017, 8, 15, 21, 0, 0)));
        // weight = 4.25 @ 16.08.2017, 21h00.
        std::unique_ptr<PatientCovariate> patient_weight_3(new PatientCovariate("Weight", varToString(4.25),
                                                                                DataType::Double, Unit("kg"),
                                                                                DATE_TIME_NO_VAR(2017, 8, 16, 21, 0, 0)));
        PatientVariates pVariates3;
        pVariates3.push_back(std::move(patient_gist_3));
        pVariates3.push_back(std::move(patient_weight_1));
        pVariates3.push_back(std::move(patient_weight_2));
        pVariates3.push_back(std::move(patient_weight_3));

        CovariateExtractor extractor3(cDefinitions, pVariates3, startDate, endDate);
        series.clear();
        rc = extractor3.extract(series);

        //        printCovariateSeries(series);

        fructose_assert(series.size() == 12);
        fructose_assert(rc == CovariateExtractor::Result::Ok);

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


    /// \brief Perform the test 2_0 in the spreadsheet.
    /// - Different end time (changes wrt weight periodicity).
    /// - Added a birthday variable.
    /// - Added another computed covariate (VerySpecial)
    void testCovariateExtraction_test2_0(const std::string& /* _testName */)
    {
        CovariateDefinitions cDefinitions;
        ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), kg, cDefinitions);
        ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);
       // ADD_CDEF_NO_R_STR(CovariateExtractor::sm_birthDateCName, DATE_TIME_NO_VAR(2017, 8, 5, 8, 0, 0), Standard,
         //                 Date, Direct, cDefinitions);

        ADD_EXPR2_CDEF(Special, "Weight * 0.5 + IsMale * 15", "Weight", "IsMale", cDefinitions);
        ADD_EXPR3_CDEF(VerySpecial, "Weight * 0.5 + IsMale * 15 + Gist * 25", "Weight", "IsMale", "Gist", cDefinitions);

        // gist == true @ 13.08.2017, 12h32.
        PatientVariates pVariates;
        ADD_PV_NO_UNIT(Gist, true, Bool, DATE_TIME_NO_VAR(2017, 8, 13, 12, 32, 0), pVariates);
        // gist == false @ 13.08.2017, 14h32.
        ADD_PV_NO_UNIT(Gist, false, Bool, DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0), pVariates);

        ADD_PV_NO_UNIT(birthdate, DATE_TIME_NO_VAR(2017, 8, 5, 8, 0, 0), Date,
                       DATE_TIME_NO_VAR(2017, 8, 5, 8, 0, 0), pVariates);

        const DATE_TIME_VAR(startDate, 2017, 8, 12, 8, 0, 0);
        const DATE_TIME_VAR(endDate,   2017, 8, 17, 21, 0, 0);

        CovariateExtractor extractor(cDefinitions, pVariates, startDate, endDate);
        CovariateSeries series;
        CovariateExtractor::Result rc;
        rc = extractor.extract(series);

//        printCovariateSeries(series);

        fructose_assert_eq(series.size() , static_cast<size_t>(7));
        fructose_assert(rc == CovariateExtractor::Result::Ok);
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
        // This should be computed from IsMale, Weight, and Gist.
        fructose_assert(covariateEventIsPresent("VerySpecial",
                                                DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0),
                                                41.75,
                                                series));
        fructose_assert(covariateEventIsPresent("VerySpecial",
                                                DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0),
                                                16.75,
                                                series));

        fructose_assert(extractor.m_birthDate == DATE_TIME_NO_VAR(2017, 8, 5, 8, 0, 0));
        fructose_assert(extractor.m_hasBirthDate == true);
        //fructose_assert(extractor.m_pvValued.count(CovariateExtractor::sm_birthDateCName) == 1);
    }


    /// \brief Perform the test 2_1 in the spreadsheet, where measures for the weight are added (and therefore values
    ///        have to be correctly interpolated) without the periodicity of the weight.
    void testCovariateExtraction_test2_1(const std::string& /* _testName */)
    {
        CovariateDefinitions cDefinitions;
        ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_NO_R_UNIT(Weight, 3.5, Standard, Double, Linear, kg, cDefinitions);
        ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);

        ADD_EXPR2_CDEF(Special, "Weight * 0.5 + IsMale * 15", "Weight", "IsMale", cDefinitions);
        ADD_EXPR3_CDEF(VerySpecial, "Weight * 0.5 + IsMale * 15 + Gist * 25", "Weight", "IsMale", "Gist", cDefinitions);

        // gist == true @ 13.08.2017, 12h32.
        PatientVariates pVariates;
        ADD_PV_NO_UNIT(Gist, true, Bool, DATE_TIME_NO_VAR(2017, 8, 12, 12, 32, 0), pVariates);
        // gist == false @ 13.08.2017, 14h32.
        ADD_PV_NO_UNIT(Gist, false, Bool, DATE_TIME_NO_VAR(2017, 8, 13, 6, 32, 0), pVariates);
        // weight = 6.0 @ 12.08.2017, 14h32
        ADD_PV_W_UNIT(Weight, 6.0, Double, kg, DATE_TIME_NO_VAR(2017, 8, 12, 14, 32, 0), pVariates);
        // weight = 30.0 @ 13.08.2017, 14h32
        ADD_PV_W_UNIT(Weight, 30.0, Double, kg, DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0), pVariates);
        // weight = 40.0 @ 15.08.2017, 14h32
        ADD_PV_W_UNIT(Weight, 40.0, Double, kg, DATE_TIME_NO_VAR(2017, 8, 15, 14, 32, 0), pVariates);

        const DATE_TIME_VAR(startDate, 2017, 8, 12, 8, 0, 0);
        const DATE_TIME_VAR(endDate,   2017, 8, 17, 21, 0, 0);

        CovariateExtractor extractor(cDefinitions, pVariates, startDate, endDate);
        CovariateSeries series;
        CovariateExtractor::Result rc;
        rc = extractor.extract(series);

//        printCovariateSeries(series);

        fructose_assert(series.size() == 15);
        fructose_assert(rc == CovariateExtractor::Result::Ok);
        // First measure propagated back to the start of the interval.
        fructose_assert(covariateEventIsPresent("Gist",
                                                DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0),
                                                1,
                                                series));
        // Second measure at the moment it is performed.
        fructose_assert(covariateEventIsPresent("Gist",
                                                DATE_TIME_NO_VAR(2017, 8, 13, 6, 32, 0),
                                                0,
                                                series));
        // Weight, interpolated at the moment the gist changed.
        fructose_assert(covariateEventIsPresent("Weight",
                                                DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0),
                                                6.0,
                                                series));
        fructose_assert(covariateEventIsPresent("Weight",
                                                DATE_TIME_NO_VAR(2017, 8, 13, 6, 32, 0),
                                                22.0,
                                                series));
        fructose_assert(covariateEventIsPresent("Weight",
                                                DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0),
                                                30.0,
                                                series));
        fructose_assert(covariateEventIsPresent("Weight",
                                                DATE_TIME_NO_VAR(2017, 8, 15, 14, 32, 0),
                                                40.0,
                                                series));
        // This does not change.
        fructose_assert(covariateEventIsPresent("IsMale",
                                                DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0),
                                                1,
                                                series));
        // This should be computed from IsMale and Weight.
        fructose_assert(covariateEventIsPresent("Special",
                                                DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0),
                                                18.0,
                                                series));
        fructose_assert(covariateEventIsPresent("Special",
                                                DATE_TIME_NO_VAR(2017, 8, 13, 6, 32, 0),
                                                26.0,
                                                series));
        fructose_assert(covariateEventIsPresent("Special",
                                                DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0),
                                                30.0,
                                                series));
        fructose_assert(covariateEventIsPresent("Special",
                                                DATE_TIME_NO_VAR(2017, 8, 15, 14, 32, 0),
                                                35.0,
                                                series));
        // This should be computed from IsMale, Weight, and Gist.
        fructose_assert(covariateEventIsPresent("VerySpecial",
                                                DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0),
                                                43.0,
                                                series));
        fructose_assert(covariateEventIsPresent("VerySpecial",
                                                DATE_TIME_NO_VAR(2017, 8, 13, 6, 32, 0),
                                                26.0,
                                                series));
        fructose_assert(covariateEventIsPresent("VerySpecial",
                                                DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0),
                                                30.0,
                                                series));
        fructose_assert(covariateEventIsPresent("VerySpecial",
                                                DATE_TIME_NO_VAR(2017, 8, 15, 14, 32, 0),
                                                35.0,
                                                series));
    }


    /// \brief Perform the test 2_2 in the spreadsheet, where measures for the weight are added (and therefore values
    ///        have to be correctly interpolated) keeping a weight periodicity of 1 day.
    void testCovariateExtraction_test2_2(const std::string& /* _testName */)
    {
        CovariateDefinitions cDefinitions;
        ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), kg, cDefinitions);
        ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);

        ADD_EXPR2_CDEF(Special, "Weight * 0.5 + IsMale * 15", "Weight", "IsMale", cDefinitions);
        ADD_EXPR3_CDEF(VerySpecial, "Weight * 0.5 + IsMale * 15 + Gist * 25", "Weight", "IsMale", "Gist", cDefinitions);

        // gist == true @ 13.08.2017, 12h32.
        PatientVariates pVariates;
        ADD_PV_NO_UNIT(Gist, true, Bool, DATE_TIME_NO_VAR(2017, 8, 12, 12, 32, 0), pVariates);
        // gist == false @ 13.08.2017, 06h00.
        ADD_PV_NO_UNIT(Gist, false, Bool, DATE_TIME_NO_VAR(2017, 8, 13, 6, 00, 0), pVariates);
        // weight = 6.0 @ 12.08.2017, 14h00
        ADD_PV_W_UNIT(Weight, 6.0, Double, kg, DATE_TIME_NO_VAR(2017, 8, 12, 14, 00, 0), pVariates);
        // weight = 30.0 @ 13.08.2017, 14h00
        ADD_PV_W_UNIT(Weight, 30.0, Double, kg, DATE_TIME_NO_VAR(2017, 8, 13, 14, 00, 0), pVariates);
        // weight = 78.0 @ 15.08.2017, 14h00
        ADD_PV_W_UNIT(Weight, 78.0, Double, kg, DATE_TIME_NO_VAR(2017, 8, 15, 14, 00, 0), pVariates);

        const DATE_TIME_VAR(startDate, 2017, 8, 12, 8, 0, 0);
        const DATE_TIME_VAR(endDate,   2017, 8, 16, 21, 0, 0);

        CovariateExtractor extractor(cDefinitions, pVariates, startDate, endDate);
        CovariateSeries series;
        CovariateExtractor::Result rc;
        rc = extractor.extract(series);

//        printCovariateSeries(series);

        fructose_assert(series.size() == 21);
        fructose_assert(rc == CovariateExtractor::Result::Ok);
        // This does not change.
        fructose_assert(covariateEventIsPresent("IsMale",
                                                DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0),
                                                1,
                                                series));
        // First measure propagated back to the start of the interval.
        fructose_assert(covariateEventIsPresent("Gist",
                                                DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0),
                                                1,
                                                series));
        // Second measure at the moment it is performed.
        fructose_assert(covariateEventIsPresent("Gist",
                                                DATE_TIME_NO_VAR(2017, 8, 13, 6, 0, 0),
                                                0,
                                                series));
        // Weight, interpolated at the moment the gist changed.
        fructose_assert(covariateEventIsPresent("Weight",
                                                DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0),
                                                6.0,
                                                series));
        fructose_assert(covariateEventIsPresent("Weight",
                                                DATE_TIME_NO_VAR(2017, 8, 13, 6, 0, 0),
                                                22.0,
                                                series));
        fructose_assert(covariateEventIsPresent("Weight",
                                                DATE_TIME_NO_VAR(2017, 8, 13, 8, 0, 0),
                                                24.0,
                                                series));
        fructose_assert(covariateEventIsPresent("Weight",
                                                DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0),
                                                48.0,
                                                series));
        fructose_assert(covariateEventIsPresent("Weight",
                                                DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0),
                                                72.0,
                                                series));
        fructose_assert(covariateEventIsPresent("Weight",
                                                DATE_TIME_NO_VAR(2017, 8, 16, 8, 0, 0),
                                                78.0,
                                                series));
        // This should be computed from IsMale and Weight.
        fructose_assert(covariateEventIsPresent("Special",
                                                DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0),
                                                18.0,
                                                series));
        fructose_assert(covariateEventIsPresent("Special",
                                                DATE_TIME_NO_VAR(2017, 8, 13, 6, 0, 0),
                                                26.0,
                                                series));
        fructose_assert(covariateEventIsPresent("Special",
                                                DATE_TIME_NO_VAR(2017, 8, 13, 8, 0, 0),
                                                27.0,
                                                series));
        fructose_assert(covariateEventIsPresent("Special",
                                                DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0),
                                                39.0,
                                                series));
        fructose_assert(covariateEventIsPresent("Special",
                                                DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0),
                                                51.0,
                                                series));
        fructose_assert(covariateEventIsPresent("Special",
                                                DATE_TIME_NO_VAR(2017, 8, 16, 8, 0, 0),
                                                54.0,
                                                series));
        // This should be computed from IsMale, Weight, and Gist.
        fructose_assert(covariateEventIsPresent("VerySpecial",
                                                DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0),
                                                43.0,
                                                series));
        fructose_assert(covariateEventIsPresent("VerySpecial",
                                                DATE_TIME_NO_VAR(2017, 8, 13, 6, 0, 0),
                                                26.0,
                                                series));
        fructose_assert(covariateEventIsPresent("VerySpecial",
                                                DATE_TIME_NO_VAR(2017, 8, 13, 8, 0, 0),
                                                27.0,
                                                series));
        fructose_assert(covariateEventIsPresent("VerySpecial",
                                                DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0),
                                                39.0,
                                                series));
        fructose_assert(covariateEventIsPresent("VerySpecial",
                                                DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0),
                                                51.0,
                                                series));
        fructose_assert(covariateEventIsPresent("VerySpecial",
                                                DATE_TIME_NO_VAR(2017, 8, 16, 8, 0, 0),
                                                54.0,
                                                series));
    }


    /// \brief Perform the test 2_3 in the spreadsheet, which ensures that values are correctly interpolated when
    ///        multiple CVs with linear interpolation are present.
    void testCovariateExtraction_test2_3(const std::string& /* _testName */)
    {
        CovariateDefinitions cDefinitions;
        ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), kg, cDefinitions);
        ADD_CDEF_NO_R_UNIT(Height, 100, Standard, Double, Linear, cm, cDefinitions);

        ADD_EXPR2_CDEF(Special, "Weight * 0.5 + Height", "Weight", "Height", cDefinitions);

        // gist == true @ 13.08.2017, 12h32.
        PatientVariates pVariates;
        ADD_PV_NO_UNIT(Gist, true, Bool, DATE_TIME_NO_VAR(2017, 8, 12, 12, 32, 0), pVariates);
        // gist == false @ 13.08.2017, 06h00.
        ADD_PV_NO_UNIT(Gist, false, Bool, DATE_TIME_NO_VAR(2017, 8, 13, 6, 0, 0), pVariates);

        // weight = 6.0 @ 12.08.2017, 14h00
        ADD_PV_W_UNIT(Weight, 6.0, Double, kg, DATE_TIME_NO_VAR(2017, 8, 12, 14, 0, 0), pVariates);
        // weight = 30.0 @ 13.08.2017, 14h00
        ADD_PV_W_UNIT(Weight, 30.0, Double, kg, DATE_TIME_NO_VAR(2017, 8, 13, 14, 0, 0), pVariates);
        // weight = 78.0 @ 15.08.2017, 14h00
        ADD_PV_W_UNIT(Weight, 78.0, Double, kg, DATE_TIME_NO_VAR(2017, 8, 15, 14, 0, 0), pVariates);

        // height = 20.0 @ 12.08.2017, 20h00
        ADD_PV_W_UNIT(Height, 20.0, Double, kg, DATE_TIME_NO_VAR(2017, 8, 12, 20, 0, 0), pVariates);
        // height = 40.0 @ 12.08.2017, 22h00
        ADD_PV_W_UNIT(Height, 40.0, Double, kg, DATE_TIME_NO_VAR(2017, 8, 12, 22, 0, 0), pVariates);
        // height = 280.0 @ 13.08.2017, 22h00
        ADD_PV_W_UNIT(Height, 280.0, Double, kg, DATE_TIME_NO_VAR(2017, 8, 13, 22, 0, 0), pVariates);
        // height = 380.0 @ 14.08.2017, 08h00
        ADD_PV_W_UNIT(Height, 380.0, Double, kg, DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0), pVariates);

        const DATE_TIME_VAR(startDate, 2017, 8, 12, 8, 0, 0);
        const DATE_TIME_VAR(endDate,   2017, 8, 16, 21, 0, 0);

        CovariateExtractor extractor(cDefinitions, pVariates, startDate, endDate);
        CovariateSeries series;
        CovariateExtractor::Result rc;
        rc = extractor.extract(series);

//        printCovariateSeries(series);

        fructose_assert(series.size() == 26);
        fructose_assert(rc == CovariateExtractor::Result::Ok);
        // First measure propagated back to the start of the interval.
        fructose_assert(covariateEventIsPresent("Gist",
                                                DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0),
                                                1,
                                                series));
        // Second measure at the moment it is performed.
        fructose_assert(covariateEventIsPresent("Gist",
                                                DATE_TIME_NO_VAR(2017, 8, 13, 6, 0, 0),
                                                0,
                                                series));
        // Weight.
        fructose_assert(covariateEventIsPresent("Weight",
                                                DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0),
                                                6.0,
                                                series));
        fructose_assert(covariateEventIsPresent("Weight",
                                                DATE_TIME_NO_VAR(2017, 8, 12, 20, 0, 0),
                                                12.0,
                                                series));
        fructose_assert(covariateEventIsPresent("Weight",
                                                DATE_TIME_NO_VAR(2017, 8, 12, 22, 0, 0),
                                                14.0,
                                                series));
        fructose_assert(covariateEventIsPresent("Weight",
                                                DATE_TIME_NO_VAR(2017, 8, 13, 6, 0, 0),
                                                22.0,
                                                series));
        fructose_assert(covariateEventIsPresent("Weight",
                                                DATE_TIME_NO_VAR(2017, 8, 13, 8, 0, 0),
                                                24.0,
                                                series));
        fructose_assert(covariateEventIsPresent("Weight",
                                                DATE_TIME_NO_VAR(2017, 8, 13, 22, 0, 0),
                                                38.0,
                                                series));
        fructose_assert(covariateEventIsPresent("Weight",
                                                DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0),
                                                48.0,
                                                series));
        fructose_assert(covariateEventIsPresent("Weight",
                                                DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0),
                                                72.0,
                                                series));
        fructose_assert(covariateEventIsPresent("Weight",
                                                DATE_TIME_NO_VAR(2017, 8, 16, 8, 0, 0),
                                                78.0,
                                                series));
        // Height.
        fructose_assert(covariateEventIsPresent("Height",
                                                DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0),
                                                20.0,
                                                series));
        fructose_assert(covariateEventIsPresent("Height",
                                                DATE_TIME_NO_VAR(2017, 8, 12, 22, 0, 0),
                                                40.0,
                                                series));
        fructose_assert(covariateEventIsPresent("Height",
                                                DATE_TIME_NO_VAR(2017, 8, 13, 6, 0, 0),
                                                120.0,
                                                series));
        fructose_assert(covariateEventIsPresent("Height",
                                                DATE_TIME_NO_VAR(2017, 8, 13, 8, 0, 0),
                                                140.0,
                                                series));
        fructose_assert(covariateEventIsPresent("Height",
                                                DATE_TIME_NO_VAR(2017, 8, 13, 22, 0, 0),
                                                280.0,
                                                series));
        fructose_assert(covariateEventIsPresent("Height",
                                                DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0),
                                                380.0,
                                                series));
        // This should be computed from Height and Weight.
        fructose_assert(covariateEventIsPresent("Special",
                                                DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0),
                                                23.0,
                                                series));
        fructose_assert(covariateEventIsPresent("Special",
                                                DATE_TIME_NO_VAR(2017, 8, 12, 20, 0, 0),
                                                26.0,
                                                series));
        fructose_assert(covariateEventIsPresent("Special",
                                                DATE_TIME_NO_VAR(2017, 8, 12, 22, 0, 0),
                                                47.0,
                                                series));
        fructose_assert(covariateEventIsPresent("Special",
                                                DATE_TIME_NO_VAR(2017, 8, 13, 6, 0, 0),
                                                131.0,
                                                series));
        fructose_assert(covariateEventIsPresent("Special",
                                                DATE_TIME_NO_VAR(2017, 8, 13, 8, 0, 0),
                                                152.0,
                                                series));
        fructose_assert(covariateEventIsPresent("Special",
                                                DATE_TIME_NO_VAR(2017, 8, 13, 22, 0, 0),
                                                299.0,
                                                series));
        fructose_assert(covariateEventIsPresent("Special",
                                                DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0),
                                                404.0,
                                                series));
        fructose_assert(covariateEventIsPresent("Special",
                                                DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0),
                                                416.0,
                                                series));
        fructose_assert(covariateEventIsPresent("Special",
                                                DATE_TIME_NO_VAR(2017, 8, 16, 8, 0, 0),
                                                419.0,
                                                series));
    }


    /// \brief Perform the test 3_0 in the spreadsheet, which expects the age (expressed in days) to be updated each
    ///        day, with an operator depending on it.
    void testCovariateExtraction_test3_0(const std::string& /* _testName */)
    {
        CovariateDefinitions cDefinitions;
        ADD_CDEF_NO_R_UNIT(Weight, 3.5, Standard, Double, Linear, kg, cDefinitions);
        ADD_CDEF_NO_R(AgeDays, 35, AgeInDays, Double, Direct, cDefinitions);
        //ADD_CDEF_NO_R_STR(CovariateExtractor::sm_birthDateCName, DATE_TIME_NO_VAR(2017, 8, 5, 18, 0, 0), Standard,
        //                  Date, Direct, cDefinitions);

        ADD_EXPR2_CDEF(DepOnAge, "AgeDays + Weight * 2", "AgeDays", "Weight", cDefinitions);

        PatientVariates pVariates;
        // weight = 6.0 @ 12.08.2017, 14h00
        ADD_PV_W_UNIT(Weight, 4.0, Double, kg, DATE_TIME_NO_VAR(2017, 8, 13, 12, 0, 0), pVariates);
        // weight = 30.0 @ 13.08.2017, 14h00
        ADD_PV_W_UNIT(Weight, 100.0, Double, kg, DATE_TIME_NO_VAR(2017, 8, 17, 12, 0, 0), pVariates);
        ADD_PV_NO_UNIT(birthdate, DATE_TIME_NO_VAR(2017, 8, 5, 18, 0, 0), Date,
                       DATE_TIME_NO_VAR(2017, 8, 5, 18, 0, 0), pVariates);

        const DATE_TIME_VAR(startDate, 2017, 8, 12, 8, 0, 0);
        const DATE_TIME_VAR(endDate,   2017, 8, 18, 21, 0, 0);

        CovariateExtractor extractor(cDefinitions, pVariates, startDate, endDate);
        CovariateSeries series;
        CovariateExtractor::Result rc;
        rc = extractor.extract(series);

//        printCovariateSeries(series);

        fructose_assert(series.size() == 23);
        fructose_assert(rc == CovariateExtractor::Result::Ok);
        fructose_assert(covariateEventIsPresent("Weight",
                                                DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0),
                                                4.0,
                                                series));
        fructose_assert(covariateEventIsPresent("Weight",
                                                DATE_TIME_NO_VAR(2017, 8, 13, 18, 0, 0),
                                                10.0,
                                                series));
        fructose_assert(covariateEventIsPresent("Weight",
                                                DATE_TIME_NO_VAR(2017, 8, 14, 18, 0, 0),
                                                34.0,
                                                series));
        fructose_assert(covariateEventIsPresent("Weight",
                                                DATE_TIME_NO_VAR(2017, 8, 15, 18, 0, 0),
                                                58.0,
                                                series));
        fructose_assert(covariateEventIsPresent("Weight",
                                                DATE_TIME_NO_VAR(2017, 8, 16, 18, 0, 0),
                                                82.0,
                                                series));
        fructose_assert(covariateEventIsPresent("Weight",
                                                DATE_TIME_NO_VAR(2017, 8, 17, 12, 0, 0),
                                                100.0,
                                                series));

        fructose_assert(covariateEventIsPresent("AgeDays",
                                                DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0),
                                                6.0,
                                                series));
        fructose_assert(covariateEventIsPresent("AgeDays",
                                                DATE_TIME_NO_VAR(2017, 8, 12, 18, 0, 0),
                                                7.0,
                                                series));
        fructose_assert(covariateEventIsPresent("AgeDays",
                                                DATE_TIME_NO_VAR(2017, 8, 13, 18, 0, 0),
                                                8.0,
                                                series));
        fructose_assert(covariateEventIsPresent("AgeDays",
                                                DATE_TIME_NO_VAR(2017, 8, 14, 18, 0, 0),
                                                9.0,
                                                series));
        fructose_assert(covariateEventIsPresent("AgeDays",
                                                DATE_TIME_NO_VAR(2017, 8, 15, 18, 0, 0),
                                                10.0,
                                                series));
        fructose_assert(covariateEventIsPresent("AgeDays",
                                                DATE_TIME_NO_VAR(2017, 8, 16, 18, 0, 0),
                                                11.0,
                                                series));
        fructose_assert(covariateEventIsPresent("AgeDays",
                                                DATE_TIME_NO_VAR(2017, 8, 17, 18, 0, 0),
                                                12.0,
                                                series));
        fructose_assert(covariateEventIsPresent("AgeDays",
                                                DATE_TIME_NO_VAR(2017, 8, 18, 18, 0, 0),
                                                13.0,
                                                series));

        fructose_assert(covariateEventIsPresent("DepOnAge",
                                                DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0),
                                                14.0,
                                                series));
        fructose_assert(covariateEventIsPresent("DepOnAge",
                                                DATE_TIME_NO_VAR(2017, 8, 12, 18, 0, 0),
                                                15.0,
                                                series));
        fructose_assert(covariateEventIsPresent("DepOnAge",
                                                DATE_TIME_NO_VAR(2017, 8, 13, 18, 0, 0),
                                                28.0,
                                                series));
        fructose_assert(covariateEventIsPresent("DepOnAge",
                                                DATE_TIME_NO_VAR(2017, 8, 14, 18, 0, 0),
                                                77.0,
                                                series));
        fructose_assert(covariateEventIsPresent("DepOnAge",
                                                DATE_TIME_NO_VAR(2017, 8, 15, 18, 0, 0),
                                                126.0,
                                                series));
        fructose_assert(covariateEventIsPresent("DepOnAge",
                                                DATE_TIME_NO_VAR(2017, 8, 16, 18, 0, 0),
                                                175.0,
                                                series));
        fructose_assert(covariateEventIsPresent("DepOnAge",
                                                DATE_TIME_NO_VAR(2017, 8, 17, 12, 0, 0),
                                                211.0,
                                                series));
        fructose_assert(covariateEventIsPresent("DepOnAge",
                                                DATE_TIME_NO_VAR(2017, 8, 17, 18, 0, 0),
                                                212.0,
                                                series));
        fructose_assert(covariateEventIsPresent("DepOnAge",
                                                DATE_TIME_NO_VAR(2017, 8, 18, 18, 0, 0),
                                                213.0,
                                                series));

        fructose_assert(extractor.m_birthDate == DATE_TIME_NO_VAR(2017, 8, 5, 18, 0, 0));
        fructose_assert(extractor.m_hasBirthDate == true);
        fructose_assert(extractor.m_pvValued.count(CovariateExtractor::sm_birthDateCName) == 0);
    }


    /// \brief Perform the test 3_1 in the spreadsheet, which expects the age (expressed in days) to be updated each
    ///        day, with an operator depending on it -- same as 3_0, but add another operator depending on this latter.
    void testCovariateExtraction_test3_1(const std::string& /* _testName */)
    {
        CovariateDefinitions cDefinitions;
        ADD_CDEF_NO_R_UNIT(Weight, 3.5, Standard, Double, Linear, kg, cDefinitions);
        ADD_CDEF_NO_R(AgeDays, 35, AgeInDays, Double, Direct, cDefinitions);
        //ADD_CDEF_NO_R_STR(CovariateExtractor::sm_birthDateCName, DATE_TIME_NO_VAR(2017, 8, 5, 18, 0, 0), Standard,
        //                  Date, Direct, cDefinitions);


        ADD_EXPR2_CDEF(DepOnAge, "AgeDays + Weight * 2", "AgeDays", "Weight", cDefinitions);
        ADD_EXPR2_CDEF(DepDep, "DepOnAge + 42 + AgeDays", "AgeDays", "DepOnAge", cDefinitions);

        PatientVariates pVariates;
        // weight = 6.0 @ 12.08.2017, 14h00
        ADD_PV_W_UNIT(Weight, 4.0, Double, kg, DATE_TIME_NO_VAR(2017, 8, 13, 12, 0, 0), pVariates);
        // weight = 30.0 @ 13.08.2017, 14h00
        ADD_PV_W_UNIT(Weight, 100.0, Double, kg, DATE_TIME_NO_VAR(2017, 8, 17, 12, 0, 0), pVariates);

        ADD_PV_NO_UNIT(birthdate, DATE_TIME_NO_VAR(2017, 8, 5, 18, 0, 0), Date,
                       DATE_TIME_NO_VAR(2017, 8, 5, 18, 0, 0), pVariates);

        const DATE_TIME_VAR(startDate, 2017, 8, 12, 8, 0, 0);
        const DATE_TIME_VAR(endDate,   2017, 8, 18, 21, 0, 0);

        CovariateExtractor extractor(cDefinitions, pVariates, startDate, endDate);
        CovariateSeries series;
        CovariateExtractor::Result rc;
        rc = extractor.extract(series);

//        printCovariateSeries(series);

        fructose_assert(series.size() == 32);
        fructose_assert(rc == CovariateExtractor::Result::Ok);
        fructose_assert(covariateEventIsPresent("Weight",
                                                DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0),
                                                4.0,
                                                series));
        fructose_assert(covariateEventIsPresent("Weight",
                                                DATE_TIME_NO_VAR(2017, 8, 13, 18, 0, 0),
                                                10.0,
                                                series));
        fructose_assert(covariateEventIsPresent("Weight",
                                                DATE_TIME_NO_VAR(2017, 8, 14, 18, 0, 0),
                                                34.0,
                                                series));
        fructose_assert(covariateEventIsPresent("Weight",
                                                DATE_TIME_NO_VAR(2017, 8, 15, 18, 0, 0),
                                                58.0,
                                                series));
        fructose_assert(covariateEventIsPresent("Weight",
                                                DATE_TIME_NO_VAR(2017, 8, 16, 18, 0, 0),
                                                82.0,
                                                series));
        fructose_assert(covariateEventIsPresent("Weight",
                                                DATE_TIME_NO_VAR(2017, 8, 17, 12, 0, 0),
                                                100.0,
                                                series));

        fructose_assert(covariateEventIsPresent("AgeDays",
                                                DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0),
                                                6.0,
                                                series));
        fructose_assert(covariateEventIsPresent("AgeDays",
                                                DATE_TIME_NO_VAR(2017, 8, 12, 18, 0, 0),
                                                7.0,
                                                series));
        fructose_assert(covariateEventIsPresent("AgeDays",
                                                DATE_TIME_NO_VAR(2017, 8, 13, 18, 0, 0),
                                                8.0,
                                                series));
        fructose_assert(covariateEventIsPresent("AgeDays",
                                                DATE_TIME_NO_VAR(2017, 8, 14, 18, 0, 0),
                                                9.0,
                                                series));
        fructose_assert(covariateEventIsPresent("AgeDays",
                                                DATE_TIME_NO_VAR(2017, 8, 15, 18, 0, 0),
                                                10.0,
                                                series));
        fructose_assert(covariateEventIsPresent("AgeDays",
                                                DATE_TIME_NO_VAR(2017, 8, 16, 18, 0, 0),
                                                11.0,
                                                series));
        fructose_assert(covariateEventIsPresent("AgeDays",
                                                DATE_TIME_NO_VAR(2017, 8, 17, 18, 0, 0),
                                                12.0,
                                                series));
        fructose_assert(covariateEventIsPresent("AgeDays",
                                                DATE_TIME_NO_VAR(2017, 8, 18, 18, 0, 0),
                                                13.0,
                                                series));

        fructose_assert(covariateEventIsPresent("DepOnAge",
                                                DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0),
                                                14.0,
                                                series));
        fructose_assert(covariateEventIsPresent("DepOnAge",
                                                DATE_TIME_NO_VAR(2017, 8, 12, 18, 0, 0),
                                                15.0,
                                                series));
        fructose_assert(covariateEventIsPresent("DepOnAge",
                                                DATE_TIME_NO_VAR(2017, 8, 13, 18, 0, 0),
                                                28.0,
                                                series));
        fructose_assert(covariateEventIsPresent("DepOnAge",
                                                DATE_TIME_NO_VAR(2017, 8, 14, 18, 0, 0),
                                                77.0,
                                                series));
        fructose_assert(covariateEventIsPresent("DepOnAge",
                                                DATE_TIME_NO_VAR(2017, 8, 15, 18, 0, 0),
                                                126.0,
                                                series));
        fructose_assert(covariateEventIsPresent("DepOnAge",
                                                DATE_TIME_NO_VAR(2017, 8, 16, 18, 0, 0),
                                                175.0,
                                                series));
        fructose_assert(covariateEventIsPresent("DepOnAge",
                                                DATE_TIME_NO_VAR(2017, 8, 17, 12, 0, 0),
                                                211.0,
                                                series));
        fructose_assert(covariateEventIsPresent("DepOnAge",
                                                DATE_TIME_NO_VAR(2017, 8, 17, 18, 0, 0),
                                                212.0,
                                                series));
        fructose_assert(covariateEventIsPresent("DepOnAge",
                                                DATE_TIME_NO_VAR(2017, 8, 18, 18, 0, 0),
                                                213.0,
                                                series));

        fructose_assert(covariateEventIsPresent("DepDep",
                                                DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0),
                                                62.0,
                                                series));
        fructose_assert(covariateEventIsPresent("DepDep",
                                                DATE_TIME_NO_VAR(2017, 8, 12, 18, 0, 0),
                                                64.0,
                                                series));
        fructose_assert(covariateEventIsPresent("DepDep",
                                                DATE_TIME_NO_VAR(2017, 8, 13, 18, 0, 0),
                                                78.0,
                                                series));
        fructose_assert(covariateEventIsPresent("DepDep",
                                                DATE_TIME_NO_VAR(2017, 8, 14, 18, 0, 0),
                                                128.0,
                                                series));
        fructose_assert(covariateEventIsPresent("DepDep",
                                                DATE_TIME_NO_VAR(2017, 8, 15, 18, 0, 0),
                                                178.0,
                                                series));
        fructose_assert(covariateEventIsPresent("DepDep",
                                                DATE_TIME_NO_VAR(2017, 8, 16, 18, 0, 0),
                                                228.0,
                                                series));
        fructose_assert(covariateEventIsPresent("DepDep",
                                                DATE_TIME_NO_VAR(2017, 8, 17, 12, 0, 0),
                                                264.0,
                                                series));
        fructose_assert(covariateEventIsPresent("DepDep",
                                                DATE_TIME_NO_VAR(2017, 8, 17, 18, 0, 0),
                                                266.0,
                                                series));
        fructose_assert(covariateEventIsPresent("DepDep",
                                                DATE_TIME_NO_VAR(2017, 8, 18, 18, 0, 0),
                                                268.0,
                                                series));

        fructose_assert(extractor.m_birthDate == DATE_TIME_NO_VAR(2017, 8, 5, 18, 0, 0));
        fructose_assert(extractor.m_hasBirthDate == true);
        fructose_assert(extractor.m_pvValued.count(CovariateExtractor::sm_birthDateCName) == 0);
    }


    /// \brief Perform the test 3_2 in the spreadsheet, which expects the age (expressed in days) to be updated each
    ///        day, with an operator depending on it -- same as 3_0, but this time no birthdate.
    void testCovariateExtraction_test3_2(const std::string& /* _testName */)
    {
        CovariateDefinitions cDefinitions;
        ADD_CDEF_NO_R_UNIT(Weight, 3.5, Standard, Double, Linear, kg, cDefinitions);
        ADD_CDEF_NO_R(AgeDays, 42, AgeInDays, Double, Direct, cDefinitions);

        ADD_EXPR2_CDEF(DepOnAge, "AgeDays + Weight * 2", "AgeDays", "Weight", cDefinitions);

        PatientVariates pVariates;
        // weight = 6.0 @ 12.08.2017, 14h00
        ADD_PV_W_UNIT(Weight, 4.0, Double, kg, DATE_TIME_NO_VAR(2017, 8, 13, 12, 0, 0), pVariates);
        // weight = 30.0 @ 13.08.2017, 14h00
        ADD_PV_W_UNIT(Weight, 100.0, Double, kg, DATE_TIME_NO_VAR(2017, 8, 17, 12, 0, 0), pVariates);

        const DATE_TIME_VAR(startDate, 2017, 8, 12, 8, 0, 0);
        const DATE_TIME_VAR(endDate,   2017, 8, 18, 21, 0, 0);

        CovariateExtractor extractor(cDefinitions, pVariates, startDate, endDate);
        CovariateSeries series;
        CovariateExtractor::Result rc;
        rc = extractor.extract(series);

//        printCovariateSeries(series);

        fructose_assert(series.size() == 21);
        fructose_assert(rc == CovariateExtractor::Result::Ok);
        fructose_assert(covariateEventIsPresent("Weight",
                                                DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0),
                                                4.0,
                                                series));
        fructose_assert(covariateEventIsPresent("Weight",
                                                DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0),
                                                24.0,
                                                series));
        fructose_assert(covariateEventIsPresent("Weight",
                                                DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0),
                                                48.0,
                                                series));
        fructose_assert(covariateEventIsPresent("Weight",
                                                DATE_TIME_NO_VAR(2017, 8, 16, 8, 0, 0),
                                                72.0,
                                                series));
        fructose_assert(covariateEventIsPresent("Weight",
                                                DATE_TIME_NO_VAR(2017, 8, 17, 8, 0, 0),
                                                96.0,
                                                series));
        fructose_assert(covariateEventIsPresent("Weight",
                                                DATE_TIME_NO_VAR(2017, 8, 17, 12, 0, 0),
                                                100.0,
                                                series));

        fructose_assert(covariateEventIsPresent("AgeDays",
                                                DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0),
                                                42.0,
                                                series));
        fructose_assert(covariateEventIsPresent("AgeDays",
                                                DATE_TIME_NO_VAR(2017, 8, 13, 8, 0, 0),
                                                43.0,
                                                series));
        fructose_assert(covariateEventIsPresent("AgeDays",
                                                DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0),
                                                44.0,
                                                series));
        fructose_assert(covariateEventIsPresent("AgeDays",
                                                DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0),
                                                45.0,
                                                series));
        fructose_assert(covariateEventIsPresent("AgeDays",
                                                DATE_TIME_NO_VAR(2017, 8, 16, 8, 0, 0),
                                                46.0,
                                                series));
        fructose_assert(covariateEventIsPresent("AgeDays",
                                                DATE_TIME_NO_VAR(2017, 8, 17, 8, 0, 0),
                                                47.0,
                                                series));
        fructose_assert(covariateEventIsPresent("AgeDays",
                                                DATE_TIME_NO_VAR(2017, 8, 18, 8, 0, 0),
                                                48.0,
                                                series));

        fructose_assert(covariateEventIsPresent("DepOnAge",
                                                DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0),
                                                50.0,
                                                series));
        fructose_assert(covariateEventIsPresent("DepOnAge",
                                                DATE_TIME_NO_VAR(2017, 8, 13, 8, 0, 0),
                                                51.0,
                                                series));
        fructose_assert(covariateEventIsPresent("DepOnAge",
                                                DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0),
                                                92.0,
                                                series));
        fructose_assert(covariateEventIsPresent("DepOnAge",
                                                DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0),
                                                141.0,
                                                series));
        fructose_assert(covariateEventIsPresent("DepOnAge",
                                                DATE_TIME_NO_VAR(2017, 8, 16, 8, 0, 0),
                                                190.0,
                                                series));
        fructose_assert(covariateEventIsPresent("DepOnAge",
                                                DATE_TIME_NO_VAR(2017, 8, 17, 8, 0, 0),
                                                239.0,
                                                series));
        fructose_assert(covariateEventIsPresent("DepOnAge",
                                                DATE_TIME_NO_VAR(2017, 8, 17, 12, 0, 0),
                                                247.0,
                                                series));
        fructose_assert(covariateEventIsPresent("DepOnAge",
                                                DATE_TIME_NO_VAR(2017, 8, 18, 8, 0, 0),
                                                248.0,
                                                series));

        fructose_assert(extractor.m_hasBirthDate == false);
    }
};

#endif // TEST_COVARIATEEXTRACTOR_H
