/*
* Copyright (C) 2017 Tucuxi SA
*/

#include <iostream>
#include <string>

#include "tucucommon/utils.h"
#include "tucucore/dosage.h"

#include "test_dosage.h"
#include "test_intakeextractor.h"

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
        printf("Dosage test failed");
        exit(1);
    }

    // --- INTAKE EXTRACTOR --- //
    TestIntakeExtractor intakeExtractorTests;
    intakeExtractorTests.add_test("IntakeExtractor-basic", &TestIntakeExtractor::basic);
    intakeExtractorTests.add_test("IntakeExtractor-weekWithoutWeekEnd", &TestIntakeExtractor::weekWithoutWeekEnd);
    intakeExtractorTests.add_test("IntakeExtractor-weekWithWeekEnd", &TestIntakeExtractor::weekWithoutWeekEnd);

    res = intakeExtractorTests.run(argc, argv);
    if (res != 0) {
        printf("IntakeExtractor test failed");
        exit(1);
    }

    return 0;
}
