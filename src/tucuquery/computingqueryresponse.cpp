#include "computingqueryresponse.h"


namespace Tucuxi {
namespace Query {


Tucuxi::Core::RequestResponseId ComputingQueryResponse::getQueryId() const
{
    return m_queryId;
}

const std::vector<SingleResponseData>& ComputingQueryResponse::getRequestResponses() const
{
    return m_requestResponses;
}

} // namespace Query
} // namespace Tucuxi
