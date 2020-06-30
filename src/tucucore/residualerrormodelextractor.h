#ifndef TUCUXI_CORE_RESIDUALERRORMODELEXTRACTOR_H
#define TUCUXI_CORE_RESIDUALERRORMODELEXTRACTOR_H

#include "tucucommon/unit.h"

#include "tucucore/covariateevent.h"
#include "tucucore/sampleevent.h"
#include "tucucore/computingservice/computingresult.h"

namespace Tucuxi {
namespace Core {

class ErrorModel;
class IResidualErrorModel;

class ResidualErrorModelExtractor
{
public:

    ResidualErrorModelExtractor();

    ComputingStatus extract(
            const ErrorModel &_errorModel,
            const Common::Unit &_fromUnit,
            const CovariateSeries &_covariateSeries,
            std::unique_ptr<IResidualErrorModel> &_residualErrorModel);

};

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_RESIDUALERRORMODELEXTRACTOR_H
