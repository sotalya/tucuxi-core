#ifndef TEST_TARGETEVALUATOR_H
#define TEST_TARGETEVALUATOR_H

#include "fructose/fructose.h"

#include "tucucore/targetevaluator.h"
#include "tucucore/targetextractor.h"
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

using namespace Tucuxi::Core;

struct TestTargetEvaluator : public fructose::test_base<TestTargetEvaluator>
{
    static const int CYCLE_SIZE = 251;

    static constexpr double NB_POINTS_PER_HOUR = CYCLE_SIZE / 24.0;

    static constexpr double NB_ARRAY_ELEMENTS = 10;

    TestTargetEvaluator() {}

    void testResidual(const std::string& /* _testName */)
    {

        IntakeSeries expectedIntakes = createIntakeSeries();

        TargetEvaluationResult targetEvaluationResult;

        const Target target(ActiveMoietyId("imatinib"),
                      TargetType::Residual,
                      Unit("mg/l"),     //unit
                      Value(20),        //min
                      Value(25),        //best
                      Value(30),        //max
                      Value(15),        //mic
                      Unit("mg/l"),     //mic Unit
                      Duration(),       //Not used
                      Duration(),       //Not used
                      Duration());      //Not used

        TargetExtractor extractor;
        TargetEvent targetEvent = extractor.targetEventFromTarget(&target);


        double lastTime = 50;
        TimeOffsets timeOffsets = fillTimeOffsets(lastTime);


        double minConcentration = 20;
        double maxConcentration = 30;
        Concentrations concentrations = fillGrowingConcentrations(minConcentration, maxConcentration);
        double higherValueConcentration = concentrations.at(NB_ARRAY_ELEMENTS - 1);



        ConcentrationPrediction concentrationPrediction;
        concentrationPrediction.appendConcentrations(timeOffsets, concentrations);



        TargetEvaluator targetEvaluator;
        ComputingStatus status = targetEvaluator.evaluate(concentrationPrediction,
                                                          expectedIntakes,
                                                          targetEvent,
                                                          targetEvaluationResult);


        fructose_assert(status == ComputingStatus::Ok);

        fructose_assert(targetEvaluationResult.getUnit() == Unit("ug/l")); //WRONG must be final unit ?
        fructose_assert(targetEvaluationResult.getTargetType() == TargetType::Residual);
        fructose_assert(targetEvaluationResult.getValue() == (higherValueConcentration * 1000)); //*1000 to convert in ug/l
        //To compare score, create another function ?

    }

    void testPeak(const std::string& /* _testName */)
    {

        IntakeSeries expectedIntakes = createIntakeSeries();

        TargetEvaluationResult targetEvaluationResult;

        Target target(ActiveMoietyId("imatinib"),
                      TargetType::Peak,
                      Unit("mg/l"),     //unit
                      Value(5),        //min
                      Value(25),        //best
                      Value(30),        //max
                      Value(15),        //mic
                      Unit("mg/l"),     //mic Unit
                      Duration(std::chrono::minutes(0)),       //min
                      Duration(std::chrono::minutes(60)),       //best
                      Duration(std::chrono::minutes(120)));      //max

        TargetExtractor extractor;
        TargetEvent targetEvent = extractor.targetEventFromTarget(&target);

        TimeOffsets timeOffsets = {0, 1, 2, 3, 4};

        Concentrations concentrations = {24, 29, 16, 9, 20};

        double empiriqueValue = 29.0;

        ConcentrationPrediction concentrationPrediction;
        concentrationPrediction.appendConcentrations(timeOffsets, concentrations);

        TargetEvaluator targetEvaluator;
        ComputingStatus status = targetEvaluator.evaluate(concentrationPrediction,
                                                          expectedIntakes,
                                                          targetEvent,
                                                          targetEvaluationResult);


        fructose_assert(status == ComputingStatus::Ok);

        fructose_assert(targetEvaluationResult.getUnit() == Unit("mg/l"));
        fructose_assert(targetEvaluationResult.getTargetType() == TargetType::Peak);
        fructose_assert(targetEvaluationResult.getValue() == empiriqueValue);

    }

    void testMean(const std::string& /* _testName */)
    {

        IntakeSeries expectedIntakes = createIntakeSeries();

        TargetEvaluationResult targetEvaluationResult;

        Target target(ActiveMoietyId("imatinib"),
                      TargetType::Mean,
                      Unit("mg/l"),     //unit
                      Value(5),        //min
                      Value(25),        //best
                      Value(35),        //max
                      Value(15),        //mic
                      Unit("mg/l"),     //mic Unit
                      Duration(std::chrono::minutes(0)),       //min
                      Duration(std::chrono::minutes(60)),       //best
                      Duration(std::chrono::minutes(120)));      //max

        TargetExtractor extractor;
        TargetEvent targetEvent = extractor.targetEventFromTarget(&target);

        TimeOffsets timeOffsets = {0, 1, 2, 3, 4};

        Concentrations concentrations = {24, 29, 16, 9, 60};

        double empiriqueValue = (24.0 / 2.0 + 29.0 + 16.0 + 9.0 + 60.0 / 2.0) / 4.0;

        ConcentrationPrediction concentrationPrediction;
        concentrationPrediction.appendConcentrations(timeOffsets, concentrations);

        TargetEvaluator targetEvaluator;
        ComputingStatus status = targetEvaluator.evaluate(concentrationPrediction,
                                                          expectedIntakes,
                                                          targetEvent,
                                                          targetEvaluationResult);


        fructose_assert(status == ComputingStatus::Ok);

        fructose_assert(targetEvaluationResult.getUnit() == Unit("mg/l"));
        fructose_assert(targetEvaluationResult.getTargetType() == TargetType::Mean);
        fructose_assert_eq(targetEvaluationResult.getValue() , empiriqueValue);
    }

    void testAuc(const std::string& /* _testName */)
    {
        IntakeSeries expectedIntakes = createIntakeSeries();

        TargetEvaluationResult targetEvaluationResult;

        Target target(ActiveMoietyId("imatinib"),
                      TargetType::Auc,
                      Unit("mg/l"),     //unit
                      Value(5),        //min
                      Value(25),        //best
                      Value(100),        //max
                      Value(15),        //mic
                      Unit("mg/l"),     //mic Unit
                      Duration(std::chrono::minutes(0)),       //min
                      Duration(std::chrono::minutes(60)),       //best
                      Duration(std::chrono::minutes(120)));      //max

        TargetExtractor extractor;
        TargetEvent targetEvent = extractor.targetEventFromTarget(&target);

        TimeOffsets timeOffsets = {0, 1, 2, 3, 4};

        Concentrations concentrations = {17, 18, 19, 18, 17};

        double empiriqueValue = 4 * 17 + ((4 * (19 - 17)) / 2);

        ConcentrationPrediction concentrationPrediction;
        concentrationPrediction.appendConcentrations(timeOffsets, concentrations);

        TargetEvaluator targetEvaluator;
        ComputingStatus status = targetEvaluator.evaluate(concentrationPrediction,
                                                          expectedIntakes,
                                                          targetEvent,
                                                          targetEvaluationResult);


        fructose_assert(status == ComputingStatus::Ok);

        fructose_assert(targetEvaluationResult.getUnit() == Unit("mg/l"));
        fructose_assert(targetEvaluationResult.getTargetType() == TargetType::Auc);
        fructose_assert(targetEvaluationResult.getValue() == empiriqueValue);
    }

    void testAuc24(const std::string& /* _testName */)
    {
        IntakeSeries expectedIntakes = createIntakeSeries();

        TargetEvaluationResult targetEvaluationResult;

        Target target(ActiveMoietyId("imatinib"),
                      TargetType::Auc24,
                      Unit("mg/l"),     //unit
                      Value(400),        //min
                      Value(425),        //best
                      Value(450),        //max
                      Value(15),        //mic
                      Unit("mg/l"),     //mic Unit
                      Duration(std::chrono::minutes(0)),       //min
                      Duration(std::chrono::minutes(60)),       //best
                      Duration(std::chrono::minutes(120)));      //max

        TargetExtractor extractor;
        TargetEvent targetEvent = extractor.targetEventFromTarget(&target);


        TimeOffsets timeOffsets = {0, 1, 2, 3, 4};

        Concentrations concentrations = {17, 18, 19, 18, 17};

        double empiriqueAuc = 4 * 17 + ((4 * (19 - 17)) / 2);
        double empiriqueValue = empiriqueAuc * 24 / 4; // * 24heures / intervale

        ConcentrationPrediction concentrationPrediction;
        concentrationPrediction.appendConcentrations(timeOffsets, concentrations);

        TargetEvaluator targetEvaluator;
        ComputingStatus status = targetEvaluator.evaluate(concentrationPrediction,
                                                          expectedIntakes,
                                                          targetEvent,
                                                          targetEvaluationResult);


        fructose_assert(status == ComputingStatus::Ok);

        fructose_assert(targetEvaluationResult.getUnit() == Unit("mg/l"));
        fructose_assert(targetEvaluationResult.getTargetType() == TargetType::Auc24);
        fructose_assert(targetEvaluationResult.getValue() == empiriqueValue);

    }

    void testCumulativeAuc(const std::string& /* _testName */)
    {
        IntakeSeries expectedIntakes = createIntakeSeries();

        TargetEvaluationResult targetEvaluationResult;

        Target target(ActiveMoietyId("imatinib"),
                      TargetType::CumulativeAuc,
                      Unit("mg/l"),     //unit
                      Value(5),        //min
                      Value(500),        //best
                      Value(1000),        //max
                      Value(15),        //mic
                      Unit("mg/l"),     //mic Unit
                      Duration(std::chrono::minutes(0)),       //min
                      Duration(std::chrono::minutes(60)),       //best
                      Duration(std::chrono::minutes(120)));      //max



        TargetExtractor extractor;
        TargetEvent targetEvent = extractor.targetEventFromTarget(&target);

        TimeOffsets timeOffsets = {0, 1, 2, 3, 4};

        Concentrations concentrations = {17, 18, 19, 18, 17};

        double empiriqueValue = 4 * 17 + ((4 * (19 - 17)) / 2); // Demander Yann

        ConcentrationPrediction concentrationPrediction;
        concentrationPrediction.appendConcentrations(timeOffsets, concentrations);

        TargetEvaluator targetEvaluator;
        ComputingStatus status = targetEvaluator.evaluate(concentrationPrediction,
                                                          expectedIntakes,
                                                          targetEvent,
                                                          targetEvaluationResult);


        fructose_assert(status == ComputingStatus::Ok);

        fructose_assert(targetEvaluationResult.getUnit() == Unit("mg/l"));
        fructose_assert(targetEvaluationResult.getTargetType() == TargetType::CumulativeAuc);
        fructose_assert(targetEvaluationResult.getValue() == empiriqueValue);
    }

    void testAucOverMic(const std::string& /* _testName */)
    {
        IntakeSeries expectedIntakes = createIntakeSeries();

        TargetEvaluationResult targetEvaluationResult;

        Target target(ActiveMoietyId("imatinib"),
                      TargetType::AucOverMic,
                      Unit("mg/l"),     //unit
                      Value(0),        //min
                      Value(25),        //best
                      Value(100),        //max
                      Value(15),        //mic
                      Unit("mg/l"),     //mic Unit
                      Duration(std::chrono::minutes(0)),       //min
                      Duration(std::chrono::minutes(60)),       //best
                      Duration(std::chrono::minutes(120)));      //max

        TargetExtractor extractor;
        TargetEvent targetEvent = extractor.targetEventFromTarget(&target);

        TimeOffsets timeOffsets = {0, 1, 2, 3, 4, 5, 6, 7};

        Concentrations concentrations = {12, 16, 15, 13, 15, 17, 12, 22};

        double empiriqueValue = 4.475;

        ConcentrationPrediction concentrationPrediction;
        concentrationPrediction.appendConcentrations(timeOffsets, concentrations);

        TargetEvaluator targetEvaluator;
        ComputingStatus status = targetEvaluator.evaluate(concentrationPrediction,
                                                          expectedIntakes,
                                                          targetEvent,
                                                          targetEvaluationResult);


        fructose_assert(status == ComputingStatus::Ok);

        fructose_assert(targetEvaluationResult.getUnit() == Unit("mg/l"));
        fructose_assert(targetEvaluationResult.getTargetType() == TargetType::AucOverMic);
        fructose_assert(targetEvaluationResult.getValue() == empiriqueValue);
    }

    void testAuc24OverMic(const std::string& /* _testName */)
    {
        IntakeSeries expectedIntakes = createIntakeSeries();

        TargetEvaluationResult targetEvaluationResult;

        Target target(ActiveMoietyId("imatinib"),
                      TargetType::Auc24OverMic,
                      Unit("mg/l"),     //unit
                      Value(0),        //min
                      Value(15),        //best
                      Value(30),        //max
                      Value(15),        //mic
                      Unit("mg/l"),     //mic Unit
                      Duration(std::chrono::minutes(0)),       //min
                      Duration(std::chrono::minutes(60)),       //best
                      Duration(std::chrono::minutes(120)));      //max

        TargetExtractor extractor;
        TargetEvent targetEvent = extractor.targetEventFromTarget(&target);

        TimeOffsets timeOffsets = {0, 1, 2, 3, 4, 5, 6, 7};

        Concentrations concentrations = {12, 16, 15, 13, 15, 17, 12, 22};

        double empiriqueValue = 4.475 * 24 / (timeOffsets.size() - 1); // divisé par le nombre d'intervales

        ConcentrationPrediction concentrationPrediction;
        concentrationPrediction.appendConcentrations(timeOffsets, concentrations);

        TargetEvaluator targetEvaluator;
        ComputingStatus status = targetEvaluator.evaluate(concentrationPrediction,
                                                          expectedIntakes,
                                                          targetEvent,
                                                          targetEvaluationResult);


        fructose_assert(status == ComputingStatus::Ok);

        fructose_assert(targetEvaluationResult.getUnit() == Unit("mg/l"));
        fructose_assert(targetEvaluationResult.getTargetType() == TargetType::Auc24OverMic);
        fructose_assert(targetEvaluationResult.getValue() == empiriqueValue);

    }

    void testTimeOverMic(const std::string& /* _testName */)
    {
        IntakeSeries expectedIntakes = createIntakeSeries();

        TargetEvaluationResult targetEvaluationResult;

        Target target(ActiveMoietyId("imatinib"),
                      TargetType::TimeOverMic,
                      Unit("h"),     //unit
                      Value(15),        //min
                      Value(25),        //best
                      Value(30),        //max
                      Value(15),        //mic
                      Unit("mg/l"),     //mic Unit
                      Duration(std::chrono::minutes(0)),       //min
                      Duration(std::chrono::minutes(60)),       //best
                      Duration(std::chrono::minutes(120)));      //max

        TargetExtractor extractor;
        TargetEvent targetEvent = extractor.targetEventFromTarget(&target);

        double lastTime = 50;
        TimeOffsets timeOffsets = fillTimeOffsets(lastTime);

        //test1 (oscillation around mic)
        {

            Concentrations concentrations = {12, 17, 13, 18, 12, 17, 15, 16, 15, 10};


            double empiriqueValue = 27.0;


            ConcentrationPrediction concentrationPrediction;
            concentrationPrediction.appendConcentrations(timeOffsets, concentrations);

            TargetEvaluator targetEvaluator;
            ComputingStatus status = targetEvaluator.evaluate(concentrationPrediction,
                                                              expectedIntakes,
                                                              targetEvent,
                                                              targetEvaluationResult);


            fructose_assert(status == ComputingStatus::Ok);

            fructose_assert(targetEvaluationResult.getUnit() == Unit("h"));
            fructose_assert(targetEvaluationResult.getTargetType() == TargetType::TimeOverMic);
            fructose_assert(targetEvaluationResult.getValue() == empiriqueValue);
        }

        //test2 (concentration values increasing)
        {

            Concentrations concentrations = {12, 13, 14, 15, 16, 17, 18, 19, 20, 21};

            double empiriqueValue = 30.0;


            ConcentrationPrediction concentrationPrediction;
            concentrationPrediction.appendConcentrations(timeOffsets, concentrations);


            TargetEvaluator targetEvaluator;
            ComputingStatus status = targetEvaluator.evaluate(concentrationPrediction,
                                                              expectedIntakes,
                                                              targetEvent,
                                                              targetEvaluationResult);


            fructose_assert(status == ComputingStatus::Ok);

            fructose_assert(targetEvaluationResult.getUnit() == Unit("h"));
            fructose_assert(targetEvaluationResult.getTargetType() == TargetType::TimeOverMic);
            fructose_assert(targetEvaluationResult.getValue() == empiriqueValue);
        }

        //test3 (concentration values descreasing)
        {
            Concentrations concentrations = {19, 18, 17, 16, 14, 13, 12, 11, 10, 9};

            double empiriqueValue = 17.5;


            ConcentrationPrediction concentrationPrediction;
            concentrationPrediction.appendConcentrations(timeOffsets, concentrations);


            TargetEvaluator targetEvaluator;
            ComputingStatus status = targetEvaluator.evaluate(concentrationPrediction,
                                                              expectedIntakes,
                                                              targetEvent,
                                                              targetEvaluationResult);

            fructose_assert(status == ComputingStatus::Ok);

            fructose_assert(targetEvaluationResult.getUnit() == Unit("h"));
            fructose_assert(targetEvaluationResult.getTargetType() == TargetType::TimeOverMic);
            fructose_assert(targetEvaluationResult.getValue() == empiriqueValue);
        }

        //test4 (concentration values under mic)
        {
            Target target(ActiveMoietyId("imatinib"),
                          TargetType::TimeOverMic,
                          Unit("h"),     //unit
                          Value(0.0),        //min
                          Value(25),        //best
                          Value(30),        //max
                          Value(15),        //mic
                          Unit("mg/l"),     //mic Unit
                          Duration(std::chrono::minutes(0)),       //min
                          Duration(std::chrono::minutes(60)),       //best
                          Duration(std::chrono::minutes(120)));      //max

            TargetExtractor extractor;
            TargetEvent targetEvent = extractor.targetEventFromTarget(&target);

            Concentrations concentrations = {14, 13, 13, 12, 11, 13, 14, 12, 13, 11};

            double empiriqueValue = 0.0;


            ConcentrationPrediction concentrationPrediction;
            concentrationPrediction.appendConcentrations(timeOffsets, concentrations);


            TargetEvaluator targetEvaluator;
            ComputingStatus status = targetEvaluator.evaluate(concentrationPrediction,
                                                              expectedIntakes,
                                                              targetEvent,
                                                              targetEvaluationResult);

            fructose_assert(status == ComputingStatus::Ok);

            fructose_assert(targetEvaluationResult.getUnit() == Unit("h"));
            fructose_assert(targetEvaluationResult.getTargetType() == TargetType::TimeOverMic);
            fructose_assert(targetEvaluationResult.getValue() == empiriqueValue);
        }

        //test5 (concentration values over mic)
        {
            Target target(ActiveMoietyId("imatinib"),
                          TargetType::TimeOverMic,
                          Unit("h"),     //unit
                          Value(15),        //min
                          Value(25),        //best
                          Value(55),        //max
                          Value(15),        //mic
                          Unit("mg/l"),     //mic Unit
                          Duration(std::chrono::minutes(0)),       //min
                          Duration(std::chrono::minutes(60)),       //best
                          Duration(std::chrono::minutes(120)));      //max

            TargetExtractor extractor;
            TargetEvent targetEvent = extractor.targetEventFromTarget(&target);



            Concentrations concentrations = {16, 19, 21, 17, 16, 18, 20, 17, 18, 22};

            double empiriqueValue = 45.0;


            ConcentrationPrediction concentrationPrediction;
            concentrationPrediction.appendConcentrations(timeOffsets, concentrations);


            TargetEvaluator targetEvaluator;
            ComputingStatus status = targetEvaluator.evaluate(concentrationPrediction,
                                                              expectedIntakes,
                                                              targetEvent,
                                                              targetEvaluationResult);

            fructose_assert(status == ComputingStatus::Ok);

            fructose_assert(targetEvaluationResult.getUnit() == Unit("h"));
            fructose_assert(targetEvaluationResult.getTargetType() == TargetType::TimeOverMic);
            fructose_assert(targetEvaluationResult.getValue() == empiriqueValue);
        }

    }

    void testAucDividedByMic(const std::string& /* _testName */)
    {
        IntakeSeries expectedIntakes = createIntakeSeries();

        TargetEvaluationResult targetEvaluationResult;

        Target target(ActiveMoietyId("imatinib"),
                      TargetType::AucDividedByMic,
                      Unit("mg/l"),     //unit
                      Value(0),        //min
                      Value(25),        //best
                      Value(100),        //max
                      Value(15),        //mic
                      Unit("mg/l"),     //mic Unit
                      Duration(std::chrono::minutes(0)),       //min
                      Duration(std::chrono::minutes(60)),       //best
                      Duration(std::chrono::minutes(120)));      //max

        TargetExtractor extractor;
        TargetEvent targetEvent = extractor.targetEventFromTarget(&target);

        TimeOffsets timeOffsets = {0, 1, 2, 3, 4};

        Concentrations concentrations = {17, 18, 19, 18, 17};

        double empiriqueValue = (4 * 17 + ((4 * (19 - 17)) / 2)) / 15.0; // auc / mic

        ConcentrationPrediction concentrationPrediction;
        concentrationPrediction.appendConcentrations(timeOffsets, concentrations);

        TargetEvaluator targetEvaluator;
        ComputingStatus status = targetEvaluator.evaluate(concentrationPrediction,
                                                          expectedIntakes,
                                                          targetEvent,
                                                          targetEvaluationResult);


        fructose_assert(status == ComputingStatus::Ok);

        fructose_assert(targetEvaluationResult.getUnit() == Unit("mg/l"));
        fructose_assert(targetEvaluationResult.getTargetType() == TargetType::AucDividedByMic);
        fructose_assert(targetEvaluationResult.getValue() == empiriqueValue);

    }

    void testAuc24DividedByMic(const std::string& /* _testName */)
    {
        IntakeSeries expectedIntakes = createIntakeSeries();

        TargetEvaluationResult targetEvaluationResult;

        Target target(ActiveMoietyId("imatinib"),
                      TargetType::Auc24DividedByMic,
                      Unit("mg/l"),     //unit
                      Value(5),        //min
                      Value(25),        //best
                      Value(100),        //max
                      Value(15.0),        //mic
                      Unit("mg/l"),     //mic Unit
                      Duration(std::chrono::minutes(0)),       //min
                      Duration(std::chrono::minutes(60)),       //best
                      Duration(std::chrono::minutes(120)));      //max

        TargetExtractor extractor;
        TargetEvent targetEvent = extractor.targetEventFromTarget(&target);

        TimeOffsets timeOffsets = {0, 1, 2, 3, 4};

        Concentrations concentrations = {17, 18, 19, 18, 17};

        double empiriqueValue = 28.8; // auc / mic * 24heures / intervale

        ConcentrationPrediction concentrationPrediction;
        concentrationPrediction.appendConcentrations(timeOffsets, concentrations);

        TargetEvaluator targetEvaluator;
        ComputingStatus status = targetEvaluator.evaluate(concentrationPrediction,
                                                          expectedIntakes,
                                                          targetEvent,
                                                          targetEvaluationResult);


        fructose_assert(status == ComputingStatus::Ok);

        fructose_assert(targetEvaluationResult.getUnit() == Unit("mg/l"));
        fructose_assert(targetEvaluationResult.getTargetType() == TargetType::Auc24DividedByMic);
        fructose_assert(targetEvaluationResult.getValue() == empiriqueValue);
    }

    void testPeakDividedByMic(const std::string& /* _testName */)
    {
        IntakeSeries expectedIntakes = createIntakeSeries();

        TargetEvaluationResult targetEvaluationResult;

        Target target(ActiveMoietyId("imatinib"),
                      TargetType::PeakDividedByMic,
                      Unit("mg/l"),     //unit
                      Value(0),        //min
                      Value(25),        //best
                      Value(100),        //max
                      Value(15),        //mic
                      Unit("mg/l"),     //mic Unit
                      Duration(std::chrono::minutes(0)),       //min
                      Duration(std::chrono::minutes(60)),       //best
                      Duration(std::chrono::minutes(120)));      //max

        TargetExtractor extractor;
        TargetEvent targetEvent = extractor.targetEventFromTarget(&target);

        TimeOffsets timeOffsets = {0, 1, 2, 3, 4};

        Concentrations concentrations = {17, 18, 19, 18, 17};

        double empiriqueValue = 19 / 15.0; // peak value / mic value

        ConcentrationPrediction concentrationPrediction;
        concentrationPrediction.appendConcentrations(timeOffsets, concentrations);

        TargetEvaluator targetEvaluator;
        ComputingStatus status = targetEvaluator.evaluate(concentrationPrediction,
                                                          expectedIntakes,
                                                          targetEvent,
                                                          targetEvaluationResult);


        fructose_assert(status == ComputingStatus::Ok);

        fructose_assert(targetEvaluationResult.getUnit() == Unit("mg/l"));
        fructose_assert(targetEvaluationResult.getTargetType() == TargetType::PeakDividedByMic);
        fructose_assert(targetEvaluationResult.getValue() == empiriqueValue);
    }


private:

    ///
    /// \brief fillConcentrations is used to fill Concentrations vector (fixed number) with incremental values between boudaries
    /// \param minValue : minimum asked concentration value
    /// \param maxValue : maximum asked concentration value
    /// \return Concentrations (Concentration vector)
    ///
    Concentrations fillGrowingConcentrations(double minValue, double maxValue)
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


    IntakeSeries createIntakeSeries()
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

        return expectedIntakes;
    }

};


#endif // TEST_TARGETEVALUATOR_H
