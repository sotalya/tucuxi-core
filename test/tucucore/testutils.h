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


#ifndef TESTUTILS_H
#define TESTUTILS_H

#include "tucucommon/timeofday.h"

/// \brief Create a DateTime temporary variable (without creating an explicit named variable for this).
/// \param YY Year in 4 digits format.
/// \param MM Month in 2 digits format.
/// \param DD Day in 2 digits format.
/// \param hh Hour in 2 digits format.
/// \param mm Minutes in 2 digits format.
/// \param ss Seconds in 2 digits format.
#define DATE_TIME_NO_VAR(YY, MM, DD, hh, mm, ss)                                  \
    Tucuxi::Common::DateTime(                                                     \
            date::year_month_day(date::year(YY), date::month(MM), date::day(DD)), \
            Tucuxi::Common::TimeOfDay(Tucuxi::Common::Duration(                   \
                    std::chrono::hours(hh), std::chrono::minutes(mm), std::chrono::seconds(ss))))


/// \brief Create a named DateTime variable.
/// \param YY Year in 4 digits format.
/// \param MM Month in 2 digits format.
/// \param DD Day in 2 digits format.
/// \param hh Hour in 2 digits format.
/// \param mm Minutes in 2 digits format.
/// \param ss Seconds in 2 digits format.
#define DATE_TIME_VAR(varName, YY, MM, DD, hh, mm, ss)                            \
    Tucuxi::Common::DateTime varName(                                             \
            date::year_month_day(date::year(YY), date::month(MM), date::day(DD)), \
            Tucuxi::Common::TimeOfDay(Tucuxi::Common::Duration(                   \
                    std::chrono::hours(hh), std::chrono::minutes(mm), std::chrono::seconds(ss))))


/// \brief Add a covariate definition (without a refresh period or unit) to a given covariate definitions vector.
/// \param NAME Name of the covariate to add.
/// \param VALUE Default value of the covariate to add.
/// \param C_TYPE Type of the covariate (from CovariateType).
/// \param D_TYPE Data type of the covariate (from DataType).
/// \param INTER_TYPE Interpolation type for the values (from InterpolationType).
/// \param CD_VEC Covariate definitions vector in which the covariate has to be pushed.
#define ADD_CDEF_NO_R(NAME, VALUE, C_TYPE, D_TYPE, INTER_TYPE, CD_VEC)                                               \
    do {                                                                                                             \
        auto tmp = std::make_unique<CovariateDefinition>(                                                            \
                #NAME, Tucuxi::Common::Utils::varToString(VALUE), nullptr, CovariateType::C_TYPE, DataType::D_TYPE); \
        tmp->setInterpolationType(InterpolationType::INTER_TYPE);                                                    \
        CD_VEC.push_back(std::move(tmp));                                                                            \
    } while (0);


/// \brief Add a covariate definition (without a refresh period or unit) to a given covariate definitions vector,
///        starting from an std::string.
/// \param NAME Name of the covariate to add.
/// \param VALUE Default value of the covariate to add.
/// \param C_TYPE Type of the covariate (from CovariateType).
/// \param D_TYPE Data type of the covariate (from DataType).
/// \param INTER_TYPE Interpolation type for the values (from InterpolationType).
/// \param CD_VEC Covariate definitions vector in which the covariate has to be pushed.
#define ADD_CDEF_NO_R_STR(NAME, VALUE, C_TYPE, D_TYPE, INTER_TYPE, CD_VEC)                                          \
    do {                                                                                                            \
        auto tmp = std::make_unique<CovariateDefinition>(                                                           \
                NAME, Tucuxi::Common::Utils::varToString(VALUE), nullptr, CovariateType::C_TYPE, DataType::D_TYPE); \
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
#define ADD_CDEF_NO_R_UNIT(NAME, VALUE, C_TYPE, D_TYPE, INTER_TYPE, UNIT, CD_VEC)                                    \
    do {                                                                                                             \
        auto tmp = std::make_unique<CovariateDefinition>(                                                            \
                #NAME, Tucuxi::Common::Utils::varToString(VALUE), nullptr, CovariateType::C_TYPE, DataType::D_TYPE); \
        tmp->setInterpolationType(InterpolationType::INTER_TYPE);                                                    \
        tmp->setUnit(Unit(UNIT));                                                                                    \
        CD_VEC.push_back(std::move(tmp));                                                                            \
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
#define ADD_CDEF_W_R(NAME, VALUE, C_TYPE, D_TYPE, INTER_TYPE, REFR_INT, CD_VEC)                                      \
    do {                                                                                                             \
        auto tmp = std::make_unique<CovariateDefinition>(                                                            \
                #NAME, Tucuxi::Common::Utils::varToString(VALUE), nullptr, CovariateType::C_TYPE, DataType::D_TYPE); \
        tmp->setInterpolationType(InterpolationType::INTER_TYPE);                                                    \
        tmp->setRefreshPeriod(REFR_INT);                                                                             \
        CD_VEC.push_back(std::move(tmp));                                                                            \
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
#define ADD_CDEF_W_R_UNIT(NAME, VALUE, C_TYPE, D_TYPE, INTER_TYPE, REFR_INT, UNIT, CD_VEC)                           \
    do {                                                                                                             \
        auto tmp = std::make_unique<CovariateDefinition>(                                                            \
                #NAME, Tucuxi::Common::Utils::varToString(VALUE), nullptr, CovariateType::C_TYPE, DataType::D_TYPE); \
        tmp->setInterpolationType(InterpolationType::INTER_TYPE);                                                    \
        tmp->setRefreshPeriod(REFR_INT);                                                                             \
        tmp->setUnit(Unit(UNIT));                                                                                    \
        CD_VEC.push_back(std::move(tmp));                                                                            \
    } while (0);


/// \brief Add a patient variate (without a specified unit of measure) to a given patient variates vector.
/// \param NAME Name of the patient variate to add.
/// \param VALUE Default value of the patient variate to add.
/// \param D_TYPE Data type of the patient variate (from DataType).
/// \param DATE Date of the measurement.
/// \param PV_VEC Patient variates vector in which the variate has to be pushed.
#define ADD_PV_NO_UNIT(NAME, VALUE, D_TYPE, DATE, PV_VEC)                                          \
    do {                                                                                           \
        auto tmp = std::make_unique<PatientCovariate>(                                             \
                #NAME, Tucuxi::Common::Utils::varToString(VALUE), DataType::D_TYPE, Unit(), DATE); \
        PV_VEC.push_back(std::move(tmp));                                                          \
    } while (0);


/// \brief Add a patient variate (with a specified unit of measure) to a given patient variates vector.
/// \param NAME Name of the patient variate to add.
/// \param VALUE Default value of the patient variate to add.
/// \param D_TYPE Data type of the patient variate (from DataType).
/// \param UNIT Unit of measure of the patient variate.
/// \param DATE Date of the measurement.
/// \param PV_VEC Patient variates vector in which the variate has to be pushed.
#define ADD_PV_W_UNIT(NAME, VALUE, D_TYPE, UNIT, DATE, PV_VEC)                                         \
    do {                                                                                               \
        auto tmp = std::make_unique<PatientCovariate>(                                                 \
                #NAME, Tucuxi::Common::Utils::varToString(VALUE), DataType::D_TYPE, Unit(UNIT), DATE); \
        PV_VEC.push_back(std::move(tmp));                                                              \
    } while (0);


/// \brief Add a computed covariate definition to a given covariate definitions vector.
///        The operation has 2 inputs whose names are specified as parameters.
/// \param NAME Name of the covariate to add.
/// \param OPERATION Operation performed to get the values of the computed covariate.
/// \param OP1 Name of the first operand.
/// \param OP2 Name of the second operand.
/// \param CD_VEC Covariate definitions vector in which the covariate has to be pushed.
#define ADD_OP2_CDEF(NAME, OPERATION, OP1, OP2, CD_VEC)                                                                \
    do {                                                                                                               \
        auto op = std::make_unique<JSOperation>(                                                                       \
                OPERATION,                                                                                             \
                OperationInputList{OperationInput(OP1, InputType::DOUBLE), OperationInput(OP2, InputType::DOUBLE)});   \
        auto tmp = std::make_unique<CovariateDefinition>(#NAME, Tucuxi::Common::Utils::varToString(0), std::move(op)); \
        CD_VEC.push_back(std::move(tmp));                                                                              \
    } while (0);

/// \brief Add a computed covariate definition to a given covariate definitions vector.
///        The operation has 2 inputs whose names are specified as parameters.
/// \param NAME Name of the covariate to add.
/// \param EXPRESSION Expression performed to get the values of the computed covariate.
/// \param OP1 Name of the first operand.
/// \param OP2 Name of the second operand.
/// \param CD_VEC Covariate definitions vector in which the covariate has to be pushed.
#define ADD_EXPR2_CDEF(NAME, EXPRESSION, OP1, OP2, CD_VEC)                                                             \
    do {                                                                                                               \
        auto op = std::make_unique<JSExpression>(                                                                      \
                EXPRESSION,                                                                                            \
                OperationInputList{OperationInput(OP1, InputType::DOUBLE), OperationInput(OP2, InputType::DOUBLE)});   \
        auto tmp = std::make_unique<CovariateDefinition>(#NAME, Tucuxi::Common::Utils::varToString(0), std::move(op)); \
        CD_VEC.push_back(std::move(tmp));                                                                              \
    } while (0);


/// \brief Add a computed covariate definition to a given covariate definitions vector.
///        The operation has 3 inputs whose names are specified as parameters.
/// \param NAME Name of the covariate to add.
/// \param OPERATION Operation performed to get the values of the computed covariate.
/// \param OP1 Name of the first operand.
/// \param OP2 Name of the second operand.
/// \param OP3 Name of the third operand.
/// \param CD_VEC Covariate definitions vector in which the covariate has to be pushed.
#define ADD_OP3_CDEF(NAME, OPERATION, OP1, OP2, OP3, CD_VEC)                                                           \
    do {                                                                                                               \
        auto op = std::make_unique<JSOperation>(                                                                       \
                OPERATION,                                                                                             \
                OperationInputList{                                                                                    \
                        OperationInput(OP1, InputType::DOUBLE),                                                        \
                        OperationInput(OP2, InputType::DOUBLE),                                                        \
                        OperationInput(OP3, InputType::DOUBLE)});                                                      \
        auto tmp = std::make_unique<CovariateDefinition>(#NAME, Tucuxi::Common::Utils::varToString(0), std::move(op)); \
        CD_VEC.push_back(std::move(tmp));                                                                              \
    } while (0);

/// \brief Add a computed covariate definition to a given covariate definitions vector.
///        The expression has 3 inputs whose names are specified as parameters.
/// \param NAME Name of the covariate to add.
/// \param EXPRESSION Expression performed to get the values of the computed covariate.
/// \param OP1 Name of the first operand.
/// \param OP2 Name of the second operand.
/// \param OP3 Name of the third operand.
/// \param CD_VEC Covariate definitions vector in which the covariate has to be pushed.
#define ADD_EXPR3_CDEF(NAME, EXPRESSION, OP1, OP2, OP3, CD_VEC)                                                        \
    do {                                                                                                               \
        auto op = std::make_unique<JSExpression>(                                                                      \
                EXPRESSION,                                                                                            \
                OperationInputList{                                                                                    \
                        OperationInput(OP1, InputType::DOUBLE),                                                        \
                        OperationInput(OP2, InputType::DOUBLE),                                                        \
                        OperationInput(OP3, InputType::DOUBLE)});                                                      \
        auto tmp = std::make_unique<CovariateDefinition>(#NAME, Tucuxi::Common::Utils::varToString(0), std::move(op)); \
        CD_VEC.push_back(std::move(tmp));                                                                              \
    } while (0);

/// \brief Check a refresh map for the presence of a given covariate at a specified time.
/// \param NAME Name of the covariate to search.
/// \param DATE Time instant when the covariate is supposed to show up.
/// \param REFRESH_MAP Refresh map where the covariate has be to sought.
#define CHECK_REFRESH(NAME, DATE, REFRESH_MAP)                                                                         \
    do {                                                                                                               \
        ASSERT_FALSE(std::find(REFRESH_MAP[DATE].begin(), REFRESH_MAP[DATE].end(), #NAME) == REFRESH_MAP[DATE].end()); \
    } while (0);


/// \brief Add a computed parameter definition to a given parameter definitions vector.
///        The operation has 2 inputs whose names are specified as parameters.
/// \param NAME Name of the parameter to add.
/// \param OPERATION Operation performed to get the values of the computed parameter.
/// \param OP1 Name of the first operand.
/// \param OP2 Name of the second operand.
/// \param PD_VEC Parameter definitions vector in which the parameter has to be pushed.
#define ADD_OP2_PDEF(NAME, OPERATION, OP1, OP2, PD_VEC)                                                              \
    do {                                                                                                             \
        auto op = std::make_unique<JSOperation>(                                                                     \
                OPERATION,                                                                                           \
                OperationInputList{OperationInput(OP1, InputType::DOUBLE), OperationInput(OP2, InputType::DOUBLE)}); \
        auto tmp = std::make_unique<ParameterDefinition>(#NAME, 0, std::move(op), ParameterVariabilityType::None);   \
        PD_VEC.push_back(std::move(tmp));                                                                            \
    } while (0);

/// \brief Add a computed parameter definition to a given parameter definitions vector.
///        The operation has 2 inputs whose names are specified as parameters.
/// \param NAME Name of the parameter to add.
/// \param EXPRESSION Expression performed to get the values of the computed parameter.
/// \param OP1 Name of the first operand.
/// \param OP2 Name of the second operand.
/// \param PD_VEC Parameter definitions vector in which the parameter has to be pushed.
#define ADD_EXPR2_PDEF(NAME, EXPRESSION, OP1, OP2, PD_VEC)                                                           \
    do {                                                                                                             \
        auto op = std::make_unique<JSExpression>(                                                                    \
                EXPRESSION,                                                                                          \
                OperationInputList{OperationInput(OP1, InputType::DOUBLE), OperationInput(OP2, InputType::DOUBLE)}); \
        auto tmp = std::make_unique<ParameterDefinition>(#NAME, 0, std::move(op), ParameterVariabilityType::None);   \
        PD_VEC.push_back(std::move(tmp));                                                                            \
    } while (0);
#endif // TESTUTILS_H
