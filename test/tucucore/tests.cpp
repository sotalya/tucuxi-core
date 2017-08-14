/*
* Copyright (C) 2017 Tucuxi SA
*/

#include <iostream>
#include <string>

#include "tucucommon/utils.h"
#include "tucucore/dosage.h"

#include "test_dosage.h"
#include "test_intakeextractor.h"
#include "test_operation.h"

int main(int argc, char** argv) 
{
    // Get application folder
    std::string appFolder = Tucuxi::Common::Utils::getAppFolder(argv);

    // --- DOSAGE --- //
    TestDosage dosageTests;
    dosageTests.add_test("SingleDose test", &TestDosage::testSingleDose);
    dosageTests.add_test("LastingDose test", &TestDosage::testLastingDose);
    dosageTests.add_test("DailyDose test", &TestDosage::testDailyDose);
    dosageTests.add_test("WeeklyDose test", &TestDosage::testWeeklyDose);
    dosageTests.add_test("DosageTimeRange test", &TestDosage::testDosageTimeRange);

    int res = dosageTests.run(argc, argv);
    if (res != 0) {
        std::cerr << "Dosage test failed\n";
        exit(1);
    }
    std::cout << "Dosage test succeeded\n";

    // --- INTAKE EXTRACTOR --- //
    TestIntakeExtractor intakeExtractorTests;
    intakeExtractorTests.add_test("OncePerWeek test", &TestIntakeExtractor::testOncePerWeek);
    intakeExtractorTests.add_test("OnceEveryTenDays test", &TestIntakeExtractor::testOnceEveryTenDays);
    intakeExtractorTests.add_test("OnceEvery36Hours test", &TestIntakeExtractor::testOnceEvery36Hours);
    intakeExtractorTests.add_test("FiveTimesEvery12Hours test", &TestIntakeExtractor::testFiveTimesEvery12Hours);
    intakeExtractorTests.add_test("FiveTimesEvery12HoursEarlyStop test", &TestIntakeExtractor::testFiveTimesEvery12HoursEarlyStop);
    intakeExtractorTests.add_test("TwiceEveryTenDays test", &TestIntakeExtractor::testTwiceEveryTenDays);
    intakeExtractorTests.add_test("OnceEveryDay test", &TestIntakeExtractor::testOnceEveryDay);
    intakeExtractorTests.add_test("ThreeTimesEveryDay test", &TestIntakeExtractor::testThreeTimesEveryDay);
    intakeExtractorTests.add_test("ComplexParallelSequence1 test", &TestIntakeExtractor::testComplexParallelSequence1);
    intakeExtractorTests.add_test("ComplexParallelSequence2 test", &TestIntakeExtractor::testComplexParallelSequence2);
    intakeExtractorTests.add_test("FullWeekExceptMonday test", &TestIntakeExtractor::testFullWeekExceptMonday);

    res = intakeExtractorTests.run(argc, argv);
    if (res != 0) {
        std::cerr << "IntakeExtractor test failed\n";
        exit(1);
    }
    std::cout << "IntakeExtractor test succeeded\n";

    // --- OPERATION --- //
    TestOperation operationTests;
    operationTests.add_test("OperationInput test", &TestOperation::testOperationInput);
    operationTests.add_test("HardcodedOperation test", &TestOperation::testHardcodedOperation);
    operationTests.add_test("JSOperation test", &TestOperation::testJSOperation);
    operationTests.add_test("DynamicOperation test", &TestOperation::testDynamicOperation);
    operationTests.add_test("Cockcroft-Gault general equation test", &TestOperation::testCockcroftGaultGeneral);
    operationTests.add_test("Cockcroft-Gault IBW equation test", &TestOperation::testCockcroftGaultIBW);
    operationTests.add_test("Cockcroft-Gault Adjusted IBW equation test", &TestOperation::testCockcroftGaultAdjIBW);

    res = operationTests.run(argc, argv);
    if (res != 0) {
        std::cerr << "Operation test failed\n";
        exit(1);
    }
    std::cout << "Operation test succeeded\n";

    return 0;
}
