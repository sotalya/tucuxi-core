#ifndef QUERYCOMPUTER_H
#define QUERYCOMPUTER_H

#include "tucuquery/computingquery.h"
#include "tucuquery/computingqueryresponse.h"

namespace Tucuxi {
namespace Query {


class QueryComputer
{
public:
    QueryComputer();

    ///
    /// \brief computes a full query, and populates the responses
    /// \param _query The query object, embedded all information
    /// \param _response The query response
    /// The list of responses embedded in the query response correspond to the
    /// ComputingRequest embedded in _query
    ///
    void compute(ComputingQuery& _query, ComputingQueryResponse& _response);

    ///
    /// \brief computes a full query, and populates the responses
    /// \param _queryString The XML string representing the query
    /// \param _response The query response
    /// The list of responses embedded in the query response correspond to the
    /// ComputingRequest embedded in _queryString
    ///
    QueryStatus compute(const std::string& _queryString, ComputingQueryResponse& _response);

};


} // namespace Query
} // namespace Tucuxi


#endif // QUERYCOMPUTER_H
