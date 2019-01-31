#ifndef TUCUXI_CORE_COMPUTINGRESPONSEEXPORT_H
#define TUCUXI_CORE_COMPUTINGRESPONSEEXPORT_H

#include "tucucore/computingservice/computingresponse.h"

namespace Tucuxi {
namespace Core {

class ComputingResponseExport
{
public:
    ComputingResponseExport();

    bool exportToFiles(const ComputingResponse &computingResponse, std::string filePath);
};

} // namespace Core
} // namespace Tucuxi


#endif // TUCUXI_CORE_COMPUTINGRESPONSEEXPORT_H
