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

namespace Tucuxi {
namespace Core {

ComputingAdjustments::ComputingAdjustments(ComputingComponent *_computingComponent) :
    m_computingComponent(_computingComponent)
{

}


DosageTimeRange *ComputingAdjustments::createDosage(
        ComputingAdjustments::AdjustmentCandidate &_candidate,
        DateTime _startTime,
        DateTime _endTime,
        FormulationAndRoute _routeOfAdministration)
{
    unsigned int nbTimes;

    // At least a number of intervals allowing to fill the interval asked
    nbTimes = static_cast<unsigned int>(std::ceil((_endTime - _startTime) / _candidate.m_interval));
    LastingDose lastingDose(_candidate.m_dose, _routeOfAdministration, _candidate.m_infusionTime, _candidate.m_interval);
    DosageRepeat repeat(lastingDose, nbTimes);
    DosageTimeRange *newTimeRange = new DosageTimeRange(_startTime, _endTime, repeat);
    return newTimeRange;
}

ComputingResult ComputingAdjustments::buildCandidates(const FullFormulationAndRoute* _formulationAndRoute, std::vector<ComputingAdjustments::AdjustmentCandidate> &_candidates)
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
                _candidates.push_back({dose, interval, infusion});
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
            bestDosage.push_back(_candidates.at(0));
        }
        return bestDosage;
    } // break;
    case BestCandidatesOption::BestDosagePerInterval : {
        for(size_t i = 0; i < _candidates.size(); i++) {
            const DosageRepeat *repeat = dynamic_cast<const DosageRepeat *>(_candidates.at(i).m_history.getDosageTimeRanges().at(_candidates.at(i).m_history.getDosageTimeRanges().size() - 1)
                                                                            ->getDosage());
            if (repeat != nullptr) {
                const LastingDose *dose = dynamic_cast<const LastingDose *>(repeat->getDosage());
                if (dose != nullptr) {
                    Duration interval = dose->getTimeStep();
                    for(size_t j = i + 1; j < _candidates.size(); j++) {

                        const DosageRepeat *repeat2 = dynamic_cast<const DosageRepeat *>(_candidates.at(j).m_history.getDosageTimeRanges().at(_candidates.at(j).m_history.getDosageTimeRanges().size() - 1)
                                                                                         ->getDosage());
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
                targetExtractor.extract(covariateSeries, _request.getDrugModel().getActiveMoieties().at(0).get()->getTargetDefinitions(),
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
                        TimeOffsets times = activeMoietiesPredictions[0]->getTimes().at(i);
                        DateTime start = intakeSeriesPerGroup[analyteGroupId].at(i).getEventTime();
                        DateTime end = start + std::chrono::milliseconds(static_cast<int>(times.at(times.size() - 1)) * 1000);
                        if (start >= _traits->getAdjustmentTime()) {
                            CycleData cycle(start, end, Unit("ug/l"));
                            cycle.addData(times, activeMoietiesPredictions[0]->getValues().at(i));

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

} // namespace Core
} // namespace Tucuxi
