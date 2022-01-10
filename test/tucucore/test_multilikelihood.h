#ifndef TEST_MULTILIKELIHOOD_H
#define TEST_MULTILIKELIHOOD_H

#endif // TEST_MULTILIKELIHOOD_H

#include <iostream>
#include <memory>

#include "tucucommon/duration.h"
#include "tucucommon/general.h"

#include "tucucore/concentrationcalculator.h"
#include "tucucore/dosage.h"
#include "tucucore/drugmodel/formulationandroute.h"
#include "tucucore/intakeevent.h"
#include "tucucore/intakeextractor.h"
#include "tucucore/intakeintervalcalculator.h"
#include "tucucore/likelihood.h"
#include "tucucore/multiconcentrationcalculator.h"
#include "tucucore/multilikelihood.h"
#include "tucucore/pkmodels/onecompartmentbolus.h"
#include "tucucore/pkmodels/onecompartmentextra.h"
#include "tucucore/pkmodels/onecompartmentextralag.h"
#include "tucucore/pkmodels/onecompartmentinfusion.h"
#include "tucucore/pkmodels/rkonecompartmentextra.h"
#include "tucucore/pkmodels/rkonecompartmentgammaextra.h"
#include "tucucore/pkmodels/rktwocompartmenterlang.h"
#include "tucucore/pkmodels/threecompartmentbolus.h"
#include "tucucore/pkmodels/threecompartmentextra.h"
#include "tucucore/pkmodels/threecompartmentinfusion.h"
#include "tucucore/pkmodels/twocompartmentbolus.h"
#include "tucucore/pkmodels/twocompartmentextra.h"
#include "tucucore/pkmodels/twocompartmentinfusion.h"
#include "tucucore/residualerrormodel.h"

#include "fructose/fructose.h"
#include "pkmodels/constanteliminationbolus.h"
#include "pkmodels/multiconstanteliminationbolus.h"


//I'll let you add a test_multilikelihood.h file in the tests, and start with a first test. This test should create the objects we send to the constructor of MultiLikelihood, a set of Etas, and then call negativeLogLikelihood() on it. It is quite hard to know what results would be expected, but already having a computation with "something" would be good. I would suggest various scenarios: 1) Only a single analyte and one sample (Using ConstantEliminationBolus intakes, 2) A single analyte and 3 samples, 3) A 2-analyte with one sample on analyte 1 (using MultiConstantEliminationBolus), 4) A 2-analyte with one sample on analyte 2, 5) A 2-analyte with one sample per analytes at different times, 6) A 2-analyte with one sample per analytes at the same time, 7) A 2-analyte with three samples per analyte, with one of them at the same time, the others at different times.


using namespace Tucuxi::Core;

namespace Tucuxi {
namespace Core {
/*
template<typename T>
std::ostream& operator<<(typename std::enable_if<std::is_enum<T>::value, std::ostream>::type& stream, const T& e)
{
    return stream << static_cast<typename std::underlying_type<T>::type>(e);


ASK ABOUT THIS



}
*/


struct TestMultiLikeliHood : public fructose::test_base<TestMultiLikeliHood>
{

    static const int CYCLE_SIZE = 251;

    TestMultiLikeliHood() {}



    FormulationAndRoute getBolusFormulationAndRoute()
    {
        return FormulationAndRoute(
                Formulation::Test, AdministrationRoute::IntravenousBolus, AbsorptionModel::Intravascular);
    }



    void test1(const std::string& /* _testName */)
    {

        //first scenario: Only a single analyte and one sample (Using ConstantEliminationBolus intakes)

        if (verbose()) {
            std::cout << __FUNCTION__ << std::endl;
        }


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


        SigmaResidualErrorModel* newErrorModel = new SigmaResidualErrorModel();
        Tucuxi::Core::Sigma sigma(1);
        sigma(0) = 0.3138;
        newErrorModel->setErrorModel(Tucuxi::Core::ResidualErrorType::PROPORTIONAL);
        newErrorModel->setSigma(sigma);
        residualErrorModel.push_back(newErrorModel);
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




        Tucuxi::Core::MultiLikelihood aux(
                omega, residualErrorModel, samples, intakes, parameters, concentrationCalculator);

        // Set initial etas to 0 for CL and V

        etas.push_back(0.1);
        //etas.push_back(0.0);

        Value x = aux.negativeLogLikelihood(etas);

        // We compute the expected result

        double expectedSampleValue = 201.1;
        double omegaAdd = static_cast<double>(omega.rows()) * log(2 * PI) + log(omega.determinant());

        EigenVector etasmd(1);
        etasmd[0] = 0.1;

        double expectedValue = 0.5 * (etasmd.transpose() * omega.inverse() * etasmd + omegaAdd)
                               - residualErrorModel[0]->calculateSampleLikelihood(expectedSampleValue, s0.getValue());
        fructose_assert_double_eq(x, expectedValue);
    }




    void test2(const std::string& /* _testName */)
    {

        //Second scenario: A single analyte and 3 samples

        if (verbose()) {
            std::cout << __FUNCTION__ << std::endl;
        }


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


        SigmaResidualErrorModel* newErrorModel = new SigmaResidualErrorModel();
        Tucuxi::Core::Sigma sigma(1);
        sigma(0) = 0.3138;
        newErrorModel->setErrorModel(Tucuxi::Core::ResidualErrorType::PROPORTIONAL);
        newErrorModel->setSigma(sigma);
        residualErrorModel.push_back(newErrorModel);
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

        Tucuxi::Core::MultiLikelihood aux(
                omega, residualErrorModel, samples, intakes, parameters, concentrationCalculator);

        // Set initial etas to 0 for CL and V

        etas.push_back(0.1);
        //etas.push_back(0.0);

        Value x = aux.negativeLogLikelihood(etas);

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
        fructose_assert_double_eq(x, expectedValue);
    }



    void test3(const std::string& /* _testName */)
    // Third scenario: A 2-analyte with one sample on analyte 1 (using MultiConstantEliminationBolus)

    {

        if (verbose()) {
            std::cout << __FUNCTION__ << std::endl;
        }


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


        SigmaResidualErrorModel* newErrorModel0 = new SigmaResidualErrorModel();
        Tucuxi::Core::Sigma sigma0(1);
        sigma0(0) = 0.3138;
        newErrorModel0->setErrorModel(Tucuxi::Core::ResidualErrorType::PROPORTIONAL);
        newErrorModel0->setSigma(sigma0);
        residualErrorModel.push_back(newErrorModel0);
        //here i'm supposed to use a vector of pointers of IResidualErrorModel or a vector of pointers of SigmaResidualErrorModel


        SigmaResidualErrorModel* newErrorModel1 = new SigmaResidualErrorModel();
        Tucuxi::Core::Sigma sigma1(1);
        sigma1(0) = 0.3138;
        newErrorModel1->setErrorModel(Tucuxi::Core::ResidualErrorType::PROPORTIONAL);
        newErrorModel1->setSigma(sigma1);
        residualErrorModel.push_back(newErrorModel1);
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



        Tucuxi::Core::MultiLikelihood aux(
                omega, residualErrorModel, samples, intakes, parameters, concentrationCalculator);

        // Set initial etas to 0 for CL and V

        etas.push_back(0.1);
        etas.push_back(0.1);

        Value x = aux.negativeLogLikelihood(etas);

        // We compute the expected result

        double expectedSampleValue = 201.1;
        double omegaAdd = static_cast<double>(omega.rows()) * log(2 * PI) + log(omega.determinant());

        EigenVector etasmd(2);
        etasmd[0] = 0.1;
        etasmd[1] = 0.1;

        double expectedValuestep0 = 0.5 * (etasmd.transpose() * omega.inverse() * etasmd + omegaAdd);



        double expectedValuestep1 = expectedValuestep0 - residualErrorModel[0]->calculateSampleLikelihood(
                                       expectedSampleValue, s0.getValue()); //0 as the second sample is empty

        fructose_assert_double_ne(x, std::numeric_limits<double>::max());
        fructose_assert_double_eq(x, expectedValuestep1);
    }


    void test4(const std::string& /* _testName */)
    {

        //A 2-analyte with one sample on analyte 2

        if (verbose()) {
            std::cout << __FUNCTION__ << std::endl;
        }


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


        SigmaResidualErrorModel* newErrorModel0 = new SigmaResidualErrorModel();
        Tucuxi::Core::Sigma sigma0(1);
        sigma0(0) = 0.3138;
        newErrorModel0->setErrorModel(Tucuxi::Core::ResidualErrorType::PROPORTIONAL);
        newErrorModel0->setSigma(sigma0);
        residualErrorModel.push_back(newErrorModel0);
        //here i'm supposed to use a vector of pointers of IResidualErrorModel or a vector of pointers of SigmaResidualErrorModel



        SigmaResidualErrorModel* newErrorModel1 = new SigmaResidualErrorModel();
        Tucuxi::Core::Sigma sigma1(1);
        sigma1(0) = 0.3138;
        newErrorModel1->setErrorModel(Tucuxi::Core::ResidualErrorType::PROPORTIONAL);
        newErrorModel1->setSigma(sigma1);
        residualErrorModel.push_back(newErrorModel1);
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

        Tucuxi::Core::MultiLikelihood aux(
                omega, residualErrorModel, samples, intakes, parameters, concentrationCalculator);

        // Set initial etas to 0 for CL and V

        etas.push_back(0.1);
        etas.push_back(0.1);

        Value x = aux.negativeLogLikelihood(etas);

        // We compute the expected result

        double expectedSampleValue = 201.1;
        double omegaAdd = static_cast<double>(omega.rows()) * log(2 * PI) + log(omega.determinant());

        EigenVector etasmd(2);
        etasmd[0] = 0.1;
        etasmd[1] = 0.1;

        double expectedValue = 0.5 * (etasmd.transpose() * omega.inverse() * etasmd + omegaAdd)
                               - residualErrorModel[1]->calculateSampleLikelihood(
                                       expectedSampleValue, s1.getValue()); //0 as the first sample is empty
        fructose_assert_double_ne(x, std::numeric_limits<double>::max());
        fructose_assert_double_eq(x, expectedValue);
    }

    void test5(const std::string& /* _testName */)
    {

        //A 2-analyte with one sample per analytes at different times
        if (verbose()) {
            std::cout << __FUNCTION__ << std::endl;
        }


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


        SigmaResidualErrorModel* newErrorModel0 = new SigmaResidualErrorModel();
        Tucuxi::Core::Sigma sigma0(1);
        sigma0(0) = 0.3138;
        newErrorModel0->setErrorModel(Tucuxi::Core::ResidualErrorType::PROPORTIONAL);
        newErrorModel0->setSigma(sigma0);
        residualErrorModel.push_back(newErrorModel0);
        //here i'm supposed to use a vector of pointers of IResidualErrorModel or a vector of pointers of SigmaResidualErrorModel



        SigmaResidualErrorModel* newErrorModel1 = new SigmaResidualErrorModel();
        Tucuxi::Core::Sigma sigma1(1);
        sigma1(0) = 0.3138;
        newErrorModel1->setErrorModel(Tucuxi::Core::ResidualErrorType::PROPORTIONAL);
        newErrorModel1->setSigma(sigma1);
        residualErrorModel.push_back(newErrorModel1);
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




        Tucuxi::Core::MultiLikelihood aux(
                omega, residualErrorModel, samples, intakes, parameters, concentrationCalculator);

        // Set initial etas to 0 for CL and V

        etas.push_back(0.1);
        etas.push_back(0.1);

        Value x = aux.negativeLogLikelihood(etas);

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

        double expectedValue =
                0.5 * (etasmd.transpose() * omega.inverse() * etasmd + omegaAdd)
                - residualErrorModel[0]->calculateSampleLikelihood(expectedSampleValue0, s0.getValue())
                - residualErrorModel[1]->calculateSampleLikelihood(
                        expectedSampleValue1,
                        s1.getValue());
        fructose_assert_double_ne(x, std::numeric_limits<double>::max());
        fructose_assert_double_eq(x, expectedValue);
    }

    void test6(const std::string& /* _testName */)
    {

        //A 2-analyte with one sample per analytes at the same time
        if (verbose()) {
            std::cout << __FUNCTION__ << std::endl;
        }


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


        SigmaResidualErrorModel* newErrorModel0 = new SigmaResidualErrorModel();
        Tucuxi::Core::Sigma sigma0(1);
        sigma0(0) = 0.3138;
        newErrorModel0->setErrorModel(Tucuxi::Core::ResidualErrorType::PROPORTIONAL);
        newErrorModel0->setSigma(sigma0);
        residualErrorModel.push_back(newErrorModel0);
        //here i'm supposed to use a vector of pointers of IResidualErrorModel or a vector of pointers of SigmaResidualErrorModel



        SigmaResidualErrorModel* newErrorModel1 = new SigmaResidualErrorModel();
        Tucuxi::Core::Sigma sigma1(1);
        sigma1(0) = 0.3138;
        newErrorModel1->setErrorModel(Tucuxi::Core::ResidualErrorType::PROPORTIONAL);
        newErrorModel1->setSigma(sigma1);
        residualErrorModel.push_back(newErrorModel1);
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
                Duration(std::chrono::hours(12 ), std::chrono::minutes(30), std::chrono::seconds(0)));
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


        Tucuxi::Core::MultiLikelihood aux(
                omega, residualErrorModel, samples, intakes, parameters, concentrationCalculator);

        // Set initial etas to 0 for CL and V

        etas.push_back(0.1);
        etas.push_back(0.1);

        Value x = aux.negativeLogLikelihood(etas);

        // We compute the expected result

        double expectedSampleValue = 201.1;
        double omegaAdd = static_cast<double>(omega.rows()) * log(2 * PI) + log(omega.determinant());

        EigenVector etasmd(2);
        etasmd[0] = 0.1;
        etasmd[1] = 0.1;

        double expectedValue =
                0.5 * (etasmd.transpose() * omega.inverse() * etasmd + omegaAdd)
                - residualErrorModel[0]->calculateSampleLikelihood(expectedSampleValue, s0.getValue())
                - residualErrorModel[1]->calculateSampleLikelihood(
                        expectedSampleValue,
                        s1.getValue()); //WE CAN NOT USE 2.0 * m_residual... as they are 2 different analytes
        fructose_assert_double_ne(x, std::numeric_limits<double>::max());
        fructose_assert_double_eq(x, expectedValue);
    }


    void test7(const std::string& /* _testName */)
    {


        //A 2-analyte with three samples per analyte, with one of them at the same time, the others at different times.


        if (verbose()) {
            std::cout << __FUNCTION__ << std::endl;
        }


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


        SigmaResidualErrorModel* newErrorModel0 = new SigmaResidualErrorModel();
        Tucuxi::Core::Sigma sigma0(1);
        sigma0(0) = 0.3138;
        newErrorModel0->setErrorModel(Tucuxi::Core::ResidualErrorType::PROPORTIONAL);
        newErrorModel0->setSigma(sigma0);
        residualErrorModel.push_back(newErrorModel0);
        //here i'm supposed to use a vector of pointers of IResidualErrorModel or a vector of pointers of SigmaResidualErrorModel



        SigmaResidualErrorModel* newErrorModel1 = new SigmaResidualErrorModel();
        Tucuxi::Core::Sigma sigma1(1);
        sigma1(0) = 0.3138;
        newErrorModel1->setErrorModel(Tucuxi::Core::ResidualErrorType::PROPORTIONAL);
        newErrorModel1->setSigma(sigma1);
        residualErrorModel.push_back(newErrorModel1);
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
                Duration(std::chrono::hours(15), std::chrono::minutes(57), std::chrono::seconds(44)));
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
                "TestM1", 1.0, Tucuxi::Core::ParameterVariabilityType::None));
        parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "TestR1", 0.0, Tucuxi::Core::ParameterVariabilityType::None));
        parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "TestS1", 0.1, Tucuxi::Core::ParameterVariabilityType::None));
        Tucuxi::Core::ParameterSetEvent parameterset(DateTime::now(), parameterDefs);
        parameters.addParameterSetEvent(parameterset);



        Tucuxi::Core::MultiLikelihood aux(
                omega, residualErrorModel, samples, intakes, parameters, concentrationCalculator);

        // Set initial etas to 0 for CL and V

        etas.push_back(0.1);
        etas.push_back(0.1);

        Value x = aux.negativeLogLikelihood(etas);

        // We compute the expected result


        double expectedSampleValue0 = 121.1;
        double expectedSampleValue1 = 41.1;
        double expectedSampleValue2 = 21.1;
        double expectedSampleValue3 = 1211;
        double expectedSampleValue4 = 411;
        double expectedSampleValue5 = 211;
        double omegaAdd = static_cast<double>(omega.rows()) * log(2 * PI) + log(omega.determinant());

        EigenVector etasmd(2);
        etasmd[0] = 0.1;
        etasmd[1] = 0.1;

        double expectedValue0 =
                0.5 * (etasmd.transpose() * omega.inverse() * etasmd + omegaAdd)
                - residualErrorModel[0]->calculateSampleLikelihood(expectedSampleValue0, s0.getValue())
                - residualErrorModel[0]->calculateSampleLikelihood(
                        expectedSampleValue1,
                        s1.getValue())
                 - residualErrorModel[0]->calculateSampleLikelihood(expectedSampleValue2, s2.getValue());

        double expectedValue1 = expectedValue0
                                - residualErrorModel[1]->calculateSampleLikelihood(expectedSampleValue3, s3.getValue());

        double expectedValue2 = expectedValue1
                                - residualErrorModel[1]->calculateSampleLikelihood(expectedSampleValue4, s4.getValue());
        double expectedValue3 = expectedValue2
                                - residualErrorModel[1]->calculateSampleLikelihood(
                                        expectedSampleValue5, s5.getValue());

        fructose_assert_double_ne(x, std::numeric_limits<double>::max());
        fructose_assert_double_eq(x, expectedValue3);
    }
};

} // namespace Core



} // namespace Tucuxi
