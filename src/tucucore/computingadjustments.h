#ifndef TUCUXI_CORE_COMPUTINGADJUSTMENTS_H
#define TUCUXI_CORE_COMPUTINGADJUSTMENTS_H

#include "tucucommon/loggerhelper.h"

#include "tucucore/computingservice/computingresponse.h"
#include "tucucore/computingservice/computingtrait.h"
#include "tucucore/computingservice/computingresult.h"
#include "tucucore/parameter.h"

namespace Tucuxi {
namespace Core {

class FullFormulationAndRoute;
class ComputingComponent;
class PkModel;

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


    DosageTimeRange *createSteadyStateDosage(
            AdjustmentCandidate &_candidate,
            DateTime _startTime,
            FormulationAndRoute _routeOfAdministration);

    ComputingResult addLoadOrRest(std::vector<FullDosage> &_dosages);
    ComputingResult addLoadOrRest(FullDosage &_dosage);

    ComputingResult generatePredictions(std::vector<FullDosage> &_dosages,
                                        const ComputingTraitAdjustment *_traits,
                                        const ComputingRequest &_request,
                                        const std::vector<AnalyteGroupId> &allGroupIds,
                                        const DateTime calculationStartTime,
                                        std::map<AnalyteGroupId, std::shared_ptr<PkModel> > &pkModel,
                                        GroupsParameterSetSeries &parameterSeries,
                                        std::map<AnalyteGroupId, Etas> &etas
                                        );


    ComputingResult generatePrediction(FullDosage &_dosage,
                                       const ComputingTraitAdjustment *_traits,
                                       const ComputingRequest &_request,
                                       const std::vector<AnalyteGroupId> &allGroupIds,
                                       const DateTime calculationStartTime,
                                       std::map<AnalyteGroupId, std::shared_ptr<PkModel> > &pkModel,
                                       GroupsParameterSetSeries &parameterSeries,
                                       std::map<AnalyteGroupId, Etas> &etas
                                       );

protected:

    Tucuxi::Common::LoggerHelper m_logger;
    ComputingComponent *m_computingComponent;

};

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_COMPUTINGADJUSTMENTS_H
