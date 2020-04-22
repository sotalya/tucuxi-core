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


ComputingQueryResponse::ComputingQueryResponse()
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
        std::unique_ptr<Core::SingleComputingResponse> _computingResponse,
        Tucuxi::Core::ComputingResult _status,
        std::unique_ptr<ComputingResponseMetaData> _metaData)
{
    m_requestResponses.push_back(SingleResponseData(std::move(_computingResponse),
                                  _status,
                                  std::move(_metaData)));
}

void ComputingQueryResponse::setRequestResponseId(Tucuxi::Core::RequestResponseId _requestResponseId)
{
   m_queryId =_requestResponseId;
}

ComputingResponseMetaData::ComputingResponseMetaData(
        std::string _drugModelID ) :
    m_drugModelId(_drugModelID)
{

}



} // namespace Query
} // namespace Tucuxi
