/*
 * Tucuxi - Tucuxi-core library and command line tool.
 * This code allows to perform prediction of drug concentration in blood
 * and to propose dosage adaptations.
 * It has been developed by HEIG-VD, in close collaboration with CHUV.
 * Copyright (C) 2023 HEIG-VD, maintained by Yann Thoma  <yann.thoma@heig-vd.ch>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */


#ifndef TUCUXI_CORE_COMPUTINGADJUSTMENTS_H
#define TUCUXI_CORE_COMPUTINGADJUSTMENTS_H

#include "tucucommon/loggerhelper.h"

#include "tucucore/computingservice/computingresponse.h"
#include "tucucore/computingservice/computingresult.h"
#include "tucucore/computingservice/computingtrait.h"
#include "tucucore/concentrationprediction.h"
#include "tucucore/parameter.h"

namespace Tucuxi {
namespace Core {

class FullFormulationAndRoute;
class ComputingUtils;
class PkModel;

class ComputingAdjustments
{
public:
    ComputingAdjustments(ComputingUtils* _computingUtils);


    [[nodiscard]] ComputingStatus compute(
            const ComputingTraitAdjustment* _traits,
            const ComputingRequest& _request,
            std::unique_ptr<ComputingResponse>& _response);

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
    std::vector<DosageAdjustment> sortAndFilterCandidates(
            std::vector<DosageAdjustment>& _candidates, BestCandidatesOption _option);


    /// Structure representing dosing candidates in terms of dose, interval and infusion time
    typedef struct SimpleDosageCandidateStruct
    {
        /// Formulation and route of the candidate
        FormulationAndRoute m_formulationAndRoute;
        /// The dose value
        Value m_dose{};
        /// The dose unit
        TucuUnit m_doseUnit;
        /// The dosing interval
        Duration m_interval;
        /// The infusion time. 0 means no infusion for a model without infusion.
        Duration m_infusionTime;
    } SimpleDosageCandidate;


    [[nodiscard]] ComputingStatus buildCandidates(
            const FullFormulationAndRoute* _formulationAndRoute, std::vector<SimpleDosageCandidate>& _candidates);

    [[nodiscard]] ComputingStatus buildCandidatesForInterval(
            const FullFormulationAndRoute* _formulationAndRoute,
            const Common::Duration& _interval,
            std::vector<ComputingAdjustments::SimpleDosageCandidate>& _candidates);

    [[nodiscard]] std::unique_ptr<DosageTimeRange> createDosage(
            const SimpleDosageCandidate& _candidate, const DateTime& _startTime, const DateTime& _endTime);


    [[nodiscard]] std::unique_ptr<DosageTimeRange> createLoadingDosageOrRestPeriod(
            const SimpleDosageCandidate& _candidate, const DateTime& _startTime);

    [[nodiscard]] std::unique_ptr<DosageTimeRange> createSteadyStateDosage(
            const SimpleDosageCandidate& _candidate, const DateTime& _startTime);

    [[nodiscard]] ComputingStatus addLoadOrRest(
            std::vector<DosageAdjustment>& _dosages,
            const ComputingTraitAdjustment* _traits,
            const ComputingRequest& _request,
            const std::vector<AnalyteGroupId>& _allGroupIds,
            const DateTime& _calculationStartTime,
            std::map<AnalyteGroupId, std::shared_ptr<PkModel> >& _pkModel,
            GroupsParameterSetSeries& _parameterSeries,
            std::map<AnalyteGroupId, Etas>& _etas);

    [[nodiscard]] ComputingStatus addLoadOrRest(
            DosageAdjustment& _dosage,
            const ComputingTraitAdjustment* _traits,
            const ComputingRequest& _request,
            const std::vector<AnalyteGroupId>& _allGroupIds,
            const DateTime& _calculationStartTime,
            std::map<AnalyteGroupId, std::shared_ptr<PkModel> >& _pkModel,
            GroupsParameterSetSeries& _parameterSeries,
            std::map<AnalyteGroupId, Etas>& _etas);

    [[nodiscard]] ComputingStatus addLoad(
            DosageAdjustment& _dosage,
            const ComputingTraitAdjustment* _traits,
            const ComputingRequest& _request,
            const std::vector<AnalyteGroupId>& _allGroupIds,
            const DateTime& _calculationStartTime,
            std::map<AnalyteGroupId, std::shared_ptr<PkModel> >& _pkModel,
            GroupsParameterSetSeries& _parameterSeries,
            std::map<AnalyteGroupId, Etas>& _etas,
            bool& _modified);

    [[nodiscard]] ComputingStatus addRest(
            DosageAdjustment& _dosage,
            const ComputingTraitAdjustment* _traits,
            const ComputingRequest& _request,
            const std::vector<AnalyteGroupId>& _allGroupIds,
            const DateTime& _calculationStartTime,
            std::map<AnalyteGroupId, std::shared_ptr<PkModel> >& _pkModel,
            GroupsParameterSetSeries& _parameterSeries,
            std::map<AnalyteGroupId, Etas>& _etas,
            bool& _modified);

    [[nodiscard]] ComputingStatus generatePredictions(
            std::vector<DosageAdjustment>& _dosages,
            const ComputingTraitAdjustment* _traits,
            const ComputingRequest& _request,
            const std::vector<AnalyteGroupId>& _allGroupIds,
            const Common::DateTime& _calculationStartTime,
            std::map<AnalyteGroupId, std::shared_ptr<PkModel> >& _pkModel,
            GroupsParameterSetSeries& _parameterSeries,
            std::map<AnalyteGroupId, Etas>& _etas);

    [[nodiscard]] ComputingStatus generatePrediction(
            DosageAdjustment& _dosage,
            const ComputingTraitAdjustment* _traits,
            const ComputingRequest& _request,
            const std::vector<AnalyteGroupId>& _allGroupIds,
            const DateTime& _calculationStartTime,
            std::map<AnalyteGroupId, std::shared_ptr<PkModel> >& _pkModel,
            GroupsParameterSetSeries& _parameterSeries,
            std::map<AnalyteGroupId, Etas>& _etas);

    [[nodiscard]] ComputingStatus extractCandidates(
            const ComputingTraitAdjustment* _traits,
            const ComputingRequest& _request,
            std::vector<SimpleDosageCandidate>& _candidates,
            bool& _multipleFormulationAndRoutes);

    [[nodiscard]] ComputingStatus computeCandidate(
            const SimpleDosageCandidate& _candidate,
            const ComputingTraitAdjustment* _traits,
            const ComputingRequest& _request,
            std::map<AnalyteGroupId, std::shared_ptr<PkModel> >& _pkModel,
            std::vector<AnalyteGroupId>& _allGroupIds,
            std::map<AnalyteGroupId, Etas> _etas,
            std::vector<DosageAdjustment>& _allAdjustments,
            DateTime& _calculationStartTime,
            bool& _isValidCandidate,
            std::vector<Tucuxi::Core::ConcentrationPredictionPtr>& _analytesPredictions,
            std::unique_ptr<DosageTimeRange>& _newDosage,
            GroupsIntakeSeries& _intakeSeriesPerGroup);


    std::vector<const FullFormulationAndRoute*> selectFormulationAndRoutes(
            FormulationAndRouteSelectionOption _option,
            const FormulationAndRoutes& _availableFormulationAndRoutes,
            const DosageHistory& _dosageHistory);

    ///
    /// \brief Evaluate the current dosage history with respect to the targets
    /// \param _traits The computing traits to use
    /// \param _request The request from which all the informations are extracted
    /// \param _evaluationResults The evaluation results, one per target
    /// \param _isInRange true if the current dosage is in the target range, false else
    /// \return ComputingStatus::Ok if everything went well
    ///
    [[nodiscard]] ComputingStatus evaluateCurrentDosageHistory(
            const ComputingTraitAdjustment* _traits,
            const ComputingRequest& _request,
            AdjustmentData& _adjustmentData,
            std::map<AnalyteGroupId, std::shared_ptr<PkModel> >& _pkModel,
            std::vector<AnalyteGroupId>& _allGroupIds,
            std::map<AnalyteGroupId, Etas> _etas,
            std::map<ActiveMoietyId, TargetSeries> _targetSeries,
            DateTime _calculationStartTime);

    ///
    /// \brief Extract a DosageHistory for steady state computation
    /// \param _oldHistory The existing history
    /// \param _newHistory The new extracted history
    /// \param _adjustmentTime The time at which the steady state history should start
    /// \param _newStartTime The time the new history actually starts
    /// \param _newEndTime The end of the new history
    /// \return ComputingStatus::Ok if there is a potential steady state, ComputingStatus::NoSteadyState else
    ///
    /// This function tries to extract the last DosageTimeRange of the history and build
    /// a new history for steady state computation. The result is meant to be used for statistics computations.
    /// The _adjustmentTime is the time at this the adjustment starts and should be the beginning of the
    /// new history. There could be some differences for instance if the old history is a daily dose.
    ///
    [[nodiscard]] ComputingStatus extractnewHistoryForSteadyState(
            const DosageHistory& _oldHistory,
            DosageHistory& _newHistory,
            const DateTime& _adjustmentTime,
            DateTime& _newStartTime,
            DateTime& _newEndTime);

    [[nodiscard]] TucuUnit getFinalUnit(const ComputingTraitAdjustment* _traits, ActiveMoiety* _activeMoiety) const;

    Tucuxi::Common::LoggerHelper m_logger;
    ComputingUtils* m_utils;
};

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_COMPUTINGADJUSTMENTS_H
