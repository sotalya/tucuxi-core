#include "querycomputer.h"


namespace Tucuxi {
namespace Query {

QueryComputer::QueryComputer()
{

}

void QueryComputer::compute(ComputingQuery& _query, ComputingQueryResponse& _response)
{
    // The ComputingRequest objects of the _query

}

void QueryComputer::compute(std::string _queryString, ComputingQueryResponse& _response)
{
    // First build the ComputingQuery object

    // 1. Build the QueryData from _queryString

    // 2. Build a ComputingQuery from QueryData
    //    This implies getting a drugmodel for each request from the drugmodelrepository

    // Then call compute(query, _response);

}


} // namespace Query
} // namespace Tucuxi

