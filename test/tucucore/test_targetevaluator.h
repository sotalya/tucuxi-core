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

#include "tucucore/definitions.h"

#include "testutils.h"

namespace Tucuxi {
namespace Core {



struct TestTargetEvaluator : public fructose::test_base<TestTargetEvaluator>
{
    static const int CYCLE_SIZE = 251;

    static constexpr double NB_POINTS_PER_HOUR = CYCLE_SIZE / 24.0;

    TestTargetEvaluator() {}

    void test1(const std::string& /* _testName */)
    {
        static const double NB_CASE = 20;

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
                      Unit("mg/l"),
                      Value(20),    //min
                      Value(25),    //best
                      Value(30),    //max
                      Value(15),    //inefficacyAlarm
                      Value(50));   //toxicityAlarm

        TimeOffsets timeOffset;


        double lastTime = 50;
        double timeInterval = lastTime / NB_CASE;

        for(double i = 0; i < lastTime; i += timeInterval)
        {
            timeOffset.push_back(i);
            if(timeOffset.size() > NB_CASE)
            {
                timeOffset.pop_back();
            }
        }

        double minConcentration = 20; // between 20 and 30
        double lastConcentration = 30;// between 20 and 30
        double concentrationInterval = (lastConcentration - minConcentration) / NB_CASE;

        Concentrations concentrations;
        for(double i = minConcentration; i < lastConcentration; i += concentrationInterval)
        {
            concentrations.push_back(i);
            if(concentrations.size() > NB_CASE)
            {
                concentrations.pop_back();
            }
        }



        ConcentrationPrediction concentrationPrediction;

        concentrationPrediction.appendConcentrations(timeOffset, concentrations);



        TargetEvaluator targetEvaluator;

        ComputingStatus status = targetEvaluator.evaluate(concentrationPrediction,
                                                          expectedIntakes,
                                                          target,
                                                          targetEvaluationResult);

        fructose_assert(status == ComputingStatus::Ok);

        if(status == ComputingStatus::Ok)
        {
            fructose_assert(targetEvaluationResult.getUnit() == Unit("")); //WRONG must be final unit ?
            fructose_assert(targetEvaluationResult.getTargetType() == TargetType::Residual);
//            fructose_assert(targetEvaluationResult.getValue() == concentration.at(NB_CASE-1) dans l'unité finale; //
            //To compare score, create another function ?

        }



    }

};

}// Core
}// Tucuxi

#endif // TEST_TARGETEVALUATOR_H
