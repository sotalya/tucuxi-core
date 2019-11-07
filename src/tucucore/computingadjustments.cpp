#include "computingadjustments.h"

#include "tucucommon/datetime.h"
#include "tucucore/drugmodel/formulationandroute.h"
#include "tucucore/dosage.h"
#include "tucucore/pkmodel.h"
#include "tucucore/covariateevent.h"
#include "tucucore/computingcomponent.h"
#include "tucucore/generalextractor.h"
#include "tucucore/computingservice/computingrequest.h"
#include "tucucore/targetextractor.h"
#include "tucucore/targetevaluator.h"
#include "tucucore/intakeextractor.h"
#include "tucucore/intaketocalculatorassociator.h"
#include "tucucore/concentrationcalculator.h"
#include "tucucore/computingutils.h"

namespace Tucuxi {
namespace Core {

ComputingAdjustments::ComputingAdjustments(ComputingUtils *_computingUtils) :
    m_utils(_computingUtils)
{

}


DosageTimeRange *ComputingAdjustments::createDosage(const ComputingAdjustments::SimpleDosageCandidate &_candidate,
        DateTime _startTime,
        DateTime _endTime)
{
    unsigned int nbTimes;

    // At least a number of intervals allowing to fill the interval asked
    nbTimes = static_cast<unsigned int>(std::ceil((_endTime - _startTime) / _candidate.m_interval));
    LastingDose lastingDose(_candidate.m_dose, _candidate.m_formulationAndRoute, _candidate.m_infusionTime, _candidate.m_interval);
    DosageRepeat repeat(lastingDose, nbTimes);
    DosageTimeRange *newTimeRange = new DosageTimeRange(_startTime, _endTime, repeat);
    return newTimeRange;
}

DosageTimeRange *ComputingAdjustments::createLoadingDosage(const SimpleDosageCandidate &_candidate,
        DateTime _startTime)
{
    unsigned int nbTimes = 1;

    LastingDose lastingDose(_candidate.m_dose, _candidate.m_formulationAndRoute, _candidate.m_infusionTime, _candidate.m_interval);
    DosageRepeat repeat(lastingDose, nbTimes);
    DateTime endTime = _startTime + _candidate.m_interval;
    DosageTimeRange *newTimeRange = new DosageTimeRange(_startTime, endTime, repeat);
    return newTimeRange;
}

DosageTimeRange *ComputingAdjustments::createSteadyStateDosage(const SimpleDosageCandidate &_candidate,
        DateTime _startTime)
{
    unsigned int nbTimes;

    // A single interval
    nbTimes = 1;
    LastingDose lastingDose(_candidate.m_dose, _candidate.m_formulationAndRoute, _candidate.m_infusionTime, _candidate.m_interval);
    DosageRepeat repeat(lastingDose, nbTimes);
    DateTime endTime = _startTime + _candidate.m_interval;
    DosageTimeRange *newTimeRange = new DosageTimeRange(_startTime, endTime, repeat);
    return newTimeRange;
}

ComputingResult ComputingAdjustments::buildCandidates(const FullFormulationAndRoute* _formulationAndRoute, std::vector<ComputingAdjustments::SimpleDosageCandidate> &_candidates)
{
    std::vector<Value> doseValues;
    std::vector<Duration> intervalValues;
    std::vector<Duration> infusionTimes;

    const ValidDoses * doses = _formulationAndRoute->getValidDoses();
    if (doses != nullptr) {
        doseValues = doses->getValues();
    }

    const ValidDurations * intervals = _formulationAndRoute->getValidIntervals();
    if (intervals != nullptr) {
        intervalValues = intervals->getDurations();
    }

    const ValidDurations * infusions = _formulationAndRoute->getValidInfusionTimes();
    if (infusions != nullptr) {
        infusionTimes = infusions->getDurations();
    }

    if (doseValues.size() == 0) {
        m_logger.error("No available potential dose");
        return ComputingResult::NoAvailableDose;
    }

    if (intervalValues.size() == 0) {
        m_logger.error("No available interval");
        return ComputingResult::NoAvailableInterval;
    }

    if (infusionTimes.size() == 0) {
        if (_formulationAndRoute->getFormulationAndRoute().getAbsorptionModel() == AbsorptionModel::Infusion) {
            m_logger.error("Infusion selected, but no potential infusion time");
            return ComputingResult::NoAvailableInfusionTime;
        }
        infusionTimes.push_back(Duration(0h));
    }


    // Creation of all candidates
    for(auto dose : doseValues) {
        for(auto interval : intervalValues) {
            for(auto infusion : infusionTimes) {
                _candidates.push_back({_formulationAndRoute->getFormulationAndRoute(), dose, interval, infusion});
#if 0
                std::string mess;
                mess = "Potential adjustment. Dose :  \t" + std::to_string(dose)
                        + " , Interval: \t" + std::to_string((interval).toHours()) + " hours. "
                        + " , Infusion: \t" + std::to_string((infusion).toMinutes()) + " minutes";
                m_logger.info(mess);
#endif // 0
            }
        }
    }
    return ComputingResult::Ok;
}

ComputingResult ComputingAdjustments::buildCandidatesForInterval(const FullFormulationAndRoute* _formulationAndRoute,
                                                                 Duration _interval,
                                                                 std::vector<ComputingAdjustments::SimpleDosageCandidate> &_candidates)
{
    std::vector<Value> doseValues;
    std::vector<Duration> infusionTimes;

    const ValidDoses * doses = _formulationAndRoute->getValidDoses();
    if (doses != nullptr) {
        doseValues = doses->getValues();
    }

    const ValidDurations * infusions = _formulationAndRoute->getValidInfusionTimes();
    if (infusions != nullptr) {
        infusionTimes = infusions->getDurations();
    }

    if (doseValues.size() == 0) {
        m_logger.error("No available potential dose");
        return ComputingResult::NoAvailableDose;
    }

    if (infusionTimes.size() == 0) {
        if (_formulationAndRoute->getFormulationAndRoute().getAbsorptionModel() == AbsorptionModel::Infusion) {
            m_logger.error("Infusion selected, but no potential infusion time");
            return ComputingResult::NoAvailableInfusionTime;
        }
        infusionTimes.push_back(Duration(0h));
    }


    // Creation of all candidates
    for(auto dose : doseValues) {
        for(auto infusion : infusionTimes) {
            _candidates.push_back({_formulationAndRoute->getFormulationAndRoute(), dose, _interval, infusion});
#if 0
            std::string mess;
            mess = "Potential adjustment. Dose :  \t" + std::to_string(dose)
                    + " , Interval: \t" + std::to_string((interval).toHours()) + " hours. "
                    + " , Infusion: \t" + std::to_string((infusion).toMinutes()) + " minutes";
            m_logger.info(mess);
#endif // 0
        }
    }
    return ComputingResult::Ok;
}


bool compareCandidates(const FullDosage &_a, const FullDosage &_b)
{
    return _a.getGlobalScore() < _b.getGlobalScore();
}


std::vector<FullDosage> ComputingAdjustments::sortAndFilterCandidates(std::vector<FullDosage> &_candidates, BestCandidatesOption _option)
{
    // Sort in reverse order. The highest score will be the first element
    // There is an issue with DosageHistory as the copy don't work correctly
    std::sort(_candidates.rbegin(), _candidates.rend(), compareCandidates);

#if 0
    std::cout << "Sorted..." << std::endl;
    // For debugging purpose only
    for (const auto & candidates : dosageCandidates)
    {
        std::cout << "Evaluation. Score : " << candidates.getGlobalScore()  << std::endl;
    }
#endif // 0

    switch (_option) {
    case BestCandidatesOption::AllDosages : {
        return _candidates;
    } // break;
    case BestCandidatesOption::BestDosage : {
        std::vector<FullDosage> bestDosage;
        if (_candidates.size() != 0) {
            bestDosage.push_back(_candidates[0]);
        }
        return bestDosage;
    } // break;
    case BestCandidatesOption::BestDosagePerInterval : {
        for(size_t i = 0; i < _candidates.size(); i++) {
            const DosageRepeat *repeat = dynamic_cast<const DosageRepeat *>(_candidates[i].m_history.getDosageTimeRanges().back()->getDosage());
            if (repeat != nullptr) {
                const LastingDose *dose = dynamic_cast<const LastingDose *>(repeat->getDosage());
                if (dose != nullptr) {
                    Duration interval = dose->getTimeStep();
                    for(size_t j = i + 1; j < _candidates.size(); j++) {

                        const DosageRepeat *repeat2 = dynamic_cast<const DosageRepeat *>(_candidates[j].m_history.getDosageTimeRanges().back()->getDosage());
                        if (repeat2 != nullptr) {
                            const LastingDose *dose2 = dynamic_cast<const LastingDose *>(repeat2->getDosage());
                            if (dose2 != nullptr) {
                                if (dose2->getTimeStep() == interval) {
                                    _candidates.erase(_candidates.begin() + static_cast<long>(j));
                                    j = j - 1;
                                }
                            }
                        }

                    }
                }
            }
        }
        return _candidates;
    } // break;
    }

    // This should not happen
    return _candidates;
}

std::vector<const FullFormulationAndRoute*> selectFormulationAndRoutes(
        FormulationAndRouteSelectionOption _option,
        const FormulationAndRoutes & _availableFormulationAndRoutes,
        const DosageHistory & _dosageHistory)
{
    std::vector<const FullFormulationAndRoute*> result;

    switch (_option) {
    case FormulationAndRouteSelectionOption::LastFormulationAndRoute : {
        if (_dosageHistory.isEmpty()) {
            // If the dosage history is empty, then use the default of DrugModel
            result.push_back(_availableFormulationAndRoutes.getDefault());
        }
        else {
            // We get the last formulation and retrieve the corresponding full spec from the available ones
            FormulationAndRoute formulation = _dosageHistory.getLastFormulationAndRoute();
            const FullFormulationAndRoute *selectedFormulationAndRoute = _availableFormulationAndRoutes.get(formulation);
            if (selectedFormulationAndRoute != nullptr) {
                result.push_back(selectedFormulationAndRoute);
            }
        }
    } break;

    case FormulationAndRouteSelectionOption::DefaultFormulationAndRoute : {
        // Get the default
        result.push_back(_availableFormulationAndRoutes.getDefault());
    } break;
    case FormulationAndRouteSelectionOption::AllFormulationAndRoutes : {
        // Get the entire available list
        for (auto const & f : _availableFormulationAndRoutes) {
            result.push_back(f.get());
        }
    } break;
    }
    return result;
}

ComputingResult ComputingAdjustments::compute(
        const ComputingTraitAdjustment *_traits,
        const ComputingRequest &_request,
        std::unique_ptr<ComputingResponse> &_response)
{

    if (_traits == nullptr) {
        m_logger.error("The computing traits sent for computation are nullptr");
        return ComputingResult::NoComputingTraits;
    }

    std::map<AnalyteGroupId, std::shared_ptr<PkModel> > pkModel;

    GroupsIntakeSeries intakeSeries;
    CovariateSeries covariateSeries;
    GroupsParameterSetSeries parameterSeries;
    DateTime calculationStartTime;

    // Be carefull here, as the endTime could be different...
    ComputingResult extractionResult = m_utils->m_generalExtractor->generalExtractions(_traits,
                                                                                       _request,
                                                                                       m_utils->m_models.get(),
                                                                                       pkModel,
                                                                                       intakeSeries,
                                                                                       covariateSeries,
                                                                                       parameterSeries,
                                                                                       calculationStartTime);

    if (extractionResult != ComputingResult::Ok) {
        return extractionResult;
    }

    DateTime initialCalculationTime = calculationStartTime;


    std::map<AnalyteGroupId, Etas> etas;

    std::vector<AnalyteGroupId> allGroupIds;
    for(const auto &analyteGroup : _request.getDrugModel().getAnalyteSets()) {
        allGroupIds.push_back(analyteGroup->getId());
    }

    for(auto analyteGroupId : allGroupIds) {
        if (_traits->getComputingOption().getParametersType() == PredictionParameterType::Aposteriori) {
            ComputingResult aposterioriEtasExtractionResult = m_utils->m_generalExtractor->extractAposterioriEtas(etas[analyteGroupId], _request, analyteGroupId, intakeSeries[analyteGroupId], parameterSeries[analyteGroupId], covariateSeries, calculationStartTime, _traits->getEnd());
            if (aposterioriEtasExtractionResult != ComputingResult::Ok) {
                return aposterioriEtasExtractionResult;
            }
        }
    }

    const FormulationAndRoutes &formulationAndRoutes = _request.getDrugModel().getFormulationAndRoutes();

    const DosageHistory &dosageHistory = _request.getDrugTreatment().getDosageHistory();

    std::vector<const FullFormulationAndRoute*> selectedFormulationAndRoutes = selectFormulationAndRoutes(
                _traits->getFormulationAndRouteSelectionOption(),
                formulationAndRoutes,
                dosageHistory);

    if (selectedFormulationAndRoutes.size() == 0) {
        m_logger.error("Can not find a formulation and route for adjustment");
        return ComputingResult::NoFormulationAndRouteForAdjustment;
    }


    std::vector<SimpleDosageCandidate> candidates;

    int index = 0;

    // We iterate over the potential formulation and routes
    for (auto const *selectedFormulationAndRoute : selectedFormulationAndRoutes) {

        if (selectedFormulationAndRoute == nullptr) {
            m_logger.error("Can not find a formulation and route for adjustment");
            return ComputingResult::NoFormulationAndRouteForAdjustment;
        }


        ComputingResult buildingResult = buildCandidates(selectedFormulationAndRoute, candidates);

        if (buildingResult != ComputingResult::Ok) {
            m_logger.error("Can not build adjustment candidates");
            return buildingResult;
        }
    }

    TargetExtractor targetExtractor;
    std::map<ActiveMoietyId, TargetSeries> targetSeries;
    TargetExtractionOption targetExtractionOption = _traits->getTargetExtractionOption();

    for (const auto &activeMoiety : _request.getDrugModel().getActiveMoieties()) {
        ComputingResult targetExtractionResult =
                targetExtractor.extract(covariateSeries, activeMoiety.get()->getTargetDefinitions(),
                                        _request.getDrugTreatment().getTargets(), _traits->getStart(), _traits->getEnd(),
                                        targetExtractionOption, targetSeries[activeMoiety->getActiveMoietyId()]);

        if (targetExtractionResult != ComputingResult::Ok) {
            return targetExtractionResult;
        }
    }

    std::vector<FullDosage> dosageCandidates;

    // A vector of vector because each adjustment candidate can have various targets
    std::vector< std::vector< TargetEvaluationResult> > evaluationResults;


    TargetEvaluator targetEvaluator;
    for (auto candidate : candidates) {
        DateTime newEndTime;

        std::unique_ptr<DosageTimeRange> newDosage;

        std::unique_ptr<DosageHistory> newHistory;

        // If in steady state mode, then calculate the real end time
        if (_traits->getSteadyStateTargetOption() == SteadyStateTargetOption::AtSteadyState) {

            // Rounding the new duration to be a multiple of the new interval
            int nbIntervals = 1;

            Duration roundedNewDuration = candidate.m_interval * nbIntervals;

            newEndTime = _traits->getAdjustmentTime() + roundedNewDuration;

            // We only need to start at the time of adjustments
            calculationStartTime = _traits->getAdjustmentTime();

            newDosage = std::unique_ptr<DosageTimeRange>(
                        createSteadyStateDosage(candidate, _traits->getAdjustmentTime()));

            newHistory = std::make_unique<DosageHistory>();
            newHistory->addTimeRange(*newDosage.get());
        }
        else {
            newEndTime = _traits->getEnd();

            newDosage = std::unique_ptr<DosageTimeRange>(
                        createDosage(candidate, _traits->getAdjustmentTime(), newEndTime));

            newHistory = std::unique_ptr<DosageHistory>(_request.getDrugTreatment().getDosageHistory().clone());
            newHistory->mergeDosage(newDosage.get());
        }

        GroupsIntakeSeries intakeSeriesPerGroup;

        std::vector<ConcentrationPredictionPtr> analytesPredictions;

        for (auto analyteGroupId : allGroupIds) {
            ConcentrationPredictionPtr pPrediction = std::make_unique<ConcentrationPrediction>();

            IntakeExtractor intakeExtractor;
            double nbPointsPerHour = _traits->getNbPointsPerHour();
            ComputingResult intakeExtractionResult = intakeExtractor.extract(*newHistory, calculationStartTime, newEndTime,
                                                                             nbPointsPerHour, intakeSeriesPerGroup[analyteGroupId],
                                                                             ExtractionOption::EndofDate);

            m_utils->m_generalExtractor->convertAnalytes(intakeSeriesPerGroup[analyteGroupId], _request.getDrugModel(), _request.getDrugModel().getAnalyteSet(analyteGroupId));


            if (intakeExtractionResult != ComputingResult::Ok) {
                return intakeExtractionResult;
            }

            ComputingResult intakeAssociationResult =
                    IntakeToCalculatorAssociator::associate(intakeSeriesPerGroup[analyteGroupId], *pkModel[analyteGroupId]);

            if (intakeAssociationResult != ComputingResult::Ok) {
                m_logger.error("Can not associate intake calculators for the specified route");
                return intakeAssociationResult;
            }



            ComputingResult predictionComputingResult;

            if (_traits->getSteadyStateTargetOption() == SteadyStateTargetOption::AtSteadyState) {
                ConcentrationCalculator concentrationCalculator;
                predictionComputingResult = concentrationCalculator.computeConcentrationsAtSteadyState(
                            pPrediction,
                            false,
                            calculationStartTime,
                            newEndTime,
                            intakeSeriesPerGroup[analyteGroupId],
                            parameterSeries[analyteGroupId],
                            etas[analyteGroupId]);
            }
            else {
                ConcentrationCalculator concentrationCalculator;
                predictionComputingResult = concentrationCalculator.computeConcentrations(
                            pPrediction,
                            false,
                            calculationStartTime,
                            newEndTime,
                            intakeSeriesPerGroup[analyteGroupId],
                            parameterSeries[analyteGroupId],
                            etas[analyteGroupId]);
            }

            if (predictionComputingResult != ComputingResult::Ok) {
                m_logger.error("Error with the computation of a single adjustment candidate");
                return predictionComputingResult;
            }
            else {
                analytesPredictions.push_back(std::move(pPrediction));
            }
        }

        std::vector<ConcentrationPredictionPtr> activeMoietiesPredictions;

        if (!_request.getDrugModel().isSingleAnalyte()) {

            for (const auto & activeMoiety : _request.getDrugModel().getActiveMoieties()) {
                ConcentrationPredictionPtr activeMoietyPrediction = std::make_unique<ConcentrationPrediction>();
                ComputingResult activeMoietyComputingResult = m_utils->computeActiveMoiety(activeMoiety.get(), analytesPredictions, activeMoietyPrediction);
                if (activeMoietyComputingResult != ComputingResult::Ok) {
                    return activeMoietyComputingResult;
                }
                activeMoietiesPredictions.push_back(std::move(activeMoietyPrediction));
            }
        }
        else {
            activeMoietiesPredictions.push_back(std::move(analytesPredictions[0]));
        }

        std::vector< TargetEvaluationResult> candidateResults;
        bool isValidCandidate = true;

        if (targetSeries.size() == 0) {
            isValidCandidate = false;
        }

        size_t moietyIndex = 0;
        for (const auto &activeMoiety : _request.getDrugModel().getActiveMoieties()) {
            for(const auto & target : targetSeries[activeMoiety->getActiveMoietyId()]) {
                TargetEvaluationResult localResult;

                // Now the score calculation
                ComputingResult evaluationResult = targetEvaluator.evaluate(*activeMoietiesPredictions[moietyIndex].get(),
                                                                            intakeSeriesPerGroup[allGroupIds[0]],
                                                                            target, localResult);

                // Here we do not compare to Result::Ok, because the result can also be
                // Result::InvalidCandidate
                if (evaluationResult == ComputingResult::TargetEvaluationError) {
                    m_logger.error("Error in the evaluation of targets");
                    return evaluationResult;
                }

                // If the candidate is valid:
                if (evaluationResult == ComputingResult::Ok) {
                    candidateResults.push_back(localResult);
                }
                else {
                    isValidCandidate = false;
                }
            }
            moietyIndex ++;
        }

        if (isValidCandidate) {

#if 0
            // Stream that to file, only for debugging purpose
            // To be removed later on, or at least commented
            std::string fileName = "candidate_" + std::to_string(index) + ".dat";
            pPrediction.get()->streamToFile(fileName);
#endif // 0
            index ++;

            FullDosage dosage;
            dosage.m_targetsEvaluation = candidateResults;


            if (_traits->getSteadyStateTargetOption() == SteadyStateTargetOption::AtSteadyState) {
                // In that case we create the dosage with a good end time
                newDosage = std::unique_ptr<DosageTimeRange>(
                            createDosage(candidate, _traits->getAdjustmentTime(), _traits->getEnd()));

            }
            dosage.m_history.addTimeRange(*newDosage);


            for (auto analyteGroupId : allGroupIds) {
                for (size_t i = 0; i < intakeSeriesPerGroup[analyteGroupId].size(); i++) {
                    TimeOffsets times = activeMoietiesPredictions[0]->getTimes()[i];
                    DateTime start = intakeSeriesPerGroup[analyteGroupId][i].getEventTime();
                    DateTime end = start + std::chrono::milliseconds(static_cast<int>(times.back()) * 1000);
                    if (start >= _traits->getAdjustmentTime()) {
                        CycleData cycle(start, end, Unit("ug/l"));
                        cycle.addData(times, activeMoietiesPredictions[0]->getValues()[i]);

                        AnalyteGroupId analyteGroupId = _request.getDrugModel().getAnalyteSets()[0]->getId();
                        ParameterSetEventPtr params = parameterSeries[analyteGroupId].getAtTime(start, etas[analyteGroupId]);

                        for (auto p = params.get()->begin() ; p < params.get()->end() ; p++) {
                            cycle.m_parameters.push_back({(*p).getParameterId(), (*p).getValue()});
                        }

                        std::sort(cycle.m_parameters.begin(), cycle.m_parameters.end(),
                                  [&] (const ParameterValue &_v1, const ParameterValue &_v2) { return _v1.m_parameterId < _v2.m_parameterId; });


                        dosage.m_data.push_back(cycle);
                    }
                }
            }

            dosageCandidates.push_back(dosage);
        }

        evaluationResults.push_back(candidateResults);


    }

#if 0
    // For debugging purpose only
    for (const auto & evaluationResult : evaluationResults)
    {
        for (const auto & targetEvaluationResult : evaluationResult) {
            std::cout << "Evaluation. Score : " << targetEvaluationResult.getScore() <<
                         " . Value : " << targetEvaluationResult.getValue() << std::endl;
        }
    }

    // For debugging purpose only
    for (const auto & candidates : dosageCandidates)
    {
        std::cout << "Evaluation. Score : " << candidates.getGlobalScore()  << std::endl;
    }
#endif // 0



    std::vector<FullDosage> finalCandidates;
    finalCandidates = sortAndFilterCandidates(dosageCandidates, _traits->getBestCandidatesOption());

    ComputingResult addResult = addLoadOrRest(finalCandidates, _traits, _request, allGroupIds, initialCalculationTime,
                                              pkModel, parameterSeries, etas);
    if (addResult != ComputingResult::Ok) {
        return addResult;
    }

    ComputingResult generateResult = generatePredictions(finalCandidates, _traits, _request,
                                                         allGroupIds, initialCalculationTime, pkModel, parameterSeries,
                                                         etas);
    if (generateResult != ComputingResult::Ok) {
        return generateResult;
    }


    // Now we have adjustments, predictions, and target evaluation results, let's build the response
    std::unique_ptr<AdjustmentResponse> resp = std::make_unique<AdjustmentResponse>(_traits->getId());

    resp->setAdjustments(finalCandidates);

    // Finally add the response to the set of responses
    _response->addResponse(std::move(resp));

    return ComputingResult::Ok;
}


ComputingResult ComputingAdjustments::addLoadOrRest(std::vector<FullDosage> &_dosages,
                                                     const ComputingTraitAdjustment *_traits,
                                                     const ComputingRequest &_request,
                                                     const std::vector<AnalyteGroupId> &_allGroupIds,
                                                     const Common::DateTime &_calculationStartTime,
                                                     std::map<AnalyteGroupId, std::shared_ptr<PkModel> > &_pkModel,
                                                     GroupsParameterSetSeries &_parameterSeries,
                                                     std::map<AnalyteGroupId, Etas> &_etas)
{
    for (auto & dosage : _dosages) {
        ComputingResult result = addLoadOrRest(dosage, _traits, _request, _allGroupIds, _calculationStartTime,
                                               _pkModel, _parameterSeries, _etas);
        if (result != ComputingResult::Ok) {
            return result;
        }
    }
    return ComputingResult::Ok;
}


typedef struct {
    FullDosage loadingDosage; // NOLINT(readability-identifier-naming)
    double score;             // NOLINT(readability-identifier-naming)
} LoadingCandidate;


ComputingResult ComputingAdjustments::addLoadOrRest(FullDosage &_dosage,
                                                    const ComputingTraitAdjustment *_traits,
                                                    const ComputingRequest &_request,
                                                    const std::vector<AnalyteGroupId> &_allGroupIds,
                                                    const Common::DateTime &_calculationStartTime,
                                                    std::map<AnalyteGroupId, std::shared_ptr<PkModel> > &_pkModel,
                                                    GroupsParameterSetSeries &_parameterSeries,
                                                    std::map<AnalyteGroupId, Etas> &_etas)
{
    if (_traits->getLoadingOption() == LoadingOption::NoLoadingDose) {
        return ComputingResult::Ok;
    }

    const DosageRepeat *repeat = static_cast<const DosageRepeat *>(_dosage.m_history.getDosageTimeRanges()[0]->getDosage());
    const LastingDose *dosage = static_cast<const LastingDose *>(repeat->getDosage());
    Duration interval = dosage->getTimeStep();


    std::vector<double> &lastConcentrations = _dosage.m_data.back().m_concentrations[0];
    double steadyStateResidual = lastConcentrations.back();

    FormulationAndRoute formulationAndRoute = dosage->getLastFormulationAndRoute();


    // TODO : This is wrong to take the default here
    const FullFormulationAndRoute *fullFormulationAndRoute = _request.getDrugModel().getFormulationAndRoutes().getDefault();
    std::vector<ComputingAdjustments::SimpleDosageCandidate> candidates;
    ComputingResult buildResult = buildCandidatesForInterval(fullFormulationAndRoute, interval, candidates);
    if (buildResult != ComputingResult::Ok) {
        return buildResult;
    }

    std::vector<LoadingCandidate> loadingCandidates;

    for (const auto &candidate : candidates) {
        FullDosage loadingDosage;
        // Create the loading dose
        std::unique_ptr<DosageTimeRange> newDosage = std::unique_ptr<DosageTimeRange>(
                    createLoadingDosage(candidate, _traits->getAdjustmentTime()));
        loadingDosage.m_history.addTimeRange(*newDosage);

        ComputingResult generateResult = generatePrediction(loadingDosage, _traits, _request,
                                                             _allGroupIds, _calculationStartTime, _pkModel, _parameterSeries,
                                                             _etas);
        if (generateResult != ComputingResult::Ok) {
            return generateResult;
        }

        std::vector<double> &concentrations = loadingDosage.m_data[0].m_concentrations[0];
        double residual = concentrations.back();
        double score = steadyStateResidual - residual;
        loadingCandidates.push_back({loadingDosage, score});
    }

    double bestScore = 1000000000.0;
    size_t bestIndex = 0;
    size_t index = 0;
    for (const auto & candidate : loadingCandidates) {
        if (std::abs(candidate.score) < bestScore) {
            bestScore = std::abs(candidate.score);
            bestIndex = index;
        }
        index ++;
    }


    DosageHistory steadyStateHistory = _dosage.m_history;
    DosageTimeRange range = *steadyStateHistory.getDosageTimeRanges()[0].get();
    auto d = range.getDosage();
    DosageTimeRange newRange(range.getStartDate() + interval, range.getEndDate(), *d);
    _dosage.m_history = DosageHistory();
    _dosage.m_history.addTimeRange(*loadingCandidates[bestIndex].loadingDosage.m_history.getDosageTimeRanges()[0].get());
    _dosage.m_history.mergeDosage(&newRange);

    return ComputingResult::Ok;
}

ComputingResult ComputingAdjustments::generatePredictions(std::vector<FullDosage> &_dosages,
                                                          const ComputingTraitAdjustment *_traits,
                                                          const ComputingRequest &_request,
                                                          const std::vector<AnalyteGroupId> &_allGroupIds,
                                                          const DateTime &_calculationStartTime,
                                                          std::map<AnalyteGroupId, std::shared_ptr<PkModel> > &_pkModel,
                                                          GroupsParameterSetSeries &_parameterSeries,
                                                          std::map<AnalyteGroupId, Etas> &_etas
                                                          )
{
    for (auto & dosage : _dosages) {
        ComputingResult result = generatePrediction(dosage, _traits, _request, _allGroupIds, _calculationStartTime, _pkModel, _parameterSeries, _etas);
        if (result != ComputingResult::Ok) {
            return result;
        }
    }
    return ComputingResult::Ok;
}


ComputingResult ComputingAdjustments::generatePrediction(FullDosage &_dosage,
                                                         const ComputingTraitAdjustment *_traits,
                                                         const ComputingRequest &_request,
                                                         const std::vector<AnalyteGroupId> &_allGroupIds,
                                                         const Common::DateTime &_calculationStartTime,
                                                         std::map<AnalyteGroupId, std::shared_ptr<PkModel> > &_pkModel,
                                                         GroupsParameterSetSeries &_parameterSeries,
                                                         std::map<AnalyteGroupId, Etas> &_etas
                                                         )
{
    DateTime newEndTime = _traits->getEnd();

    std::unique_ptr<DosageTimeRange> newDosage;

    std::unique_ptr<DosageHistory> newHistory;

    newHistory = std::unique_ptr<DosageHistory>(_request.getDrugTreatment().getDosageHistory().clone());
    for (const auto & timeRange : _dosage.m_history.getDosageTimeRanges()) {
        newHistory->mergeDosage(timeRange.get());
    }

    GroupsIntakeSeries intakeSeriesPerGroup;

    std::vector<ConcentrationPredictionPtr> analytesPredictions;

    for (auto analyteGroupId : _allGroupIds) {
        ConcentrationPredictionPtr pPrediction = std::make_unique<ConcentrationPrediction>();

        IntakeExtractor intakeExtractor;
        double nbPointsPerHour = _traits->getNbPointsPerHour();
        ComputingResult intakeExtractionResult = intakeExtractor.extract(*newHistory, _calculationStartTime, newEndTime,
                                                                         nbPointsPerHour, intakeSeriesPerGroup[analyteGroupId],
                                                                         ExtractionOption::EndofDate);

        m_utils->m_generalExtractor->convertAnalytes(intakeSeriesPerGroup[analyteGroupId], _request.getDrugModel(), _request.getDrugModel().getAnalyteSet(analyteGroupId));


        if (intakeExtractionResult != ComputingResult::Ok) {
            return intakeExtractionResult;
        }

        ComputingResult intakeAssociationResult =
                IntakeToCalculatorAssociator::associate(intakeSeriesPerGroup[analyteGroupId], *_pkModel[analyteGroupId]);

        if (intakeAssociationResult != ComputingResult::Ok) {
            m_logger.error("Can not associate intake calculators for the specified route");
            return intakeAssociationResult;
        }



        ComputingResult predictionComputingResult;
        ConcentrationCalculator concentrationCalculator;
        predictionComputingResult = concentrationCalculator.computeConcentrations(
                    pPrediction,
                    false,
                    _calculationStartTime,
                    newEndTime,
                    intakeSeriesPerGroup[analyteGroupId],
                    _parameterSeries[analyteGroupId],
                    _etas[analyteGroupId]);

        if (predictionComputingResult != ComputingResult::Ok) {
            m_logger.error("Error with the computation of a single adjustment candidate");
            return predictionComputingResult;
        }
        else {
            analytesPredictions.push_back(std::move(pPrediction));
        }
    }

    std::vector<ConcentrationPredictionPtr> activeMoietiesPredictions;

    if (!_request.getDrugModel().isSingleAnalyte()) {

        for (const auto & activeMoiety : _request.getDrugModel().getActiveMoieties()) {
            ConcentrationPredictionPtr activeMoietyPrediction = std::make_unique<ConcentrationPrediction>();
            ComputingResult activeMoietyComputingResult = m_utils->computeActiveMoiety(activeMoiety.get(), analytesPredictions, activeMoietyPrediction);
            if (activeMoietyComputingResult != ComputingResult::Ok) {
                return activeMoietyComputingResult;
            }
            activeMoietiesPredictions.push_back(std::move(activeMoietyPrediction));
        }
    }
    else {
        activeMoietiesPredictions.push_back(std::move(analytesPredictions[0]));
    }

    // We clear the prediction data
    _dosage.m_data.clear();

    for (auto analyteGroupId : _allGroupIds) {
        for (size_t i = 0; i < intakeSeriesPerGroup[analyteGroupId].size(); i++) {
            TimeOffsets times = activeMoietiesPredictions[0]->getTimes()[i];
            DateTime start = intakeSeriesPerGroup[analyteGroupId][i].getEventTime();
            DateTime end = start + std::chrono::milliseconds(static_cast<int>(times.back()) * 1000);
            if (start >= _traits->getAdjustmentTime()) {
                CycleData cycle(start, end, Unit("ug/l"));
                cycle.addData(times, activeMoietiesPredictions[0]->getValues()[i]);

                AnalyteGroupId analyteGroupId = _request.getDrugModel().getAnalyteSets()[0]->getId();
                ParameterSetEventPtr params = _parameterSeries[analyteGroupId].getAtTime(start, _etas[analyteGroupId]);

                for (auto p = params.get()->begin() ; p < params.get()->end() ; p++) {
                    cycle.m_parameters.push_back({(*p).getParameterId(), (*p).getValue()});
                }

                std::sort(cycle.m_parameters.begin(), cycle.m_parameters.end(),
                          [&] (const ParameterValue &_v1, const ParameterValue &_v2) { return _v1.m_parameterId < _v2.m_parameterId; });


                _dosage.m_data.push_back(cycle);
            }
        }
    }

    return ComputingResult::Ok;
}


#if 0

ComputingResult ComputingAdjustments::compute(
        const ComputingTraitAdjustment *_traits,
        const ComputingRequest &_request,
        std::unique_ptr<ComputingResponse> &_response)
{

    if (_traits == nullptr) {
        m_logger.error("The computing traits sent for computation are nullptr");
        return ComputingResult::NoComputingTraits;
    }

    std::map<AnalyteGroupId, std::shared_ptr<PkModel> > pkModel;

    GroupsIntakeSeries intakeSeries;
    CovariateSeries covariateSeries;
    GroupsParameterSetSeries parameterSeries;
    DateTime calculationStartTime;

    // Be carefull here, as the endTime could be different...
    ComputingResult extractionResult = m_computingComponent->m_generalExtractor->generalExtractions(_traits,
                                                                              _request,
                                                                              m_computingComponent->m_models.get(),
                                                                              pkModel,
                                                                              intakeSeries,
                                                                              covariateSeries,
                                                                              parameterSeries,
                                                                              calculationStartTime);

    if (extractionResult != ComputingResult::Ok) {
        return extractionResult;
    }


    std::map<AnalyteGroupId, Etas> etas;

    std::vector<AnalyteGroupId> allGroupIds;
    for(const auto &analyteGroup : _request.getDrugModel().getAnalyteSets()) {
        allGroupIds.push_back(analyteGroup->getId());
    }

    for(auto analyteGroupId : allGroupIds) {
        if (_traits->getComputingOption().getParametersType() == PredictionParameterType::Aposteriori) {
            ComputingResult aposterioriEtasExtractionResult = m_computingComponent->m_generalExtractor->extractAposterioriEtas(etas[analyteGroupId], _request, analyteGroupId, intakeSeries[analyteGroupId], parameterSeries[analyteGroupId], covariateSeries, calculationStartTime, _traits->getEnd());
            if (aposterioriEtasExtractionResult != ComputingResult::Ok) {
                return aposterioriEtasExtractionResult;
            }
        }
    }

    std::vector<FullDosage> dosageCandidates;

    const FormulationAndRoutes &formulationAndRoutes = _request.getDrugModel().getFormulationAndRoutes();

    const DosageHistory &dosageHistory = _request.getDrugTreatment().getDosageHistory();

    std::vector<const FullFormulationAndRoute*> selectedFormulationAndRoutes = selectFormulationAndRoutes(
                _traits->getFormulationAndRouteSelectionOption(),
                formulationAndRoutes,
                dosageHistory);

    if (selectedFormulationAndRoutes.size() == 0) {
        m_logger.error("Can not find a formulation and route for adjustment");
        return ComputingResult::NoFormulationAndRouteForAdjustment;
    }


    int index = 0;

    // We iterate over the potential formulation and routes
    for (auto const *selectedFormulationAndRoute : selectedFormulationAndRoutes) {

        if (selectedFormulationAndRoute == nullptr) {
            m_logger.error("Can not find a formulation and route for adjustment");
            return ComputingResult::NoFormulationAndRouteForAdjustment;
        }

        std::vector<AdjustmentCandidate> candidates;

        ComputingResult buildingResult = buildCandidates(selectedFormulationAndRoute, candidates);

        if (buildingResult != ComputingResult::Ok) {
            m_logger.error("Can not build adjustment candidates");
            return buildingResult;
        }

        TargetExtractor targetExtractor;
        TargetSeries targetSeries;
        TargetExtractionOption targetExtractionOption = _traits->getTargetExtractionOption();

        ComputingResult targetExtractionResult =
                targetExtractor.extract(covariateSeries, _request.getDrugModel().getActiveMoieties()[0].get()->getTargetDefinitions(),
                                        _request.getDrugTreatment().getTargets(), _traits->getStart(), _traits->getEnd(),
                                        targetExtractionOption, targetSeries);

        if (targetExtractionResult != ComputingResult::Ok) {
            return targetExtractionResult;
        }

        // A vector of vector because each adjustment candidate can have various targets
        std::vector< std::vector< TargetEvaluationResult> > evaluationResults;


        TargetEvaluator targetEvaluator;
        for (auto candidate : candidates) {
            std::unique_ptr<DosageHistory> newHistory = std::unique_ptr<DosageHistory>(_request.getDrugTreatment().getDosageHistory().clone());
            DateTime newEndTime;

            // If in steady state mode, then calculate the real end time
            if (_traits->getSteadyStateTargetOption() == SteadyStateTargetOption::AtSteadyState) {

                const HalfLife &halfLife = _request.getDrugModel().getTimeConsiderations().getHalfLife();

                // Use the same function as for the start date of calculation when in steady state mode
                Duration newDuration = m_computingComponent->m_generalExtractor->secureStartDuration(halfLife);

                // Rounding the new duration to be a multiple of the new interval
                int nbIntervals = static_cast<int>(newDuration / candidate.m_interval);

                Duration roundedNewDuration = candidate.m_interval * nbIntervals;

                newEndTime = _traits->getAdjustmentTime() + roundedNewDuration;
            }
            else {
                newEndTime = _traits->getEnd();
            }

            std::unique_ptr<DosageTimeRange> newDosage = std::unique_ptr<DosageTimeRange>(
                        createDosage(candidate, _traits->getAdjustmentTime(), newEndTime,
                                                      selectedFormulationAndRoute->getFormulationAndRoute()));
            newHistory->mergeDosage(newDosage.get());

            GroupsIntakeSeries intakeSeriesPerGroup;

            std::vector<ConcentrationPredictionPtr> analytesPredictions;

            for (auto analyteGroupId : allGroupIds) {
                ConcentrationPredictionPtr pPrediction = std::make_unique<ConcentrationPrediction>();

                IntakeExtractor intakeExtractor;
                double nbPointsPerHour = _traits->getNbPointsPerHour();
                ComputingResult intakeExtractionResult = intakeExtractor.extract(*newHistory, calculationStartTime, newEndTime,
                                                                                 nbPointsPerHour, intakeSeriesPerGroup[analyteGroupId],
                                                                                 ExtractionOption::EndofDate);

                m_computingComponent->m_generalExtractor->convertAnalytes(intakeSeriesPerGroup[analyteGroupId], _request.getDrugModel(), _request.getDrugModel().getAnalyteSet(analyteGroupId));


                if (intakeExtractionResult != ComputingResult::Ok) {
                    return intakeExtractionResult;
                }

                ComputingResult intakeAssociationResult =
                        IntakeToCalculatorAssociator::associate(intakeSeriesPerGroup[analyteGroupId], *pkModel[analyteGroupId]);

                if (intakeAssociationResult != ComputingResult::Ok) {
                    m_logger.error("Can not associate intake calculators for the specified route");
                    return intakeAssociationResult;
                }



                ComputingResult predictionComputingResult;

                predictionComputingResult = m_computingComponent->computeConcentrations(
                            pPrediction,
                            false,
                            //                            _traits->getStart(),
                            calculationStartTime,
                            newEndTime,
                            intakeSeriesPerGroup[analyteGroupId],
                            parameterSeries[analyteGroupId],
                            etas[analyteGroupId]);

                if (predictionComputingResult != ComputingResult::Ok) {
                    m_logger.error("Error with the computation of a single adjustment candidate");
                    return predictionComputingResult;
                }
                else {
                    analytesPredictions.push_back(std::move(pPrediction));
                }
            }

            std::vector<ConcentrationPredictionPtr> activeMoietiesPredictions;

            if (!_request.getDrugModel().isSingleAnalyte()) {

                for (const auto & activeMoiety : _request.getDrugModel().getActiveMoieties()) {
                    ConcentrationPredictionPtr activeMoietyPrediction = std::make_unique<ConcentrationPrediction>();
                    ComputingResult activeMoietyComputingResult = m_computingComponent->computeActiveMoiety(_request.getDrugModel(), activeMoiety.get(), analytesPredictions, activeMoietyPrediction);
                    if (activeMoietyComputingResult != ComputingResult::Ok) {
                        return activeMoietyComputingResult;
                    }
                    activeMoietiesPredictions.push_back(std::move(activeMoietyPrediction));
                }
            }
            else {
                activeMoietiesPredictions.push_back(std::move(analytesPredictions[0]));
            }

            std::vector< TargetEvaluationResult> candidateResults;
            bool isValidCandidate = true;

            if (targetSeries.size() == 0) {
                isValidCandidate = false;
            }

            for(const auto & target : targetSeries) {
                TargetEvaluationResult localResult;

                // Now the score calculation
                ComputingResult evaluationResult = targetEvaluator.evaluate(*activeMoietiesPredictions[0].get(), intakeSeriesPerGroup[allGroupIds[0]], target, localResult);

                // Here we do not compare to Result::Ok, because the result can also be
                // Result::InvalidCandidate
                if (evaluationResult == ComputingResult::TargetEvaluationError) {
                    m_logger.error("Error in the evaluation of targets");
                    return evaluationResult;
                }

                // If the candidate is valid:
                if (evaluationResult == ComputingResult::Ok) {
                    candidateResults.push_back(localResult);
                }
                else {
                    isValidCandidate = false;
                }
            }

            if (isValidCandidate) {

#if 0
                // Stream that to file, only for debugging purpose
                // To be removed later on, or at least commented
                std::string fileName = "candidate_" + std::to_string(index) + ".dat";
                pPrediction.get()->streamToFile(fileName);
#endif // 0
                index ++;

                FullDosage dosage;
                dosage.m_targetsEvaluation = candidateResults;
                dosage.m_history.addTimeRange(*newDosage);


                for (auto analyteGroupId : allGroupIds) {
                    for (size_t i = 0; i < intakeSeriesPerGroup[analyteGroupId].size(); i++) {
                        TimeOffsets times = activeMoietiesPredictions[0]->getTimes()[i];
                        DateTime start = intakeSeriesPerGroup[analyteGroupId][i].getEventTime();
                        DateTime end = start + std::chrono::milliseconds(static_cast<int>(times.back()) * 1000);
                        if (start >= _traits->getAdjustmentTime()) {
                            CycleData cycle(start, end, Unit("ug/l"));
                            cycle.addData(times, activeMoietiesPredictions[0]->getValues()[i]);

                            AnalyteGroupId analyteGroupId = _request.getDrugModel().getAnalyteSets()[0]->getId();
                            ParameterSetEventPtr params = parameterSeries[analyteGroupId].getAtTime(start, etas[analyteGroupId]);

                            for (auto p = params.get()->begin() ; p < params.get()->end() ; p++) {
                                cycle.m_parameters.push_back({(*p).getParameterId(), (*p).getValue()});
                            }

                            std::sort(cycle.m_parameters.begin(), cycle.m_parameters.end(),
                                      [&] (const ParameterValue &_v1, const ParameterValue &_v2) { return _v1.m_parameterId < _v2.m_parameterId; });


                            dosage.m_data.push_back(cycle);
                        }
                    }
                }

                dosageCandidates.push_back(dosage);
            }

            evaluationResults.push_back(candidateResults);


        }
    }

#if 0
    // For debugging purpose only
    for (const auto & evaluationResult : evaluationResults)
    {
        for (const auto & targetEvaluationResult : evaluationResult) {
            std::cout << "Evaluation. Score : " << targetEvaluationResult.getScore() <<
                         " . Value : " << targetEvaluationResult.getValue() << std::endl;
        }
    }

    // For debugging purpose only
    for (const auto & candidates : dosageCandidates)
    {
        std::cout << "Evaluation. Score : " << candidates.getGlobalScore()  << std::endl;
    }
#endif // 0



    std::vector<FullDosage> finalCandidates;
    finalCandidates = sortAndFilterCandidates(dosageCandidates, _traits->getBestCandidatesOption());


    // Now we have adjustments, predictions, and target evaluation results, let's build the response
    std::unique_ptr<AdjustmentResponse> resp = std::make_unique<AdjustmentResponse>(_traits->getId());

    resp->setAdjustments(finalCandidates);

    // Finally add the response to the set of responses
    _response->addResponse(std::move(resp));

    return ComputingResult::Ok;
}
#endif // 0


} // namespace Core
} // namespace Tucuxi
