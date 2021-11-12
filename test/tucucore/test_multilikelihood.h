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


    void testSimple(const std::string& /* _testName */){

        Tucuxi::Core::OmegaMatrix omega;
        std::vector<IResidualErrorModel> m_residualErrorModel;
        std::vector<SampleSeries> _samples;
        IntakeSeries _intakes;
        ParameterSetSeries _parameters;
        MultiConcentrationCalculator _concentrationCalculator;

        const ValueVector _etas;

        Value x = Tucuxi::Core::MultiLikelihood::negativeLogLikelihood(_etas); //have to fix that and add the 7 scenarios, understood how to do it


    }

    void test1(const std::string& /* _testName */){
        Tucuxi::Core::OmegaMatrix omega;
        std::vector<IResidualErrorModel> m_residualErrorModel;
        std::SampleSeries _samples;
        IntakeSeries _intakes;
        ParameterSetSeries _parameters;
        MultiConcentrationCalculator _concentrationCalculator;

        const Value _etas;
        Value x = Tucuxi::Core::MultiLikelihood::negativeLogLikelihood(_etas);

    }

    void test2(const std::string& /* _testName */){
        Tucuxi::Core::OmegaMatrix omega;
        std::vector<IResidualErrorModel> m_residualErrorModel;
        std::vector<SampleSeries> _samples;
        IntakeSeries _intakes;
        ParameterSetSeries _parameters;
        MultiConcentrationCalculator _concentrationCalculator;

        const ValueVector _etas;
        Value x = Tucuxi::Core::MultiLikelihood::negativeLogLikelihood(_etas);
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
