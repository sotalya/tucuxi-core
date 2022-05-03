//@@license@@

#ifndef QUERY_H
#define QUERY_H

#include <memory>
#include <string>
#include <vector>

namespace Tucuxi {

namespace Core {
class ComputingRequest;
}

namespace Query {



class ComputingQuery
{
public:
    ComputingQuery(std::string _queryId);

    void addComputingRequest(std::unique_ptr<Tucuxi::Core::ComputingRequest> _computingRequest);

    std::string m_queryId;

    std::vector<std::unique_ptr<Tucuxi::Core::ComputingRequest> > m_computingRequests;
};


} // namespace Query
} // namespace Tucuxi


#endif // QUERY_H
