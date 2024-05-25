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


#include <memory>

#include <gtest/gtest.h>

#include "tucucore/dosage.h"
#include "tucucore/drugmodel/formulationandroute.h"
#include "tucucore/intakeevent.h"
#include "tucucore/intakeextractor.h"
#include "tucucore/intakeintervalcalculator.h"
#include "tucucore/likelihood.h"
#include "tucucore/multiconcentrationcalculator.h"
#include "tucucore/multilikelihood.h"
#include "tucucore/residualerrormodel.h"

#include "gtest_core.h"
#include "pkmodels/constanteliminationbolus.h"
#include "pkmodels/multiconstanteliminationbolus.h"


using namespace Tucuxi::Core;

TEST(Core_TestMultiLikeliHood, oneAnalyte1Sample)
{
    //first scenario: Only a single analyte and one sample (Using ConstantEliminationBolus intakes)
#if GTEST_VERBOSE
    std::cout << __FUNCTION__ << std::endl;
#endif

    Tucuxi::Core::OmegaMatrix omega;
    std::vector<IResidualErrorModel*> residualErrorModel;
    std::vector<SampleSeries> samples;
    IntakeSeries intakes;
    ParameterSetSeries parameters;
    MultiConcentrationCalculator concentrationCalculator;
    Etas etas;

    //definition of the omega matrix
    omega = Tucuxi::Core::OmegaMatrix(1, 1);
    omega(0, 0) = 0.1; // Variance of A



    //definition of the residualErrorModel


    std::unique_ptr<SigmaResidualErrorModel> newErrorModel = std::make_unique<SigmaResidualErrorModel>();
    Tucuxi::Core::Sigma sigma(1);
    sigma(0) = 0.3138;
    newErrorModel->setErrorModel(Tucuxi::Core::ResidualErrorType::PROPORTIONAL);
    newErrorModel->setSigma(sigma);
    residualErrorModel.push_back(newErrorModel.get());
    //here i'm supposed to use a vector of pointers of IResidualErrorModel or a vector of pointers of SigmaResidualErrorModel

    //definition of the samples
    Tucuxi::Core::SampleSeries sampleSeries;
    DateTime date0 = DateTime(
            date::year_month_day(date::year(2017), date::month(6), date::day(6)),
            Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0)));

    Tucuxi::Core::SampleEvent s0(date0, 200.0);
    sampleSeries.push_back(s0);

    samples.push_back(sampleSeries);



    //definition of the intakes


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

    std::shared_ptr<Tucuxi::Core::IntakeIntervalCalculator> calculator =
            std::make_shared<Tucuxi::Core::ConstantEliminationBolus>();
    intakes[0].setCalculator(calculator);
    intakes[1].setCalculator(calculator);


    //Definition of the parameters


    Tucuxi::Core::ParameterDefinitions parameterDefs;
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestA", 1.0, Tucuxi::Core::ParameterVariabilityType::Additive));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestM", 1.0, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestR", 0.0, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestS", 0.0, Tucuxi::Core::ParameterVariabilityType::None));
    Tucuxi::Core::ParameterSetEvent parameterset(DateTime::now(), parameterDefs);
    parameters.addParameterSetEvent(parameterset);




    Tucuxi::Core::MultiLikelihood multilikelihood(
            omega, residualErrorModel, samples, intakes, parameters, concentrationCalculator);

    // Set initial etas to 0 for CL and V

    etas.push_back(0.1);
    //etas.push_back(0.0);

    Value x = multilikelihood.negativeLogLikelihood(etas);

    // We compute the expected result

    double expectedSampleValue = 201.1;
    double omegaAdd = static_cast<double>(omega.rows()) * log(2 * PI) + log(omega.determinant());

    EigenVector etasmd(1);
    etasmd[0] = 0.1;

    double expectedValue = 0.5 * (etasmd.transpose() * omega.inverse() * etasmd + omegaAdd)
                           - residualErrorModel[0]->calculateSampleLikelihood(expectedSampleValue, s0.getValue());
    ASSERT_DOUBLE_EQ(x, expectedValue);
}

TEST(Core_TestMultiLikeliHood, oneAnalyte3Samples)
{
    //Second scenario: A single analyte and 3 samples
#if GTEST_VERBOSE
    std::cout << __FUNCTION__ << std::endl;
#endif

    Tucuxi::Core::OmegaMatrix omega;
    std::vector<IResidualErrorModel*> residualErrorModel;
    std::vector<SampleSeries> samples;
    IntakeSeries intakes;
    ParameterSetSeries parameters;
    MultiConcentrationCalculator concentrationCalculator;
    Etas etas;

    //definition of the omega matrix
    omega = Tucuxi::Core::OmegaMatrix(1, 1);
    omega(0, 0) = 0.1; // Variance of A



    //definition of the residualErrorModel


    std::unique_ptr<SigmaResidualErrorModel> newErrorModel = std::make_unique<SigmaResidualErrorModel>();
    Tucuxi::Core::Sigma sigma(1);
    sigma(0) = 0.3138;
    newErrorModel->setErrorModel(Tucuxi::Core::ResidualErrorType::PROPORTIONAL);
    newErrorModel->setSigma(sigma);
    residualErrorModel.push_back(newErrorModel.get());
    //here i'm supposed to use a vector of pointers of IResidualErrorModel or a vector of pointers of SigmaResidualErrorModel

    //definition of the samples
    Tucuxi::Core::SampleSeries sampleSeries;

    DateTime date0 = DateTime(
            date::year_month_day(date::year(2017), date::month(6), date::day(6)),
            Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0)));
    Tucuxi::Core::SampleEvent s0(date0, 200.0);
    sampleSeries.push_back(s0);

    DateTime date1 = DateTime(
            date::year_month_day(date::year(2017), date::month(6), date::day(6)),
            Duration(std::chrono::hours(16), std::chrono::minutes(30), std::chrono::seconds(0)));
    Tucuxi::Core::SampleEvent s1(date1, 200.0);
    sampleSeries.push_back(s1);

    DateTime date2 = DateTime(
            date::year_month_day(date::year(2017), date::month(6), date::day(6)),
            Duration(std::chrono::hours(17), std::chrono::minutes(30), std::chrono::seconds(0)));
    Tucuxi::Core::SampleEvent s2(date2, 200.0);
    sampleSeries.push_back(s2);
    samples.push_back(sampleSeries);



    //definition of the intakes


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


    std::shared_ptr<Tucuxi::Core::IntakeIntervalCalculator> calculator =
            std::make_shared<Tucuxi::Core::ConstantEliminationBolus>();
    intakes[0].setCalculator(calculator);
    intakes[1].setCalculator(calculator);


    //Definition of the parameters


    Tucuxi::Core::ParameterDefinitions parameterDefs;
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestA", 1.0, Tucuxi::Core::ParameterVariabilityType::Additive));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestM", 1.0, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestR", 0.0, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestS", 0.1, Tucuxi::Core::ParameterVariabilityType::None));
    Tucuxi::Core::ParameterSetEvent parameterset(DateTime::now(), parameterDefs);
    parameters.addParameterSetEvent(parameterset);

    Tucuxi::Core::MultiLikelihood multilikelihood(
            omega, residualErrorModel, samples, intakes, parameters, concentrationCalculator);

    // Set initial etas to 0 for CL and V

    etas.push_back(0.1);
    //etas.push_back(0.0);

    Value x = multilikelihood.negativeLogLikelihood(etas);

    // We compute the expected result

    double expectedSampleValue0 = 121.1;
    double expectedSampleValue1 = 41.1;
    double expectedSampleValue2 = 21.1;
    double omegaAdd = static_cast<double>(omega.rows()) * log(2 * PI) + log(omega.determinant());

    EigenVector etasmd(1);
    etasmd[0] = 0.1;

    double expectedValue = 0.5 * (etasmd.transpose() * omega.inverse() * etasmd + omegaAdd)
                           - residualErrorModel[0]->calculateSampleLikelihood(expectedSampleValue0, s0.getValue())
                           - residualErrorModel[0]->calculateSampleLikelihood(expectedSampleValue1, s1.getValue())
                           - residualErrorModel[0]->calculateSampleLikelihood(expectedSampleValue2, s2.getValue());
    ASSERT_DOUBLE_EQ(x, expectedValue);
}

TEST(Core_TestMultiLikeliHood, DISABLED_twoAnalytes1SampleOnAnalyte1)
{
    // Third scenario: A 2-analyte with one sample on analyte 1 (using MultiConstantEliminationBolus)
#if GTEST_VERBOSE
    std::cout << __FUNCTION__ << std::endl;
#endif

    Tucuxi::Core::OmegaMatrix omega;
    std::vector<IResidualErrorModel*> residualErrorModel;
    std::vector<SampleSeries> samples;
    IntakeSeries intakes;
    ParameterSetSeries parameters;
    MultiConcentrationCalculator concentrationCalculator;
    Etas etas;

    //definition of the omega matrix
    omega = Tucuxi::Core::OmegaMatrix(2, 2);
    omega(0, 0) = 0.1; // Variance of A
    omega(1, 1) = 0.1;


    //definition of the residualErrorModel


    std::unique_ptr<SigmaResidualErrorModel> newErrorModel0 = std::make_unique<SigmaResidualErrorModel>();
    Tucuxi::Core::Sigma sigma0(1);
    sigma0(0) = 0.3138;
    newErrorModel0->setErrorModel(Tucuxi::Core::ResidualErrorType::PROPORTIONAL);
    newErrorModel0->setSigma(sigma0);
    residualErrorModel.push_back(newErrorModel0.get());
    //here i'm supposed to use a vector of pointers of IResidualErrorModel or a vector of pointers of SigmaResidualErrorModel


    std::unique_ptr<SigmaResidualErrorModel> newErrorModel1 = std::make_unique<SigmaResidualErrorModel>();
    Tucuxi::Core::Sigma sigma1(1);
    sigma1(0) = 0.3138;
    newErrorModel1->setErrorModel(Tucuxi::Core::ResidualErrorType::PROPORTIONAL);
    newErrorModel1->setSigma(sigma0);
    residualErrorModel.push_back(newErrorModel1.get());
    //here i'm supposed to use a vector of pointers of IResidualErrorModel or a vector of pointers of SigmaResidualErrorModel

    //definition of the samples
    Tucuxi::Core::SampleSeries sampleSeries0;
    DateTime date0 = DateTime(
            date::year_month_day(date::year(2017), date::month(6), date::day(6)),
            Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0)));
    Tucuxi::Core::SampleEvent s0(date0, 200.0);
    sampleSeries0.push_back(s0);
    samples.push_back(sampleSeries0);

    Tucuxi::Core::SampleSeries sampleSeries1;
    samples.push_back(sampleSeries1);


    //definition of the intakes


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
    std::shared_ptr<Tucuxi::Core::IntakeIntervalCalculator> calculator =
            std::make_shared<Tucuxi::Core::MultiConstantEliminationBolus>();
    intakes[0].setCalculator(calculator);
    intakes[1].setCalculator(calculator);


    //Definition of the parameters


    Tucuxi::Core::ParameterDefinitions parameterDefs;
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestA0", 1.0, Tucuxi::Core::ParameterVariabilityType::Additive));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestM0", 1.0, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestR0", 0.0, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestS0", 0.0, Tucuxi::Core::ParameterVariabilityType::None));

    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestA1", 1.0, Tucuxi::Core::ParameterVariabilityType::Additive));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestM1", 1.0, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestR1", 0.0, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestS1", 0.0, Tucuxi::Core::ParameterVariabilityType::None));

    Tucuxi::Core::ParameterSetEvent parameterset(DateTime::now(), parameterDefs);
    parameters.addParameterSetEvent(parameterset);



    Tucuxi::Core::MultiLikelihood multilikelihood(
            omega, residualErrorModel, samples, intakes, parameters, concentrationCalculator);

    // Set initial etas to 0 for CL and V

    etas.push_back(0.1);
    etas.push_back(0.1);

    Value x = multilikelihood.negativeLogLikelihood(etas);

    // We compute the expected result

    double expectedSampleValue = 201.1;
    double omegaAdd = static_cast<double>(omega.rows()) * log(2 * PI) + log(omega.determinant());

    EigenVector etasmd(2);
    etasmd[0] = 0.1;
    etasmd[1] = 0.1;

    double expectedValuestep0 = 0.5 * (etasmd.transpose() * omega.inverse() * etasmd + omegaAdd);



    double expectedValuestep1 = expectedValuestep0
                                - residualErrorModel[0]->calculateSampleLikelihood(
                                        expectedSampleValue, s0.getValue()); //0 as the second sample is empty

    ASSERT_PRED4(double_ne_rel_abs, x, std::numeric_limits<double>::max(), DEFAULT_PRECISION, DEFAULT_PRECISION);
    ASSERT_DOUBLE_EQ(x, expectedValuestep1);
}

TEST(Core_TestMultiLikeliHood, DISABLED_twoAnalytes1SampleOnAnalyte2)
{
    //A 2-analyte with one sample on analyte 2
#if GTEST_VERBOSE
    std::cout << __FUNCTION__ << std::endl;
#endif

    Tucuxi::Core::OmegaMatrix omega;
    std::vector<IResidualErrorModel*> residualErrorModel;
    std::vector<SampleSeries> samples;
    IntakeSeries intakes;
    ParameterSetSeries parameters;
    MultiConcentrationCalculator concentrationCalculator;
    Etas etas;

    //definition of the omega matrix
    omega = Tucuxi::Core::OmegaMatrix(2, 2);
    omega(0, 0) = 0.1; // Variance of A
    omega(1, 1) = 0.1;



    //definition of the residualErrorModel


    std::unique_ptr<SigmaResidualErrorModel> newErrorModel0 = std::make_unique<SigmaResidualErrorModel>();
    Tucuxi::Core::Sigma sigma0(1);
    sigma0(0) = 0.3138;
    newErrorModel0->setErrorModel(Tucuxi::Core::ResidualErrorType::PROPORTIONAL);
    newErrorModel0->setSigma(sigma0);
    residualErrorModel.push_back(newErrorModel0.get());
    //here i'm supposed to use a vector of pointers of IResidualErrorModel or a vector of pointers of SigmaResidualErrorModel


    std::unique_ptr<SigmaResidualErrorModel> newErrorModel1 = std::make_unique<SigmaResidualErrorModel>();
    Tucuxi::Core::Sigma sigma1(1);
    sigma1(0) = 0.3138;
    newErrorModel1->setErrorModel(Tucuxi::Core::ResidualErrorType::PROPORTIONAL);
    newErrorModel1->setSigma(sigma0);
    residualErrorModel.push_back(newErrorModel1.get());
    //here i'm supposed to use a vector of pointers of IResidualErrorModel or a vector of pointers of SigmaResidualErrorModel

    //definition of the samples
    Tucuxi::Core::SampleSeries sampleSeries0;
    samples.push_back(sampleSeries0);

    Tucuxi::Core::SampleSeries sampleSeries1;
    DateTime date0 = DateTime(
            date::year_month_day(date::year(2017), date::month(6), date::day(6)),
            Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0)));
    Tucuxi::Core::SampleEvent s1(date0, 200.0);
    sampleSeries1.push_back(s1);
    samples.push_back(sampleSeries1);


    //definition of the intakes


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
    std::shared_ptr<Tucuxi::Core::IntakeIntervalCalculator> calculator =
            std::make_shared<Tucuxi::Core::MultiConstantEliminationBolus>();
    intakes[0].setCalculator(calculator);
    intakes[1].setCalculator(calculator);


    //Definition of the parameters


    Tucuxi::Core::ParameterDefinitions parameterDefs;
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestA0", 1.0, Tucuxi::Core::ParameterVariabilityType::Additive));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestM0", 1.0, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestR0", 0.0, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestS0", 0.0, Tucuxi::Core::ParameterVariabilityType::None));

    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestA1", 1.0, Tucuxi::Core::ParameterVariabilityType::Additive));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestM1", 1.0, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestR1", 0.0, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestS1", 0.0, Tucuxi::Core::ParameterVariabilityType::None));

    Tucuxi::Core::ParameterSetEvent parameterset(DateTime::now(), parameterDefs);
    parameters.addParameterSetEvent(parameterset);

    Tucuxi::Core::MultiLikelihood multilikelihood(
            omega, residualErrorModel, samples, intakes, parameters, concentrationCalculator);

    // Set initial etas to 0 for CL and V

    etas.push_back(0.1);
    etas.push_back(0.1);

    Value x = multilikelihood.negativeLogLikelihood(etas);

    // We compute the expected result

    double expectedSampleValue = 201.1;
    double omegaAdd = static_cast<double>(omega.rows()) * log(2 * PI) + log(omega.determinant());

    EigenVector etasmd(2);
    etasmd[0] = 0.1;
    etasmd[1] = 0.1;

    double expectedValue = 0.5 * (etasmd.transpose() * omega.inverse() * etasmd + omegaAdd)
                           - residualErrorModel[1]->calculateSampleLikelihood(
                                   expectedSampleValue, s1.getValue()); //0 as the first sample is empty
    ASSERT_PRED4(double_ne_rel_abs, x, std::numeric_limits<double>::max(), DEFAULT_PRECISION, DEFAULT_PRECISION);
    ASSERT_DOUBLE_EQ(x, expectedValue);
}

TEST(Core_TestMultiLikeliHood, DISABLED_twoAnalytes2SamplesDifferentTimes)
{
//A 2-analyte with one sample per analytes at different times
#if GTEST_VERBOSE
    std::cout << __FUNCTION__ << std::endl;
#endif


    Tucuxi::Core::OmegaMatrix omega;
    std::vector<IResidualErrorModel*> residualErrorModel;
    std::vector<SampleSeries> samples;
    IntakeSeries intakes;
    ParameterSetSeries parameters;
    MultiConcentrationCalculator concentrationCalculator;
    Etas etas;


    omega = Tucuxi::Core::OmegaMatrix(2, 2);
    omega(0, 0) = 0.1; // Variance of A
    omega(1, 1) = 0.1;

    //definition of the residualErrorModel


    std::unique_ptr<SigmaResidualErrorModel> newErrorModel0 = std::make_unique<SigmaResidualErrorModel>();
    Tucuxi::Core::Sigma sigma0(1);
    sigma0(0) = 0.3138;
    newErrorModel0->setErrorModel(Tucuxi::Core::ResidualErrorType::PROPORTIONAL);
    newErrorModel0->setSigma(sigma0);
    residualErrorModel.push_back(newErrorModel0.get());
    //here i'm supposed to use a vector of pointers of IResidualErrorModel or a vector of pointers of SigmaResidualErrorModel


    std::unique_ptr<SigmaResidualErrorModel> newErrorModel1 = std::make_unique<SigmaResidualErrorModel>();
    Tucuxi::Core::Sigma sigma1(1);
    sigma1(0) = 0.3138;
    newErrorModel1->setErrorModel(Tucuxi::Core::ResidualErrorType::PROPORTIONAL);
    newErrorModel1->setSigma(sigma0);
    residualErrorModel.push_back(newErrorModel1.get());
    //here i'm supposed to use a vector of pointers of IResidualErrorModel or a vector of pointers of SigmaResidualErrorModel

    //definition of the samples
    Tucuxi::Core::SampleSeries sampleSeries0;
    DateTime date0 = DateTime(
            date::year_month_day(date::year(2017), date::month(6), date::day(6)),
            Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0)));
    Tucuxi::Core::SampleEvent s0(date0, 200.0);
    sampleSeries0.push_back(s0);
    samples.push_back(sampleSeries0);

    Tucuxi::Core::SampleSeries sampleSeries1;
    DateTime date1 = DateTime(
            date::year_month_day(date::year(2017), date::month(6), date::day(6)),
            Duration(std::chrono::hours(16), std::chrono::minutes(30), std::chrono::seconds(0)));
    Tucuxi::Core::SampleEvent s1(date1, 200.0);
    sampleSeries1.push_back(s1);
    samples.push_back(sampleSeries1);


    //definition of the intakes


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
    std::shared_ptr<Tucuxi::Core::IntakeIntervalCalculator> calculator =
            std::make_shared<Tucuxi::Core::MultiConstantEliminationBolus>();
    intakes[0].setCalculator(calculator);
    intakes[1].setCalculator(calculator);


    //Definition of the parameters


    Tucuxi::Core::ParameterDefinitions parameterDefs;
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestA0", 1.0, Tucuxi::Core::ParameterVariabilityType::Additive));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestM0", 1.0, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestR0", 0.0, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestS0", 0.1, Tucuxi::Core::ParameterVariabilityType::None));

    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestA1", 1.0, Tucuxi::Core::ParameterVariabilityType::Additive));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestM1", 1.0, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestR1", 0.0, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestS1", 0.1, Tucuxi::Core::ParameterVariabilityType::None));

    Tucuxi::Core::ParameterSetEvent parameterset(DateTime::now(), parameterDefs);
    parameters.addParameterSetEvent(parameterset);




    Tucuxi::Core::MultiLikelihood multilikelihood(
            omega, residualErrorModel, samples, intakes, parameters, concentrationCalculator);

    // Set initial etas to 0 for CL and V

    etas.push_back(0.1);
    etas.push_back(0.1);

    Value x = multilikelihood.negativeLogLikelihood(etas);

    // We compute the expected result

    // Prior: 0.5 * (etas.transpose() * m_inverseOmega * etas + m_omegaAdd)
    // m_omega = m_omegaAdd(static_cast<double>(_omega.rows()) * log(2 * PI) + log(_omega.determinant()))
    // sample likelihood: - _residualErrorModel.calculateSampleLikelihood(_expected, _observed.getValue());
    double expectedSampleValue0 = 121.1;
    double expectedSampleValue1 = 41.1;
    double omegaAdd = static_cast<double>(omega.rows()) * log(2 * PI) + log(omega.determinant());

    EigenVector etasmd(2);
    etasmd[0] = 0.1;
    etasmd[1] = 0.1;

    double expectedValue = 0.5 * (etasmd.transpose() * omega.inverse() * etasmd + omegaAdd)
                           - residualErrorModel[0]->calculateSampleLikelihood(expectedSampleValue0, s0.getValue())
                           - residualErrorModel[1]->calculateSampleLikelihood(expectedSampleValue1, s1.getValue());
    ASSERT_PRED4(double_ne_rel_abs, x, std::numeric_limits<double>::max(), DEFAULT_PRECISION, DEFAULT_PRECISION);
    ASSERT_DOUBLE_EQ(x, expectedValue);
}

TEST(Core_TestMultiLikeliHood, DISABLED_twoAnalytes2SamplesSameTime)
{
    //A 2-analyte with one sample per analytes at the same time
#if GTEST_VERBOSE
    std::cout << __FUNCTION__ << std::endl;
#endif

    Tucuxi::Core::OmegaMatrix omega;
    std::vector<IResidualErrorModel*> residualErrorModel;
    std::vector<SampleSeries> samples;
    IntakeSeries intakes;
    ParameterSetSeries parameters;
    MultiConcentrationCalculator concentrationCalculator;
    Etas etas;

    //definition of the omega matrix
    omega = Tucuxi::Core::OmegaMatrix(2, 2);
    omega(0, 0) = 0.1; // Variance of A
    omega(1, 1) = 0.1;


    //definition of the residualErrorModel


    std::unique_ptr<SigmaResidualErrorModel> newErrorModel0 = std::make_unique<SigmaResidualErrorModel>();
    Tucuxi::Core::Sigma sigma0(1);
    sigma0(0) = 0.3138;
    newErrorModel0->setErrorModel(Tucuxi::Core::ResidualErrorType::PROPORTIONAL);
    newErrorModel0->setSigma(sigma0);
    residualErrorModel.push_back(newErrorModel0.get());
    //here i'm supposed to use a vector of pointers of IResidualErrorModel or a vector of pointers of SigmaResidualErrorModel


    std::unique_ptr<SigmaResidualErrorModel> newErrorModel1 = std::make_unique<SigmaResidualErrorModel>();
    Tucuxi::Core::Sigma sigma1(1);
    sigma1(0) = 0.3138;
    newErrorModel1->setErrorModel(Tucuxi::Core::ResidualErrorType::PROPORTIONAL);
    newErrorModel1->setSigma(sigma0);
    residualErrorModel.push_back(newErrorModel1.get());
    //here i'm supposed to use a vector of pointers of IResidualErrorModel or a vector of pointers of SigmaResidualErrorModel

    //definition of the samples
    Tucuxi::Core::SampleSeries sampleSeries0;
    DateTime date0 = DateTime(
            date::year_month_day(date::year(2017), date::month(6), date::day(6)),
            Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0)));
    Tucuxi::Core::SampleEvent s0(date0, 200.0);
    sampleSeries0.push_back(s0);
    samples.push_back(sampleSeries0);

    Tucuxi::Core::SampleSeries sampleSeries1;
    DateTime date1 = DateTime(
            date::year_month_day(date::year(2017), date::month(6), date::day(6)),
            Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0)));
    Tucuxi::Core::SampleEvent s1(date1, 200.0);
    sampleSeries1.push_back(s1);
    samples.push_back(sampleSeries1);


    //definition of the intakes


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
    std::shared_ptr<Tucuxi::Core::IntakeIntervalCalculator> calculator =
            std::make_shared<Tucuxi::Core::MultiConstantEliminationBolus>();
    intakes[0].setCalculator(calculator);
    intakes[1].setCalculator(calculator);


    //Definition of the parameters


    Tucuxi::Core::ParameterDefinitions parameterDefs;
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestA0", 1.0, Tucuxi::Core::ParameterVariabilityType::Additive));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestM0", 1.0, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestR0", 0.0, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestS0", 0.0, Tucuxi::Core::ParameterVariabilityType::None));

    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestA1", 1.0, Tucuxi::Core::ParameterVariabilityType::Additive));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestM1", 1.0, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestR1", 0.0, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestS1", 0.0, Tucuxi::Core::ParameterVariabilityType::None));
    Tucuxi::Core::ParameterSetEvent parameterset(DateTime::now(), parameterDefs);
    parameters.addParameterSetEvent(parameterset);


    Tucuxi::Core::MultiLikelihood multilikelihood(
            omega, residualErrorModel, samples, intakes, parameters, concentrationCalculator);

    // Set initial etas to 0 for CL and V

    etas.push_back(0.1);
    etas.push_back(0.1);

    Value x = multilikelihood.negativeLogLikelihood(etas);

    // We compute the expected result

    double expectedSampleValue = 201.1;
    double omegaAdd = static_cast<double>(omega.rows()) * log(2 * PI) + log(omega.determinant());

    EigenVector etasmd(2);
    etasmd[0] = 0.1;
    etasmd[1] = 0.1;

    double expectedValue = 0.5 * (etasmd.transpose() * omega.inverse() * etasmd + omegaAdd)
                           - residualErrorModel[0]->calculateSampleLikelihood(expectedSampleValue, s0.getValue())
                           - residualErrorModel[1]->calculateSampleLikelihood(
                                   expectedSampleValue,
                                   s1.getValue()); //WE CAN NOT USE 2.0 * m_residual... as they are 2 different analytes
    ASSERT_PRED4(double_ne_rel_abs, x, std::numeric_limits<double>::max(), DEFAULT_PRECISION, DEFAULT_PRECISION);
    ASSERT_DOUBLE_EQ(x, expectedValue);
}

TEST(Core_TestMultiLikeliHood, DISABLED_twoAnalytes6Samples)
{
    //A 2-analyte with three samples per analyte, with one of them at the same time, the others at different times.
#if GTEST_VERBOSE
    std::cout << __FUNCTION__ << std::endl;
#endif

    Tucuxi::Core::OmegaMatrix omega;
    std::vector<IResidualErrorModel*> residualErrorModel;
    std::vector<SampleSeries> samples;
    IntakeSeries intakes;
    ParameterSetSeries parameters;
    MultiConcentrationCalculator concentrationCalculator;
    Etas etas;

    //definition of the omega matrix
    omega = Tucuxi::Core::OmegaMatrix(2, 2);
    omega(0, 0) = 0.1; // Variance of A
    omega(1, 1) = 0.1;

    //definition of the residualErrorModel


    std::unique_ptr<SigmaResidualErrorModel> newErrorModel0 = std::make_unique<SigmaResidualErrorModel>();
    Tucuxi::Core::Sigma sigma0(1);
    sigma0(0) = 0.3138;
    newErrorModel0->setErrorModel(Tucuxi::Core::ResidualErrorType::PROPORTIONAL);
    newErrorModel0->setSigma(sigma0);
    residualErrorModel.push_back(newErrorModel0.get());
    //here i'm supposed to use a vector of pointers of IResidualErrorModel or a vector of pointers of SigmaResidualErrorModel


    std::unique_ptr<SigmaResidualErrorModel> newErrorModel1 = std::make_unique<SigmaResidualErrorModel>();
    Tucuxi::Core::Sigma sigma1(1);
    sigma1(0) = 0.3138;
    newErrorModel1->setErrorModel(Tucuxi::Core::ResidualErrorType::PROPORTIONAL);
    newErrorModel1->setSigma(sigma0);
    residualErrorModel.push_back(newErrorModel1.get());
    //here i'm supposed to use a vector of pointers of IResidualErrorModel or a vector of pointers of SigmaResidualErrorModel

    //definition of the samples
    Tucuxi::Core::SampleSeries sampleSeries0;

    DateTime date0 = DateTime(
            date::year_month_day(date::year(2017), date::month(6), date::day(6)),
            Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0)));
    Tucuxi::Core::SampleEvent s0(date0, 200.0);
    sampleSeries0.push_back(s0);

    DateTime date1 = DateTime(
            date::year_month_day(date::year(2017), date::month(6), date::day(6)),
            Duration(std::chrono::hours(16), std::chrono::minutes(30), std::chrono::seconds(0)));
    Tucuxi::Core::SampleEvent s1(date1, 200.0);
    sampleSeries0.push_back(s1);

    DateTime date2 = DateTime(
            date::year_month_day(date::year(2017), date::month(6), date::day(6)),
            Duration(std::chrono::hours(17), std::chrono::minutes(30), std::chrono::seconds(0)));
    Tucuxi::Core::SampleEvent s2(date2, 100.0);
    sampleSeries0.push_back(s2);


    samples.push_back(sampleSeries0);

    Tucuxi::Core::SampleSeries sampleSeries1;

    DateTime date3 = DateTime(
            date::year_month_day(date::year(2017), date::month(6), date::day(6)),
            Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0)));
    Tucuxi::Core::SampleEvent s3(date3, 200.0);
    sampleSeries1.push_back(s3);

    DateTime date4 = DateTime(
            date::year_month_day(date::year(2017), date::month(6), date::day(6)),
            Duration(std::chrono::hours(13), std::chrono::minutes(30), std::chrono::seconds(0)));
    Tucuxi::Core::SampleEvent s4(date4, 200.0);
    sampleSeries1.push_back(s4);

    DateTime date5 = DateTime(
            date::year_month_day(date::year(2017), date::month(6), date::day(6)),
            Duration(std::chrono::hours(15), std::chrono::minutes(30), std::chrono::seconds(0)));
    Tucuxi::Core::SampleEvent s5(date5, 100.0);
    sampleSeries1.push_back(s5);

    samples.push_back(sampleSeries1);


    //definition of the intakes


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
    std::shared_ptr<Tucuxi::Core::IntakeIntervalCalculator> calculator =
            std::make_shared<Tucuxi::Core::MultiConstantEliminationBolus>();
    intakes[0].setCalculator(calculator);
    intakes[1].setCalculator(calculator);


    //Definition of the parameters


    Tucuxi::Core::ParameterDefinitions parameterDefs;
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestA0", 1.0, Tucuxi::Core::ParameterVariabilityType::Additive));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestM0", 1.0, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestR0", 0.0, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestS0", 0.1, Tucuxi::Core::ParameterVariabilityType::None));

    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestA1", 1.0, Tucuxi::Core::ParameterVariabilityType::Additive));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestM1", 10.0, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestR1", 0.0, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestS1", 0.1, Tucuxi::Core::ParameterVariabilityType::None));
    Tucuxi::Core::ParameterSetEvent parameterset(DateTime::now(), parameterDefs);
    parameters.addParameterSetEvent(parameterset);



    Tucuxi::Core::MultiLikelihood multilikelihood(
            omega, residualErrorModel, samples, intakes, parameters, concentrationCalculator);

    // Set initial etas to 0 for CL and V

    etas.push_back(0.1);
    etas.push_back(0.1);

    Value x = multilikelihood.negativeLogLikelihood(etas);

    // We compute the expected result


    double expectedSampleValue0 = 121.1;
    double expectedSampleValue1 = 41.1;
    double expectedSampleValue2 = 21.1;
    double expectedSampleValue3 = 1201.1;
    double expectedSampleValue4 = 1001.1;
    double expectedSampleValue5 = 601.1;
    double omegaAdd = static_cast<double>(omega.rows()) * log(2 * PI) + log(omega.determinant());

    EigenVector etasmd(2);
    etasmd[0] = 0.1;
    etasmd[1] = 0.1;

    double expectedValue0 = 0.5 * (etasmd.transpose() * omega.inverse() * etasmd + omegaAdd)
                            - residualErrorModel[0]->calculateSampleLikelihood(expectedSampleValue0, s0.getValue())
                            - residualErrorModel[0]->calculateSampleLikelihood(expectedSampleValue1, s1.getValue())
                            - residualErrorModel[0]->calculateSampleLikelihood(expectedSampleValue2, s2.getValue());

    double expectedValue1 =
            expectedValue0 - residualErrorModel[1]->calculateSampleLikelihood(expectedSampleValue3, s3.getValue());

    double expectedValue2 =
            expectedValue1 - residualErrorModel[1]->calculateSampleLikelihood(expectedSampleValue4, s4.getValue());
    double expectedValue3 =
            expectedValue2 - residualErrorModel[1]->calculateSampleLikelihood(expectedSampleValue5, s5.getValue());


    ASSERT_PRED4(double_ne_rel_abs, x, std::numeric_limits<double>::max(), DEFAULT_PRECISION, DEFAULT_PRECISION);
    ASSERT_DOUBLE_EQ(x, expectedValue3);
}
