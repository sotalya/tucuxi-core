#ifndef TEST_RESIDUALERRORMODEL_H
#define TEST_RESIDUALERRORMODEL_H

#include "fructose/fructose.h"

#include "tucucore/residualerrormodel.h"

using namespace Tucuxi::Core;

struct TestResidualErrorModel : public fructose::test_base<TestResidualErrorModel>
{
    TestResidualErrorModel() { }

    Value calculateSampleLikelihood(ResidualErrorType _errorModelType,
                                    double _sigma,
                                    double _expected,
                                    double _observed)
    {
        Tucuxi::Core::SigmaResidualErrorModel errorModel;
        errorModel.setErrorModel(_errorModelType);
        Sigma sigma(1);
        sigma[0] = _sigma;
        errorModel.setSigma(sigma);
        return errorModel.calculateSampleLikelihood(_expected, _observed);
    }

    Value calculateSampleLikelihood2(ResidualErrorType _errorModelType,
                                    double _sigma0,
                                    double _sigma1,
                                    double _expected,
                                    double _observed)
    {
        Tucuxi::Core::SigmaResidualErrorModel errorModel;
        errorModel.setErrorModel(_errorModelType);
        Sigma sigma(2);
        sigma[0] = _sigma0;
        sigma[1] = _sigma1;
        errorModel.setSigma(sigma);
        return errorModel.calculateSampleLikelihood(_expected, _observed);
    }

    /// \brief Test basic functionalities of a PkModel.
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

};


#endif // TEST_RESIDUALERRORMODEL_H
