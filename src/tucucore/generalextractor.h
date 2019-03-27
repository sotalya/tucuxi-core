#ifndef TUCUXI_CORE_GENERALEXTRACTOR_H
#define TUCUXI_CORE_GENERALEXTRACTOR_H

#include "tucucommon/datetime.h"
#include "tucucommon/loggerhelper.h"

#include "tucucore/computingservice/icomputingservice.h"
#include "tucucore/definitions.h"
#include "tucucore/dosage.h"
#include "tucucore/covariateevent.h"

namespace Tucuxi {
namespace Core {

class ComputingRequest;
class DrugModel;
class ComputingTrait;
class ParameterSeries;
class ComputingTraitStandard;
class PkModel;
class HalfLife;
class PkModelCollection;

class GeneralExtractor
{
public:
    GeneralExtractor();

    Duration secureStartDuration(const HalfLife &_halfLife);

    ComputingResult extractAposterioriEtas(Etas &_etas, const ComputingRequest &_request, const Tucuxi::Core::IntakeSeries &_intakeSeries, const ParameterSetSeries &_parameterSeries, const Tucuxi::Core::CovariateSeries &_covariateSeries, Tucuxi::Common::DateTime _calculationStartTime, Common::DateTime _endTime);

    ComputationResult extractOmega(
        const DrugModel &_drugModel,
        OmegaMatrix &_omega);


    ComputingResult generalExtractions(
            const Tucuxi::Core::ComputingTraitStandard *_traits,
            const ComputingRequest &_request,
            const PkModelCollection *_modelCollection,
            std::shared_ptr<PkModel> &_pkModel,
            IntakeSeries &_intakeSeries,
            CovariateSeries &_covariatesSeries,
            ParameterSetSeries &_parameterSeries,
            Common::DateTime &_calculationStartTime);

protected:
    bool findFormulationAndRoutes(std::vector<FormulationAndRoute> &_treatmentFandR, const FormulationAndRoutes &_drugModelFandR, std::map<FormulationAndRoute, const FullFormulationAndRoute *> &_result);

    Tucuxi::Common::LoggerHelper m_logger;
};

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_GENERALEXTRACTOR_H
