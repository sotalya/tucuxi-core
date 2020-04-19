#ifndef COMPUTINGQUERYRESPONSE_H
#define COMPUTINGQUERYRESPONSE_H

#include "tucucore/computingservice/computingresult.h"
#include "tucucore/computingservice/computingresponse.h"

namespace Tucuxi {
namespace Query {

///
/// \brief The QueryResponseMetaData class
/// This class will embed information such as computation time
///
class QueryResponseMetaData
{

};

class SingleResponseData {

public:

    Tucuxi::Core::ComputingResult m_status;
    QueryResponseMetaData m_metaData;
    std::unique_ptr<Tucuxi::Core::SingleComputingResponse> m_computingResponse;
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

    Tucuxi::Core::ComputingResult m_queryStatus;

    std::vector<SingleResponseData> m_requestResponses;
};


} // namespace Query
} // namespace Tucuxi



#endif // COMPUTINGQUERYRESPONSE_H
