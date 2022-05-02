//@@license@@

#ifndef TEST_TARGETEXTRACTOR_H
#define TEST_TARGETEXTRACTOR_H


#include <chrono>
#include <iostream>
#include <memory>

#include <date/date.h>

#include "tucucommon/basetypes.h"
#include "tucucommon/timeofday.h"

#include "tucucore/drugmodel/covariatedefinition.h"
#include "tucucore/drugtreatment/patientcovariate.h"
#include "tucucore/operation.h"
#include "tucucore/targetextractor.h"

#include "fructose/fructose.h"
#include "testutils.h"

using namespace Tucuxi::Common::Utils;
using namespace Tucuxi::Core;


struct TestTargetExtractor : public fructose::test_base<TestTargetExtractor>
{
    TestTargetExtractor() {}

    void testAprioriValues()
    {

        TargetExtractor extractor;


        CovariateSeries covariates;
        TargetDefinitions targetDefinitions;
        Targets targets;
        DateTime start = DateTime::now();
        DateTime end = DateTime::now();
        TargetExtractionOption extractionOption;
        TargetSeries series;

        ComputingStatus result;

        {
            extractionOption = TargetExtractionOption::AprioriValues;

            result = extractor.extract(
                    ActiveMoietyId("NoId"),
                    covariates,
                    targetDefinitions,
                    targets,
                    start,
                    end,
                    TucuUnit("ug/l"),
                    extractionOption,
                    series);

            // Not yet implemented, so extraction error
            fructose_assert(result == ComputingStatus::TargetExtractionError);
        }
    }

    void testPopulationValues()
    {

        TargetExtractor extractor;


        CovariateSeries covariates;
        TargetDefinitions targetDefinitions;
        Targets targets;
        DateTime start = DateTime::now();
        DateTime end = DateTime::now();
        TargetExtractionOption extractionOption;
        TargetSeries series;


        ComputingStatus result;

        {
            // Test with only population values

            // Add targets
            auto target = std::make_unique<TargetDefinition>(
                    TargetType::Residual,
                    TucuUnit("ug/l"),
                    ActiveMoietyId("imatinib"),
                    std::make_unique<SubTargetDefinition>("cMin", 750.0, nullptr),
                    std::make_unique<SubTargetDefinition>("cMax", 1500.0, nullptr),
                    std::make_unique<SubTargetDefinition>("cBest", 1000.0, nullptr),
                    std::make_unique<SubTargetDefinition>("mic", 2.0, nullptr),
                    std::make_unique<SubTargetDefinition>("tMin", 1000.0, nullptr),
                    std::make_unique<SubTargetDefinition>("tMax", 1200.0, nullptr),
                    std::make_unique<SubTargetDefinition>("tBest", 1100.0, nullptr),
                    std::make_unique<SubTargetDefinition>("toxicity", 10000.0, nullptr),
                    std::make_unique<SubTargetDefinition>("inefficacy", 1.0, nullptr));


            targetDefinitions.push_back(std::move(target));

            extractionOption = TargetExtractionOption::PopulationValues;
            result = extractor.extract(
                    ActiveMoietyId("imatinib"),
                    covariates,
                    targetDefinitions,
                    targets,
                    start,
                    end,
                    TucuUnit("ug/l"),
                    extractionOption,
                    series);
            fructose_assert_eq(result, ComputingStatus::Ok);

            fructose_assert(series.size() == 1);
            {
                TargetEvent targetResult = series[0];

                fructose_assert_double_eq(targetResult.m_valueMin, 750.0);
                fructose_assert_double_eq(targetResult.m_valueMax, 1500.0);
                fructose_assert_double_eq(targetResult.m_valueBest, 1000.0);
                // The MIC is not defined in a target event Residual
                //fructose_assert( targetResult.m_mic == 2.0);
                //fructose_assert( targetResult.m_tMin == 1000.0);
                //fructose_assert( targetResult.m_tMax == 1200.0);
                //fructose_assert( targetResult.m_tBest == 1100.0);
                //fructose_assert( targetResult.m_toxicity == 1100.0);
                //fructose_assert( targetResult.m_tBest == 1100.0);
            }
        }
    }

    void testIndividualTargets()
    {

        TargetExtractor extractor;


        CovariateSeries covariates;
        TargetDefinitions targetDefinitions;
        Targets targets;
        DateTime start = DateTime::now();
        DateTime end = DateTime::now();
        TargetExtractionOption extractionOption;
        TargetSeries series;

        ComputingStatus result;


        {
            // Test with individual values

            // Add targets
            auto target = std::make_unique<TargetDefinition>(
                    TargetType::Residual,
                    TucuUnit("mg/l"),
                    ActiveMoietyId("imatinib"),
                    std::make_unique<SubTargetDefinition>("cMin", 750.0, nullptr),
                    std::make_unique<SubTargetDefinition>("cMax", 1500.0, nullptr),
                    std::make_unique<SubTargetDefinition>("cBest", 1000.0, nullptr),
                    std::make_unique<SubTargetDefinition>("mic", 2.0, nullptr),
                    std::make_unique<SubTargetDefinition>("tMin", 1000.0, nullptr),
                    std::make_unique<SubTargetDefinition>("tMax", 1200.0, nullptr),
                    std::make_unique<SubTargetDefinition>("tBest", 1100.0, nullptr),
                    std::make_unique<SubTargetDefinition>("toxicity", 10000.0, nullptr),
                    std::make_unique<SubTargetDefinition>("inefficacy", 1.0, nullptr));


            targetDefinitions.push_back(std::move(target));


            auto patientTarget = std::make_unique<Target>(
                    ActiveMoietyId("imatinib"), TargetType::Residual, TucuUnit("mg/l"), 50.0, 100.0, 150.0, 2.0, 200.0);

            targets.push_back(std::move(patientTarget));

            extractionOption = TargetExtractionOption::IndividualTargets;
            result = extractor.extract(
                    ActiveMoietyId("imatinib"),
                    covariates,
                    targetDefinitions,
                    targets,
                    start,
                    end,
                    TucuUnit("ug/l"),
                    extractionOption,
                    series);
            fructose_assert_eq(result, ComputingStatus::Ok);

            fructose_assert(series.size() == 1);
            {
                TargetEvent targetResult = series[0];

                // Here the values are x1000 the target values, as the units in the definition are mg/l. The final
                // target unit is in ug/l
                fructose_assert_double_eq(targetResult.m_valueMin, 50000.0);
                fructose_assert_double_eq(targetResult.m_valueMax, 150000.0);
                fructose_assert_double_eq(targetResult.m_valueBest, 100000.0);
                //fructose_assert( targetResult.m_mic == 2000.0);
                //fructose_assert( targetResult.m_tMin == 1000.0);
                //fructose_assert( targetResult.m_tMax == 1200.0);
                //fructose_assert( targetResult.m_tBest == 1100.0);
                //fructose_assert( targetResult.m_toxicity == 1100.0);
                //fructose_assert( targetResult.m_tBest == 1100.0);
            }
        }
    }

    void testIndividualTargetsNoDefinition()
    {

        TargetExtractor extractor;


        CovariateSeries covariates;
        TargetDefinitions targetDefinitions;
        Targets targets;
        DateTime start = DateTime::now();
        DateTime end = DateTime::now();
        TargetExtractionOption extractionOption;
        TargetSeries series;

        ComputingStatus result;


        {
            // Test with individual values

            // Add targets

            auto patientTarget = std::make_unique<Target>(
                    ActiveMoietyId("imatinib"), TargetType::Residual, TucuUnit("mg/l"), 50.0, 100.0, 150.0, 2.0, 200.0);

            targets.push_back(std::move(patientTarget));

            extractionOption = TargetExtractionOption::DefinitionIfNoIndividualTarget;
            result = extractor.extract(
                    ActiveMoietyId("imatinib"),
                    covariates,
                    targetDefinitions,
                    targets,
                    start,
                    end,
                    TucuUnit("ug/l"),
                    extractionOption,
                    series);
            fructose_assert_eq(result, ComputingStatus::Ok);

            fructose_assert(series.size() == 1);
            {
                TargetEvent targetResult = series[0];

                // Here the values are x1000 the target values, as the units in the definition are mg/l. The final
                // target unit is in ug/l
                fructose_assert_double_eq(targetResult.m_valueMin, 50000.0);
                fructose_assert_double_eq(targetResult.m_valueMax, 150000.0);
                fructose_assert_double_eq(targetResult.m_valueBest, 100000.0);
                //fructose_assert( targetResult.m_mic == 2000.0);
                //fructose_assert( targetResult.m_tMin == 1000.0);
                //fructose_assert( targetResult.m_tMax == 1200.0);
                //fructose_assert( targetResult.m_tBest == 1100.0);
                //fructose_assert( targetResult.m_toxicity == 1100.0);
                //fructose_assert( targetResult.m_tBest == 1100.0);
            }
        }
    }


    /// \brief Check that objects are correctly constructed by the constructor.
    void testTargetExtractor(const std::string& /* _testName */)
    {
        testAprioriValues();

        testPopulationValues();

        testIndividualTargets();

        testIndividualTargetsNoDefinition();
    }
};


#endif // TEST_TARGETEXTRACTOR_H
