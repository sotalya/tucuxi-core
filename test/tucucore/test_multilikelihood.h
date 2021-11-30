#ifndef TEST_MULTILIKELIHOOD_H
#define TEST_MULTILIKELIHOOD_H

#endif // TEST_MULTILIKELIHOOD_H

#include <iostream>
#include <memory>

#include "fructose/fructose.h"

#include "tucucommon/general.h"

#include "tucucommon/duration.h"
#include "tucucore/dosage.h"
#include "tucucore/intakeextractor.h"
#include "tucucore/intakeintervalcalculator.h"
#include "tucucore/concentrationcalculator.h"
#include "tucucore/multiconcentrationcalculator.h"
#include "tucucore/pkmodels/onecompartmentbolus.h"
#include "tucucore/pkmodels/onecompartmentinfusion.h"
#include "tucucore/pkmodels/onecompartmentextra.h"
#include "tucucore/pkmodels/onecompartmentextralag.h"
#include "tucucore/pkmodels/twocompartmentbolus.h"
#include "tucucore/pkmodels/twocompartmentinfusion.h"
#include "tucucore/pkmodels/twocompartmentextra.h"
#include "tucucore/pkmodels/threecompartmentbolus.h"
#include "tucucore/pkmodels/threecompartmentinfusion.h"
#include "tucucore/pkmodels/threecompartmentextra.h"

#include "pkmodels/constanteliminationbolus.h"
#include "pkmodels/multiconstanteliminationbolus.h"

#include "tucucore/drugmodel/formulationandroute.h"
#include "tucucore/likelihood.h"
#include "tucucore/multilikelihood.h"

#include "tucucore/pkmodels/rkonecompartmentextra.h"
#include "tucucore/pkmodels/rkonecompartmentgammaextra.h"
#include "tucucore/pkmodels/rktwocompartmenterlang.h"
#include "tucucore/residualerrormodel.h"
#include "tucucore/intakeevent.h"


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


struct TestMultiLikeliHood : public fructose::test_base<TestMultiLikeliHood>{

    static const int CYCLE_SIZE = 251;

    TestMultiLikeliHood(){   }



    FormulationAndRoute getBolusFormulationAndRoute()
    {
        return FormulationAndRoute(
                    Formulation::Test,
                    AdministrationRoute::IntravenousBolus,
                    AbsorptionModel::Intravascular);
    }

    void test1(const std::string& /* _testName */){ 
        
        //first scenario: Only a single analyte and one sample (Using ConstantEliminationBolus intakes)

        if (verbose()) {
              std::cout << __FUNCTION__ << std::endl;
          }


        Tucuxi::Core::OmegaMatrix omega;
        std::vector<IResidualErrorModel*> m_residualErrorModel;
        std::vector<SampleSeries> _samples;
        IntakeSeries _intakes;
        ParameterSetSeries _parameters;
        MultiConcentrationCalculator _concentrationCalculator;
        Etas _etas;

        //definition of the omega matrix
        omega = Tucuxi::Core::OmegaMatrix(1,1);
        omega(0,0) = 0.1; // Variance of A



        //definition of the m_residualErrorModel


        SigmaResidualErrorModel *newErrorModel = new SigmaResidualErrorModel();
        Tucuxi::Core::Sigma sigma(1);
        sigma(0) = 0.3138;
        newErrorModel->setErrorModel(Tucuxi::Core::ResidualErrorType::PROPORTIONAL);
        newErrorModel->setSigma(sigma);
        m_residualErrorModel.push_back(newErrorModel);
        //here i'm supposed to use a vector of pointers of IResidualErrorModel or a vector of pointers of SigmaResidualErrorModel

        //definition of the samples
        Tucuxi::Core::SampleSeries sampleSeries;
        DateTime date0 = DateTime((date::year(2017), date::month(6), date::day(6), Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))));

        Tucuxi::Core::SampleEvent s0(date0, 200.0);
        sampleSeries.push_back(s0);

        _samples.push_back(sampleSeries);



        //definition of the intakes


        _intakes.emplace_back(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(6)),
                                      Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
                             Duration(),
                             DoseValue(200.0),
                             TucuUnit("mg"),
                             Duration(std::chrono::hours(24)),
                             getBolusFormulationAndRoute(),
                             getBolusFormulationAndRoute().getAbsorptionModel(),
                             Duration(std::chrono::minutes(20)),
                             static_cast<int>(CYCLE_SIZE));

        _intakes.emplace_back(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(7)),
                                      Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
                             Duration(),
                             DoseValue(200.0),
                             TucuUnit("mg"),
                             Duration(std::chrono::hours(24)),
                             getBolusFormulationAndRoute(),
                             getBolusFormulationAndRoute().getAbsorptionModel(),
                             Duration(std::chrono::minutes(20)),

                             static_cast<int>(CYCLE_SIZE));
        std::shared_ptr<Tucuxi::Core::IntakeIntervalCalculator> calculator = std::make_shared<Tucuxi::Core::ConstantEliminationBolus>();
        _intakes[0].setCalculator(calculator);
        _intakes[1].setCalculator(calculator);


        //Definition of the parameters


        Tucuxi::Core::ParameterDefinitions parameterDefs;
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestA", 1.0, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestR", 0.0, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestS", 0.0, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestM", 1.0, Tucuxi::Core::ParameterVariabilityType::None)));
        Tucuxi::Core::ParameterSetEvent parameters(DateTime::now(), parameterDefs);
        Tucuxi::Core::ParameterSetSeries parametersSeries;
        parametersSeries.addParameterSetEvent(parameters);


        //Definition of the Concentration Calculator

        //Tucuxi::Core::MultiConcentrationPredictionPtr predictionPtr;
        /*{
            predictionPtr = std::make_unique<Tucuxi::Core::MultiConcentrationPrediction>();

            DateTime recordFrom = date::year_month_day(date::year(2018), date::month(9), date::day(1));
            DateTime recordTo = recordFrom + interval;

            Tucuxi::Core::IntakeSeries intakeSeries;
            std::shared_ptr<IntakeIntervalCalculator> calculator2 = std::make_shared<ConstantEliminationBolus>();
            intakeEvent.setCalculator(calculator2);
            intakeSeries.push_back(intakeEvent);
            Tucuxi::Core::IMultiConcentrationCalculator *concentrationCalculator = new Tucuxi::Core::MultiConcentrationCalculator();
            auto status = concentrationCalculator->computeConcentrations(
                predictionPtr,
                false,
                recordFrom,
                recordTo,
                intakeSeries,
                _parameters);
            delete concentrationCalculator;
            fructose_assert_eq(status, ComputingStatus::Ok);

    #if 0
            for (int i = 0; i<nbPoints; i++) {
                std::cout << i << ":" << predictionPtr->getValues()[0][i] << std::endl;
            }
    #endif
        }
        */



            Tucuxi::Core::MultiLikelihood aux(omega, m_residualErrorModel, _samples, _intakes, _parameters, _concentrationCalculator);

            // Set initial etas to 0 for CL and V

            _etas.push_back(0.1);
            //_etas.push_back(0.0);

            Value x = aux.negativeLogLikelihood(_etas);

            // We compute the expected result

            // Prior: 0.5 * (_etas.transpose() * m_inverseOmega * _etas + m_omegaAdd)
            // m_omega = m_omegaAdd(static_cast<double>(_omega.rows()) * log(2 * PI) + log(_omega.determinant()))
            // sample likelihood: - _residualErrorModel.calculateSampleLikelihood(_expected, _observed.getValue());
            double expectedSampleValue = 201.1;
            double omegaAdd = static_cast<double>(omega.rows()) * log(2 * PI) + log(omega.determinant());

            EigenVector etasmd(1);
            etasmd[0] = 0.1;

            double expectedValue = 0.5 * (etasmd.transpose() * omega.inverse() * etasmd + omegaAdd) -
                    m_residualErrorModel[0]->calculateSampleLikelihood(expectedSampleValue, s0.getValue());
            fructose_assert_double_eq(x, expectedValue);

    }






    void test2(const std::string& /* _testName */){
        
        //Second scenario: A single analyte and 3 samples

        if (verbose()) {
              std::cout << __FUNCTION__ << std::endl;
          }


        Tucuxi::Core::OmegaMatrix omega;
        std::vector<IResidualErrorModel*> m_residualErrorModel;
        std::vector<SampleSeries> _samples;
        IntakeSeries _intakes;
        ParameterSetSeries _parameters;
        MultiConcentrationCalculator _concentrationCalculator;
        Etas _etas;

        //definition of the omega matrix
        omega = Tucuxi::Core::OmegaMatrix(1,1);
        omega(0,0) = 0.1; // Variance of A



        //definition of the m_residualErrorModel


        SigmaResidualErrorModel *newErrorModel = new SigmaResidualErrorModel();
        Tucuxi::Core::Sigma sigma(1);
        sigma(0) = 0.3138;
        newErrorModel->setErrorModel(Tucuxi::Core::ResidualErrorType::PROPORTIONAL);
        newErrorModel->setSigma(sigma);
        m_residualErrorModel.push_back(newErrorModel);
        //here i'm supposed to use a vector of pointers of IResidualErrorModel or a vector of pointers of SigmaResidualErrorModel

        //definition of the samples
        Tucuxi::Core::SampleSeries sampleSeries;

        DateTime date0 = DateTime((date::year(2017), date::month(6), date::day(6), Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))));
        Tucuxi::Core::SampleEvent s0(date0, 200.0);
        sampleSeries.push_back(s0);

        DateTime date1 = DateTime((date::year(2017), date::month(6), date::day(6), Duration(std::chrono::hours(16), std::chrono::minutes(30), std::chrono::seconds(0))));
        Tucuxi::Core::SampleEvent s1(date1, 200.0);
        sampleSeries.push_back(s1);


        DateTime date2 = DateTime((date::year(2017), date::month(6), date::day(6), Duration(std::chrono::hours(20), std::chrono::minutes(30), std::chrono::seconds(0))));
        Tucuxi::Core::SampleEvent s2(date2, 200.0);
        sampleSeries.push_back(s2);

        _samples.push_back(sampleSeries);



        //definition of the intakes


        _intakes.emplace_back(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(6)),
                                      Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
                             Duration(),
                             DoseValue(200.0),
                             TucuUnit("mg"),
                             Duration(std::chrono::hours(24)),
                             getBolusFormulationAndRoute(),
                             getBolusFormulationAndRoute().getAbsorptionModel(),
                             Duration(std::chrono::minutes(20)),
                             static_cast<int>(CYCLE_SIZE));

        _intakes.emplace_back(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(7)),
                                      Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
                             Duration(),
                             DoseValue(200.0),
                             TucuUnit("mg"),
                             Duration(std::chrono::hours(24)),
                             getBolusFormulationAndRoute(),
                             getBolusFormulationAndRoute().getAbsorptionModel(),
                             Duration(std::chrono::minutes(20)),

                             static_cast<int>(CYCLE_SIZE));
        std::shared_ptr<Tucuxi::Core::IntakeIntervalCalculator> calculator = std::make_shared<Tucuxi::Core::ConstantEliminationBolus>();
        _intakes[0].setCalculator(calculator);
        _intakes[1].setCalculator(calculator);


        //Definition of the parameters


        Tucuxi::Core::ParameterDefinitions parameterDefs;
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestA", 1.0, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestR", 0.0, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestS", 0.1, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestM", 1.0, Tucuxi::Core::ParameterVariabilityType::None)));
        Tucuxi::Core::ParameterSetEvent parameters(DateTime::now(), parameterDefs);
        Tucuxi::Core::ParameterSetSeries parametersSeries;
        parametersSeries.addParameterSetEvent(parameters);


        //Definition of the Concentration Calculator

        //Tucuxi::Core::MultiConcentrationPredictionPtr predictionPtr;
        /*{
            predictionPtr = std::make_unique<Tucuxi::Core::MultiConcentrationPrediction>();

            DateTime recordFrom = date::year_month_day(date::year(2018), date::month(9), date::day(1));
            DateTime recordTo = recordFrom + interval;

            Tucuxi::Core::IntakeSeries intakeSeries;
            std::shared_ptr<IntakeIntervalCalculator> calculator2 = std::make_shared<ConstantEliminationBolus>();
            intakeEvent.setCalculator(calculator2);
            intakeSeries.push_back(intakeEvent);
            Tucuxi::Core::IMultiConcentrationCalculator *concentrationCalculator = new Tucuxi::Core::MultiConcentrationCalculator();
            auto status = concentrationCalculator->computeConcentrations(
                predictionPtr,
                false,
                recordFrom,
                recordTo,
                intakeSeries,
                _parameters);
            delete concentrationCalculator;
            fructose_assert_eq(status, ComputingStatus::Ok);

    #if 0
            for (int i = 0; i<nbPoints; i++) {
                std::cout << i << ":" << predictionPtr->getValues()[0][i] << std::endl;
            }
    #endif
        }
        */



            Tucuxi::Core::MultiLikelihood aux(omega, m_residualErrorModel, _samples, _intakes, _parameters, _concentrationCalculator);

            // Set initial etas to 0 for CL and V

            _etas.push_back(0.1);
            //_etas.push_back(0.0);

            Value x = aux.negativeLogLikelihood(_etas);

            // We compute the expected result

            // Prior: 0.5 * (_etas.transpose() * m_inverseOmega * _etas + m_omegaAdd)
            // m_omega = m_omegaAdd(static_cast<double>(_omega.rows()) * log(2 * PI) + log(_omega.determinant()))
            // sample likelihood: - _residualErrorModel.calculateSampleLikelihood(_expected, _observed.getValue());
            double expectedSampleValue1 = 201.1;
            double expectedSampleValue2 = 201.1;
            double expectedSampleValue3 = 201.1;
            double omegaAdd = static_cast<double>(omega.rows()) * log(2 * PI) + log(omega.determinant());

            EigenVector etasmd(1);
            etasmd[0] = 0.1;

            double expectedValue = 0.5 * (etasmd.transpose() * omega.inverse() * etasmd + omegaAdd) -
                    m_residualErrorModel[0]->calculateSampleLikelihood(expectedSampleValue1, s0.getValue() - m_residualErrorModel[0]->calculateSampleLikelihood(expectedSampleValue2, s1.getValue()) - m_residualErrorModel[0]->calculateSampleLikelihood(expectedSampleValue3, s2.getValue()));
            fructose_assert_double_eq(x, expectedValue);
    }
    
    
    void test3(const std::string& /* _testName */)  //HAVE TO FINISH THIS
        // Third scenario: A 2-analyte with one sample on analyte 1 (using MultiConstantEliminationBolus)

    {

        if (verbose()) {
              std::cout << __FUNCTION__ << std::endl;
          }


        Tucuxi::Core::OmegaMatrix omega;
        std::vector<IResidualErrorModel*> m_residualErrorModel;
        std::vector<SampleSeries> _samples;
        IntakeSeries _intakes;
        ParameterSetSeries _parameters;
        MultiConcentrationCalculator _concentrationCalculator;
        Etas _etas;

        //definition of the omega matrix
        omega = Tucuxi::Core::OmegaMatrix(1,1);
        omega(0,0) = 0.1; // Variance of A



        //definition of the m_residualErrorModel


        SigmaResidualErrorModel *newErrorModel1 = new SigmaResidualErrorModel();
        Tucuxi::Core::Sigma sigma1(1);
        sigma1(0) = 0.3138;
        newErrorModel1->setErrorModel(Tucuxi::Core::ResidualErrorType::PROPORTIONAL);
        newErrorModel1->setSigma(sigma1);
        m_residualErrorModel.push_back(newErrorModel1);
        //here i'm supposed to use a vector of pointers of IResidualErrorModel or a vector of pointers of SigmaResidualErrorModel



        SigmaResidualErrorModel *newErrorModel2 = new SigmaResidualErrorModel();
        Tucuxi::Core::Sigma sigma2(1);
        sigma2(0) = 0.3138;
        newErrorModel1->setErrorModel(Tucuxi::Core::ResidualErrorType::PROPORTIONAL);
        newErrorModel1->setSigma(sigma2);
        m_residualErrorModel.push_back(newErrorModel2);
        //here i'm supposed to use a vector of pointers of IResidualErrorModel or a vector of pointers of SigmaResidualErrorModel

        //definition of the samples
        Tucuxi::Core::SampleSeries sampleSeries1;
        DateTime date0 = DateTime((date::year(2017), date::month(6), date::day(6), Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))));
        Tucuxi::Core::SampleEvent s0(date0, 200.0);
        sampleSeries1.push_back(s0);
        _samples.push_back(sampleSeries1);

        Tucuxi::Core::SampleSeries sampleSeries2;
         _samples.push_back(sampleSeries2);


        //definition of the intakes


        _intakes.emplace_back(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(6)),
                                      Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
                             Duration(),
                             DoseValue(200.0),
                             TucuUnit("mg"),
                             Duration(std::chrono::hours(24)),
                             getBolusFormulationAndRoute(),
                             getBolusFormulationAndRoute().getAbsorptionModel(),
                             Duration(std::chrono::minutes(20)),
                             static_cast<int>(CYCLE_SIZE));

        _intakes.emplace_back(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(7)),
                                      Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
                             Duration(),
                             DoseValue(200.0),
                             TucuUnit("mg"),
                             Duration(std::chrono::hours(24)),
                             getBolusFormulationAndRoute(),
                             getBolusFormulationAndRoute().getAbsorptionModel(),
                             Duration(std::chrono::minutes(20)),

                             static_cast<int>(CYCLE_SIZE));
        std::shared_ptr<Tucuxi::Core::IntakeIntervalCalculator> calculator = std::make_shared<Tucuxi::Core::ConstantEliminationBolus>();
        _intakes[0].setCalculator(calculator);
        _intakes[1].setCalculator(calculator);


        //Definition of the parameters


        Tucuxi::Core::ParameterDefinitions parameterDefs;
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestA0", 1.0, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestR0", 0.0, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestS0", 0.1, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestM0", 0.0, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestA1", 2.0, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestR1", 0.0, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestS1", 0.0, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestM1", 0.0, Tucuxi::Core::ParameterVariabilityType::None)));
        Tucuxi::Core::ParameterSetEvent parameters(DateTime::now(), parameterDefs);
        Tucuxi::Core::ParameterSetSeries parametersSeries;
        parametersSeries.addParameterSetEvent(parameters);


        //Definition of the Concentration Calculator

        //Tucuxi::Core::MultiConcentrationPredictionPtr predictionPtr;
        /*{
            predictionPtr = std::make_unique<Tucuxi::Core::MultiConcentrationPrediction>();

            DateTime recordFrom = date::year_month_day(date::year(2018), date::month(9), date::day(1));
            DateTime recordTo = recordFrom + interval;

            Tucuxi::Core::IntakeSeries intakeSeries;
            std::shared_ptr<IntakeIntervalCalculator> calculator2 = std::make_shared<ConstantEliminationBolus>();
            intakeEvent.setCalculator(calculator2);
            intakeSeries.push_back(intakeEvent);
            Tucuxi::Core::IMultiConcentrationCalculator *concentrationCalculator = new Tucuxi::Core::MultiConcentrationCalculator();
            auto status = concentrationCalculator->computeConcentrations(
                predictionPtr,
                false,
                recordFrom,
                recordTo,
                intakeSeries,
                _parameters);
            delete concentrationCalculator;
            fructose_assert_eq(status, ComputingStatus::Ok);

    #if 0
            for (int i = 0; i<nbPoints; i++) {
                std::cout << i << ":" << predictionPtr->getValues()[0][i] << std::endl;
            }
    #endif
        }
        */



            Tucuxi::Core::MultiLikelihood aux(omega, m_residualErrorModel, _samples, _intakes, _parameters, _concentrationCalculator);

            // Set initial etas to 0 for CL and V

            _etas.push_back(0.1);
            //_etas.push_back(0.0);

            Value x = aux.negativeLogLikelihood(_etas);

            // We compute the expected result

            // Prior: 0.5 * (_etas.transpose() * m_inverseOmega * _etas + m_omegaAdd)
            // m_omega = m_omegaAdd(static_cast<double>(_omega.rows()) * log(2 * PI) + log(_omega.determinant()))
            // sample likelihood: - _residualErrorModel.calculateSampleLikelihood(_expected, _observed.getValue());
            double expectedSampleValue = 201.1;
            double omegaAdd = static_cast<double>(omega.rows()) * log(2 * PI) + log(omega.determinant());

            EigenVector etasmd(1);
            etasmd[0] = 0.1;

            double expectedValue = 0.5 * (etasmd.transpose() * omega.inverse() * etasmd + omegaAdd) -
                    m_residualErrorModel[0]->calculateSampleLikelihood(expectedSampleValue, s0.getValue()) - m_residualErrorModel[1]->calculateSampleLikelihood(expectedSampleValue, 0); //0 as the second sample is empty
            fructose_assert_double_eq(x, expectedValue);
        }


    void test4(const std::string& /* _testName */){
        //A 2-analyte with one sample on analyte 2


        if (verbose()) {
              std::cout << __FUNCTION__ << std::endl;
          }


        Tucuxi::Core::OmegaMatrix omega;
        std::vector<IResidualErrorModel*> m_residualErrorModel;
        std::vector<SampleSeries> _samples;
        IntakeSeries _intakes;
        ParameterSetSeries _parameters;
        MultiConcentrationCalculator _concentrationCalculator;
        Etas _etas;

        //definition of the omega matrix
        omega = Tucuxi::Core::OmegaMatrix(1,1);
        omega(0,0) = 0.1; // Variance of A



        //definition of the m_residualErrorModel


        SigmaResidualErrorModel *newErrorModel1 = new SigmaResidualErrorModel();
        Tucuxi::Core::Sigma sigma1(1);
        sigma1(0) = 0.3138;
        newErrorModel1->setErrorModel(Tucuxi::Core::ResidualErrorType::PROPORTIONAL);
        newErrorModel1->setSigma(sigma1);
        m_residualErrorModel.push_back(newErrorModel1);
        //here i'm supposed to use a vector of pointers of IResidualErrorModel or a vector of pointers of SigmaResidualErrorModel



        SigmaResidualErrorModel *newErrorModel2 = new SigmaResidualErrorModel();
        Tucuxi::Core::Sigma sigma2(1);
        sigma2(0) = 0.3138;
        newErrorModel1->setErrorModel(Tucuxi::Core::ResidualErrorType::PROPORTIONAL);
        newErrorModel1->setSigma(sigma2);
        m_residualErrorModel.push_back(newErrorModel2);
        //here i'm supposed to use a vector of pointers of IResidualErrorModel or a vector of pointers of SigmaResidualErrorModel

        //definition of the samples
        Tucuxi::Core::SampleSeries sampleSeries1;
        _samples.push_back(sampleSeries1);

        Tucuxi::Core::SampleSeries sampleSeries2;
        DateTime date0 = DateTime((date::year(2017), date::month(6), date::day(6), Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))));
        Tucuxi::Core::SampleEvent s0(date0, 200.0);
        sampleSeries2.push_back(s0);
         _samples.push_back(sampleSeries2);


        //definition of the intakes


        _intakes.emplace_back(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(6)),
                                      Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
                             Duration(),
                             DoseValue(200.0),
                             TucuUnit("mg"),
                             Duration(std::chrono::hours(24)),
                             getBolusFormulationAndRoute(),
                             getBolusFormulationAndRoute().getAbsorptionModel(),
                             Duration(std::chrono::minutes(20)),
                             static_cast<int>(CYCLE_SIZE));

        _intakes.emplace_back(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(7)),
                                      Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
                             Duration(),
                             DoseValue(200.0),
                             TucuUnit("mg"),
                             Duration(std::chrono::hours(24)),
                             getBolusFormulationAndRoute(),
                             getBolusFormulationAndRoute().getAbsorptionModel(),
                             Duration(std::chrono::minutes(20)),

                             static_cast<int>(CYCLE_SIZE));
        std::shared_ptr<Tucuxi::Core::IntakeIntervalCalculator> calculator = std::make_shared<Tucuxi::Core::ConstantEliminationBolus>();
        _intakes[0].setCalculator(calculator);
        _intakes[1].setCalculator(calculator);


        //Definition of the parameters


        Tucuxi::Core::ParameterDefinitions parameterDefs;
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestA0", 1.0, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestR0", 0.0, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestS0", 0.1, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestM0", 0.0, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestA1", 2.0, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestR1", 0.0, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestS1", 0.0, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestM1", 0.0, Tucuxi::Core::ParameterVariabilityType::None)));
        Tucuxi::Core::ParameterSetEvent parameters(DateTime::now(), parameterDefs);
        Tucuxi::Core::ParameterSetSeries parametersSeries;
        parametersSeries.addParameterSetEvent(parameters);


        //Definition of the Concentration Calculator

        //Tucuxi::Core::MultiConcentrationPredictionPtr predictionPtr;
        /*{
            predictionPtr = std::make_unique<Tucuxi::Core::MultiConcentrationPrediction>();

            DateTime recordFrom = date::year_month_day(date::year(2018), date::month(9), date::day(1));
            DateTime recordTo = recordFrom + interval;

            Tucuxi::Core::IntakeSeries intakeSeries;
            std::shared_ptr<IntakeIntervalCalculator> calculator2 = std::make_shared<ConstantEliminationBolus>();
            intakeEvent.setCalculator(calculator2);
            intakeSeries.push_back(intakeEvent);
            Tucuxi::Core::IMultiConcentrationCalculator *concentrationCalculator = new Tucuxi::Core::MultiConcentrationCalculator();
            auto status = concentrationCalculator->computeConcentrations(
                predictionPtr,
                false,
                recordFrom,
                recordTo,
                intakeSeries,
                _parameters);
            delete concentrationCalculator;
            fructose_assert_eq(status, ComputingStatus::Ok);

    #if 0
            for (int i = 0; i<nbPoints; i++) {
                std::cout << i << ":" << predictionPtr->getValues()[0][i] << std::endl;
            }
    #endif
        }
        */



            Tucuxi::Core::MultiLikelihood aux(omega, m_residualErrorModel, _samples, _intakes, _parameters, _concentrationCalculator);

            // Set initial etas to 0 for CL and V

            _etas.push_back(0.1);
            //_etas.push_back(0.0);

            Value x = aux.negativeLogLikelihood(_etas);

            // We compute the expected result

            // Prior: 0.5 * (_etas.transpose() * m_inverseOmega * _etas + m_omegaAdd)
            // m_omega = m_omegaAdd(static_cast<double>(_omega.rows()) * log(2 * PI) + log(_omega.determinant()))
            // sample likelihood: - _residualErrorModel.calculateSampleLikelihood(_expected, _observed.getValue());
            double expectedSampleValue = 201.1;
            double omegaAdd = static_cast<double>(omega.rows()) * log(2 * PI) + log(omega.determinant());

            EigenVector etasmd(1);
            etasmd[0] = 0.1;

            double expectedValue = 0.5 * (etasmd.transpose() * omega.inverse() * etasmd + omegaAdd) -
                    m_residualErrorModel[0]->calculateSampleLikelihood(expectedSampleValue, 0) - m_residualErrorModel[1]->calculateSampleLikelihood(expectedSampleValue, s0.getValue()); //0 as the first sample is empty
            fructose_assert_double_eq(x, expectedValue);


    }

    void test5(const std::string& /* _testName */ ){

        //A 2-analyte with one sample per analytes at different times
        if (verbose()) {
              std::cout << __FUNCTION__ << std::endl;
          }


        Tucuxi::Core::OmegaMatrix omega;
        std::vector<IResidualErrorModel*> m_residualErrorModel;
        std::vector<SampleSeries> _samples;
        IntakeSeries _intakes;
        ParameterSetSeries _parameters;
        MultiConcentrationCalculator _concentrationCalculator;
        Etas _etas;

        //definition of the omega matrix
        omega = Tucuxi::Core::OmegaMatrix(1,1);
        omega(0,0) = 0.1; // Variance of A



        //definition of the m_residualErrorModel


        SigmaResidualErrorModel *newErrorModel1 = new SigmaResidualErrorModel();
        Tucuxi::Core::Sigma sigma1(1);
        sigma1(0) = 0.3138;
        newErrorModel1->setErrorModel(Tucuxi::Core::ResidualErrorType::PROPORTIONAL);
        newErrorModel1->setSigma(sigma1);
        m_residualErrorModel.push_back(newErrorModel1);
        //here i'm supposed to use a vector of pointers of IResidualErrorModel or a vector of pointers of SigmaResidualErrorModel



        SigmaResidualErrorModel *newErrorModel2 = new SigmaResidualErrorModel();
        Tucuxi::Core::Sigma sigma2(1);
        sigma2(0) = 0.3138;
        newErrorModel1->setErrorModel(Tucuxi::Core::ResidualErrorType::PROPORTIONAL);
        newErrorModel1->setSigma(sigma2);
        m_residualErrorModel.push_back(newErrorModel2);
        //here i'm supposed to use a vector of pointers of IResidualErrorModel or a vector of pointers of SigmaResidualErrorModel

        //definition of the samples
        Tucuxi::Core::SampleSeries sampleSeries1;
        DateTime date0 = DateTime((date::year(2017), date::month(6), date::day(6), Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))));
        Tucuxi::Core::SampleEvent s0(date0, 200.0);
        sampleSeries1.push_back(s0);
        _samples.push_back(sampleSeries1);

        Tucuxi::Core::SampleSeries sampleSeries2;
        DateTime date1 = DateTime((date::year(2017), date::month(6), date::day(6), Duration(std::chrono::hours(16), std::chrono::minutes(30), std::chrono::seconds(0))));
        Tucuxi::Core::SampleEvent s1(date1, 200.0);
        sampleSeries2.push_back(s0);
         _samples.push_back(sampleSeries2);


        //definition of the intakes


        _intakes.emplace_back(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(6)),
                                      Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
                             Duration(),
                             DoseValue(200.0),
                             TucuUnit("mg"),
                             Duration(std::chrono::hours(24)),
                             getBolusFormulationAndRoute(),
                             getBolusFormulationAndRoute().getAbsorptionModel(),
                             Duration(std::chrono::minutes(20)),
                             static_cast<int>(CYCLE_SIZE));

        _intakes.emplace_back(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(7)),
                                      Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
                             Duration(),
                             DoseValue(200.0),
                             TucuUnit("mg"),
                             Duration(std::chrono::hours(24)),
                             getBolusFormulationAndRoute(),
                             getBolusFormulationAndRoute().getAbsorptionModel(),
                             Duration(std::chrono::minutes(20)),

                             static_cast<int>(CYCLE_SIZE));
        std::shared_ptr<Tucuxi::Core::IntakeIntervalCalculator> calculator = std::make_shared<Tucuxi::Core::ConstantEliminationBolus>();
        _intakes[0].setCalculator(calculator);
        _intakes[1].setCalculator(calculator);


        //Definition of the parameters


        Tucuxi::Core::ParameterDefinitions parameterDefs;
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestA0", 1.0, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestR0", 0.0, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestS0", 0.1, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestM0", 0.0, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestA1", 2.0, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestR1", 0.0, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestS1", 0.0, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestM1", 0.0, Tucuxi::Core::ParameterVariabilityType::None)));
        Tucuxi::Core::ParameterSetEvent parameters(DateTime::now(), parameterDefs);
        Tucuxi::Core::ParameterSetSeries parametersSeries;
        parametersSeries.addParameterSetEvent(parameters);


        //Definition of the Concentration Calculator

        //Tucuxi::Core::MultiConcentrationPredictionPtr predictionPtr;
        /*{
            predictionPtr = std::make_unique<Tucuxi::Core::MultiConcentrationPrediction>();

            DateTime recordFrom = date::year_month_day(date::year(2018), date::month(9), date::day(1));
            DateTime recordTo = recordFrom + interval;

            Tucuxi::Core::IntakeSeries intakeSeries;
            std::shared_ptr<IntakeIntervalCalculator> calculator2 = std::make_shared<ConstantEliminationBolus>();
            intakeEvent.setCalculator(calculator2);
            intakeSeries.push_back(intakeEvent);
            Tucuxi::Core::IMultiConcentrationCalculator *concentrationCalculator = new Tucuxi::Core::MultiConcentrationCalculator();
            auto status = concentrationCalculator->computeConcentrations(
                predictionPtr,
                false,
                recordFrom,
                recordTo,
                intakeSeries,
                _parameters);
            delete concentrationCalculator;
            fructose_assert_eq(status, ComputingStatus::Ok);

    #if 0
            for (int i = 0; i<nbPoints; i++) {
                std::cout << i << ":" << predictionPtr->getValues()[0][i] << std::endl;
            }
    #endif
        }
        */



            Tucuxi::Core::MultiLikelihood aux(omega, m_residualErrorModel, _samples, _intakes, _parameters, _concentrationCalculator);

            // Set initial etas to 0 for CL and V

            _etas.push_back(0.1);
            //_etas.push_back(0.0);

            Value x = aux.negativeLogLikelihood(_etas);

            // We compute the expected result

            // Prior: 0.5 * (_etas.transpose() * m_inverseOmega * _etas + m_omegaAdd)
            // m_omega = m_omegaAdd(static_cast<double>(_omega.rows()) * log(2 * PI) + log(_omega.determinant()))
            // sample likelihood: - _residualErrorModel.calculateSampleLikelihood(_expected, _observed.getValue());
            double expectedSampleValue1 = 121.1;
            double expectedSampleValue2 = 41.1;
            double omegaAdd = static_cast<double>(omega.rows()) * log(2 * PI) + log(omega.determinant());

            EigenVector etasmd(1);
            etasmd[0] = 0.1;

            double expectedValue = 0.5 * (etasmd.transpose() * omega.inverse() * etasmd + omegaAdd) -
                    m_residualErrorModel[0]->calculateSampleLikelihood(expectedSampleValue1, s0.getValue()) - m_residualErrorModel[1]->calculateSampleLikelihood(expectedSampleValue2, s1.getValue());
            fructose_assert_double_eq(x, expectedValue);
    }

    void test6(const std::string& /* _testName */ ){

        //A 2-analyte with one sample per analytes at the same time
        if (verbose()) {
              std::cout << __FUNCTION__ << std::endl;
          }


        Tucuxi::Core::OmegaMatrix omega;
        std::vector<IResidualErrorModel*> m_residualErrorModel;
        std::vector<SampleSeries> _samples;
        IntakeSeries _intakes;
        ParameterSetSeries _parameters;
        MultiConcentrationCalculator _concentrationCalculator;
        Etas _etas;

        //definition of the omega matrix
        omega = Tucuxi::Core::OmegaMatrix(1,1);
        omega(0,0) = 0.1; // Variance of A



        //definition of the m_residualErrorModel


        SigmaResidualErrorModel *newErrorModel1 = new SigmaResidualErrorModel();
        Tucuxi::Core::Sigma sigma1(1);
        sigma1(0) = 0.3138;
        newErrorModel1->setErrorModel(Tucuxi::Core::ResidualErrorType::PROPORTIONAL);
        newErrorModel1->setSigma(sigma1);
        m_residualErrorModel.push_back(newErrorModel1);
        //here i'm supposed to use a vector of pointers of IResidualErrorModel or a vector of pointers of SigmaResidualErrorModel



        SigmaResidualErrorModel *newErrorModel2 = new SigmaResidualErrorModel();
        Tucuxi::Core::Sigma sigma2(1);
        sigma2(0) = 0.3138;
        newErrorModel1->setErrorModel(Tucuxi::Core::ResidualErrorType::PROPORTIONAL);
        newErrorModel1->setSigma(sigma2);
        m_residualErrorModel.push_back(newErrorModel2);
        //here i'm supposed to use a vector of pointers of IResidualErrorModel or a vector of pointers of SigmaResidualErrorModel

        //definition of the samples
        Tucuxi::Core::SampleSeries sampleSeries1;
        DateTime date0 = DateTime((date::year(2017), date::month(6), date::day(6), Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))));
        Tucuxi::Core::SampleEvent s0(date0, 200.0);
        sampleSeries1.push_back(s0);
        _samples.push_back(sampleSeries1);

        Tucuxi::Core::SampleSeries sampleSeries2;
        DateTime date1 = DateTime((date::year(2017), date::month(6), date::day(6), Duration(std::chrono::hours(16), std::chrono::minutes(30), std::chrono::seconds(0))));
        Tucuxi::Core::SampleEvent s1(date1, 200.0);
        sampleSeries2.push_back(s0);
         _samples.push_back(sampleSeries2);


        //definition of the intakes


        _intakes.emplace_back(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(6)),
                                      Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
                             Duration(),
                             DoseValue(200.0),
                             TucuUnit("mg"),
                             Duration(std::chrono::hours(24)),
                             getBolusFormulationAndRoute(),
                             getBolusFormulationAndRoute().getAbsorptionModel(),
                             Duration(std::chrono::minutes(20)),
                             static_cast<int>(CYCLE_SIZE));

        _intakes.emplace_back(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(7)),
                                      Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
                             Duration(),
                             DoseValue(200.0),
                             TucuUnit("mg"),
                             Duration(std::chrono::hours(24)),
                             getBolusFormulationAndRoute(),
                             getBolusFormulationAndRoute().getAbsorptionModel(),
                             Duration(std::chrono::minutes(20)),

                             static_cast<int>(CYCLE_SIZE));
        std::shared_ptr<Tucuxi::Core::IntakeIntervalCalculator> calculator = std::make_shared<Tucuxi::Core::ConstantEliminationBolus>();
        _intakes[0].setCalculator(calculator);
        _intakes[1].setCalculator(calculator);


        //Definition of the parameters


        Tucuxi::Core::ParameterDefinitions parameterDefs;
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestA0", 1.0, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestR0", 0.0, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestS0", 0.1, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestM0", 0.0, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestA1", 2.0, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestR1", 0.0, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestS1", 0.0, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestM1", 0.0, Tucuxi::Core::ParameterVariabilityType::None)));
        Tucuxi::Core::ParameterSetEvent parameters(DateTime::now(), parameterDefs);
        Tucuxi::Core::ParameterSetSeries parametersSeries;
        parametersSeries.addParameterSetEvent(parameters);


        //Definition of the Concentration Calculator

        //Tucuxi::Core::MultiConcentrationPredictionPtr predictionPtr;
        /*{
            predictionPtr = std::make_unique<Tucuxi::Core::MultiConcentrationPrediction>();

            DateTime recordFrom = date::year_month_day(date::year(2018), date::month(9), date::day(1));
            DateTime recordTo = recordFrom + interval;

            Tucuxi::Core::IntakeSeries intakeSeries;
            std::shared_ptr<IntakeIntervalCalculator> calculator2 = std::make_shared<ConstantEliminationBolus>();
            intakeEvent.setCalculator(calculator2);
            intakeSeries.push_back(intakeEvent);
            Tucuxi::Core::IMultiConcentrationCalculator *concentrationCalculator = new Tucuxi::Core::MultiConcentrationCalculator();
            auto status = concentrationCalculator->computeConcentrations(
                predictionPtr,
                false,
                recordFrom,
                recordTo,
                intakeSeries,
                _parameters);
            delete concentrationCalculator;
            fructose_assert_eq(status, ComputingStatus::Ok);

    #if 0
            for (int i = 0; i<nbPoints; i++) {
                std::cout << i << ":" << predictionPtr->getValues()[0][i] << std::endl;
            }
    #endif
        }
        */



            Tucuxi::Core::MultiLikelihood aux(omega, m_residualErrorModel, _samples, _intakes, _parameters, _concentrationCalculator);

            // Set initial etas to 0 for CL and V

            _etas.push_back(0.1);
            //_etas.push_back(0.0);

            Value x = aux.negativeLogLikelihood(_etas);

            // We compute the expected result

            // Prior: 0.5 * (_etas.transpose() * m_inverseOmega * _etas + m_omegaAdd)
            // m_omega = m_omegaAdd(static_cast<double>(_omega.rows()) * log(2 * PI) + log(_omega.determinant()))
            // sample likelihood: - _residualErrorModel.calculateSampleLikelihood(_expected, _observed.getValue());
            double expectedSampleValue = 201.1;
            double omegaAdd = static_cast<double>(omega.rows()) * log(2 * PI) + log(omega.determinant());

            EigenVector etasmd(1);
            etasmd[0] = 0.1;

            double expectedValue = 0.5 * (etasmd.transpose() * omega.inverse() * etasmd + omegaAdd) -
                    m_residualErrorModel[0]->calculateSampleLikelihood(expectedSampleValue, s0.getValue()) - m_residualErrorModel[1]->calculateSampleLikelihood(expectedSampleValue, s1.getValue());   //WE CAN NOT USE 2.0 * m_residual... as they are 2 different analytes
            fructose_assert_double_eq(x, expectedValue);
    }


    void test7(const std::string& /* _testName */ ){

        //A 2-analyte with three samples per analyte, with one of them at the same time, the others at different times.


        if (verbose()) {
              std::cout << __FUNCTION__ << std::endl;
          }


        Tucuxi::Core::OmegaMatrix omega;
        std::vector<IResidualErrorModel*> m_residualErrorModel;
        std::vector<SampleSeries> _samples;
        IntakeSeries _intakes;
        ParameterSetSeries _parameters;
        MultiConcentrationCalculator _concentrationCalculator;
        Etas _etas;

        //definition of the omega matrix
        omega = Tucuxi::Core::OmegaMatrix(1,1);
        omega(0,0) = 0.1; // Variance of A



        //definition of the m_residualErrorModel


        SigmaResidualErrorModel *newErrorModel1 = new SigmaResidualErrorModel();
        Tucuxi::Core::Sigma sigma1(1);
        sigma1(0) = 0.3138;
        newErrorModel1->setErrorModel(Tucuxi::Core::ResidualErrorType::PROPORTIONAL);
        newErrorModel1->setSigma(sigma1);
        m_residualErrorModel.push_back(newErrorModel1);
        //here i'm supposed to use a vector of pointers of IResidualErrorModel or a vector of pointers of SigmaResidualErrorModel



        SigmaResidualErrorModel *newErrorModel2 = new SigmaResidualErrorModel();
        Tucuxi::Core::Sigma sigma2(1);
        sigma2(0) = 0.3138;
        newErrorModel1->setErrorModel(Tucuxi::Core::ResidualErrorType::PROPORTIONAL);
        newErrorModel1->setSigma(sigma2);
        m_residualErrorModel.push_back(newErrorModel2);
        //here i'm supposed to use a vector of pointers of IResidualErrorModel or a vector of pointers of SigmaResidualErrorModel

        //definition of the samples
        Tucuxi::Core::SampleSeries sampleSeries1;

        DateTime date0 = DateTime((date::year(2017), date::month(6), date::day(6), Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))));
        Tucuxi::Core::SampleEvent s0(date0, 200.0);
        sampleSeries1.push_back(s0);

        DateTime date1 = DateTime((date::year(2017), date::month(6), date::day(6), Duration(std::chrono::hours(16), std::chrono::minutes(30), std::chrono::seconds(0))));
        Tucuxi::Core::SampleEvent s1(date1, 200.0);
        sampleSeries1.push_back(s1);

        DateTime date2 = DateTime((date::year(2017), date::month(6), date::day(6), Duration(std::chrono::hours(20), std::chrono::minutes(30), std::chrono::seconds(0))));
        Tucuxi::Core::SampleEvent s2(date2, 200.0);
        sampleSeries1.push_back(s2);


        _samples.push_back(sampleSeries1);

        Tucuxi::Core::SampleSeries sampleSeries2;

        DateTime date3 = DateTime((date::year(2017), date::month(6), date::day(6), Duration(std::chrono::hours(12), std::chrono::minutes(30), std::chrono::seconds(0))));
        Tucuxi::Core::SampleEvent s3(date3, 200.0);
        sampleSeries1.push_back(s3);

        DateTime date4 = DateTime((date::year(2017), date::month(6), date::day(6), Duration(std::chrono::hours(16), std::chrono::minutes(30), std::chrono::seconds(0))));
        Tucuxi::Core::SampleEvent s4(date4, 200.0);
        sampleSeries1.push_back(s4);

        DateTime date5 = DateTime((date::year(2017), date::month(6), date::day(6), Duration(std::chrono::hours(20), std::chrono::minutes(30), std::chrono::seconds(0))));
        Tucuxi::Core::SampleEvent s5(date5, 200.0);
        sampleSeries1.push_back(s5);

         _samples.push_back(sampleSeries2);


        //definition of the intakes


        _intakes.emplace_back(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(6)),
                                      Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
                             Duration(),
                             DoseValue(200.0),
                             TucuUnit("mg"),
                             Duration(std::chrono::hours(24)),
                             getBolusFormulationAndRoute(),
                             getBolusFormulationAndRoute().getAbsorptionModel(),
                             Duration(std::chrono::minutes(20)),
                             static_cast<int>(CYCLE_SIZE));

        _intakes.emplace_back(DateTime(date::year_month_day(date::year(2017), date::month(6), date::day(7)),
                                      Duration(std::chrono::hours(8), std::chrono::minutes(30), std::chrono::seconds(0))),
                             Duration(),
                             DoseValue(200.0),
                             TucuUnit("mg"),
                             Duration(std::chrono::hours(24)),
                             getBolusFormulationAndRoute(),
                             getBolusFormulationAndRoute().getAbsorptionModel(),
                             Duration(std::chrono::minutes(20)),

                             static_cast<int>(CYCLE_SIZE));
        std::shared_ptr<Tucuxi::Core::IntakeIntervalCalculator> calculator = std::make_shared<Tucuxi::Core::ConstantEliminationBolus>();
        _intakes[0].setCalculator(calculator);
        _intakes[1].setCalculator(calculator);


        //Definition of the parameters


        Tucuxi::Core::ParameterDefinitions parameterDefs;
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestA0", 1.0, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestR0", 0.0, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestS0", 0.1, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestM0", 0.0, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestA1", 2.0, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestR1", 0.0, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestS1", 0.0, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestM1", 0.0, Tucuxi::Core::ParameterVariabilityType::None)));
        Tucuxi::Core::ParameterSetEvent parameters(DateTime::now(), parameterDefs);
        Tucuxi::Core::ParameterSetSeries parametersSeries;
        parametersSeries.addParameterSetEvent(parameters);


        //Definition of the Concentration Calculator

        //Tucuxi::Core::MultiConcentrationPredictionPtr predictionPtr;
        /*{
            predictionPtr = std::make_unique<Tucuxi::Core::MultiConcentrationPrediction>();

            DateTime recordFrom = date::year_month_day(date::year(2018), date::month(9), date::day(1));
            DateTime recordTo = recordFrom + interval;

            Tucuxi::Core::IntakeSeries intakeSeries;
            std::shared_ptr<IntakeIntervalCalculator> calculator2 = std::make_shared<ConstantEliminationBolus>();
            intakeEvent.setCalculator(calculator2);
            intakeSeries.push_back(intakeEvent);
            Tucuxi::Core::IMultiConcentrationCalculator *concentrationCalculator = new Tucuxi::Core::MultiConcentrationCalculator();
            auto status = concentrationCalculator->computeConcentrations(
                predictionPtr,
                false,
                recordFrom,
                recordTo,
                intakeSeries,
                _parameters);
            delete concentrationCalculator;
            fructose_assert_eq(status, ComputingStatus::Ok);

    #if 0
            for (int i = 0; i<nbPoints; i++) {
                std::cout << i << ":" << predictionPtr->getValues()[0][i] << std::endl;
            }
    #endif
        }
        */



            Tucuxi::Core::MultiLikelihood aux(omega, m_residualErrorModel, _samples, _intakes, _parameters, _concentrationCalculator);

            // Set initial etas to 0 for CL and V

            _etas.push_back(0.1);
            //_etas.push_back(0.0);

            Value x = aux.negativeLogLikelihood(_etas);

            // We compute the expected result

            // Prior: 0.5 * (_etas.transpose() * m_inverseOmega * _etas + m_omegaAdd)
            // m_omega = m_omegaAdd(static_cast<double>(_omega.rows()) * log(2 * PI) + log(_omega.determinant()))
            // sample likelihood: - _residualErrorModel.calculateSampleLikelihood(_expected, _observed.getValue());
            double expectedSampleValue1 = 201.1;
            double expectedSampleValue2 = 201.1;
            double expectedSampleValue3 = 201.1;
            double expectedSampleValue4 = 201.1;
            double expectedSampleValue5 = 201.1;
            double expectedSampleValue6 = 201.1;
            double omegaAdd = static_cast<double>(omega.rows()) * log(2 * PI) + log(omega.determinant());

            EigenVector etasmd(1);
            etasmd[0] = 0.1;

            double expectedValue = 0.5 * (etasmd.transpose() * omega.inverse() * etasmd + omegaAdd) -
                    m_residualErrorModel[0]->calculateSampleLikelihood(expectedSampleValue1, s0.getValue()) - m_residualErrorModel[0]->calculateSampleLikelihood(expectedSampleValue2, s1.getValue() -m_residualErrorModel[0]->calculateSampleLikelihood(expectedSampleValue3, s2.getValue()) - m_residualErrorModel[1]->calculateSampleLikelihood(expectedSampleValue4, s3.getValue()) - m_residualErrorModel[1]->calculateSampleLikelihood(expectedSampleValue5, s4.getValue()) -m_residualErrorModel[1]->calculateSampleLikelihood(expectedSampleValue6, s5.getValue()));
            fructose_assert_double_eq(x, expectedValue);
    }

/*     template<class CalculatorClass>
     void testCalculator(const Tucuxi::Core::ParameterSetSeries &_parameters,
                            double _dose,
                            Tucuxi::Core::AbsorptionModel _route,
                            std::chrono::hours _interval,
                            std::chrono::seconds _infusionTime,
                            CycleSize _nbPoints);

*/

};

}

}
