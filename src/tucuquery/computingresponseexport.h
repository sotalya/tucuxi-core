#ifndef TUCUXI_CORE_COMPUTINGRESPONSEEXPORT_H
#define TUCUXI_CORE_COMPUTINGRESPONSEEXPORT_H

#include "tucucore/computingservice/computingresponse.h"
#include "tucuquery/computingqueryresponse.h"

namespace Tucuxi {
namespace Core {

class ComputingResponseExport
{
public:
    ComputingResponseExport();

    bool exportToFiles(const Query::ComputingQueryResponse &_computingQueryResponse, std::string _filePath);
};

} // namespace Core
} // namespace Tucuxi


#endif // TUCUXI_CORE_COMPUTINGRESPONSEEXPORT_H
