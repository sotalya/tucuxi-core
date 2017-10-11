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


/// \brief Create a DateTime temporary variable (without creating an explicit named variable for this).
/// \param YY Year in 4 digits format.
/// \param MM Month in 2 digits format.
/// \param DD Day in 2 digits format.
/// \param hh Hour in 2 digits format.
/// \param mm Minutes in 2 digits format.
/// \param ss Seconds in 2 digits format.
#define DATE_TIME_NO_VAR(YY, MM, DD, hh, mm, ss)                                                            \
    Tucuxi::Common::DateTime(date::year_month_day(date::year(YY), date::month(MM), date::day(DD)),          \
    Tucuxi::Common::TimeOfDay(Tucuxi::Common::Duration(std::chrono::hours(hh),     \
    std::chrono::minutes(mm),   \
    std::chrono::seconds(ss))))


/// \brief Create a named DateTime variable.
/// \param YY Year in 4 digits format.
/// \param MM Month in 2 digits format.
/// \param DD Day in 2 digits format.
/// \param hh Hour in 2 digits format.
/// \param mm Minutes in 2 digits format.
/// \param ss Seconds in 2 digits format.
#define DATE_TIME_VAR(varName, YY, MM, DD, hh, mm, ss)                                                              \
    Tucuxi::Common::DateTime varName(date::year_month_day(date::year(YY), date::month(MM), date::day(DD)),          \
    Tucuxi::Common::TimeOfDay(Tucuxi::Common::Duration(std::chrono::hours(hh),     \
    std::chrono::minutes(mm),   \
    std::chrono::seconds(ss))))


/// \brief Add a covariate definition (without a refresh period or unit) to a given covariate definitions vector.
/// \param NAME Name of the covariate to add.
/// \param VALUE Default value of the covariate to add.
/// \param C_TYPE Type of the covariate (from CovariateType).
/// \param D_TYPE Data type of the covariate (from DataType).
/// \param INTER_TYPE Interpolation type for the values (from InterpolationType).
/// \param CD_VEC Covariate definitions vector in which the covariate has to be pushed.
#define ADD_CDEF_NO_R(NAME, VALUE, C_TYPE, D_TYPE, INTER_TYPE, CD_VEC)                                          \
    do {                                                                                                        \
    std::unique_ptr<CovariateDefinition> tmp(new CovariateDefinition(#NAME, valueToString(VALUE), nullptr,      \
    CovariateType::C_TYPE, DataType::D_TYPE)); \
    tmp->setInterpolationType(InterpolationType::INTER_TYPE);                                                   \
    CD_VEC.push_back(std::move(tmp));                                                                           \
    } while (0);


/// \brief Add a covariate definition (without a refresh period) to a given covariate definitions vector.
/// \param NAME Name of the covariate to add.
/// \param VALUE Default value of the covariate to add.
/// \param C_TYPE Type of the covariate (from CovariateType).
/// \param D_TYPE Data type of the covariate (from DataType).
/// \param INTER_TYPE Interpolation type for the values (from InterpolationType).
/// \param UNIT Unit of measure of the covariate.
/// \param CD_VEC Covariate definitions vector in which the covariate has to be pushed.
#define ADD_CDEF_NO_R_UNIT(NAME, VALUE, C_TYPE, D_TYPE, INTER_TYPE, UNIT, CD_VEC)                               \
    do {                                                                                                        \
    std::unique_ptr<CovariateDefinition> tmp(new CovariateDefinition(#NAME, valueToString(VALUE), nullptr,      \
    CovariateType::C_TYPE, DataType::D_TYPE)); \
    tmp->setInterpolationType(InterpolationType::INTER_TYPE);                                                   \
    tmp->->setUnit(Unit(#UNIT));                                                                                \
    CD_VEC.push_back(std::move(tmp));                                                                           \
    } while (0);


/// \brief Add a covariate definition (with a refresh period but without a unit) to a given covariate definitions
/// vector.
/// \param NAME Name of the covariate to add.
/// \param VALUE Default value of the covariate to add.
/// \param C_TYPE Type of the covariate (from CovariateType).
/// \param D_TYPE Data type of the covariate (from DataType).
/// \param INTER_TYPE Interpolation type for the values (from InterpolationType).
/// \param REFR_INT Refresh interval for the covariate.
/// \param CD_VEC Covariate definitions vector in which the covariate has to be pushed.
#define ADD_CDEF_W_R(NAME, VALUE, C_TYPE, D_TYPE, INTER_TYPE, REFR_INT, CD_VEC)                                 \
    do {                                                                                                        \
    std::unique_ptr<CovariateDefinition> tmp(new CovariateDefinition(#NAME, valueToString(VALUE), nullptr,      \
    CovariateType::C_TYPE, DataType::D_TYPE)); \
    tmp->setInterpolationType(InterpolationType::INTER_TYPE);                                                   \
    tmp->setRefreshPeriod(REFR_INT);                                                                            \
    CD_VEC.push_back(std::move(tmp));                                                                           \
    } while (0);


/// \brief Add a covariate definition (with both a refresh period and unit) to a given covariate definitions vector.
/// \param NAME Name of the covariate to add.
/// \param VALUE Default value of the covariate to add.
/// \param C_TYPE Type of the covariate (from CovariateType).
/// \param D_TYPE Data type of the covariate (from DataType).
/// \param INTER_TYPE Interpolation type for the values (from InterpolationType).
/// \param REFR_INT Refresh interval for the covariate.
/// \param UNIT Unit of measure of the covariate.
/// \param CD_VEC Covariate definitions vector in which the covariate has to be pushed.
#define ADD_CDEF_W_R_UNIT(NAME, VALUE, C_TYPE, D_TYPE, INTER_TYPE, REFR_INT, UNIT, CD_VEC)                      \
    do {                                                                                                        \
    std::unique_ptr<CovariateDefinition> tmp(new CovariateDefinition(#NAME, valueToString(VALUE), nullptr,      \
    CovariateType::C_TYPE, DataType::D_TYPE)); \
    tmp->setInterpolationType(InterpolationType::INTER_TYPE);                                                   \
    tmp->setRefreshPeriod(REFR_INT);                                                                            \
    tmp->setUnit(Unit(#UNIT));                                                                                  \
    CD_VEC.push_back(std::move(tmp));                                                                           \
    } while (0);


/// \brief Add a patient variate (without a specified unit of measure) to a given patient variates vector.
/// \param NAME Name of the patient variate to add.
/// \param VALUE Default value of the patient variate to add.
/// \param D_TYPE Data type of the patient variate (from DataType).
/// \param DATE Date of the measurement.
/// \param PV_VEC Patient variates vector in which the variate has to be pushed.
#define ADD_PV_NO_UNIT(NAME, VALUE, D_TYPE, DATE, PV_VEC)                                   \
    do {                                                                                    \
    std::unique_ptr<PatientCovariate> tmp(new PatientCovariate(#NAME, valueToString(VALUE), \
    DataType::D_TYPE, Unit(), DATE));                                                       \
    PV_VEC.push_back(std::move(tmp));                                                       \
    } while (0);


/// \brief Add a patient variate (with a specified unit of measure) to a given patient variates vector.
/// \param NAME Name of the patient variate to add.
/// \param VALUE Default value of the patient variate to add.
/// \param D_TYPE Data type of the patient variate (from DataType).
/// \param UNIT Unit of measure of the patient variate.
/// \param DATE Date of the measurement.
/// \param PV_VEC Patient variates vector in which the variate has to be pushed.
#define ADD_PV_W_UNIT(NAME, VALUE, D_TYPE, UNIT, DATE, PV_VEC)                                   \
    do {                                                                                    \
    std::unique_ptr<PatientCovariate> tmp(new PatientCovariate(#NAME, valueToString(VALUE), \
    DataType::D_TYPE, Unit(#UNIT), DATE));                                                       \
    PV_VEC.push_back(std::move(tmp));                                                       \
    } while (0);


/// \brief Add a computed covariate definition, with no refresh interval, to a given covariate definitions vector.
/// The operation has 3 inputs whose names are specified as parameters.
/// \param NAME Name of the covariate to add.
/// \param OPERATION Operation performed to get the values of the computed covariate.
/// \param OP1 Name of the first operand.
/// \param OP2 Name of the second operand.
/// \param OP3 Name of the third operand.
/// \param CD_VEC Covariate definitions vector in which the covariate has to be pushed.
#define ADD_OP3_CDEF_NO_R(NAME, OPERATION, OP1, OP2, OP3, CD_VEC) \
    do { \
    Operation *op = \
    new JSOperation(OPERATION, { \
    OperationInput(OP1, InputType::DOUBLE), \
    OperationInput(OP2, InputType::DOUBLE), \
    OperationInput(OP3, InputType::DOUBLE)}); \
    std::unique_ptr<CovariateDefinition> tmp(new CovariateDefinition(#NAME, valueToString(0), op)); \
    CD_VEC.push_back(std::move(tmp)); \
    } while (0);


/// \brief Add a computed covariate definition, with a refresh interval, to a given covariate definitions vector.
/// The operation has 3 inputs whose names are specified as parameters.
/// \param NAME Name of the covariate to add.
/// \param OPERATION Operation performed to get the values of the computed covariate.
/// \param OP1 Name of the first operand.
/// \param OP2 Name of the second operand.
/// \param OP3 Name of the third operand.
/// \param REFR_INT Refresh interval for the covariate.
/// \param CD_VEC Covariate definitions vector in which the covariate has to be pushed.
#define ADD_OP3_CDEF_W_R(NAME, OPERATION, OP1, OP2, OP3, REFR_INT, CD_VEC) \
    do { \
    Operation *op = \
    new JSOperation(OPERATION, { \
    OperationInput(OP1, InputType::DOUBLE), \
    OperationInput(OP2, InputType::DOUBLE), \
    OperationInput(OP3, InputType::DOUBLE)}); \
    std::unique_ptr<CovariateDefinition> tmp(new CovariateDefinition(#NAME, valueToString(0), op)); \
    tmp->setRefreshPeriod(REFR_INT); \
    CD_VEC.push_back(std::move(tmp)); \
    } while (0);


/// \brief Check a refresh map for the presence of a given covariate at a specified time.
/// \param NAME Name of the covariate to search.
/// \param DATE Time instant when the covariate is supposed to show up.
/// \param REFRESH_MAP Refresh map where the covariate has be to sought.
#define CHECK_CCV_REFRESH(NAME, DATE, REFRESH_MAP) \
    do { \
    fructose_assert(std::find(REFRESH_MAP[DATE].begin(), \
                              REFRESH_MAP[DATE].end(), #NAME) != REFRESH_MAP[DATE].end()); \
    } while (0);


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
                  << covEl.getEventTime().day() << "."
                  << covEl.getEventTime().month() << "."
                  << covEl.getEventTime().year() << " "
                  << covEl.getEventTime().hour() << "h"
                  << covEl.getEventTime().minute() << "m"
                  << covEl.getEventTime().second() << "s";
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
    }


    /// \brief Test the collectRefreshIntervals helper function.
    void testCE_collectRefreshIntervals(const std::string& /* _testName */)
    {
        CovariateDefinitions cDefinitions;
        PatientVariates pVariates;

        ADD_CDEF_NO_R(A1, 1.0, Standard, Double, Linear, cDefinitions);
        ADD_CDEF_NO_R(B2, 2.0, Standard, Double, Linear, cDefinitions);
        ADD_CDEF_NO_R(C3, 3.0, Standard, Double, Linear, cDefinitions);

        ADD_OP3_CDEF_W_R(op1, "A1 + B2 + C3", "A1", "B2", "C3", Tucuxi::Common::days(1), cDefinitions);
        ADD_OP3_CDEF_W_R(op2, "A1 + B2 * C3", "A1", "B2", "C3", Tucuxi::Common::days(2), cDefinitions);
        ADD_OP3_CDEF_W_R(op3, "A1 - B2 * C3", "A1", "B2", "C3", Tucuxi::Common::days(3), cDefinitions);

        CovariateExtractor extractor(cDefinitions, pVariates,
                                     DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0),
                                     DATE_TIME_NO_VAR(2017, 8, 23, 14, 0, 0));
        extractor.sortPatientVariates();
        std::map<std::string, std::shared_ptr<CovariateEvent>> nccValuesMap;
        CovariateSeries series;
        extractor.createNonComputedCEvents(nccValuesMap, series);
        std::map<std::string, std::pair<std::shared_ptr<CovariateEvent>, Value>> computedValuesMap;
        extractor.createComputedCEvents(computedValuesMap, series);

        std::map<DateTime, std::vector<std::string>> refreshMap;
        extractor.collectRefreshIntervals(computedValuesMap, refreshMap);

        fructose_assert(refreshMap.size() == 7);
        // 17.08.2017
        fructose_assert(refreshMap[DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0)].size() == 3);
        CHECK_CCV_REFRESH(op1, DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0), refreshMap);
        CHECK_CCV_REFRESH(op2, DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0), refreshMap);
        CHECK_CCV_REFRESH(op3, DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0), refreshMap);

        // 18.08.2017
        fructose_assert(refreshMap[DATE_TIME_NO_VAR(2017, 8, 18, 14, 0, 0)].size() == 1);
        CHECK_CCV_REFRESH(op1, DATE_TIME_NO_VAR(2017, 8, 18, 14, 0, 0), refreshMap);

        // 19.08.2017
        fructose_assert(refreshMap[DATE_TIME_NO_VAR(2017, 8, 19, 14, 0, 0)].size() == 2);
        CHECK_CCV_REFRESH(op1, DATE_TIME_NO_VAR(2017, 8, 19, 14, 0, 0), refreshMap);
        CHECK_CCV_REFRESH(op2, DATE_TIME_NO_VAR(2017, 8, 19, 14, 0, 0), refreshMap);

        // 20.08.2017
        fructose_assert(refreshMap[DATE_TIME_NO_VAR(2017, 8, 20, 14, 0, 0)].size() == 2);
        CHECK_CCV_REFRESH(op1, DATE_TIME_NO_VAR(2017, 8, 20, 14, 0, 0), refreshMap);
        CHECK_CCV_REFRESH(op3, DATE_TIME_NO_VAR(2017, 8, 20, 14, 0, 0), refreshMap);

        // 21.08.2017
        fructose_assert(refreshMap[DATE_TIME_NO_VAR(2017, 8, 21, 14, 0, 0)].size() == 2);
        CHECK_CCV_REFRESH(op1, DATE_TIME_NO_VAR(2017, 8, 21, 14, 0, 0), refreshMap);
        CHECK_CCV_REFRESH(op2, DATE_TIME_NO_VAR(2017, 8, 21, 14, 0, 0), refreshMap);

        // 22.08.2017
        fructose_assert(refreshMap[DATE_TIME_NO_VAR(2017, 8, 22, 14, 0, 0)].size() == 1);
        CHECK_CCV_REFRESH(op1, DATE_TIME_NO_VAR(2017, 8, 22, 14, 0, 0), refreshMap);

        // 23.08.2017
        fructose_assert(refreshMap[DATE_TIME_NO_VAR(2017, 8, 23, 14, 0, 0)].size() == 3);
        CHECK_CCV_REFRESH(op1, DATE_TIME_NO_VAR(2017, 8, 23, 14, 0, 0), refreshMap);
        CHECK_CCV_REFRESH(op2, DATE_TIME_NO_VAR(2017, 8, 23, 14, 0, 0), refreshMap);
        CHECK_CCV_REFRESH(op3, DATE_TIME_NO_VAR(2017, 8, 23, 14, 0, 0), refreshMap);
    }


    /// \brief Test the createComputedCEvents helper function.
    /// Check that:
    /// - the values of the computed covariates is correct given the non-computed covariate's default values.
    /// - the Operable Graph Manager contains the good values.
    /// - the computed values map contains the correct values.
    void testCE_createComputedCEvents(const std::string& /* _testName */)
    {
        CovariateDefinitions cDefinitions;
        PatientVariates pVariates;

        ADD_CDEF_NO_R(A1, 1.0, Standard, Double, Linear, cDefinitions);
        ADD_CDEF_NO_R(B2, 2.0, Standard, Double, Linear, cDefinitions);
        ADD_CDEF_NO_R(C3, 3.0, Standard, Double, Linear, cDefinitions);

        ADD_OP3_CDEF_NO_R(op1, "A1 + B2 + C3", "A1", "B2", "C3", cDefinitions);
        ADD_OP3_CDEF_NO_R(op2, "A1 + B2 * C3", "A1", "B2", "C3", cDefinitions);
        ADD_OP3_CDEF_NO_R(op3, "A1 - B2 * C3", "A1", "B2", "C3", cDefinitions);

        CovariateExtractor extractor(cDefinitions, pVariates,
                                     DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0),
                                     DATE_TIME_NO_VAR(2017, 8, 25, 14, 0, 0));
        extractor.sortPatientVariates();
        std::map<std::string, std::shared_ptr<CovariateEvent>> nccValuesMap;
        CovariateSeries series;
        extractor.createNonComputedCEvents(nccValuesMap, series);
        std::map<std::string, std::pair<std::shared_ptr<CovariateEvent>, Value>> computedValuesMap;
        extractor.createComputedCEvents(computedValuesMap, series);

        // Check that the expected events are present.
        fructose_assert(series.size() == 6);
        fructose_assert(covariateEventIsPresent("A1", DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0),
                                                1.0, series));
        fructose_assert(covariateEventIsPresent("B2", DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0),
                                                2.0, series));
        fructose_assert(covariateEventIsPresent("C3", DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0),
                                                3.0, series));
        fructose_assert(covariateEventIsPresent("op1", DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0),
                                                6.0, series));
        fructose_assert(covariateEventIsPresent("op2", DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0),
                                                7.0, series));
        fructose_assert(covariateEventIsPresent("op3", DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0),
                                                -5.0, series));

        // Check that the Operable Graph Manager contains the good values.
        bool rc;
        double val;

        rc = extractor.m_ogm.getValue("A1", val);
        fructose_assert(rc == true);
        fructose_assert(val == 1.0);

        rc = extractor.m_ogm.getValue("B2", val);
        fructose_assert(rc == true);
        fructose_assert(val == 2.0);

        rc = extractor.m_ogm.getValue("C3", val);
        fructose_assert(rc == true);
        fructose_assert(val == 3.0);

        rc = extractor.m_ogm.getValue("op1", val);
        fructose_assert(rc == true);
        fructose_assert(val == 6.0);

        rc = extractor.m_ogm.getValue("op2", val);
        fructose_assert(rc == true);
        fructose_assert(val == 7.0);

        rc = extractor.m_ogm.getValue("op3", val);
        fructose_assert(rc == true);
        fructose_assert(val == -5.0);

        // Check that the computedValuesMap has all the good values.
        fructose_assert(nccValuesMap.size() == 3);

        fructose_assert((computedValuesMap["op1"].first)->getId() == "op1");
        fructose_assert((computedValuesMap["op1"].first)->getValue() == 6.0);
        fructose_assert((computedValuesMap["op1"].first)->getEventTime() == DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0));
        fructose_assert(computedValuesMap["op1"].second == 6.0);

        fructose_assert((computedValuesMap["op2"].first)->getId() == "op2");
        fructose_assert((computedValuesMap["op2"].first)->getValue() == 7.0);
        fructose_assert((computedValuesMap["op2"].first)->getEventTime() == DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0));
        fructose_assert(computedValuesMap["op2"].second == 7.0);

        fructose_assert((computedValuesMap["op3"].first)->getId() == "op3");
        fructose_assert((computedValuesMap["op3"].first)->getValue() == -5.0);
        fructose_assert((computedValuesMap["op3"].first)->getEventTime() == DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0));
        fructose_assert(computedValuesMap["op3"].second == -5.0);
    }


    /// \brief Test the createNonComputedCEvents helper function.
    /// Test the following cases:
    /// - no patient variate associated -> take the default value.
    /// - a single patient variate value associated -> take it as initial value and keep it constant.
    /// - first available measurement past m_start (other measurements available afterwards) -> take the value of the
    ///   first measurement as initial value.
    /// - two measurements across m_start (one before, one after) -> use interpolation to get initial value.
    /// - two measurements across the whole interval -> use interpolation to get initial value.
    /// The corresponding events have to be created, and the covariates have to be registered in the Operable Graph
    /// Manager and in the map recording their values.
    void testCE_createNonComputedCEvents(const std::string& /* _testName */)
    {
        CovariateDefinitions cDefinitions;
        PatientVariates pVariates;

        ADD_CDEF_NO_R(NoPVAssociated, false, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_W_R_UNIT(SinglePVAssociated, 6.5, Standard, Double, Linear, Tucuxi::Common::days(1), kg, cDefinitions);
        ADD_CDEF_NO_R(StartPVAfterStart, 5, Standard, Int, Direct, cDefinitions);
        ADD_CDEF_NO_R(InterpPVDirect, 1.0, Standard, Double, Direct, cDefinitions);
        ADD_CDEF_NO_R(InterpPVLinear, 10.0, Standard, Double, Linear, cDefinitions);
        ADD_CDEF_NO_R(InterpPVDirectInterv, 0.0, Standard, Double, Direct, cDefinitions);
        ADD_CDEF_NO_R(InterpPVLinearInterv, 11.0, Standard, Double, Linear, cDefinitions);

        // No PV for NoPVAssociated.

        // Single PV for SinglePVAssociated.
        ADD_PV_NO_UNIT(SinglePVAssociated, true, Bool, DATE_TIME_NO_VAR(2017, 8, 19, 22, 32, 0), pVariates);

        // Multiple PVs for StartPVAfterStart, with the first after m_start.
        ADD_PV_NO_UNIT(StartPVAfterStart, 7, Int, DATE_TIME_NO_VAR(2017, 8, 19, 12, 32, 0), pVariates);
        ADD_PV_NO_UNIT(StartPVAfterStart, 9, Int, DATE_TIME_NO_VAR(2017, 8, 25, 12, 32, 0), pVariates);

        // Two values across m_start to interpolate from (direct).
        ADD_PV_NO_UNIT(InterpPVDirect, 7.4, Double, DATE_TIME_NO_VAR(2017, 8, 16, 14, 0, 0), pVariates);
        ADD_PV_NO_UNIT(InterpPVDirect, 7.6, Double, DATE_TIME_NO_VAR(2017, 8, 18, 14, 0, 0), pVariates);

        // Two values across m_start to interpolate from (linear).
        ADD_PV_NO_UNIT(InterpPVLinear, 7.6, Double, DATE_TIME_NO_VAR(2017, 8, 16, 14, 0, 0), pVariates);
        ADD_PV_NO_UNIT(InterpPVLinear, 7.4, Double, DATE_TIME_NO_VAR(2017, 8, 18, 14, 0, 0), pVariates);

        // Two values across the whole interval to interpolate from (direct).
        ADD_PV_NO_UNIT(InterpPVDirectInterv, 0.0, Double, DATE_TIME_NO_VAR(2017, 8, 16, 14, 0, 0), pVariates);
        ADD_PV_NO_UNIT(InterpPVDirectInterv, 10.0, Double, DATE_TIME_NO_VAR(2017, 8, 26, 14, 0, 0), pVariates);

        // Two values across the whole interval to interpolate from (linear).
        ADD_PV_NO_UNIT(InterpPVLinearInterv, 10.0, Double, DATE_TIME_NO_VAR(2017, 8, 16, 14, 0, 0), pVariates);
        ADD_PV_NO_UNIT(InterpPVLinearInterv, 0.0, Double, DATE_TIME_NO_VAR(2017, 8, 26, 14, 0, 0), pVariates);

        CovariateExtractor extractor(cDefinitions, pVariates,
                                     DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0),
                                     DATE_TIME_NO_VAR(2017, 8, 25, 14, 0, 0));
        extractor.sortPatientVariates();
        std::map<std::string, std::shared_ptr<CovariateEvent>> nccValuesMap;
        CovariateSeries series;
        extractor.createNonComputedCEvents(nccValuesMap, series);

        // Check that the expected events are present.
        fructose_assert(series.size() == 7);
        // NoPVAssociated.
        fructose_assert(covariateEventIsPresent("NoPVAssociated", DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0),
                                                false, series));
        // SinglePVAssociated.
        fructose_assert(covariateEventIsPresent("SinglePVAssociated", DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0),
                                                true, series));
        // StartPVAfterStart.
        fructose_assert(covariateEventIsPresent("StartPVAfterStart", DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0),
                                                7, series));
        // InterpPVDirect.
        fructose_assert(covariateEventIsPresent("InterpPVDirect", DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0),
                                                7.4, series));
        // InterpPVLinear
        fructose_assert(covariateEventIsPresent("InterpPVLinear", DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0),
                                                7.5, series));
        // InterpPVDirectInterv
        fructose_assert(covariateEventIsPresent("InterpPVDirectInterv", DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0),
                                                0.0, series));
        // InterpPVLinearInterv
        fructose_assert(covariateEventIsPresent("InterpPVLinearInterv", DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0),
                                                9.0, series));

        // Check that the Operable Graph Manager contains the good values.
        bool rc;
        double val;

        rc = extractor.m_ogm.getValue("NoPVAssociated", val);
        fructose_assert(rc == true);
        fructose_assert(val == 0.0);

        rc = extractor.m_ogm.getValue("SinglePVAssociated", val);
        fructose_assert(rc == true);
        fructose_assert(val == 1.0);

        rc = extractor.m_ogm.getValue("StartPVAfterStart", val);
        fructose_assert(rc == true);
        fructose_assert(val == 7.0);

        rc = extractor.m_ogm.getValue("InterpPVDirect", val);
        fructose_assert(rc == true);
        fructose_assert(val == 7.4);

        rc = extractor.m_ogm.getValue("InterpPVLinear", val);
        fructose_assert(rc == true);
        fructose_assert(val == 7.5);

        rc = extractor.m_ogm.getValue("InterpPVDirectInterv", val);
        fructose_assert(rc == true);
        fructose_assert(val == 0.0);

        rc = extractor.m_ogm.getValue("InterpPVLinearInterv", val);
        fructose_assert(rc == true);
        fructose_assert(val == 9.0);

        // Check that the nccValuesMap has all the good values.
        fructose_assert(nccValuesMap.size() == 7);

        fructose_assert(nccValuesMap["NoPVAssociated"]->getId() == "NoPVAssociated");
        fructose_assert(nccValuesMap["NoPVAssociated"]->getValue() == 0.0);
        fructose_assert(nccValuesMap["NoPVAssociated"]->getEventTime() == DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0));

        fructose_assert(nccValuesMap["SinglePVAssociated"]->getId() == "SinglePVAssociated");
        fructose_assert(nccValuesMap["SinglePVAssociated"]->getValue() == 1.0);
        fructose_assert(nccValuesMap["SinglePVAssociated"]->getEventTime() == DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0));

        fructose_assert(nccValuesMap["StartPVAfterStart"]->getId() == "StartPVAfterStart");
        fructose_assert(nccValuesMap["StartPVAfterStart"]->getValue() == 7.0);
        fructose_assert(nccValuesMap["StartPVAfterStart"]->getEventTime() == DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0));

        fructose_assert(nccValuesMap["InterpPVDirect"]->getId() == "InterpPVDirect");
        fructose_assert(nccValuesMap["InterpPVDirect"]->getValue() == 7.4);
        fructose_assert(nccValuesMap["InterpPVDirect"]->getEventTime() == DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0));

        fructose_assert(nccValuesMap["InterpPVLinear"]->getId() == "InterpPVLinear");
        fructose_assert(nccValuesMap["InterpPVLinear"]->getValue() == 7.5);
        fructose_assert(nccValuesMap["InterpPVLinear"]->getEventTime() == DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0));

        fructose_assert(nccValuesMap["InterpPVDirectInterv"]->getId() == "InterpPVDirectInterv");
        fructose_assert(nccValuesMap["InterpPVDirectInterv"]->getValue() == 0.0);
        fructose_assert(nccValuesMap["InterpPVDirectInterv"]->getEventTime() == DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0));

        fructose_assert(nccValuesMap["InterpPVLinearInterv"]->getId() == "InterpPVLinearInterv");
        fructose_assert(nccValuesMap["InterpPVLinearInterv"]->getValue() == 9.0);
        fructose_assert(nccValuesMap["InterpPVLinearInterv"]->getEventTime() == DATE_TIME_NO_VAR(2017, 8, 17, 14, 0, 0));
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
            res_pvVec.push_back(std::unique_ptr<PatientCovariate>(new PatientCovariate("Weight", valueToString(7.6), DataType::Double, Unit(), DATE_TIME_NO_VAR(2017, 8, 12, 12, 32, 0))));
            res_pvVec.push_back(std::unique_ptr<PatientCovariate>(new PatientCovariate("Weight", valueToString(7.2), DataType::Double, Unit(), DATE_TIME_NO_VAR(2017, 8, 19, 12, 32, 0))));
            res_pvVec.push_back(std::unique_ptr<PatientCovariate>(new PatientCovariate("Weight", valueToString(6.3), DataType::Double, Unit(), DATE_TIME_NO_VAR(2017, 8, 19, 22, 32, 0))));
            res_pvVec.push_back(std::unique_ptr<PatientCovariate>(new PatientCovariate("Weight", valueToString(7.4), DataType::Double, Unit(), DATE_TIME_NO_VAR(2017, 8, 21, 12, 32, 0))));
            res_pvVec.push_back(std::unique_ptr<PatientCovariate>(new PatientCovariate("Weight", valueToString(7.0), DataType::Double, Unit(), DATE_TIME_NO_VAR(2017, 8, 30, 12, 32, 0))));

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
            res_pvVec.push_back(std::unique_ptr<PatientCovariate>(new PatientCovariate("Weight", valueToString(7.9), DataType::Double, Unit(), DATE_TIME_NO_VAR(2017, 8, 11, 12, 32, 0))));
            res_pvVec.push_back(std::unique_ptr<PatientCovariate>(new PatientCovariate("Weight", valueToString(7.6), DataType::Double, Unit(), DATE_TIME_NO_VAR(2017, 8, 12, 12, 32, 0))));
            res_pvVec.push_back(std::unique_ptr<PatientCovariate>(new PatientCovariate("Weight", valueToString(7.2), DataType::Double, Unit(), DATE_TIME_NO_VAR(2017, 8, 19, 12, 32, 0))));
            res_pvVec.push_back(std::unique_ptr<PatientCovariate>(new PatientCovariate("Weight", valueToString(6.3), DataType::Double, Unit(), DATE_TIME_NO_VAR(2017, 8, 19, 22, 32, 0))));
            res_pvVec.push_back(std::unique_ptr<PatientCovariate>(new PatientCovariate("Weight", valueToString(7.4), DataType::Double, Unit(), DATE_TIME_NO_VAR(2017, 8, 21, 12, 32, 0))));
            res_pvVec.push_back(std::unique_ptr<PatientCovariate>(new PatientCovariate("Weight", valueToString(7.0), DataType::Double, Unit(), DATE_TIME_NO_VAR(2017, 8, 30, 12, 32, 0))));
            res_pvVec.push_back(std::unique_ptr<PatientCovariate>(new PatientCovariate("Weight", valueToString(7.8), DataType::Double, Unit(), DATE_TIME_NO_VAR(2017, 8, 30, 16, 32, 0))));

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
            res_pvVec.push_back(std::unique_ptr<PatientCovariate>(new PatientCovariate("Weight", valueToString(7.9), DataType::Double, Unit(), DATE_TIME_NO_VAR(2017, 8, 11, 12, 32, 0))));
            res_pvVec.push_back(std::unique_ptr<PatientCovariate>(new PatientCovariate("Weight", valueToString(7.6), DataType::Double, Unit(), DATE_TIME_NO_VAR(2017, 8, 12, 12, 32, 0))));
            res_pvVec.push_back(std::unique_ptr<PatientCovariate>(new PatientCovariate("Weight", valueToString(7.2), DataType::Double, Unit(), DATE_TIME_NO_VAR(2017, 8, 19, 12, 32, 0))));
            res_pvVec.push_back(std::unique_ptr<PatientCovariate>(new PatientCovariate("Weight", valueToString(6.3), DataType::Double, Unit(), DATE_TIME_NO_VAR(2017, 8, 19, 22, 32, 0))));
            res_pvVec.push_back(std::unique_ptr<PatientCovariate>(new PatientCovariate("Weight", valueToString(7.4), DataType::Double, Unit(), DATE_TIME_NO_VAR(2017, 8, 21, 12, 32, 0))));
            res_pvVec.push_back(std::unique_ptr<PatientCovariate>(new PatientCovariate("Weight", valueToString(7.0), DataType::Double, Unit(), DATE_TIME_NO_VAR(2017, 8, 30, 12, 32, 0))));
            res_pvVec.push_back(std::unique_ptr<PatientCovariate>(new PatientCovariate("Weight", valueToString(7.8), DataType::Double, Unit(), DATE_TIME_NO_VAR(2017, 8, 30, 16, 32, 0))));

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
            res_pvVec.push_back(std::unique_ptr<PatientCovariate>(new PatientCovariate("Weight", valueToString(6.3), DataType::Double, Unit(), DATE_TIME_NO_VAR(2017, 8, 9, 22, 32, 0))));
            res_pvVec.push_back(std::unique_ptr<PatientCovariate>(new PatientCovariate("Weight", valueToString(7.2), DataType::Double, Unit(), DATE_TIME_NO_VAR(2017, 8, 12, 12, 32, 0))));

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
            res_pvVec.push_back(std::unique_ptr<PatientCovariate>(new PatientCovariate("Weight", valueToString(7.2), DataType::Double, Unit(), DATE_TIME_NO_VAR(2017, 8, 19, 12, 32, 0))));
            res_pvVec.push_back(std::unique_ptr<PatientCovariate>(new PatientCovariate("Weight", valueToString(6.3), DataType::Double, Unit(), DATE_TIME_NO_VAR(2017, 8, 19, 22, 32, 0))));

            const auto &pvVals = extractor.m_pvValued.at("Weight");

            fructose_assert(pvVals.size() == res_pvVec.size());
            for (size_t i = 0; i < std::min(pvVals.size(), res_pvVec.size()); ++i) {
                fructose_assert(**(pvVals.at(i)) == *(res_pvVec.at(i)));
            }
        }
    }


    /// \brief Test covariate extraction using the example presented in the specs.
    void testCovariateExtraction_test1(const std::string& /* _testName */)
    {
        CovariateDefinitions cDefinitions;

        ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), kg, cDefinitions);
        ADD_CDEF_NO_R(IsMale, true, Standard, Bool, Direct, cDefinitions);

        /// \todo Change the plain-old-pointer below when unique_ptr will be in place!
        Operation *opSpecial =
                new JSOperation("Weight * 0.5 + IsMale * 15",
        { OperationInput("Weight", InputType::DOUBLE), OperationInput("IsMale", InputType::DOUBLE)});
        std::unique_ptr<CovariateDefinition> special(new CovariateDefinition("Special", valueToString(16.75), opSpecial));
        cDefinitions.push_back(std::move(special));

        const DATE_TIME_VAR(startDate, 2017, 8, 12, 8, 0, 0);
        const DATE_TIME_VAR(endDate,   2017, 8, 17, 8, 0, 0);

        // -- Patient covariates #1 --
        PatientVariates pVariates1;
        // gist == true @ 13.08.2017, 12h32.
        ADD_PV_NO_UNIT(Gist, true, Bool, DATE_TIME_NO_VAR(2017, 8, 13, 12, 32, 0), pVariates1);
        // gist == false @ 13.08.2017, 14h32.
        ADD_PV_NO_UNIT(Gist, false, Bool, DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0), pVariates1);

        // Test 1: call the extractor with the two patient covariates above.
        // We expect the first one to be "back-propagated" to the beginning of the interval.

        CovariateExtractor extractor1(cDefinitions, pVariates1, startDate, endDate);
        CovariateSeries series;
        int rc;
        rc = extractor1.extract(series);

        //        printCovariateSeries(series);

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

        PatientVariates pVariates2;
        CovariateExtractor extractor2(cDefinitions, pVariates2, startDate, endDate);
        series.clear();
        rc = extractor2.extract(series);

        //        printCovariateSeries(series);

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

        // gist == true @ 13.08.2017, 12h32.
        std::unique_ptr<PatientCovariate> patient_gist_3(new PatientCovariate("Gist", valueToString(true),
                                                                              DataType::Bool, Unit("kg"),
                                                                              DATE_TIME_NO_VAR(2017, 8, 13, 12, 32, 0)));

        // weight = 3.8 @ 13.08.2017, 9h00.
        std::unique_ptr<PatientCovariate> patient_weight_1(new PatientCovariate("Weight", valueToString(3.8),
                                                                                DataType::Double, Unit("kg"),
                                                                                DATE_TIME_NO_VAR(2017, 8, 13, 9, 0, 0)));
        // weight = 4.05 @ 15.08.2017, 21h00.
        std::unique_ptr<PatientCovariate> patient_weight_2(new PatientCovariate("Weight", valueToString(4.05),
                                                                                DataType::Double, Unit("kg"),
                                                                                DATE_TIME_NO_VAR(2017, 8, 15, 21, 0, 0)));
        // weight = 4.25 @ 16.08.2017, 21h00.
        std::unique_ptr<PatientCovariate> patient_weight_3(new PatientCovariate("Weight", valueToString(4.25),
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


    /// \brief Perform the second test in the spreadsheet.
    /// - Different end time (changes wrt weight periodicity).
    /// - Added a date variable.
    /// - Added another computed covariate (VerySpecial)
    void testCovariateExtraction_test2(const std::string& /* _testName */)
    {
        std::cerr << "++++++++++++++ TEST2 +++++++++++++++\n";

        std::unique_ptr<CovariateDefinition> gist(new CovariateDefinition("Gist", valueToString(false), nullptr,
                                                                          CovariateType::Standard, DataType::Bool));
        gist->setInterpolationType(InterpolationType::Direct);

        std::unique_ptr<CovariateDefinition> weight(new CovariateDefinition("Weight", valueToString(3.5), nullptr));
        weight->setRefreshPeriod(Tucuxi::Common::days(1));
        weight->setInterpolationType(InterpolationType::Linear);
        weight->setUnit(Unit("kg"));

        std::unique_ptr<CovariateDefinition> isMale(new CovariateDefinition("IsMale", valueToString(true), nullptr,
                                                                            CovariateType::Standard, DataType::Bool));

        std::unique_ptr<CovariateDefinition> birthDate(new CovariateDefinition("BirthDate",
                                                                               valueToString(DATE_TIME_NO_VAR(2017, 8, 5, 8, 0, 0)),
                                                                               nullptr,
                                                                               CovariateType::Standard,
                                                                               DataType::Date));

        /// \todo Change the plain-old-pointers below when unique_ptr will be in place!
        Operation *opSpecial =
                new JSOperation("Weight * 0.5 + IsMale * 15",
        { OperationInput("Weight", InputType::DOUBLE),
          OperationInput("IsMale", InputType::DOUBLE)});
        std::unique_ptr<CovariateDefinition> special(new CovariateDefinition("Special", valueToString(16.75), opSpecial));

        Operation *opVerySpecial =
                new JSOperation("Weight * 0.5 + IsMale * 15 + Gist * 25",
        { OperationInput("Weight", InputType::DOUBLE),
          OperationInput("IsMale", InputType::DOUBLE),
          OperationInput("Gist", InputType::DOUBLE)});
        std::unique_ptr<CovariateDefinition> verySpecial(new CovariateDefinition("VerySpecial", valueToString(16.75), opVerySpecial));

        CovariateDefinitions cDefinitions;
        cDefinitions.push_back(std::move(gist));
        cDefinitions.push_back(std::move(weight));
        cDefinitions.push_back(std::move(isMale));
        cDefinitions.push_back(std::move(birthDate));
        cDefinitions.push_back(std::move(special));
        cDefinitions.push_back(std::move(verySpecial));

        const DATE_TIME_VAR(startDate, 2017, 8, 12, 8, 0, 0);
        const DATE_TIME_VAR(endDate,   2017, 8, 17, 21, 0, 0);

        // gist == true @ 13.08.2017, 12h32.
        std::unique_ptr<PatientCovariate> patient_gist_1(new PatientCovariate("Gist", valueToString(true),
                                                                              DataType::Bool, Unit(),
                                                                              DATE_TIME_NO_VAR(2017, 8, 13, 12, 32, 0)));
        // gist == false @ 13.08.2017, 14h32.
        std::unique_ptr<PatientCovariate> patient_gist_2(new PatientCovariate("Gist", valueToString(false),
                                                                              DataType::Bool, Unit(),
                                                                              DATE_TIME_NO_VAR(2017, 8, 13, 14, 32, 0)));
        PatientVariates pVariates1;
        pVariates1.push_back(std::move(patient_gist_1));
        pVariates1.push_back(std::move(patient_gist_2));

        CovariateExtractor extractor1(cDefinitions, pVariates1, startDate, endDate);
        CovariateSeries series;
        int rc;
        rc = extractor1.extract(series);

        double _valRes;
        std::cerr << extractor1.interpolateValues(1, DATE_TIME_NO_VAR(2017, 8, 13, 12, 32, 0),
                                                  3, DATE_TIME_NO_VAR(2017, 8, 15, 12, 32, 0),
                                                  DATE_TIME_NO_VAR(2017, 8, 14, 12, 32, 0),
                                                  InterpolationType::Linear,
                                                  _valRes) << " <<<<<\n";
        std::cerr << _valRes << "\n";


        printCovariateSeries(series);

        fructose_assert(series.size() == 8);
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
        fructose_assert(covariateEventIsPresent("BirthDate",
                                                DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0),
                                                stringToValue("2017-08-05 08:00:00", DataType::Date),
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
    }
};

#endif // TEST_COVARIATEEXTRACTOR_H
