#ifndef TEST_TARGETEVALUATOR_H
#define TEST_TARGETEVALUATOR_H

#include "tucucommon/datetime.h"
#include "tucucommon/duration.h"
#include "tucucommon/timeofday.h"

#include "tucucore/concentrationprediction.h"
#include "tucucore/definitions.h"
#include "tucucore/drugmodel/formulationandroute.h"
#include "tucucore/drugtreatment/target.h"
#include "tucucore/intakeevent.h"
#include "tucucore/targetevaluationresult.h"
#include "tucucore/targetevaluator.h"
#include "tucucore/targetextractor.h"

#include "fructose/fructose.h"
#include "testutils.h"

using namespace Tucuxi::Core;

struct TestTargetEvaluator : public fructose::test_base<TestTargetEvaluator>
{
    static const int CYCLE_SIZE = 251;

    static constexpr double NB_POINTS_PER_HOUR = CYCLE_SIZE / 24.0;

    static constexpr std::size_t NB_ARRAY_ELEMENTS = 10;

    TestTargetEvaluator() {}

    void testResidual(const std::string& /* _testName */)
    {

        IntakeSeries expectedIntakes = createIntakeSeries();

        TargetEvaluationResult targetEvaluationResult;

        const Target target(
                ActiveMoietyId("imatinib"),
                TargetType::Residual,
                TucuUnit("mg/l"), //unit
                Value(1),         //min
                Value(5),         //best
                Value(10),        //max
                Value(0),         //not used
                TucuUnit("mg/l"), //not used
                Duration(),       //Not used
                Duration(),       //Not used
                Duration());      //Not used

        TargetExtractor extractor;
        TargetEvent targetEvent = extractor.targetEventFromTarget(&target, TucuUnit("ug/l"));


        double lastTime = 50;
        TimeOffsets timeOffsets = fillTimeOffsets(lastTime);


        double minConcentration = 0;
        double maxConcentration = 10 * 1000;
        Concentrations concentrations = fillGrowingConcentrations(minConcentration, maxConcentration);
        double higherValueConcentration = 9;



        ConcentrationPrediction concentrationPrediction;
        concentrationPrediction.appendConcentrations(timeOffsets, concentrations);



        TargetEvaluator targetEvaluator;
        ComputingStatus status =
                targetEvaluator.evaluate(concentrationPrediction, expectedIntakes, targetEvent, targetEvaluationResult);


        fructose_assert(status == ComputingStatus::Ok);

        if (status != ComputingStatus::InvalidCandidate) {
            fructose_assert(targetEvaluationResult.getUnit() == TucuUnit("mg/l"));
            fructose_assert(targetEvaluationResult.getTargetType() == TargetType::Residual);
            fructose_assert(targetEvaluationResult.getValue() == higherValueConcentration);
        }
    }

    void testPeak(const std::string& /* _testName */)
    {

        IntakeSeries expectedIntakes = createIntakeSeries();

        TargetEvaluationResult targetEvaluationResult;

        Target target(
                ActiveMoietyId("imatinib"),
                TargetType::Peak,
                TucuUnit("mg/l"),                     //unit
                Value(5),                             //min
                Value(25),                            //best
                Value(30),                            //max
                Value(0),                             //not used
                TucuUnit("ug/l"),                     //not used
                Duration(std::chrono::minutes(0)),    //min
                Duration(std::chrono::minutes(60)),   //best
                Duration(std::chrono::minutes(120))); //max

        TargetExtractor extractor;
        TargetEvent targetEvent = extractor.targetEventFromTarget(&target, TucuUnit("ug/l"));

        TimeOffsets timeOffsets = {0, 1, 2, 3, 4};

        Concentrations concentrations = {24000, 29000, 16000, 9000, 20000}; // in ug/l


        double expectedValue = 29.0; // in mg/l

        ConcentrationPrediction concentrationPrediction;
        concentrationPrediction.appendConcentrations(timeOffsets, concentrations);

        TargetEvaluator targetEvaluator;
        ComputingStatus status =
                targetEvaluator.evaluate(concentrationPrediction, expectedIntakes, targetEvent, targetEvaluationResult);


        fructose_assert(status == ComputingStatus::Ok);

        if (status != ComputingStatus::InvalidCandidate) {
            fructose_assert(targetEvaluationResult.getUnit() == TucuUnit("mg/l"));
            fructose_assert(targetEvaluationResult.getTargetType() == TargetType::Peak);
            fructose_assert_double_eq(targetEvaluationResult.getValue(), expectedValue);
        }
    }

    void testMean(const std::string& /* _testName */)
    {

        IntakeSeries expectedIntakes = createIntakeSeries();

        TargetEvaluationResult targetEvaluationResult;

        Target target(
                ActiveMoietyId("imatinib"),
                TargetType::Mean,
                TucuUnit("ug/l"),                     //unit
                Value(5),                             //min
                Value(25),                            //best
                Value(35),                            //max
                Value(0),                             //not used
                TucuUnit("ug/l"),                     //not used
                Duration(std::chrono::minutes(0)),    //min
                Duration(std::chrono::minutes(60)),   //best
                Duration(std::chrono::minutes(120))); //max

        TargetExtractor extractor;
        TargetEvent targetEvent = extractor.targetEventFromTarget(&target, TucuUnit("ug/l"));

        TimeOffsets timeOffsets = {0, 1, 2, 3, 4};

        Concentrations concentrations = {24, 29, 16, 9, 60};

        double expectedValue = (24.0 / 2.0 + 29.0 + 16.0 + 9.0 + 60.0 / 2.0) / 4.0;

        ConcentrationPrediction concentrationPrediction;
        concentrationPrediction.appendConcentrations(timeOffsets, concentrations);

        TargetEvaluator targetEvaluator;
        ComputingStatus status =
                targetEvaluator.evaluate(concentrationPrediction, expectedIntakes, targetEvent, targetEvaluationResult);


        fructose_assert(status == ComputingStatus::Ok);

        if (status != ComputingStatus::InvalidCandidate) {
            fructose_assert(targetEvaluationResult.getUnit() == TucuUnit("ug/l"));
            fructose_assert(targetEvaluationResult.getTargetType() == TargetType::Mean);
            fructose_assert_eq(targetEvaluationResult.getValue(), expectedValue);
        }
    }

    void testAuc(const std::string& /* _testName */)
    {
        IntakeSeries expectedIntakes = createIntakeSeries();

        TargetEvaluationResult targetEvaluationResult;

        Target target(
                ActiveMoietyId("imatinib"),
                TargetType::Auc,
                TucuUnit("ug*h/l"),                   //unit
                Value(5),                             //min
                Value(25),                            //best
                Value(100),                           //max
                Value(0),                             // mic not used
                TucuUnit("ug/l"),                     // mic unit not used
                Duration(std::chrono::minutes(0)),    //min
                Duration(std::chrono::minutes(60)),   //best
                Duration(std::chrono::minutes(120))); //max

        TargetExtractor extractor;
        TargetEvent targetEvent = extractor.targetEventFromTarget(&target, TucuUnit("ug/l"));

        TimeOffsets timeOffsets = {0, 1, 2, 3, 4};

        Concentrations concentrations = {17, 18, 19, 18, 17};

        double expectedValue = 4 * 17 + ((4 * (19 - 17)) / 2);

        ConcentrationPrediction concentrationPrediction;
        concentrationPrediction.appendConcentrations(timeOffsets, concentrations);

        TargetEvaluator targetEvaluator;
        ComputingStatus status =
                targetEvaluator.evaluate(concentrationPrediction, expectedIntakes, targetEvent, targetEvaluationResult);


        fructose_assert(status == ComputingStatus::Ok);

        if (status != ComputingStatus::InvalidCandidate) {
            fructose_assert(targetEvaluationResult.getUnit() == TucuUnit("ug*h/l"));
            fructose_assert(targetEvaluationResult.getTargetType() == TargetType::Auc);
            fructose_assert(targetEvaluationResult.getValue() == expectedValue);
        }
    }

    void testAuc24(const std::string& /* _testName */)
    {
        IntakeSeries expectedIntakes = createIntakeSeries();

        TargetEvaluationResult targetEvaluationResult;

        Target target(
                ActiveMoietyId("imatinib"),
                TargetType::Auc24,
                TucuUnit("ug*h/l"),                   //unit
                Value(0),                             //min
                Value(250),                           //best
                Value(500),                           //max
                Value(0),                             //not used
                TucuUnit("ug/l"),                     //not used
                Duration(std::chrono::minutes(0)),    //min
                Duration(std::chrono::minutes(60)),   //best
                Duration(std::chrono::minutes(120))); //max

        TargetExtractor extractor;
        TargetEvent targetEvent = extractor.targetEventFromTarget(&target, TucuUnit("ug/l"));


        TimeOffsets timeOffsets = {0, 1, 2, 3, 4};

        Concentrations concentrations = {17, 18, 19, 18, 17};

        double empiriqueAuc = 4 * 17 + ((4 * (19 - 17)) / 2);
        double expectedValue = empiriqueAuc * 24 / 4; // * 24heures / intervale

        ConcentrationPrediction concentrationPrediction;
        concentrationPrediction.appendConcentrations(timeOffsets, concentrations);

        TargetEvaluator targetEvaluator;
        ComputingStatus status =
                targetEvaluator.evaluate(concentrationPrediction, expectedIntakes, targetEvent, targetEvaluationResult);


        fructose_assert(status == ComputingStatus::Ok);

        if (status != ComputingStatus::InvalidCandidate) {
            fructose_assert(targetEvaluationResult.getUnit() == TucuUnit("ug*h/l"));
            fructose_assert(targetEvaluationResult.getTargetType() == TargetType::Auc24);
            fructose_assert(targetEvaluationResult.getValue() == expectedValue);
        }
    }

    void testCumulativeAuc(const std::string& /* _testName */)
    {
        IntakeSeries expectedIntakes = createIntakeSeries();

        TargetEvaluationResult targetEvaluationResult;

        Target target(
                ActiveMoietyId("imatinib"),
                TargetType::CumulativeAuc,
                TucuUnit("ug*h/l"),                   //unit
                Value(0),                             //min
                Value(50),                            //best
                Value(100),                           //max
                Value(0),                             //not used
                TucuUnit("ug/l"),                     //not used
                Duration(std::chrono::minutes(0)),    //min
                Duration(std::chrono::minutes(60)),   //best
                Duration(std::chrono::minutes(120))); //max



        TargetExtractor extractor;
        TargetEvent targetEvent = extractor.targetEventFromTarget(&target, TucuUnit("ug/l"));

        TimeOffsets timeOffsets = {0, 1, 2, 3, 4};

        Concentrations concentrations = {17, 18, 19, 18, 17};

        double expectedValue = 4 * 17 + ((4 * (19 - 17)) / 2); // Demander Yann

        ConcentrationPrediction concentrationPrediction;
        concentrationPrediction.appendConcentrations(timeOffsets, concentrations);

        TargetEvaluator targetEvaluator;
        ComputingStatus status =
                targetEvaluator.evaluate(concentrationPrediction, expectedIntakes, targetEvent, targetEvaluationResult);


        fructose_assert(status == ComputingStatus::Ok);

        if (status != ComputingStatus::InvalidCandidate) {
            fructose_assert(targetEvaluationResult.getUnit() == TucuUnit("ug*h/l"));
            fructose_assert(targetEvaluationResult.getTargetType() == TargetType::CumulativeAuc);
            fructose_assert(targetEvaluationResult.getValue() == expectedValue);
        }
    }

    void testAucOverMic(const std::string& /* _testName */)
    {
        IntakeSeries expectedIntakes = createIntakeSeries();

        TargetEvaluationResult targetEvaluationResult;

        Target target(
                ActiveMoietyId("imatinib"),
                TargetType::AucOverMic,
                TucuUnit("ug*h/l"),                   //unit
                Value(0),                             //min
                Value(25),                            //best
                Value(100),                           //max
                Value(15),                            //mic
                TucuUnit("ug/l"),                     //mic Unit
                Duration(std::chrono::minutes(0)),    //min
                Duration(std::chrono::minutes(60)),   //best
                Duration(std::chrono::minutes(120))); //max

        TargetExtractor extractor;
        TargetEvent targetEvent = extractor.targetEventFromTarget(&target, TucuUnit("ug/l"));

        TimeOffsets timeOffsets = {0, 1, 2, 3, 4, 5, 6, 7};

        Concentrations concentrations = {12, 16, 15, 13, 15, 17, 12, 22};


        double expectedValue = 4.475;

        ConcentrationPrediction concentrationPrediction;
        concentrationPrediction.appendConcentrations(timeOffsets, concentrations);

        TargetEvaluator targetEvaluator;
        ComputingStatus status =
                targetEvaluator.evaluate(concentrationPrediction, expectedIntakes, targetEvent, targetEvaluationResult);


        fructose_assert(status == ComputingStatus::Ok);

        if (status != ComputingStatus::InvalidCandidate) {
            fructose_assert(targetEvaluationResult.getUnit() == TucuUnit("ug*h/l"));
            fructose_assert(targetEvaluationResult.getTargetType() == TargetType::AucOverMic);
            fructose_assert(targetEvaluationResult.getValue() == expectedValue);
        }
    }

    void testAuc24OverMic(const std::string& /* _testName */)
    {
        IntakeSeries expectedIntakes = createIntakeSeries();

        TargetEvaluationResult targetEvaluationResult;

        Target target(
                ActiveMoietyId("imatinib"),
                TargetType::Auc24OverMic,
                TucuUnit("ug*h/l"),                   //unit
                Value(0),                             //min
                Value(15),                            //best
                Value(30),                            //max
                Value(15),                            //mic
                TucuUnit("ug/l"),                     //mic Unit
                Duration(std::chrono::minutes(0)),    //min
                Duration(std::chrono::minutes(60)),   //best
                Duration(std::chrono::minutes(120))); //max

        TargetExtractor extractor;
        TargetEvent targetEvent = extractor.targetEventFromTarget(&target, TucuUnit("ug/l"));

        TimeOffsets timeOffsets = {0, 1, 2, 3, 4, 5, 6, 7};

        Concentrations concentrations = {12, 16, 15, 13, 15, 17, 12, 22};

        double expectedValue =
                4.475 * 24.0 / (static_cast<double>(timeOffsets.size() - 1)); // divisé par le nombre d'intervales

        ConcentrationPrediction concentrationPrediction;
        concentrationPrediction.appendConcentrations(timeOffsets, concentrations);

        TargetEvaluator targetEvaluator;
        ComputingStatus status =
                targetEvaluator.evaluate(concentrationPrediction, expectedIntakes, targetEvent, targetEvaluationResult);


        fructose_assert(status == ComputingStatus::Ok);

        if (status != ComputingStatus::InvalidCandidate) {
            fructose_assert(targetEvaluationResult.getUnit() == TucuUnit("ug*h/l"));
            fructose_assert(targetEvaluationResult.getTargetType() == TargetType::Auc24OverMic);
            fructose_assert(targetEvaluationResult.getValue() == expectedValue);
        }
    }

    void testTimeOverMic(const std::string& /* _testName */)
    {
        IntakeSeries expectedIntakes = createIntakeSeries();

        TargetEvaluationResult targetEvaluationResult;

        Target target(
                ActiveMoietyId("imatinib"),
                TargetType::TimeOverMic,
                TucuUnit("h"),                        //unit
                Value(15),                            //min
                Value(25),                            //best
                Value(30),                            //max
                Value(15),                            //mic
                TucuUnit("ug/l"),                     //mic Unit
                Duration(std::chrono::minutes(0)),    //min
                Duration(std::chrono::minutes(60)),   //best
                Duration(std::chrono::minutes(120))); //max

        TargetExtractor extractor;
        TargetEvent targetEvent = extractor.targetEventFromTarget(&target, TucuUnit("ug/l"));

        double lastTime = 50;
        TimeOffsets timeOffsets = fillTimeOffsets(lastTime);

        //test1 (oscillation around mic)
        {

            Concentrations concentrations = {12, 17, 13, 18, 12, 17, 15, 16, 15, 10};


            double expectedValue = 27.0;


            ConcentrationPrediction concentrationPrediction;
            concentrationPrediction.appendConcentrations(timeOffsets, concentrations);

            TargetEvaluator targetEvaluator;
            ComputingStatus status = targetEvaluator.evaluate(
                    concentrationPrediction, expectedIntakes, targetEvent, targetEvaluationResult);


            fructose_assert(status == ComputingStatus::Ok);

            if (status != ComputingStatus::InvalidCandidate) {
                fructose_assert(targetEvaluationResult.getUnit() == TucuUnit("h"));
                fructose_assert(targetEvaluationResult.getTargetType() == TargetType::TimeOverMic);
                fructose_assert(targetEvaluationResult.getValue() == expectedValue);
            }
        }

        //test2 (concentration values increasing)
        {

            Concentrations concentrations = {12, 13, 14, 15, 16, 17, 18, 19, 20, 21};

            double expectedValue = 30.0;


            ConcentrationPrediction concentrationPrediction;
            concentrationPrediction.appendConcentrations(timeOffsets, concentrations);


            TargetEvaluator targetEvaluator;
            ComputingStatus status = targetEvaluator.evaluate(
                    concentrationPrediction, expectedIntakes, targetEvent, targetEvaluationResult);


            fructose_assert(status == ComputingStatus::Ok);

            if (status != ComputingStatus::InvalidCandidate) {
                fructose_assert(targetEvaluationResult.getUnit() == TucuUnit("h"));
                fructose_assert(targetEvaluationResult.getTargetType() == TargetType::TimeOverMic);
                fructose_assert(targetEvaluationResult.getValue() == expectedValue);
            }
        }

        //test3 (concentration values descreasing)
        {
            Concentrations concentrations = {19, 18, 17, 16, 14, 13, 12, 11, 10, 9};

            double expectedValue = 17.5;


            ConcentrationPrediction concentrationPrediction;
            concentrationPrediction.appendConcentrations(timeOffsets, concentrations);


            TargetEvaluator targetEvaluator;
            ComputingStatus status = targetEvaluator.evaluate(
                    concentrationPrediction, expectedIntakes, targetEvent, targetEvaluationResult);

            fructose_assert(status == ComputingStatus::Ok);

            if (status != ComputingStatus::InvalidCandidate) {
                fructose_assert(targetEvaluationResult.getUnit() == TucuUnit("h"));
                fructose_assert(targetEvaluationResult.getTargetType() == TargetType::TimeOverMic);
                fructose_assert(targetEvaluationResult.getValue() == expectedValue);
            }
        }

        //test4 (concentration values under mic)
        {
            Target target(
                    ActiveMoietyId("imatinib"),
                    TargetType::TimeOverMic,
                    TucuUnit("h"),                        //unit
                    Value(0),                             //min
                    Value(25),                            //best
                    Value(30),                            //max
                    Value(15),                            //mic
                    TucuUnit("ug/l"),                     //mic Unit
                    Duration(std::chrono::minutes(0)),    //min
                    Duration(std::chrono::minutes(60)),   //best
                    Duration(std::chrono::minutes(120))); //max

            TargetExtractor extractor;
            TargetEvent targetEvent = extractor.targetEventFromTarget(&target, TucuUnit("ug/l"));

            Concentrations concentrations = {14, 13, 13, 12, 11, 13, 14, 12, 13, 11};

            double expectedValue = 0.0;


            ConcentrationPrediction concentrationPrediction;
            concentrationPrediction.appendConcentrations(timeOffsets, concentrations);


            TargetEvaluator targetEvaluator;
            ComputingStatus status = targetEvaluator.evaluate(
                    concentrationPrediction, expectedIntakes, targetEvent, targetEvaluationResult);

            fructose_assert(status == ComputingStatus::Ok);

            if (status != ComputingStatus::InvalidCandidate) {
                fructose_assert(targetEvaluationResult.getUnit() == TucuUnit("h"));
                fructose_assert(targetEvaluationResult.getTargetType() == TargetType::TimeOverMic);
                fructose_assert(targetEvaluationResult.getValue() == expectedValue);
            }
        }

        //test5 (concentration values over mic)
        {
            Target target(
                    ActiveMoietyId("imatinib"),
                    TargetType::TimeOverMic,
                    TucuUnit("h"),                        //unit
                    Value(15),                            //min
                    Value(25),                            //best
                    Value(55),                            //max
                    Value(15),                            //mic
                    TucuUnit("ug/l"),                     //mic Unit
                    Duration(std::chrono::minutes(0)),    //min
                    Duration(std::chrono::minutes(60)),   //best
                    Duration(std::chrono::minutes(120))); //max

            TargetExtractor extractor;
            TargetEvent targetEvent = extractor.targetEventFromTarget(&target, TucuUnit("ug/l"));



            Concentrations concentrations = {16, 19, 21, 17, 16, 18, 20, 17, 18, 22};

            double expectedValue = 45.0;


            ConcentrationPrediction concentrationPrediction;
            concentrationPrediction.appendConcentrations(timeOffsets, concentrations);


            TargetEvaluator targetEvaluator;
            ComputingStatus status = targetEvaluator.evaluate(
                    concentrationPrediction, expectedIntakes, targetEvent, targetEvaluationResult);

            fructose_assert(status == ComputingStatus::Ok);

            if (status != ComputingStatus::InvalidCandidate) {
                fructose_assert(targetEvaluationResult.getUnit() == TucuUnit("h"));
                fructose_assert(targetEvaluationResult.getTargetType() == TargetType::TimeOverMic);
                fructose_assert(targetEvaluationResult.getValue() == expectedValue);
            }
        }
    }

    void testFractionTimeOverMic(const std::string& /* _testName */)
    {
        IntakeSeries expectedIntakes = createIntakeSeries();

        TargetEvaluationResult targetEvaluationResult;

        Target target(
                ActiveMoietyId("imatinib"),
                TargetType::FractionTimeOverMic,
                TucuUnit(""),                         //unit
                Value(0.0),                           //min
                Value(0.5),                           //best
                Value(1.0),                           //max
                Value(15),                            //mic
                TucuUnit("ug/l"),                     //mic Unit
                Duration(std::chrono::minutes(0)),    //min
                Duration(std::chrono::minutes(60)),   //best
                Duration(std::chrono::minutes(120))); //max

        TargetExtractor extractor;
        TargetEvent targetEvent = extractor.targetEventFromTarget(&target, TucuUnit("ug/l"));

        double lastTime = 50;
        TimeOffsets timeOffsets = fillTimeOffsets(lastTime);

        //test1 (oscillation around mic)
        {

            Concentrations concentrations = {12, 17, 13, 18, 12, 17, 15, 16, 15, 10};


            double expectedValue = 27.0 / (50.0 / 10.0 * 9.0);


            ConcentrationPrediction concentrationPrediction;
            concentrationPrediction.appendConcentrations(timeOffsets, concentrations);

            TargetEvaluator targetEvaluator;
            ComputingStatus status = targetEvaluator.evaluate(
                    concentrationPrediction, expectedIntakes, targetEvent, targetEvaluationResult);


            fructose_assert(status == ComputingStatus::Ok);

            if (status != ComputingStatus::InvalidCandidate) {
                fructose_assert(targetEvaluationResult.getUnit() == TucuUnit(""));
                fructose_assert(targetEvaluationResult.getTargetType() == TargetType::FractionTimeOverMic);
                fructose_assert_double_eq(targetEvaluationResult.getValue(), expectedValue);
            }
        }

        //test2 (concentration values increasing)
        {

            Concentrations concentrations = {12, 13, 14, 15, 16, 17, 18, 19, 20, 21};

            double expectedValue = 30.0 / (50.0 / 10.0 * 9.0);


            ConcentrationPrediction concentrationPrediction;
            concentrationPrediction.appendConcentrations(timeOffsets, concentrations);


            TargetEvaluator targetEvaluator;
            ComputingStatus status = targetEvaluator.evaluate(
                    concentrationPrediction, expectedIntakes, targetEvent, targetEvaluationResult);


            fructose_assert(status == ComputingStatus::Ok);

            if (status != ComputingStatus::InvalidCandidate) {
                fructose_assert(targetEvaluationResult.getUnit() == TucuUnit(""));
                fructose_assert(targetEvaluationResult.getTargetType() == TargetType::FractionTimeOverMic);
                fructose_assert_double_eq(targetEvaluationResult.getValue(), expectedValue);
            }
        }

        //test3 (concentration values descreasing)
        {
            Concentrations concentrations = {19, 18, 17, 16, 14, 13, 12, 11, 10, 9};

            double expectedValue = 17.5 / (50.0 / 10.0 * 9.0);


            ConcentrationPrediction concentrationPrediction;
            concentrationPrediction.appendConcentrations(timeOffsets, concentrations);


            TargetEvaluator targetEvaluator;
            ComputingStatus status = targetEvaluator.evaluate(
                    concentrationPrediction, expectedIntakes, targetEvent, targetEvaluationResult);

            fructose_assert(status == ComputingStatus::Ok);

            if (status != ComputingStatus::InvalidCandidate) {
                fructose_assert(targetEvaluationResult.getUnit() == TucuUnit(""));
                fructose_assert(targetEvaluationResult.getTargetType() == TargetType::FractionTimeOverMic);
                fructose_assert_double_eq(targetEvaluationResult.getValue(), expectedValue);
            }
        }

        //test4 (concentration values under mic)
        {
            Target target(
                    ActiveMoietyId("imatinib"),
                    TargetType::FractionTimeOverMic,
                    TucuUnit(""),                         //unit
                    Value(0.0),                           //min
                    Value(0.5),                           //best
                    Value(1.0),                           //max
                    Value(15),                            //mic
                    TucuUnit("ug/l"),                     //mic Unit
                    Duration(std::chrono::minutes(0)),    //min
                    Duration(std::chrono::minutes(60)),   //best
                    Duration(std::chrono::minutes(120))); //max

            TargetExtractor extractor;
            TargetEvent targetEvent = extractor.targetEventFromTarget(&target, TucuUnit("ug/l"));

            Concentrations concentrations = {14, 13, 13, 12, 11, 13, 14, 12, 13, 11};

            double expectedValue = 0.0 / (50.0 / 10.0 * 9.0);


            ConcentrationPrediction concentrationPrediction;
            concentrationPrediction.appendConcentrations(timeOffsets, concentrations);


            TargetEvaluator targetEvaluator;
            ComputingStatus status = targetEvaluator.evaluate(
                    concentrationPrediction, expectedIntakes, targetEvent, targetEvaluationResult);

            fructose_assert(status == ComputingStatus::Ok);

            if (status != ComputingStatus::InvalidCandidate) {
                fructose_assert(targetEvaluationResult.getUnit() == TucuUnit(""));
                fructose_assert(targetEvaluationResult.getTargetType() == TargetType::FractionTimeOverMic);
                fructose_assert_double_eq(targetEvaluationResult.getValue(), expectedValue);
            }
        }

        //test5 (concentration values over mic)
        {
            Target target(
                    ActiveMoietyId("imatinib"),
                    TargetType::FractionTimeOverMic,
                    TucuUnit(""),                         //unit
                    Value(0.0),                           //min
                    Value(0.5),                           //best
                    Value(1.0),                           //max
                    Value(15),                            //mic
                    TucuUnit("ug/l"),                     //mic Unit
                    Duration(std::chrono::minutes(0)),    //min
                    Duration(std::chrono::minutes(60)),   //best
                    Duration(std::chrono::minutes(120))); //max

            TargetExtractor extractor;
            TargetEvent targetEvent = extractor.targetEventFromTarget(&target, TucuUnit("ug/l"));



            Concentrations concentrations = {16, 19, 21, 17, 16, 18, 20, 17, 18, 22};

            double expectedValue = 45.0 / (50.0 / 10.0 * 9.0);


            ConcentrationPrediction concentrationPrediction;
            concentrationPrediction.appendConcentrations(timeOffsets, concentrations);


            TargetEvaluator targetEvaluator;
            ComputingStatus status = targetEvaluator.evaluate(
                    concentrationPrediction, expectedIntakes, targetEvent, targetEvaluationResult);

            fructose_assert(status == ComputingStatus::Ok);

            if (status != ComputingStatus::InvalidCandidate) {
                fructose_assert(targetEvaluationResult.getUnit() == TucuUnit(""));
                fructose_assert(targetEvaluationResult.getTargetType() == TargetType::FractionTimeOverMic);
                fructose_assert_double_eq(targetEvaluationResult.getValue(), expectedValue);
            }
        }
    }

    void testAucDividedByMic(const std::string& /* _testName */)
    {
        IntakeSeries expectedIntakes = createIntakeSeries();

        TargetEvaluationResult targetEvaluationResult;

        Target target(
                ActiveMoietyId("imatinib"),
                TargetType::AucDividedByMic,
                TucuUnit("h"),                        //unit
                Value(0),                             //min
                Value(25),                            //best
                Value(100),                           //max
                Value(15),                            //mic
                TucuUnit("ug/l"),                     //mic Unit
                Duration(std::chrono::minutes(0)),    //min
                Duration(std::chrono::minutes(60)),   //best
                Duration(std::chrono::minutes(120))); //max

        TargetExtractor extractor;
        TargetEvent targetEvent = extractor.targetEventFromTarget(&target, TucuUnit("ug/l"));

        TimeOffsets timeOffsets = {0, 1, 2, 3, 4};

        Concentrations concentrations = {17, 18, 19, 18, 17};

        double expectedValue = (4 * 17 + ((4 * (19 - 17)) / 2)) / 15.0; // auc / mic

        ConcentrationPrediction concentrationPrediction;
        concentrationPrediction.appendConcentrations(timeOffsets, concentrations);

        TargetEvaluator targetEvaluator;
        ComputingStatus status =
                targetEvaluator.evaluate(concentrationPrediction, expectedIntakes, targetEvent, targetEvaluationResult);


        fructose_assert(status == ComputingStatus::Ok);

        if (status != ComputingStatus::InvalidCandidate) {
            fructose_assert(targetEvaluationResult.getUnit() == TucuUnit("h"));
            fructose_assert(targetEvaluationResult.getTargetType() == TargetType::AucDividedByMic);
            fructose_assert(targetEvaluationResult.getValue() == expectedValue);
        }
    }

    void testAuc24DividedByMic(const std::string& /* _testName */)
    {
        IntakeSeries expectedIntakes = createIntakeSeries();

        TargetEvaluationResult targetEvaluationResult;

        Target target(
                ActiveMoietyId("imatinib"),
                TargetType::Auc24DividedByMic,
                TucuUnit("h"),                        //unit
                Value(5),                             //min
                Value(25),                            //best
                Value(100),                           //max
                Value(15),                            //mic
                TucuUnit("ug/l"),                     //mic Unit
                Duration(std::chrono::minutes(0)),    //min
                Duration(std::chrono::minutes(60)),   //best
                Duration(std::chrono::minutes(120))); //max

        TargetExtractor extractor;
        TargetEvent targetEvent = extractor.targetEventFromTarget(&target, TucuUnit("ug/l"));

        TimeOffsets timeOffsets = {0, 1, 2, 3, 4};

        Concentrations concentrations = {17, 18, 19, 18, 17};

        double expectedValue = 28.8; // auc / mic * 24heures / intervale

        ConcentrationPrediction concentrationPrediction;
        concentrationPrediction.appendConcentrations(timeOffsets, concentrations);

        TargetEvaluator targetEvaluator;
        ComputingStatus status =
                targetEvaluator.evaluate(concentrationPrediction, expectedIntakes, targetEvent, targetEvaluationResult);


        fructose_assert(status == ComputingStatus::Ok);

        if (status != ComputingStatus::InvalidCandidate) {
            fructose_assert(targetEvaluationResult.getUnit() == TucuUnit("h"));
            fructose_assert(targetEvaluationResult.getTargetType() == TargetType::Auc24DividedByMic);
            fructose_assert(targetEvaluationResult.getValue() == expectedValue);
        }
    }

    void testPeakDividedByMic(const std::string& /* _testName */)
    {
        IntakeSeries expectedIntakes = createIntakeSeries();

        TargetEvaluationResult targetEvaluationResult;

        Target target(
                ActiveMoietyId("imatinib"),
                TargetType::PeakDividedByMic,
                TucuUnit(""),                         //unit
                Value(0),                             //min
                Value(25),                            //best
                Value(100),                           //max
                Value(15),                            //mic
                TucuUnit("ug/l"),                     //mic Unit
                Duration(std::chrono::minutes(0)),    //min
                Duration(std::chrono::minutes(60)),   //best
                Duration(std::chrono::minutes(120))); //max

        TargetExtractor extractor;
        TargetEvent targetEvent = extractor.targetEventFromTarget(&target, TucuUnit("ug/l"));

        TimeOffsets timeOffsets = {0, 1, 2, 3, 4};

        Concentrations concentrations = {17, 18, 19, 18, 17};

        double expectedValue = 19 / 15.0; // peak value / mic value

        ConcentrationPrediction concentrationPrediction;
        concentrationPrediction.appendConcentrations(timeOffsets, concentrations);

        TargetEvaluator targetEvaluator;
        ComputingStatus status =
                targetEvaluator.evaluate(concentrationPrediction, expectedIntakes, targetEvent, targetEvaluationResult);


        fructose_assert(status == ComputingStatus::Ok);

        if (status != ComputingStatus::InvalidCandidate) {
            fructose_assert(targetEvaluationResult.getUnit() == TucuUnit(""));
            fructose_assert(targetEvaluationResult.getTargetType() == TargetType::PeakDividedByMic);
            fructose_assert(targetEvaluationResult.getValue() == expectedValue);
        }
    }

    void testResidualDividedByMic(const std::string& /* _testName */)
    {
        IntakeSeries expectedIntakes = createIntakeSeries();

        TargetEvaluationResult targetEvaluationResult;

        Target target(
                ActiveMoietyId("imatinib"),
                TargetType::ResidualDividedByMic,
                TucuUnit(""),                         //unit
                Value(0),                             //min
                Value(25),                            //best
                Value(100),                           //max
                Value(15),                            //mic
                TucuUnit("ug/l"),                     //mic Unit
                Duration(std::chrono::minutes(0)),    //min
                Duration(std::chrono::minutes(60)),   //best
                Duration(std::chrono::minutes(120))); //max

        TargetExtractor extractor;
        TargetEvent targetEvent = extractor.targetEventFromTarget(&target, TucuUnit("ug/l"));

        TimeOffsets timeOffsets = {0, 1, 2, 3, 4};

        Concentrations concentrations = {17, 18, 19, 18, 13};

        double expectedValue = 13 / 15.0; // resdidual value / mic value

        ConcentrationPrediction concentrationPrediction;
        concentrationPrediction.appendConcentrations(timeOffsets, concentrations);

        TargetEvaluator targetEvaluator;
        ComputingStatus status =
                targetEvaluator.evaluate(concentrationPrediction, expectedIntakes, targetEvent, targetEvaluationResult);


        fructose_assert(status == ComputingStatus::Ok);

        if (status != ComputingStatus::InvalidCandidate) {
            fructose_assert(targetEvaluationResult.getUnit() == TucuUnit(""));
            fructose_assert(targetEvaluationResult.getTargetType() == TargetType::ResidualDividedByMic);
            fructose_assert(targetEvaluationResult.getValue() == expectedValue);
        }
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

        double concentrationInterval = (maxValue - minValue) / static_cast<double>(NB_ARRAY_ELEMENTS);

        for (double i = minValue; i < maxValue; i += concentrationInterval) {
            concentrations.push_back(i);
            if (concentrations.size() > NB_ARRAY_ELEMENTS) {
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

        double timeInterval = lastTime / static_cast<double>(NB_ARRAY_ELEMENTS);

        for (double i = 0; i < lastTime; i += timeInterval) {
            timeOffsets.push_back(i);
            if (timeOffsets.size() > NB_ARRAY_ELEMENTS) {
                timeOffsets.pop_back();
            }
        }

        return timeOffsets;
    }


    IntakeSeries createIntakeSeries()
    {
        IntakeSeries expectedIntakes;
        expectedIntakes.push_back(IntakeEvent(
                DATE_TIME_NO_VAR(2015, 06, 11, 11, 46, 23),
                Duration(),
                DoseValue(200.0),
                TucuUnit("mg"),
                Duration(std::chrono::hours(7 * 24)),
                FormulationAndRoute(Formulation::Test, AdministrationRoute::IntravenousDrip, AbsorptionModel::Infusion),
                AbsorptionModel::Infusion,
                Duration(std::chrono::minutes(20)),
                static_cast<int>(7 * 24 * NB_POINTS_PER_HOUR + 1)));

        return expectedIntakes;
    }
};


#endif // TEST_TARGETEVALUATOR_H
