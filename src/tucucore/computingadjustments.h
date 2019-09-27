#ifndef TUCUXI_CORE_COMPUTINGADJUSTMENTS_H
#define TUCUXI_CORE_COMPUTINGADJUSTMENTS_H

#include "tucucommon/loggerhelper.h"

#include "tucucore/computingservice/computingresponse.h"
#include "tucucore/computingservice/computingtrait.h"
#include "tucucore/computingservice/computingresult.h"

namespace Tucuxi {
namespace Core {

class FullFormulationAndRoute;
class ComputingComponent;

class ComputingAdjustments
{
public:
    ComputingAdjustments(ComputingComponent *_computingComponent);


    ComputingResult compute(
            const ComputingTraitAdjustment *_traits,
            const ComputingRequest &_request,
            std::unique_ptr<ComputingResponse> &_response);


    std::vector<FullDosage> sortAndFilterCandidates(std::vector<FullDosage> &_candidates, BestCandidatesOption _option);

    typedef struct {
        Value m_dose;
        Duration m_interval;
        Duration m_infusionTime;
    } AdjustmentCandidate;


    ComputingResult buildCandidates(const FullFormulationAndRoute* _formulationAndRoute, std::vector<AdjustmentCandidate> &_candidates);

    DosageTimeRange *createDosage(
            AdjustmentCandidate &_candidate,
            DateTime _startTime,
            DateTime _endTime,
            FormulationAndRoute _routeOfAdministration);

protected:

    Tucuxi::Common::LoggerHelper m_logger;
    ComputingComponent *m_computingComponent;

};

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_COMPUTINGADJUSTMENTS_H
