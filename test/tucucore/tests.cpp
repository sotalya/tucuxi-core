//@@license@@

#include <iostream>
#include <string>

#include "tucucommon/loggerhelper.h"
#include "tucucommon/utils.h"


/// The following strange way of including files is meant to include all of them
/// || only a selection, thanks to DEFINES.
/// If no specific DEFINES is added, then all tests are compiled.
/// If DO_NOT_COMPILE_ALL_TESTS is set, then only the tests selected will be compiled.
/// For instance, to compile only the dosage tests, then run
/// qmake ... DEFINES+=DO_NOT_COMPILE_ALL_TESTS DEFINES+=test_dosage
///
/// The rationale here is to speed up compilation when working on a specific test.
/// As all the tests are in headers, a compilation can be quite time consuming.
/// Therefore, only selecting the test of interest allows to gain some time.



namespace Tucuxi {
namespace Core {

template<typename T>
std::ostream& operator<<(typename std::enable_if<std::is_enum<T>::value, std::ostream>::type& stream, const T& e)
{
    return stream << static_cast<typename std::underlying_type<T>::type>(e);
}

} // namespace Core
} // namespace Tucuxi


#if defined(test_multiconstanteliminationbolus) || !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "test_multiconstanteliminationbolus.h"
#endif
#if defined(test_likelihood) || !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "test_likelihood.h"
#endif
#if defined(test_parameter) || !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "test_parameter.h"
#endif
#if defined(test_multianalytessingleactivemoiety) || !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "drugmodels/test_multianalytessingleactivemoiety.h"
#endif
#if defined(test_multianalytesmultiactivemoieties) || !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "drugmodels/test_multianalytesmultiactivemoieties.h"
#endif
#if defined(test_computingcomponentadjustments) || !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "test_computingcomponentadjustments.h"
#endif
#if defined(test_computingcomponentconcentration) || !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "test_computingcomponentconcentration.h"
#endif
#if defined(test_computingcomponentpercentiles) || !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "test_computingcomponentpercentiles.h"
#endif
#if defined(test_concentrationcalculator) || !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "test_concentrationcalculator.h"
#endif
#if defined(test_multiconcentrationcalculator) || !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "test_multiconcentrationcalculator.h"
#endif
#if defined(test_covariateextractor) || !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "test_covariateextractor.h"
#endif
#if defined(test_cyclestatistics) || !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "test_cyclestatistics.h"
#endif
#if defined(test_dosage) || !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "test_dosage.h"
#endif
#if defined(test_drugdomainconstraintsevaluator) || !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "test_drugdomainconstraintsevaluator.h"
#endif
#if defined(test_drugmodelimport) || !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "test_drugmodelimport.h"
#endif
#if defined(test_drugmodels) || !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "test_drugmodels.h"
#endif
#if defined(test_intakeextractor) || !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "test_intakeextractor.h"
#endif
#if defined(test_intakeintervalcalculator) || !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "test_intakeintervalcalculator.h"
#endif
#if defined(test_nonmemdrugs) || !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "test_nonmemdrugs.h"
#endif
#if defined(test_operablegraphmanager) || !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "test_operablegraphmanager.h"
#endif
#if defined(test_operation) || !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "test_operation.h"
#endif
#if defined(test_operationcollection) || !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "test_operationcollection.h"
#endif
#if defined(test_parameterextractor) || !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "test_parameterextractor.h"
#endif
#if defined(test_percentilecalculator) || !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "test_percentilecalculator.h"
#endif
#if defined(test_pertinenttimescalculator) || !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "test_pertinenttimescalculator.h"
#endif
#if defined(test_pkmodel) || !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "test_pkmodel.h"
#endif
#if defined(test_residualerrormodel) || !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "test_residualerrormodel.h"
#endif
#if defined(test_sampleextractor) || !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "test_sampleextractor.h"
#endif
#if defined(test_targetextractor) || !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "test_targetextractor.h"
#endif
#if defined(test_targetevaluator) || !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "test_targetevaluator.h"
#endif
#if defined(test_constanteliminationbolus) || !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "drugmodels/test_constanteliminationbolus.h"
#endif
#if defined(test_pkasymptotic) || !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "drugmodels/test_pkasymptotic.h"
#endif
#if defined(test_drug_tobramycin) || !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "drugmodels/test_drug_tobramycin.h"
#endif
#if defined(test_drug_vancomycin) || !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "drugmodels/test_drug_vancomycin.h"
#endif
#if defined(test_michaelismenten1comp) || !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "drugmodels/test_michaelismenten1comp.h"
#endif
#if defined(test_michaelismenten2comp) || !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "drugmodels/test_michaelismenten2comp.h"
#endif
#if defined(test_cachecomputing) || !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "test_cachecomputing.h"
#endif
#if defined(test_michaelismentenenzyme1comp) || !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "pkmodels/test_rkmichaelismentenenzyme.h"
#endif
#if defined(test_multilikelihood) || !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "test_multilikelihood.h"
#endif
#if defined(test_twocompartmentextralag) || !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "pkmodels/test_twocompartmentextralag.h"
#endif
#if defined(test_rkmichaelismententwocompvmaxamount) || !defined(DO_NOT_COMPILE_ALL_TESTS)
#include "pkmodels/test_rkmichaelismententwocompvmaxamount.h"
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

#if defined(test_parameter) || !defined(DO_NOT_COMPILE_ALL_TESTS)
    TestParameter parameterTests;

    parameterTests.add_test("testApplyEta", &TestParameter::testApplyEta);
    parameterTests.add_test("testGetTime", &TestParameter::testGetAtTime);

    res = parameterTests.run(argc, argv);
    tot_res |= res;
    if (res != 0) {
        std::cerr << "Parameter test failed\n";
    }
    else {
        std::cout << "Parameter test succeeded\n";
    }
#endif

#if defined(test_residualerrormodel) || !defined(DO_NOT_COMPILE_ALL_TESTS)
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

#if defined(test_pertinenttimescalculator) || !defined(DO_NOT_COMPILE_ALL_TESTS)
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

#if defined(test_drugdomainconstraintsevaluator) || !defined(DO_NOT_COMPILE_ALL_TESTS)
    TestDrugDomainConstraintsEvaluator drugDomainConstraintsEvaluatorTests;

    drugDomainConstraintsEvaluatorTests.add_test("simpleTest1", &TestDrugDomainConstraintsEvaluator::test1);
    drugDomainConstraintsEvaluatorTests.add_test("simpleTest2", &TestDrugDomainConstraintsEvaluator::test2);
    drugDomainConstraintsEvaluatorTests.add_test("simpleTest3", &TestDrugDomainConstraintsEvaluator::test3);
    drugDomainConstraintsEvaluatorTests.add_test("simpleTest4", &TestDrugDomainConstraintsEvaluator::test4);
    drugDomainConstraintsEvaluatorTests.add_test("simpleTest5", &TestDrugDomainConstraintsEvaluator::test5);
    drugDomainConstraintsEvaluatorTests.add_test("simpleTest6", &TestDrugDomainConstraintsEvaluator::test6);

    res = drugDomainConstraintsEvaluatorTests.run(argc, argv);
    tot_res |= res;
    if (res != 0) {
        std::cerr << "Drug model domain constraints evaluator test failed\n";
    }
    else {
        std::cout << "Drug model domain constraints evaluator test succeeded\n";
    }
#endif

#if defined(test_targetevaluator) || !defined(DO_NOT_COMPILE_ALL_TESTS)
    TestTargetEvaluator targetEvaluatorTests;


    targetEvaluatorTests.add_test("testResidual", &TestTargetEvaluator::testResidual);
    targetEvaluatorTests.add_test("testPeak", &TestTargetEvaluator::testPeak);
    targetEvaluatorTests.add_test("testMean", &TestTargetEvaluator::testMean);
    targetEvaluatorTests.add_test("testAuc", &TestTargetEvaluator::testAuc);
    targetEvaluatorTests.add_test("testAuc24", &TestTargetEvaluator::testAuc24);
    targetEvaluatorTests.add_test("testCumulativeAuc", &TestTargetEvaluator::testCumulativeAuc);
    targetEvaluatorTests.add_test("testAucOverMic", &TestTargetEvaluator::testAucOverMic);
    targetEvaluatorTests.add_test("testAuc24OverMic", &TestTargetEvaluator::testAuc24OverMic);
    targetEvaluatorTests.add_test("testTimeOverMic", &TestTargetEvaluator::testTimeOverMic);
    targetEvaluatorTests.add_test("testAucDividedByMic", &TestTargetEvaluator::testAucDividedByMic);
    targetEvaluatorTests.add_test("testAuc24DividedByMic", &TestTargetEvaluator::testAuc24DividedByMic);
    targetEvaluatorTests.add_test("testPeakDividedByMic", &TestTargetEvaluator::testPeakDividedByMic);
    targetEvaluatorTests.add_test("testFractionTimeOverMic", &TestTargetEvaluator::testFractionTimeOverMic);

    res = targetEvaluatorTests.run(argc, argv);
    tot_res |= res;
    if (res != 0) {
        std::cerr << "Target evaluator test failed\n";
    }
    else {
        std::cout << "Target evaluator test succeeded\n";
    }
#endif

#if defined(test_drugmodelimport) || !defined(DO_NOT_COMPILE_ALL_TESTS)
    TestDrugModelImport drugModelImportTests;
    drugModelImportTests.add_test("DrugModelImportFake", &TestDrugModelImport::testFake);
    drugModelImportTests.add_test("DrugModelImportBusulfan1", &TestDrugModelImport::testBusulfan1);
    drugModelImportTests.add_test("DrugModelImportOperation", &TestDrugModelImport::testOperation);

    res = drugModelImportTests.run(argc, argv);
    tot_res |= res;
    if (res != 0) {
        std::cerr << "Drug model import test failed\n";
    }
    else {
        std::cout << "Drug model import test succeeded\n";
    }
#endif

#if defined(test_computingcomponentconcentration) || !defined(DO_NOT_COMPILE_ALL_TESTS)
    TestComputingComponentConcentration computingComponentConcentrationTests;

    // one compartment
    computingComponentConcentrationTests.add_test("Simple1", &TestComputingComponentConcentration::testImatinib1);
    computingComponentConcentrationTests.add_test(
            "ImatinibSteadyState", &TestComputingComponentConcentration::testImatinibSteadyState);



    res = computingComponentConcentrationTests.run(argc, argv);
    tot_res |= res;
    if (res != 0) {
        //std::cerr << "Computing Component Concentration test failed\n";
    }
    else {
        // std::cout << "Computing Component Concentration test succeeded\n";
    }
#endif

#if defined(test_computingcomponentadjustments) || !defined(DO_NOT_COMPILE_ALL_TESTS)
    TestComputingComponentAdjusements computingComponentAdjusementsTests;

    // one compartment
    computingComponentAdjusementsTests.add_test(
            "Simple test 1", &TestComputingComponentAdjusements::testImatinibLastFormulationAndRouteAllDosages);
    computingComponentAdjusementsTests.add_test(
            "Simple test 2", &TestComputingComponentAdjusements::testImatinibDefaultFormulationAndRouteAllDosages);
    computingComponentAdjusementsTests.add_test(
            "Simple test 3", &TestComputingComponentAdjusements::testImatinibAllFormulationAndRouteAllDosages);
    computingComponentAdjusementsTests.add_test(
            "Simple test 4", &TestComputingComponentAdjusements::testImatinibLastFormulationAndRouteBestDosage);
    computingComponentAdjusementsTests.add_test(
            "Simple test 5", &TestComputingComponentAdjusements::testImatinibDefaultFormulationAndRouteBestDosage);
    computingComponentAdjusementsTests.add_test(
            "Simple test 6", &TestComputingComponentAdjusements::testImatinibAllFormulationAndRouteBestDosage);
    computingComponentAdjusementsTests.add_test(
            "Simple test 7",
            &TestComputingComponentAdjusements::testImatinibEmptyTreatmentDefaultFormulationAndRouteAllDosages);
    computingComponentAdjusementsTests.add_test(
            "Simple test 8",
            &TestComputingComponentAdjusements::testImatinibSteadyStateLastFormulationAndRouteAllDosages);
    computingComponentAdjusementsTests.add_test(
            "Simple test 9",
            &TestComputingComponentAdjusements::testImatinibLastFormulationAndRouteAllDosagesAtSteadyState);
    computingComponentAdjusementsTests.add_test(
            "Simple test 10",
            &TestComputingComponentAdjusements::testImatinibAllFormulationAndRouteBestDosageLoadingDose);
    computingComponentAdjusementsTests.add_test(
            "Simple test 11",
            &TestComputingComponentAdjusements::testImatinibAllFormulationAndRouteBestDosageRestPeriod);

    res = computingComponentAdjusementsTests.run(argc, argv);
    tot_res |= res;
    if (res != 0) {
        std::cerr << "Computing Component Adjustments test failed\n";
    }
    else {
        std::cout << "Computing Component Adjustments test succeeded\n";
    }
#endif

#if defined(test_intakeintervalcalculator) || !defined(DO_NOT_COMPILE_ALL_TESTS)
    TestIntervalCalculator calculatorsTests;

    // one compartment
    calculatorsTests.add_test(
            "1 comp bolus single vs multiple test", &TestIntervalCalculator::test1compBolusSingleVsMultiple);
    calculatorsTests.add_test(
            "1 comp extra single vs multiple test", &TestIntervalCalculator::test1compExtraSingleVsMultiple);
    calculatorsTests.add_test(
            "1 comp extra lag single vs multiple test", &TestIntervalCalculator::test1compExtraLagSingleVsMultiple);
    calculatorsTests.add_test(
            "1 comp infusion single vs multiple test", &TestIntervalCalculator::test1compInfusionSingleVsMultiple);

    calculatorsTests.add_test(
            "rk 1 comp extra single vs multiple test", &TestIntervalCalculator::testRK41compExtraSingleVsMultiple);
    calculatorsTests.add_test(
            "rk 1 comp gamma extra single vs multiple test",
            &TestIntervalCalculator::testRK41compGammaExtraSingleVsMultiple);

    calculatorsTests.add_test("1 comp extra analytical vs Rk4", &TestIntervalCalculator::test1compExtraAnalyticalVsRk4);
    calculatorsTests.add_test("2 comp erlang transit", &TestIntervalCalculator::test2compErlang);


    // two compartment
    calculatorsTests.add_test(
            "2 comp bolus single vs multiple test", &TestIntervalCalculator::test2compBolusSingleVsMultiple);
    calculatorsTests.add_test(
            "2 comp extra single vs multiple test", &TestIntervalCalculator::test2compExtraSingleVsMultiple);
    calculatorsTests.add_test(
            "2 comp infusion single vs multiple test", &TestIntervalCalculator::test2compInfusionSingleVsMultiple);
    calculatorsTests.add_test(
            "2 comp extra lag single vs multiple test", &TestIntervalCalculator::test2compExtraLagSingleVsMultiple);
    calculatorsTests.add_test(
            "2 comp extra lag analytical vs Rk4 test", &TestIntervalCalculator::test2compExtraLagAnalyticalVsRk4);

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
    else {
        std::cout << "Calculators test succeeded\n";
    }
#endif

#if defined(test_concentrationcalculator) || !defined(DO_NOT_COMPILE_ALL_TESTS)
    // --- ConcentrationCalculator --- //
    TestConcentrationCalculator concentrationCalculatorTests;
    concentrationCalculatorTests.add_test(
            "linear bolus test", &TestConcentrationCalculator::testConstantEliminationBolus);
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
    else {
        std::cout << "ConcentrationCalculator test succeeded\n";
    }
#endif

#if defined(test_multiconstanteliminationbolus) || !defined(DO_NOT_COMPILE_ALL_TESTS)
    // --- ConcentrationCalculator --- //
    TestMultiConstantEliminationBolus multiConstantEliminationBolusTests;
    multiConstantEliminationBolusTests.add_test(
            "multiConstant bolus test", &TestMultiConstantEliminationBolus::testMultiConstantEliminationBolus);

    res = multiConstantEliminationBolusTests.run(argc, argv);
    tot_res |= res;
    if (res != 0) {
        std::cerr << "MultiConstantEliminationBolus test failed\n";
    }
    else {
        std::cout << "MultiConstantEliminationBolus test succeeded\n";
    }
#endif

#if defined(test_multiconcentrationcalculator) || !defined(DO_NOT_COMPILE_ALL_TESTS)
    // --- ConcentrationCalculator --- //
    TestMultiConcentrationCalculator multiConcentrationCalculatorTests;
    multiConcentrationCalculatorTests.add_test(
            "constant elimination bolus test", &TestMultiConcentrationCalculator::testConstantEliminationBolus);
    multiConcentrationCalculatorTests.add_test("1 comp bolus test", &TestMultiConcentrationCalculator::test1compBolus);
    multiConcentrationCalculatorTests.add_test("1 comp extra test", &TestMultiConcentrationCalculator::test1compExtra);
    multiConcentrationCalculatorTests.add_test(
            "1 comp infusion test", &TestMultiConcentrationCalculator::test1compInfusion);
    multiConcentrationCalculatorTests.add_test("2 comp bolus test", &TestMultiConcentrationCalculator::test2compBolus);
    multiConcentrationCalculatorTests.add_test("2 comp extra test", &TestMultiConcentrationCalculator::test2compExtra);
    multiConcentrationCalculatorTests.add_test(
            "2 comp infusion test", &TestMultiConcentrationCalculator::test2compInfusion);
    multiConcentrationCalculatorTests.add_test(
            "multiconstant elimination bolus test",
            &TestMultiConcentrationCalculator::testMultiConstantEliminationBolus);

    res = multiConcentrationCalculatorTests.run(argc, argv);
    tot_res |= res;
    if (res != 0) {
        std::cerr << "MultiConcentrationCalculator test failed\n";
    }
    else {
        std::cout << "MultiConcentrationCalculator test succeeded\n";
    }
#endif
#if defined(test_dosage) || !defined(DO_NOT_COMPILE_ALL_TESTS)
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
    }
    else {
        std::cout << "Dosage test succeeded\n";
    }
#endif

#if defined(test_multilikelihood) || !defined(DO_NOT_COMPILE_ALL_TESTS)
    // --- MultiLikelihood tests --- //
    TestMultiLikeliHood multilikelihoodTests;
    multilikelihoodTests.add_test("multilikelihoodTest1", &TestMultiLikeliHood::test1analyte1sample);
    multilikelihoodTests.add_test("multilikelihoodTest2", &TestMultiLikeliHood::test1analyte3samples);
    multilikelihoodTests.add_test("multilikelihoodTest3", &TestMultiLikeliHood::test2analytes1sampleonanalyte1);
    multilikelihoodTests.add_test("multilikelihoodTest4", &TestMultiLikeliHood::test2analytes1sampleonanalyte2);
    multilikelihoodTests.add_test("multilikelihoodTest5", &TestMultiLikeliHood::test2analytes2samplesdifferenttimes);
    multilikelihoodTests.add_test("multilikelihoodTest6", &TestMultiLikeliHood::test2analytes2samplessametime);
    multilikelihoodTests.add_test("multilikelihoodTest7", &TestMultiLikeliHood::test2analytes6samples);

    res = multilikelihoodTests.run(argc, argv);
    tot_res |= res;
    if (res != 0) {
        std::cerr << "Multilikelihood test failed\n";
    }
    else {
        std::cout << "Multilikelihood test succeeded\n";
    }
#endif



#if defined(test_intakeextractor) || !defined(DO_NOT_COMPILE_ALL_TESTS)
    // --- INTAKE EXTRACTOR --- //
    TestIntakeExtractor intakeExtractorTests;
    intakeExtractorTests.add_test("OncePerWeek test", &TestIntakeExtractor::testOncePerWeek);
    intakeExtractorTests.add_test("OnceEveryTenDays test", &TestIntakeExtractor::testOnceEveryTenDays);
    intakeExtractorTests.add_test("OnceEvery36Hours test", &TestIntakeExtractor::testOnceEvery36Hours);
    intakeExtractorTests.add_test("FiveTimesEvery12Hours test", &TestIntakeExtractor::testFiveTimesEvery12Hours);
    intakeExtractorTests.add_test(
            "FiveTimesEvery12HoursEarlyStop test", &TestIntakeExtractor::testFiveTimesEvery12HoursEarlyStop);
    intakeExtractorTests.add_test("TwiceEveryTenDays test", &TestIntakeExtractor::testTwiceEveryTenDays);
    intakeExtractorTests.add_test("OnceEveryDay test", &TestIntakeExtractor::testOnceEveryDay);
    intakeExtractorTests.add_test("ThreeTimesEveryDay test", &TestIntakeExtractor::testThreeTimesEveryDay);
    intakeExtractorTests.add_test("ComplexParallelSequence1 test", &TestIntakeExtractor::testComplexParallelSequence1);
    intakeExtractorTests.add_test("ComplexParallelSequence2 test", &TestIntakeExtractor::testComplexParallelSequence2);
    intakeExtractorTests.add_test("FullWeekExceptMonday test", &TestIntakeExtractor::testFullWeekExceptMonday);
    intakeExtractorTests.add_test(
            "OnceEvery36HoursAtSteadyState test", &TestIntakeExtractor::testOnceEvery36HoursAtSteadyState);

    res = intakeExtractorTests.run(argc, argv);
    tot_res |= res;
    if (res != 0) {
        std::cerr << "IntakeExtractor test failed\n";
    }
    else {
        std::cout << "IntakeExtractor test succeeded\n";
    }
#endif

#if defined(test_operation) || !defined(DO_NOT_COMPILE_ALL_TESTS)
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
    }
    else {
        std::cout << "Operation test succeeded\n";
    }
#endif

#if defined(test_operationcollection) || !defined(DO_NOT_COMPILE_ALL_TESTS)
    // --- OPERATIONCOLLECTION --- //
    TestOperationCollection operationCollectionTests;
    operationCollectionTests.add_test("OperationCollection test", &TestOperationCollection::testOperationCollection);

    res = operationCollectionTests.run(argc, argv);
    tot_res |= res;
    if (res != 0) {
        std::cerr << "OperationCollection test failed\n";
    }
    else {
        std::cout << "OperationCollection test succeeded\n";
    }

    // --- PkModel --- //
    TestPkModel pkmodelTest;
    pkmodelTest.add_test("testPkModelFunctions test", &TestPkModel::testPkModelFunctions);

    res = pkmodelTest.run(argc, argv);
    tot_res |= res;
    if (res != 0) {
        std::cerr << "PkModel test failed\n";
    }
    else {
        std::cout << "PkModel test succeeded\n";
    }
#endif

#if defined(test_covariateextractor) || !defined(DO_NOT_COMPILE_ALL_TESTS)
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
    ceTests.add_test("testOperableFunctions_3_3", &TestCovariateExtractor::testCovariateExtraction_test3_3);
    ceTests.add_test("testOperableFunctions_3_4", &TestCovariateExtractor::testCovariateExtraction_test3_4);

    res = ceTests.run(argc, argv);
    tot_res |= res;

    if (res != 0) {
        std::cerr << "Covariate Extractor test failed\n";
    }
    else {
        std::cout << "Covariate Extractor test succeeded\n";
    }
#endif

#if defined(test_parameterextractor) || !defined(DO_NOT_COMPILE_ALL_TESTS)
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
    }
    else {
        std::cout << "Parameter Extractor test succeeded\n";
    }
#endif

#if defined(test_operablegraphmanager) || !defined(DO_NOT_COMPILE_ALL_TESTS)
    // --- OperableGraphManager --- //
    TestOpGraph opGraphTests;
    opGraphTests.add_test("testOperableFunctions", &TestOpGraph::testOperableFunctions);
    opGraphTests.add_test("testOperableCockcroftGaultIBW", &TestOpGraph::testOperableCockcroftGaultIBW);
    opGraphTests.add_test("testOperableCyclic", &TestOpGraph::testOperableCyclic);

    res = opGraphTests.run(argc, argv);
    tot_res |= res;

    if (res != 0) {
        std::cerr << "Operable Graph Manager test failed\n";
    }
    else {
        std::cout << "Operable Graph Manager test succeeded\n";
    }
#endif

#if defined(test_percentilecalculator) || !defined(DO_NOT_COMPILE_ALL_TESTS)
    // --- percentile calculator --- //
    TestPercentileCalculator percentileCalculatorTests;

    percentileCalculatorTests.add_test(
            "Apriori Monte Carlo Percentile with invalid parameters",
            &TestPercentileCalculator::testAprioriInvalidParameters);
    percentileCalculatorTests.add_test("Apriori Monte Carlo Percentile", &TestPercentileCalculator::testApriori);
    percentileCalculatorTests.add_test(
            "Aposteriori Normal Monte Carlo Percentile", &TestPercentileCalculator::testAposterioriNormal);
    percentileCalculatorTests.add_test(
            "Aposteriori Matrix cache", &TestPercentileCalculator::testAposterioriMatrixCache);
    percentileCalculatorTests.add_test(
            "Aposteriori Monte Carlo Percentile", &TestPercentileCalculator::testAposteriori);
    percentileCalculatorTests.add_test(
            "Aposteriori Monte Carlo Percentile with invalid parameters",
            &TestPercentileCalculator::testAposterioriInvalidParameters);
    percentileCalculatorTests.add_test(
            "Aposteriori percentiles with samples far away in time",
            &TestPercentileCalculator::testAposterioriFarSamples);

    res = percentileCalculatorTests.run(argc, argv);
    tot_res |= res;
    if (res != 0) {
        std::cerr << "Percentile Calculator test failed\n";
    }
#endif

#if defined(test_nonmemdrugs) || !defined(DO_NOT_COMPILE_ALL_TESTS)
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

#if defined(test_drugmodels) || !defined(DO_NOT_COMPILE_ALL_TESTS)
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

#if defined(test_cyclestatistics) || !defined(DO_NOT_COMPILE_ALL_TESTS)
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

#if defined(test_computingcomponentpercentiles) || !defined(DO_NOT_COMPILE_ALL_TESTS)
    TestComputingComponentPercentiles computingComponentPercentilesTests;

    // one compartment
    computingComponentPercentilesTests.add_test(
            "ImatinibPercentiles", &TestComputingComponentPercentiles::testImatinib1);
    computingComponentPercentilesTests.add_test(
            "ImatinibSteadyStatePercentiles", &TestComputingComponentPercentiles::testImatinibSteadyState);
    computingComponentPercentilesTests.add_test(
            "AposterioriPercentiles", &TestComputingComponentPercentiles::testAposterioriPercentiles);



    res = computingComponentPercentilesTests.run(argc, argv);
    tot_res |= res;
    if (res != 0) {
        std::cerr << "Computing Component Percentiles test failed\n";
    }
    else {
        std::cout << "Computing Component Percentiles test succeeded\n";
    }
#endif

#if defined(test_sampleextractor) || !defined(DO_NOT_COMPILE_ALL_TESTS)
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

#if defined(test_targetextractor) || !defined(DO_NOT_COMPILE_ALL_TESTS)
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

#if defined(test_drug_tobramycin) || !defined(DO_NOT_COMPILE_ALL_TESTS)
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

#if defined(test_drug_vancomycin) || !defined(DO_NOT_COMPILE_ALL_TESTS)
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

#if defined(test_constanteliminationbolus) || !defined(DO_NOT_COMPILE_ALL_TESTS)
    // --- Constant elimination tests --- //
    TestConstantEliminationBolus constantEliminationBolusTests;

    constantEliminationBolusTests.add_test(
            "testConstantEliminationBolus", &TestConstantEliminationBolus::testConstantElimination);

    res = constantEliminationBolusTests.run(argc, argv);
    tot_res |= res;
    if (res != 0) {
        std::cerr << "Constant Elimination Bolus test failed\n";
    }
    else {
        std::cout << "Constant Elimination Bolus test succeeded\n";
    }
#endif


#if defined(test_pkasymptotic) || !defined(DO_NOT_COMPILE_ALL_TESTS)
    // --- Constant elimination tests --- //
    TestPkAsymptotic pkAsymptoticTests;

    pkAsymptoticTests.add_test("testPkAsymptotic", &TestPkAsymptotic::testPkAsymptotic);

    res = pkAsymptoticTests.run(argc, argv);
    tot_res |= res;
    if (res != 0) {
        std::cerr << "Pk asymptotic test failed\n";
    }
    else {
        std::cout << "Pk asymptotic test succeeded\n";
    }
#endif

#if defined(test_multianalytessingleactivemoiety) || !defined(DO_NOT_COMPILE_ALL_TESTS)
    // --- Multi analytes multi active moieties tests --- //
    TestMultiAnalytesSingleActiveMoiety multiAnalytesSingleActiveMoietyTests;

    multiAnalytesSingleActiveMoietyTests.add_test(
            "testMultiAnalytesSingleActiveMoieties",
            &TestMultiAnalytesSingleActiveMoiety::testMultiAnalytesSingleActiveMoiety);
    multiAnalytesSingleActiveMoietyTests.add_test(
            "testMultiAnalytesSingleActiveMoietiesConversion",
            &TestMultiAnalytesSingleActiveMoiety::testMultiAnalytesSingleActiveMoietyConversion);
    multiAnalytesSingleActiveMoietyTests.add_test(
            "testMultiAnalytesSingleActiveMoietyAdjustments", &TestMultiAnalytesSingleActiveMoiety::testAdjustments);

    res = multiAnalytesSingleActiveMoietyTests.run(argc, argv);
    tot_res |= res;
    if (res != 0) {
        std::cerr << "Multi analytes single active moiety test failed\n";
    }
    else {
        std::cout << "Multi analytes single active moiety test succeeded\n";
    }
#endif

    /*
#if defined(test_multianalytesmultiactivemoieties) || !defined(DO_NOT_COMPILE_ALL_TESTS)
    // --- Multi analytes multi active moieties tests --- //
    TestMultiAnalytesMultiActiveMoieties multiAnalytesMultiActiveMoietiesTests;

    multiAnalytesMultiActiveMoietiesTests.add_test(
            "testMultiAnalytesMultiActiveMoieties",
            &TestMultiAnalytesMultiActiveMoieties::testMultiAnalytesMultiActiveMoieties);
    multiAnalytesMultiActiveMoietiesTests.add_test(
            "testMultiAnalytesMultiActiveMoietiesConversion",
            &TestMultiAnalytesMultiActiveMoieties::testMultiAnalytesMultiActiveMoietiesConversion);
    multiAnalytesMultiActiveMoietiesTests.add_test(
            "testMultiAnalytesMultiActiveMoietyAdjustments", &TestMultiAnalytesMultiActiveMoieties::testAdjustments);

    res = multiAnalytesMultiActiveMoietiesTests.run(argc, argv);
    tot_res |= res;
    if (res != 0) {
        std::cerr << "Multi analytes multi active moieties test failed\n";
    }
    else {
        std::cout << "Multi analytes multi active moieties test succeeded\n";
    }
#endif
*/


#if defined(test_michaelismenten1comp) || !defined(DO_NOT_COMPILE_ALL_TESTS)
    // --- Multi analytes multi active moieties tests --- //
    TestMichaelisMenten1comp michaelisMentent1compTests;

    michaelisMentent1compTests.add_test(
            "testMichaelisMenten1comp", &TestMichaelisMenten1comp::testMichaelisMenten1comp);

    res = michaelisMentent1compTests.run(argc, argv);
    tot_res |= res;
    if (res != 0) {
        std::cerr << "Michaelis Menten 1 comp test failed\n";
    }
    else {
        std::cout << "Michaelis Menten 1 comp test succeeded\n";
    }
#endif


#if defined(test_michaelismenten2comp) || !defined(DO_NOT_COMPILE_ALL_TESTS)
    // --- Multi analytes multi active moieties tests --- //
    TestMichaelisMenten2comp michaelisMentent2compTests;

    michaelisMentent2compTests.add_test("testMichaelisMenten2compBolus", &TestMichaelisMenten2comp::testBolus);
    michaelisMentent2compTests.add_test("testMichaelisMenten2compInfusion", &TestMichaelisMenten2comp::testInfusion);
    michaelisMentent2compTests.add_test("testMichaelisMenten2compExtra", &TestMichaelisMenten2comp::testExtra);
    michaelisMentent2compTests.add_test("testMichaelisMenten2compExtraLag", &TestMichaelisMenten2comp::testExtraLag);
    michaelisMentent2compTests.add_test("testMichaelisMenten2compMix", &TestMichaelisMenten2comp::testMix);
    michaelisMentent2compTests.add_test(
            "testMichaelisMenten2compMixPercentiles", &TestMichaelisMenten2comp::testMixPercentiles);

    res = michaelisMentent2compTests.run(argc, argv);
    tot_res |= res;
    if (res != 0) {
        std::cerr << "Michaelis Menten 2 comp test failed\n";
    }
    else {
        std::cout << "Michaelis Menten 2 comp test succeeded\n";
    }
#endif

#if defined(test_likelihood) || !defined(DO_NOT_COMPILE_ALL_TESTS)
    // --- Multi analytes multi active moieties tests --- //
    TestLikelihood likelihoodTests;

    likelihoodTests.add_test("testLikelihoodWrongParameters", &TestLikelihood::testWrongParameters);
    likelihoodTests.add_test("testLikelihood1Sample", &TestLikelihood::test1Sample);
    likelihoodTests.add_test("testLikelihoodweightsample", &TestLikelihood::testWeightSample);
    likelihoodTests.add_test("testLikelihood2SamplesSameDate", &TestLikelihood::test2SamplesSameDate);
    likelihoodTests.add_test("testLikelihood2SamplesDifferentDates", &TestLikelihood::test2SamplesDifferentDates);
    likelihoodTests.add_test("testLikelihood3SamplesDifferentDates", &TestLikelihood::test3SamplesDifferentDates);



    res = likelihoodTests.run(argc, argv);
    tot_res |= res;
    if (res != 0) {
        std::cerr << "Likelihood comp test failed\n";
    }
    else {
        std::cout << "Likelihood test succeeded\n";
    }
#endif


#if defined(test_cachecomputing) || !defined(DO_NOT_COMPILE_ALL_TESTS)
    TestCacheComputing cacheComputingTests;

    // one compartment
    cacheComputingTests.add_test("ImatinibSplitInterval", &TestCacheComputing::testImatinibSplitInterval);
    cacheComputingTests.add_test("ImatinibFullInterval", &TestCacheComputing::testImatinibFullInterval);

    res = cacheComputingTests.run(argc, argv);
    tot_res |= res;
    if (res != 0) {
        std::cerr << "Cache Computing test failed\n";
    }
    else {
        std::cout << "Cache Computing test succeeded\n";
    }
#endif


#if defined(test_michaelismentenenzyme1comp) || !defined(DO_NOT_COMPILE_ALL_TESTS)
    // --- Multi analytes multi active moieties tests --- //
    TestMichaelisMentenEnzyme1comp michaelisMententEnzyme1compTests;

    michaelisMententEnzyme1compTests.add_test(
            "testMichaelisMenten2compBolus", &TestMichaelisMentenEnzyme1comp::testBolus);
    michaelisMententEnzyme1compTests.add_test(
            "testMichaelisMenten1compExtra", &TestMichaelisMentenEnzyme1comp::testExtra);
    michaelisMententEnzyme1compTests.add_test(
            "testMichaelisMenten1compPercentilesApriori", &TestMichaelisMentenEnzyme1comp::testExtraPercentilesApriori);



    res = michaelisMententEnzyme1compTests.run(argc, argv);
    tot_res |= res;
    if (res != 0) {
        std::cerr << "Michaelis Menten Enzyme 1 comp test failed\n";
    }
    else {
        std::cout << "Michaelis Menten Enzyme 1 comp test succeeded\n";
    }
#endif


#if defined(test_twocompartmentextralag) || !defined(DO_NOT_COMPILE_ALL_TESTS)
    // --- Multi analytes multi active moieties tests --- //
    TestTwoCompartmentExtraLag twoCompartmentExtraLagTests;

    twoCompartmentExtraLagTests.add_test(
            "testTwoCompartmentExtraLag", &TestTwoCompartmentExtraLag::test2compExtraLagSingleDose);

    res = twoCompartmentExtraLagTests.run(argc, argv);
    tot_res |= res;
    if (res != 0) {
        std::cerr << "2 comp extra lag test failed\n";
    }
    else {
        std::cout << "2 comp extra lag test succeeded\n";
    }
#endif

#if defined(test_rkmichaelismententwocompvmaxamount) || !defined(DO_NOT_COMPILE_ALL_TESTS)
    // --- Multi analytes multi active moieties tests --- //
    TestMichaelisMenten2compVmaxAmount twoCompMMVmaxAmountTests;

    twoCompMMVmaxAmountTests.add_test("testTwoCompMMVmaxAmountBolus", &TestMichaelisMenten2compVmaxAmount::testBolus);
    twoCompMMVmaxAmountTests.add_test("testTwoCompMMVmaxAmountInfu", &TestMichaelisMenten2compVmaxAmount::testInfusion);
    twoCompMMVmaxAmountTests.add_test("testTwoCompMMVmaxAmountExtra", &TestMichaelisMenten2compVmaxAmount::testExtra);
    twoCompMMVmaxAmountTests.add_test("testTwoCompMMVmaxAmountMix", &TestMichaelisMenten2compVmaxAmount::testMix);

    res = twoCompMMVmaxAmountTests.run(argc, argv);
    tot_res |= res;
    if (res != 0) {
        std::cerr << "2 comp Michaelis Menten VMax Amount test failed\n";
    }
    else {
        std::cout << "2 comp Michaelis Menten VMax Amount test succeeded\n";
    }
#endif


    // Delete the logger to avoid a warning when using valgrind --leak-check=full
    Tucuxi::Common::LoggerHelper::beforeExit();

    return tot_res;
}
