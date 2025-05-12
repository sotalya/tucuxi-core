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


#include <iostream>
#include <memory>

#include <date/date.h>
#include <gtest/gtest.h>

#include "tucucommon/timeofday.h"

#include "tucucore/covariateextractor.h"
#include "tucucore/dosage.h"
#include "tucucore/drugmodel/covariatedefinition.h"
#include "tucucore/drugtreatment/patientcovariate.h"

#include "testutils.h"


using namespace Tucuxi::Common::Utils;
using namespace Tucuxi::Core;

/// \brief Check whether a covariate event with a given name, date, and value is present in a series.
/// \param _id Name of the covariate to check.
/// \param _date Expected date of the covariate event.
/// \param _value Expected value of the covariate at the time of the event.
/// \param _series Vector of events in which the event is sought.
/// \return True if the event is present, false otherwise.
static bool covariateEventIsPresent(
        const std::string& _id, const DateTime& _date, const Value& _value, const CovariateSeries& _series)
{
    for (const auto& covEl : _series) {
        if (covEl.getId() == _id && fabs(covEl.getValue() - _value) < 1e-4 && covEl.getEventTime() == _date) {
            return true;
        }
    }
    return false;
}


/// \brief Print the elements of a covariate series.
/// \param _series Series to print.
static void printCovariateSeries(const CovariateSeries& _series)
{
    std::cerr << "--------- " << _series.size() << " ----------\n";
    for (const auto& covEl : _series) {
        std::cerr << covEl.getId() << " @ " << covEl.getEventTime();
        std::cerr << " = " << covEl.getValue() << "\n";
    }
    std::cerr << "-----------------------------------------\n";
}

class TestCovariateExtractor
{
public:
    static bool get_m_hasBirthDate(CovariateExtractor* extractor)
    {
        return extractor->m_hasBirthDate;
    };

    static DateTime get_m_birthDate(CovariateExtractor* extractor)
    {
        return extractor->m_birthDate;
    };

    static bool get_m_hasStartOfTreatmentDate(CovariateExtractor* extractor)
    {
        return extractor->m_hasStartOfTreatmentDate;
    };

    static double get_m_initTimeFromStartInHours(CovariateExtractor* extractor)
    {
        return extractor->m_initTimeFromStartInHours;
    };

    static double get_m_initTimeFromStartInDays(CovariateExtractor* extractor)
    {
        return extractor->m_initTimeFromStartInDays;
    };

    static double get_m_initTimeFromStartInWeeks(CovariateExtractor* extractor)
    {
        return extractor->m_initTimeFromStartInWeeks;
    };

    static double get_m_initTimeFromStartInMonths(CovariateExtractor* extractor)
    {
        return extractor->m_initTimeFromStartInMonths;
    };

    static double get_m_initTimeFromStartInYears(CovariateExtractor* extractor)
    {
        return extractor->m_initTimeFromStartInYears;
    };

    static double get_m_initAgeInDays(CovariateExtractor* extractor)
    {
        return extractor->m_initAgeInDays;
    };

    static double get_m_initAgeInWeeks(CovariateExtractor* extractor)
    {
        return extractor->m_initAgeInWeeks;
    };

    static double get_m_initAgeInMonths(CovariateExtractor* extractor)
    {
        return extractor->m_initAgeInMonths;
    };

    static double get_m_initAgeInYears(CovariateExtractor* extractor)
    {
        return extractor->m_initAgeInYears;
    };

    static OperableGraphManager get_m_ogm(CovariateExtractor* extractor)
    {
        return extractor->m_ogm;
    };

    static std::map<std::string, cdIterator_t> get_m_cdValued(CovariateExtractor* extractor)
    {
        return extractor->m_cdValued;
    };

    static std::map<std::string, cdIterator_t> get_m_cdComputed(CovariateExtractor* extractor)
    {
        return extractor->m_cdComputed;
    };

    static std::map<std::string, std::vector<pvIterator_t>> get_m_pvValued(CovariateExtractor* extractor)
    {
        return extractor->m_pvValued;
    };

    static void test_sortPatientVariates(CovariateExtractor* extractor)
    {
        extractor->sortPatientVariates();
    }

    static void test_collectRefreshIntervals(
            std::map<DateTime, std::vector<std::string>>& _refreshMap, CovariateExtractor* extractor)
    {
        extractor->collectRefreshIntervals(_refreshMap);
    }

    static bool test_interpolateValues(
            const Value _val1,
            const DateTime& _date1,
            const Value _val2,
            const DateTime& _date2,
            const DateTime& _dateRes,
            const InterpolationType _interpolationType,
            Value& _valRes,
            CovariateExtractor* extractor)
    {
        return extractor->interpolateValues(_val1, _date1, _val2, _date2, _dateRes, _interpolationType, _valRes);
    }

    static Tucuxi::Core::DailyDose get_dosage()
    {
        const DoseValue validDose = 100.0;
        const FormulationAndRoute routePerfusion(Formulation::Test, AdministrationRoute::IntravenousDrip);
        const Duration validInfusionTime(std::chrono::minutes(120));
        const TimeOfDay validTimeOfDay(Duration(std::chrono::seconds(12345)));

        return Tucuxi::Core::DailyDose(validDose, TucuUnit("mg"), routePerfusion, validInfusionTime, validTimeOfDay);
    }

    static DosageHistory get_dosage_history(DateTime const& _start, DateTime const& _end)
    {
        DosageHistory dosage_history = DosageHistory();
        dosage_history.addTimeRange(DosageTimeRange(_start, _end, TestCovariateExtractor::get_dosage()));
        return dosage_history;
    }
};

TEST(Core_TestCovariateExtractor, CE_constructor)
{
    // Even without covariates, no exception should be raised.
    {
        ASSERT_NO_THROW(CovariateExtractor(
                CovariateDefinitions(),
                PatientVariates(),
                DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0),
                DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0)));
    }

    // No covariates, but a dosage history.
    {
        DosageHistory dosage_history = TestCovariateExtractor::get_dosage_history(
                DATE_TIME_NO_VAR(2017, 8, 11, 14, 32, 0), DATE_TIME_NO_VAR(2017, 8, 14, 14, 32, 0));
        ASSERT_NO_THROW(CovariateExtractor(
                CovariateDefinitions(),
                PatientVariates(),
                DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0),
                DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                dosage_history));
    }

    // Start date past end date.
    {
        ASSERT_THROW(
                CovariateExtractor(
                        CovariateDefinitions(),
                        PatientVariates(),
                        DATE_TIME_NO_VAR(2017, 8, 19, 14, 32, 0),
                        DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0)),
                std::runtime_error);
    }

    // Build a covariate extractor from a set of covariate definitions.
    {
        CovariateDefinitions cDefinitions;

        ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
        ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);

        ASSERT_NO_THROW(CovariateExtractor(
                cDefinitions,
                PatientVariates(),
                DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                DATE_TIME_NO_VAR(2017, 8, 19, 14, 32, 0)));
    }

    // Build a covariate extractor from a set of covariate definitions and a
    // dosage history that ends before the desired interval.
    {
        CovariateDefinitions cDefinitions;

        ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
        ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);

        DosageHistory dosage_history = TestCovariateExtractor::get_dosage_history(
                DATE_TIME_NO_VAR(2017, 8, 11, 14, 32, 0), DATE_TIME_NO_VAR(2017, 8, 14, 14, 32, 0));
        ASSERT_NO_THROW(CovariateExtractor(
                cDefinitions,
                PatientVariates(),
                DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                DATE_TIME_NO_VAR(2017, 8, 19, 14, 32, 0),
                dosage_history));
    }

    // Build a covariate extractor from a set of covariate definitions and a
    // dosage history that matches the desired interval.
    {
        CovariateDefinitions cDefinitions;

        ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
        ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);

        DosageHistory dosage_history = TestCovariateExtractor::get_dosage_history(
                DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0), DATE_TIME_NO_VAR(2017, 8, 19, 14, 32, 0));
        ASSERT_NO_THROW(CovariateExtractor(
                cDefinitions,
                PatientVariates(),
                DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                DATE_TIME_NO_VAR(2017, 8, 19, 14, 32, 0),
                dosage_history));
    }

    // Build a covariate extractor from a set of covariate definitions and a
    // dosage history that are past the desired interval.
    {
        CovariateDefinitions cDefinitions;

        ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
        ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);

        DosageHistory dosage_history = TestCovariateExtractor::get_dosage_history(
                DATE_TIME_NO_VAR(2017, 9, 18, 14, 32, 0), DATE_TIME_NO_VAR(2017, 9, 19, 14, 32, 0));
        ASSERT_NO_THROW(CovariateExtractor(
                cDefinitions,
                PatientVariates(),
                DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                DATE_TIME_NO_VAR(2017, 8, 19, 14, 32, 0),
                dosage_history));
    }

    // Build a covariate extractor from a set of covariate definitions, where
    // non-standard definitions are present.
    {
        CovariateDefinitions cDefinitions;
        PatientVariates pVariates;

        ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
        ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_NO_R(AgeDays, 20, AgeInDays, Double, Direct, cDefinitions);
        ADD_CDEF_NO_R(AgeMonths, 3, AgeInMonths, Double, Direct, cDefinitions);
        ADD_CDEF_NO_R(AgeYears, 15, AgeInYears, Double, Direct, cDefinitions);

        ASSERT_NO_THROW(CovariateExtractor(
                cDefinitions,
                PatientVariates(),
                DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                DATE_TIME_NO_VAR(2017, 8, 19, 14, 32, 0)));

        CovariateExtractor extractor(
                cDefinitions,
                pVariates,
                DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                DATE_TIME_NO_VAR(2017, 8, 29, 14, 32, 0));
        ASSERT_FALSE(TestCovariateExtractor::get_m_hasBirthDate(&extractor));
        ASSERT_EQ(TestCovariateExtractor::get_m_pvValued(&extractor).count("AgeDays"), static_cast<size_t>(0));
        ASSERT_EQ(TestCovariateExtractor::get_m_pvValued(&extractor).count("AgeMonths"), static_cast<size_t>(0));
        ASSERT_EQ(TestCovariateExtractor::get_m_pvValued(&extractor).count("AgeYears"), static_cast<size_t>(0));
    }

    // Check that the definition of the age (expressed in days) is consistent (1).
    {
        CovariateDefinitions cDefinitions;
        PatientVariates pVariates;

        ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
        ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_NO_R(AgeDays, 20, AgeInDays, Double, Direct, cDefinitions);

        ASSERT_NO_THROW(CovariateExtractor(
                cDefinitions,
                PatientVariates(),
                DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                DATE_TIME_NO_VAR(2017, 8, 19, 14, 32, 0)));

        CovariateExtractor extractor(
                cDefinitions,
                pVariates,
                DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                DATE_TIME_NO_VAR(2017, 8, 29, 14, 32, 0));
        ASSERT_FALSE(TestCovariateExtractor::get_m_hasBirthDate(&extractor));
        ASSERT_EQ(TestCovariateExtractor::get_m_pvValued(&extractor).count("AgeDays"), static_cast<size_t>(0));
        ASSERT_EQ(TestCovariateExtractor::get_m_pvValued(&extractor).count("AgeMonths"), static_cast<size_t>(0));
        ASSERT_EQ(TestCovariateExtractor::get_m_pvValued(&extractor).count("AgeYears"), static_cast<size_t>(0));
        ASSERT_EQ(TestCovariateExtractor::get_m_cdValued(&extractor).count("AgeDays"), static_cast<size_t>(1));
        ASSERT_EQ(TestCovariateExtractor::get_m_cdValued(&extractor).count("AgeMonths"), static_cast<size_t>(0));
        ASSERT_EQ(TestCovariateExtractor::get_m_cdValued(&extractor).count("AgeYears"), static_cast<size_t>(0));
        ASSERT_DOUBLE_EQ(TestCovariateExtractor::get_m_initAgeInDays(&extractor), 20);
        ASSERT_DOUBLE_EQ(TestCovariateExtractor::get_m_initAgeInMonths(&extractor), -1);
        ASSERT_DOUBLE_EQ(TestCovariateExtractor::get_m_initAgeInYears(&extractor), -1);
    }

    // Check that the definition of the age (expressed in days) is consistent (2).
    {
        CovariateDefinitions cDefinitions;
        PatientVariates pVariates;

        ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
        ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_NO_R(AgeDays, -20, AgeInDays, Double, Direct, cDefinitions);

        ASSERT_THROW(
                CovariateExtractor(
                        cDefinitions,
                        PatientVariates(),
                        DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                        DATE_TIME_NO_VAR(2017, 8, 19, 14, 32, 0)),
                std::runtime_error);
    }

    // Check that the definition of the age (expressed in days) is consistent (3).
    {
        CovariateDefinitions cDefinitions;
        PatientVariates pVariates;

        ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
        ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_NO_R(AgeDays, 20, AgeInDays, Double, Direct, cDefinitions);
        ADD_CDEF_NO_R(AgeDays2, 20, AgeInDays, Double, Direct, cDefinitions);

        ASSERT_THROW(
                CovariateExtractor(
                        cDefinitions,
                        PatientVariates(),
                        DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                        DATE_TIME_NO_VAR(2017, 8, 19, 14, 32, 0)),
                std::runtime_error);
    }

    // Check that the definition of the age (expressed in months) is consistent (1).
    {
        CovariateDefinitions cDefinitions;
        PatientVariates pVariates;

        ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
        ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_NO_R(AgeMonths, 20, AgeInMonths, Double, Direct, cDefinitions);

        ASSERT_NO_THROW(CovariateExtractor(
                cDefinitions,
                PatientVariates(),
                DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                DATE_TIME_NO_VAR(2017, 8, 19, 14, 32, 0)));

        CovariateExtractor extractor(
                cDefinitions,
                pVariates,
                DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                DATE_TIME_NO_VAR(2017, 8, 29, 14, 32, 0));
        ASSERT_FALSE(TestCovariateExtractor::get_m_hasBirthDate(&extractor));
        ASSERT_EQ(TestCovariateExtractor::get_m_pvValued(&extractor).count("AgeDays"), static_cast<size_t>(0));
        ASSERT_EQ(TestCovariateExtractor::get_m_pvValued(&extractor).count("AgeMonths"), static_cast<size_t>(0));
        ASSERT_EQ(TestCovariateExtractor::get_m_pvValued(&extractor).count("AgeYears"), static_cast<size_t>(0));
        ASSERT_EQ(TestCovariateExtractor::get_m_cdValued(&extractor).count("AgeDays"), static_cast<size_t>(0));
        ASSERT_EQ(TestCovariateExtractor::get_m_cdValued(&extractor).count("AgeMonths"), static_cast<size_t>(1));
        ASSERT_EQ(TestCovariateExtractor::get_m_cdValued(&extractor).count("AgeYears"), static_cast<size_t>(0));
        ASSERT_DOUBLE_EQ(TestCovariateExtractor::get_m_initAgeInDays(&extractor), -1);
        ASSERT_DOUBLE_EQ(TestCovariateExtractor::get_m_initAgeInMonths(&extractor), 20);
        ASSERT_DOUBLE_EQ(TestCovariateExtractor::get_m_initAgeInYears(&extractor), -1);
    }

    // Check that the definition of the age (expressed in months) is consistent (2).
    {
        CovariateDefinitions cDefinitions;
        PatientVariates pVariates;

        ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
        ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_NO_R(AgeMonths, -20, AgeInMonths, Double, Direct, cDefinitions);

        ASSERT_THROW(
                CovariateExtractor(
                        cDefinitions,
                        PatientVariates(),
                        DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                        DATE_TIME_NO_VAR(2017, 8, 19, 14, 32, 0)),
                std::runtime_error);
    }

    // Check that the definition of the age (expressed in months) is consistent (3).
    {
        CovariateDefinitions cDefinitions;
        PatientVariates pVariates;

        ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
        ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_NO_R(AgeMonths, 20, AgeInMonths, Double, Direct, cDefinitions);
        ADD_CDEF_NO_R(AgeMonths2, 20, AgeInMonths, Double, Direct, cDefinitions);

        ASSERT_THROW(
                CovariateExtractor(
                        cDefinitions,
                        PatientVariates(),
                        DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                        DATE_TIME_NO_VAR(2017, 8, 19, 14, 32, 0)),
                std::runtime_error);
    }

    // Check that the definition of the age (expressed in years) is consistent (1).
    {
        CovariateDefinitions cDefinitions;
        PatientVariates pVariates;

        ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
        ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_NO_R(AgeYears, 20, AgeInYears, Double, Direct, cDefinitions);

        ASSERT_NO_THROW(CovariateExtractor(
                cDefinitions,
                PatientVariates(),
                DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                DATE_TIME_NO_VAR(2017, 8, 19, 14, 32, 0)));

        CovariateExtractor extractor(
                cDefinitions,
                pVariates,
                DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                DATE_TIME_NO_VAR(2017, 8, 29, 14, 32, 0));
        ASSERT_FALSE(TestCovariateExtractor::get_m_hasBirthDate(&extractor));
        ASSERT_EQ(TestCovariateExtractor::get_m_pvValued(&extractor).count("AgeDays"), static_cast<size_t>(0));
        ASSERT_EQ(TestCovariateExtractor::get_m_pvValued(&extractor).count("AgeMonths"), static_cast<size_t>(0));
        ASSERT_EQ(TestCovariateExtractor::get_m_pvValued(&extractor).count("AgeYears"), static_cast<size_t>(0));
        ASSERT_EQ(TestCovariateExtractor::get_m_cdValued(&extractor).count("AgeDays"), static_cast<size_t>(0));
        ASSERT_EQ(TestCovariateExtractor::get_m_cdValued(&extractor).count("AgeMonths"), static_cast<size_t>(0));
        ASSERT_EQ(TestCovariateExtractor::get_m_cdValued(&extractor).count("AgeYears"), static_cast<size_t>(1));
        ASSERT_DOUBLE_EQ(TestCovariateExtractor::get_m_initAgeInDays(&extractor), -1);
        ASSERT_DOUBLE_EQ(TestCovariateExtractor::get_m_initAgeInMonths(&extractor), -1);
        ASSERT_DOUBLE_EQ(TestCovariateExtractor::get_m_initAgeInYears(&extractor), 20);
    }

    // Check that the definition of the age (expressed in years) is consistent (2).
    {
        CovariateDefinitions cDefinitions;
        PatientVariates pVariates;

        ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
        ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_NO_R(AgeYears, -20, AgeInYears, Double, Direct, cDefinitions);

        ASSERT_THROW(
                CovariateExtractor(
                        cDefinitions,
                        PatientVariates(),
                        DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                        DATE_TIME_NO_VAR(2017, 8, 19, 14, 32, 0)),
                std::runtime_error);
    }

    // Check that the definition of the age (expressed in years) is consistent (3).
    {
        CovariateDefinitions cDefinitions;
        PatientVariates pVariates;

        ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
        ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_NO_R(AgeYears, 20, AgeInYears, Double, Direct, cDefinitions);
        ADD_CDEF_NO_R(AgeYears2, 20, AgeInYears, Double, Direct, cDefinitions);

        ASSERT_THROW(
                CovariateExtractor(
                        cDefinitions,
                        PatientVariates(),
                        DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                        DATE_TIME_NO_VAR(2017, 8, 19, 14, 32, 0)),
                std::runtime_error);
    }

    // Check that the definition of the time from the start of the treatment
    // (expressed in hours) is consistent (1).
    {
        CovariateDefinitions cDefinitions;
        PatientVariates pVariates;

        ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
        ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_NO_R(AgeDays, 20, AgeInDays, Double, Direct, cDefinitions);
        ADD_CDEF_NO_R(TimeFromStartInHours, 18, TimeFromStartInHours, Double, Direct, cDefinitions);

        ASSERT_NO_THROW(CovariateExtractor(
                cDefinitions,
                PatientVariates(),
                DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                DATE_TIME_NO_VAR(2017, 8, 19, 14, 32, 0)));

        CovariateExtractor extractor(
                cDefinitions,
                pVariates,
                DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                DATE_TIME_NO_VAR(2017, 8, 29, 14, 32, 0));

        ASSERT_EQ(
                TestCovariateExtractor::get_m_pvValued(&extractor).count("TimeFromStartInHours"),
                static_cast<size_t>(0));
        ASSERT_EQ(
                TestCovariateExtractor::get_m_pvValued(&extractor).count("TimeFromStartInDays"),
                static_cast<size_t>(0));
        ASSERT_EQ(
                TestCovariateExtractor::get_m_pvValued(&extractor).count("TimeFromStartInWeeks"),
                static_cast<size_t>(0));
        ASSERT_EQ(
                TestCovariateExtractor::get_m_pvValued(&extractor).count("TimeFromStartInMonths"),
                static_cast<size_t>(0));
        ASSERT_EQ(
                TestCovariateExtractor::get_m_pvValued(&extractor).count("TimeFromStartInYears"),
                static_cast<size_t>(0));

        ASSERT_EQ(
                TestCovariateExtractor::get_m_cdValued(&extractor).count("TimeFromStartInHours"),
                static_cast<size_t>(1));
        ASSERT_EQ(
                TestCovariateExtractor::get_m_cdValued(&extractor).count("TimeFromStartInDays"),
                static_cast<size_t>(0));
        ASSERT_EQ(
                TestCovariateExtractor::get_m_cdValued(&extractor).count("TimeFromStartInWeeks"),
                static_cast<size_t>(0));
        ASSERT_EQ(
                TestCovariateExtractor::get_m_cdValued(&extractor).count("TimeFromStartInMonths"),
                static_cast<size_t>(0));
        ASSERT_EQ(
                TestCovariateExtractor::get_m_cdValued(&extractor).count("TimeFromStartInYears"),
                static_cast<size_t>(0));

        ASSERT_DOUBLE_EQ(TestCovariateExtractor::get_m_initTimeFromStartInHours(&extractor), 18);
        ASSERT_DOUBLE_EQ(TestCovariateExtractor::get_m_initTimeFromStartInDays(&extractor), -1);
        ASSERT_DOUBLE_EQ(TestCovariateExtractor::get_m_initTimeFromStartInWeeks(&extractor), -1);
        ASSERT_DOUBLE_EQ(TestCovariateExtractor::get_m_initTimeFromStartInMonths(&extractor), -1);
        ASSERT_DOUBLE_EQ(TestCovariateExtractor::get_m_initTimeFromStartInYears(&extractor), -1);
    }

    // Check that the definition of the time from the start of the treatment
    // (expressed in hours) is consistent (2).
    {
        CovariateDefinitions cDefinitions;
        PatientVariates pVariates;

        ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
        ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_NO_R(AgeDays, 20, AgeInDays, Double, Direct, cDefinitions);
        ADD_CDEF_NO_R(TimeFromStartInHours, -18, TimeFromStartInHours, Double, Direct, cDefinitions);

        ASSERT_THROW(
                CovariateExtractor(
                        cDefinitions,
                        PatientVariates(),
                        DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                        DATE_TIME_NO_VAR(2017, 8, 19, 14, 32, 0)),
                std::runtime_error);
    }

    // Check that the definition of the time from the start of the treatment
    // (expressed in hours) is consistent (3).
    {
        CovariateDefinitions cDefinitions;
        PatientVariates pVariates;

        ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
        ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_NO_R(AgeDays, 20, AgeInDays, Double, Direct, cDefinitions);
        ADD_CDEF_NO_R(TimeFromStartInHours, 18, TimeFromStartInHours, Double, Direct, cDefinitions);
        ADD_CDEF_NO_R(TimeFromStartInHours2, 18, TimeFromStartInHours, Double, Direct, cDefinitions);

        ASSERT_THROW(
                CovariateExtractor(
                        cDefinitions,
                        PatientVariates(),
                        DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                        DATE_TIME_NO_VAR(2017, 8, 19, 14, 32, 0)),
                std::runtime_error);
    }

    // Check that the definition of the time from the start of the treatment
    // (expressed in days) is consistent (1).
    {
        CovariateDefinitions cDefinitions;
        PatientVariates pVariates;

        ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
        ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_NO_R(AgeDays, 20, AgeInDays, Double, Direct, cDefinitions);
        ADD_CDEF_NO_R(TimeFromStartInDays, 18, TimeFromStartInDays, Double, Direct, cDefinitions);

        ASSERT_NO_THROW(CovariateExtractor(
                cDefinitions,
                PatientVariates(),
                DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                DATE_TIME_NO_VAR(2017, 8, 19, 14, 32, 0)));

        CovariateExtractor extractor(
                cDefinitions,
                pVariates,
                DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                DATE_TIME_NO_VAR(2017, 8, 29, 14, 32, 0));

        ASSERT_EQ(
                TestCovariateExtractor::get_m_pvValued(&extractor).count("TimeFromStartInHours"),
                static_cast<size_t>(0));
        ASSERT_EQ(
                TestCovariateExtractor::get_m_pvValued(&extractor).count("TimeFromStartInDays"),
                static_cast<size_t>(0));
        ASSERT_EQ(
                TestCovariateExtractor::get_m_pvValued(&extractor).count("TimeFromStartInWeeks"),
                static_cast<size_t>(0));
        ASSERT_EQ(
                TestCovariateExtractor::get_m_pvValued(&extractor).count("TimeFromStartInMonths"),
                static_cast<size_t>(0));
        ASSERT_EQ(
                TestCovariateExtractor::get_m_pvValued(&extractor).count("TimeFromStartInYears"),
                static_cast<size_t>(0));

        ASSERT_EQ(
                TestCovariateExtractor::get_m_cdValued(&extractor).count("TimeFromStartInHours"),
                static_cast<size_t>(0));
        ASSERT_EQ(
                TestCovariateExtractor::get_m_cdValued(&extractor).count("TimeFromStartInDays"),
                static_cast<size_t>(1));
        ASSERT_EQ(
                TestCovariateExtractor::get_m_cdValued(&extractor).count("TimeFromStartInWeeks"),
                static_cast<size_t>(0));
        ASSERT_EQ(
                TestCovariateExtractor::get_m_cdValued(&extractor).count("TimeFromStartInMonths"),
                static_cast<size_t>(0));
        ASSERT_EQ(
                TestCovariateExtractor::get_m_cdValued(&extractor).count("TimeFromStartInYears"),
                static_cast<size_t>(0));

        ASSERT_DOUBLE_EQ(TestCovariateExtractor::get_m_initTimeFromStartInHours(&extractor), -1);
        ASSERT_DOUBLE_EQ(TestCovariateExtractor::get_m_initTimeFromStartInDays(&extractor), 18);
        ASSERT_DOUBLE_EQ(TestCovariateExtractor::get_m_initTimeFromStartInWeeks(&extractor), -1);
        ASSERT_DOUBLE_EQ(TestCovariateExtractor::get_m_initTimeFromStartInMonths(&extractor), -1);
        ASSERT_DOUBLE_EQ(TestCovariateExtractor::get_m_initTimeFromStartInYears(&extractor), -1);
    }

    // Check that the definition of the time from the start of the treatment
    // (expressed in days) is consistent (2).
    {
        CovariateDefinitions cDefinitions;
        PatientVariates pVariates;

        ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
        ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_NO_R(AgeDays, 20, AgeInDays, Double, Direct, cDefinitions);
        ADD_CDEF_NO_R(TimeFromStartInDays, -18, TimeFromStartInDays, Double, Direct, cDefinitions);

        ASSERT_THROW(
                CovariateExtractor(
                        cDefinitions,
                        PatientVariates(),
                        DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                        DATE_TIME_NO_VAR(2017, 8, 19, 14, 32, 0)),
                std::runtime_error);
    }

    // Check that the definition of the time from the start of the treatment
    // (expressed in days) is consistent (3).
    {
        CovariateDefinitions cDefinitions;
        PatientVariates pVariates;

        ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
        ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_NO_R(AgeDays, 20, AgeInDays, Double, Direct, cDefinitions);
        ADD_CDEF_NO_R(TimeFromStartInDays, 18, TimeFromStartInDays, Double, Direct, cDefinitions);
        ADD_CDEF_NO_R(TimeFromStartInDays2, 18, TimeFromStartInDays, Double, Direct, cDefinitions);

        ASSERT_THROW(
                CovariateExtractor(
                        cDefinitions,
                        PatientVariates(),
                        DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                        DATE_TIME_NO_VAR(2017, 8, 19, 14, 32, 0)),
                std::runtime_error);
    }

    // Check that the definition of the time from the start of the treatment
    // (expressed in weeks) is consistent (1).
    {
        CovariateDefinitions cDefinitions;
        PatientVariates pVariates;

        ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
        ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_NO_R(AgeDays, 20, AgeInDays, Double, Direct, cDefinitions);
        ADD_CDEF_NO_R(TimeFromStartInWeeks, 18, TimeFromStartInWeeks, Double, Direct, cDefinitions);

        ASSERT_NO_THROW(CovariateExtractor(
                cDefinitions,
                PatientVariates(),
                DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                DATE_TIME_NO_VAR(2017, 8, 19, 14, 32, 0)));

        CovariateExtractor extractor(
                cDefinitions,
                pVariates,
                DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                DATE_TIME_NO_VAR(2017, 8, 29, 14, 32, 0));

        ASSERT_EQ(
                TestCovariateExtractor::get_m_pvValued(&extractor).count("TimeFromStartInHours"),
                static_cast<size_t>(0));
        ASSERT_EQ(
                TestCovariateExtractor::get_m_pvValued(&extractor).count("TimeFromStartInDays"),
                static_cast<size_t>(0));
        ASSERT_EQ(
                TestCovariateExtractor::get_m_pvValued(&extractor).count("TimeFromStartInWeeks"),
                static_cast<size_t>(0));
        ASSERT_EQ(
                TestCovariateExtractor::get_m_pvValued(&extractor).count("TimeFromStartInMonths"),
                static_cast<size_t>(0));
        ASSERT_EQ(
                TestCovariateExtractor::get_m_pvValued(&extractor).count("TimeFromStartInYears"),
                static_cast<size_t>(0));

        ASSERT_EQ(
                TestCovariateExtractor::get_m_cdValued(&extractor).count("TimeFromStartInHours"),
                static_cast<size_t>(0));
        ASSERT_EQ(
                TestCovariateExtractor::get_m_cdValued(&extractor).count("TimeFromStartInDays"),
                static_cast<size_t>(0));
        ASSERT_EQ(
                TestCovariateExtractor::get_m_cdValued(&extractor).count("TimeFromStartInWeeks"),
                static_cast<size_t>(1));
        ASSERT_EQ(
                TestCovariateExtractor::get_m_cdValued(&extractor).count("TimeFromStartInMonths"),
                static_cast<size_t>(0));
        ASSERT_EQ(
                TestCovariateExtractor::get_m_cdValued(&extractor).count("TimeFromStartInYears"),
                static_cast<size_t>(0));

        ASSERT_DOUBLE_EQ(TestCovariateExtractor::get_m_initTimeFromStartInHours(&extractor), -1);
        ASSERT_DOUBLE_EQ(TestCovariateExtractor::get_m_initTimeFromStartInDays(&extractor), -1);
        ASSERT_DOUBLE_EQ(TestCovariateExtractor::get_m_initTimeFromStartInWeeks(&extractor), 18);
        ASSERT_DOUBLE_EQ(TestCovariateExtractor::get_m_initTimeFromStartInMonths(&extractor), -1);
        ASSERT_DOUBLE_EQ(TestCovariateExtractor::get_m_initTimeFromStartInYears(&extractor), -1);
    }

    // Check that the definition of the time from the start of the treatment
    // (expressed in weeks) is consistent (2).
    {
        CovariateDefinitions cDefinitions;
        PatientVariates pVariates;

        ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
        ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_NO_R(AgeDays, 20, AgeInDays, Double, Direct, cDefinitions);
        ADD_CDEF_NO_R(TimeFromStartInWeeks, -18, TimeFromStartInWeeks, Double, Direct, cDefinitions);

        ASSERT_THROW(
                CovariateExtractor(
                        cDefinitions,
                        PatientVariates(),
                        DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                        DATE_TIME_NO_VAR(2017, 8, 19, 14, 32, 0)),
                std::runtime_error);
    }

    // Check that the definition of the time from the start of the treatment
    // (expressed in weeks) is consistent (3).
    {
        CovariateDefinitions cDefinitions;
        PatientVariates pVariates;

        ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
        ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_NO_R(AgeDays, 20, AgeInDays, Double, Direct, cDefinitions);
        ADD_CDEF_NO_R(TimeFromStartInWeeks, 18, TimeFromStartInWeeks, Double, Direct, cDefinitions);
        ADD_CDEF_NO_R(TimeFromStartInWeeks2, 18, TimeFromStartInWeeks, Double, Direct, cDefinitions);

        ASSERT_THROW(
                CovariateExtractor(
                        cDefinitions,
                        PatientVariates(),
                        DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                        DATE_TIME_NO_VAR(2017, 8, 19, 14, 32, 0)),
                std::runtime_error);
    }

    // Check that the definition of the time from the start of the treatment
    // (expressed in months) is consistent (1).
    {
        CovariateDefinitions cDefinitions;
        PatientVariates pVariates;

        ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
        ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_NO_R(AgeDays, 20, AgeInDays, Double, Direct, cDefinitions);
        ADD_CDEF_NO_R(TimeFromStartInMonths, 18, TimeFromStartInMonths, Double, Direct, cDefinitions);

        ASSERT_NO_THROW(CovariateExtractor(
                cDefinitions,
                PatientVariates(),
                DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                DATE_TIME_NO_VAR(2017, 8, 19, 14, 32, 0)));

        CovariateExtractor extractor(
                cDefinitions,
                pVariates,
                DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                DATE_TIME_NO_VAR(2017, 8, 29, 14, 32, 0));

        ASSERT_EQ(
                TestCovariateExtractor::get_m_pvValued(&extractor).count("TimeFromStartInHours"),
                static_cast<size_t>(0));
        ASSERT_EQ(
                TestCovariateExtractor::get_m_pvValued(&extractor).count("TimeFromStartInDays"),
                static_cast<size_t>(0));
        ASSERT_EQ(
                TestCovariateExtractor::get_m_pvValued(&extractor).count("TimeFromStartInWeeks"),
                static_cast<size_t>(0));
        ASSERT_EQ(
                TestCovariateExtractor::get_m_pvValued(&extractor).count("TimeFromStartInMonths"),
                static_cast<size_t>(0));
        ASSERT_EQ(
                TestCovariateExtractor::get_m_pvValued(&extractor).count("TimeFromStartInYears"),
                static_cast<size_t>(0));

        ASSERT_EQ(
                TestCovariateExtractor::get_m_cdValued(&extractor).count("TimeFromStartInHours"),
                static_cast<size_t>(0));
        ASSERT_EQ(
                TestCovariateExtractor::get_m_cdValued(&extractor).count("TimeFromStartInDays"),
                static_cast<size_t>(0));
        ASSERT_EQ(
                TestCovariateExtractor::get_m_cdValued(&extractor).count("TimeFromStartInWeeks"),
                static_cast<size_t>(0));
        ASSERT_EQ(
                TestCovariateExtractor::get_m_cdValued(&extractor).count("TimeFromStartInMonths"),
                static_cast<size_t>(1));
        ASSERT_EQ(
                TestCovariateExtractor::get_m_cdValued(&extractor).count("TimeFromStartInYears"),
                static_cast<size_t>(0));

        ASSERT_DOUBLE_EQ(TestCovariateExtractor::get_m_initTimeFromStartInHours(&extractor), -1);
        ASSERT_DOUBLE_EQ(TestCovariateExtractor::get_m_initTimeFromStartInDays(&extractor), -1);
        ASSERT_DOUBLE_EQ(TestCovariateExtractor::get_m_initTimeFromStartInWeeks(&extractor), -1);
        ASSERT_DOUBLE_EQ(TestCovariateExtractor::get_m_initTimeFromStartInMonths(&extractor), 18);
        ASSERT_DOUBLE_EQ(TestCovariateExtractor::get_m_initTimeFromStartInYears(&extractor), -1);
    }

    // Check that the definition of the time from the start of the treatment
    // (expressed in months) is consistent (2).
    {
        CovariateDefinitions cDefinitions;
        PatientVariates pVariates;

        ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
        ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_NO_R(AgeDays, 20, AgeInDays, Double, Direct, cDefinitions);
        ADD_CDEF_NO_R(TimeFromStartInMonths, -18, TimeFromStartInMonths, Double, Direct, cDefinitions);

        ASSERT_THROW(
                CovariateExtractor(
                        cDefinitions,
                        PatientVariates(),
                        DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                        DATE_TIME_NO_VAR(2017, 8, 19, 14, 32, 0)),
                std::runtime_error);
    }

    // Check that the definition of the time from the start of the treatment
    // (expressed in months) is consistent (3).
    {
        CovariateDefinitions cDefinitions;
        PatientVariates pVariates;

        ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
        ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_NO_R(AgeDays, 20, AgeInDays, Double, Direct, cDefinitions);
        ADD_CDEF_NO_R(TimeFromStartInMonths, 18, TimeFromStartInMonths, Double, Direct, cDefinitions);
        ADD_CDEF_NO_R(TimeFromStartInMonths2, 18, TimeFromStartInMonths, Double, Direct, cDefinitions);

        ASSERT_THROW(
                CovariateExtractor(
                        cDefinitions,
                        PatientVariates(),
                        DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                        DATE_TIME_NO_VAR(2017, 8, 19, 14, 32, 0)),
                std::runtime_error);
    }

    // Check that the definition of the time from the start of the treatment
    // (expressed in years) is consistent (1).
    {
        CovariateDefinitions cDefinitions;
        PatientVariates pVariates;

        ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
        ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_NO_R(AgeDays, 20, AgeInDays, Double, Direct, cDefinitions);
        ADD_CDEF_NO_R(TimeFromStartInYears, 18, TimeFromStartInYears, Double, Direct, cDefinitions);

        ASSERT_NO_THROW(CovariateExtractor(
                cDefinitions,
                PatientVariates(),
                DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                DATE_TIME_NO_VAR(2017, 8, 19, 14, 32, 0)));

        CovariateExtractor extractor(
                cDefinitions,
                pVariates,
                DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                DATE_TIME_NO_VAR(2017, 8, 29, 14, 32, 0));

        ASSERT_EQ(
                TestCovariateExtractor::get_m_pvValued(&extractor).count("TimeFromStartInHours"),
                static_cast<size_t>(0));
        ASSERT_EQ(
                TestCovariateExtractor::get_m_pvValued(&extractor).count("TimeFromStartInDays"),
                static_cast<size_t>(0));
        ASSERT_EQ(
                TestCovariateExtractor::get_m_pvValued(&extractor).count("TimeFromStartInWeeks"),
                static_cast<size_t>(0));
        ASSERT_EQ(
                TestCovariateExtractor::get_m_pvValued(&extractor).count("TimeFromStartInMonths"),
                static_cast<size_t>(0));
        ASSERT_EQ(
                TestCovariateExtractor::get_m_pvValued(&extractor).count("TimeFromStartInYears"),
                static_cast<size_t>(0));

        ASSERT_EQ(
                TestCovariateExtractor::get_m_cdValued(&extractor).count("TimeFromStartInHours"),
                static_cast<size_t>(0));
        ASSERT_EQ(
                TestCovariateExtractor::get_m_cdValued(&extractor).count("TimeFromStartInDays"),
                static_cast<size_t>(0));
        ASSERT_EQ(
                TestCovariateExtractor::get_m_cdValued(&extractor).count("TimeFromStartInWeeks"),
                static_cast<size_t>(0));
        ASSERT_EQ(
                TestCovariateExtractor::get_m_cdValued(&extractor).count("TimeFromStartInMonths"),
                static_cast<size_t>(0));
        ASSERT_EQ(
                TestCovariateExtractor::get_m_cdValued(&extractor).count("TimeFromStartInYears"),
                static_cast<size_t>(1));

        ASSERT_DOUBLE_EQ(TestCovariateExtractor::get_m_initTimeFromStartInHours(&extractor), -1);
        ASSERT_DOUBLE_EQ(TestCovariateExtractor::get_m_initTimeFromStartInDays(&extractor), -1);
        ASSERT_DOUBLE_EQ(TestCovariateExtractor::get_m_initTimeFromStartInWeeks(&extractor), -1);
        ASSERT_DOUBLE_EQ(TestCovariateExtractor::get_m_initTimeFromStartInMonths(&extractor), -1);
        ASSERT_DOUBLE_EQ(TestCovariateExtractor::get_m_initTimeFromStartInYears(&extractor), 18);
    }

    // Check that the definition of the time from the start of the treatment
    // (expressed in years) is consistent (2).
    {
        CovariateDefinitions cDefinitions;
        PatientVariates pVariates;

        ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
        ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_NO_R(AgeDays, 20, AgeInDays, Double, Direct, cDefinitions);
        ADD_CDEF_NO_R(TimeFromStartInYears, -18, TimeFromStartInYears, Double, Direct, cDefinitions);

        ASSERT_THROW(
                CovariateExtractor(
                        cDefinitions,
                        PatientVariates(),
                        DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                        DATE_TIME_NO_VAR(2017, 8, 19, 14, 32, 0)),
                std::runtime_error);
    }

    // Check that the definition of the time from the start of the treatment
    // (expressed in years) is consistent (3).
    {
        CovariateDefinitions cDefinitions;
        PatientVariates pVariates;

        ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
        ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_NO_R(AgeDays, 20, AgeInDays, Double, Direct, cDefinitions);
        ADD_CDEF_NO_R(TimeFromStartInYears, 18, TimeFromStartInYears, Double, Direct, cDefinitions);
        ADD_CDEF_NO_R(TimeFromStartInYears2, 18, TimeFromStartInYears, Double, Direct, cDefinitions);

        ASSERT_THROW(
                CovariateExtractor(
                        cDefinitions,
                        PatientVariates(),
                        DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                        DATE_TIME_NO_VAR(2017, 8, 19, 14, 32, 0)),
                std::runtime_error);
    }

    // Build a covariate extractor from a set of covariate definitions and patient variates.
    {
        CovariateDefinitions cDefinitions;
        PatientVariates pVariates;

        ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
        ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);
        ADD_PV_NO_UNIT(Gist, true, Bool, DATE_TIME_NO_VAR(2017, 8, 19, 12, 32, 0), pVariates);
        ADD_PV_NO_UNIT(Gist, false, Bool, DATE_TIME_NO_VAR(2017, 8, 19, 22, 32, 0), pVariates);

        ASSERT_NO_THROW(CovariateExtractor(
                cDefinitions,
                pVariates,
                DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                DATE_TIME_NO_VAR(2017, 8, 29, 14, 32, 0)));
    }

    /*
    // Add a birth date with the appropriate type.
    {
    CovariateDefinitions cDefinitions;
    PatientVariates pVariates;

    ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
    ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
    ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);
    ADD_CDEF_NO_R_STR(CovariateExtractor::sm_birthDateCName, DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0), Standard,
    Date, Direct, cDefinitions);

    ADD_PV_NO_UNIT(Gist, true, Bool, DATE_TIME_NO_VAR(2017, 8, 19, 12, 32, 0), pVariates);
    ADD_PV_NO_UNIT(Gist, false, Bool, DATE_TIME_NO_VAR(2017, 8, 19, 22, 32, 0), pVariates);

    ASSERT_NO_THROW(CovariateExtractor(cDefinitions, pVariates,
    DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
    DATE_TIME_NO_VAR(2017, 8, 29, 14, 32, 0)));

    CovariateExtractor extractor(cDefinitions, pVariates,
    DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
    DATE_TIME_NO_VAR(2017, 8, 29, 14, 32, 0));
    ASSERT_EQ(TestCovariateExtractor::get_m_birthDate(&extractor), DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0));
    ASSERT_TRUE(TestCovariateExtractor::get_m_hasBirthDate(&extractor));
    ASSERT_EQ(TestCovariateExtractor::get_m_pvValued(&extractor).count(CovariateExtractor::sm_birthDateCName), static_cast<size_t>(0));
    }
    */
    /*
    // Check that a reasonable birth date is given.
    {
    CovariateDefinitions cDefinitions;
    PatientVariates pVariates;

    ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
    ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
    ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);
    ADD_CDEF_NO_R_STR(CovariateExtractor::sm_birthDateCName, DATE_TIME_NO_VAR(2017, 8, 19, 14, 32, 0), Standard,
    Date, Direct, cDefinitions);


    ASSERT_THROW(CovariateExtractor(cDefinitions, PatientVariates(),
    DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
    DATE_TIME_NO_VAR(2017, 8, 29, 14, 32, 0)),
    std::runtime_error);
    }
    */

    // Add twice the same covariate definition (with different values).
    {
        CovariateDefinitions cDefinitions;

        ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
        ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_W_R_UNIT(Weight, 3500, Standard, Bool, Linear, Tucuxi::Common::days(2), "mg", cDefinitions);

        ASSERT_THROW(
                CovariateExtractor(
                        cDefinitions,
                        PatientVariates(),
                        DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                        DATE_TIME_NO_VAR(2017, 8, 19, 14, 32, 0)),
                std::runtime_error);
    }


    // // Added a birth date, but not with date type.
    // {
    // CovariateDefinitions cDefinitions;
    //
    // ADD_CDEF_NO_R_STR(CovariateExtractor::sm_birthDateCName, false, Standard, Bool, Direct, cDefinitions);
    //
    // ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
    // ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);
    //
    // ASSERT_THROW(CovariateExtractor(cDefinitions, PatientVariates(),
    // DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
    // DATE_TIME_NO_VAR(2017, 8, 19, 14, 32, 0)),
    // std::runtime_error);
    // }

    // Try build a covariate extractor from a set of covariate definitions with a null pointer included.
    {
        CovariateDefinitions cDefinitions;

        ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
        cDefinitions.push_back(nullptr);
        ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);

        ASSERT_THROW(
                CovariateExtractor(
                        cDefinitions,
                        PatientVariates(),
                        DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
                        DATE_TIME_NO_VAR(2017, 8, 19, 14, 32, 0)),
                std::runtime_error);
    }

    // Try build a covariate extractor from a set of patient variates with a null pointer included.
    {
        CovariateDefinitions cDefinitions;
        PatientVariates pVariates;

        ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
        ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);
        ADD_PV_NO_UNIT(Gist, true, Bool, DATE_TIME_NO_VAR(2017, 8, 19, 12, 32, 0), pVariates);
        pVariates.push_back(nullptr);
        ADD_PV_NO_UNIT(Gist, false, Bool, DATE_TIME_NO_VAR(2017, 8, 19, 22, 32, 0), pVariates);

        ASSERT_THROW(
                CovariateExtractor(
                        cDefinitions,
                        pVariates,
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
        ASSERT_EQ(v1, varToValue(t1));
        Value v2 = stringToValue(varToString(t2), DataType::Date);
        ASSERT_EQ(v2, varToValue(t2));

        ASSERT_EQ(dateDiffInDays(t1, t2), 10);
        ASSERT_EQ(dateDiffInMonths(t1, t2), 0);
        ASSERT_EQ(dateDiffInYears(t1, t2), 0);

        ASSERT_EQ(dateDiffInDays(t1, t3), 406);
        ASSERT_EQ(dateDiffInMonths(t1, t3), 13);
        ASSERT_EQ(dateDiffInYears(t1, t3), 1);

        ASSERT_EQ(dateDiffInDays(t1, t4), 42261);
        ASSERT_EQ(dateDiffInMonths(t1, t4), 1388);
        ASSERT_EQ(dateDiffInYears(t1, t4), 115);

        ASSERT_EQ(dateDiffInDays(t1, t5), 21);
        ASSERT_EQ(dateDiffInMonths(t1, t5), 0);
        ASSERT_EQ(dateDiffInYears(t1, t5), 0);
    }
}

TEST(Core_TestCovariateExtractor, CE_collectRefreshIntervals)
{
    // Sanity check on TimeDate/Duration functions that are used by collectRefreshIntervals().
    {
        DATE_TIME_VAR(t1, 2017, 8, 17, 14, 0, 0);

        DateTime tDayAfter = t1;
        tDayAfter.addDays(1);
        ASSERT_EQ(tDayAfter, DATE_TIME_NO_VAR(2017, 8, 18, 14, 0, 0));

        DateTime t10DaysAfter = t1;
        t10DaysAfter.addDays(10);
        ASSERT_EQ(t10DaysAfter, DATE_TIME_NO_VAR(2017, 8, 27, 14, 0, 0));

        DateTime t20DaysAfter = t1;
        t20DaysAfter.addDays(20);
        ASSERT_EQ(t20DaysAfter, DATE_TIME_NO_VAR(2017, 9, 6, 14, 0, 0));

        DateTime tMonthAfter = t1;
        tMonthAfter.addMonths(1);
        ASSERT_EQ(tMonthAfter, DATE_TIME_NO_VAR(2017, 9, 17, 14, 0, 0));

        DateTime t14MonthsAfter = t1;
        t14MonthsAfter.addMonths(14);
        ASSERT_EQ(t14MonthsAfter, DATE_TIME_NO_VAR(2018, 10, 17, 14, 0, 0));

        DateTime tYearAfter = t1;
        tYearAfter.addYears(1);
        ASSERT_EQ(tYearAfter, DATE_TIME_NO_VAR(2018, 8, 17, 14, 0, 0));

        DateTime t14YearsAfter = t1;
        t14YearsAfter.addYears(14);
        ASSERT_EQ(t14YearsAfter, DATE_TIME_NO_VAR(2031, 8, 17, 14, 0, 0));

        // Now check conversions.
        ASSERT_EQ(varToString(DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0)), "2017-08-18 14:32:00");
        ASSERT_EQ(
                ValueToDate(stringToValue(varToString(DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0)), DataType::Date)),
                DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0));
    }

    // Check that refresh times for covariates with no patient variates are respected.
    {
        CovariateDefinitions cDefinitions;
        PatientVariates pVariates;

        ADD_CDEF_W_R(A1, 1.0, Standard, Double, Linear, Tucuxi::Common::days(1), cDefinitions);
        ADD_CDEF_W_R(B2, 2.0, Standard, Double, Linear, Tucuxi::Common::days(2), cDefinitions);
        ADD_CDEF_W_R(C3, 3.0, Standard, Double, Linear, Tucuxi::Common::days(3), cDefinitions);

        CovariateExtractor extractor(
                cDefinitions,
                pVariates,
                DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0),
                DATE_TIME_NO_VAR(2017, 8, 23, 14, 0, 0));

        TestCovariateExtractor::test_sortPatientVariates(&extractor);

        std::map<DateTime, std::vector<std::string>> refreshMap;
        TestCovariateExtractor::test_collectRefreshIntervals(refreshMap, &extractor);

        ASSERT_EQ(refreshMap.size(), static_cast<size_t>(7));
        // At each date we are supposed to have the three of them, since an update of one will have to trigger the
        // update of the others.
        for (unsigned int i = 17; i <= 23; ++i) {
            ASSERT_EQ(refreshMap[DATE_TIME_NO_VAR(2017, 8, i, 14, 0, 0)].size(), static_cast<size_t>(3));
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

        CovariateExtractor extractor(
                cDefinitions,
                pVariates,
                DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0),
                DATE_TIME_NO_VAR(2017, 8, 23, 14, 0, 0));
        TestCovariateExtractor::test_sortPatientVariates(&extractor);

        std::map<DateTime, std::vector<std::string>> refreshMap;
        TestCovariateExtractor::test_collectRefreshIntervals(refreshMap, &extractor);

        ASSERT_EQ(refreshMap.size(), static_cast<size_t>(7));
        // At each date we are supposed to have the three of them, since an update of one will have to trigger the
        // update of the others.
        for (unsigned int i = 17; i <= 23; ++i) {
            ASSERT_EQ(refreshMap[DATE_TIME_NO_VAR(2017, 8, i, 14, 0, 0)].size(), static_cast<size_t>(3));
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

        CovariateExtractor extractor(
                cDefinitions,
                pVariates,
                DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0),
                DATE_TIME_NO_VAR(2017, 8, 23, 14, 0, 0));
        TestCovariateExtractor::test_sortPatientVariates(&extractor);

        std::map<DateTime, std::vector<std::string>> refreshMap;
        TestCovariateExtractor::test_collectRefreshIntervals(refreshMap, &extractor);

        ASSERT_EQ(refreshMap.size(), static_cast<size_t>(7));
        // At each date we are supposed to have the three of them, since an update of one will have to trigger the
        // update of the others.
        for (unsigned int i = 17; i <= 23; ++i) {
            ASSERT_EQ(refreshMap[DATE_TIME_NO_VAR(2017, 8, i, 14, 0, 0)].size(), static_cast<size_t>(3));
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

        CovariateExtractor extractor(
                cDefinitions,
                pVariates,
                DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0),
                DATE_TIME_NO_VAR(2017, 8, 23, 14, 0, 0));
        TestCovariateExtractor::test_sortPatientVariates(&extractor);

        std::map<DateTime, std::vector<std::string>> refreshMap;
        TestCovariateExtractor::test_collectRefreshIntervals(refreshMap, &extractor);

        ASSERT_EQ(refreshMap.size(), static_cast<size_t>(9));
        // At each date we are supposed to have the three of them, since an update of one will have to trigger the
        // update of the others.
        for (unsigned int i = 17; i <= 23; ++i) {
            ASSERT_EQ(refreshMap[DATE_TIME_NO_VAR(2017, 8, i, 14, 0, 0)].size(), static_cast<size_t>(8));
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
        ASSERT_EQ(refreshMap[DATE_TIME_NO_VAR(2017, 8, 21, 12, 32, 0)].size(), static_cast<size_t>(8));
        CHECK_REFRESH(A1, DATE_TIME_NO_VAR(2017, 8, 21, 12, 32, 0), refreshMap);
        CHECK_REFRESH(B2, DATE_TIME_NO_VAR(2017, 8, 21, 12, 32, 0), refreshMap);
        CHECK_REFRESH(C3, DATE_TIME_NO_VAR(2017, 8, 21, 12, 32, 0), refreshMap);
        CHECK_REFRESH(D4, DATE_TIME_NO_VAR(2017, 8, 21, 12, 32, 0), refreshMap);
        CHECK_REFRESH(E5, DATE_TIME_NO_VAR(2017, 8, 21, 12, 32, 0), refreshMap);
        CHECK_REFRESH(F6, DATE_TIME_NO_VAR(2017, 8, 21, 12, 32, 0), refreshMap);
        CHECK_REFRESH(G7, DATE_TIME_NO_VAR(2017, 8, 21, 12, 32, 0), refreshMap);
        CHECK_REFRESH(H8, DATE_TIME_NO_VAR(2017, 8, 21, 12, 32, 0), refreshMap);

        // 23.08.2017 @ 12h32
        ASSERT_EQ(refreshMap[DATE_TIME_NO_VAR(2017, 8, 23, 12, 32, 0)].size(), static_cast<size_t>(8));
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

        CovariateExtractor extractor(
                cDefinitions,
                pVariates,
                DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0),
                DATE_TIME_NO_VAR(2018, 9, 23, 14, 0, 0));
        TestCovariateExtractor::test_sortPatientVariates(&extractor);

        std::map<DateTime, std::vector<std::string>> refreshMap;
        TestCovariateExtractor::test_collectRefreshIntervals(refreshMap, &extractor);

        ASSERT_EQ(refreshMap.size(), static_cast<size_t>(403));
        // 17.08.2017
        ASSERT_EQ(refreshMap[DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0)].size(), static_cast<size_t>(3));
        CHECK_REFRESH(AgeDays, DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0), refreshMap);
        CHECK_REFRESH(AgeMonths, DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0), refreshMap);
        CHECK_REFRESH(AgeYears, DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0), refreshMap);

        // 18.08-16.09.2017
        for (unsigned int i = 18; i <= 31; ++i) {
            ASSERT_EQ(refreshMap[DATE_TIME_NO_VAR(2017, 8, i, 14, 0, 0)].size(), static_cast<size_t>(1));
            CHECK_REFRESH(AgeDays, DATE_TIME_NO_VAR(2017, 8, i, 14, 0, 0), refreshMap);
        }
        for (unsigned int i = 1; i <= 16; ++i) {
            ASSERT_EQ(refreshMap[DATE_TIME_NO_VAR(2017, 9, i, 14, 0, 0)].size(), static_cast<size_t>(1));
            CHECK_REFRESH(AgeDays, DATE_TIME_NO_VAR(2017, 9, i, 14, 0, 0), refreshMap);
        }

        // 17.09.2017
        ASSERT_EQ(refreshMap[DATE_TIME_NO_VAR(2017, 9, 17, 14, 0, 0)].size(), static_cast<size_t>(2));
        CHECK_REFRESH(AgeDays, DATE_TIME_NO_VAR(2017, 9, 17, 14, 0, 0), refreshMap);
        CHECK_REFRESH(AgeMonths, DATE_TIME_NO_VAR(2017, 9, 17, 14, 0, 0), refreshMap);

        // 17.08.2018
        ASSERT_EQ(refreshMap[DATE_TIME_NO_VAR(2018, 8, 17, 14, 0, 0)].size(), static_cast<size_t>(3));
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


        ADD_PV_NO_UNIT(
                birthdate,
                DATE_TIME_NO_VAR(1995, 12, 13, 14, 15, 0),
                Date,
                DATE_TIME_NO_VAR(1994, 12, 13, 14, 15, 0),
                pVariates);

        CovariateExtractor extractor(
                cDefinitions,
                pVariates,
                DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0),
                DATE_TIME_NO_VAR(2018, 9, 23, 14, 0, 0));
        TestCovariateExtractor::test_sortPatientVariates(&extractor);

        std::map<DateTime, std::vector<std::string>> refreshMap;
        TestCovariateExtractor::test_collectRefreshIntervals(refreshMap, &extractor);

        ASSERT_EQ(refreshMap.size(), static_cast<size_t>(403));
        // 17.08.2017 @ 14h00 (start)
        ASSERT_EQ(refreshMap[DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0)].size(), static_cast<size_t>(3));
        CHECK_REFRESH(AgeDays, DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0), refreshMap);
        CHECK_REFRESH(AgeMonths, DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0), refreshMap);
        CHECK_REFRESH(AgeYears, DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0), refreshMap);
        // 17.08.2017 @ 14h15
        ASSERT_EQ(refreshMap[DATE_TIME_NO_VAR(2017, 8, 17, 14, 15, 0)].size(), static_cast<size_t>(1));
        CHECK_REFRESH(AgeDays, DATE_TIME_NO_VAR(2017, 8, 17, 14, 15, 0), refreshMap);

        // 13.09.2017
        ASSERT_EQ(refreshMap[DATE_TIME_NO_VAR(2017, 9, 13, 14, 15, 0)].size(), static_cast<size_t>(2));
        CHECK_REFRESH(AgeDays, DATE_TIME_NO_VAR(2017, 9, 13, 14, 15, 0), refreshMap);
        CHECK_REFRESH(AgeMonths, DATE_TIME_NO_VAR(2017, 9, 13, 14, 15, 0), refreshMap);

        // 13.12.2017
        ASSERT_EQ(refreshMap.count(DATE_TIME_NO_VAR(2017, 12, 13, 14, 0, 0)), static_cast<size_t>(0));
        ASSERT_EQ(refreshMap[DATE_TIME_NO_VAR(2017, 12, 13, 14, 15, 0)].size(), static_cast<size_t>(3));
        CHECK_REFRESH(AgeDays, DATE_TIME_NO_VAR(2017, 12, 13, 14, 15, 0), refreshMap);
        CHECK_REFRESH(AgeMonths, DATE_TIME_NO_VAR(2017, 12, 13, 14, 15, 0), refreshMap);
        CHECK_REFRESH(AgeYears, DATE_TIME_NO_VAR(2017, 12, 13, 14, 15, 0), refreshMap);
    }

    // Check the refresh times for the time from the start of treatment when no
    // treatmentStartDate is specified.
    {
        CovariateDefinitions cDefinitions;
        PatientVariates pVariates;

        ADD_CDEF_NO_R(TimeFromStartHours, 10, TimeFromStartInHours, Double, Direct, cDefinitions);
        ADD_CDEF_NO_R(TimeFromStartDays, 15, TimeFromStartInDays, Double, Direct, cDefinitions);
        ADD_CDEF_NO_R(TimeFromStartWeeks, 20, TimeFromStartInWeeks, Double, Direct, cDefinitions);
        ADD_CDEF_NO_R(TimeFromStartMonths, 4, TimeFromStartInMonths, Double, Direct, cDefinitions);
        ADD_CDEF_NO_R(TimeFromStartYears, 1, TimeFromStartInYears, Double, Direct, cDefinitions);

        CovariateExtractor extractor(
                cDefinitions,
                pVariates,
                DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0),
                DATE_TIME_NO_VAR(2017, 8, 21, 14, 0, 0));
        TestCovariateExtractor::test_sortPatientVariates(&extractor);

        std::map<DateTime, std::vector<std::string>> refreshMap;
        TestCovariateExtractor::test_collectRefreshIntervals(refreshMap, &extractor);

        // 4 x 24 hours + 1 (start) = 97
        // 5 days (has to include start)
        // 1 week (start)
        // 1 month (start)
        // 1 year (start)
        // -> however, the hours event subsumes the remaing ones, so the end
        //    size will be dictated by the number of hours events.
        ASSERT_EQ(refreshMap.size(), static_cast<size_t>(97));

        // 17.08.2017 @ 14h00 (start)
        ASSERT_EQ(refreshMap[DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0)].size(), static_cast<size_t>(5));
        CHECK_REFRESH(TimeFromStartHours, DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0), refreshMap);
        CHECK_REFRESH(TimeFromStartDays, DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0), refreshMap);
        CHECK_REFRESH(TimeFromStartWeeks, DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0), refreshMap);
        CHECK_REFRESH(TimeFromStartMonths, DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0), refreshMap);
        CHECK_REFRESH(TimeFromStartYears, DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0), refreshMap);

        // 17.08.2017 15h00 - 18.08.2017 13h00
        for (unsigned int i = 15; i <= 23; ++i) {
            ASSERT_EQ(refreshMap[DATE_TIME_NO_VAR(2017, 8, 17, i, 0, 0)].size(), static_cast<size_t>(1));
            CHECK_REFRESH(TimeFromStartHours, DATE_TIME_NO_VAR(2017, 8, 17, i, 0, 0), refreshMap);
        }
        for (unsigned int i = 0; i <= 13; ++i) {
            ASSERT_EQ(refreshMap[DATE_TIME_NO_VAR(2017, 8, 18, i, 0, 0)].size(), static_cast<size_t>(1));
            CHECK_REFRESH(TimeFromStartHours, DATE_TIME_NO_VAR(2017, 8, 18, i, 0, 0), refreshMap);
        }

        // 18.08-21.08.2017
        for (unsigned int i = 18; i <= 21; ++i) {
            ASSERT_EQ(refreshMap[DATE_TIME_NO_VAR(2017, 8, i, 14, 0, 0)].size(), static_cast<size_t>(2));
            CHECK_REFRESH(TimeFromStartHours, DATE_TIME_NO_VAR(2017, 8, i, 14, 0, 0), refreshMap);
            CHECK_REFRESH(TimeFromStartDays, DATE_TIME_NO_VAR(2017, 8, i, 14, 0, 0), refreshMap);
        }
    }

    // Check the refresh times for the time from the start of treatment when no
    // treatmentStartDate is specified.
    {
        CovariateDefinitions cDefinitions;
        PatientVariates pVariates;

        ADD_CDEF_NO_R(TimeFromStartHours, 10, TimeFromStartInHours, Double, Direct, cDefinitions);
        ADD_CDEF_NO_R(TimeFromStartDays, 15, TimeFromStartInDays, Double, Direct, cDefinitions);
        ADD_CDEF_NO_R(TimeFromStartWeeks, 20, TimeFromStartInWeeks, Double, Direct, cDefinitions);
        ADD_CDEF_NO_R(TimeFromStartMonths, 4, TimeFromStartInMonths, Double, Direct, cDefinitions);
        ADD_CDEF_NO_R(TimeFromStartYears, 1, TimeFromStartInYears, Double, Direct, cDefinitions);

        CovariateExtractor extractor(
                cDefinitions,
                pVariates,
                DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0),
                DATE_TIME_NO_VAR(2018, 9, 23, 14, 0, 0));
        TestCovariateExtractor::test_sortPatientVariates(&extractor);

        std::map<DateTime, std::vector<std::string>> refreshMap;
        TestCovariateExtractor::test_collectRefreshIntervals(refreshMap, &extractor);
        // for (auto &ri : refreshMap) {
        //     std::cerr << ri.first << "\n";
        //     for (auto &rh : ri.second) {
        //         std::cerr << "\t" << rh << "\n";
        //     }
        // }

        // 402 days x 24 hours + the end hour.
        ASSERT_EQ(refreshMap.size(), static_cast<size_t>(402 * 24 + 1));

        // 17.08.2017 @ 14h00 (start)
        ASSERT_EQ(refreshMap[DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0)].size(), static_cast<size_t>(5));
        CHECK_REFRESH(TimeFromStartHours, DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0), refreshMap);
        CHECK_REFRESH(TimeFromStartDays, DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0), refreshMap);
        CHECK_REFRESH(TimeFromStartWeeks, DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0), refreshMap);
        CHECK_REFRESH(TimeFromStartMonths, DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0), refreshMap);
        CHECK_REFRESH(TimeFromStartYears, DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0), refreshMap);

        // 17.08.2017 15h00 - 18.08.2017 13h00
        for (unsigned int i = 15; i <= 23; ++i) {
            ASSERT_EQ(refreshMap[DATE_TIME_NO_VAR(2017, 8, 17, i, 0, 0)].size(), static_cast<size_t>(1));
            CHECK_REFRESH(TimeFromStartHours, DATE_TIME_NO_VAR(2017, 8, 17, i, 0, 0), refreshMap);
        }
        for (unsigned int i = 0; i <= 13; ++i) {
            ASSERT_EQ(refreshMap[DATE_TIME_NO_VAR(2017, 8, 18, i, 0, 0)].size(), static_cast<size_t>(1));
            CHECK_REFRESH(TimeFromStartHours, DATE_TIME_NO_VAR(2017, 8, 18, i, 0, 0), refreshMap);
        }

        // 18.08-23.08.2017
        for (unsigned int i = 18; i <= 23; ++i) {
            ASSERT_EQ(refreshMap[DATE_TIME_NO_VAR(2017, 8, i, 14, 0, 0)].size(), static_cast<size_t>(2));
            CHECK_REFRESH(TimeFromStartHours, DATE_TIME_NO_VAR(2017, 8, i, 14, 0, 0), refreshMap);
            CHECK_REFRESH(TimeFromStartDays, DATE_TIME_NO_VAR(2017, 8, i, 14, 0, 0), refreshMap);
        }

        // 24.08.2017 14h00
        ASSERT_EQ(refreshMap[DATE_TIME_NO_VAR(2017, 8, 24, 14, 0, 0)].size(), static_cast<size_t>(3));
        CHECK_REFRESH(TimeFromStartHours, DATE_TIME_NO_VAR(2017, 8, 24, 14, 0, 0), refreshMap);
        CHECK_REFRESH(TimeFromStartDays, DATE_TIME_NO_VAR(2017, 8, 24, 14, 0, 0), refreshMap);
        CHECK_REFRESH(TimeFromStartWeeks, DATE_TIME_NO_VAR(2017, 8, 24, 14, 0, 0), refreshMap);

        // 24.08.2017 15h00 - 24.08.2017 13h00
        for (unsigned int i = 15; i <= 23; ++i) {
            ASSERT_EQ(refreshMap[DATE_TIME_NO_VAR(2017, 8, 24, i, 0, 0)].size(), static_cast<size_t>(1));
            CHECK_REFRESH(TimeFromStartHours, DATE_TIME_NO_VAR(2017, 8, 24, i, 0, 0), refreshMap);
        }

        // 14.09.2017 14h00
        ASSERT_EQ(refreshMap[DATE_TIME_NO_VAR(2017, 9, 14, 14, 0, 0)].size(), static_cast<size_t>(3));
        CHECK_REFRESH(TimeFromStartHours, DATE_TIME_NO_VAR(2017, 9, 14, 14, 0, 0), refreshMap);
        CHECK_REFRESH(TimeFromStartDays, DATE_TIME_NO_VAR(2017, 9, 14, 14, 0, 0), refreshMap);
        CHECK_REFRESH(TimeFromStartWeeks, DATE_TIME_NO_VAR(2017, 9, 14, 14, 0, 0), refreshMap);

        // 17.08.2018 14h00
        ASSERT_EQ(refreshMap[DATE_TIME_NO_VAR(2018, 8, 17, 14, 0, 0)].size(), static_cast<size_t>(4));
        CHECK_REFRESH(TimeFromStartHours, DATE_TIME_NO_VAR(2018, 8, 17, 14, 0, 0), refreshMap);
        CHECK_REFRESH(TimeFromStartDays, DATE_TIME_NO_VAR(2018, 8, 17, 14, 0, 0), refreshMap);
        CHECK_REFRESH(TimeFromStartMonths, DATE_TIME_NO_VAR(2018, 8, 17, 14, 0, 0), refreshMap);
        CHECK_REFRESH(TimeFromStartYears, DATE_TIME_NO_VAR(2018, 8, 17, 14, 0, 0), refreshMap);
    }

    // Check the refresh times for the time from the start of treatment when a
    // treatmentStartDate is specified.
    {
        CovariateDefinitions cDefinitions;
        PatientVariates pVariates;

        ADD_CDEF_NO_R(TimeFromStartHours, 10, TimeFromStartInHours, Double, Direct, cDefinitions);
        ADD_CDEF_NO_R(TimeFromStartDays, 15, TimeFromStartInDays, Double, Direct, cDefinitions);
        ADD_CDEF_NO_R(TimeFromStartWeeks, 20, TimeFromStartInWeeks, Double, Direct, cDefinitions);
        ADD_CDEF_NO_R(TimeFromStartMonths, 4, TimeFromStartInMonths, Double, Direct, cDefinitions);
        ADD_CDEF_NO_R(TimeFromStartYears, 1, TimeFromStartInYears, Double, Direct, cDefinitions);

        DosageHistory dosage_history = TestCovariateExtractor::get_dosage_history(
                DATE_TIME_NO_VAR(2017, 8, 11, 14, 0, 0), DATE_TIME_NO_VAR(2017, 8, 14, 14, 0, 0));

        CovariateExtractor extractor(
                cDefinitions,
                pVariates,
                DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0),
                DATE_TIME_NO_VAR(2017, 8, 21, 14, 0, 0));
        TestCovariateExtractor::test_sortPatientVariates(&extractor);

        std::map<DateTime, std::vector<std::string>> refreshMap;
        TestCovariateExtractor::test_collectRefreshIntervals(refreshMap, &extractor);

        // 4 x 24 hours + 1 (start) = 97
        // 5 days (has to include start)
        // 1 week (start)
        // 1 month (start)
        // 1 year (start)
        // -> however, the hours event subsumes the remaing ones, so the end
        //    size will be dictated by the number of hours events.
        ASSERT_EQ(refreshMap.size(), static_cast<size_t>(97));

        // 17.08.2017 @ 14h00 (start)
        ASSERT_EQ(refreshMap[DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0)].size(), static_cast<size_t>(5));
        CHECK_REFRESH(TimeFromStartHours, DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0), refreshMap);
        CHECK_REFRESH(TimeFromStartDays, DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0), refreshMap);
        CHECK_REFRESH(TimeFromStartWeeks, DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0), refreshMap);
        CHECK_REFRESH(TimeFromStartMonths, DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0), refreshMap);
        CHECK_REFRESH(TimeFromStartYears, DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0), refreshMap);

        // 17.08.2017 15h00 - 18.08.2017 13h00
        for (unsigned int i = 15; i <= 23; ++i) {
            ASSERT_EQ(refreshMap[DATE_TIME_NO_VAR(2017, 8, 17, i, 0, 0)].size(), static_cast<size_t>(1));
            CHECK_REFRESH(TimeFromStartHours, DATE_TIME_NO_VAR(2017, 8, 17, i, 0, 0), refreshMap);
        }
        for (unsigned int i = 0; i <= 13; ++i) {
            ASSERT_EQ(refreshMap[DATE_TIME_NO_VAR(2017, 8, 18, i, 0, 0)].size(), static_cast<size_t>(1));
            CHECK_REFRESH(TimeFromStartHours, DATE_TIME_NO_VAR(2017, 8, 18, i, 0, 0), refreshMap);
        }

        // 18.08-21.08.2017
        for (unsigned int i = 18; i <= 21; ++i) {
            ASSERT_EQ(refreshMap[DATE_TIME_NO_VAR(2017, 8, i, 14, 0, 0)].size(), static_cast<size_t>(2));
            CHECK_REFRESH(TimeFromStartHours, DATE_TIME_NO_VAR(2017, 8, i, 14, 0, 0), refreshMap);
            CHECK_REFRESH(TimeFromStartDays, DATE_TIME_NO_VAR(2017, 8, i, 14, 0, 0), refreshMap);
        }
    }

    // Check the refresh times for the time from the start of treatment when a
    // treatmentStartDate is specified.
    {
        CovariateDefinitions cDefinitions;
        PatientVariates pVariates;

        ADD_CDEF_NO_R(TimeFromStartHours, 10, TimeFromStartInHours, Double, Direct, cDefinitions);
        ADD_CDEF_NO_R(TimeFromStartDays, 15, TimeFromStartInDays, Double, Direct, cDefinitions);
        ADD_CDEF_NO_R(TimeFromStartWeeks, 20, TimeFromStartInWeeks, Double, Direct, cDefinitions);
        ADD_CDEF_NO_R(TimeFromStartMonths, 4, TimeFromStartInMonths, Double, Direct, cDefinitions);
        ADD_CDEF_NO_R(TimeFromStartYears, 1, TimeFromStartInYears, Double, Direct, cDefinitions);

        DosageHistory dosage_history = TestCovariateExtractor::get_dosage_history(
                DATE_TIME_NO_VAR(2017, 8, 11, 14, 33, 0), DATE_TIME_NO_VAR(2017, 8, 14, 14, 33, 0));

        CovariateExtractor extractor(
                cDefinitions,
                pVariates,
                DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0),
                DATE_TIME_NO_VAR(2017, 8, 21, 14, 0, 0),
                dosage_history);
        TestCovariateExtractor::test_sortPatientVariates(&extractor);

        std::map<DateTime, std::vector<std::string>> refreshMap;
        TestCovariateExtractor::test_collectRefreshIntervals(refreshMap, &extractor);

        // 4 x 24 hours + 1 (start) = 97
        // 5 days (has to include start)
        // 1 week (start)
        // 1 month (start)
        // 1 year (start)
        // -> however, the hours event subsumes the remaing ones, so the end
        //    size will be dictated by the number of hours events.
        ASSERT_EQ(refreshMap.size(), static_cast<size_t>(97));

        // 17.08.2017 @ 14h00 (start)
        ASSERT_EQ(refreshMap[DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0)].size(), static_cast<size_t>(5));
        CHECK_REFRESH(TimeFromStartHours, DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0), refreshMap);
        CHECK_REFRESH(TimeFromStartDays, DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0), refreshMap);
        CHECK_REFRESH(TimeFromStartWeeks, DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0), refreshMap);
        CHECK_REFRESH(TimeFromStartMonths, DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0), refreshMap);
        CHECK_REFRESH(TimeFromStartYears, DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0), refreshMap);

        // 17.08.2017 @ 14h33 (start)
        ASSERT_EQ(refreshMap[DATE_TIME_NO_VAR(2017, 8, 17, 14, 33, 0)].size(), static_cast<size_t>(2));
        CHECK_REFRESH(TimeFromStartHours, DATE_TIME_NO_VAR(2017, 8, 17, 14, 33, 0), refreshMap);
        CHECK_REFRESH(TimeFromStartDays, DATE_TIME_NO_VAR(2017, 8, 17, 14, 33, 0), refreshMap);

        // 17.08.2017 15h33 - 18.08.2017 13h33
        for (unsigned int i = 15; i <= 23; ++i) {
            ASSERT_EQ(refreshMap[DATE_TIME_NO_VAR(2017, 8, 17, i, 33, 0)].size(), static_cast<size_t>(1));
            CHECK_REFRESH(TimeFromStartHours, DATE_TIME_NO_VAR(2017, 8, 17, i, 33, 0), refreshMap);
        }
        for (unsigned int i = 0; i <= 13; ++i) {
            ASSERT_EQ(refreshMap[DATE_TIME_NO_VAR(2017, 8, 18, i, 33, 0)].size(), static_cast<size_t>(1));
            CHECK_REFRESH(TimeFromStartHours, DATE_TIME_NO_VAR(2017, 8, 18, i, 33, 0), refreshMap);
        }

        ASSERT_EQ(refreshMap[DATE_TIME_NO_VAR(2017, 8, 18, 14, 33, 0)].size(), static_cast<size_t>(3));
        CHECK_REFRESH(TimeFromStartHours, DATE_TIME_NO_VAR(2017, 8, 18, 14, 33, 0), refreshMap);
        CHECK_REFRESH(TimeFromStartDays, DATE_TIME_NO_VAR(2017, 8, 18, 14, 33, 0), refreshMap);
        CHECK_REFRESH(TimeFromStartWeeks, DATE_TIME_NO_VAR(2017, 8, 18, 14, 33, 0), refreshMap);

        // 19.08-21.08.2017
        for (unsigned int i = 19; i < 21; ++i) {
            ASSERT_EQ(refreshMap[DATE_TIME_NO_VAR(2017, 8, i, 14, 33, 0)].size(), static_cast<size_t>(2));
            CHECK_REFRESH(TimeFromStartHours, DATE_TIME_NO_VAR(2017, 8, i, 14, 33, 0), refreshMap);
            CHECK_REFRESH(TimeFromStartDays, DATE_TIME_NO_VAR(2017, 8, i, 14, 33, 0), refreshMap);
        }

        ASSERT_EQ(refreshMap[DATE_TIME_NO_VAR(2017, 8, 21, 14, 33, 0)].size(), static_cast<size_t>(0));
    }
}

TEST(Core_TestCovariateExtractor, CE_interpolateValues)
{
    CovariateExtractor extractor(
            CovariateDefinitions(),
            PatientVariates(),
            DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0),
            DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0));
    bool rc;
    Value valRes;

    // Violate precondition.

    rc = TestCovariateExtractor::test_interpolateValues(
            0,
            DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0),
            1,
            DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0),
            DATE_TIME_NO_VAR(2017, 8, 15, 14, 32, 0),
            InterpolationType::Linear,
            valRes,
            &extractor);
    ASSERT_FALSE(rc);

    // Interpolate in the middle.
    rc = TestCovariateExtractor::test_interpolateValues(
            0,
            DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0),
            10,
            DATE_TIME_NO_VAR(2017, 8, 23, 14, 32, 0),
            DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
            InterpolationType::Linear,
            valRes,
            &extractor);
    ASSERT_TRUE(rc);
    ASSERT_DOUBLE_EQ(valRes, 5.0);

    // Extrapolate before.
    rc = TestCovariateExtractor::test_interpolateValues(
            5,
            DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
            10,
            DATE_TIME_NO_VAR(2017, 8, 23, 14, 32, 0),
            DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0),
            InterpolationType::Linear,
            valRes,
            &extractor);
    ASSERT_TRUE(rc);
    ASSERT_DOUBLE_EQ(valRes, 0.0);

    // Extrapolate after.
    rc = TestCovariateExtractor::test_interpolateValues(
            5,
            DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0),
            10,
            DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
            DATE_TIME_NO_VAR(2017, 8, 23, 14, 32, 0),
            InterpolationType::Linear,
            valRes,
            &extractor);
    ASSERT_TRUE(rc);
    ASSERT_DOUBLE_EQ(valRes, 15.0);

    // Test direct interpolation (before _date1).
    rc = TestCovariateExtractor::test_interpolateValues(
            5,
            DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
            10,
            DATE_TIME_NO_VAR(2017, 8, 19, 14, 32, 0),
            DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0),
            InterpolationType::Direct,
            valRes,
            &extractor);
    ASSERT_TRUE(rc);
    ASSERT_DOUBLE_EQ(valRes, 5.0);

    // Test direct interpolation (between _date1 and _date2).
    rc = TestCovariateExtractor::test_interpolateValues(
            5,
            DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
            10,
            DATE_TIME_NO_VAR(2017, 8, 29, 14, 32, 0),
            DATE_TIME_NO_VAR(2017, 8, 19, 14, 32, 0),
            InterpolationType::Direct,
            valRes,
            &extractor);
    ASSERT_TRUE(rc);
    ASSERT_DOUBLE_EQ(valRes, 5.0);

    // Test direct interpolation (after _date2).
    rc = TestCovariateExtractor::test_interpolateValues(
            5,
            DATE_TIME_NO_VAR(2017, 8, 18, 14, 32, 0),
            10,
            DATE_TIME_NO_VAR(2017, 8, 19, 14, 32, 0),
            DATE_TIME_NO_VAR(2017, 8, 23, 14, 32, 0),
            InterpolationType::Direct,
            valRes,
            &extractor);
    ASSERT_TRUE(rc);
    ASSERT_DOUBLE_EQ(valRes, 10.0);
}

TEST(Core_TestCovariateExtractor, CE_sortPatientVariates)
{
    // The weight measurement the 11.08 and the one at 16h30 on the 30.08 should disappear. The remaining ones
    // should be sorted.
    {
        CovariateDefinitions cDefinitions;
        PatientVariates pVariates;

        ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_W_R_UNIT(Weight, 6.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
        ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);
        ADD_PV_NO_UNIT(Weight, 6.3, Double, DATE_TIME_NO_VAR(2017, 8, 19, 22, 32, 0), pVariates);
        ADD_PV_NO_UNIT(Weight, 7.2, Double, DATE_TIME_NO_VAR(2017, 8, 19, 12, 32, 0), pVariates);
        ADD_PV_NO_UNIT(Weight, 7.4, Double, DATE_TIME_NO_VAR(2017, 8, 21, 12, 32, 0), pVariates);
        ADD_PV_NO_UNIT(Weight, 7.6, Double, DATE_TIME_NO_VAR(2017, 8, 12, 12, 32, 0), pVariates);
        ADD_PV_NO_UNIT(Weight, 7.9, Double, DATE_TIME_NO_VAR(2017, 8, 11, 12, 32, 0), pVariates);
        ADD_PV_NO_UNIT(Weight, 7.0, Double, DATE_TIME_NO_VAR(2017, 8, 30, 12, 32, 0), pVariates);
        ADD_PV_NO_UNIT(Weight, 7.8, Double, DATE_TIME_NO_VAR(2017, 8, 30, 16, 32, 0), pVariates);

        CovariateExtractor extractor(
                cDefinitions,
                pVariates,
                DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0),
                DATE_TIME_NO_VAR(2017, 8, 29, 14, 0, 0));
        TestCovariateExtractor::test_sortPatientVariates(&extractor);

        std::vector<std::unique_ptr<PatientCovariate>> res_pvVec;
        res_pvVec.push_back(std::make_unique<PatientCovariate>(
                "Weight", varToString(7.6), DataType::Double, TucuUnit(), DATE_TIME_NO_VAR(2017, 8, 12, 12, 32, 0)));
        res_pvVec.push_back(std::make_unique<PatientCovariate>(
                "Weight", varToString(7.2), DataType::Double, TucuUnit(), DATE_TIME_NO_VAR(2017, 8, 19, 12, 32, 0)));
        res_pvVec.push_back(std::make_unique<PatientCovariate>(
                "Weight", varToString(6.3), DataType::Double, TucuUnit(), DATE_TIME_NO_VAR(2017, 8, 19, 22, 32, 0)));
        res_pvVec.push_back(std::make_unique<PatientCovariate>(
                "Weight", varToString(7.4), DataType::Double, TucuUnit(), DATE_TIME_NO_VAR(2017, 8, 21, 12, 32, 0)));
        res_pvVec.push_back(std::make_unique<PatientCovariate>(
                "Weight", varToString(7.0), DataType::Double, TucuUnit(), DATE_TIME_NO_VAR(2017, 8, 30, 12, 32, 0)));

        const auto pvVals = TestCovariateExtractor::get_m_pvValued(&extractor).at("Weight");

        ASSERT_EQ(pvVals.size(), res_pvVec.size());
        for (size_t i = 0; i < std::min(pvVals.size(), res_pvVec.size()); ++i) {
            ASSERT_EQ(**(pvVals[i]), *(res_pvVec[i]));
        }
    }

    // No patient variates -- the sort just has to be nice and smile.
    {
        CovariateDefinitions cDefinitions;
        PatientVariates pVariates;

        ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_W_R_UNIT(Weight, 6.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
        ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);

        CovariateExtractor extractor(
                cDefinitions,
                pVariates,
                DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0),
                DATE_TIME_NO_VAR(2017, 8, 29, 14, 0, 0));
        ASSERT_NO_THROW(TestCovariateExtractor::test_sortPatientVariates(&extractor));
    }

    // We should keep all the measurements, properly sorted.
    {
        CovariateDefinitions cDefinitions;
        PatientVariates pVariates;

        ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_W_R_UNIT(Weight, 6.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
        ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);
        ADD_PV_NO_UNIT(Weight, 6.3, Double, DATE_TIME_NO_VAR(2017, 8, 19, 22, 32, 0), pVariates);
        ADD_PV_NO_UNIT(Weight, 7.2, Double, DATE_TIME_NO_VAR(2017, 8, 19, 12, 32, 0), pVariates);
        ADD_PV_NO_UNIT(Weight, 7.4, Double, DATE_TIME_NO_VAR(2017, 8, 21, 12, 32, 0), pVariates);
        ADD_PV_NO_UNIT(Weight, 7.6, Double, DATE_TIME_NO_VAR(2017, 8, 12, 12, 32, 0), pVariates);
        ADD_PV_NO_UNIT(Weight, 7.9, Double, DATE_TIME_NO_VAR(2017, 8, 11, 12, 32, 0), pVariates);
        ADD_PV_NO_UNIT(Weight, 7.0, Double, DATE_TIME_NO_VAR(2017, 8, 30, 12, 32, 0), pVariates);
        ADD_PV_NO_UNIT(Weight, 7.8, Double, DATE_TIME_NO_VAR(2017, 8, 30, 16, 32, 0), pVariates);

        CovariateExtractor extractor(
                cDefinitions,
                pVariates,
                DATE_TIME_NO_VAR(2017, 8, 12, 12, 0, 0),
                DATE_TIME_NO_VAR(2017, 8, 30, 14, 0, 0));
        TestCovariateExtractor::test_sortPatientVariates(&extractor);

        std::vector<std::unique_ptr<PatientCovariate>> res_pvVec;
        res_pvVec.push_back(std::make_unique<PatientCovariate>(
                "Weight", varToString(7.9), DataType::Double, TucuUnit(), DATE_TIME_NO_VAR(2017, 8, 11, 12, 32, 0)));
        res_pvVec.push_back(std::make_unique<PatientCovariate>(
                "Weight", varToString(7.6), DataType::Double, TucuUnit(), DATE_TIME_NO_VAR(2017, 8, 12, 12, 32, 0)));
        res_pvVec.push_back(std::make_unique<PatientCovariate>(
                "Weight", varToString(7.2), DataType::Double, TucuUnit(), DATE_TIME_NO_VAR(2017, 8, 19, 12, 32, 0)));
        res_pvVec.push_back(std::make_unique<PatientCovariate>(
                "Weight", varToString(6.3), DataType::Double, TucuUnit(), DATE_TIME_NO_VAR(2017, 8, 19, 22, 32, 0)));
        res_pvVec.push_back(std::make_unique<PatientCovariate>(
                "Weight", varToString(7.4), DataType::Double, TucuUnit(), DATE_TIME_NO_VAR(2017, 8, 21, 12, 32, 0)));
        res_pvVec.push_back(std::make_unique<PatientCovariate>(
                "Weight", varToString(7.0), DataType::Double, TucuUnit(), DATE_TIME_NO_VAR(2017, 8, 30, 12, 32, 0)));
        res_pvVec.push_back(std::make_unique<PatientCovariate>(
                "Weight", varToString(7.8), DataType::Double, TucuUnit(), DATE_TIME_NO_VAR(2017, 8, 30, 16, 32, 0)));

        const auto pvVals = TestCovariateExtractor::get_m_pvValued(&extractor).at("Weight");

        ASSERT_EQ(pvVals.size(), res_pvVec.size());
        for (size_t i = 0; i < std::min(pvVals.size(), res_pvVec.size()); ++i) {
            ASSERT_EQ(**(pvVals[i]), *(res_pvVec[i]));
        }
    }

    // We should keep all the measurements, properly sorted.
    {
        CovariateDefinitions cDefinitions;
        PatientVariates pVariates;

        ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_W_R_UNIT(Weight, 6.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
        ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);
        ADD_PV_NO_UNIT(Weight, 6.3, Double, DATE_TIME_NO_VAR(2017, 8, 19, 22, 32, 0), pVariates);
        ADD_PV_NO_UNIT(Weight, 7.2, Double, DATE_TIME_NO_VAR(2017, 8, 19, 12, 32, 0), pVariates);
        ADD_PV_NO_UNIT(Weight, 7.4, Double, DATE_TIME_NO_VAR(2017, 8, 21, 12, 32, 0), pVariates);
        ADD_PV_NO_UNIT(Weight, 7.6, Double, DATE_TIME_NO_VAR(2017, 8, 12, 12, 32, 0), pVariates);
        ADD_PV_NO_UNIT(Weight, 7.9, Double, DATE_TIME_NO_VAR(2017, 8, 11, 12, 32, 0), pVariates);
        ADD_PV_NO_UNIT(Weight, 7.0, Double, DATE_TIME_NO_VAR(2017, 8, 30, 12, 32, 0), pVariates);
        ADD_PV_NO_UNIT(Weight, 7.8, Double, DATE_TIME_NO_VAR(2017, 8, 30, 16, 32, 0), pVariates);

        CovariateExtractor extractor(
                cDefinitions,
                pVariates,
                DATE_TIME_NO_VAR(2017, 8, 10, 12, 0, 0),
                DATE_TIME_NO_VAR(2017, 8, 31, 14, 0, 0));
        TestCovariateExtractor::test_sortPatientVariates(&extractor);

        std::vector<std::unique_ptr<PatientCovariate>> res_pvVec;
        res_pvVec.push_back(std::make_unique<PatientCovariate>(
                "Weight", varToString(7.9), DataType::Double, TucuUnit(), DATE_TIME_NO_VAR(2017, 8, 11, 12, 32, 0)));
        res_pvVec.push_back(std::make_unique<PatientCovariate>(
                "Weight", varToString(7.6), DataType::Double, TucuUnit(), DATE_TIME_NO_VAR(2017, 8, 12, 12, 32, 0)));
        res_pvVec.push_back(std::make_unique<PatientCovariate>(
                "Weight", varToString(7.2), DataType::Double, TucuUnit(), DATE_TIME_NO_VAR(2017, 8, 19, 12, 32, 0)));
        res_pvVec.push_back(std::make_unique<PatientCovariate>(
                "Weight", varToString(6.3), DataType::Double, TucuUnit(), DATE_TIME_NO_VAR(2017, 8, 19, 22, 32, 0)));
        res_pvVec.push_back(std::make_unique<PatientCovariate>(
                "Weight", varToString(7.4), DataType::Double, TucuUnit(), DATE_TIME_NO_VAR(2017, 8, 21, 12, 32, 0)));
        res_pvVec.push_back(std::make_unique<PatientCovariate>(
                "Weight", varToString(7.0), DataType::Double, TucuUnit(), DATE_TIME_NO_VAR(2017, 8, 30, 12, 32, 0)));
        res_pvVec.push_back(std::make_unique<PatientCovariate>(
                "Weight", varToString(7.8), DataType::Double, TucuUnit(), DATE_TIME_NO_VAR(2017, 8, 30, 16, 32, 0)));

        const auto pvVals = TestCovariateExtractor::get_m_pvValued(&extractor).at("Weight");

        ASSERT_EQ(pvVals.size(), res_pvVec.size());
        for (size_t i = 0; i < std::min(pvVals.size(), res_pvVec.size()); ++i) {
            ASSERT_EQ(**(pvVals[i]), *(res_pvVec[i]));
        }
    }

    // One measure before m_start and one after m_end. We should keep both to interpolate.
    {
        CovariateDefinitions cDefinitions;
        PatientVariates pVariates;

        ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_W_R_UNIT(Weight, 6.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
        ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);
        ADD_PV_NO_UNIT(Weight, 6.3, Double, DATE_TIME_NO_VAR(2017, 8, 9, 22, 32, 0), pVariates);
        ADD_PV_NO_UNIT(Weight, 7.2, Double, DATE_TIME_NO_VAR(2017, 8, 12, 12, 32, 0), pVariates);

        CovariateExtractor extractor(
                cDefinitions,
                pVariates,
                DATE_TIME_NO_VAR(2017, 8, 10, 12, 0, 0),
                DATE_TIME_NO_VAR(2017, 8, 11, 14, 0, 0));
        TestCovariateExtractor::test_sortPatientVariates(&extractor);

        std::vector<std::unique_ptr<PatientCovariate>> res_pvVec;
        res_pvVec.push_back(std::make_unique<PatientCovariate>(
                "Weight", varToString(6.3), DataType::Double, TucuUnit(), DATE_TIME_NO_VAR(2017, 8, 9, 22, 32, 0)));
        res_pvVec.push_back(std::make_unique<PatientCovariate>(
                "Weight", varToString(7.2), DataType::Double, TucuUnit(), DATE_TIME_NO_VAR(2017, 8, 12, 12, 32, 0)));

        const auto pvVals = TestCovariateExtractor::get_m_pvValued(&extractor).at("Weight");

        ASSERT_EQ(pvVals.size(), res_pvVec.size());
        for (size_t i = 0; i < std::min(pvVals.size(), res_pvVec.size()); ++i) {
            ASSERT_EQ(**(pvVals[i]), *(res_pvVec[i]));
        }
    }

    // We should drop all measurements except those of the 19.08.
    {
        CovariateDefinitions cDefinitions;
        PatientVariates pVariates;

        ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_W_R_UNIT(Weight, 6.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
        ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);
        ADD_PV_NO_UNIT(Weight, 6.3, Double, DATE_TIME_NO_VAR(2017, 8, 19, 22, 32, 0), pVariates);
        ADD_PV_NO_UNIT(Weight, 7.2, Double, DATE_TIME_NO_VAR(2017, 8, 19, 12, 32, 0), pVariates);
        ADD_PV_NO_UNIT(Weight, 7.4, Double, DATE_TIME_NO_VAR(2017, 8, 21, 12, 32, 0), pVariates);
        ADD_PV_NO_UNIT(Weight, 7.6, Double, DATE_TIME_NO_VAR(2017, 8, 12, 12, 32, 0), pVariates);
        ADD_PV_NO_UNIT(Weight, 7.9, Double, DATE_TIME_NO_VAR(2017, 8, 11, 12, 32, 0), pVariates);
        ADD_PV_NO_UNIT(Weight, 7.0, Double, DATE_TIME_NO_VAR(2017, 8, 30, 12, 32, 0), pVariates);
        ADD_PV_NO_UNIT(Weight, 7.8, Double, DATE_TIME_NO_VAR(2017, 8, 30, 16, 32, 0), pVariates);

        CovariateExtractor extractor(
                cDefinitions,
                pVariates,
                DATE_TIME_NO_VAR(2017, 8, 19, 13, 0, 0),
                DATE_TIME_NO_VAR(2017, 8, 19, 17, 0, 0));
        TestCovariateExtractor::test_sortPatientVariates(&extractor);

        std::vector<std::unique_ptr<PatientCovariate>> res_pvVec;
        res_pvVec.push_back(std::make_unique<PatientCovariate>(
                "Weight", varToString(7.2), DataType::Double, TucuUnit(), DATE_TIME_NO_VAR(2017, 8, 19, 12, 32, 0)));
        res_pvVec.push_back(std::make_unique<PatientCovariate>(
                "Weight", varToString(6.3), DataType::Double, TucuUnit(), DATE_TIME_NO_VAR(2017, 8, 19, 22, 32, 0)));

        const auto pvVals = TestCovariateExtractor::get_m_pvValued(&extractor).at("Weight");

        ASSERT_EQ(pvVals.size(), res_pvVec.size());
        for (size_t i = 0; i < std::min(pvVals.size(), res_pvVec.size()); ++i) {
            ASSERT_EQ(**(pvVals[i]), *(res_pvVec[i]));
        }
    }
}

TEST(Core_TestCovariateExtractor, CovariateExtraction_test1_0)
{
    CovariateDefinitions cDefinitions;

    ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
    ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
    ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);

    ADD_EXPR2_CDEF(Special, "Weight * 0.5 + IsMale * 15", "Weight", "IsMale", cDefinitions);

    const DATE_TIME_VAR(startDate, 2017, 8, 12, 8, 0, 0);
    const DATE_TIME_VAR(endDate, 2017, 8, 17, 8, 0, 0);

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
    ComputingStatus rc;
    rc = extractor.extract(series);

    //                printCovariateSeries(series);

    ASSERT_EQ(series.size(), static_cast<size_t>(5));
    ASSERT_EQ(rc, ComputingStatus::Ok);
    // First measure propagated back to the start of the interval.
    ASSERT_TRUE(covariateEventIsPresent("Gist", DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), 1, series));
    // Second measure at the moment it is performed.
    ASSERT_TRUE(covariateEventIsPresent("Gist", DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0), 0, series));
    // Remaining values propagated back to the start of the interval.
    ASSERT_TRUE(covariateEventIsPresent("Weight", DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), 3.5, series));
    ASSERT_TRUE(covariateEventIsPresent("IsMale", DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), 1, series));
    // This should be computed from IsMale and Weight.
    ASSERT_TRUE(covariateEventIsPresent("Special", DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), 16.75, series));

    /// \todo test1 with first event before start interval -> must start with one!
    /// \todo test1 with second event also before start interval -> must start with zero!

    // Test 2: call the extractor with no patient covariates.

    PatientVariates pVariates2;
    CovariateExtractor extractor2(cDefinitions, pVariates2, startDate, endDate);
    series.clear();
    rc = extractor2.extract(series);

    //                printCovariateSeries(series);

    ASSERT_EQ(series.size(), static_cast<size_t>(4));
    ASSERT_EQ(rc, ComputingStatus::Ok);
    // All events should have the initial default value.
    ASSERT_TRUE(covariateEventIsPresent("Gist", DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), 0, series));
    ASSERT_TRUE(covariateEventIsPresent("Weight", DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), 3.5, series));
    ASSERT_TRUE(covariateEventIsPresent("IsMale", DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), 1, series));
    ASSERT_TRUE(covariateEventIsPresent("Special", DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), 16.75, series));

    // Test 3: add gist and model weight and special changes.

    // gist == true @ 13.08.2017, 12h32.
    auto patient_gist_3 = std::make_unique<PatientCovariate>(
            "Gist", varToString(true), DataType::Bool, TucuUnit("-"), DATE_TIME_NO_VAR(2017, 8, 13, 12, 32, 0));

    // weight = 3.8 @ 13.08.2017, 9h00.
    auto patient_weight_1 = std::make_unique<PatientCovariate>(
            "Weight", varToString(3.8), DataType::Double, TucuUnit("kg"), DATE_TIME_NO_VAR(2017, 8, 13, 9, 0, 0));
    // weight = 4.05 @ 15.08.2017, 21h00.
    auto patient_weight_2 = std::make_unique<PatientCovariate>(
            "Weight", varToString(4.05), DataType::Double, TucuUnit("kg"), DATE_TIME_NO_VAR(2017, 8, 15, 21, 0, 0));
    // weight = 4.25 @ 16.08.2017, 21h00.
    auto patient_weight_3 = std::make_unique<PatientCovariate>(
            "Weight", varToString(4.25), DataType::Double, TucuUnit("kg"), DATE_TIME_NO_VAR(2017, 8, 16, 21, 0, 0));
    PatientVariates pVariates3;
    pVariates3.push_back(std::move(patient_gist_3));
    pVariates3.push_back(std::move(patient_weight_1));
    pVariates3.push_back(std::move(patient_weight_2));
    pVariates3.push_back(std::move(patient_weight_3));

    CovariateExtractor extractor3(cDefinitions, pVariates3, startDate, endDate);
    series.clear();
    rc = extractor3.extract(series);

    //                printCovariateSeries(series);

    ASSERT_EQ(series.size(), static_cast<size_t>(12));
    ASSERT_EQ(rc, ComputingStatus::Ok);

    ASSERT_TRUE(covariateEventIsPresent("Gist", DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), 1, series));
    ASSERT_TRUE(covariateEventIsPresent("IsMale", DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), 1, series));
    // Since the first measure is past the beginning of the interval, we expect a value
    // at the beginning of the interval (the refresh period would have put this value one
    // day before, but that could have been weird).
    ASSERT_TRUE(covariateEventIsPresent("Weight", DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), 3.8, series));
    ASSERT_TRUE(covariateEventIsPresent("Weight", DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0), 3.89583, series));
    ASSERT_TRUE(covariateEventIsPresent("Weight", DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), 3.99583, series));
    ASSERT_TRUE(covariateEventIsPresent("Weight", DATE_TIME_NO_VAR(2017, 8, 16, 8, 0, 0), 4.14167, series));
    ASSERT_TRUE(covariateEventIsPresent("Weight", DATE_TIME_NO_VAR(2017, 8, 17, 8, 0, 0), 4.25, series));
    // Computed value at the beginning of the interval.
    ASSERT_TRUE(covariateEventIsPresent("Special", DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), 16.9, series));
    ASSERT_TRUE(covariateEventIsPresent("Special", DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0), 16.9479, series));
    ASSERT_TRUE(covariateEventIsPresent("Special", DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), 16.9979, series));
    ASSERT_TRUE(covariateEventIsPresent("Special", DATE_TIME_NO_VAR(2017, 8, 16, 8, 0, 0), 17.0708, series));
    ASSERT_TRUE(covariateEventIsPresent("Special", DATE_TIME_NO_VAR(2017, 8, 17, 8, 0, 0), 17.125, series));
}

TEST(Core_TestCovariateExtractor, CovariateExtraction_test2_0)
{
    CovariateDefinitions cDefinitions;
    ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
    ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
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

    ADD_PV_NO_UNIT(
            birthdate, DATE_TIME_NO_VAR(2017, 8, 5, 8, 0, 0), Date, DATE_TIME_NO_VAR(2017, 8, 5, 8, 0, 0), pVariates);

    const DATE_TIME_VAR(startDate, 2017, 8, 12, 8, 0, 0);
    const DATE_TIME_VAR(endDate, 2017, 8, 17, 21, 0, 0);

    CovariateExtractor extractor(cDefinitions, pVariates, startDate, endDate);
    CovariateSeries series;
    ComputingStatus rc;
    rc = extractor.extract(series);

    //        printCovariateSeries(series);

    ASSERT_EQ(series.size(), static_cast<size_t>(7));
    ASSERT_EQ(rc, ComputingStatus::Ok);
    // First measure propagated back to the start of the interval.
    ASSERT_TRUE(covariateEventIsPresent("Gist", DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), 1, series));
    // Second measure at the moment it is performed.
    ASSERT_TRUE(covariateEventIsPresent("Gist", DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0), 0, series));
    // Remaining values propagated back to the start of the interval.
    ASSERT_TRUE(covariateEventIsPresent("Weight", DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), 3.5, series));
    ASSERT_TRUE(covariateEventIsPresent("IsMale", DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), 1, series));
    // This should be computed from IsMale and Weight.
    ASSERT_TRUE(covariateEventIsPresent("Special", DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), 16.75, series));
    // This should be computed from IsMale, Weight, and Gist.
    ASSERT_TRUE(covariateEventIsPresent("VerySpecial", DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), 41.75, series));
    ASSERT_TRUE(covariateEventIsPresent("VerySpecial", DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0), 16.75, series));

    ASSERT_EQ(TestCovariateExtractor::get_m_birthDate(&extractor), DATE_TIME_NO_VAR(2017, 8, 5, 8, 0, 0));
    ASSERT_TRUE(TestCovariateExtractor::get_m_hasBirthDate(&extractor));
    //ASSERT_EQ(extractor.m_pvValued.count(CovariateExtractor::sm_birthDateCName), static_cast<size_t>(1));
}

TEST(Core_TestCovariateExtractor, CovariateExtraction_test2_1)
{
    CovariateDefinitions cDefinitions;
    ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
    ADD_CDEF_NO_R_UNIT(Weight, 3.5, Standard, Double, Linear, "kg", cDefinitions);
    ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);

    ADD_EXPR2_CDEF(Special, "Weight * 0.5 + IsMale * 15", "Weight", "IsMale", cDefinitions);
    ADD_EXPR3_CDEF(VerySpecial, "Weight * 0.5 + IsMale * 15 + Gist * 25", "Weight", "IsMale", "Gist", cDefinitions);

    // gist == true @ 13.08.2017, 12h32.
    PatientVariates pVariates;
    ADD_PV_NO_UNIT(Gist, true, Bool, DATE_TIME_NO_VAR(2017, 8, 12, 12, 32, 0), pVariates);
    // gist == false @ 13.08.2017, 14h32.
    ADD_PV_NO_UNIT(Gist, false, Bool, DATE_TIME_NO_VAR(2017, 8, 13, 6, 32, 0), pVariates);
    // weight = 6.0 @ 12.08.2017, 14h32
    ADD_PV_W_UNIT(Weight, 6.0, Double, "kg", DATE_TIME_NO_VAR(2017, 8, 12, 14, 32, 0), pVariates);
    // weight = 30.0 @ 13.08.2017, 14h32
    ADD_PV_W_UNIT(Weight, 30.0, Double, "kg", DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0), pVariates);
    // weight = 40.0 @ 15.08.2017, 14h32
    ADD_PV_W_UNIT(Weight, 40.0, Double, "kg", DATE_TIME_NO_VAR(2017, 8, 15, 14, 32, 0), pVariates);

    const DATE_TIME_VAR(startDate, 2017, 8, 12, 8, 0, 0);
    const DATE_TIME_VAR(endDate, 2017, 8, 17, 21, 0, 0);

    CovariateExtractor extractor(cDefinitions, pVariates, startDate, endDate);
    CovariateSeries series;
    ComputingStatus rc;
    rc = extractor.extract(series);

    //        printCovariateSeries(series);

    ASSERT_EQ(series.size(), static_cast<size_t>(15));
    ASSERT_EQ(rc, ComputingStatus::Ok);
    // First measure propagated back to the start of the interval.
    ASSERT_TRUE(covariateEventIsPresent("Gist", DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), 1, series));
    // Second measure at the moment it is performed.
    ASSERT_TRUE(covariateEventIsPresent("Gist", DATE_TIME_NO_VAR(2017, 8, 13, 6, 32, 0), 0, series));
    // Weight, interpolated at the moment the gist changed.
    ASSERT_TRUE(covariateEventIsPresent("Weight", DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), 6.0, series));
    ASSERT_TRUE(covariateEventIsPresent("Weight", DATE_TIME_NO_VAR(2017, 8, 13, 6, 32, 0), 22.0, series));
    ASSERT_TRUE(covariateEventIsPresent("Weight", DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0), 30.0, series));
    ASSERT_TRUE(covariateEventIsPresent("Weight", DATE_TIME_NO_VAR(2017, 8, 15, 14, 32, 0), 40.0, series));
    // This does not change.
    ASSERT_TRUE(covariateEventIsPresent("IsMale", DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), 1, series));
    // This should be computed from IsMale and Weight.
    ASSERT_TRUE(covariateEventIsPresent("Special", DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), 18.0, series));
    ASSERT_TRUE(covariateEventIsPresent("Special", DATE_TIME_NO_VAR(2017, 8, 13, 6, 32, 0), 26.0, series));
    ASSERT_TRUE(covariateEventIsPresent("Special", DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0), 30.0, series));
    ASSERT_TRUE(covariateEventIsPresent("Special", DATE_TIME_NO_VAR(2017, 8, 15, 14, 32, 0), 35.0, series));
    // This should be computed from IsMale, Weight, and Gist.
    ASSERT_TRUE(covariateEventIsPresent("VerySpecial", DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), 43.0, series));
    ASSERT_TRUE(covariateEventIsPresent("VerySpecial", DATE_TIME_NO_VAR(2017, 8, 13, 6, 32, 0), 26.0, series));
    ASSERT_TRUE(covariateEventIsPresent("VerySpecial", DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0), 30.0, series));
    ASSERT_TRUE(covariateEventIsPresent("VerySpecial", DATE_TIME_NO_VAR(2017, 8, 15, 14, 32, 0), 35.0, series));
}

TEST(Core_TestCovariateExtractor, CovariateExtraction_test2_2)
{
    CovariateDefinitions cDefinitions;
    ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
    ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
    ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);

    ADD_EXPR2_CDEF(Special, "Weight * 0.5 + IsMale * 15", "Weight", "IsMale", cDefinitions);
    ADD_EXPR3_CDEF(VerySpecial, "Weight * 0.5 + IsMale * 15 + Gist * 25", "Weight", "IsMale", "Gist", cDefinitions);

    // gist == true @ 13.08.2017, 12h32.
    PatientVariates pVariates;
    ADD_PV_NO_UNIT(Gist, true, Bool, DATE_TIME_NO_VAR(2017, 8, 12, 12, 32, 0), pVariates);
    // gist == false @ 13.08.2017, 06h00.
    ADD_PV_NO_UNIT(Gist, false, Bool, DATE_TIME_NO_VAR(2017, 8, 13, 6, 00, 0), pVariates);
    // weight = 6.0 @ 12.08.2017, 14h00
    ADD_PV_W_UNIT(Weight, 6.0, Double, "kg", DATE_TIME_NO_VAR(2017, 8, 12, 14, 00, 0), pVariates);
    // weight = 30.0 @ 13.08.2017, 14h00
    ADD_PV_W_UNIT(Weight, 30.0, Double, "kg", DATE_TIME_NO_VAR(2017, 8, 13, 14, 00, 0), pVariates);
    // weight = 78.0 @ 15.08.2017, 14h00
    ADD_PV_W_UNIT(Weight, 78.0, Double, "kg", DATE_TIME_NO_VAR(2017, 8, 15, 14, 00, 0), pVariates);

    const DATE_TIME_VAR(startDate, 2017, 8, 12, 8, 0, 0);
    const DATE_TIME_VAR(endDate, 2017, 8, 16, 21, 0, 0);

    CovariateExtractor extractor(cDefinitions, pVariates, startDate, endDate);
    CovariateSeries series;
    ComputingStatus rc;
    rc = extractor.extract(series);

    //        printCovariateSeries(series);

    ASSERT_EQ(series.size(), static_cast<size_t>(21));
    ASSERT_EQ(rc, ComputingStatus::Ok);
    // This does not change.
    ASSERT_TRUE(covariateEventIsPresent("IsMale", DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), 1, series));
    // First measure propagated back to the start of the interval.
    ASSERT_TRUE(covariateEventIsPresent("Gist", DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), 1, series));
    // Second measure at the moment it is performed.
    ASSERT_TRUE(covariateEventIsPresent("Gist", DATE_TIME_NO_VAR(2017, 8, 13, 6, 0, 0), 0, series));
    // Weight, interpolated at the moment the gist changed.
    ASSERT_TRUE(covariateEventIsPresent("Weight", DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), 6.0, series));
    ASSERT_TRUE(covariateEventIsPresent("Weight", DATE_TIME_NO_VAR(2017, 8, 13, 6, 0, 0), 22.0, series));
    ASSERT_TRUE(covariateEventIsPresent("Weight", DATE_TIME_NO_VAR(2017, 8, 13, 8, 0, 0), 24.0, series));
    ASSERT_TRUE(covariateEventIsPresent("Weight", DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0), 48.0, series));
    ASSERT_TRUE(covariateEventIsPresent("Weight", DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), 72.0, series));
    ASSERT_TRUE(covariateEventIsPresent("Weight", DATE_TIME_NO_VAR(2017, 8, 16, 8, 0, 0), 78.0, series));
    // This should be computed from IsMale and Weight.
    ASSERT_TRUE(covariateEventIsPresent("Special", DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), 18.0, series));
    ASSERT_TRUE(covariateEventIsPresent("Special", DATE_TIME_NO_VAR(2017, 8, 13, 6, 0, 0), 26.0, series));
    ASSERT_TRUE(covariateEventIsPresent("Special", DATE_TIME_NO_VAR(2017, 8, 13, 8, 0, 0), 27.0, series));
    ASSERT_TRUE(covariateEventIsPresent("Special", DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0), 39.0, series));
    ASSERT_TRUE(covariateEventIsPresent("Special", DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), 51.0, series));
    ASSERT_TRUE(covariateEventIsPresent("Special", DATE_TIME_NO_VAR(2017, 8, 16, 8, 0, 0), 54.0, series));
    // This should be computed from IsMale, Weight, and Gist.
    ASSERT_TRUE(covariateEventIsPresent("VerySpecial", DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), 43.0, series));
    ASSERT_TRUE(covariateEventIsPresent("VerySpecial", DATE_TIME_NO_VAR(2017, 8, 13, 6, 0, 0), 26.0, series));
    ASSERT_TRUE(covariateEventIsPresent("VerySpecial", DATE_TIME_NO_VAR(2017, 8, 13, 8, 0, 0), 27.0, series));
    ASSERT_TRUE(covariateEventIsPresent("VerySpecial", DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0), 39.0, series));
    ASSERT_TRUE(covariateEventIsPresent("VerySpecial", DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), 51.0, series));
    ASSERT_TRUE(covariateEventIsPresent("VerySpecial", DATE_TIME_NO_VAR(2017, 8, 16, 8, 0, 0), 54.0, series));
}

TEST(Core_TestCovariateExtractor, CovariateExtraction_test2_3)
{
    CovariateDefinitions cDefinitions;
    ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
    ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
    ADD_CDEF_NO_R_UNIT(Height, 100, Standard, Double, Linear, "cm", cDefinitions);

    ADD_EXPR2_CDEF(Special, "Weight * 0.5 + Height", "Weight", "Height", cDefinitions);

    // gist == true @ 13.08.2017, 12h32.
    PatientVariates pVariates;
    ADD_PV_NO_UNIT(Gist, true, Bool, DATE_TIME_NO_VAR(2017, 8, 12, 12, 32, 0), pVariates);
    // gist == false @ 13.08.2017, 06h00.
    ADD_PV_NO_UNIT(Gist, false, Bool, DATE_TIME_NO_VAR(2017, 8, 13, 6, 0, 0), pVariates);

    // weight = 6.0 @ 12.08.2017, 14h00
    ADD_PV_W_UNIT(Weight, 6.0, Double, "kg", DATE_TIME_NO_VAR(2017, 8, 12, 14, 0, 0), pVariates);
    // weight = 30.0 @ 13.08.2017, 14h00
    ADD_PV_W_UNIT(Weight, 30.0, Double, "kg", DATE_TIME_NO_VAR(2017, 8, 13, 14, 0, 0), pVariates);
    // weight = 78.0 @ 15.08.2017, 14h00
    ADD_PV_W_UNIT(Weight, 78.0, Double, "kg", DATE_TIME_NO_VAR(2017, 8, 15, 14, 0, 0), pVariates);

    // height = 20.0 @ 12.08.2017, 20h00
    ADD_PV_W_UNIT(Height, 20.0, Double, "cm", DATE_TIME_NO_VAR(2017, 8, 12, 20, 0, 0), pVariates);
    // height = 40.0 @ 12.08.2017, 22h00
    ADD_PV_W_UNIT(Height, 40.0, Double, "cm", DATE_TIME_NO_VAR(2017, 8, 12, 22, 0, 0), pVariates);
    // height = 280.0 @ 13.08.2017, 22h00
    ADD_PV_W_UNIT(Height, 280.0, Double, "cm", DATE_TIME_NO_VAR(2017, 8, 13, 22, 0, 0), pVariates);
    // height = 380.0 @ 14.08.2017, 08h00
    ADD_PV_W_UNIT(Height, 380.0, Double, "cm", DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0), pVariates);

    const DATE_TIME_VAR(startDate, 2017, 8, 12, 8, 0, 0);
    const DATE_TIME_VAR(endDate, 2017, 8, 16, 21, 0, 0);

    CovariateExtractor extractor(cDefinitions, pVariates, startDate, endDate);
    CovariateSeries series;
    ComputingStatus rc;
    rc = extractor.extract(series);

    //        printCovariateSeries(series);

    ASSERT_EQ(series.size(), static_cast<size_t>(26));
    ASSERT_EQ(rc, ComputingStatus::Ok);
    // First measure propagated back to the start of the interval.
    ASSERT_TRUE(covariateEventIsPresent("Gist", DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), 1, series));
    // Second measure at the moment it is performed.
    ASSERT_TRUE(covariateEventIsPresent("Gist", DATE_TIME_NO_VAR(2017, 8, 13, 6, 0, 0), 0, series));
    // Weight.
    ASSERT_TRUE(covariateEventIsPresent("Weight", DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), 6.0, series));
    ASSERT_TRUE(covariateEventIsPresent("Weight", DATE_TIME_NO_VAR(2017, 8, 12, 20, 0, 0), 12.0, series));
    ASSERT_TRUE(covariateEventIsPresent("Weight", DATE_TIME_NO_VAR(2017, 8, 12, 22, 0, 0), 14.0, series));
    ASSERT_TRUE(covariateEventIsPresent("Weight", DATE_TIME_NO_VAR(2017, 8, 13, 6, 0, 0), 22.0, series));
    ASSERT_TRUE(covariateEventIsPresent("Weight", DATE_TIME_NO_VAR(2017, 8, 13, 8, 0, 0), 24.0, series));
    ASSERT_TRUE(covariateEventIsPresent("Weight", DATE_TIME_NO_VAR(2017, 8, 13, 22, 0, 0), 38.0, series));
    ASSERT_TRUE(covariateEventIsPresent("Weight", DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0), 48.0, series));
    ASSERT_TRUE(covariateEventIsPresent("Weight", DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), 72.0, series));
    ASSERT_TRUE(covariateEventIsPresent("Weight", DATE_TIME_NO_VAR(2017, 8, 16, 8, 0, 0), 78.0, series));
    // Height.
    ASSERT_TRUE(covariateEventIsPresent("Height", DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), 20.0, series));
    ASSERT_TRUE(covariateEventIsPresent("Height", DATE_TIME_NO_VAR(2017, 8, 12, 22, 0, 0), 40.0, series));
    ASSERT_TRUE(covariateEventIsPresent("Height", DATE_TIME_NO_VAR(2017, 8, 13, 6, 0, 0), 120.0, series));
    ASSERT_TRUE(covariateEventIsPresent("Height", DATE_TIME_NO_VAR(2017, 8, 13, 8, 0, 0), 140.0, series));
    ASSERT_TRUE(covariateEventIsPresent("Height", DATE_TIME_NO_VAR(2017, 8, 13, 22, 0, 0), 280.0, series));
    ASSERT_TRUE(covariateEventIsPresent("Height", DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0), 380.0, series));
    // This should be computed from Height and Weight.
    ASSERT_TRUE(covariateEventIsPresent("Special", DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), 23.0, series));
    ASSERT_TRUE(covariateEventIsPresent("Special", DATE_TIME_NO_VAR(2017, 8, 12, 20, 0, 0), 26.0, series));
    ASSERT_TRUE(covariateEventIsPresent("Special", DATE_TIME_NO_VAR(2017, 8, 12, 22, 0, 0), 47.0, series));
    ASSERT_TRUE(covariateEventIsPresent("Special", DATE_TIME_NO_VAR(2017, 8, 13, 6, 0, 0), 131.0, series));
    ASSERT_TRUE(covariateEventIsPresent("Special", DATE_TIME_NO_VAR(2017, 8, 13, 8, 0, 0), 152.0, series));
    ASSERT_TRUE(covariateEventIsPresent("Special", DATE_TIME_NO_VAR(2017, 8, 13, 22, 0, 0), 299.0, series));
    ASSERT_TRUE(covariateEventIsPresent("Special", DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0), 404.0, series));
    ASSERT_TRUE(covariateEventIsPresent("Special", DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), 416.0, series));
    ASSERT_TRUE(covariateEventIsPresent("Special", DATE_TIME_NO_VAR(2017, 8, 16, 8, 0, 0), 419.0, series));
}

TEST(Core_TestCovariateExtractor, CovariateExtraction_test3_0)
{
    CovariateDefinitions cDefinitions;
    ADD_CDEF_NO_R_UNIT(Weight, 3.5, Standard, Double, Linear, "kg", cDefinitions);
    ADD_CDEF_NO_R(AgeDays, 35, AgeInDays, Double, Direct, cDefinitions);
    //ADD_CDEF_NO_R_STR(CovariateExtractor::sm_birthDateCName, DATE_TIME_NO_VAR(2017, 8, 5, 18, 0, 0), Standard,
    //                  Date, Direct, cDefinitions);

    ADD_EXPR2_CDEF(DepOnAge, "AgeDays + Weight * 2", "AgeDays", "Weight", cDefinitions);

    PatientVariates pVariates;
    // weight = 6.0 @ 12.08.2017, 14h00
    ADD_PV_W_UNIT(Weight, 4.0, Double, "kg", DATE_TIME_NO_VAR(2017, 8, 13, 12, 0, 0), pVariates);
    // weight = 30.0 @ 13.08.2017, 14h00
    ADD_PV_W_UNIT(Weight, 100.0, Double, "kg", DATE_TIME_NO_VAR(2017, 8, 17, 12, 0, 0), pVariates);
    ADD_PV_NO_UNIT(
            birthdate, DATE_TIME_NO_VAR(2017, 8, 5, 18, 0, 0), Date, DATE_TIME_NO_VAR(2017, 8, 5, 18, 0, 0), pVariates);

    const DATE_TIME_VAR(startDate, 2017, 8, 12, 8, 0, 0);
    const DATE_TIME_VAR(endDate, 2017, 8, 18, 21, 0, 0);

    CovariateExtractor extractor(cDefinitions, pVariates, startDate, endDate);
    CovariateSeries series;
    ComputingStatus rc;
    rc = extractor.extract(series);

    //        printCovariateSeries(series);

    ASSERT_EQ(series.size(), static_cast<size_t>(23));
    ASSERT_EQ(rc, ComputingStatus::Ok);
    ASSERT_TRUE(covariateEventIsPresent("Weight", DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), 4.0, series));
    ASSERT_TRUE(covariateEventIsPresent("Weight", DATE_TIME_NO_VAR(2017, 8, 13, 18, 0, 0), 10.0, series));
    ASSERT_TRUE(covariateEventIsPresent("Weight", DATE_TIME_NO_VAR(2017, 8, 14, 18, 0, 0), 34.0, series));
    ASSERT_TRUE(covariateEventIsPresent("Weight", DATE_TIME_NO_VAR(2017, 8, 15, 18, 0, 0), 58.0, series));
    ASSERT_TRUE(covariateEventIsPresent("Weight", DATE_TIME_NO_VAR(2017, 8, 16, 18, 0, 0), 82.0, series));
    ASSERT_TRUE(covariateEventIsPresent("Weight", DATE_TIME_NO_VAR(2017, 8, 17, 12, 0, 0), 100.0, series));

    ASSERT_TRUE(covariateEventIsPresent("AgeDays", DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), 6.0, series));
    ASSERT_TRUE(covariateEventIsPresent("AgeDays", DATE_TIME_NO_VAR(2017, 8, 12, 18, 0, 0), 7.0, series));
    ASSERT_TRUE(covariateEventIsPresent("AgeDays", DATE_TIME_NO_VAR(2017, 8, 13, 18, 0, 0), 8.0, series));
    ASSERT_TRUE(covariateEventIsPresent("AgeDays", DATE_TIME_NO_VAR(2017, 8, 14, 18, 0, 0), 9.0, series));
    ASSERT_TRUE(covariateEventIsPresent("AgeDays", DATE_TIME_NO_VAR(2017, 8, 15, 18, 0, 0), 10.0, series));
    ASSERT_TRUE(covariateEventIsPresent("AgeDays", DATE_TIME_NO_VAR(2017, 8, 16, 18, 0, 0), 11.0, series));
    ASSERT_TRUE(covariateEventIsPresent("AgeDays", DATE_TIME_NO_VAR(2017, 8, 17, 18, 0, 0), 12.0, series));
    ASSERT_TRUE(covariateEventIsPresent("AgeDays", DATE_TIME_NO_VAR(2017, 8, 18, 18, 0, 0), 13.0, series));

    ASSERT_TRUE(covariateEventIsPresent("DepOnAge", DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), 14.0, series));
    ASSERT_TRUE(covariateEventIsPresent("DepOnAge", DATE_TIME_NO_VAR(2017, 8, 12, 18, 0, 0), 15.0, series));
    ASSERT_TRUE(covariateEventIsPresent("DepOnAge", DATE_TIME_NO_VAR(2017, 8, 13, 18, 0, 0), 28.0, series));
    ASSERT_TRUE(covariateEventIsPresent("DepOnAge", DATE_TIME_NO_VAR(2017, 8, 14, 18, 0, 0), 77.0, series));
    ASSERT_TRUE(covariateEventIsPresent("DepOnAge", DATE_TIME_NO_VAR(2017, 8, 15, 18, 0, 0), 126.0, series));
    ASSERT_TRUE(covariateEventIsPresent("DepOnAge", DATE_TIME_NO_VAR(2017, 8, 16, 18, 0, 0), 175.0, series));
    ASSERT_TRUE(covariateEventIsPresent("DepOnAge", DATE_TIME_NO_VAR(2017, 8, 17, 12, 0, 0), 211.0, series));
    ASSERT_TRUE(covariateEventIsPresent("DepOnAge", DATE_TIME_NO_VAR(2017, 8, 17, 18, 0, 0), 212.0, series));
    ASSERT_TRUE(covariateEventIsPresent("DepOnAge", DATE_TIME_NO_VAR(2017, 8, 18, 18, 0, 0), 213.0, series));

    ASSERT_TRUE(TestCovariateExtractor::get_m_birthDate(&extractor) == DATE_TIME_NO_VAR(2017, 8, 5, 18, 0, 0));
    ASSERT_TRUE(TestCovariateExtractor::get_m_hasBirthDate(&extractor));
    ASSERT_TRUE(TestCovariateExtractor::get_m_pvValued(&extractor).count(CovariateExtractor::BIRTHDATE_CNAME) == 0);
}

TEST(Core_TestCovariateExtractor, CovariateExtraction_test3_1)
{
    CovariateDefinitions cDefinitions;
    ADD_CDEF_NO_R_UNIT(Weight, 3.5, Standard, Double, Linear, "kg", cDefinitions);
    ADD_CDEF_NO_R(AgeDays, 35, AgeInDays, Double, Direct, cDefinitions);
    //ADD_CDEF_NO_R_STR(CovariateExtractor::sm_birthDateCName, DATE_TIME_NO_VAR(2017, 8, 5, 18, 0, 0), Standard,
    //                  Date, Direct, cDefinitions);


    ADD_EXPR2_CDEF(DepOnAge, "AgeDays + Weight * 2", "AgeDays", "Weight", cDefinitions);
    ADD_EXPR2_CDEF(DepDep, "DepOnAge + 42 + AgeDays", "AgeDays", "DepOnAge", cDefinitions);

    PatientVariates pVariates;
    // weight = 6.0 @ 12.08.2017, 14h00
    ADD_PV_W_UNIT(Weight, 4.0, Double, "kg", DATE_TIME_NO_VAR(2017, 8, 13, 12, 0, 0), pVariates);
    // weight = 30.0 @ 13.08.2017, 14h00
    ADD_PV_W_UNIT(Weight, 100.0, Double, "kg", DATE_TIME_NO_VAR(2017, 8, 17, 12, 0, 0), pVariates);

    ADD_PV_NO_UNIT(
            birthdate, DATE_TIME_NO_VAR(2017, 8, 5, 18, 0, 0), Date, DATE_TIME_NO_VAR(2017, 8, 5, 18, 0, 0), pVariates);

    const DATE_TIME_VAR(startDate, 2017, 8, 12, 8, 0, 0);
    const DATE_TIME_VAR(endDate, 2017, 8, 18, 21, 0, 0);

    CovariateExtractor extractor(cDefinitions, pVariates, startDate, endDate);
    CovariateSeries series;
    ComputingStatus rc;
    rc = extractor.extract(series);

    //        printCovariateSeries(series);

    ASSERT_EQ(series.size(), static_cast<size_t>(32));
    ASSERT_EQ(rc, ComputingStatus::Ok);
    ASSERT_TRUE(covariateEventIsPresent("Weight", DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), 4.0, series));
    ASSERT_TRUE(covariateEventIsPresent("Weight", DATE_TIME_NO_VAR(2017, 8, 13, 18, 0, 0), 10.0, series));
    ASSERT_TRUE(covariateEventIsPresent("Weight", DATE_TIME_NO_VAR(2017, 8, 14, 18, 0, 0), 34.0, series));
    ASSERT_TRUE(covariateEventIsPresent("Weight", DATE_TIME_NO_VAR(2017, 8, 15, 18, 0, 0), 58.0, series));
    ASSERT_TRUE(covariateEventIsPresent("Weight", DATE_TIME_NO_VAR(2017, 8, 16, 18, 0, 0), 82.0, series));
    ASSERT_TRUE(covariateEventIsPresent("Weight", DATE_TIME_NO_VAR(2017, 8, 17, 12, 0, 0), 100.0, series));

    ASSERT_TRUE(covariateEventIsPresent("AgeDays", DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), 6.0, series));
    ASSERT_TRUE(covariateEventIsPresent("AgeDays", DATE_TIME_NO_VAR(2017, 8, 12, 18, 0, 0), 7.0, series));
    ASSERT_TRUE(covariateEventIsPresent("AgeDays", DATE_TIME_NO_VAR(2017, 8, 13, 18, 0, 0), 8.0, series));
    ASSERT_TRUE(covariateEventIsPresent("AgeDays", DATE_TIME_NO_VAR(2017, 8, 14, 18, 0, 0), 9.0, series));
    ASSERT_TRUE(covariateEventIsPresent("AgeDays", DATE_TIME_NO_VAR(2017, 8, 15, 18, 0, 0), 10.0, series));
    ASSERT_TRUE(covariateEventIsPresent("AgeDays", DATE_TIME_NO_VAR(2017, 8, 16, 18, 0, 0), 11.0, series));
    ASSERT_TRUE(covariateEventIsPresent("AgeDays", DATE_TIME_NO_VAR(2017, 8, 17, 18, 0, 0), 12.0, series));
    ASSERT_TRUE(covariateEventIsPresent("AgeDays", DATE_TIME_NO_VAR(2017, 8, 18, 18, 0, 0), 13.0, series));

    ASSERT_TRUE(covariateEventIsPresent("DepOnAge", DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), 14.0, series));
    ASSERT_TRUE(covariateEventIsPresent("DepOnAge", DATE_TIME_NO_VAR(2017, 8, 12, 18, 0, 0), 15.0, series));
    ASSERT_TRUE(covariateEventIsPresent("DepOnAge", DATE_TIME_NO_VAR(2017, 8, 13, 18, 0, 0), 28.0, series));
    ASSERT_TRUE(covariateEventIsPresent("DepOnAge", DATE_TIME_NO_VAR(2017, 8, 14, 18, 0, 0), 77.0, series));
    ASSERT_TRUE(covariateEventIsPresent("DepOnAge", DATE_TIME_NO_VAR(2017, 8, 15, 18, 0, 0), 126.0, series));
    ASSERT_TRUE(covariateEventIsPresent("DepOnAge", DATE_TIME_NO_VAR(2017, 8, 16, 18, 0, 0), 175.0, series));
    ASSERT_TRUE(covariateEventIsPresent("DepOnAge", DATE_TIME_NO_VAR(2017, 8, 17, 12, 0, 0), 211.0, series));
    ASSERT_TRUE(covariateEventIsPresent("DepOnAge", DATE_TIME_NO_VAR(2017, 8, 17, 18, 0, 0), 212.0, series));
    ASSERT_TRUE(covariateEventIsPresent("DepOnAge", DATE_TIME_NO_VAR(2017, 8, 18, 18, 0, 0), 213.0, series));

    ASSERT_TRUE(covariateEventIsPresent("DepDep", DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), 62.0, series));
    ASSERT_TRUE(covariateEventIsPresent("DepDep", DATE_TIME_NO_VAR(2017, 8, 12, 18, 0, 0), 64.0, series));
    ASSERT_TRUE(covariateEventIsPresent("DepDep", DATE_TIME_NO_VAR(2017, 8, 13, 18, 0, 0), 78.0, series));
    ASSERT_TRUE(covariateEventIsPresent("DepDep", DATE_TIME_NO_VAR(2017, 8, 14, 18, 0, 0), 128.0, series));
    ASSERT_TRUE(covariateEventIsPresent("DepDep", DATE_TIME_NO_VAR(2017, 8, 15, 18, 0, 0), 178.0, series));
    ASSERT_TRUE(covariateEventIsPresent("DepDep", DATE_TIME_NO_VAR(2017, 8, 16, 18, 0, 0), 228.0, series));
    ASSERT_TRUE(covariateEventIsPresent("DepDep", DATE_TIME_NO_VAR(2017, 8, 17, 12, 0, 0), 264.0, series));
    ASSERT_TRUE(covariateEventIsPresent("DepDep", DATE_TIME_NO_VAR(2017, 8, 17, 18, 0, 0), 266.0, series));
    ASSERT_TRUE(covariateEventIsPresent("DepDep", DATE_TIME_NO_VAR(2017, 8, 18, 18, 0, 0), 268.0, series));

    ASSERT_EQ(TestCovariateExtractor::get_m_birthDate(&extractor), DATE_TIME_NO_VAR(2017, 8, 5, 18, 0, 0));
    ASSERT_TRUE(TestCovariateExtractor::get_m_hasBirthDate(&extractor));
    ASSERT_EQ(TestCovariateExtractor::get_m_pvValued(&extractor).count(CovariateExtractor::BIRTHDATE_CNAME), 0);
}

TEST(Core_TestCovariateExtractor, CovariateExtraction_test3_2)
{
    CovariateDefinitions cDefinitions;
    ADD_CDEF_NO_R_UNIT(Weight, 3.5, Standard, Double, Linear, "kg", cDefinitions);
    ADD_CDEF_NO_R(AgeDays, 42, AgeInDays, Double, Direct, cDefinitions);

    ADD_EXPR2_CDEF(DepOnAge, "AgeDays + Weight * 2", "AgeDays", "Weight", cDefinitions);

    PatientVariates pVariates;
    // weight = 6.0 @ 12.08.2017, 14h00
    ADD_PV_W_UNIT(Weight, 4.0, Double, "kg", DATE_TIME_NO_VAR(2017, 8, 13, 12, 0, 0), pVariates);
    // weight = 30.0 @ 13.08.2017, 14h00
    ADD_PV_W_UNIT(Weight, 100.0, Double, "kg", DATE_TIME_NO_VAR(2017, 8, 17, 12, 0, 0), pVariates);

    const DATE_TIME_VAR(startDate, 2017, 8, 12, 8, 0, 0);
    const DATE_TIME_VAR(endDate, 2017, 8, 18, 21, 0, 0);

    CovariateExtractor extractor(cDefinitions, pVariates, startDate, endDate);
    CovariateSeries series;
    ComputingStatus rc;
    rc = extractor.extract(series);

    //        printCovariateSeries(series);

    ASSERT_EQ(series.size(), static_cast<size_t>(21));
    ASSERT_EQ(rc, ComputingStatus::Ok);
    ASSERT_TRUE(covariateEventIsPresent("Weight", DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), 4.0, series));
    ASSERT_TRUE(covariateEventIsPresent("Weight", DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0), 24.0, series));
    ASSERT_TRUE(covariateEventIsPresent("Weight", DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), 48.0, series));
    ASSERT_TRUE(covariateEventIsPresent("Weight", DATE_TIME_NO_VAR(2017, 8, 16, 8, 0, 0), 72.0, series));
    ASSERT_TRUE(covariateEventIsPresent("Weight", DATE_TIME_NO_VAR(2017, 8, 17, 8, 0, 0), 96.0, series));
    ASSERT_TRUE(covariateEventIsPresent("Weight", DATE_TIME_NO_VAR(2017, 8, 17, 12, 0, 0), 100.0, series));

    ASSERT_TRUE(covariateEventIsPresent("AgeDays", DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), 42.0, series));
    ASSERT_TRUE(covariateEventIsPresent("AgeDays", DATE_TIME_NO_VAR(2017, 8, 13, 8, 0, 0), 43.0, series));
    ASSERT_TRUE(covariateEventIsPresent("AgeDays", DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0), 44.0, series));
    ASSERT_TRUE(covariateEventIsPresent("AgeDays", DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), 45.0, series));
    ASSERT_TRUE(covariateEventIsPresent("AgeDays", DATE_TIME_NO_VAR(2017, 8, 16, 8, 0, 0), 46.0, series));
    ASSERT_TRUE(covariateEventIsPresent("AgeDays", DATE_TIME_NO_VAR(2017, 8, 17, 8, 0, 0), 47.0, series));
    ASSERT_TRUE(covariateEventIsPresent("AgeDays", DATE_TIME_NO_VAR(2017, 8, 18, 8, 0, 0), 48.0, series));

    ASSERT_TRUE(covariateEventIsPresent("DepOnAge", DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), 50.0, series));
    ASSERT_TRUE(covariateEventIsPresent("DepOnAge", DATE_TIME_NO_VAR(2017, 8, 13, 8, 0, 0), 51.0, series));
    ASSERT_TRUE(covariateEventIsPresent("DepOnAge", DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0), 92.0, series));
    ASSERT_TRUE(covariateEventIsPresent("DepOnAge", DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), 141.0, series));
    ASSERT_TRUE(covariateEventIsPresent("DepOnAge", DATE_TIME_NO_VAR(2017, 8, 16, 8, 0, 0), 190.0, series));
    ASSERT_TRUE(covariateEventIsPresent("DepOnAge", DATE_TIME_NO_VAR(2017, 8, 17, 8, 0, 0), 239.0, series));
    ASSERT_TRUE(covariateEventIsPresent("DepOnAge", DATE_TIME_NO_VAR(2017, 8, 17, 12, 0, 0), 247.0, series));
    ASSERT_TRUE(covariateEventIsPresent("DepOnAge", DATE_TIME_NO_VAR(2017, 8, 18, 8, 0, 0), 248.0, series));

    ASSERT_FALSE(TestCovariateExtractor::get_m_hasBirthDate(&extractor));
}

TEST(Core_TestCovariateExtractor, CovariateExtraction_test3_3)
{
    CovariateDefinitions cDefinitions;

    ADD_CDEF_W_R_UNIT(Weight, 40, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
    ADD_CDEF_W_R_UNIT(Height, 195, Standard, Double, Linear, Tucuxi::Common::days(1), "cm", cDefinitions);
    ADD_CDEF_W_R_UNIT(Sex, 0, Standard, Int, Direct, Tucuxi::Common::days(1), "-", cDefinitions);
    ADD_CDEF_W_R_UNIT(Gist, false, Standard, Bool, Direct, Tucuxi::Common::days(1), "", cDefinitions);
    ADD_CDEF_W_R_UNIT(Scr, 50, Standard, Double, Linear, Tucuxi::Common::days(1), "umol/l", cDefinitions);
    ADD_CDEF_W_R_UNIT(CT, 50, Standard, Double, Linear, Tucuxi::Common::days(1), "g*h/l", cDefinitions);
    ADD_CDEF_W_R_UNIT(T, 12, Standard, Double, Linear, Tucuxi::Common::days(1), "s", cDefinitions);
    ADD_CDEF_W_R_UNIT(MM, 50, Standard, Double, Linear, Tucuxi::Common::days(1), "kg/umol", cDefinitions);
    ADD_CDEF_W_R_UNIT(FR, 15, Standard, Double, Linear, Tucuxi::Common::days(1), "ml/h", cDefinitions);



    ASSERT_NO_THROW(CovariateExtractor(
            cDefinitions,
            PatientVariates(),
            DATE_TIME_NO_VAR(2017, 8, 12, 14, 32, 0),
            DATE_TIME_NO_VAR(2017, 8, 19, 14, 32, 0)));

    PatientVariates pVariates;

    ADD_PV_W_UNIT(Weight, 1000000.0, Double, "mg", DATE_TIME_NO_VAR(2017, 8, 13, 16, 0, 0), pVariates);
    //        ADD_PV_W_UNIT(Weight, 1000000000.0, Double, ug, DATE_TIME_NO_VAR(2017, 8, 14, 16, 0, 0), pVariates);
    ADD_PV_W_UNIT(Height, 1200, Double, "mm", DATE_TIME_NO_VAR(2017, 8, 14, 16, 0, 0), pVariates);
    ADD_PV_W_UNIT(Sex, 1, Int, "", DATE_TIME_NO_VAR(2017, 8, 15, 16, 0, 0), pVariates);
    ADD_PV_W_UNIT(Gist, true, Bool, "-", DATE_TIME_NO_VAR(2017, 8, 16, 16, 0, 0), pVariates);
    ADD_PV_W_UNIT(Scr, 0.08, Double, "mmol/l", DATE_TIME_NO_VAR(2017, 8, 17, 16, 0, 0), pVariates);
    ADD_PV_W_UNIT(CT, 1, Double, "min*ug/ml", DATE_TIME_NO_VAR(2017, 8, 18, 16, 0, 0), pVariates);
    ADD_PV_W_UNIT(T, 1, Double, "h", DATE_TIME_NO_VAR(2017, 8, 19, 16, 0, 0), pVariates);
    ADD_PV_W_UNIT(MM, 30, Double, "g/umol", DATE_TIME_NO_VAR(2017, 8, 20, 16, 0, 0), pVariates);
    ADD_PV_W_UNIT(FR, 10, Double, "l/min", DATE_TIME_NO_VAR(2017, 8, 21, 16, 0, 0), pVariates);



    CovariateExtractor extractor(
            cDefinitions,
            pVariates,
            DATE_TIME_NO_VAR(2017, 8, 12, 14, 32, 0),
            DATE_TIME_NO_VAR(2017, 8, 22, 14, 32, 0));

    CovariateSeries cSeries;
    ComputingStatus rc;

    rc = extractor.extract(cSeries);

    ASSERT_EQ(rc, ComputingStatus::Ok);

    for (const auto& covariate : cSeries) {
        if (covariate.getId() == "Weight") {
            // Weight : mg -> kg
            ASSERT_DOUBLE_EQ(covariate.getValue(), 1);
        }
        else if (covariate.getId() == "Height") {
            // Height : mm -> cm
            ASSERT_DOUBLE_EQ(covariate.getValue(), 120);
        }
        else if (covariate.getId() == "Sex") {
            // Sex : no unit -> no unit
            ASSERT_EQ(covariate.getValue(), 1);
        }
        else if (covariate.getId() == "Gist") {
            // Gist : no unit -> no unit
            ASSERT_EQ(covariate.getValue(), true);
        }
        else if (covariate.getId() == "Scr") {
            // Scr : mmol/l -> umol/l
            ASSERT_DOUBLE_EQ(covariate.getValue(), 80);
        }
        else if (covariate.getId() == "CT") {
            // ConcentrationTime : min*ug/ml -> g*h/l
            ASSERT_DOUBLE_EQ(covariate.getValue(), 1.0 / 60000);
        }
        else if (covariate.getId() == "T") {
            // Time : h -> s
            ASSERT_DOUBLE_EQ(covariate.getValue(), 3600);
        }
        else if (covariate.getId() == "MM") {
            // MolarMass : g/umol -> kg/umol
            ASSERT_DOUBLE_EQ(covariate.getValue(), 0.03);
        }
        else if (covariate.getId() == "FR") {
            // FlowRate : l/min -> ml/h
            ASSERT_DOUBLE_EQ(covariate.getValue(), 600000);
        }
    }

    PatientVariates pVariates2;

    ADD_PV_W_UNIT(Weight, 100000.0, Double, "g", DATE_TIME_NO_VAR(2017, 8, 13, 16, 0, 0), pVariates2);
    //        ADD_PV_W_UNIT(Weight, 1000000000.0, Double, ug, DATE_TIME_NO_VAR(2017, 8, 14, 16, 0, 0), pVariates2);
    ADD_PV_W_UNIT(Height, 1200, Double, "dm", DATE_TIME_NO_VAR(2017, 8, 14, 16, 0, 0), pVariates2);
    ADD_PV_W_UNIT(Sex, 0, Int, "-", DATE_TIME_NO_VAR(2017, 8, 15, 16, 0, 0), pVariates2);
    ADD_PV_W_UNIT(Gist, true, Bool, "", DATE_TIME_NO_VAR(2017, 8, 16, 16, 0, 0), pVariates2);
    ADD_PV_W_UNIT(Scr, 0.08, Double, "umol/ml", DATE_TIME_NO_VAR(2017, 8, 17, 16, 0, 0), pVariates2);
    ADD_PV_W_UNIT(CT, 3600, Double, "mg*min/l", DATE_TIME_NO_VAR(2017, 8, 18, 16, 0, 0), pVariates2);
    ADD_PV_W_UNIT(T, 1, Double, "w", DATE_TIME_NO_VAR(2017, 8, 19, 16, 0, 0), pVariates2);
    ADD_PV_W_UNIT(MM, 30, Double, "kg/mol", DATE_TIME_NO_VAR(2017, 8, 20, 16, 0, 0), pVariates2);
    ADD_PV_W_UNIT(FR, 10, Double, "l/h", DATE_TIME_NO_VAR(2017, 8, 21, 16, 0, 0), pVariates2);



    CovariateExtractor extractor2(
            cDefinitions,
            pVariates2,
            DATE_TIME_NO_VAR(2017, 8, 12, 14, 32, 0),
            DATE_TIME_NO_VAR(2017, 8, 22, 14, 32, 0));

    CovariateSeries cSeries2;
    ComputingStatus rc2;

    rc2 = extractor2.extract(cSeries2);

    ASSERT_EQ(rc2, ComputingStatus::Ok);

    for (const auto& covariate : cSeries2) {
        if (covariate.getId() == "Weight") {
            // Weight : g -> kg
            ASSERT_DOUBLE_EQ(covariate.getValue(), 100);
        }
        else if (covariate.getId() == "Height") {
            // Height : dm -> cm
            ASSERT_DOUBLE_EQ(covariate.getValue(), 12000);
        }
        else if (covariate.getId() == "Sex") {
            // Sex : no unit -> no unit
            ASSERT_EQ(covariate.getValue(), 0);
        }
        else if (covariate.getId() == "Gist") {
            // Gist : no unit -> no unit
            ASSERT_EQ(covariate.getValue(), true);
        }
        else if (covariate.getId() == "Scr") {
            // Scr : umol/ml -> umol/l
            ASSERT_DOUBLE_EQ(covariate.getValue(), 80);
        }
        else if (covariate.getId() == "CT") {
            // ConcentrationTime : mg*min/l -> g*h/l
            ASSERT_DOUBLE_EQ(covariate.getValue(), 0.06);
        }
        else if (covariate.getId() == "T") {
            // Time : w -> s
            ASSERT_DOUBLE_EQ(covariate.getValue(), 604800);
        }
        else if (covariate.getId() == "MM") {
            // MolarMass : kg/mol -> kg/umol
            ASSERT_DOUBLE_EQ(covariate.getValue(), 0.00003);
        }
        else if (covariate.getId() == "FR") {
            // FlowRate : l/h -> ml/h
            ASSERT_DOUBLE_EQ(covariate.getValue(), 10000);
        }
    }
}

TEST(Core_TestCovariateExtractor, CovariateExtraction_test3_4)
{
    CovariateDefinitions cDefinitions;

    ADD_CDEF_W_R_UNIT(Weight, 40, Standard, Double, Linear, Tucuxi::Common::days(1), "g", cDefinitions);



    ASSERT_NO_THROW(CovariateExtractor(
            cDefinitions,
            PatientVariates(),
            DATE_TIME_NO_VAR(2017, 8, 12, 14, 32, 0),
            DATE_TIME_NO_VAR(2017, 8, 19, 14, 32, 0)));

    PatientVariates pVariates;

    ADD_PV_W_UNIT(Weight, 100.0, Double, "mg", DATE_TIME_NO_VAR(2017, 8, 13, 16, 0, 0), pVariates);
    ADD_PV_W_UNIT(Weight, 200000.0, Double, "ug", DATE_TIME_NO_VAR(2017, 8, 14, 16, 0, 0), pVariates);



    CovariateExtractor extractor(
            cDefinitions, pVariates, DATE_TIME_NO_VAR(2017, 8, 13, 15, 0, 0), DATE_TIME_NO_VAR(2017, 8, 14, 17, 0, 0));

    CovariateSeries cSeries;
    ComputingStatus rc;

    rc = extractor.extract(cSeries);

    ASSERT_EQ(rc, ComputingStatus::Ok);


    //        ASSERT_DOUBLE_EQ(c.getValue(), 100);
}
