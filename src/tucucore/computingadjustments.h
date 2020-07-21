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
class ComputingUtils;
class PkModel;

class ComputingAdjustments
{
public:
    ComputingAdjustments(ComputingUtils *_computingUtils);


    ComputingStatus compute(
            const ComputingTraitAdjustment *_traits,
            const ComputingRequest &_request,
            std::unique_ptr<ComputingResponse> &_response);

protected:

    ///
    /// \brief sortAndFilterCandidates
    /// \param _candidates All valid candidates for which a score has been calculated
    /// \param _option Options for selecting and filter the candidates.
    /// \return A vector of selected and filtered candidates
    /// The candidates will be sorted thanks to their respective score. The best score is the first
    /// in the returned vector.
    /// The filtering is made depending on _option. It can:
    /// - Return all valid candidates
    /// - Return only the best candidate
    /// - Return the best valid candidate for each interval
    ///
    std::vector<DosageAdjustment> sortAndFilterCandidates(std::vector<DosageAdjustment> &_candidates, BestCandidatesOption _option);


    /// Structure representing dosing candidates in terms of dose, interval and infusion time
    typedef struct {
        /// Formulation and route of the candidate
        FormulationAndRoute m_formulationAndRoute;
        /// The dose value
        Value m_dose;
        /// The dose unit
        Unit m_doseUnit;
        /// The dosing interval
        Duration m_interval;
        /// The infusion time. 0 means no infusion for a model without infusion.
        Duration m_infusionTime;
    } SimpleDosageCandidate;


    ComputingStatus buildCandidates(const FullFormulationAndRoute* _formulationAndRoute,
                                    std::vector<SimpleDosageCandidate> &_candidates);

    ComputingStatus buildCandidatesForInterval(const FullFormulationAndRoute* _formulationAndRoute,
                                               Common::Duration _interval,
                                               std::vector<ComputingAdjustments::SimpleDosageCandidate> &_candidates);

    DosageTimeRange *createDosage(
            const SimpleDosageCandidate &_candidate,
            DateTime _startTime,
            DateTime _endTime);


    DosageTimeRange *createLoadingDosageOrRestPeriod(const SimpleDosageCandidate &_candidate,
            DateTime _startTime);

    DosageTimeRange *createSteadyStateDosage(
            const SimpleDosageCandidate &_candidate,
            DateTime _startTime);

    ComputingStatus addLoadOrRest(std::vector<DosageAdjustment> &_dosages,
                                  const ComputingTraitAdjustment *_traits,
                                  const ComputingRequest &_request,
                                  const std::vector<AnalyteGroupId> &_allGroupIds,
                                  const DateTime &_calculationStartTime,
                                  std::map<AnalyteGroupId, std::shared_ptr<PkModel> > &_pkModel,
                                  GroupsParameterSetSeries &_parameterSeries,
                                  std::map<AnalyteGroupId, Etas> &_etas);

    ComputingStatus addLoadOrRest(DosageAdjustment &_dosage,
                                  const ComputingTraitAdjustment *_traits,
                                  const ComputingRequest &_request,
                                  const std::vector<AnalyteGroupId> &_allGroupIds,
                                  const DateTime &_calculationStartTime,
                                  std::map<AnalyteGroupId, std::shared_ptr<PkModel> > &_pkModel,
                                  GroupsParameterSetSeries &_parameterSeries,
                                  std::map<AnalyteGroupId, Etas> &_etas);

    ComputingStatus addLoad(DosageAdjustment &_dosage,
                            const ComputingTraitAdjustment *_traits,
                            const ComputingRequest &_request,
                            const std::vector<AnalyteGroupId> &_allGroupIds,
                            const DateTime &_calculationStartTime,
                            std::map<AnalyteGroupId, std::shared_ptr<PkModel> > &_pkModel,
                            GroupsParameterSetSeries &_parameterSeries,
                            std::map<AnalyteGroupId, Etas> &_etas,
                            bool &_modified);

    ComputingStatus addRest(DosageAdjustment &_dosage,
                            const ComputingTraitAdjustment *_traits,
                            const ComputingRequest &_request,
                            const std::vector<AnalyteGroupId> &_allGroupIds,
                            const DateTime &_calculationStartTime,
                            std::map<AnalyteGroupId, std::shared_ptr<PkModel> > &_pkModel,
                            GroupsParameterSetSeries &_parameterSeries,
                            std::map<AnalyteGroupId, Etas> &_etas,
                            bool &_modified);

    ComputingStatus generatePredictions(std::vector<DosageAdjustment> &_dosages,
                                        const ComputingTraitAdjustment *_traits,
                                        const ComputingRequest &_request,
                                        const std::vector<AnalyteGroupId> &_allGroupIds,
                                        const Common::DateTime &_calculationStartTime,
                                        std::map<AnalyteGroupId, std::shared_ptr<PkModel> > &_pkModel,
                                        GroupsParameterSetSeries &_parameterSeries,
                                        std::map<AnalyteGroupId, Etas> &_etas
                                        );


    ComputingStatus generatePrediction(DosageAdjustment &_dosage,
                                       const ComputingTraitAdjustment *_traits,
                                       const ComputingRequest &_request,
                                       const std::vector<AnalyteGroupId> &_allGroupIds,
                                       const DateTime &_calculationStartTime,
                                       std::map<AnalyteGroupId, std::shared_ptr<PkModel> > &_pkModel,
                                       GroupsParameterSetSeries &_parameterSeries,
                                       std::map<AnalyteGroupId, Etas> &_etas
                                       );

protected:

    Tucuxi::Common::LoggerHelper m_logger;
    ComputingUtils *m_utils;

};

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_COMPUTINGADJUSTMENTS_H
