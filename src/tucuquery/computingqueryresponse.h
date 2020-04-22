#ifndef COMPUTINGQUERYRESPONSE_H
#define COMPUTINGQUERYRESPONSE_H

#include "tucucore/computingservice/computingresult.h"
#include "tucucore/computingservice/computingresponse.h"

namespace Tucuxi {
namespace Query {

///
/// \brief The QueryResponseMetaData class
/// This class will embed information such as computation time for a ComputingQuery
///
class QueryResponseMetaData
{

};

///
/// \brief The ComputingResponseMetaData class
/// This class will embed information such as the drug model ID
/// used for computation, and the computation time, for a single ComputingRequest
///
class ComputingResponseMetaData
{
public:
    ComputingResponseMetaData(std::string drugModelId);

private:
    std::string m_drugModelId;
};

class SingleResponseData {

public:

    SingleResponseData(std::unique_ptr<Tucuxi::Core::ComputingResponse> _computingResponse,
                       std::unique_ptr<ComputingResponseMetaData> _metaData);

    std::unique_ptr<Tucuxi::Core::ComputingResponse> m_computingResponse;
    std::unique_ptr<ComputingResponseMetaData> m_metaData;
};

///
/// \brief The ComputingQueryResponse class
/// This class contains all the responses of a ComputingQuery.
/// It can take advantage of ComputingResponse class, but should also embed
/// information about the drug model Id used (for instance)
///
class ComputingQueryResponse
{
public:
    ComputingQueryResponse();

    Tucuxi::Core::RequestResponseId getQueryId() const;
    const std::vector<SingleResponseData>& getRequestResponses() const;

    void addRequestResponse(std::unique_ptr<Tucuxi::Core::ComputingResponse> _computingResponse,
                            std::unique_ptr<ComputingResponseMetaData> _metaData);
    void setRequestResponseId(Tucuxi::Core::RequestResponseId _requestResponseId);

    Tucuxi::Core::RequestResponseId m_queryId;

    Tucuxi::Core::ComputingStatus m_queryStatus;

    std::vector<SingleResponseData> m_requestResponses;

    std::unique_ptr<QueryResponseMetaData> m_metaData;
};


} // namespace Query
} // namespace Tucuxi



#endif // COMPUTINGQUERYRESPONSE_H
