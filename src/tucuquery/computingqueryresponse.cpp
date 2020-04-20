#include "computingqueryresponse.h"


namespace Tucuxi {
namespace Query {


SingleResponseData::SingleResponseData(
        std::unique_ptr<Tucuxi::Core::SingleComputingResponse> _computingResponse,
        Tucuxi::Core::ComputingResult _status,
        std::unique_ptr<ComputingResponseMetaData> _metaData) :
    m_computingResponse(std::move(_computingResponse)),
    m_status(_status),
    m_metaData(std::move(_metaData))
{
}

Tucuxi::Core::RequestResponseId ComputingQueryResponse::getQueryId() const
{
    return m_queryId;
}

const std::vector<SingleResponseData>& ComputingQueryResponse::getRequestResponses() const
{
    return m_requestResponses;
}


void ComputingQueryResponse::addRequestResponse(
        std::unique_ptr<Tucuxi::Core::SingleComputingResponse> _computingResponse,
        Tucuxi::Core::ComputingResult _status,
        std::unique_ptr<ComputingResponseMetaData> _metaData)
{
    m_requestResponses.push_back(SingleResponseData(std::move(_computingResponse),
                                  _status,
                                  std::move(_metaData)));
}

} // namespace Query
} // namespace Tucuxi
