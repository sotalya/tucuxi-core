#include "multicomputingadjustments.h"

#include "tucucommon/datetime.h"

#include "tucucore/computingservice/computingrequest.h"
#include "tucucore/computingutils.h"
#include "tucucore/concentrationcalculator.h"
#include "tucucore/covariateevent.h"
#include "tucucore/cyclestatisticscalculator.h"
#include "tucucore/dosage.h"
#include "tucucore/drugmodel/formulationandroute.h"
#include "tucucore/generalextractor.h"
#include "tucucore/hardcodedoperation.h"
#include "tucucore/intakeextractor.h"
#include "tucucore/intaketocalculatorassociator.h"
#include "tucucore/multicomputingcomponent.h"
#include "tucucore/multiconcentrationcalculator.h"
#include "tucucore/pkmodel.h"
#include "tucucore/targetevaluator.h"
#include "tucucore/targetextractor.h"

namespace Tucuxi {
namespace Core {

MultiComputingAdjustments::MultiComputingAdjustments(ComputingUtils* _computingUtils) : m_utils(_computingUtils) {}


DosageTimeRange* MultiComputingAdjustments::createDosage(
        const MultiComputingAdjustments::SimpleDosageCandidate& _candidate, DateTime _startTime, DateTime _endTime)
{
    unsigned int nbTimes;

    // At least a number of intervals allowing to fill the interval asked
    nbTimes = static_cast<unsigned int>(std::ceil((_endTime - _startTime) / _candidate.m_interval));
    LastingDose lastingDose(
            _candidate.m_dose,
            _candidate.m_doseUnit,
            _candidate.m_formulationAndRoute,
            _candidate.m_infusionTime,
            _candidate.m_interval);
    DosageRepeat repeat(lastingDose, nbTimes);
    DosageTimeRange* newTimeRange = new DosageTimeRange(_startTime, _endTime, repeat);
    return newTimeRange;
}

DosageTimeRange* MultiComputingAdjustments::createLoadingDosageOrRestPeriod(
        const SimpleDosageCandidate& _candidate, DateTime _startTime)
{
    unsigned int nbTimes = 1;

    LastingDose lastingDose(
            _candidate.m_dose,
            _candidate.m_doseUnit,
            _candidate.m_formulationAndRoute,
            _candidate.m_infusionTime,
            _candidate.m_interval);
    DosageRepeat repeat(lastingDose, nbTimes);
    DateTime endTime = _startTime + _candidate.m_interval;
    DosageTimeRange* newTimeRange = new DosageTimeRange(_startTime, endTime, repeat);
    return newTimeRange;
}

DosageTimeRange* MultiComputingAdjustments::createSteadyStateDosage(
        const SimpleDosageCandidate& _candidate, DateTime _startTime)
{
    unsigned int nbTimes;

    // A single interval
    nbTimes = 1;
    LastingDose lastingDose(
            _candidate.m_dose,
            _candidate.m_doseUnit,
            _candidate.m_formulationAndRoute,
            _candidate.m_infusionTime,
            _candidate.m_interval);
    DosageRepeat repeat(lastingDose, nbTimes);
    DateTime endTime = _startTime + _candidate.m_interval;
    DosageTimeRange* newTimeRange = new DosageTimeRange(_startTime, endTime, repeat);
    return newTimeRange;
}

ComputingStatus MultiComputingAdjustments::buildCandidates(
        const FullFormulationAndRoute* _formulationAndRoute,
        std::vector<MultiComputingAdjustments::SimpleDosageCandidate>& _candidates)
{
    std::vector<Value> doseValues;
    TucuUnit doseUnit;
    std::vector<Duration> intervalValues;
    std::vector<Duration> infusionTimes;

    const ValidDoses* doses = _formulationAndRoute->getValidDoses();
    if (doses != nullptr) {
        doseValues = doses->getValues();
        doseUnit = doses->getUnit();
    }

    const ValidDurations* intervals = _formulationAndRoute->getValidIntervals();
    if (intervals != nullptr) {
        intervalValues = intervals->getDurations();
    }

    const ValidDurations* infusions = _formulationAndRoute->getValidInfusionTimes();
    if (infusions != nullptr) {
        infusionTimes = infusions->getDurations();
    }

    if (doseValues.empty()) {
        m_logger.error("No available potential dose");
        return ComputingStatus::NoAvailableDose;
    }

    if (intervalValues.empty()) {
        m_logger.error("No available interval");
        return ComputingStatus::NoAvailableInterval;
    }

    if (infusionTimes.empty()) {
        if (_formulationAndRoute->getFormulationAndRoute().getAbsorptionModel() == AbsorptionModel::Infusion) {
            m_logger.error("Infusion selected, but no potential infusion time");
            return ComputingStatus::NoAvailableInfusionTime;
        }
        infusionTimes.emplace_back(0h);
    }


    // Creation of all candidates
    for (auto dose : doseValues) {
        for (auto interval : intervalValues) {
            for (auto infusion : infusionTimes) {
                _candidates.push_back(
                        {_formulationAndRoute->getFormulationAndRoute(), dose, doseUnit, interval, infusion});
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
    return ComputingStatus::Ok;
}

ComputingStatus MultiComputingAdjustments::buildCandidatesForInterval(
        const FullFormulationAndRoute* _formulationAndRoute,
        Duration _interval,
        std::vector<MultiComputingAdjustments::SimpleDosageCandidate>& _candidates)
{
    std::vector<Value> doseValues;
    TucuUnit doseUnit;
    std::vector<Duration> infusionTimes;

    const ValidDoses* doses = _formulationAndRoute->getValidDoses();
    if (doses != nullptr) {
        doseValues = doses->getValues();
        doseUnit = doses->getUnit();
    }

    const ValidDurations* infusions = _formulationAndRoute->getValidInfusionTimes();
    if (infusions != nullptr) {
        infusionTimes = infusions->getDurations();
    }

    if (doseValues.empty()) {
        m_logger.error("No available potential dose");
        return ComputingStatus::NoAvailableDose;
    }

    if (infusionTimes.empty()) {
        if (_formulationAndRoute->getFormulationAndRoute().getAbsorptionModel() == AbsorptionModel::Infusion) {
            m_logger.error("Infusion selected, but no potential infusion time");
            return ComputingStatus::NoAvailableInfusionTime;
        }
        infusionTimes.emplace_back(0h);
    }


    // Creation of all candidates
    for (auto dose : doseValues) {
        for (auto infusion : infusionTimes) {
            _candidates.push_back(
                    {_formulationAndRoute->getFormulationAndRoute(), dose, doseUnit, _interval, infusion});
#if 0
            std::string mess;
            mess = "Potential adjustment. Dose :  \t" + std::to_string(dose)
                    + " , Interval: \t" + std::to_string((interval).toHours()) + " hours. "
                    + " , Infusion: \t" + std::to_string((infusion).toMinutes()) + " minutes";
            m_logger.info(mess);
#endif // 0
        }
    }

    return ComputingStatus::Ok;
}


std::vector<DosageAdjustment> MultiComputingAdjustments::sortAndFilterCandidates(
        std::vector<DosageAdjustment>& _candidates, BestCandidatesOption _option)
{
    // Sort in reverse order. The highest score will be the first element
    // There is an issue with DosageHistory as the copy don't work correctly
    std::sort(_candidates.rbegin(), _candidates.rend(), [](const DosageAdjustment& _a, const DosageAdjustment& _b) {
        return _a.getGlobalScore() < _b.getGlobalScore();
    });

#if 0
    std::cout << "Sorted..." << std::endl;
    // For debugging purpose only
    for (const auto & candidates : dosageCandidates)
    {
        std::cout << "Evaluation. Score : " << candidates.getGlobalScore()  << std::endl;
    }
#endif // 0

    switch (_option) {
    case BestCandidatesOption::AllDosages: {
        return _candidates;
    } // break;
    case BestCandidatesOption::BestDosage: {
        std::vector<DosageAdjustment> bestDosage;
        if (!_candidates.empty()) {
            bestDosage.push_back(_candidates[0]);
        }
        return bestDosage;
    } // break;
    case BestCandidatesOption::BestDosagePerInterval: {
        for (size_t i = 0; i < _candidates.size(); i++) {
            const DosageRepeat* repeat = dynamic_cast<const DosageRepeat*>(
                    _candidates[i].m_history.getDosageTimeRanges().back()->getDosage());
            if (repeat != nullptr) {
                const LastingDose* dose = dynamic_cast<const LastingDose*>(repeat->getDosage());
                if (dose != nullptr) {
                    Duration interval = dose->getTimeStep();
                    for (size_t j = i + 1; j < _candidates.size(); j++) {

                        const DosageRepeat* repeat2 = dynamic_cast<const DosageRepeat*>(
                                _candidates[j].m_history.getDosageTimeRanges().back()->getDosage());
                        if (repeat2 != nullptr) {
                            const LastingDose* dose2 = dynamic_cast<const LastingDose*>(repeat2->getDosage());
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

std::vector<const FullFormulationAndRoute*> MultiComputingAdjustments::selectFormulationAndRoutes(
        FormulationAndRouteSelectionOption _option,
        const FormulationAndRoutes& _availableFormulationAndRoutes,
        const DosageHistory& _dosageHistory)
{
    std::vector<const FullFormulationAndRoute*> result;

    switch (_option) {
    case FormulationAndRouteSelectionOption::LastFormulationAndRoute: {
        if (_dosageHistory.isEmpty()) {
            // If the dosage history is empty, then use the default of DrugModel
            result.push_back(_availableFormulationAndRoutes.getDefault());
        }
        else {
            // We get the last formulation and retrieve the corresponding full spec from the available ones
            FormulationAndRoute formulation = _dosageHistory.getLastFormulationAndRoute();
            const FullFormulationAndRoute* selectedFormulationAndRoute =
                    _availableFormulationAndRoutes.get(formulation);
            if (selectedFormulationAndRoute != nullptr) {
                result.push_back(selectedFormulationAndRoute);
            }
        }
    } break;

    case FormulationAndRouteSelectionOption::DefaultFormulationAndRoute: {
        // Get the default
        result.push_back(_availableFormulationAndRoutes.getDefault());
    } break;
    case FormulationAndRouteSelectionOption::AllFormulationAndRoutes: {
        // Get the entire available list
        for (auto const& f : _availableFormulationAndRoutes) {
            result.push_back(f.get());
        }
    } break;
    }
    return result;
}

ComputingStatus MultiComputingAdjustments::extractCandidates(
        const ComputingTraitAdjustment* _traits,
        const ComputingRequest& _request,
        std::vector<SimpleDosageCandidate>& _candidates,
        bool& _multipleFormulationAndRoutes)
{

    const FormulationAndRoutes& formulationAndRoutes = _request.getDrugModel().getFormulationAndRoutes();

    const DosageHistory& dosageHistory = _request.getDrugTreatment().getDosageHistory();

    std::vector<const FullFormulationAndRoute*> selectedFormulationAndRoutes = selectFormulationAndRoutes(
            _traits->getFormulationAndRouteSelectionOption(), formulationAndRoutes, dosageHistory);

    _multipleFormulationAndRoutes = (selectedFormulationAndRoutes.size() > 1);

    if (selectedFormulationAndRoutes.empty()) {
        m_logger.error("Can not find a formulation and route for adjustment");
        return ComputingStatus::NoFormulationAndRouteForAdjustment;
    }

    // We iterate over the potential formulation and routes
    for (auto const* selectedFormulationAndRoute : selectedFormulationAndRoutes) {

        if (selectedFormulationAndRoute == nullptr) {
            m_logger.error("Can not find a formulation and route for adjustment");
            return ComputingStatus::NoFormulationAndRouteForAdjustment;
        }


        ComputingStatus buildingResult = buildCandidates(selectedFormulationAndRoute, _candidates);

        if (buildingResult != ComputingStatus::Ok) {
            m_logger.error("Can not build adjustment candidates");
            return buildingResult;
        }
    }

    return ComputingStatus::Ok;
}

ComputingStatus MultiComputingAdjustments::compute(
        const ComputingTraitAdjustment* _traits,
        const ComputingRequest& _request,
        std::unique_ptr<ComputingResponse>& _response)
{

    if (_traits == nullptr) {
        m_logger.error("The computing traits sent for computation are nullptr");
        return ComputingStatus::NoComputingTraits;
    }



    int index = 0;

    std::vector<SimpleDosageCandidate> candidates;
    bool multipleFormulationAndRoutes = false;

    // 1. Extract all candidates
    auto status = extractCandidates(_traits, _request, candidates, multipleFormulationAndRoutes);
    if (status != ComputingStatus::Ok) {
        return status;
    }


    // 2. Get the list of drug model analyte group Ids
    std::vector<AnalyteGroupId> allGroupIds;
    for (const auto& analyteGroup : _request.getDrugModel().getAnalyteSets()) {
        allGroupIds.push_back(analyteGroup->getId());
    }

    std::map<AnalyteGroupId, std::shared_ptr<PkModel> > pkModel;

    CovariateSeries covariateSeries;
    DateTime calculationStartTime;

    std::map<AnalyteGroupId, Etas> etas;

    // 2. Use the general extractor to get the covariates, the pkModel, and the calculationStartTime.
    // The parameterSeries and intakeSeries are declared within a new scope, to be sure they
    // won't be used as is.
    // They are only used to get the etas of parameters
    {
        GroupsParameterSetSeries parameterSeries;
        GroupsIntakeSeries intakeSeries;

        ComputingStatus extractionResult = m_utils->m_generalExtractor->generalExtractions(
                _traits,
                _request,
                m_utils->m_models.get(),
                pkModel,
                intakeSeries,
                covariateSeries,
                parameterSeries,
                calculationStartTime);

        if (extractionResult != ComputingStatus::Ok) {
            return extractionResult;
        }




        for (const auto& analyteGroupId : allGroupIds) {
            if (_traits->getComputingOption().getParametersType() == PredictionParameterType::Aposteriori) {
                ComputingStatus aposterioriEtasExtractionResult = m_utils->m_generalExtractor->extractAposterioriEtas(
                        etas[analyteGroupId],
                        _request,
                        analyteGroupId,
                        intakeSeries[analyteGroupId],
                        parameterSeries[analyteGroupId],
                        covariateSeries,
                        calculationStartTime,
                        _traits->getEnd());

                if (aposterioriEtasExtractionResult != ComputingStatus::Ok) {
                    return aposterioriEtasExtractionResult;
                }
            }
        }
    }


    // For each active moiety, a target series
    std::map<ActiveMoietyId, TargetSeries> targetSeries;

    // Extract the targets
    TargetExtractor targetExtractor;
    TargetExtractionOption targetExtractionOption = _traits->getTargetExtractionOption();

    for (const auto& activeMoiety : _request.getDrugModel().getActiveMoieties()) {

        // The final unit depends on the computing options
        TucuUnit finalUnit("ug/l");
        if (_traits->getComputingOption().forceUgPerLiter() == ForceUgPerLiterOption::DoNotForce) {
            finalUnit = activeMoiety->getUnit();
        }

        ComputingStatus targetExtractionResult = targetExtractor.extract(
                activeMoiety->getActiveMoietyId(),
                covariateSeries,
                activeMoiety->getTargetDefinitions(),
                _request.getDrugTreatment().getTargets(),
                _traits->getStart(),
                _traits->getEnd(),
                finalUnit,
                targetExtractionOption,
                targetSeries[activeMoiety->getActiveMoietyId()]);

        if (targetExtractionResult != ComputingStatus::Ok) {
            return targetExtractionResult;
        }
    }



    DateTime initialCalculationTime = calculationStartTime;




    std::vector<DosageAdjustment> dosageCandidates;

    // A vector of vector because each adjustment candidate can have various targets
    std::vector<std::vector<TargetEvaluationResult> > evaluationResults;


    // Iterate over pre-selected candidates
    TargetEvaluator targetEvaluator;
    for (const auto& candidate : candidates) {


        GroupsParameterSetSeries parameterSeries;


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

            newDosage =
                    std::unique_ptr<DosageTimeRange>(createSteadyStateDosage(candidate, _traits->getAdjustmentTime()));

            newHistory = std::make_unique<DosageHistory>();
            newHistory->addTimeRange(*newDosage);
        }
        else {
            newEndTime = _traits->getEnd();

            newDosage =
                    std::unique_ptr<DosageTimeRange>(createDosage(candidate, _traits->getAdjustmentTime(), newEndTime));

            newHistory = _request.getDrugTreatment().getDosageHistory().clone();
            newHistory->mergeDosage(newDosage.get());
        }


        // TODO : To be checked. The adjustment engine should not depend on the user choice
        double nbPointsPerHour = _traits->getNbPointsPerHour();
        nbPointsPerHour = 20;

        ComputingTraitConcentration traits(
                "0", _traits->getAdjustmentTime(), newEndTime, nbPointsPerHour, _traits->getComputingOption());


        GroupsIntakeSeries intakeSeries;
        CovariateSeries unusedCovariateSeries;
        ComputingStatus extractionResult = m_utils->m_generalExtractor->generalExtractions(
                &traits,
                _request.getDrugModel(),
                *newHistory.get(),
                _request.getDrugTreatment().getSamples(),
                _request.getDrugTreatment().getCovariates(),
                m_utils->m_models.get(),
                pkModel,
                intakeSeries,
                unusedCovariateSeries,
                parameterSeries,
                calculationStartTime);

        if (extractionResult != ComputingStatus::Ok) {
            return extractionResult;
        }

        std::vector<ConcentrationPredictionPtr> analytesPredictions;

        GroupsIntakeSeries intakeSeriesPerGroup;

        for (const auto& analyteGroupId : allGroupIds) {


            ConcentrationPredictionPtr pPrediction = std::make_unique<ConcentrationPrediction>();

            IntakeExtractor intakeExtractor;
            double nbPointsPerHour = _traits->getNbPointsPerHour();
            ComputingStatus intakeExtractionResult = intakeExtractor.extract(
                    *newHistory,
                    calculationStartTime,
                    newEndTime,
                    nbPointsPerHour,
                    _request.getDrugModel().getAnalyteSet(analyteGroupId)->getDoseUnit(),
                    intakeSeriesPerGroup[analyteGroupId],
                    ExtractionOption::EndofDate);

            if (intakeExtractionResult != ComputingStatus::Ok) {
                return intakeExtractionResult;
            }

            auto status = m_utils->m_generalExtractor->convertAnalytes(
                    intakeSeriesPerGroup[analyteGroupId],
                    _request.getDrugModel(),
                    _request.getDrugModel().getAnalyteSet(analyteGroupId));
            if (status != ComputingStatus::Ok) {
                return status;
            }

            ComputingStatus intakeAssociationResult = IntakeToCalculatorAssociator::associate(
                    intakeSeriesPerGroup[analyteGroupId], *pkModel[analyteGroupId]);

            if (intakeAssociationResult != ComputingStatus::Ok) {
                m_logger.error("Can not associate intake calculators for the specified route");
                return intakeAssociationResult;
            }



            ComputingStatus predictionComputingResult;

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

            if (predictionComputingResult != ComputingStatus::Ok) {
                m_logger.error("Error with the computation of a single adjustment candidate");
                return predictionComputingResult;
            }
            else {

                // The final unit depends on the computing options
                TucuUnit finalUnit("ug/l");
                if (_traits->getComputingOption().forceUgPerLiter() == ForceUgPerLiterOption::DoNotForce) {
                    finalUnit = _request.getDrugModel().getActiveMoieties()[0]->getUnit();
                }
                for (size_t i = 0; i < pPrediction->getValues().size(); i++) {
                    Tucuxi::Common::UnitManager::updateAndConvertToUnit<
                            Tucuxi::Common::UnitManager::UnitType::Concentration,
                            Concentrations>(
                            pPrediction->getModifiableValues()[i],
                            _request.getDrugModel().getActiveMoieties()[0]->getUnit(),
                            finalUnit);
                }
                analytesPredictions.push_back(std::move(pPrediction));
            }
        }

        std::vector<ConcentrationPredictionPtr> activeMoietiesPredictions;

        if (!_request.getDrugModel().isSingleAnalyte()) {

            for (const auto& activeMoiety : _request.getDrugModel().getActiveMoieties()) {
                ConcentrationPredictionPtr activeMoietyPrediction = std::make_unique<ConcentrationPrediction>();
                ComputingStatus activeMoietyComputingResult =
                        m_utils->computeActiveMoiety(activeMoiety.get(), analytesPredictions, activeMoietyPrediction);
                if (activeMoietyComputingResult != ComputingStatus::Ok) {
                    return activeMoietyComputingResult;
                }
                activeMoietiesPredictions.push_back(std::move(activeMoietyPrediction));
            }
        }
        else {
            activeMoietiesPredictions.push_back(std::move(analytesPredictions[0]));
        }

        std::vector<TargetEvaluationResult> candidateResults;
        bool isValidCandidate = true;

        if (targetSeries.empty()) {
            isValidCandidate = false;
        }

        size_t moietyIndex = 0;

        // Check wheter value (after extraction) is in target or not
        for (const auto& activeMoiety : _request.getDrugModel().getActiveMoieties()) {
            for (const auto& target : targetSeries[activeMoiety->getActiveMoietyId()]) {
                TargetEvaluationResult localResult;

                // Now the score calculation
                ComputingStatus evaluationResult = targetEvaluator.evaluate(
                        *activeMoietiesPredictions[moietyIndex].get(),
                        intakeSeriesPerGroup[allGroupIds[0]],
                        target,
                        localResult);

                // Here we do not compare to Result::Ok, because the result can also be
                // Result::InvalidCandidate
                if (evaluationResult == ComputingStatus::TargetEvaluationError) {
                    m_logger.error("Error in the evaluation of targets");
                    return evaluationResult;
                }

                // If the candidate is valid:
                if (evaluationResult == ComputingStatus::Ok) {
                    candidateResults.push_back(localResult);
                }
                else {
                    isValidCandidate = false;
                }
            }
            moietyIndex++;
        }

        if (isValidCandidate) {

#if 0
            // Stream that to file, only for debugging purpose
            // To be removed later on, or at least commented
            std::string fileName = "candidate_" + std::to_string(index) + ".dat";
            pPrediction->streamToFile(fileName);
#endif // 0
            index++;

            DosageAdjustment dosage;
            dosage.m_targetsEvaluation = candidateResults;


            if (_traits->getSteadyStateTargetOption() == SteadyStateTargetOption::AtSteadyState) {
                // In that case we create the dosage with a good end time
                newDosage = std::unique_ptr<DosageTimeRange>(
                        createDosage(candidate, _traits->getAdjustmentTime(), _traits->getEnd()));
            }
            dosage.m_history.addTimeRange(*newDosage);


            // The final unit depends on the computing options
            TucuUnit finalUnit("ug/l");
            if (_traits->getComputingOption().forceUgPerLiter() == ForceUgPerLiterOption::DoNotForce) {
                finalUnit = _request.getDrugModel().getActiveMoieties()[0]->getUnit();
            }

            // TODO : There is something wrong here that will go badly when we'll have multi-analytes drugmodels
            for (const auto& analyteGroupId : allGroupIds) {
                for (size_t i = 0; i < intakeSeriesPerGroup[analyteGroupId].size(); i++) {
                    TimeOffsets times = activeMoietiesPredictions[0]->getTimes()[i];
                    DateTime start = intakeSeriesPerGroup[analyteGroupId][i].getEventTime();
                    DateTime end = start + std::chrono::milliseconds(static_cast<int>(times.back()) * 1000 * 3600);
                    if (start >= _traits->getAdjustmentTime()) {
                        CycleData cycle(start, end, finalUnit);
                        cycle.addData(
                                times,
                                Tucuxi::Common::UnitManager::convertToUnit<
                                        Tucuxi::Common::UnitManager::UnitType::Concentration>(
                                        activeMoietiesPredictions[0]->getValues()[i],
                                        _request.getDrugModel().getActiveMoieties()[0]->getUnit(),
                                        finalUnit));

                        // Here we do not add parameters and covariates information on the cycle data, as
                        // it only serves to see if a loading dose or rest period is worth it

                        dosage.addCycleData(cycle);
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


    GroupsParameterSetSeries parameterSeries;


    std::vector<DosageAdjustment> finalCandidates;
    finalCandidates = sortAndFilterCandidates(dosageCandidates, _traits->getBestCandidatesOption());

    // Verify if Loading dose or Rest period is needed
    ComputingStatus addResult = addLoadOrRest(
            finalCandidates, _traits, _request, allGroupIds, initialCalculationTime, pkModel, parameterSeries, etas);
    if (addResult != ComputingStatus::Ok) {
        return addResult;
    }


    ComputingStatus generateResult = generatePredictions(
            finalCandidates, _traits, _request, allGroupIds, initialCalculationTime, pkModel, parameterSeries, etas);
    if (generateResult != ComputingStatus::Ok) {
        return generateResult;
    }


    // Now we have adjustments, predictions, and target evaluation results, let's build the response
    std::unique_ptr<AdjustmentData> resp = std::make_unique<AdjustmentData>(_request.getId());

    resp->setAdjustments(finalCandidates);

    // Finally add the response to the set of responses
    _response->addResponse(std::move(resp));

    return ComputingStatus::Ok;
}


ComputingStatus MultiComputingAdjustments::addLoadOrRest(
        std::vector<DosageAdjustment>& _dosages,
        const ComputingTraitAdjustment* _traits,
        const ComputingRequest& _request,
        const std::vector<AnalyteGroupId>& _allGroupIds,
        const Common::DateTime& _calculationStartTime,
        std::map<AnalyteGroupId, std::shared_ptr<PkModel> >& _pkModel,
        GroupsParameterSetSeries& _parameterSeries,
        std::map<AnalyteGroupId, Etas>& _etas)
{
    for (auto& dosage : _dosages) {
        ComputingStatus result = addLoadOrRest(
                dosage, _traits, _request, _allGroupIds, _calculationStartTime, _pkModel, _parameterSeries, _etas);
        if (result != ComputingStatus::Ok) {
            return result;
        }
    }
    return ComputingStatus::Ok;
}


typedef struct
{
    DosageAdjustment loadingDosage; // NOLINT(readability-identifier-naming)
    double score;                   // NOLINT(readability-identifier-naming)
    Duration interval;              // NOLINT(readability-identifier-naming)
} LoadingCandidate;

typedef struct
{
    DosageAdjustment restDosage; // NOLINT(readability-identifier-naming)
    double score;                // NOLINT(readability-identifier-naming)
    Duration interval;           // NOLINT(readability-identifier-naming)
} RestCandidate;

ComputingStatus MultiComputingAdjustments::addLoadOrRest(
        DosageAdjustment& _dosage,
        const ComputingTraitAdjustment* _traits,
        const ComputingRequest& _request,
        const std::vector<AnalyteGroupId>& _allGroupIds,
        const Common::DateTime& _calculationStartTime,
        std::map<AnalyteGroupId, std::shared_ptr<PkModel> >& _pkModel,
        GroupsParameterSetSeries& _parameterSeries,
        std::map<AnalyteGroupId, Etas>& _etas)
{
    bool modified = false;
    if (_traits->getRestPeriodOption() == RestPeriodOption::RestPeriodAllowed) {
        auto result2 =
                addRest(_dosage,
                        _traits,
                        _request,
                        _allGroupIds,
                        _calculationStartTime,
                        _pkModel,
                        _parameterSeries,
                        _etas,
                        modified);
        if (result2 != ComputingStatus::Ok) {
            return result2;
        }
        if (modified) {
            return ComputingStatus::Ok;
        }
    }
    if (_traits->getLoadingOption() == LoadingOption::LoadingDoseAllowed) {
        auto result1 =
                addLoad(_dosage,
                        _traits,
                        _request,
                        _allGroupIds,
                        _calculationStartTime,
                        _pkModel,
                        _parameterSeries,
                        _etas,
                        modified);
        if (result1 != ComputingStatus::Ok) {
            return result1;
        }
    }
    return ComputingStatus::Ok;
}


ComputingStatus MultiComputingAdjustments::addRest(
        DosageAdjustment& _dosage,
        const ComputingTraitAdjustment* _traits,
        const ComputingRequest& _request,
        const std::vector<AnalyteGroupId>& _allGroupIds,
        const Common::DateTime& _calculationStartTime,
        std::map<AnalyteGroupId, std::shared_ptr<PkModel> >& _pkModel,
        GroupsParameterSetSeries& _parameterSeries,
        std::map<AnalyteGroupId, Etas>& _etas,
        bool& _modified)
{
    if (_traits->getRestPeriodOption() == RestPeriodOption::NoRestPeriod) {
        return ComputingStatus::Ok;
    }

    const DosageRepeat* repeat =
            static_cast<const DosageRepeat*>(_dosage.m_history.getDosageTimeRanges()[0]->getDosage());
    const LastingDose* dosage = static_cast<const LastingDose*>(repeat->getDosage());
    Duration interval = dosage->getTimeStep();
    auto dose = dosage->getDose();


    const Concentrations& lastConcentrations = _dosage.getData().back().m_concentrations[0];
    double steadyStateResidual = lastConcentrations.back();

    FormulationAndRoute formulationAndRoute = dosage->getLastFormulationAndRoute();


    // TODO : This is wrong to take the default here
    const FullFormulationAndRoute* fullFormulationAndRoute =
            _request.getDrugModel().getFormulationAndRoutes().getDefault();
    std::vector<MultiComputingAdjustments::SimpleDosageCandidate> candidates;


    // Add candidates in order to decrease dosage
    const ValidDurations* intervals = fullFormulationAndRoute->getValidIntervals();
    std::vector<Tucuxi::Common::Duration> intervalValues;
    if (intervals != nullptr) {
        intervalValues = intervals->getDurations();
    }
    TucuUnit dUnit("ug");
    for (auto interval : intervalValues) {
        candidates.push_back(
                {fullFormulationAndRoute->getFormulationAndRoute(),
                 0,
                 dUnit,
                 interval,
                 Duration(std::chrono::hours(0))});
    }

    std::vector<LoadingCandidate> loadingCandidates;

    for (const auto& candidate : candidates) {

        DosageAdjustment loadingDosage;
        // Create the loading dose
        std::unique_ptr<DosageTimeRange> newDosage = std::unique_ptr<DosageTimeRange>(
                createLoadingDosageOrRestPeriod(candidate, _traits->getAdjustmentTime()));
        loadingDosage.m_history.addTimeRange(*newDosage);

        ComputingStatus generateResult = generatePrediction(
                loadingDosage,
                _traits,
                _request,
                _allGroupIds,
                _calculationStartTime,
                _pkModel,
                _parameterSeries,
                _etas);
        if (generateResult != ComputingStatus::Ok) {
            return generateResult;
        }

        const Concentrations& concentrations = loadingDosage.getData()[0].m_concentrations[0];
        double residual = concentrations.back();
        double score = steadyStateResidual - residual;
        loadingCandidates.push_back({loadingDosage, score, candidate.m_interval});
    }

    Duration loadingInterval;

    double bestScore = 1000000000.0;
    size_t bestIndex = 0;
    size_t index = 0;
    for (const auto& candidate : loadingCandidates) {
        if (std::abs(candidate.score) < bestScore) {
            bestScore = std::abs(candidate.score);
            loadingInterval = candidate.interval;
            bestIndex = index;
        }
        index++;
    }

    double existingDosageScore;
    {

        DosageAdjustment loadingDosage;
        loadingDosage.m_history = _dosage.m_history;

        ComputingStatus generateResult = generatePrediction(
                loadingDosage,
                _traits,
                _request,
                _allGroupIds,
                _calculationStartTime,
                _pkModel,
                _parameterSeries,
                _etas);
        if (generateResult != ComputingStatus::Ok) {
            return generateResult;
        }

        const Concentrations& concentrations = loadingDosage.getData()[0].m_concentrations[0];
        double residual = concentrations.back();
        existingDosageScore = steadyStateResidual - residual;
    }

    if (bestScore >= std::abs(existingDosageScore)) {
        _modified = false;
        return ComputingStatus::Ok;
    }


    {
        const DosageRepeat* repeat = static_cast<const DosageRepeat*>(
                loadingCandidates[bestIndex].loadingDosage.m_history.getDosageTimeRanges()[0]->getDosage());
        const LastingDose* dosage = static_cast<const LastingDose*>(repeat->getDosage());
        Duration loadInterval = dosage->getTimeStep();
        auto loadDose = dosage->getDose();

        if ((dose == loadDose) && (interval == loadInterval)) {
            // We do not want to add the same dosage before the current adjustment. It is non sense
            _modified = false;
            return ComputingStatus::Ok;
        }
    }

    //Add the new time range to dosage history (load)
    DosageHistory steadyStateHistory = _dosage.m_history;
    DosageTimeRange range = *steadyStateHistory.getDosageTimeRanges()[0].get();
    auto d = range.getDosage();
    DosageTimeRange newRange(range.getStartDate() + loadingInterval, range.getEndDate(), *d);
    _dosage.m_history = DosageHistory();
    _dosage.m_history.addTimeRange(
            *loadingCandidates[bestIndex].loadingDosage.m_history.getDosageTimeRanges()[0].get());
    _dosage.m_history.mergeDosage(&newRange);

    _modified = true;
    return ComputingStatus::Ok;
}

ComputingStatus MultiComputingAdjustments::addLoad(
        DosageAdjustment& _dosage,
        const ComputingTraitAdjustment* _traits,
        const ComputingRequest& _request,
        const std::vector<AnalyteGroupId>& _allGroupIds,
        const Common::DateTime& _calculationStartTime,
        std::map<AnalyteGroupId, std::shared_ptr<PkModel> >& _pkModel,
        GroupsParameterSetSeries& _parameterSeries,
        std::map<AnalyteGroupId, Etas>& _etas,
        bool& _modified)
{
    if (_traits->getLoadingOption() == LoadingOption::NoLoadingDose) {
        return ComputingStatus::Ok;
    }

    const DosageRepeat* repeat =
            static_cast<const DosageRepeat*>(_dosage.m_history.getDosageTimeRanges()[0]->getDosage());
    const LastingDose* dosage = static_cast<const LastingDose*>(repeat->getDosage());
    Duration interval = dosage->getTimeStep();
    auto dose = dosage->getDose();


    const Concentrations& lastConcentrations = _dosage.getData().back().m_concentrations[0];
    double steadyStateResidual = lastConcentrations.back();

    FormulationAndRoute formulationAndRoute = dosage->getLastFormulationAndRoute();


    // TODO : This is wrong to take the default here
    const FullFormulationAndRoute* fullFormulationAndRoute =
            _request.getDrugModel().getFormulationAndRoutes().getDefault();
    std::vector<MultiComputingAdjustments::SimpleDosageCandidate> candidates;
    // Add candidates in order to increase dosage
    ComputingStatus buildResult = buildCandidatesForInterval(fullFormulationAndRoute, interval, candidates);
    if (buildResult != ComputingStatus::Ok) {
        return buildResult;
    }

    /*
    const ValidDurations * intervals = fullFormulationAndRoute->getValidIntervals();
    std::vector<Tucuxi::Common::Duration> intervalValues;
    if (intervals != nullptr) {
        intervalValues = intervals->getDurations();
    }
    Unit dUnit("ug");
    for (auto interval : intervalValues) {
        candidates.push_back({fullFormulationAndRoute->getFormulationAndRoute(), 0, dUnit, interval, Duration(std::chrono::hours(0))});
    }
*/





    std::vector<LoadingCandidate> loadingCandidates;

    for (const auto& candidate : candidates) {

        DosageAdjustment loadingDosage;
        // Create the loading dose
        std::unique_ptr<DosageTimeRange> newDosage = std::unique_ptr<DosageTimeRange>(
                createLoadingDosageOrRestPeriod(candidate, _traits->getAdjustmentTime()));
        loadingDosage.m_history.addTimeRange(*newDosage);

        ComputingStatus generateResult = generatePrediction(
                loadingDosage,
                _traits,
                _request,
                _allGroupIds,
                _calculationStartTime,
                _pkModel,
                _parameterSeries,
                _etas);
        if (generateResult != ComputingStatus::Ok) {
            return generateResult;
        }

        const Concentrations& concentrations = loadingDosage.getData()[0].m_concentrations[0];
        double residual = concentrations.back();
        double score = steadyStateResidual - residual;
        loadingCandidates.push_back({loadingDosage, score, candidate.m_interval});
    }

    Duration loadingInterval;

    double bestScore = 1000000000.0;
    size_t bestIndex = 0;
    size_t index = 0;
    for (const auto& candidate : loadingCandidates) {
        if (std::abs(candidate.score) < bestScore) {
            bestScore = std::abs(candidate.score);
            loadingInterval = candidate.interval;
            bestIndex = index;
        }
        index++;
    }


    double existingDosageScore;
    {

        DosageAdjustment loadingDosage;
        loadingDosage.m_history = _dosage.m_history;

        ComputingStatus generateResult = generatePrediction(
                loadingDosage,
                _traits,
                _request,
                _allGroupIds,
                _calculationStartTime,
                _pkModel,
                _parameterSeries,
                _etas);
        if (generateResult != ComputingStatus::Ok) {
            return generateResult;
        }

        const Concentrations& concentrations = loadingDosage.getData()[0].m_concentrations[0];
        double residual = concentrations.back();
        existingDosageScore = steadyStateResidual - residual;
    }

    if (bestScore >= std::abs(existingDosageScore)) {
        _modified = false;
        return ComputingStatus::Ok;
    }

    {
        const DosageRepeat* repeat = static_cast<const DosageRepeat*>(
                loadingCandidates[bestIndex].loadingDosage.m_history.getDosageTimeRanges()[0]->getDosage());
        const LastingDose* dosage = static_cast<const LastingDose*>(repeat->getDosage());
        Duration loadInterval = dosage->getTimeStep();
        auto loadDose = dosage->getDose();

        if ((dose == loadDose) && (interval == loadInterval)) {
            // We do not want to add the same dosage before the current adjustment. It is non sense
            _modified = false;
            return ComputingStatus::Ok;
        }
    }

    //Add the new time range to dosage history (load)
    DosageHistory steadyStateHistory = _dosage.m_history;
    DosageTimeRange range = *steadyStateHistory.getDosageTimeRanges()[0].get();
    auto d = range.getDosage();
    DosageTimeRange newRange(range.getStartDate() + loadingInterval, range.getEndDate(), *d);
    _dosage.m_history = DosageHistory();
    _dosage.m_history.addTimeRange(
            *loadingCandidates[bestIndex].loadingDosage.m_history.getDosageTimeRanges()[0].get());
    _dosage.m_history.mergeDosage(&newRange);

    _modified = true;
    return ComputingStatus::Ok;
}

ComputingStatus MultiComputingAdjustments::generatePredictions(
        std::vector<DosageAdjustment>& _dosages,
        const ComputingTraitAdjustment* _traits,
        const ComputingRequest& _request,
        const std::vector<AnalyteGroupId>& _allGroupIds,
        const DateTime& _calculationStartTime,
        std::map<AnalyteGroupId, std::shared_ptr<PkModel> >& _pkModel,
        GroupsParameterSetSeries& _parameterSeries,
        std::map<AnalyteGroupId, Etas>& _etas)
{
    for (auto& dosage : _dosages) {
        ComputingStatus result = generatePrediction(
                dosage, _traits, _request, _allGroupIds, _calculationStartTime, _pkModel, _parameterSeries, _etas);
        if (result != ComputingStatus::Ok) {
            return result;
        }
    }
    return ComputingStatus::Ok;
}


ComputingStatus MultiComputingAdjustments::generatePrediction(
        DosageAdjustment& _dosage,
        const ComputingTraitAdjustment* _traits,
        const ComputingRequest& _request,
        const std::vector<AnalyteGroupId>& _allGroupIds,
        const Common::DateTime& _calculationStartTime,
        std::map<AnalyteGroupId, std::shared_ptr<PkModel> >& _pkModel,
        GroupsParameterSetSeries& _parameterSeries,
        std::map<AnalyteGroupId, Etas>& _etas)
{
    TMP_UNUSED_PARAMETER(_parameterSeries);
    DateTime newEndTime = _traits->getEnd();

    std::unique_ptr<DosageHistory> newHistory;

    newHistory = _request.getDrugTreatment().getDosageHistory().clone();
    for (const auto& timeRange : _dosage.m_history.getDosageTimeRanges()) {
        newHistory->mergeDosage(timeRange.get());
    }

    GroupsIntakeSeries intakeSeriesPerGroup;


    std::vector<MultiConcentrationPredictionPtr> multianalytesPredictions;


    CovariateSeries covariateSeries;
    DateTime calculationStartTime;

    GroupsParameterSetSeries parameterSeries;
    GroupsIntakeSeries intakeSeries;
    ComputingStatus extractionResult = m_utils->m_generalExtractor->generalExtractions(
            _traits,
            _request.getDrugModel(),
            *newHistory.get(),
            _request.getDrugTreatment().getSamples(),
            _request.getDrugTreatment().getCovariates(),
            m_utils->m_models.get(),
            _pkModel,
            intakeSeries,
            covariateSeries,
            parameterSeries,
            calculationStartTime);
    if (extractionResult != ComputingStatus::Ok) {
        return extractionResult;
    }



    for (const auto& analyteGroupId : _allGroupIds) {
        MultiConcentrationPredictionPtr pPrediction = std::make_unique<MultiConcentrationPrediction>();

        IntakeExtractor intakeExtractor;
        double nbPointsPerHour = _traits->getNbPointsPerHour();
        //Extract all Time range
        ComputingStatus intakeExtractionResult = intakeExtractor.extract(
                *newHistory,
                _calculationStartTime,
                newEndTime,
                nbPointsPerHour,
                _request.getDrugModel().getAnalyteSet(analyteGroupId)->getDoseUnit(),
                intakeSeriesPerGroup[analyteGroupId],
                ExtractionOption::EndofDate);

        auto status = m_utils->m_generalExtractor->convertAnalytes(
                intakeSeriesPerGroup[analyteGroupId],
                _request.getDrugModel(),
                _request.getDrugModel().getAnalyteSet(analyteGroupId));
        if (status != ComputingStatus::Ok) {
            return status;
        }


        if (intakeExtractionResult != ComputingStatus::Ok) {
            return intakeExtractionResult;
        }

        ComputingStatus intakeAssociationResult = IntakeToCalculatorAssociator::associate(
                intakeSeriesPerGroup[analyteGroupId], *_pkModel[analyteGroupId]);

        if (intakeAssociationResult != ComputingStatus::Ok) {
            m_logger.error("Can not associate intake calculators for the specified route");
            return intakeAssociationResult;
        }



        ComputingStatus predictionComputingResult;
        MultiConcentrationCalculator concentrationCalculator;
        predictionComputingResult = concentrationCalculator.computeConcentrations(
                pPrediction,
                false,
                _calculationStartTime,
                newEndTime,
                intakeSeriesPerGroup[analyteGroupId],
                parameterSeries[analyteGroupId],
                _etas[analyteGroupId]);

        if (predictionComputingResult != ComputingStatus::Ok) {
            m_logger.error("Error with the computation of a single adjustment candidate");
            return predictionComputingResult;
        }

        multianalytesPredictions.push_back(std::move(pPrediction));
    }

    std::vector<ConcentrationPredictionPtr> activeMoietiesPredictions;

    /*
    if (!_request.getDrugModel().isSingleAnalyte()) {
       Tucuxi::Core::OperationCollection collection;
        collection.populate();
        std::shared_ptr<Operation> sharedOperation = collection.getOperationFromId("sum2");
        std::unique_ptr<Operation> activeMoietyOperation = std::unique_ptr<Operation>(sharedOperation.get()->clone());
        std::vector<AnalyteId> analyteList0;
        std::unique_ptr<ActiveMoiety> activeMoiety0 = std::make_unique<ActiveMoiety>(ActiveMoietyId("activeMoietyMulti"), TucuUnit("ug/l"), analyteList0, std::move(activeMoietyOperation));

        ActiveMoiety *a1 = new ActiveMoiety();
        ActiveMoiety *a2 = new ActiveMoiety();

        for (const auto & activeMoiety : _request.getDrugModel().getActiveMoieties())
            {
            //a1 = activeMoiety.get();
            ConcentrationPredictionPtr activeMoietyPrediction = std::make_unique<ConcentrationPrediction>();
            ComputingStatus activeMoietyComputingResult = m_utils->computeMultiActiveMoiety(activeMoiety.get(), multianalytesPredictions, activeMoietyPrediction);
            if (activeMoietyComputingResult != ComputingStatus::Ok) {
                return activeMoietyComputingResult;
            }
            activeMoietiesPredictions.push_back(std::move(activeMoietyPrediction));
        }

    // else {
    //     activeMoietiesPredictions.push_back(std::move(analytesPredictions[0]));
    // }

    // We clear the prediction data
    _dosage.getModifiableData().clear();


    auto defaultGroupId = _request.getDrugModel().getAnalyteSets()[0]->getId();

    IntakeSeries recordedIntakes;
    selectRecordedIntakes(recordedIntakes, intakeSeriesPerGroup[defaultGroupId], _calculationStartTime, newEndTime);

    if ((recordedIntakes.size() != multianalytesPredictions[0]->getTimes().size()) ||
            (recordedIntakes.size() != multianalytesPredictions[0]->getValues().size())) {
        return ComputingStatus::RecordedIntakesSizeError;
    }



    for (size_t i = 0; i < recordedIntakes.size(); i++) {
        if (i >= multianalytesPredictions[0]->getTimes().size()) {
            break;
        }
        TimeOffsets times = multianalytesPredictions[0]->getTimes()[i];
        DateTime start = recordedIntakes[i].getEventTime();
        DateTime end = start + std::chrono::milliseconds(static_cast<int>(times.back()) * 1000 * 3600);
        if (start >= _traits->getAdjustmentTime()) {

            ComputingStatus recordStatus = MultiComputingComponent::recordCycle(
                        _traits,
                        _request,
                        _dosage,
                        start,
                        end,
                        times,
                        activeMoietiesPredictions,
                        multianalytesPredictions,
                        i,
                        _etas,
                        parameterSeries);
            if (recordStatus != ComputingStatus::Ok) {
                return recordStatus;
            }

        }
    }

    MultiComputingComponent::endRecord(_traits, _request, _dosage);

    return ComputingStatus::Ok;
}

*/

} // namespace Core
} // namespace Core
} // namespace Tucuxi
