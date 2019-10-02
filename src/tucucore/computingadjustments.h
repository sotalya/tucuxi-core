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


    ComputingResult buildCandidates(const FullFormulationAndRoute* _formulationAndRoute,
                                    std::vector<AdjustmentCandidate> &_candidates);

    ComputingResult buildCandidatesForInterval(const FullFormulationAndRoute* _formulationAndRoute,
                                               Common::Duration _interval,
                                               std::vector<ComputingAdjustments::AdjustmentCandidate> &_candidates);

    DosageTimeRange *createDosage(
            const AdjustmentCandidate &_candidate,
            DateTime _startTime,
            DateTime _endTime,
            FormulationAndRoute _routeOfAdministration);


    DosageTimeRange *createLoadingDosage(const AdjustmentCandidate &_candidate,
            DateTime _startTime,
            FormulationAndRoute _routeOfAdministration);


    DosageTimeRange *createSteadyStateDosage(
            const AdjustmentCandidate &_candidate,
            DateTime _startTime,
            FormulationAndRoute _routeOfAdministration);

    ComputingResult addLoadOrRest(std::vector<FullDosage> &_dosages,
                                  const ComputingTraitAdjustment *_traits,
                                  const ComputingRequest &_request,
                                  const std::vector<AnalyteGroupId> &_allGroupIds,
                                  const DateTime _calculationStartTime,
                                  std::map<AnalyteGroupId, std::shared_ptr<PkModel> > &_pkModel,
                                  GroupsParameterSetSeries &_parameterSeries,
                                  std::map<AnalyteGroupId, Etas> &_etas);

    ComputingResult addLoadOrRest(FullDosage &_dosage,
                                  const ComputingTraitAdjustment *_traits,
                                  const ComputingRequest &_request,
                                  const std::vector<AnalyteGroupId> &_allGroupIds,
                                  const DateTime _calculationStartTime,
                                  std::map<AnalyteGroupId, std::shared_ptr<PkModel> > &_pkModel,
                                  GroupsParameterSetSeries &_parameterSeries,
                                  std::map<AnalyteGroupId, Etas> &_etas);

    ComputingResult generatePredictions(std::vector<FullDosage> &_dosages,
                                        const ComputingTraitAdjustment *_traits,
                                        const ComputingRequest &_request,
                                        const std::vector<AnalyteGroupId> &_allGroupIds,
                                        const DateTime _calculationStartTime,
                                        std::map<AnalyteGroupId, std::shared_ptr<PkModel> > &_pkModel,
                                        GroupsParameterSetSeries &_parameterSeries,
                                        std::map<AnalyteGroupId, Etas> &_etas
                                        );


    ComputingResult generatePrediction(FullDosage &_dosage,
                                       const ComputingTraitAdjustment *_traits,
                                       const ComputingRequest &_request,
                                       const std::vector<AnalyteGroupId> &_allGroupIds,
                                       const DateTime _calculationStartTime,
                                       std::map<AnalyteGroupId, std::shared_ptr<PkModel> > &_pkModel,
                                       GroupsParameterSetSeries &_parameterSeries,
                                       std::map<AnalyteGroupId, Etas> &_etas
                                       );

protected:

    Tucuxi::Common::LoggerHelper m_logger;
    ComputingComponent *m_computingComponent;

};

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_COMPUTINGADJUSTMENTS_H
