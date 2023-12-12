//@@license@@

#include <memory>

#include <gtest/gtest.h>

#include "gtest_core.h"

#include "tucucore/concentrationcalculator.h"
#include "tucucore/drugmodel/formulationandroute.h"
#include "tucucore/intakeevent.h"
#include "tucucore/likelihood.h"
#include "tucucore/residualerrormodel.h"

#include "pkmodels/constanteliminationbolus.h"

using namespace Tucuxi::Core;
using namespace std::chrono_literals;

///
/// \brief Builds a simple intake series
/// \return A intake series with the associated calculators
///
/// The Intake seriesHere has two intakes of 24h interval
/// Dates : 06/06/2017 at 8:30
///         07/06/2017 at 8:30
///
static IntakeSeries buildIntakeSeries()
{
    IntakeSeries intakes;
    intakes.emplace_back(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(6)),
                    Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(200.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(24)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(std::chrono::minutes(20)),
            static_cast<int>(CYCLE_SIZE));
    intakes.emplace_back(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(7)),
                    Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
            Duration(),
            DoseValue(200.0),
            TucuUnit("mg"),
            Duration(std::chrono::hours(24)),
            getBolusFormulationAndRoute(),
            getBolusFormulationAndRoute().getAbsorptionModel(),
            Duration(std::chrono::minutes(20)),
            static_cast<int>(CYCLE_SIZE));

    // Associate the intake calculator to the intakes
    std::shared_ptr<ConstantEliminationBolus> intakeCalculator = std::make_shared<ConstantEliminationBolus>();
    intakes[0].setCalculator(intakeCalculator);
    intakes[1].setCalculator(intakeCalculator);
    return intakes;
}

/// \brief Test the likelihood calculation
/// \param _testName Test name.
///
/// \testing{Tucuxi::Core::Likelihood::negativeLogLikelihood()}
///
/// A Likelihood object computes a negativeLogLikelihood.
/// This test uses wrong parameters, and as such the result should be the maximum value for double.
///
TEST (Core_TestLikelihood, WrongParameters){
    IntakeSeries intakes = buildIntakeSeries();

    // Only one sample at date 06/06/2017 at 12:30
    // Sample value: 200.0
    SampleEvent sample1(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(6)),
                    Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
            200.0);
    SampleSeries samples;
    samples.push_back(sample1);

    // Here we have wrong parameters, and a matrix with a size corresponding to the number of variable parameters
    auto omega = Tucuxi::Core::OmegaMatrix(2, 2);
    omega(0, 0) = 0.356 * 0.356;
    omega(0, 1) = 0.798 * 0.356 * 0.629;
    omega(1, 1) = 0.629 * 0.629;
    omega(1, 0) = 0.798 * 0.356 * 0.629;

    SigmaResidualErrorModel residualErrorModel;
    Tucuxi::Core::Sigma sigma(1);
    sigma(0) = 0.3138;
    residualErrorModel.setErrorModel(Tucuxi::Core::ResidualErrorType::PROPORTIONAL);
    residualErrorModel.setSigma(sigma);

    Tucuxi::Core::ParameterDefinitions microParameterDefs;
    microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "V", 347, Tucuxi::Core::ParameterVariabilityType::Additive));
    microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "Ke", 0.0435331, Tucuxi::Core::ParameterVariabilityType::Additive));
    Tucuxi::Core::ParameterSetEvent parameterEvent(DateTime::now(), microParameterDefs);
    ConcentrationCalculator calculator;
    ParameterSetSeries parameters;
    parameters.addParameterSetEvent(parameterEvent);
    Etas etas(2);
    etas[0] = 0.1;
    etas[1] = 0.1;

    Likelihood likelihood(omega, residualErrorModel, samples, intakes, parameters, calculator);
    Value res = likelihood.negativeLogLikelihood(etas);
    // The parameters do not fit the ConstantEliminationBolus, so we should get the max value
    ASSERT_DOUBLE_EQ(res, std::numeric_limits<double>::max());
}

/// \brief Test the likelihood calculation
/// \param _testName Test name.
///
/// \testing{Tucuxi::Core::Likelihood::negativeLogLikelihood()}
///
/// A Likelihood object computes a negativeLogLikelihood.
/// This test is done thanks to ConstantEliminationBolus, with one single sample.
///
TEST (Core_TestLikelihood, oneSample){
    IntakeSeries intakes = buildIntakeSeries();

    // Only one sample at date 06/06/2017 at 12:30
    // Sample value: 200.0
    SampleEvent sample1(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(6)),
                    Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
            200.0);
    SampleSeries samples;
    samples.push_back(sample1);

    // Here we build parameters that correspond to the intake calculator (ConstantEliminationBolus)
    Tucuxi::Core::ParameterDefinitions microParameterDefs;
    microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestA", 1.0, Tucuxi::Core::ParameterVariabilityType::Additive));
    microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestM", 1.0, Tucuxi::Core::ParameterVariabilityType::None));
    microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestR", 0.0, Tucuxi::Core::ParameterVariabilityType::None));
    microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestS", 0.0, Tucuxi::Core::ParameterVariabilityType::None));
    Tucuxi::Core::ParameterSetEvent parameterEvent(DateTime::now(), microParameterDefs);
    ConcentrationCalculator calculator;
    ParameterSetSeries parameters;
    parameters.addParameterSetEvent(parameterEvent);


    SigmaResidualErrorModel residualErrorModel;
    Tucuxi::Core::Sigma sigma(1);
    sigma(0) = 0.3138;
    residualErrorModel.setErrorModel(Tucuxi::Core::ResidualErrorType::PROPORTIONAL);
    residualErrorModel.setSigma(sigma);

    double theEta = 0.1;
    Etas etas(1);
    etas[0] = theEta;

    auto omega = Tucuxi::Core::OmegaMatrix(1, 1);
    omega(0, 0) = 0.1; // Variance of A

    Likelihood likelihood(omega, residualErrorModel, samples, intakes, parameters, calculator);

    Value res = likelihood.negativeLogLikelihood(etas);
    // We compute the expected result

    // Prior: 0.5 * (_etas.transpose() * m_inverseOmega * _etas + m_omegaAdd)
    // m_omega = m_omegaAdd(static_cast<double>(_omega.rows()) * log(2 * PI) + log(_omega.determinant()))
    // sample likelihood: - _residualErrorModel.calculateSampleLikelihood(_expected, _observed.getValue());
    double expectedSampleValue = 201.1;
    double omegaAdd = static_cast<double>(omega.rows()) * log(2 * PI) + log(omega.determinant());

    EigenVector etasmd(1);
    etasmd[0] = theEta;

    double expectedValue = 0.5 * (etasmd.transpose() * omega.inverse() * etasmd + omegaAdd)
                           - residualErrorModel.calculateSampleLikelihood(expectedSampleValue, sample1.getValue());
    ASSERT_DOUBLE_EQ(res, expectedValue);
}

/// \brief Test the likelihood calculation, with weight for sample
/// \param _testName Test name.
///
/// \testing{Tucuxi::Core::Likelihood::negativeLogLikelihood()}
///
/// A Likelihood object computes a negativeLogLikelihood.
/// This test is done thanks to ConstantEliminationBolus, with one single sample.
///
TEST (Core_TestLikelihood, WeightSample){
    IntakeSeries intakes = buildIntakeSeries();

    // First sample at date 06/06/2017 at 12:30, with a weight of 0.3
    // Sample value: 200.0
    SampleSeries samples;
    SampleEvent sample1(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(6)),
                    Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
            200.0,
            0.3);

    samples.push_back(sample1);

    // Second sample at date 06/06/2017 at 12:30, with a weight of 0.7
    // Sample value: 200.0
    SampleEvent sample2(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(6)),
                    Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
            200.0,
            0.7);

    samples.push_back(sample2);

    // Here we build parameters that correspond to the intake calculator (ConstantEliminationBolus)
    Tucuxi::Core::ParameterDefinitions microParameterDefs;
    microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestA", 1.0, Tucuxi::Core::ParameterVariabilityType::Additive));
    microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestM", 1.0, Tucuxi::Core::ParameterVariabilityType::None));
    microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestR", 0.0, Tucuxi::Core::ParameterVariabilityType::None));
    microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestS", 0.0, Tucuxi::Core::ParameterVariabilityType::None));
    Tucuxi::Core::ParameterSetEvent parameterEvent(DateTime::now(), microParameterDefs);
    ConcentrationCalculator calculator;
    ParameterSetSeries parameters;
    parameters.addParameterSetEvent(parameterEvent);


    SigmaResidualErrorModel residualErrorModel;
    Tucuxi::Core::Sigma sigma(1);
    sigma(0) = 0.3138;
    residualErrorModel.setErrorModel(Tucuxi::Core::ResidualErrorType::PROPORTIONAL);
    residualErrorModel.setSigma(sigma);

    double theEta = 0.1;
    Etas etas(1);
    etas[0] = theEta;

    auto omega = Tucuxi::Core::OmegaMatrix(1, 1);
    omega(0, 0) = 0.1; // Variance of A

    Likelihood likelihood(omega, residualErrorModel, samples, intakes, parameters, calculator);

    Value res = likelihood.negativeLogLikelihood(etas);

    // We compute the expected resut by using one sample with a default weight if 1. It should be
    // equal to two sample with weights of 0.3 and 0.7.
    SampleSeries samplesEpected;
    SampleEvent expectedSample1(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(6)),
                    Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
            200.0);

    samplesEpected.push_back(expectedSample1);

    Likelihood likelihoodExpected(omega, residualErrorModel, samplesEpected, intakes, parameters, calculator);
    Value expectedValue = likelihoodExpected.negativeLogLikelihood(etas);

    ASSERT_DOUBLE_EQ(res, expectedValue);
}

/// \brief Test the likelihood calculation
/// \param _testName Test name.
///
/// \testing{Tucuxi::Core::Likelihood::negativeLogLikelihood()}
///
/// A Likelihood object computes a negativeLogLikelihood.
/// This test is done thanks to ConstantEliminationBolus, with two samples at the same date.
///
TEST (Core_TestLikelihood, twoSamplesSameDate){
    IntakeSeries intakes = buildIntakeSeries();

    // Here we build parameters that correspond to the intake calculator (ConstantEliminationBolus)
    Tucuxi::Core::ParameterDefinitions microParameterDefs;
    microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestA", 1.0, Tucuxi::Core::ParameterVariabilityType::Additive));
    microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestM", 1.0, Tucuxi::Core::ParameterVariabilityType::None));
    microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestR", 0.0, Tucuxi::Core::ParameterVariabilityType::None));
    microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestS", 0.0, Tucuxi::Core::ParameterVariabilityType::None));
    Tucuxi::Core::ParameterSetEvent parameterEvent(DateTime::now(), microParameterDefs);
    ConcentrationCalculator calculator;
    ParameterSetSeries parameters;
    parameters.addParameterSetEvent(parameterEvent);


    SigmaResidualErrorModel residualErrorModel;
    Tucuxi::Core::Sigma sigma(1);
    sigma(0) = 0.3138;
    residualErrorModel.setErrorModel(Tucuxi::Core::ResidualErrorType::PROPORTIONAL);
    residualErrorModel.setSigma(sigma);

    double theEta = 0.1;
    Etas etas(1);
    etas[0] = theEta;

    auto omega = Tucuxi::Core::OmegaMatrix(1, 1);
    omega(0, 0) = 0.1; // Variance of A

    // Two samples at date 06/06/2017 at 12:30
    // Sample value: 200.0
    SampleSeries samples;
    SampleEvent sample1(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(6)),
                    Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
            200.0);
    samples.push_back(sample1);
    SampleEvent sample2(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(6)),
                    Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
            200.0);
    samples.push_back(sample2);


    Likelihood likelihood(omega, residualErrorModel, samples, intakes, parameters, calculator);

    Value res = likelihood.negativeLogLikelihood(etas);
    // We compute the expected result

    // Prior: 0.5 * (_etas.transpose() * m_inverseOmega * _etas + m_omegaAdd)
    // m_omega = m_omegaAdd(static_cast<double>(_omega.rows()) * log(2 * PI) + log(_omega.determinant()))
    // sample likelihood: - _residualErrorModel.calculateSampleLikelihood(_expected, _observed.getValue());
    double expectedSampleValue = 201.1;
    double omegaAdd = static_cast<double>(omega.rows()) * log(2 * PI) + log(omega.determinant());

    EigenVector etasmd(1);
    etasmd[0] = theEta;

    double expectedValue =
            0.5 * (etasmd.transpose() * omega.inverse() * etasmd + omegaAdd)
            - 2.0 * residualErrorModel.calculateSampleLikelihood(expectedSampleValue, sample1.getValue());
    ASSERT_DOUBLE_EQ(res, expectedValue);
}

/// \brief Test the likelihood calculation
/// \param _testName Test name.
///
/// \testing{Tucuxi::Core::Likelihood::negativeLogLikelihood()}
///
/// A Likelihood object computes a negativeLogLikelihood.
/// This test is done thanks to ConstantEliminationBolus, with two samples at different dates.
///
TEST (Core_TestLikelihood, twoSamplesDifferentDates){
    IntakeSeries intakes = buildIntakeSeries();

    // Here we build parameters that correspond to the intake calculator (ConstantEliminationBolus)
    Tucuxi::Core::ParameterDefinitions microParameterDefs;
    microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestA", 1.0, Tucuxi::Core::ParameterVariabilityType::Additive));
    microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestM", 1.0, Tucuxi::Core::ParameterVariabilityType::None));
    microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestR", 0.0, Tucuxi::Core::ParameterVariabilityType::None));
    microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestS", 0.1, Tucuxi::Core::ParameterVariabilityType::None));
    Tucuxi::Core::ParameterSetEvent parameterEvent(DateTime::now(), microParameterDefs);
    ConcentrationCalculator calculator;
    ParameterSetSeries parameters;
    parameters.addParameterSetEvent(parameterEvent);


    SigmaResidualErrorModel residualErrorModel;
    Tucuxi::Core::Sigma sigma(1);
    sigma(0) = 0.3138;
    residualErrorModel.setErrorModel(Tucuxi::Core::ResidualErrorType::PROPORTIONAL);
    residualErrorModel.setSigma(sigma);

    double theEta = 0.1;
    Etas etas(1);
    etas[0] = theEta;

    auto omega = Tucuxi::Core::OmegaMatrix(1, 1);
    omega(0, 0) = 0.1; // Variance of A

    // Two samples at date 06/06/2017.
    // First at 12:30
    // Second at 16:30
    // Sample value: 200.0
    SampleSeries samples;
    SampleEvent sample1(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(6)),
                    Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
            200.0);
    samples.push_back(sample1);
    SampleEvent sample2(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(6)),
                    Duration(std::chrono::hours(16), std::chrono::minutes(30), std::chrono::seconds(0))),
            200.0);
    samples.push_back(sample2);


    Likelihood likelihood(omega, residualErrorModel, samples, intakes, parameters, calculator);

    Value res = likelihood.negativeLogLikelihood(etas);
    // We compute the expected result

    // Prior: 0.5 * (_etas.transpose() * m_inverseOmega * _etas + m_omegaAdd)
    // m_omega = m_omegaAdd(static_cast<double>(_omega.rows()) * log(2 * PI) + log(_omega.determinant()))
    // sample likelihood: - _residualErrorModel.calculateSampleLikelihood(_expected, _observed.getValue());
    double expectedSampleValue1 = 121.1;
    double expectedSampleValue2 = 41.1;
    double omegaAdd = static_cast<double>(omega.rows()) * log(2 * PI) + log(omega.determinant());

    EigenVector etasmd(1);
    etasmd[0] = theEta;

    double expectedValue = 0.5 * (etasmd.transpose() * omega.inverse() * etasmd + omegaAdd)
                           - residualErrorModel.calculateSampleLikelihood(expectedSampleValue1, sample1.getValue())
                           - residualErrorModel.calculateSampleLikelihood(expectedSampleValue2, sample2.getValue());
    ASSERT_DOUBLE_EQ(res, expectedValue);
}

/// \brief Test the likelihood calculation
/// \param _testName Test name.
///
/// \testing{Tucuxi::Core::Likelihood::negativeLogLikelihood()}
///
/// A Likelihood object computes a negativeLogLikelihood.
/// This test is done thanks to ConstantEliminationBolus, with three samples at the different dates.
///
TEST (Core_TestLikelihood, threeSamplesDifferentDates){
    IntakeSeries intakes = buildIntakeSeries();

    // Here we build parameters that correspond to the intake calculator (ConstantEliminationBolus)
    Tucuxi::Core::ParameterDefinitions microParameterDefs;
    microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestA", 1.0, Tucuxi::Core::ParameterVariabilityType::Additive));
    microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestM", 1.0, Tucuxi::Core::ParameterVariabilityType::None));
    microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestR", 0.0, Tucuxi::Core::ParameterVariabilityType::None));
    microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestS", 0.1, Tucuxi::Core::ParameterVariabilityType::None));
    Tucuxi::Core::ParameterSetEvent parameterEvent(DateTime::now(), microParameterDefs);
    ConcentrationCalculator calculator;
    ParameterSetSeries parameters;
    parameters.addParameterSetEvent(parameterEvent);


    SigmaResidualErrorModel residualErrorModel;
    Tucuxi::Core::Sigma sigma(1);
    sigma(0) = 0.3138;
    residualErrorModel.setErrorModel(Tucuxi::Core::ResidualErrorType::PROPORTIONAL);
    residualErrorModel.setSigma(sigma);

    double theEta = 0.1;
    Etas etas(1);
    etas[0] = theEta;

    auto omega = Tucuxi::Core::OmegaMatrix(1, 1);
    omega(0, 0) = 0.1; // Variance of A

    // Three samples at date 06/06/2017.
    // First at 12:30 : value 200.0
    // Second at 16:30 : value 200.0
    // Third at 17:30 : value 100.0
    SampleSeries samples;
    SampleEvent sample1(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(6)),
                    Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
            200.0);
    samples.push_back(sample1);
    SampleEvent sample2(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(6)),
                    Duration(std::chrono::hours(16), std::chrono::minutes(30), std::chrono::seconds(0))),
            200.0);
    samples.push_back(sample2);
    SampleEvent sample3(
            DateTime(
                    date::year_month_day(date::year(2017), date::month(6), date::day(6)),
                    Duration(std::chrono::hours(17), std::chrono::minutes(30), std::chrono::seconds(0))),
            100.0);
    samples.push_back(sample3);


    Likelihood likelihood(omega, residualErrorModel, samples, intakes, parameters, calculator);

    Value res = likelihood.negativeLogLikelihood(etas);
    // We compute the expected result

    // Prior: 0.5 * (_etas.transpose() * m_inverseOmega * _etas + m_omegaAdd)
    // m_omega = m_omegaAdd(static_cast<double>(_omega.rows()) * log(2 * PI) + log(_omega.determinant()))
    // sample likelihood: - _residualErrorModel.calculateSampleLikelihood(_expected, _observed.getValue());
    double expectedSampleValue1 = 121.1;
    double expectedSampleValue2 = 41.1;
    double expectedSampleValue3 = 21.1;
    double omegaAdd = static_cast<double>(omega.rows()) * log(2 * PI) + log(omega.determinant());

    EigenVector etasmd(1);
    etasmd[0] = theEta;

    double expectedValue = 0.5 * (etasmd.transpose() * omega.inverse() * etasmd + omegaAdd)
                           - residualErrorModel.calculateSampleLikelihood(expectedSampleValue1, sample1.getValue())
                           - residualErrorModel.calculateSampleLikelihood(expectedSampleValue2, sample2.getValue())
                           - residualErrorModel.calculateSampleLikelihood(expectedSampleValue3, sample3.getValue());
    ASSERT_DOUBLE_EQ(res, expectedValue);
}


