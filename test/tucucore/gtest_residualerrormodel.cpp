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


#include <gtest/gtest.h>

#include "tucucore/residualerrormodel.h"

#include "gtest_core.h"

using namespace Tucuxi::Core;

static Value calculateSampleLikelihood(
        ResidualErrorType _errorModelType, double _sigma, double _expected, double _observed)
{
    Tucuxi::Core::SigmaResidualErrorModel errorModel;
    errorModel.setErrorModel(_errorModelType);
    Sigma sigma(1);
    sigma[0] = _sigma;
    errorModel.setSigma(sigma);
    return errorModel.calculateSampleLikelihood(_expected, _observed);
}

static Value calculateSampleLikelihood2(
        ResidualErrorType _errorModelType, double _sigma0, double _sigma1, double _expected, double _observed)
{
    Tucuxi::Core::SigmaResidualErrorModel errorModel;
    errorModel.setErrorModel(_errorModelType);
    Sigma sigma(2);
    sigma[0] = _sigma0;
    sigma[1] = _sigma1;
    errorModel.setSigma(sigma);
    return errorModel.calculateSampleLikelihood(_expected, _observed);
}

static Value applyEpsToValue(ResidualErrorType _errorModelType, double _sigma, double _value, double _eps)
{
    Tucuxi::Core::SigmaResidualErrorModel errorModel;
    errorModel.setErrorModel(_errorModelType);
    Sigma sigma(1);
    sigma[0] = _sigma;
    errorModel.setSigma(sigma);
    Tucuxi::Core::Deviations eps(1);
    eps[0] = _eps;
    Value value = _value;
    errorModel.applyEpsToValue(value, eps);
    return value;
}

static Value applyEpsToValue2(
        ResidualErrorType _errorModelType, double _sigma0, double _sigma1, double _value, double _eps)
{
    Tucuxi::Core::SigmaResidualErrorModel errorModel;
    errorModel.setErrorModel(_errorModelType);
    Sigma sigma(2);
    sigma[0] = _sigma0;
    sigma[1] = _sigma1;
    errorModel.setSigma(sigma);
    Tucuxi::Core::Deviations eps(1);
    eps[0] = _eps;
    Value value = _value;
    errorModel.applyEpsToValue(value, eps);
    return value;
}


TEST(Core_TestResidualErrorModel, LogLikelihood)
{
    Value logLikelihood0 = 0.0;
    Value logLikelihood1 = 0.0;
    Value logLikelihood2 = 0.0;

    logLikelihood0 = calculateSampleLikelihood(ResidualErrorType::ADDITIVE, 2.0, 10.0, 10.0);
    logLikelihood1 = calculateSampleLikelihood(ResidualErrorType::ADDITIVE, 2.0, 12.0, 12.0);
    ASSERT_DOUBLE_EQ(logLikelihood0, logLikelihood1);

    logLikelihood0 = calculateSampleLikelihood(ResidualErrorType::ADDITIVE, 2.0, 10.0, 10.1);
    logLikelihood1 = calculateSampleLikelihood(ResidualErrorType::ADDITIVE, 2.0, 10.0, 10.0);
    logLikelihood2 = calculateSampleLikelihood(ResidualErrorType::ADDITIVE, 2.0, 10.0, 9.9);
    ASSERT_GT(logLikelihood1, logLikelihood0);
    ASSERT_GT(logLikelihood1, logLikelihood2);


    logLikelihood0 = calculateSampleLikelihood(ResidualErrorType::PROPEXP, 2.0, 10.0, 10.0);
    logLikelihood1 = calculateSampleLikelihood(ResidualErrorType::PROPEXP, 2.0, 12.0, 12.0);
    // For a proportional error model, the log likelihood depends on the expected value, even
    // if expected and observed are equal
    ASSERT_PRED4(double_ne_rel_abs, logLikelihood0, logLikelihood1, DEFAULT_PRECISION, DEFAULT_PRECISION);

    logLikelihood0 = calculateSampleLikelihood(ResidualErrorType::PROPEXP, 2.0, 10.0, 10.1);
    logLikelihood1 = calculateSampleLikelihood(ResidualErrorType::PROPEXP, 2.0, 10.0, 10.0);
    logLikelihood2 = calculateSampleLikelihood(ResidualErrorType::PROPEXP, 2.0, 10.0, 9.9);
    ASSERT_GT(logLikelihood1, logLikelihood0);
    ASSERT_GT(logLikelihood1, logLikelihood2);

    logLikelihood0 = calculateSampleLikelihood(ResidualErrorType::EXPONENTIAL, 2.0, 10.0, 10.0);
    logLikelihood1 = calculateSampleLikelihood(ResidualErrorType::EXPONENTIAL, 2.0, 12.0, 12.0);
    ASSERT_DOUBLE_EQ(logLikelihood0, logLikelihood1);

    logLikelihood0 = calculateSampleLikelihood(ResidualErrorType::EXPONENTIAL, 2.0, 10.0, 10.1);
    logLikelihood1 = calculateSampleLikelihood(ResidualErrorType::EXPONENTIAL, 2.0, 10.0, 10.0);
    logLikelihood2 = calculateSampleLikelihood(ResidualErrorType::EXPONENTIAL, 2.0, 10.0, 9.9);
    ASSERT_GT(logLikelihood1, logLikelihood0);
    ASSERT_GT(logLikelihood1, logLikelihood2);

    logLikelihood0 = calculateSampleLikelihood(ResidualErrorType::PROPORTIONAL, 2.0, 10.0, 10.0);
    logLikelihood1 = calculateSampleLikelihood(ResidualErrorType::PROPORTIONAL, 2.0, 12.0, 12.0);
    // For a proportional error model, the log likelihood depends on the expected value, even
    // if expected and observed are equal
    ASSERT_PRED4(double_ne_rel_abs, logLikelihood0, logLikelihood1, DEFAULT_PRECISION, DEFAULT_PRECISION);


    logLikelihood0 = calculateSampleLikelihood(ResidualErrorType::PROPORTIONAL, 2.0, 10.0, 10.1);
    logLikelihood1 = calculateSampleLikelihood(ResidualErrorType::PROPORTIONAL, 2.0, 10.0, 10.0);
    logLikelihood2 = calculateSampleLikelihood(ResidualErrorType::PROPORTIONAL, 2.0, 10.0, 9.9);
    ASSERT_GT(logLikelihood1, logLikelihood0);
    ASSERT_GT(logLikelihood1, logLikelihood2);

    logLikelihood0 = calculateSampleLikelihood(ResidualErrorType::PROPORTIONAL, 2.0, 10.1, 10.0);
    logLikelihood1 = calculateSampleLikelihood(ResidualErrorType::PROPORTIONAL, 2.0, 10.0, 10.0);
    logLikelihood2 = calculateSampleLikelihood(ResidualErrorType::PROPORTIONAL, 2.0, 9.9, 10.0);
    ASSERT_GT(logLikelihood1, logLikelihood0);
    // As the expected and the observed are in the wrong order, the gt is not satisfied
    ASSERT_LT(logLikelihood1, logLikelihood2);


    logLikelihood0 = calculateSampleLikelihood2(ResidualErrorType::MIXED, 2.0, 0.5, 10.0, 10.1);
    logLikelihood1 = calculateSampleLikelihood2(ResidualErrorType::MIXED, 2.0, 0.5, 10.0, 10.0);
    logLikelihood2 = calculateSampleLikelihood2(ResidualErrorType::MIXED, 2.0, 0.5, 10.0, 9.9);
    ASSERT_GT(logLikelihood1, logLikelihood0);
    ASSERT_GT(logLikelihood1, logLikelihood2);
}

TEST(Core_TestResidualErrorModel, ApplyEpsToValue)
{
    Value calculated;

    // If epsilon is 0, then the value should not change
    calculated = applyEpsToValue(ResidualErrorType::ADDITIVE, 2.0, 10.0, 0.0);
    ASSERT_DOUBLE_EQ(calculated, 10.0);

    // If epsilon is 0, then the value should not change
    calculated = applyEpsToValue(ResidualErrorType::EXPONENTIAL, 2.0, 10.0, 0.0);
    ASSERT_DOUBLE_EQ(calculated, 10.0);

    // If epsilon is 0, then the value should not change
    calculated = applyEpsToValue(ResidualErrorType::PROPORTIONAL, 2.0, 10.0, 0.0);
    ASSERT_DOUBLE_EQ(calculated, 10.0);

    // If epsilon is 0, then the value should not change
    calculated = applyEpsToValue2(ResidualErrorType::MIXED, 2.0, 1.5, 10.0, 0.0);
    ASSERT_DOUBLE_EQ(calculated, 10.0);

    // If epsilon is 0, then the value should not change
    calculated = applyEpsToValue(ResidualErrorType::PROPEXP, 2.0, 10.0, 0.0);
    ASSERT_DOUBLE_EQ(calculated, 10.0);
}
