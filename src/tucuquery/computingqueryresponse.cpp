#include <utility>

#include "computingqueryresponse.h"

#include "tucucore/computingservice/computingresponse.h"

namespace Tucuxi {
namespace Query {

SingleResponseData::SingleResponseData(
        std::unique_ptr<Tucuxi::Core::ComputingResponse> _computingResponse,
        std::unique_ptr<ComputingResponseMetaData> _metaData)
    : m_computingResponse(std::move(_computingResponse)), m_metaData(std::move(_metaData))
{
}


ComputingQueryResponse::ComputingQueryResponse() = default;

Tucuxi::Core::RequestResponseId ComputingQueryResponse::getQueryId() const
{
    return m_queryId;
}

const std::vector<SingleResponseData>& ComputingQueryResponse::getRequestResponses() const
{
    return m_requestResponses;
}

QueryStatus ComputingQueryResponse::getQueryStatus() const
{
    return m_queryStatus;
}

std::string ComputingQueryResponse::getErrorMessage() const
{
    return m_errorMessage;
}

void ComputingQueryResponse::setQueryStatus(QueryStatus _queryStatus)
{
    m_queryStatus = _queryStatus;
}

void ComputingQueryResponse::setQueryStatus(QueryStatus _queryStatus, std::string _errorMessage)
{
    m_queryStatus = _queryStatus;
    m_errorMessage = std::move(_errorMessage);
}

void ComputingQueryResponse::addRequestResponse(
        std::unique_ptr<Core::ComputingResponse> _computingResponse,
        std::unique_ptr<ComputingResponseMetaData> _metaData)
{
    m_requestResponses.emplace_back(std::move(_computingResponse), std::move(_metaData));
}

void ComputingQueryResponse::setRequestResponseId(Tucuxi::Core::RequestResponseId _requestResponseId)
{
    m_queryId = std::move(_requestResponseId);
}

ComputingResponseMetaData::ComputingResponseMetaData(std::string _drugModelID) : m_drugModelId(std::move(_drugModelID))
{
}



} // namespace Query
} // namespace Tucuxi
