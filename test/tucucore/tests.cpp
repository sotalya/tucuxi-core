/*
* Copyright (C) 2017 Tucuxi SA
*/

#include <iostream>
#include <string>

#include "tucucommon/utils.h"
#include "tucucommon/loggerhelper.h"



/// The following strange way of including files is meant to include all of them
/// or only a selection, thanks to DEFINES.
/// If no specific DEFINES is added, then all tests are compiled.
/// If DO_NOT_COMPILE_ALL_TESTS is set, then only the tests selected will be compiled.
/// For instance, to compile only the dosage tests, then run
/// qmake ... DEFINES+=DO_NOT_COMPILE_ALL_TESTS DEFINES+=test_dosage
///
/// The rationale here is to speed up compilation when working on a specific test.
/// As all the tests are in headers, a compilation can be quite time consuming.
/// Therefore, only selecting the test of interest allows to gain some time.


#if defined(test_computingcomponentadjustments) or !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "test_computingcomponentadjustments.h"
#endif
#if defined(test_computingcomponentconcentration) or !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "test_computingcomponentconcentration.h"
#endif
#if defined(test_computingcomponentpercentiles) or !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "test_computingcomponentpercentiles.h"
#endif
#if defined(test_concentrationcalculator) or !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "test_concentrationcalculator.h"
#endif
#if defined(test_covariateextractor) or !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "test_covariateextractor.h"
#endif
#if defined(test_cyclestatistics) or !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "test_cyclestatistics.h"
#endif
#if defined(test_dosage) or !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "test_dosage.h"
#endif
#if defined(test_drugdomainconstraintsevaluator) or !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "test_drugdomainconstraintsevaluator.h"
#endif
#if defined(test_drugmodelimport) or !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "test_drugmodelimport.h"
#endif
#if defined(test_drugmodels) or !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "test_drugmodels.h"
#endif
#if defined(test_intakeextractor) or !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "test_intakeextractor.h"
#endif
#if defined(test_intakeintervalcalculator) or !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "test_intakeintervalcalculator.h"
#endif
#if defined(test_nonmemdrugs) or !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "test_nonmemdrugs.h"
#endif
#if defined(test_operablegraphmanager) or !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "test_operablegraphmanager.h"
#endif
#if defined(test_operation) or !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "test_operation.h"
#endif
#if defined(test_operationcollection) or !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "test_operationcollection.h"
#endif
#if defined(test_parameterextractor) or !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "test_parameterextractor.h"
#endif
#if defined(test_percentilecalculator) or !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "test_percentilecalculator.h"
#endif
#if defined(test_pertinenttimescalculator) or !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "test_pertinenttimescalculator.h"
#endif
#if defined(test_pkmodel) or !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "test_pkmodel.h"
#endif
#if defined(test_residualerrormodel) or !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "test_residualerrormodel.h"
#endif
#if defined(test_sampleextractor) or !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "test_sampleextractor.h"
#endif
#if defined(test_targetextractor) or !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "test_targetextractor.h"
#endif
#if defined(test_constanteliminationbolus) or !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "drugmodels/test_constanteliminationbolus.h"
#endif
#if defined(test_drug_tobramycin) or !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "drugmodels/test_drug_tobramycin.h"
#endif
#if defined(test_drug_vancomycin) or !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "drugmodels/test_drug_vancomycin.h"
#endif

int main(int argc, char** argv)
{
    // Get application folder
    std::string appFolder = Tucuxi::Common::Utils::getAppFolder(argv);

    // Initialize our logger
    const std::string& fileName = Tucuxi::Common::Utils::strFormat("%s/TucuCore-Test.log", appFolder.c_str());
    Tucuxi::Common::LoggerHelper::init(fileName);

    int res = 0;
    int tot_res = 0;

#if defined(test_residualerrormodel) or !defined(DO_NOT_COMPILE_ALL_TESTS)
    TestResidualErrorModel residualErrorModelTests;

    residualErrorModelTests.add_test("testLogLikelihood", &TestResidualErrorModel::testLogLikelihood);
    residualErrorModelTests.add_test("testApplyEpsToValue", &TestResidualErrorModel::testApplyEpsToValue);

    res = residualErrorModelTests.run(argc, argv);
    tot_res |= res;
    if (res != 0) {
        std::cerr << "ResidualErrorModel test failed\n";
    }
    else {
        std::cout << "ResidualErrorModel test succeeded\n";
    }
#endif

#if defined(test_pertinenttimescalculator) or !defined(DO_NOT_COMPILE_ALL_TESTS)
    TestPertinentTimesCalculator pertinentTimesCalculatorTests;

    pertinentTimesCalculatorTests.add_test("testStandard", &TestPertinentTimesCalculator::testStandard);
    pertinentTimesCalculatorTests.add_test("testInfusion", &TestPertinentTimesCalculator::testInfusion);

    res = pertinentTimesCalculatorTests.run(argc, argv);
    tot_res |= res;
    if (res != 0) {
        std::cerr << "Pertinent times calculator test failed\n";
    }
    else {
        std::cout << "Pertinent times calculator test succeeded\n";
    }
#endif

#if defined(test_drugdomainconstraintsevaluator) or !defined(DO_NOT_COMPILE_ALL_TESTS)
    TestDrugDomainConstraintsEvaluator drugDomainConstraintsEvaluatorTests;

    drugDomainConstraintsEvaluatorTests.add_test("simpleTest1", &TestDrugDomainConstraintsEvaluator::test1);
    drugDomainConstraintsEvaluatorTests.add_test("simpleTest2", &TestDrugDomainConstraintsEvaluator::test2);
    drugDomainConstraintsEvaluatorTests.add_test("simpleTest3", &TestDrugDomainConstraintsEvaluator::test3);

    res = drugDomainConstraintsEvaluatorTests.run(argc, argv);
    tot_res |= res;
    if (res != 0) {
        std::cerr << "Drug model domain constraints evaluator test failed\n";
    }
    else {
        std::cout << "Drug model domain constraints evaluator test succeeded\n";
    }
#endif

#if defined(test_drugmodelimport) or !defined(DO_NOT_COMPILE_ALL_TESTS)
    TestDrugModelImport drugModelImportTests;
    drugModelImportTests.add_test("DrugModelImportFake", &TestDrugModelImport::testFake);
    drugModelImportTests.add_test("DrugModelImportBusulfan1", &TestDrugModelImport::testBusulfan1);

    res = drugModelImportTests.run(argc, argv);
    tot_res |= res;
    if (res != 0) {
        std::cerr << "Drug model import test failed\n";
    }
    else {
        std::cout << "Drug model import test succeeded\n";
    }
#endif

#if defined(test_computingcomponentconcentration) or !defined(DO_NOT_COMPILE_ALL_TESTS)
    TestComputingComponentConcentration computingComponentConcentrationTests;

    // one compartment
    computingComponentConcentrationTests.add_test("Simple1", &TestComputingComponentConcentration::testImatinib1);
    computingComponentConcentrationTests.add_test("ImatinibSteadyState", &TestComputingComponentConcentration::testImatinibSteadyState);



    res = computingComponentConcentrationTests.run(argc, argv);
    tot_res |= res;
    if (res != 0) {
        std::cerr << "Computing Component Concentration test failed\n";
    }
    else {
        std::cout << "Computing Component Concentration test succeeded\n";
    }
#endif

#if defined(test_computingcomponentadjustments) or !defined(DO_NOT_COMPILE_ALL_TESTS)
    TestComputingComponentAdjusements computingComponentAdjusementsTests;

    // one compartment
    computingComponentAdjusementsTests.add_test("Simple test 1", &TestComputingComponentAdjusements::testImatinibLastFormulationAndRouteAllDosages);
    computingComponentAdjusementsTests.add_test("Simple test 2", &TestComputingComponentAdjusements::testImatinibDefaultFormulationAndRouteAllDosages);
    computingComponentAdjusementsTests.add_test("Simple test 3", &TestComputingComponentAdjusements::testImatinibAllFormulationAndRouteAllDosages);
    computingComponentAdjusementsTests.add_test("Simple test 4", &TestComputingComponentAdjusements::testImatinibLastFormulationAndRouteBestDosage);
    computingComponentAdjusementsTests.add_test("Simple test 5", &TestComputingComponentAdjusements::testImatinibDefaultFormulationAndRouteBestDosage);
    computingComponentAdjusementsTests.add_test("Simple test 6", &TestComputingComponentAdjusements::testImatinibAllFormulationAndRouteBestDosage);
    computingComponentAdjusementsTests.add_test("Simple test 7", &TestComputingComponentAdjusements::testImatinibEmptyTreatmentDefaultFormulationAndRouteAllDosages);
    computingComponentAdjusementsTests.add_test("Simple test 8", &TestComputingComponentAdjusements::testImatinibSteadyStateLastFormulationAndRouteAllDosages);
    computingComponentAdjusementsTests.add_test("Simple test 9", &TestComputingComponentAdjusements::testImatinibLastFormulationAndRouteAllDosagesAtSteadyState);

    res = computingComponentAdjusementsTests.run(argc, argv);
    tot_res |= res;
    if (res != 0) {
        std::cerr << "Computing Component Adjustments test failed\n";
    }
    else {
        std::cout << "Computing Component Adjustments test succeeded\n";
    }
#endif

#if defined(test_intakeintervalcalculator) or !defined(DO_NOT_COMPILE_ALL_TESTS)
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
    tot_res |= res;
    if (res != 0) {
        std::cerr << "Calculators test failed\n";
    }
    std::cout << "Calculators test succeeded\n";
#endif

#if defined(test_concentrationcalculator) or !defined(DO_NOT_COMPILE_ALL_TESTS)
    // --- ConcentrationCalculator --- //
    TestConcentrationCalculator concentrationCalculatorTests;
    concentrationCalculatorTests.add_test("linear bolus test", &TestConcentrationCalculator::testConstantEliminationBolus);
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
    tot_res |= res;
    if (res != 0) {
        std::cerr << "ConcentrationCalculator test failed\n";
    }
    std::cout << "ConcentrationCalculator test succeeded\n";
#endif

#if defined(test_dosage) or !defined(DO_NOT_COMPILE_ALL_TESTS)
    // --- DOSAGE --- //
    TestDosage dosageTests;
    dosageTests.add_test("SingleDose test", &TestDosage::testSingleDose);
    dosageTests.add_test("LastingDose test", &TestDosage::testLastingDose);
    dosageTests.add_test("DailyDose test", &TestDosage::testDailyDose);
    dosageTests.add_test("WeeklyDose test", &TestDosage::testWeeklyDose);
    dosageTests.add_test("DosageTimeRange test", &TestDosage::testDosageTimeRange);

    res = dosageTests.run(argc, argv);
    tot_res |= res;
    if (res != 0) {
        std::cerr << "Dosage test failed\n";
    } else {
        std::cout << "Dosage test succeeded\n";
    }
#endif

#if defined(test_intakeextractor) or !defined(DO_NOT_COMPILE_ALL_TESTS)
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
    intakeExtractorTests.add_test("OnceEvery36HoursAtSteadyState test", &TestIntakeExtractor::testOnceEvery36HoursAtSteadyState);

    res = intakeExtractorTests.run(argc, argv);
    tot_res |= res;
    if (res != 0) {
        std::cerr << "IntakeExtractor test failed\n";
    } else {
        std::cout << "IntakeExtractor test succeeded\n";
    }
#endif

#if defined(test_operation) or !defined(DO_NOT_COMPILE_ALL_TESTS)
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
    tot_res |= res;
    if (res != 0) {
        std::cerr << "Operation test failed\n";
    } else {
        std::cout << "Operation test succeeded\n";
    }
#endif

#if defined(test_operationcollection) or !defined(DO_NOT_COMPILE_ALL_TESTS)
    // --- OPERATIONCOLLECTION --- //
    TestOperationCollection operationCollectionTests;
    operationCollectionTests.add_test("OperationCollection test", &TestOperationCollection::testOperationCollection);

    res = operationCollectionTests.run(argc, argv);
    tot_res |= res;
    if (res != 0) {
        std::cerr << "OperationCollection test failed\n";
    } else {
        std::cout << "OperationCollection test succeeded\n";
    }

    // --- PkModel --- //
    TestPkModel pkmodelTest;
    pkmodelTest.add_test("testPkModelFunctions test", &TestPkModel::testPkModelFunctions);

    res = pkmodelTest.run(argc, argv);
    tot_res |= res;
    if (res != 0) {
        std::cerr << "PkModel test failed\n";
    } else {
        std::cout << "PkModel test succeeded\n";
    }
#endif

#if defined(test_covariateextractor) or !defined(DO_NOT_COMPILE_ALL_TESTS)
    // --- CovariateExtractor --- //
    TestCovariateExtractor ceTests;
    ceTests.add_test("testCE_constructor", &TestCovariateExtractor::testCE_constructor);
    ceTests.add_test("testCE_collectRefreshIntervals", &TestCovariateExtractor::testCE_collectRefreshIntervals);
    ceTests.add_test("testCE_interpolateValues", &TestCovariateExtractor::testCE_interpolateValues);
    ceTests.add_test("testCE_sortPatientVariates", &TestCovariateExtractor::testCE_sortPatientVariates);
    ceTests.add_test("testOperableFunctions_1_0", &TestCovariateExtractor::testCovariateExtraction_test1_0);
    ceTests.add_test("testOperableFunctions_2_0", &TestCovariateExtractor::testCovariateExtraction_test2_0);
    ceTests.add_test("testOperableFunctions_2_1", &TestCovariateExtractor::testCovariateExtraction_test2_1);
    ceTests.add_test("testOperableFunctions_2_2", &TestCovariateExtractor::testCovariateExtraction_test2_2);
    ceTests.add_test("testOperableFunctions_2_3", &TestCovariateExtractor::testCovariateExtraction_test2_3);
    ceTests.add_test("testOperableFunctions_3_0", &TestCovariateExtractor::testCovariateExtraction_test3_0);
    ceTests.add_test("testOperableFunctions_3_1", &TestCovariateExtractor::testCovariateExtraction_test3_1);
    ceTests.add_test("testOperableFunctions_3_2", &TestCovariateExtractor::testCovariateExtraction_test3_2);

    res = ceTests.run(argc, argv);
    tot_res |= res;

    if (res != 0) {
        std::cerr << "Covariate Extractor test failed\n";
    } else {
        std::cout << "Covariate Extractor test succeeded\n";
    }
#endif

#if defined(test_parameterextractor) or !defined(DO_NOT_COMPILE_ALL_TESTS)
    // --- ParameterExtractor --- //
    TestParameterExtractor peTests;
    peTests.add_test("testPE_constructor", &TestParameterExtractor::testPE_constructor);
    peTests.add_test("testPE_extract1_0", &TestParameterExtractor::testPE_extract1_0);
    peTests.add_test("testPE_extract1_1", &TestParameterExtractor::testPE_extract1_1);
    peTests.add_test("testPE_extractParamFromParam", &TestParameterExtractor::testPE_extractParamFromParam);

    res = peTests.run(argc, argv);
    tot_res |= res;

    if (res != 0) {
        std::cerr << "Parameter Extractor test failed\n";
    } else {
        std::cout << "Parameter Extractor test succeeded\n";
    }
#endif

#if defined(test_operablegraphmanager) or !defined(DO_NOT_COMPILE_ALL_TESTS)
    // --- OperableGraphManager --- //
    TestOpGraph opGraphTests;
    opGraphTests.add_test("testOperableFunctions", &TestOpGraph::testOperableFunctions);
    opGraphTests.add_test("testOperableCockcroftGaultIBW", &TestOpGraph::testOperableCockcroftGaultIBW);
    opGraphTests.add_test("testOperableCyclic", &TestOpGraph::testOperableCyclic);

    res = opGraphTests.run(argc, argv);
    tot_res |= res;

    if (res != 0) {
        std::cerr << "Operable Graph Manager test failed\n";
    } else {
        std::cout << "Operable Graph Manager test succeeded\n";
    }
#endif

#if defined(test_percentilecalculator) or !defined(DO_NOT_COMPILE_ALL_TESTS)
    // --- percentile calculator --- //
    TestPercentileCalculator percentileCalculatorTests;

    percentileCalculatorTests.add_test("Apriori Monte Carlo Percentile", &TestPercentileCalculator::testApriori);
    percentileCalculatorTests.add_test("Aposteriori Normal Monte Carlo Percentile", &TestPercentileCalculator::testAposterioriNormal);
    percentileCalculatorTests.add_test("Aposteriori Matrix cache", &TestPercentileCalculator::testAposterioriMatrixCache);
    percentileCalculatorTests.add_test("Aposteriori Monte Carlo Percentile", &TestPercentileCalculator::testAposteriori);

    res = percentileCalculatorTests.run(argc, argv);
    tot_res |= res;
    if (res != 0) {
        std::cerr << "Percentile Calculator test failed\n";
    }
#endif

#if defined(test_nonmemdrugs) or !defined(DO_NOT_COMPILE_ALL_TESTS)
    // --- NonMemDrugs --- //
    TestNonMemDrugs nonMemDrugsTests;

    // one compartment
    nonMemDrugsTests.add_test("testImatinib", &TestNonMemDrugs::testImatinib);

    res = nonMemDrugsTests.run(argc, argv);
    tot_res |= res;
    if (res != 0) {
        std::cerr << "NonMem Drugs test failed\n";
    }
    std::cout << "NonMem Drugs test succeeded\n";
#endif

#if defined(test_drugmodels) or !defined(DO_NOT_COMPILE_ALL_TESTS)
    TestDrugModels drugModelsTests;

    // one compartment
    drugModelsTests.add_test("testDrugModelImatinib", &TestDrugModels::testImatinib);

    res = drugModelsTests.run(argc, argv);
    tot_res |= res;
    if (res != 0) {
        std::cerr << "DrugModels test failed\n";
    }
    std::cout << "DrugModels test succeeded\n";
#endif

#if defined(test_cyclestatistics) or !defined(DO_NOT_COMPILE_ALL_TESTS)
    // --- cycle statistics --- //
    TestCycleStatistics cycleStatisticsTests;

    // one compartment
    cycleStatisticsTests.add_test("test1CycleStatistics", &TestCycleStatistics::test1CycleStatistics);

    res = cycleStatisticsTests.run(argc, argv);
    if (res != 0) {
        std::cerr << "Cycle Statistics Calculators test failed\n";
        tot_res |= res;
    }
    std::cout << "Cycle Statistics Calculators test succeeded\n";
#endif

#if defined(test_computingcomponentpercentiles) or !defined(DO_NOT_COMPILE_ALL_TESTS)
    TestComputingComponentPercentiles computingComponentPercentilesTests;

    // one compartment
    computingComponentPercentilesTests.add_test("ImatinibPercentiles", &TestComputingComponentPercentiles::testImatinib1);
    computingComponentPercentilesTests.add_test("ImatinibSteadyStatePercentiles", &TestComputingComponentPercentiles::testImatinibSteadyState);



    res = computingComponentPercentilesTests.run(argc, argv);
    tot_res |= res;
    if (res != 0) {
        std::cerr << "Computing Component Percentiles test failed\n";
    }
    else {
        std::cout << "Computing Component Percentiles test succeeded\n";
    }
#endif

#if defined(test_sampleextractor) or !defined(DO_NOT_COMPILE_ALL_TESTS)
    // --- SampleExtractor tests --- //
    TestSampleExtractor sampleExtractorTests;

    // one compartment
    sampleExtractorTests.add_test("testStandard", &TestSampleExtractor::testStandard);

    res = sampleExtractorTests.run(argc, argv);
    tot_res |= res;
    if (res != 0) {
        std::cerr << "SampleExtractor test failed\n";
    }
    else {
        std::cout << "SampleExtractor test succeeded\n";
    }
#endif

#if defined(test_targetextractor) or !defined(DO_NOT_COMPILE_ALL_TESTS)
    // --- TargetExtractor tests --- //
    TestTargetExtractor targetExtractorTests;

    // one compartment
    targetExtractorTests.add_test("testTobramycin", &TestTargetExtractor::testTargetExtractor);

    res = targetExtractorTests.run(argc, argv);
    tot_res |= res;
    if (res != 0) {
        std::cerr << "TargetExtractor test failed\n";
    }
    else {
        std::cout << "TargetExtractor test succeeded\n";
    }
#endif

#if defined(test_drug_tobramycin) or !defined(DO_NOT_COMPILE_ALL_TESTS)
    // --- Tobramycin drug tests --- //
    TestDrugTobramycin tobramycinTests;

    // one compartment
    tobramycinTests.add_test("testTobramycin", &TestDrugTobramycin::testTobramycin);

    res = tobramycinTests.run(argc, argv);
    tot_res |= res;
    if (res != 0) {
        std::cerr << "Drug Tobramycin test failed\n";
    }
    else {
        std::cout << "Drug Tobramycin test succeeded\n";
    }
#endif

#if defined(test_drug_vancomycin) or !defined(DO_NOT_COMPILE_ALL_TESTS)
    // --- Tobramycin drug tests --- //
    TestDrugVancomycin vancomycinTests;

    // one compartment
    vancomycinTests.add_test("testTobramycin", &TestDrugVancomycin::testVancomycin);

    res = vancomycinTests.run(argc, argv);
    tot_res |= res;
    if (res != 0) {
        std::cerr << "Drug Vancomycin test failed\n";
    }
    else {
        std::cout << "Drug Vancomycin test succeeded\n";
    }
#endif

#if defined(test_constanteliminationbolus) or !defined(DO_NOT_COMPILE_ALL_TESTS)
    // --- Tobramycin drug tests --- //
    TestConstantEliminationBolus constantEliminationBolusTests;

    // one compartment
    constantEliminationBolusTests.add_test("testConstantEliminationBolus", &TestConstantEliminationBolus::testConstantElimination);

    res = constantEliminationBolusTests.run(argc, argv);
    tot_res |= res;
    if (res != 0) {
        std::cerr << "Constant Elimination Bolus test failed\n";
    }
    else {
        std::cout << "Constant Elimination Bolus test succeeded\n";
    }
#endif


    // Delete the logger to avoid a warning when using valgrind --leak-check=full
    Tucuxi::Common::LoggerHelper::beforeExit();

    return tot_res;
}
