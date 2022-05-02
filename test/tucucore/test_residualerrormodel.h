//@@license@@

#ifndef TEST_RESIDUALERRORMODEL_H
#define TEST_RESIDUALERRORMODEL_H

#include "tucucore/residualerrormodel.h"

#include "fructose/fructose.h"

using namespace Tucuxi::Core;

struct TestResidualErrorModel : public fructose::test_base<TestResidualErrorModel>
{
    TestResidualErrorModel() {}

    Value calculateSampleLikelihood(
            ResidualErrorType _errorModelType, double _sigma, double _expected, double _observed)
    {
        Tucuxi::Core::SigmaResidualErrorModel errorModel;
        errorModel.setErrorModel(_errorModelType);
        Sigma sigma(1);
        sigma[0] = _sigma;
        errorModel.setSigma(sigma);
        return errorModel.calculateSampleLikelihood(_expected, _observed);
    }

    Value calculateSampleLikelihood2(
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

    Value applyEpsToValue(ResidualErrorType _errorModelType, double _sigma, double _value, double _eps)
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

    Value applyEpsToValue2(
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

    /// \brief Test basic functionalities of a PkModel.
    ///
    /// \testing{Tucuxi::Core::SigmaResidualErrorModel::calculateSampleLikelihood()}
    ///
    void testLogLikelihood(const std::string& /* _testName */)
    {
        Value logLikelihood0 = 0.0;
        Value logLikelihood1 = 0.0;
        Value logLikelihood2 = 0.0;

        logLikelihood0 = calculateSampleLikelihood(ResidualErrorType::ADDITIVE, 2.0, 10.0, 10.0);
        logLikelihood1 = calculateSampleLikelihood(ResidualErrorType::ADDITIVE, 2.0, 12.0, 12.0);
        fructose_assert_double_eq(logLikelihood0, logLikelihood1);

        logLikelihood0 = calculateSampleLikelihood(ResidualErrorType::ADDITIVE, 2.0, 10.0, 10.1);
        logLikelihood1 = calculateSampleLikelihood(ResidualErrorType::ADDITIVE, 2.0, 10.0, 10.0);
        logLikelihood2 = calculateSampleLikelihood(ResidualErrorType::ADDITIVE, 2.0, 10.0, 9.9);
        fructose_assert_double_gt(logLikelihood1, logLikelihood0);
        fructose_assert_double_gt(logLikelihood1, logLikelihood2);


        logLikelihood0 = calculateSampleLikelihood(ResidualErrorType::PROPEXP, 2.0, 10.0, 10.0);
        logLikelihood1 = calculateSampleLikelihood(ResidualErrorType::PROPEXP, 2.0, 12.0, 12.0);
        // For a proportional error model, the log likelihood depends on the expected value, even
        // if expected and observed are equal
        fructose_assert_double_ne(logLikelihood0, logLikelihood1);

        logLikelihood0 = calculateSampleLikelihood(ResidualErrorType::PROPEXP, 2.0, 10.0, 10.1);
        logLikelihood1 = calculateSampleLikelihood(ResidualErrorType::PROPEXP, 2.0, 10.0, 10.0);
        logLikelihood2 = calculateSampleLikelihood(ResidualErrorType::PROPEXP, 2.0, 10.0, 9.9);
        fructose_assert_double_gt(logLikelihood1, logLikelihood0);
        fructose_assert_double_gt(logLikelihood1, logLikelihood2);

        logLikelihood0 = calculateSampleLikelihood(ResidualErrorType::EXPONENTIAL, 2.0, 10.0, 10.0);
        logLikelihood1 = calculateSampleLikelihood(ResidualErrorType::EXPONENTIAL, 2.0, 12.0, 12.0);
        fructose_assert_double_eq(logLikelihood0, logLikelihood1);

        logLikelihood0 = calculateSampleLikelihood(ResidualErrorType::EXPONENTIAL, 2.0, 10.0, 10.1);
        logLikelihood1 = calculateSampleLikelihood(ResidualErrorType::EXPONENTIAL, 2.0, 10.0, 10.0);
        logLikelihood2 = calculateSampleLikelihood(ResidualErrorType::EXPONENTIAL, 2.0, 10.0, 9.9);
        fructose_assert_double_gt(logLikelihood1, logLikelihood0);
        fructose_assert_double_gt(logLikelihood1, logLikelihood2);

        logLikelihood0 = calculateSampleLikelihood(ResidualErrorType::PROPORTIONAL, 2.0, 10.0, 10.0);
        logLikelihood1 = calculateSampleLikelihood(ResidualErrorType::PROPORTIONAL, 2.0, 12.0, 12.0);
        // For a proportional error model, the log likelihood depends on the expected value, even
        // if expected and observed are equal
        fructose_assert_double_ne(logLikelihood0, logLikelihood1);


        logLikelihood0 = calculateSampleLikelihood(ResidualErrorType::PROPORTIONAL, 2.0, 10.0, 10.1);
        logLikelihood1 = calculateSampleLikelihood(ResidualErrorType::PROPORTIONAL, 2.0, 10.0, 10.0);
        logLikelihood2 = calculateSampleLikelihood(ResidualErrorType::PROPORTIONAL, 2.0, 10.0, 9.9);
        fructose_assert_double_gt(logLikelihood1, logLikelihood0);
        fructose_assert_double_gt(logLikelihood1, logLikelihood2);

        logLikelihood0 = calculateSampleLikelihood(ResidualErrorType::PROPORTIONAL, 2.0, 10.1, 10.0);
        logLikelihood1 = calculateSampleLikelihood(ResidualErrorType::PROPORTIONAL, 2.0, 10.0, 10.0);
        logLikelihood2 = calculateSampleLikelihood(ResidualErrorType::PROPORTIONAL, 2.0, 9.9, 10.0);
        fructose_assert_double_gt(logLikelihood1, logLikelihood0);
        // As the expected and the observed are in the wrong order, the gt is not satisfied
        fructose_assert_double_lt(logLikelihood1, logLikelihood2);


        logLikelihood0 = calculateSampleLikelihood2(ResidualErrorType::MIXED, 2.0, 0.5, 10.0, 10.1);
        logLikelihood1 = calculateSampleLikelihood2(ResidualErrorType::MIXED, 2.0, 0.5, 10.0, 10.0);
        logLikelihood2 = calculateSampleLikelihood2(ResidualErrorType::MIXED, 2.0, 0.5, 10.0, 9.9);
        fructose_assert_double_gt(logLikelihood1, logLikelihood0);
        fructose_assert_double_gt(logLikelihood1, logLikelihood2);
    }

    void testApplyEpsToValue(const std::string& /* _testName */)
    {
        Value calculated;

        // If epsilon is 0, then the value should not change
        calculated = applyEpsToValue(ResidualErrorType::ADDITIVE, 2.0, 10.0, 0.0);
        fructose_assert_eq(calculated, 10.0);

        // If epsilon is 0, then the value should not change
        calculated = applyEpsToValue(ResidualErrorType::EXPONENTIAL, 2.0, 10.0, 0.0);
        fructose_assert_eq(calculated, 10.0);

        // If epsilon is 0, then the value should not change
        calculated = applyEpsToValue(ResidualErrorType::PROPORTIONAL, 2.0, 10.0, 0.0);
        fructose_assert_eq(calculated, 10.0);

        // If epsilon is 0, then the value should not change
        calculated = applyEpsToValue2(ResidualErrorType::MIXED, 2.0, 1.5, 10.0, 0.0);
        fructose_assert_eq(calculated, 10.0);

        // If epsilon is 0, then the value should not change
        calculated = applyEpsToValue(ResidualErrorType::PROPEXP, 2.0, 10.0, 0.0);
        fructose_assert_eq(calculated, 10.0);
    }
};


#endif // TEST_RESIDUALERRORMODEL_H
