#include "computingquery.h"

#include "tucucore/computingservice/computingrequest.h"

namespace Tucuxi {
namespace Query {



ComputingQuery::ComputingQuery(std::string _queryId) : m_queryId(_queryId)
{
}

void ComputingQuery::addComputingRequest(std::unique_ptr<Tucuxi::Core::ComputingRequest> _computingRequest)
{
    m_computingRequests.push_back(std::move(_computingRequest));
}


} // namespace Query
} // namespace Tucuxi
