#ifndef COMPUTINGQUERYRESPONSE_H
#define COMPUTINGQUERYRESPONSE_H


namespace Tucuxi {
namespace Query {

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
};


} // namespace Query
} // namespace Tucuxi



#endif // COMPUTINGQUERYRESPONSE_H
