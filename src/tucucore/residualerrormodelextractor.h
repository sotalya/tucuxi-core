#ifndef TUCUXI_CORE_RESIDUALERRORMODELEXTRACTOR_H
#define TUCUXI_CORE_RESIDUALERRORMODELEXTRACTOR_H

#include "tucucore/covariateevent.h"
#include "tucucore/sampleevent.h"
#include "tucucore/unit.h"

namespace Tucuxi {
namespace Core {

class ErrorModel;
class IResidualErrorModel;

class ResidualErrorModelExtractor
{
public:

    enum class Result {
        Ok,
        ExtractionError
    };

    ResidualErrorModelExtractor();

    Result extract(const ErrorModel &_errorModel, const Unit &_fromUnit, const CovariateSeries &_covariateSeries, IResidualErrorModel **_residualErrorModel);

};

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_RESIDUALERRORMODELEXTRACTOR_H
