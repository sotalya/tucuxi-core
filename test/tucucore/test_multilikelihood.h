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
#include "tucucore/pkmodels/onecompartmentbolus.h"
#include "tucucore/pkmodels/onecompartmentinfusion.h"
#include "tucucore/pkmodels/onecompartmentextra.h"
#include "tucucore/pkmodels/twocompartmentbolus.h"
#include "tucucore/pkmodels/twocompartmentinfusion.h"
#include "tucucore/pkmodels/twocompartmentextra.h"
#include "tucucore/pkmodels/threecompartmentbolus.h"
#include "tucucore/pkmodels/threecompartmentinfusion.h"
#include "tucucore/pkmodels/threecompartmentextra.h"

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



    void test1(const std::string& /* _testName */){ 
        
        //first scenario: Only a single analyte and one sample (Using ConstantEliminationBolus intakes)

        Tucuxi::Core::MultiLikelihood aux(
            Tucuxi::Core::OmegaMatrix omega,
            std::vector<IResidualErrorModel> m_residualErrorModel,
            SampleSeries _samples,
            IntakeSeries _intakes,
            ParameterSetSeries _parameters,
            MultiConcentrationCalculator _concentrationCalculator
        );

        Value _etas;
        Value x = aux.negativeLogLikelihood(_etas);

    }

    void test2(const std::string& /* _testName */){
        
        //Second scenario: A single analyte and 3 samples
        Tucuxi::Core::MultiLikelihood aux(
            Tucuxi::Core::OmegaMatrix omega,
            std::vector<IResidualErrorModel> m_residualErrorModel,
            std::vector<SampleSeries> _samples,     //it has to have size = 3, but it can't be modified as it is a parameter
            IntakeSeries _intakes,
            ParameterSetSeries _parameters,
            MultiConcentrationCalculator _concentrationCalculator
           );

        const ValueVector _etas;
        Value x = aux.negativeLogLikelihood(_etas);
    }
    
    
    void test3(const std::string& /* _testName */){
        //A 2-analyte with one sample on analyte 1 (using MultiConstantEliminationBolus)

        Tucuxi::Core::MultiLikelihood aux(
            Tucuxi::Core::OmegaMatrix omega,
            std::vector<IResidualErrorModel> m_residualErrorModel,
            std::vector<SampleSeries> _samples,
            IntakeSeries _intakes,
            ParameterSetSeries _parameters,
            MultiConcentrationCalculator _concentrationCalculator
           );

        const ValueVector _etas;
        Value _etas2;

        //As we have a vector of residual error models, one per analyte, and a vector of sample series, also one per analyte, we can iterate on
        //the size of m_residualErrorModel, as it will represent the number of analytes


        Value x = aux.negativeLogLikelihood(_etas); //analyte1
        Value x = aux.negativeLogLikelihood(_etas2); //analyte2
    }


    void test4(const std::string& /* _testName */){
        //A 2-analyte with one sample on analyte 2

        Tucuxi::Core::MultiLikelihood aux(
            Tucuxi::Core::OmegaMatrix omega,
            std::vector<IResidualErrorModel> m_residualErrorModel,
            std::vector<SampleSeries> _samples,
            IntakeSeries _intakes,
            ParameterSetSeries _parameters,
            MultiConcentrationCalculator _concentrationCalculator
           );

        const ValueVector _etas;
        Value _etas2;

        Value x = aux.negativeLogLikelihood(_etas2); //analyte1
        Value x = aux.negativeLogLikelihood(_etas); //analyte2

    }

    void test5(const std::string& /* _testName */ ){

        //A 2-analyte with one sample per analytes at different times

        Tucuxi::Core::MultiLikelihood aux(
            Tucuxi::Core::OmegaMatrix omega,
            std::vector<IResidualErrorModel> m_residualErrorModel,
            std::vector<SampleSeries> _samples,
            IntakeSeries _intakes,
            ParameterSetSeries _parameters,
            MultiConcentrationCalculator _concentrationCalculator
           );

        const ValueVector _etas;
        Value _etas2;

        Value x = aux.negativeLogLikelihood(_etas2); //analyte1
        Value x = aux.negativeLogLikelihood(_etas); //analyte2

        //THIS IS NOT GOOD, I HAVE TO LOOK FOR HOW TO DO THE THING OF THE TIMES
    }

    void test6(const std::string& /* _testName */ ){

        //A 2-analyte with one sample per analytes at different times

        Tucuxi::Core::MultiLikelihood aux(
            Tucuxi::Core::OmegaMatrix omega,
            std::vector<IResidualErrorModel> m_residualErrorModel,
            std::vector<SampleSeries> _samples,
            IntakeSeries _intakes,
            ParameterSetSeries _parameters,
            MultiConcentrationCalculator _concentrationCalculator
           );

        const ValueVector _etas;
        Value _etas2;

        Value x = aux.negativeLogLikelihood(_etas2); //analyte1
        Value x = aux.negativeLogLikelihood(_etas); //analyte2

        //THIS IS NOT GOOD, I HAVE TO LOOK FOR HOW TO DO THE THING OF THE TIMES
    }


    void test7(const std::string& /* _testName */ ){

        //A 2-analyte with one sample per analytes at different times

        Tucuxi::Core::MultiLikelihood aux(
            Tucuxi::Core::OmegaMatrix omega,
            std::vector<IResidualErrorModel> m_residualErrorModel,
            std::vector<SampleSeries> _samples,
            IntakeSeries _intakes,
            ParameterSetSeries _parameters,
            MultiConcentrationCalculator _concentrationCalculator
           );

        const ValueVector _etas;
        Value _etas2;

        Value x = aux.negativeLogLikelihood(_etas2); //analyte1
        Value x = aux.negativeLogLikelihood(_etas); //analyte2

        //THIS IS NOT GOOD, I HAVE TO LOOK FOR HOW TO DO THE THING OF THE TIMES
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
