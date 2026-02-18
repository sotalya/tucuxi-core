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


#include <sstream>
#include <string>

#include <gtest/gtest.h>

#include "tucucommon/utils.h"

#include "tucucore/definitions.h"
#include "tucucore/drugtreatment/patientcovariate.h"

#include "testutils.h"

using namespace Tucuxi::Core;


// ============================================================
// Construction and getters
// ============================================================

TEST(Core_TestPatientCovariate, ConstructionDouble)
{
    // All properties passed to the constructor must be retrievable.
    Tucuxi::Common::DateTime date = DATE_TIME_NO_VAR(2021, 3, 15, 8, 0, 0);
    PatientCovariate cov("weight", "70.5", DataType::Double, TucuUnit("kg"), date);

    EXPECT_EQ(cov.getId(), "weight");
    EXPECT_EQ(cov.getValue(), "70.5");
    EXPECT_EQ(cov.getDataType(), DataType::Double);
    EXPECT_EQ(cov.getUnit().toString(), TucuUnit("kg").toString());
    EXPECT_EQ(cov.getEventTime(), date);
}

TEST(Core_TestPatientCovariate, ConstructionInteger)
{
    // DataType::Int must be stored and returned correctly.
    Tucuxi::Common::DateTime date = DATE_TIME_NO_VAR(2020, 1, 1, 0, 0, 0);
    PatientCovariate cov("age", "42", DataType::Int, TucuUnit(""), date);

    EXPECT_EQ(cov.getId(), "age");
    EXPECT_EQ(cov.getValue(), "42");
    EXPECT_EQ(cov.getDataType(), DataType::Int);
}

TEST(Core_TestPatientCovariate, ConstructionBool)
{
    // DataType::Bool must be stored and returned correctly.
    Tucuxi::Common::DateTime date = DATE_TIME_NO_VAR(2020, 6, 1, 0, 0, 0);
    PatientCovariate cov("male", "1", DataType::Bool, TucuUnit(""), date);

    EXPECT_EQ(cov.getId(), "male");
    EXPECT_EQ(cov.getValue(), "1");
    EXPECT_EQ(cov.getDataType(), DataType::Bool);
}

TEST(Core_TestPatientCovariate, GetId)
{
    // getId must return the identifier string exactly.
    Tucuxi::Common::DateTime date = DATE_TIME_NO_VAR(2021, 1, 1, 0, 0, 0);
    PatientCovariate cov("creatinine", "1.2", DataType::Double, TucuUnit("mg/dl"), date);
    EXPECT_EQ(cov.getId(), "creatinine");
}

TEST(Core_TestPatientCovariate, GetValue)
{
    // getValue must return the raw string value stored in the covariate.
    Tucuxi::Common::DateTime date = DATE_TIME_NO_VAR(2021, 1, 1, 0, 0, 0);
    PatientCovariate cov("height", "175", DataType::Double, TucuUnit("cm"), date);
    EXPECT_EQ(cov.getValue(), "175");
}

TEST(Core_TestPatientCovariate, GetEventTime)
{
    // getEventTime must return the date passed to the constructor.
    Tucuxi::Common::DateTime date = DATE_TIME_NO_VAR(2022, 7, 4, 12, 0, 0);
    PatientCovariate cov("weight", "80", DataType::Double, TucuUnit("kg"), date);
    EXPECT_EQ(cov.getEventTime(), date);
}

TEST(Core_TestPatientCovariate, GetUnit)
{
    // getUnit must return the unit passed to the constructor.
    Tucuxi::Common::DateTime date = DATE_TIME_NO_VAR(2021, 1, 1, 0, 0, 0);
    TucuUnit unit("mg/dl");
    PatientCovariate cov("creatinine", "1.1", DataType::Double, unit, date);
    EXPECT_EQ(cov.getUnit().toString(), unit.toString());
}


// ============================================================
// setEventTime
// ============================================================

TEST(Core_TestPatientCovariate, SetEventTime)
{
    // setEventTime must update the event time returned by getEventTime.
    Tucuxi::Common::DateTime original = DATE_TIME_NO_VAR(2021, 1, 1, 0, 0, 0);
    Tucuxi::Common::DateTime updated = DATE_TIME_NO_VAR(2021, 6, 15, 10, 30, 0);

    PatientCovariate cov("weight", "70", DataType::Double, TucuUnit("kg"), original);
    ASSERT_EQ(cov.getEventTime(), original);

    cov.setEventTime(updated);
    EXPECT_EQ(cov.getEventTime(), updated);
}


// ============================================================
// setValueAsDate / getValueAsDate
// ============================================================

TEST(Core_TestPatientCovariate, SetAndGetValueAsDate)
{
    // setValueAsDate must store the date so that getValueAsDate returns the
    // same date (up to the precision of the DateTime round-trip through string).
    Tucuxi::Common::DateTime birthdate = DATE_TIME_NO_VAR(1985, 4, 12, 0, 0, 0);
    Tucuxi::Common::DateTime eventDate = DATE_TIME_NO_VAR(2021, 1, 1, 0, 0, 0);

    PatientCovariate cov("birthdate", "", DataType::Date, TucuUnit(""), eventDate);
    cov.setValueAsDate(birthdate);

    Tucuxi::Common::DateTime retrieved = cov.getValueAsDate();
    EXPECT_EQ(retrieved, birthdate);
}


// ============================================================
// Equality / inequality operators
// ============================================================

TEST(Core_TestPatientCovariate, EqualitySameObject)
{
    // A covariate must compare equal to itself.
    Tucuxi::Common::DateTime date = DATE_TIME_NO_VAR(2021, 1, 1, 0, 0, 0);
    PatientCovariate cov("weight", "70", DataType::Double, TucuUnit("kg"), date);
    EXPECT_TRUE(cov == cov);
}

TEST(Core_TestPatientCovariate, EqualityIdenticalCovariates)
{
    // Two covariates built with the same arguments must compare equal.
    Tucuxi::Common::DateTime date = DATE_TIME_NO_VAR(2021, 3, 10, 8, 0, 0);
    PatientCovariate cov1("weight", "70", DataType::Double, TucuUnit("kg"), date);
    PatientCovariate cov2("weight", "70", DataType::Double, TucuUnit("kg"), date);
    EXPECT_TRUE(cov1 == cov2);
    EXPECT_FALSE(cov1 != cov2);
}

TEST(Core_TestPatientCovariate, InequalityDifferentId)
{
    // Covariates with different ids must not compare equal.
    Tucuxi::Common::DateTime date = DATE_TIME_NO_VAR(2021, 1, 1, 0, 0, 0);
    PatientCovariate cov1("weight", "70", DataType::Double, TucuUnit("kg"), date);
    PatientCovariate cov2("height", "70", DataType::Double, TucuUnit("kg"), date);
    EXPECT_FALSE(cov1 == cov2);
    EXPECT_TRUE(cov1 != cov2);
}

TEST(Core_TestPatientCovariate, InequalityDifferentValue)
{
    // Covariates with different values must not compare equal.
    Tucuxi::Common::DateTime date = DATE_TIME_NO_VAR(2021, 1, 1, 0, 0, 0);
    PatientCovariate cov1("weight", "70", DataType::Double, TucuUnit("kg"), date);
    PatientCovariate cov2("weight", "80", DataType::Double, TucuUnit("kg"), date);
    EXPECT_FALSE(cov1 == cov2);
    EXPECT_TRUE(cov1 != cov2);
}

TEST(Core_TestPatientCovariate, InequalityDifferentDataType)
{
    // Covariates with different data types must not compare equal.
    Tucuxi::Common::DateTime date = DATE_TIME_NO_VAR(2021, 1, 1, 0, 0, 0);
    PatientCovariate cov1("flag", "1", DataType::Bool, TucuUnit(""), date);
    PatientCovariate cov2("flag", "1", DataType::Int, TucuUnit(""), date);
    EXPECT_FALSE(cov1 == cov2);
    EXPECT_TRUE(cov1 != cov2);
}

TEST(Core_TestPatientCovariate, InequalityDifferentDate)
{
    // Covariates recorded at different times must not compare equal.
    Tucuxi::Common::DateTime date1 = DATE_TIME_NO_VAR(2021, 1, 1, 0, 0, 0);
    Tucuxi::Common::DateTime date2 = DATE_TIME_NO_VAR(2021, 6, 1, 0, 0, 0);
    PatientCovariate cov1("weight", "70", DataType::Double, TucuUnit("kg"), date1);
    PatientCovariate cov2("weight", "70", DataType::Double, TucuUnit("kg"), date2);
    EXPECT_FALSE(cov1 == cov2);
    EXPECT_TRUE(cov1 != cov2);
}


// ============================================================
// Stream operator
// ============================================================

TEST(Core_TestPatientCovariate, StreamOperatorContainsId)
{
    // The stream output must contain the covariate id.
    Tucuxi::Common::DateTime date = DATE_TIME_NO_VAR(2021, 1, 1, 0, 0, 0);
    PatientCovariate cov("weight", "70", DataType::Double, TucuUnit("kg"), date);
    std::ostringstream oss;
    oss << cov;
    EXPECT_NE(oss.str().find("weight"), std::string::npos);
}

TEST(Core_TestPatientCovariate, StreamOperatorContainsValue)
{
    // The stream output must contain the stored string value.
    Tucuxi::Common::DateTime date = DATE_TIME_NO_VAR(2021, 1, 1, 0, 0, 0);
    PatientCovariate cov("height", "175", DataType::Double, TucuUnit("cm"), date);
    std::ostringstream oss;
    oss << cov;
    EXPECT_NE(oss.str().find("175"), std::string::npos);
}
