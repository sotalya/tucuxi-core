/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TEST_LIKELIHOOD_H
#define TEST_LIKELIHOOD_H

#include <iostream>
#include <memory>

#include "fructose/fructose.h"

#include "tucucommon/duration.h"

#include "tucucore/pkmodels/onecompartmentbolus.h"
#include "tucucore/pkmodels/onecompartmentextra.h"
#include "tucucore/pkmodels/onecompartmentinfusion.h"
#include "tucucore/pkmodels/onecompartmentextralag.h"
#include "tucucore/pkmodels/twocompartmentbolus.h"
#include "tucucore/pkmodels/twocompartmentextra.h"
#include "tucucore/pkmodels/twocompartmentinfusion.h"
#include "tucucore/pkmodels/twocompartmentextralag.h"
#include "tucucore/pkmodels/threecompartmentbolus.h"
#include "tucucore/pkmodels/threecompartmentextra.h"
#include "pkmodels/constanteliminationbolus.h"

#include "tucucore/pkmodels/rkonecompartmentextra.h"
#include "tucucore/pkmodels/rkonecompartmentgammaextra.h"
#include "tucucore/pkmodels/rktwocompartmenterlang.h"
#include "tucucore/pkmodels/rktwocompartmentextralag.h"

#include "tucucore/drugmodel/formulationandroute.h"
#include "tucucore/intakeevent.h"
#include "tucucore/likelihood.h"
#include "tucucore/residualerrormodel.h"
#include "tucucore/concentrationcalculator.h"

using namespace Tucuxi::Core;
using namespace std::chrono_literals;

struct TestLikelihood : public fructose::test_base<TestLikelihood>
{
    static const int CYCLE_SIZE = 251;

    TestLikelihood() { }


    FormulationAndRoute getBolusFormulationAndRoute()
    {
        return FormulationAndRoute(
                    Formulation::Test,
                    AdministrationRoute::IntravenousBolus,
                    AbsorptionModel::Intravascular);
    }

    /// \brief Test the likelihood calculation
    /// \param _testName Test name.
    /// A Likelihood object computes a negativeLogLikelihood.
    void testSimple(const std::string& /* _testName */)
    {
        if (verbose()) {
            std::cout << __FUNCTION__ << std::endl;
        }


        // Build the intake series. Here we have two intakes of 24h interval
        // Dates : 06/06/2017 at 8:30
        //         07/06/2017 at 8:30
        IntakeSeries intakes;
        intakes.emplace_back(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(6)),
                                      Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
                             Duration(),
                             DoseValue(200.0),
                             TucuUnit("mg"),
                             Duration(std::chrono::hours(24)),
                             getBolusFormulationAndRoute(),
                             getBolusFormulationAndRoute().getAbsorptionModel(),
                             Duration(std::chrono::minutes(20)),
                             static_cast<int>(CYCLE_SIZE));
        intakes.emplace_back(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(7)),
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

        // Only one sample at date 06/06/2017 at 12:30
        // Sample value: 200.0
        SampleEvent sample1(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(6)),
                                     Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
                            200.0);
        SampleSeries samples;
        samples.push_back(sample1);

        {
            // Here we have wrong parameters, and a matrix with a size corresponding to the number of variable parameters
            auto omega = Tucuxi::Core::OmegaMatrix(2,2);
            omega(0,0) = 0.356 * 0.356;
            omega(0,1) = 0.798 * 0.356 * 0.629;
            omega(1,1) = 0.629 * 0.629;
            omega(1,0) = 0.798 * 0.356 * 0.629;

            SigmaResidualErrorModel residualErrorModel;
            Tucuxi::Core::Sigma sigma(1);
            sigma(0) = 0.3138;
            residualErrorModel.setErrorModel(Tucuxi::Core::ResidualErrorType::PROPORTIONAL);
            residualErrorModel.setSigma(sigma);

            Tucuxi::Core::ParameterDefinitions microParameterDefs;
            microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>("V", 347, Tucuxi::Core::ParameterVariabilityType::Additive));
            microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>("Ke", 0.0435331, Tucuxi::Core::ParameterVariabilityType::Additive));
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
            fructose_assert_double_eq(res, std::numeric_limits<double>::max());
        }

        {
            // Here we build parameters that correspond to the intake calculator (ConstantEliminationBolus)
            Tucuxi::Core::ParameterDefinitions microParameterDefs;
            microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>("TestA", 1.0, Tucuxi::Core::ParameterVariabilityType::Additive));
            microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>("TestM", 1.0, Tucuxi::Core::ParameterVariabilityType::None));
            microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>("TestR", 0.0, Tucuxi::Core::ParameterVariabilityType::None));
            microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>("TestS", 0.0, Tucuxi::Core::ParameterVariabilityType::None));
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

            auto omega = Tucuxi::Core::OmegaMatrix(1,1);
            omega(0,0) = 0.1; // Variance of A

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

            double expectedValue = 0.5 * (etasmd.transpose() * omega.inverse() * etasmd + omegaAdd) -
                    residualErrorModel.calculateSampleLikelihood(expectedSampleValue, sample1.getValue());
            fructose_assert_double_eq(res, expectedValue);
        }

        {
            // Here we build parameters that correspond to the intake calculator (ConstantEliminationBolus)
            Tucuxi::Core::ParameterDefinitions microParameterDefs;
            microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>("TestA", 1.0, Tucuxi::Core::ParameterVariabilityType::Additive));
            microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>("TestM", 1.0, Tucuxi::Core::ParameterVariabilityType::None));
            microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>("TestR", 0.0, Tucuxi::Core::ParameterVariabilityType::None));
            microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>("TestS", 0.0, Tucuxi::Core::ParameterVariabilityType::None));
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

            auto omega = Tucuxi::Core::OmegaMatrix(1,1);
            omega(0,0) = 0.1; // Variance of A

            // Two samples at date 06/06/2017 at 12:30
            // Sample value: 200.0
            SampleSeries samples;
            SampleEvent sample1(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(6)),
                                         Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
                                200.0);
            samples.push_back(sample1);
            SampleEvent sample2(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(6)),
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

            double expectedValue = 0.5 * (etasmd.transpose() * omega.inverse() * etasmd + omegaAdd) -
                    2.0 * residualErrorModel.calculateSampleLikelihood(expectedSampleValue, sample1.getValue());
            fructose_assert_double_eq(res, expectedValue);
        }


        {
            // Here we build parameters that correspond to the intake calculator (ConstantEliminationBolus)
            Tucuxi::Core::ParameterDefinitions microParameterDefs;
            microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>("TestA", 1.0, Tucuxi::Core::ParameterVariabilityType::Additive));
            microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>("TestM", 1.0, Tucuxi::Core::ParameterVariabilityType::None));
            microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>("TestR", 0.0, Tucuxi::Core::ParameterVariabilityType::None));
            microParameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>("TestS", 0.1, Tucuxi::Core::ParameterVariabilityType::None));
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

            auto omega = Tucuxi::Core::OmegaMatrix(1,1);
            omega(0,0) = 0.1; // Variance of A

            // Two samples at date 06/06/2017.
            // First at 12:30
            // Second at 16:30
            // Sample value: 200.0
            SampleSeries samples;
            SampleEvent sample1(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(6)),
                                         Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))),
                                200.0);
            samples.push_back(sample1);
            SampleEvent sample2(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(6)),
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

            double expectedValue = 0.5 * (etasmd.transpose() * omega.inverse() * etasmd + omegaAdd) -
                    residualErrorModel.calculateSampleLikelihood(expectedSampleValue1, sample1.getValue()) -
                    residualErrorModel.calculateSampleLikelihood(expectedSampleValue2, sample2.getValue());
            fructose_assert_double_eq(res, expectedValue);
        }
    }
};

#endif // TEST_LIKELIHOOD_H
