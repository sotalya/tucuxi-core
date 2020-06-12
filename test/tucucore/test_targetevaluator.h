#ifndef TEST_TARGETEVALUATOR_H
#define TEST_TARGETEVALUATOR_H

#include "fructose/fructose.h"

#include "tucucore/targetevaluator.h"
#include "tucucore/targetevaluationresult.h"
#include "tucucore/intakeevent.h"
#include "tucucore/drugmodel/formulationandroute.h"
#include "tucucore/drugtreatment/target.h"
#include "tucucore/concentrationprediction.h"

#include "tucucommon/timeofday.h"
#include "tucucommon/datetime.h"
#include "tucucommon/duration.h"

#include "tucucore/definitions.h"

#include "testutils.h"

namespace Tucuxi {
namespace Core {



struct TestTargetEvaluator : public fructose::test_base<TestTargetEvaluator>
{
    static const int CYCLE_SIZE = 251;

    static constexpr double NB_POINTS_PER_HOUR = CYCLE_SIZE / 24.0;

    static constexpr double NB_ARRAY_ELEMENTS = 20;

    TestTargetEvaluator() {}

    void testResidual(const std::string& /* _testName */)
    {


        IntakeSeries expectedIntakes;
        expectedIntakes.push_back(IntakeEvent(DATE_TIME_NO_VAR(2015, 06, 11, 11, 46, 23),
                                  Duration(),
                                  DoseValue(200.0),
                                  Duration(std::chrono::hours(7 * 24)),
                                  FormulationAndRoute(Formulation::Test, AdministrationRoute::IntravenousDrip, AbsorptionModel::Infusion),
                                  AbsorptionModel::Infusion,
                                  Duration(std::chrono::minutes(20)),
                                  static_cast<int>(7 * 24 * NB_POINTS_PER_HOUR + 1)));

        TargetEvaluationResult targetEvaluationResult;

        Target target(ActiveMoietyId("imatinib"),
                      TargetType::Residual,
                      Unit("mg/l"),     //unit
                      Unit("ug/l"),     //final unit
                      Value(20),        //min
                      Value(25),        //best
                      Value(30),        //max
                      Value(15),        //mic
                      Unit("mg/l"),     //mic Unit
                      Duration(),       //Not used
                      Duration(),       //Not used
                      Duration());      //Not used



        double lastTime = 50;
        TimeOffsets timeOffsets = fillTimeOffsets(lastTime);


        double minConcentration = 20;
        double maxConcentration = 30;
        Concentrations concentrations = fillConcentrations(minConcentration, maxConcentration);
        double higherValueConcentration = concentrations.at(NB_ARRAY_ELEMENTS - 1);



        ConcentrationPrediction concentrationPrediction;
        concentrationPrediction.appendConcentrations(timeOffsets, concentrations);



        TargetEvaluator targetEvaluator;
        ComputingStatus status = targetEvaluator.evaluate(concentrationPrediction,
                                                          expectedIntakes,
                                                          target,
                                                          targetEvaluationResult);


        fructose_assert(status == ComputingStatus::Ok);

        fructose_assert(targetEvaluationResult.getUnit() == Unit("ug/l")); //WRONG must be final unit ?
        fructose_assert(targetEvaluationResult.getTargetType() == TargetType::Residual);
        fructose_assert(targetEvaluationResult.getValue() == (higherValueConcentration * 1000)); //*1000 to convert in ug/l
        //To compare score, create another function ?

    }

    void testPeak(const std::string& /* _testName */)
    {

    }

    void testMean(const std::string& /* _testName */)
    {

    }

    void testAuc(const std::string& /* _testName */)
    {

    }

    void testAuc24(const std::string& /* _testName */)
    {

    }

    void testCumulativeAuc(const std::string& /* _testName */)
    {

    }

    void testAucOverMic(const std::string& /* _testName */)
    {

    }

    void testAuc24OverMic(const std::string& /* _testName */)
    {

    }

    void testTimeOverMic(const std::string& /* _testName */)
    {

    }

    void testAucDividedByMic(const std::string& /* _testName */)
    {

    }

    void testAuc24DividedByMic(const std::string& /* _testName */)
    {

    }

    void testPeakDividedByMic(const std::string& /* _testName */)
    {

    }


private:

    ///
    /// \brief fillConcentrations is used to fill Concentrations vector (fixed number) with incremental values between boudaries
    /// \param minValue : minimum asked concentration value
    /// \param maxValue : maximum asked concentration value
    /// \return Concentrations (Concentration vector)
    ///
    Concentrations fillConcentrations(double minValue, double maxValue)
    {
        Concentrations concentrations;

        double concentrationInterval = (maxValue - minValue) / (NB_ARRAY_ELEMENTS);

        for(double i = minValue; i < maxValue; i += concentrationInterval)
        {
            concentrations.push_back(i);
            if(concentrations.size() > NB_ARRAY_ELEMENTS)
            {
                concentrations.pop_back();
            }
        }

        return concentrations;

    }

    ///
    /// \brief fillTimeOffsets is used to fill TimeOffsets vector (fixed number) with incremental values between boudaries
    /// \param lastTime : maximum asked time value
    /// \return TimeOffsets (TimeOffset vector)
    ///
    TimeOffsets fillTimeOffsets(double lastTime)
    {
        TimeOffsets timeOffsets;

        double timeInterval = lastTime / NB_ARRAY_ELEMENTS;

        for(double i = 0; i < lastTime; i += timeInterval)
        {
            timeOffsets.push_back(i);
            if(timeOffsets.size() > NB_ARRAY_ELEMENTS)
            {
                timeOffsets.pop_back();
            }
        }

        return timeOffsets;
    }

};

}// Core
}// Tucuxi

#endif // TEST_TARGETEVALUATOR_H
