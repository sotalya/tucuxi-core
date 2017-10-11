/*
* Copyright (C) 2017 Tucuxi SA
*/

#include <iostream>
#include <string>

#include "tucucommon/utils.h"
#include "tucucommon/loggerhelper.h"
#include "tucucore/dosage.h"

#include "test_covariateextractor.h"
#include "test_dosage.h"
#include "test_intakeextractor.h"
#include "test_operation.h"
#include "test_operablegraphmanager.h"
#include "test_intakeintervalcalculator.h"
#include "test_concentrationcalculator.h"
#include "test_pkmodel.h"
#include "test_percentilecalculator.h"
#include "test_nonmemdrugs.h"
#include "test_drugmodels.h"

int main(int argc, char** argv)
{
    // Get application folder
    std::string appFolder = Tucuxi::Common::Utils::getAppFolder(argv);

    // Initialize our logger
    const std::string& fileName = Tucuxi::Common::Utils::strFormat("%s/TucuCore-Test.log", appFolder.c_str());
    Tucuxi::Common::LoggerHelper::init(fileName);

    int res = 0;

    TestIntervalCalculator calculatorsTests;

    // one compartment
    calculatorsTests.add_test("1 comp bolus single vs multiple test", &TestIntervalCalculator::test1compBolusSingleVsMultiple);
    calculatorsTests.add_test("1 comp extra single vs multiple test", &TestIntervalCalculator::test1compExtraSingleVsMultiple);
    calculatorsTests.add_test("1 comp infusion single vs multiple test", &TestIntervalCalculator::test1compInfusionSingleVsMultiple);

    // two compartment
    calculatorsTests.add_test("2 comp bolus single vs multiple test", &TestIntervalCalculator::test2compBolusSingleVsMultiple);
    calculatorsTests.add_test("2 comp extra single vs multiple test", &TestIntervalCalculator::test2compExtraSingleVsMultiple);
    calculatorsTests.add_test("2 comp infusion single vs multiple test", &TestIntervalCalculator::test2compInfusionSingleVsMultiple);

    // three compartment
    // TODO Active following test after fixing input parameters
#if 0
    calculatorsTests.add_test("3 comp bolus single vs multiple test", &TestIntervalCalculator::test3compBolusSingleVsMultiple);
    calculatorsTests.add_test("3 comp extra single vs multiple test", &TestIntervalCalculator::test3compExtraSingleVsMultiple);
    calculatorsTests.add_test("3 comp infusion single vs multiple test", &TestIntervalCalculator::test3compInfusionSingleVsMultiple);
#endif

    res = calculatorsTests.run(argc, argv);
    if (res != 0) {
        std::cerr << "Calculators test failed\n";
        exit(1);
    }
    std::cout << "Calculators test succeeded\n";

    // --- ConcentrationCalculator --- //
    TestConcentrationCalculator concentrationCalculatorTests;
    concentrationCalculatorTests.add_test("1 comp bolus test", &TestConcentrationCalculator::test1compBolus);
    concentrationCalculatorTests.add_test("1 comp extra test", &TestConcentrationCalculator::test1compExtra);
    concentrationCalculatorTests.add_test("1 comp infusion test", &TestConcentrationCalculator::test1compInfusion);
    concentrationCalculatorTests.add_test("2 comp bolus test", &TestConcentrationCalculator::test2compBolus);
    concentrationCalculatorTests.add_test("2 comp extra test", &TestConcentrationCalculator::test2compExtra);
    concentrationCalculatorTests.add_test("2 comp infusion test", &TestConcentrationCalculator::test2compInfusion);
    // TODO Active following test after fixing input parameters
#if 0
    concentrationCalculatorTests.add_test("3 comp bolus test", &TestConcentrationCalculator::test3compBolus);
    concentrationCalculatorTests.add_test("3 comp extra test", &TestConcentrationCalculator::test3compExtra);
    concentrationCalculatorTests.add_test("3 comp infusion test", &TestConcentrationCalculator::test3compInfusion);
#endif

    res = concentrationCalculatorTests.run(argc, argv);
    if (res != 0) {
        std::cerr << "ConcentrationCalculator test failed\n";
        exit(1);
    }
    std::cout << "ConcentrationCalculator test succeeded\n";

    // --- DOSAGE --- //
    TestDosage dosageTests;
    dosageTests.add_test("SingleDose test", &TestDosage::testSingleDose);
    dosageTests.add_test("LastingDose test", &TestDosage::testLastingDose);
    dosageTests.add_test("DailyDose test", &TestDosage::testDailyDose);
    dosageTests.add_test("WeeklyDose test", &TestDosage::testWeeklyDose);
    dosageTests.add_test("DosageTimeRange test", &TestDosage::testDosageTimeRange);

    res = dosageTests.run(argc, argv);
    if (res != 0) {
        std::cerr << "Dosage test failed\n";
    } else {
        std::cout << "Dosage test succeeded\n";
    }

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
    } else {
        std::cout << "IntakeExtractor test succeeded\n";
    }

    // --- OPERATION --- //
    TestOperation operationTests;
    operationTests.add_test("OperationInput test", &TestOperation::testOperationInput);
    operationTests.add_test("HardcodedOperation test", &TestOperation::testHardcodedOperation);
    operationTests.add_test("JSOperation test", &TestOperation::testJSOperation);
    operationTests.add_test("DynamicOperation test", &TestOperation::testDynamicOperation);
    operationTests.add_test("Cockcroft-Gault general equation test", &TestOperation::testCockcroftGaultGeneral);
    operationTests.add_test("Cockcroft-Gault IBW equation test", &TestOperation::testCockcroftGaultIBW);
    operationTests.add_test("Cockcroft-Gault Adjusted IBW equation test", &TestOperation::testCockcroftGaultAdjIBW);
    operationTests.add_test("MDRD test", &TestOperation::testMDRD);
    operationTests.add_test("CKD-EPI test", &TestOperation::testCKD_EPI);
    operationTests.add_test("Schwartz test", &TestOperation::testSchwartz);
    operationTests.add_test("Jelliffe test", &TestOperation::testJelliffe);
    operationTests.add_test("Salazar-Corcoran test", &TestOperation::testSalazarCorcoran);

    res = operationTests.run(argc, argv);
    if (res != 0) {
        std::cerr << "Operation test failed\n";
    } else {
        std::cout << "Operation test succeeded\n";
    }

    // --- PkModel --- //
    TestPkModel pkmodelTest;
    pkmodelTest.add_test("testPkModelFunctions test", &TestPkModel::testPkModelFunctions);

    res = pkmodelTest.run(argc, argv);
    if (res != 0) {
        std::cerr << "PkModel test failed\n";
    } else {
        std::cout << "PkModel test succeeded\n";
    }

    // --- CovariateExtractor --- //
    TestCovariateExtractor ceTests;
    /*ceTests.add_test("testCE_constructor", &TestCovariateExtractor::testCE_constructor);
    ceTests.add_test("testCE_collectRefreshIntervals", &TestCovariateExtractor::testCE_collectRefreshIntervals);
    ceTests.add_test("testCE_createComputedCEvents", &TestCovariateExtractor::testCE_createComputedCEvents);
    ceTests.add_test("testCE_createNonComputedCEvents", &TestCovariateExtractor::testCE_createNonComputedCEvents);
    ceTests.add_test("testCE_interpolateValues", &TestCovariateExtractor::testCE_interpolateValues);
    ceTests.add_test("testCE_sortPatientVariates", &TestCovariateExtractor::testCE_sortPatientVariates);
    ceTests.add_test("testOperableFunctions_1", &TestCovariateExtractor::testCovariateExtraction_test1);
    ceTests.add_test("testOperableFunctions_2", &TestCovariateExtractor::testCovariateExtraction_test2);*/

    res = ceTests.run(argc, argv);

    if (res != 0) {
        std::cerr << "Covariate Extractor test failed\n";
    } else {
        std::cout << "Covariate Extractor test succeeded\n";
    }

    // --- OperableGraphManager --- //
    TestOpGraph opGraphTests;
    opGraphTests.add_test("testOperableFunctions", &TestOpGraph::testOperableFunctions);
    opGraphTests.add_test("testOperableCockcroftGaultIBW", &TestOpGraph::testOperableCockcroftGaultIBW);
    opGraphTests.add_test("testOperableCyclic", &TestOpGraph::testOperableCyclic);

    res = opGraphTests.run(argc, argv);

    if (res != 0) {
        std::cerr << "Operable Graph Manager test failed\n";
    } else {
        std::cout << "Operable Graph Manager test succeeded\n";
    }

    TestPercentileCalculator percentileCalculatorTests;

    // one compartment
    percentileCalculatorTests.add_test("test1", &TestPercentileCalculator::test1);

    res = percentileCalculatorTests.run(argc, argv);
    if (res != 0) {
        std::cerr << "Percentile Calculators test failed\n";
        exit(1);
    }
    std::cout << "Percentile Calculators test succeeded\n";




    TestNonMemDrugs nonMemDrugsTests;

    // one compartment
    nonMemDrugsTests.add_test("testImatinib", &TestNonMemDrugs::testImatinib);

    res = nonMemDrugsTests.run(argc, argv);
    if (res != 0) {
        std::cerr << "NonMem Drugs test failed\n";
        exit(1);
    }
    std::cout << "NonMem Drugs test succeeded\n";



    TestDrugModels drugModelsTests;

    // one compartment
    drugModelsTests.add_test("testDrugModelImatinib", &TestDrugModels::testImatinib);

    res = drugModelsTests.run(argc, argv);
    if (res != 0) {
        std::cerr << "DrugModels test failed\n";
        exit(1);
    }
    std::cout << "DrugModels test succeeded\n";

    return 0;
}
