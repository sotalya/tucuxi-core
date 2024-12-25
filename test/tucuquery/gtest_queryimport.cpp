#include <gtest/gtest.h>

#include "tucuquery/queryimport.h"

#include "gtest_queryinputstrings.h"

TEST(Query_TestQueryImport, TestInvalidRank)
{
    Tucuxi::Query::QueryImport importer;
    std::unique_ptr<Tucuxi::Query::QueryData> data;
    Tucuxi::Query::QueryImport::Status importResult = importer.importFromString(data, tqf_invalid_rank);

    ASSERT_EQ(importResult, Tucuxi::Query::QueryImport::Status::Error);
}
