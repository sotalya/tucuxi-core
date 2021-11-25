#ifndef TEST_MULTILIKELIHOOD_H
#define TEST_MULTILIKELIHOOD_H

#endif // TEST_MULTILIKELIHOOD_H

#include <iostream>
#include <memory>

#include "fructose/fructose.h"

#include "tucucommon/general.h"

#include "tucucore/dosage.h"
#include "tucucore/intakeextractor.h"
#include "tucucore/intakeintervalcalculator.h"
#include "tucucore/concentrationcalculator.h"
#include "tucucore/multiconcentrationcalculator.h"
#include "tucucore/pkmodels/onecompartmentbolus.h"
#include "tucucore/pkmodels/onecompartmentinfusion.h"
#include "tucucore/pkmodels/onecompartmentextra.h"
#include "tucucore/pkmodels/twocompartmentbolus.h"
#include "tucucore/pkmodels/twocompartmentinfusion.h"
#include "tucucore/pkmodels/twocompartmentextra.h"
#include "tucucore/pkmodels/threecompartmentbolus.h"
#include "tucucore/pkmodels/threecompartmentinfusion.h"
#include "tucucore/pkmodels/threecompartmentextra.h"

#include "pkmodels/constanteliminationbolus.h"
#include "pkmodels/multiconstanteliminationbolus.h"

#include "tucucore/multilikelihood.h"

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


    template<class CalculatorClass>

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
        omega = Tucuxi::Core::OmegaMatrix(2,2);
        omega(0,0) = 0.356 * 0.356; // Variance of CL
        omega(0,1) = 0.798 * 0.356 * 0.629; // Covariance of CL and V
        omega(1,1) = 0.629 * 0.629; // Variance of V
        omega(1,0) = 0.798 * 0.356 * 0.629; // Covariance of CL and V






        //definition of the m_residualErrorModel


        SigmaResidualErrorModel *newErrorModel = new SigmaResidualErrorModel();
        m_residualErrorModel.push_back(newErrorModel);
        //here i'm supposed to use a vector of pointers of IResidualErrorModel or a vector of pointers of SigmaResidualErrorModel

        //definition of the samples
        Tucuxi::Core::SampleSeries sampleSeries;
        DateTime date0 = date::year_month_day(date::year(2018), date::month(9), date::day(1));

        Tucuxi::Core::SampleEvent s0(date0);
        sampleSeries.push_back(s0);

        _samples.push_back(sampleSeries);



        //definition of the intakes


        Tucuxi::Common::Duration offsetTime = 0s;
        Tucuxi::Common::Duration interval = 24h;
        Tucuxi::Common::Duration infusionTime = 0h;
        double dose = 400;
        Tucuxi::Core::AbsorptionModel route = Tucuxi::Core::AbsorptionModel::Extravascular;
        Tucuxi::Core::TimeOffsets times;
        Tucuxi::Core::IntakeEvent intakeEvent(DateTime::now(), offsetTime, dose, Tucuxi::Common::TucuUnit("mg"), interval, Tucuxi::Core::FormulationAndRoute(route), route, infusionTime, CYCLE_SIZE);
        std::shared_ptr<Tucuxi::Core::IntakeIntervalCalculator> calculator = std::make_shared<Tucuxi::Core::ConstantEliminationBolus>();
        intakeEvent.setCalculator(calculator);
        _intakes.push_back(intakeEvent);


        //Definition of the parameters


        Tucuxi::Core::ParameterDefinitions parameterDefs;
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestA", 0.0, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestR", 0.0, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestS", 0.0, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestM", 1.0, Tucuxi::Core::ParameterVariabilityType::None)));
        Tucuxi::Core::ParameterSetEvent parameters(DateTime::now(), parameterDefs);
        Tucuxi::Core::ParameterSetSeries parametersSeries;
        parametersSeries.addParameterSetEvent(parameters);


        //Definition of the Concentration Calculator

        Tucuxi::Core::MultiConcentrationPredictionPtr predictionPtr;
        {
            predictionPtr = std::make_unique<Tucuxi::Core::MultiConcentrationPrediction>();

            DateTime recordFrom = date::year_month_day(date::year(2018), date::month(9), date::day(1));
            DateTime recordTo = recordFrom + interval;

            Tucuxi::Core::IntakeSeries intakeSeries;
            std::shared_ptr<IntakeIntervalCalculator> calculator2 = std::make_shared<CalculatorClass>();
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



            Tucuxi::Core::MultiLikelihood aux(omega, m_residualErrorModel, _samples, _intakes, _parameters, _concentrationCalculator);

            // Set initial etas to 0 for CL and V
            _etas.push_back(0.0);
            _etas.push_back(0.0);

            aux.negativeLogLikelihood(_etas);

    }







    void test2(const std::string& /* _testName */){
        
        //Second scenario: A single analyte and 3 samples

        Tucuxi::Core::OmegaMatrix omega;
        std::vector<IResidualErrorModel*> m_residualErrorModel;
        std::vector<SampleSeries> _samples;
        IntakeSeries _intakes;
        ParameterSetSeries _parameters;
        MultiConcentrationCalculator _concentrationCalculator;
        Etas _etas;

        //definition of the omega matrix
        omega = Tucuxi::Core::OmegaMatrix(2,2);
        omega(0,0) = 0.356 * 0.356; // Variance of CL
        omega(0,1) = 0.798 * 0.356 * 0.629; // Covariance of CL and V
        omega(1,1) = 0.629 * 0.629; // Variance of V
        omega(1,0) = 0.798 * 0.356 * 0.629; // Covariance of CL and V



        //definition of the m_residualErrorModel


        SigmaResidualErrorModel *newErrorModel = new SigmaResidualErrorModel();
        m_residualErrorModel.push_back(newErrorModel);
        //here i'm supposed to use a vector of pointers of IResidualErrorModel or a vector of pointers of SigmaResidualErrorModel

        //definition of the samples
        Tucuxi::Core::SampleSeries sampleSeries;
        DateTime date0 = date::year_month_day(date::year(2018), date::month(9), date::day(1));

        Tucuxi::Core::SampleEvent s0(date0);
        sampleSeries.push_back(s0);


        DateTime date1 = date::year_month_day(date::year(2019), date::month(9), date::day(1));

        Tucuxi::Core::SampleEvent s1(date1);
        sampleSeries.push_back(s1);


        DateTime date2 = date::year_month_day(date::year(2020), date::month(9), date::day(1));

        Tucuxi::Core::SampleEvent s2(date2);
        sampleSeries.push_back(s2);
        _samples.push_back(sampleSeries);



        //definition of the intakes


        Tucuxi::Common::Duration offsetTime = 0s;
        Tucuxi::Common::Duration interval = 24h;
        Tucuxi::Common::Duration infusionTime = 0h;
        double dose = 400;
        Tucuxi::Core::AbsorptionModel route = Tucuxi::Core::AbsorptionModel::Extravascular;
        Tucuxi::Core::TimeOffsets times;
        Tucuxi::Core::IntakeEvent intakeEvent(DateTime::now(), offsetTime, dose, Tucuxi::Common::TucuUnit("mg"), interval, Tucuxi::Core::FormulationAndRoute(route), route, infusionTime, CYCLE_SIZE);
        std::shared_ptr<Tucuxi::Core::IntakeIntervalCalculator> calculator = std::make_shared<Tucuxi::Core::ConstantEliminationBolus>();
        intakeEvent.setCalculator(calculator);
        _intakes.push_back(intakeEvent);


        //Definition of the parameters


        Tucuxi::Core::ParameterDefinitions parameterDefs;
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestA", 0.0, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestR", 0.0, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestS", 0.0, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestM", 1.0, Tucuxi::Core::ParameterVariabilityType::None)));
        Tucuxi::Core::ParameterSetEvent parameters(DateTime::now(), parameterDefs);
        Tucuxi::Core::ParameterSetSeries parametersSeries;
        parametersSeries.addParameterSetEvent(parameters);

        //Definition of the Concentration Calculator

        Tucuxi::Core::MultiConcentrationPredictionPtr predictionPtr;
        {
            predictionPtr = std::make_unique<Tucuxi::Core::MultiConcentrationPrediction>();

            DateTime recordFrom = date::year_month_day(date::year(2018), date::month(9), date::day(1));
            DateTime recordTo = recordFrom + interval;

            Tucuxi::Core::IntakeSeries intakeSeries;
            std::shared_ptr<IntakeIntervalCalculator> calculator2 = std::make_shared<CalculatorClass>();
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

            Tucuxi::Core::MultiLikelihood aux(omega, m_residualErrorModel, _samples, _intakes, _parameters, _concentrationCalculator);


            // Set initial etas to 0 for CL and V
            _etas.push_back(0.0);
            _etas.push_back(0.0);


            Value x = aux.negativeLogLikelihood(_etas);
    }
    
    
    void test3(const std::string& /* _testName */) //INACABADO, HAY QUE ANADIR LOS PARAMETROS Y TODO
        // Third scenario: A 2-analyte with one sample on analyte 1 (using MultiConstantEliminationBolus)

    {



            Tucuxi::Core::OmegaMatrix omega;
            std::vector<IResidualErrorModel*> m_residualErrorModel;
            std::vector<SampleSeries> _samples;
            IntakeSeries _intakes;
            ParameterSetSeries _parameters;
            MultiConcentrationCalculator _concentrationCalculator;

            //definition of the omega matrix
            omega = Tucuxi::Core::OmegaMatrix(2,2);
            omega(0,0) = 0.356 * 0.356; // Variance of CL
            omega(0,1) = 0.798 * 0.356 * 0.629; // Covariance of CL and V
            omega(1,1) = 0.629 * 0.629; // Variance of V
            omega(1,0) = 0.798 * 0.356 * 0.629; // Covariance of CL and V



            //definition of the m_residualErrorModel


            SigmaResidualErrorModel *newErrorModel1 = new SigmaResidualErrorModel();
            m_residualErrorModel.push_back(newErrorModel1);

            SigmaResidualErrorModel *newErrorModel2 = new SigmaResidualErrorModel();
            m_residualErrorModel.push_back(newErrorModel2);
            //here i'm supposed to use a vector of pointers of IResidualErrorModel or a vector of pointers of SigmaResidualErrorModel

            //definition of the samples
            Tucuxi::Core::SampleSeries sampleSeries;
            DateTime date0 = date::year_month_day(date::year(2018), date::month(9), date::day(1));

            Tucuxi::Core::SampleEvent s0(date0);

            sampleSeries.push_back(s0);


            DateTime dateempty = DateTime::undefinedDateTime();
            Tucuxi::Core::SampleEvent s1(dateempty);

            sampleSeries.push_back(s1);


            _samples.push_back(sampleSeries);


            //definition of the intakes


            Tucuxi::Common::Duration offsetTime = 0s;
            Tucuxi::Common::Duration interval = 24h;
            Tucuxi::Common::Duration infusionTime = 0h;
            double dose = 400;
            Tucuxi::Core::AbsorptionModel route = Tucuxi::Core::AbsorptionModel::Extravascular;
            Tucuxi::Core::TimeOffsets times;
            Tucuxi::Core::IntakeEvent intakeEvent(DateTime::now(), offsetTime, dose, Tucuxi::Common::TucuUnit("mg"), interval, Tucuxi::Core::FormulationAndRoute(route), route, infusionTime, CYCLE_SIZE);
            std::shared_ptr<Tucuxi::Core::IntakeIntervalCalculator> calculator = std::make_shared<Tucuxi::Core::MultiConstantEliminationBolus>();
            intakeEvent.setCalculator(calculator);
            _intakes.push_back(intakeEvent);


            //Definition of the parameters


            Tucuxi::Core::ParameterDefinitions parameterDefs;
            parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestA0", 1.0, Tucuxi::Core::ParameterVariabilityType::None)));
            parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestR0", 2.0, Tucuxi::Core::ParameterVariabilityType::None)));
            parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestS0", 0.03, Tucuxi::Core::ParameterVariabilityType::None)));
            parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestM0", 4.0, Tucuxi::Core::ParameterVariabilityType::None)));
            parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestA1", 10.0, Tucuxi::Core::ParameterVariabilityType::None)));
            parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestR1", 20.0, Tucuxi::Core::ParameterVariabilityType::None)));
            parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestS1", 0.003, Tucuxi::Core::ParameterVariabilityType::None)));
            parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestM1", 40.0, Tucuxi::Core::ParameterVariabilityType::None)));
            Tucuxi::Core::ParameterSetEvent parameters(DateTime::now(), parameterDefs);
            Tucuxi::Core::ParameterSetSeries parametersSeries;
            parametersSeries.addParameterSetEvent(parameters);


            //Definition of the Concentration Calculator

            Tucuxi::Core::MultiConcentrationPredictionPtr predictionPtr;
            {
                predictionPtr = std::make_unique<Tucuxi::Core::MultiConcentrationPrediction>();

                DateTime recordFrom = date::year_month_day(date::year(2018), date::month(9), date::day(1));
                DateTime recordTo = recordFrom + interval;

                Tucuxi::Core::IntakeSeries intakeSeries;
                std::shared_ptr<IntakeIntervalCalculator> calculator2 = std::make_shared<CalculatorClass>();
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



                Tucuxi::Core::MultiLikelihood aux(omega, m_residualErrorModel, _samples, _intakes, _parameters, _concentrationCalculator);

                ValueVector _etas;
                Value x = aux.negativeLogLikelihood(_etas);
        }


    void test4(const std::string& /* _testName */){
        //A 2-analyte with one sample on analyte 2




                Tucuxi::Core::OmegaMatrix omega;
                std::vector<IResidualErrorModel*> m_residualErrorModel;
                std::vector<SampleSeries> _samples;
                IntakeSeries _intakes;
                ParameterSetSeries _parameters;
                MultiConcentrationCalculator _concentrationCalculator;

                //definition of the omega matrix
                omega = Tucuxi::Core::OmegaMatrix(2,2);
                omega(0,0) = 0.356 * 0.356; // Variance of CL
                omega(0,1) = 0.798 * 0.356 * 0.629; // Covariance of CL and V
                omega(1,1) = 0.629 * 0.629; // Variance of V
                omega(1,0) = 0.798 * 0.356 * 0.629; // Covariance of CL and V



                //definition of the m_residualErrorModel


                SigmaResidualErrorModel *newErrorModel1 = new SigmaResidualErrorModel();
                m_residualErrorModel.push_back(newErrorModel1);

                SigmaResidualErrorModel *newErrorModel2 = new SigmaResidualErrorModel();
                m_residualErrorModel.push_back(newErrorModel2);
                //here i'm supposed to use a vector of pointers of IResidualErrorModel or a vector of pointers of SigmaResidualErrorModel

                //definition of the samples
                Tucuxi::Core::SampleSeries sampleSeries;

                DateTime dateempty = DateTime::undefinedDateTime();
                Tucuxi::Core::SampleEvent s0(dateempty);
                sampleSeries.push_back(s0);



                DateTime date0 = date::year_month_day(date::year(2018), date::month(9), date::day(1));
                Tucuxi::Core::SampleEvent s1(date0);
                sampleSeries.push_back(s1);


                _samples.push_back(sampleSeries);


                //definition of the intakes


                Tucuxi::Common::Duration offsetTime = 0s;
                Tucuxi::Common::Duration interval = 24h;
                Tucuxi::Common::Duration infusionTime = 0h;
                double dose = 400;
                Tucuxi::Core::AbsorptionModel route = Tucuxi::Core::AbsorptionModel::Extravascular;
                Tucuxi::Core::TimeOffsets times;
                Tucuxi::Core::IntakeEvent intakeEvent(DateTime::now(), offsetTime, dose, Tucuxi::Common::TucuUnit("mg"), interval, Tucuxi::Core::FormulationAndRoute(route), route, infusionTime, CYCLE_SIZE);
                std::shared_ptr<Tucuxi::Core::IntakeIntervalCalculator> calculator = std::make_shared<Tucuxi::Core::MultiConstantEliminationBolus>();
                intakeEvent.setCalculator(calculator);
                _intakes.push_back(intakeEvent);


                //Definition of the parameters


                Tucuxi::Core::ParameterDefinitions parameterDefs;
                parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestA0", 1.0, Tucuxi::Core::ParameterVariabilityType::None)));
                parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestR0", 2.0, Tucuxi::Core::ParameterVariabilityType::None)));
                parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestS0", 0.03, Tucuxi::Core::ParameterVariabilityType::None)));
                parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestM0", 4.0, Tucuxi::Core::ParameterVariabilityType::None)));
                parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestA1", 10.0, Tucuxi::Core::ParameterVariabilityType::None)));
                parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestR1", 20.0, Tucuxi::Core::ParameterVariabilityType::None)));
                parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestS1", 0.003, Tucuxi::Core::ParameterVariabilityType::None)));
                parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestM1", 40.0, Tucuxi::Core::ParameterVariabilityType::None)));
                Tucuxi::Core::ParameterSetEvent parameters(DateTime::now(), parameterDefs);
                Tucuxi::Core::ParameterSetSeries parametersSeries;
                parametersSeries.addParameterSetEvent(parameters);


                //Definition of the Concentration Calculator

                Tucuxi::Core::MultiConcentrationPredictionPtr predictionPtr;
                {
                    predictionPtr = std::make_unique<Tucuxi::Core::MultiConcentrationPrediction>();

                    DateTime recordFrom = date::year_month_day(date::year(2018), date::month(9), date::day(1));
                    DateTime recordTo = recordFrom + interval;

                    Tucuxi::Core::IntakeSeries intakeSeries;
                    std::shared_ptr<IntakeIntervalCalculator> calculator2 = std::make_shared<CalculatorClass>();
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



                    Tucuxi::Core::MultiLikelihood aux(omega, m_residualErrorModel, _samples, _intakes, _parameters, _concentrationCalculator);

                    ValueVector _etas;
                    Value x = aux.negativeLogLikelihood(_etas);


    }

    void test5(const std::string& /* _testName */ ){

        //A 2-analyte with one sample per analytes at different times
        Tucuxi::Core::OmegaMatrix omega;
        std::vector<IResidualErrorModel*> m_residualErrorModel;
        std::vector<SampleSeries> _samples;
        IntakeSeries _intakes;
        ParameterSetSeries _parameters;
        MultiConcentrationCalculator _concentrationCalculator;

        //definition of the omega matrix
        omega = Tucuxi::Core::OmegaMatrix(2,2);
        omega(0,0) = 0.356 * 0.356; // Variance of CL
        omega(0,1) = 0.798 * 0.356 * 0.629; // Covariance of CL and V
        omega(1,1) = 0.629 * 0.629; // Variance of V
        omega(1,0) = 0.798 * 0.356 * 0.629; // Covariance of CL and V



        //definition of the m_residualErrorModel


        SigmaResidualErrorModel *newErrorModel1 = new SigmaResidualErrorModel();
        m_residualErrorModel.push_back(newErrorModel1);

        SigmaResidualErrorModel *newErrorModel2 = new SigmaResidualErrorModel();
        m_residualErrorModel.push_back(newErrorModel2);
        //here i'm supposed to use a vector of pointers of IResidualErrorModel or a vector of pointers of SigmaResidualErrorModel

        //definition of the samples
        Tucuxi::Core::SampleSeries sampleSeries;

        DateTime date0 = date::year_month_day(date::year(2015), date::month(9), date::day(1));
        Tucuxi::Core::SampleEvent s0(date0);
        sampleSeries.push_back(s0);



        DateTime date1 = date::year_month_day(date::year(2018), date::month(9), date::day(1));
        Tucuxi::Core::SampleEvent s1(date1);
        sampleSeries.push_back(s1);


        _samples.push_back(sampleSeries);


        //definition of the intakes


        Tucuxi::Common::Duration offsetTime = 0s;
        Tucuxi::Common::Duration interval = 24h;
        Tucuxi::Common::Duration infusionTime = 0h;
        double dose = 400;
        Tucuxi::Core::AbsorptionModel route = Tucuxi::Core::AbsorptionModel::Extravascular;
        Tucuxi::Core::TimeOffsets times;
        Tucuxi::Core::IntakeEvent intakeEvent(DateTime::now(), offsetTime, dose, Tucuxi::Common::TucuUnit("mg"), interval, Tucuxi::Core::FormulationAndRoute(route), route, infusionTime, CYCLE_SIZE);
        std::shared_ptr<Tucuxi::Core::IntakeIntervalCalculator> calculator = std::make_shared<Tucuxi::Core::MultiConstantEliminationBolus>();
        intakeEvent.setCalculator(calculator);
        _intakes.push_back(intakeEvent);


        //Definition of the parameters


        Tucuxi::Core::ParameterDefinitions parameterDefs;
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestA0", 1.0, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestR0", 2.0, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestS0", 0.03, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestM0", 4.0, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestA1", 10.0, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestR1", 20.0, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestS1", 0.003, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestM1", 40.0, Tucuxi::Core::ParameterVariabilityType::None)));
        Tucuxi::Core::ParameterSetEvent parameters(DateTime::now(), parameterDefs);
        Tucuxi::Core::ParameterSetSeries parametersSeries;
        parametersSeries.addParameterSetEvent(parameters);


        //Definition of the Concentration Calculator

        Tucuxi::Core::MultiConcentrationPredictionPtr predictionPtr;
        {
            predictionPtr = std::make_unique<Tucuxi::Core::MultiConcentrationPrediction>();

            DateTime recordFrom = date::year_month_day(date::year(2018), date::month(9), date::day(1));
            DateTime recordTo = recordFrom + interval;

            Tucuxi::Core::IntakeSeries intakeSeries;
            std::shared_ptr<IntakeIntervalCalculator> calculator2 = std::make_shared<CalculatorClass>();
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



            Tucuxi::Core::MultiLikelihood aux(omega, m_residualErrorModel, _samples, _intakes, _parameters, _concentrationCalculator);

            ValueVector _etas;
            Value x = aux.negativeLogLikelihood(_etas);
    }

    void test6(const std::string& /* _testName */ ){

        //A 2-analyte with one sample per analytes at different times
        Tucuxi::Core::OmegaMatrix omega;
        std::vector<IResidualErrorModel*> m_residualErrorModel;
        std::vector<SampleSeries> _samples;
        IntakeSeries _intakes;
        ParameterSetSeries _parameters;
        MultiConcentrationCalculator _concentrationCalculator;

        //definition of the omega matrix
        omega = Tucuxi::Core::OmegaMatrix(2,2);
        omega(0,0) = 0.356 * 0.356; // Variance of CL
        omega(0,1) = 0.798 * 0.356 * 0.629; // Covariance of CL and V
        omega(1,1) = 0.629 * 0.629; // Variance of V
        omega(1,0) = 0.798 * 0.356 * 0.629; // Covariance of CL and V



        //definition of the m_residualErrorModel


        SigmaResidualErrorModel *newErrorModel1 = new SigmaResidualErrorModel();
        m_residualErrorModel.push_back(newErrorModel1);

        SigmaResidualErrorModel *newErrorModel2 = new SigmaResidualErrorModel();
        m_residualErrorModel.push_back(newErrorModel2);
        //here i'm supposed to use a vector of pointers of IResidualErrorModel or a vector of pointers of SigmaResidualErrorModel

        //definition of the samples
        Tucuxi::Core::SampleSeries sampleSeries;

        DateTime date0 = date::year_month_day(date::year(2018), date::month(9), date::day(1));
        Tucuxi::Core::SampleEvent s0(date0);
        sampleSeries.push_back(s0);



        DateTime date1 = date::year_month_day(date::year(2018), date::month(9), date::day(1));
        Tucuxi::Core::SampleEvent s1(date1);
        sampleSeries.push_back(s1);


        _samples.push_back(sampleSeries);


        //definition of the intakes


        Tucuxi::Common::Duration offsetTime = 0s;
        Tucuxi::Common::Duration interval = 24h;
        Tucuxi::Common::Duration infusionTime = 0h;
        double dose = 400;
        Tucuxi::Core::AbsorptionModel route = Tucuxi::Core::AbsorptionModel::Extravascular;
        Tucuxi::Core::TimeOffsets times;
        Tucuxi::Core::IntakeEvent intakeEvent(DateTime::now(), offsetTime, dose, Tucuxi::Common::TucuUnit("mg"), interval, Tucuxi::Core::FormulationAndRoute(route), route, infusionTime, CYCLE_SIZE);
        std::shared_ptr<Tucuxi::Core::IntakeIntervalCalculator> calculator = std::make_shared<Tucuxi::Core::MultiConstantEliminationBolus>();
        intakeEvent.setCalculator(calculator);
        _intakes.push_back(intakeEvent);


        //Definition of the parameters


        Tucuxi::Core::ParameterDefinitions parameterDefs;
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestA0", 1.0, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestR0", 2.0, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestS0", 0.03, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestM0", 4.0, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestA1", 10.0, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestR1", 20.0, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestS1", 0.003, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestM1", 40.0, Tucuxi::Core::ParameterVariabilityType::None)));
        Tucuxi::Core::ParameterSetEvent parameters(DateTime::now(), parameterDefs);
        Tucuxi::Core::ParameterSetSeries parametersSeries;
        parametersSeries.addParameterSetEvent(parameters);


        //Definition of the Concentration Calculator

        Tucuxi::Core::MultiConcentrationPredictionPtr predictionPtr;
        {
            predictionPtr = std::make_unique<Tucuxi::Core::MultiConcentrationPrediction>();

            DateTime recordFrom = date::year_month_day(date::year(2018), date::month(9), date::day(1));
            DateTime recordTo = recordFrom + interval;

            Tucuxi::Core::IntakeSeries intakeSeries;
            std::shared_ptr<IntakeIntervalCalculator> calculator2 = std::make_shared<CalculatorClass>();
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



            Tucuxi::Core::MultiLikelihood aux(omega, m_residualErrorModel, _samples, _intakes, _parameters, _concentrationCalculator);

            ValueVector _etas;
            Value x = aux.negativeLogLikelihood(_etas);
    }


    void test7(const std::string& /* _testName */ ){

        //A 2-analyte with three samples per analyte, with one of them at the same time, the others at different times.

        Tucuxi::Core::OmegaMatrix omega;
        std::vector<IResidualErrorModel*> m_residualErrorModel;
        std::vector<SampleSeries> _samples;
        IntakeSeries _intakes;
        ParameterSetSeries _parameters;
        MultiConcentrationCalculator _concentrationCalculator;

        //definition of the omega matrix
        omega = Tucuxi::Core::OmegaMatrix(2,2);
        omega(0,0) = 0.356 * 0.356; // Variance of CL
        omega(0,1) = 0.798 * 0.356 * 0.629; // Covariance of CL and V
        omega(1,1) = 0.629 * 0.629; // Variance of V
        omega(1,0) = 0.798 * 0.356 * 0.629; // Covariance of CL and V



        //definition of the m_residualErrorModel


        SigmaResidualErrorModel *newErrorModel1 = new SigmaResidualErrorModel();
        m_residualErrorModel.push_back(newErrorModel1);

        SigmaResidualErrorModel *newErrorModel2 = new SigmaResidualErrorModel();
        m_residualErrorModel.push_back(newErrorModel2);
        //here i'm supposed to use a vector of pointers of IResidualErrorModel or a vector of pointers of SigmaResidualErrorModel

        //definition of the samples
        Tucuxi::Core::SampleSeries sampleSeries;

        DateTime date0 = date::year_month_day(date::year(2018), date::month(9), date::day(1));
        Tucuxi::Core::SampleEvent s0(date0);
        sampleSeries.push_back(s0);



        DateTime date1 = date::year_month_day(date::year(2015), date::month(9), date::day(1));
        Tucuxi::Core::SampleEvent s1(date1);
        sampleSeries.push_back(s1);

        DateTime date2 = date::year_month_day(date::year(2016), date::month(9), date::day(1));
        Tucuxi::Core::SampleEvent s2(date2);
        sampleSeries.push_back(s2);


        DateTime date3 = date::year_month_day(date::year(2018), date::month(9), date::day(1));
        Tucuxi::Core::SampleEvent s3(date3);
        sampleSeries.push_back(s3);


        DateTime date4 = date::year_month_day(date::year(2017), date::month(9), date::day(1));
        Tucuxi::Core::SampleEvent s4(date4);
        sampleSeries.push_back(s4);


        DateTime date5 = date::year_month_day(date::year(2019), date::month(9), date::day(1));
        Tucuxi::Core::SampleEvent s5(date5);
        sampleSeries.push_back(s5);

        _samples.push_back(sampleSeries);


        //definition of the intakes


        Tucuxi::Common::Duration offsetTime = 0s;
        Tucuxi::Common::Duration interval = 24h;
        Tucuxi::Common::Duration infusionTime = 0h;
        double dose = 400;
        Tucuxi::Core::AbsorptionModel route = Tucuxi::Core::AbsorptionModel::Extravascular;
        Tucuxi::Core::TimeOffsets times;
        Tucuxi::Core::IntakeEvent intakeEvent(DateTime::now(), offsetTime, dose, Tucuxi::Common::TucuUnit("mg"), interval, Tucuxi::Core::FormulationAndRoute(route), route, infusionTime, CYCLE_SIZE);
        std::shared_ptr<Tucuxi::Core::IntakeIntervalCalculator> calculator = std::make_shared<Tucuxi::Core::MultiConstantEliminationBolus>();
        intakeEvent.setCalculator(calculator);
        _intakes.push_back(intakeEvent);


        //Definition of the parameters


        Tucuxi::Core::ParameterDefinitions parameterDefs;
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestA0", 1.0, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestR0", 2.0, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestS0", 0.03, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestM0", 4.0, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestA1", 10.0, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestR1", 20.0, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestS1", 0.003, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("TestM1", 40.0, Tucuxi::Core::ParameterVariabilityType::None)));
        Tucuxi::Core::ParameterSetEvent parameters(DateTime::now(), parameterDefs);
        Tucuxi::Core::ParameterSetSeries parametersSeries;
        parametersSeries.addParameterSetEvent(parameters);


        //Definition of the Concentration Calculator

        Tucuxi::Core::MultiConcentrationPredictionPtr predictionPtr;
        {
            predictionPtr = std::make_unique<Tucuxi::Core::MultiConcentrationPrediction>();

            DateTime recordFrom = date::year_month_day(date::year(2018), date::month(9), date::day(1));
            DateTime recordTo = recordFrom + interval;

            Tucuxi::Core::IntakeSeries intakeSeries;
            std::shared_ptr<IntakeIntervalCalculator> calculator2 = std::make_shared<CalculatorClass>();
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



            Tucuxi::Core::MultiLikelihood aux(omega, m_residualErrorModel, _samples, _intakes, _parameters, _concentrationCalculator);

            ValueVector _etas;
            Value x = aux.negativeLogLikelihood(_etas);
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
