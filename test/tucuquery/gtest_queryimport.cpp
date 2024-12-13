#include <gtest/gtest.h>

#include "tucucore/definitions.h"

#include "tucuquery/queryimport.h"

#include "gtest_queryinputstrings.h"

TEST(Query_TestQueryImport, TestInvalidRank)
{
    Tucuxi::Query::QueryImport importer;
    std::unique_ptr<Tucuxi::Query::QueryData> data;
    Tucuxi::Query::QueryImport::Status importResult = importer.importFromString(data, tqf_invalid_rank);


    auto computingTrait =
            dynamic_cast<Tucuxi::Core::ComputingTraitPercentiles&>(data->getRequests()[0]->getpComputingTrait());

    ASSERT_FLOAT_EQ(computingTrait.getRanks()[0], Tucuxi::Core::PERCENTILE_RANK_MIN);
    ASSERT_FLOAT_EQ(computingTrait.getRanks()[1], Tucuxi::Core::PERCENTILE_RANK_MIN);
    ASSERT_FLOAT_EQ(computingTrait.getRanks()[2], Tucuxi::Core::PERCENTILE_RANK_MAX);
    ASSERT_FLOAT_EQ(computingTrait.getRanks()[3], Tucuxi::Core::PERCENTILE_RANK_MAX);
}
