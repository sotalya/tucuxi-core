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

#include "tucucore/computingservice/computingrequest.h"
#include "tucucore/computingservice/computingtrait.h"
#include "tucucore/drugtreatment/sample.h"
#include "tucucore/generalextractor.h"
#include "tucucore/pkmodel.h"

#include "drugmodels/buildconstantelimination.h"
#include "gtest_core.h"
#include "testutils.h"

using namespace Tucuxi::Core;


TEST(GeneralExtractorTest, SecureStartDurationConvertsDays)
{
    GeneralExtractor extractor;
    HalfLife halfLife("halfLife", 2.0, TucuUnit("d"), 3.0);

    EXPECT_EQ(extractor.secureStartDuration(halfLife), Tucuxi::Common::Duration(std::chrono::hours(144)));
}

TEST(GeneralExtractorTest, SecureStartDurationConvertsHours)
{
    GeneralExtractor extractor;
    HalfLife halfLife("halfLife", 4.0, TucuUnit("h"), 2.0);

    EXPECT_EQ(extractor.secureStartDuration(halfLife), Tucuxi::Common::Duration(std::chrono::hours(8)));
}

TEST(GeneralExtractorTest, SecureStartDurationConvertsMinutes)
{
    GeneralExtractor extractor;
    HalfLife halfLife("halfLife", 5.0, TucuUnit("min"), 2.0);

    EXPECT_EQ(extractor.secureStartDuration(halfLife), Tucuxi::Common::Duration(std::chrono::minutes(10)));
}

TEST(GeneralExtractorTest, SecureStartDurationConvertsSeconds)
{
    GeneralExtractor extractor;
    HalfLife halfLife("halfLife", 7.0, TucuUnit("s"), 3.0);

    EXPECT_EQ(extractor.secureStartDuration(halfLife), Tucuxi::Common::Duration(std::chrono::seconds(21)));
}

TEST(GeneralExtractorTest, ExtractOmegaCreatesDiagonalEntriesForAdditionalVariabilityValues)
{
    auto variability = std::make_unique<ParameterVariability>(
            ParameterVariabilityType::LogNormal, std::vector<Value>{2.0, 3.0, 4.0});
    auto parameter = std::make_unique<ParameterDefinition>("CL", 1.0, std::move(variability));

    auto dispositionParameters = std::make_unique<ParameterSetDefinition>();
    dispositionParameters->addParameter(std::move(parameter));

    auto analyteSet = std::make_unique<AnalyteSet>();
    analyteSet->setId(AnalyteGroupId("group"));
    analyteSet->setDispositionParameters(std::move(dispositionParameters));

    DrugModel drugModel;
    drugModel.addAnalyteSet(std::move(analyteSet));

    GeneralExtractor extractor;
    OmegaMatrix omega;
    std::vector<const FullFormulationAndRoute*> formulationAndRoutes;

    EXPECT_EQ(
            extractor.extractOmega(drugModel, AnalyteGroupId("group"), formulationAndRoutes, omega),
            ComputingStatus::Ok);
    ASSERT_EQ(omega.rows(), 3);
    ASSERT_EQ(omega.cols(), 3);
    EXPECT_DOUBLE_EQ(omega(0, 0), 4.0);
    EXPECT_DOUBLE_EQ(omega(1, 1), 9.0);
    EXPECT_DOUBLE_EQ(omega(2, 2), 16.0);
}

TEST(GeneralExtractorTest, GeneralExtractionsWithDoseCovariateOk)
{
    BuildConstantElimination builder;
    auto drugModel = builder.buildDrugModel();

    auto doseCovariate =
            std::make_unique<CovariateDefinition>("dose", "0.0", nullptr, CovariateType::Dose, DataType::Double);
    doseCovariate->setUnit(TucuUnit("mg"));
    drugModel->addCovariate(std::move(doseCovariate));

    const FormulationAndRoute route(Formulation::OralSolution, AdministrationRoute::Oral);
    DateTime start = DATE_TIME_NO_VAR(2018, 9, 1, 8, 0, 0);
    auto drugTreatment = buildDrugTreatment(route, start);

    PkModelCollection collection;
    defaultPopulate(collection);

    ComputingOption computingOption(PredictionParameterType::Aposteriori, CompartmentsOption::MainCompartment);
    ComputingTraitStandard traits("request", start, start + Duration(std::chrono::hours(24)), 10.0, computingOption);

    std::map<AnalyteGroupId, std::shared_ptr<PkModel> > pkModel;
    GroupsIntakeSeries intakeSeries;
    CovariateSeries covariateSeries;
    GroupsParameterSetSeries parameterSeries;
    DateTime calculationStartTime;
    GeneralExtractor extractor;

    EXPECT_EQ(
            extractor.generalExtractions(
                    &traits,
                    *drugModel,
                    drugTreatment->getDosageHistory(),
                    drugTreatment->getSamples(),
                    drugTreatment->getCovariates(),
                    &collection,
                    pkModel,
                    intakeSeries,
                    covariateSeries,
                    parameterSeries,
                    calculationStartTime),
            ComputingStatus::Ok);
}

// End-to-end test of the a-posteriori IOV path.
TEST(GeneralExtractorTest, AposterioriEtasWithIov)
{
    BuildConstantElimination builder;
    auto drugModel = builder.buildDrugModel(
            ResidualErrorType::ADDITIVE,
            std::vector<Value>({100.0}),
            ParameterVariabilityType::Additive, // BSV on TestA
            ParameterVariabilityType::None,
            ParameterVariabilityType::None,
            ParameterVariabilityType::None,
            10.0, // BSV standard deviation on TestA
            0.0,
            0.0,
            0.0,
            InterpolationType::Forward,
            InterpolationType::Forward,
            InterpolationType::Forward,
            InterpolationType::Forward,
            5.0); // IOV standard deviation on TestA

    const FormulationAndRoute route(Formulation::OralSolution, AdministrationRoute::Oral);
    DateTime start = DATE_TIME_NO_VAR(2018, 9, 1, 8, 0, 0);
    auto drugTreatment = buildDrugTreatment(route, start); // 16 doses, 6 h apart

    // Two samples in two different intervals = two IOV occasions.
    drugTreatment->addSample(std::make_unique<Sample>(
            start + Duration(std::chrono::hours(3)), AnalyteId("analyte"), 1.0, TucuUnit("mg/l")));
    drugTreatment->addSample(std::make_unique<Sample>(
            start + Duration(std::chrono::hours(27)), AnalyteId("analyte"), 2.0, TucuUnit("mg/l")));

    DateTime end = start + Duration(std::chrono::hours(96));
    ComputingOption computingOption(PredictionParameterType::Aposteriori, CompartmentsOption::MainCompartment);
    ComputingTraitStandard traits("request", start, end, 10.0, computingOption);

    PkModelCollection collection;
    defaultPopulate(collection);

    GeneralExtractor extractor;
    std::map<AnalyteGroupId, std::shared_ptr<PkModel> > pkModel;
    GroupsIntakeSeries intakeSeries;
    CovariateSeries covariateSeries;
    GroupsParameterSetSeries parameterSeries;
    DateTime calculationStartTime;

    ASSERT_EQ(
            extractor.generalExtractions(
                    &traits,
                    *drugModel,
                    drugTreatment->getDosageHistory(),
                    drugTreatment->getSamples(),
                    drugTreatment->getCovariates(),
                    &collection,
                    pkModel,
                    intakeSeries,
                    covariateSeries,
                    parameterSeries,
                    calculationStartTime),
            ComputingStatus::Ok);

    ASSERT_FALSE(parameterSeries.empty());
    AnalyteGroupId groupId = parameterSeries.begin()->first;

    std::unique_ptr<ComputingTraitConcentration> requestTraits =
            std::make_unique<ComputingTraitConcentration>("request", start, end, 10.0, computingOption);
    ComputingRequest request("request", *drugModel, *drugTreatment, std::move(requestTraits));

    Etas etas;
    Value negativeLogLikelihood = 0.0;
    ComputingStatus status = extractor.extractAposterioriEtas(
            etas,
            request,
            PredictionParameterType::Aposteriori,
            groupId,
            intakeSeries[groupId],
            parameterSeries[groupId],
            covariateSeries,
            calculationStartTime,
            end,
            negativeLogLikelihood);

    EXPECT_EQ(status, ComputingStatus::Ok);
    // One BSV eta for TestA. The IOV etas (one per sampled occasion) were used for
    // the fit and dropped afterwards, so they must NOT appear in the result.
    EXPECT_EQ(etas.size(), static_cast<size_t>(1));
}
