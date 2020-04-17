#ifndef TEST_TARGETEXTRACTOR_H
#define TEST_TARGETEXTRACTOR_H


#include <chrono>
#include <iostream>
#include <memory>

#include <date/date.h>

#include "fructose/fructose.h"

#include "tucucore/targetextractor.h"
#include "tucucore/operation.h"

#include "tucucore/drugmodel/covariatedefinition.h"
#include "tucucore/drugtreatment/patientcovariate.h"

#include "tucucommon/basetypes.h"
#include "tucucommon/timeofday.h"

#include "testutils.h"

using namespace Tucuxi::Common::Utils;
using namespace Tucuxi::Core;


struct TestTargetExtractor : public fructose::test_base<TestTargetExtractor>
{
    TestTargetExtractor() { }

    void testAprioriValues()
    {

        TargetExtractor extractor;


        CovariateSeries covariates;
        TargetDefinitions targetDefinitions;
        Targets targets;
        DateTime start;
        DateTime end;
        TargetExtractionOption extractionOption;
        TargetSeries series;

        ComputingResult result;

        {
            extractionOption = TargetExtractionOption::AprioriValues;

            result = extractor.extract(ActiveMoietyId("NoId"), covariates, targetDefinitions, targets, start, end, extractionOption, series);

            // Not yet implemented, so extraction error
            fructose_assert( result == ComputingResult::TargetExtractionError);
        }
    }

    void testPopulationValues()
    {

        TargetExtractor extractor;


        CovariateSeries covariates;
        TargetDefinitions targetDefinitions;
        Targets targets;
        DateTime start;
        DateTime end;
        TargetExtractionOption extractionOption;
        TargetSeries series;

        ComputingResult result;

        {
            // Test with only population values

            // Add targets
            std::unique_ptr<SubTargetDefinition> cMin(new SubTargetDefinition("cMin", 750.0, nullptr));
            std::unique_ptr<SubTargetDefinition> cMax(new SubTargetDefinition("cMax", 1500.0, nullptr));
            std::unique_ptr<SubTargetDefinition> cBest(new SubTargetDefinition("cBest", 1000.0, nullptr));
            TargetDefinition *target = new TargetDefinition(TargetType::Residual,
                                                            Unit("ug/l"),
                                                            ActiveMoietyId("imatinib"),
                                                            std::move(cMin),
                                                            std::move(cMax),
                                                            std::move(cBest),
                                                            std::unique_ptr<SubTargetDefinition>(new SubTargetDefinition("mic", 2.0, nullptr)),
                                                            std::unique_ptr<SubTargetDefinition>(new SubTargetDefinition("tMin", 1000.0, nullptr)),
                                                            std::unique_ptr<SubTargetDefinition>(new SubTargetDefinition("tMax", 1200.0, nullptr)),
                                                            std::unique_ptr<SubTargetDefinition>(new SubTargetDefinition("tBest", 1100.0, nullptr)),
                                                            std::unique_ptr<SubTargetDefinition>(new SubTargetDefinition("toxicity", 10000.0, nullptr)),
                                                            std::unique_ptr<SubTargetDefinition>(new SubTargetDefinition("inefficacy", 1.0, nullptr)));


            targetDefinitions.push_back(std::unique_ptr<TargetDefinition>(target));

            extractionOption = TargetExtractionOption::PopulationValues;
            result = extractor.extract(ActiveMoietyId("imatinib"), covariates, targetDefinitions, targets, start, end, extractionOption, series);
            fructose_assert( result == ComputingResult::Ok);

            fructose_assert(series.size() == 1);
            {
                Target targetResult = series[0];

                fructose_assert( targetResult.m_valueMin == 750.0);
                fructose_assert( targetResult.m_valueMax == 1500.0);
                fructose_assert( targetResult.m_valueBest == 1000.0);
                fructose_assert( targetResult.m_mic == 2.0);
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
        DateTime start;
        DateTime end;
        TargetExtractionOption extractionOption;
        TargetSeries series;

        ComputingResult result;


        {
            // Test with individual values

            // Add targets
            std::unique_ptr<SubTargetDefinition> cMin(new SubTargetDefinition("cMin", 750.0, nullptr));
            std::unique_ptr<SubTargetDefinition> cMax(new SubTargetDefinition("cMax", 1500.0, nullptr));
            std::unique_ptr<SubTargetDefinition> cBest(new SubTargetDefinition("cBest", 1000.0, nullptr));
            TargetDefinition *target = new TargetDefinition(TargetType::Residual,
                                                            Unit("mg/l"),
                                                            ActiveMoietyId("imatinib"),
                                                            std::move(cMin),
                                                            std::move(cMax),
                                                            std::move(cBest),
                                                            std::unique_ptr<SubTargetDefinition>(new SubTargetDefinition("mic", 2.0, nullptr)),
                                                            std::unique_ptr<SubTargetDefinition>(new SubTargetDefinition("tMin", 1000.0, nullptr)),
                                                            std::unique_ptr<SubTargetDefinition>(new SubTargetDefinition("tMax", 1200.0, nullptr)),
                                                            std::unique_ptr<SubTargetDefinition>(new SubTargetDefinition("tBest", 1100.0, nullptr)),
                                                            std::unique_ptr<SubTargetDefinition>(new SubTargetDefinition("toxicity", 10000.0, nullptr)),
                                                            std::unique_ptr<SubTargetDefinition>(new SubTargetDefinition("inefficacy", 1.0, nullptr)));


            targetDefinitions.push_back(std::unique_ptr<TargetDefinition>(target));


            Target *patientTarget = new Target(ActiveMoietyId("imatinib"), TargetType::Residual, Unit("mg/l"), 50.0, 100.0, 150.0, 2.0, 200.0);

            targets.push_back(std::unique_ptr<Target>(patientTarget));

            extractionOption = TargetExtractionOption::IndividualTargets;
            result = extractor.extract(ActiveMoietyId("imatinib"), covariates, targetDefinitions, targets, start, end, extractionOption, series);
            fructose_assert( result == ComputingResult::Ok);

            fructose_assert(series.size() == 1);
            {
                Target targetResult = series[0];

                // Here the values are x1000 the target values, as the units in the definition are mg/l. The final
                // target unit is in ug/l
                fructose_assert( targetResult.m_valueMin == 50000.0);
                fructose_assert( targetResult.m_valueMax == 150000.0);
                fructose_assert( targetResult.m_valueBest == 100000.0);
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
        DateTime start;
        DateTime end;
        TargetExtractionOption extractionOption;
        TargetSeries series;

        ComputingResult result;


        {
            // Test with individual values

            // Add targets

            Target *patientTarget = new Target(ActiveMoietyId("imatinib"), TargetType::Residual, Unit("mg/l"), 50.0, 100.0, 150.0, 2.0, 200.0);

            targets.push_back(std::unique_ptr<Target>(patientTarget));

            extractionOption = TargetExtractionOption::DefinitionIfNoIndividualTarget;
            result = extractor.extract(ActiveMoietyId("imatinib"), covariates, targetDefinitions, targets, start, end, extractionOption, series);
            fructose_assert( result == ComputingResult::Ok);

            fructose_assert(series.size() == 1);
            {
                Target targetResult = series[0];

                // Here the values are x1000 the target values, as the units in the definition are mg/l. The final
                // target unit is in ug/l
                fructose_assert( targetResult.m_valueMin == 50000.0);
                fructose_assert( targetResult.m_valueMax == 150000.0);
                fructose_assert( targetResult.m_valueBest == 100000.0);
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
