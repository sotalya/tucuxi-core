/*
* Copyright (C) 2017 Tucuxi SA
*/


#include "computingcomponent.h"
#include "tucucore/drugmodel/drugmodel.h"

#include "tucucore/computingcomponent.h"
#include "tucucore/drugtreatment/drugtreatment.h"
#include "tucucore/intakeextractor.h"
#include "tucucore/covariateextractor.h"
#include "tucucore/targetextractor.h"
#include "tucucore/sampleextractor.h"
#include "tucucore/parametersextractor.h"
#include "tucucore/intakeintervalcalculator.h"
#include "tucucore/concentrationcalculator.h"
#include "tucucore/aposteriorietascalculator.h"
#include "tucucore/computingservice/icomputingservice.h"
#include "tucucore/computingservice/computingrequest.h"
#include "tucucore/computingservice/computingresponse.h"
#include "tucucore/computingservice/computingtrait.h"
#include "tucucore/intaketocalculatorassociator.h"
#include "tucucore/pkmodel.h"
#include "tucucore/montecarlopercentilecalculator.h"
#include "tucucore/percentilesprediction.h"
#include "tucucore/targetevaluationresult.h"
#include "tucucore/targetevaluator.h"


namespace Tucuxi {
namespace Core {

Tucuxi::Common::Interface* ComputingComponent::createComponent()
{
    ComputingComponent *cmp = new ComputingComponent();

    cmp->initialize();

    return dynamic_cast<IComputingService*>(cmp);
}


ComputingComponent::ComputingComponent()
{
    registerInterface(dynamic_cast<IComputingService*>(this));
}


ComputingComponent::~ComputingComponent()
{
}

bool ComputingComponent::initialize()
{
    m_models = new PkModelCollection();
    if (!defaultPopulate(*m_models)) {
        m_logger.error("Could not populate the Pk models collection. No model will be available");
        return false;
    }
    return true;
}


ComputingResult ComputingComponent::compute(const ComputingRequest &_request, std::unique_ptr<ComputingResponse> &_response)
{
    // First ensure there is at least a Pk Model available
    if (m_models->getPkModelList().size() == 0) {
        m_logger.error("No Pk Model loaded. Impossible to perform computation");
        return ComputingResult::Error;
    }

    // A simple iteration on the ComputingTraits. Each one is responsible to fill the _response object with
    // a new computing response
    ComputingTraits::Iterator it = _request.getComputingTraits().begin();
    ComputingResult result = ComputingResult::Success;
    while (it != _request.getComputingTraits().end()) {

        if (it->get()->compute(*this, _request, _response) != ComputingResult::Success)
            result = ComputingResult::Error;
        it++;
    }

    return result;
    /*
    ConcentrationPredictionPtr prediction;

    IntakeSeries intakes;
    CovariateSeries covariates;
    ParameterSetSeries parameters;
    // These drug parameters shall be the ones of the drugmodel
    ParameterDefinitions drugParameters;
    // Extract intakes, covariates, parameters and samples
    IntakeExtractor::extract(*m_treatment->getDosageHistory(_request.getType() != PredictionType::Population), _request.getStart(), _request.getEnd(), intakes);
    CovariateExtractor(m_drug->getCovariates(), m_treatment->getCovariates(), _request.getStart(), _request.getEnd()).extract(covariates);
    ParametersExtractor(covariates, drugParameters, _request.getStart(), _request.getEnd()).extract(parameters);

    if (intakes.size() == 0) {
    m_logger.debug("No intakes, no calc.");
    return nullptr;
    }

    prediction = std::make_unique<ConcentrationPrediction>();

    switch (_request.getType())
    {
    case PredictionType::Population:
    computePopulation(prediction, _request.getIsAll(), _request.getNbPoints(), intakes, parameters);
    break;

    case PredictionType::Apiori:
    computeApriori(prediction, _request.getIsAll(), _request.getNbPoints(), intakes, parameters);
    break;

    case PredictionType::Aposteriori: {
    DateTime lastSampleDate;
    SampleSeries samples;
    SampleExtractor::extract(m_treatment->getSamples(), _request.getStart(), _request.getEnd(), samples);
    if (samples.size() > 0) {
    SampleSeries::iterator _smpend = samples.end();
    lastSampleDate = _smpend->getEventTime();
    }
    IntakeSeries intakesForEtas;
    IntakeExtractor::extract(*m_treatment->getDosageHistory(_request.getType() != PredictionType::Population), _request.getStart(), lastSampleDate, intakesForEtas);

    /// TODO YJE
    Etas etas;
    OmegaMatrix omega;
    SigmaResidualErrorModel residualErrorModel;
    //            extractError(m_drug->getErrorModel(), m_drug->getParemeters(), omega, residualErrorModel);
    APosterioriEtasCalculator().computeAposterioriEtas(intakesForEtas, parameters, omega, residualErrorModel, samples, etas);
    computeAposteriori(prediction, _request.getIsAll(), _request.getNbPoints(), intakes, parameters, etas);
    break;
    }

    default:
    break;
    }

    return prediction;
*/
}

ComputingResult ComputingComponent::generalExtractions(
        const ComputingTraitStandard *_traits,
        const ComputingRequest &_request,
        std::shared_ptr<PkModel> &_pkModel,
        IntakeSeries &_intakeSeries,
        CovariateSeries &_covariatesSeries,
        ParameterSetSeries &_parameterSeries
        )
{

    IntakeExtractor intakeExtractor;
    int nIntakes = intakeExtractor.extract(_request.getDrugTreatment().getDosageHistory(false), _traits->getStart(), _traits->getEnd(), _intakeSeries, _traits->getCycleSize());
    TMP_UNUSED_PARAMETER(nIntakes);


    const std::string pkModelId = _request.getDrugModel().getPkModelId();

    // Should get rid of the next 3 lines
    const std::string analyteId = "imatinib";
    const Formulation formulation = Formulation::ParenteralSolution;
    const AdministrationRoute route = AdministrationRoute::IntravenousBolus; // ???

    _pkModel = m_models->getPkModelFromId(pkModelId);

    if (_pkModel == nullptr) {
        m_logger.error("Can not find a Pk Model for the calculation");
        return ComputingResult::Error;
    }


    IntakeToCalculatorAssociator::Result intakeExtractionResult = IntakeToCalculatorAssociator::associate(_intakeSeries, *_pkModel.get());

    if (intakeExtractionResult != IntakeToCalculatorAssociator::Result::Ok) {
        m_logger.error("Can not associate intake calculators for the specified route");
        return ComputingResult::Error;
    }

    CovariateExtractor covariateExtractor(_request.getDrugModel().getCovariates(),
                                          _request.getDrugTreatment().getCovariates(),
                                          _traits->getStart(),
                                          _traits->getEnd());
    CovariateExtractor::Result covariateExtractionResult = covariateExtractor.extract(_covariatesSeries);

    if (covariateExtractionResult != CovariateExtractor::Result::Ok) {
        m_logger.error("Can not extract covariates");
        return ComputingResult::Error;
    }

    ParameterDefinitionIterator it = _request.getDrugModel().getParameterDefinitions(analyteId, formulation, route);

    ParametersExtractor parameterExtractor(_covariatesSeries,
                                           it,
                                           _traits->getStart(),
                                           _traits->getEnd());

    ParametersExtractor::Result parametersExtractionResult = parameterExtractor.extract(_parameterSeries);

    if (parametersExtractionResult != ParametersExtractor::Result::Ok) {
        m_logger.error("Can not extract parameters");
        return ComputingResult::Error;
    }

    return ComputingResult::Success;

}

ComputingResult ComputingComponent::compute(
        const ComputingTraitConcentration *_traits,
        const ComputingRequest &_request,
        std::unique_ptr<ComputingResponse> &_response)
{

    if (_traits == nullptr) {
        m_logger.error("The computing traits sent for computation are nullptr");
        return ComputingResult::Error;
    }

    std::shared_ptr<PkModel> pkModel;

    IntakeSeries intakeSeries;
    CovariateSeries covariateSeries;
    ParameterSetSeries parameterSeries;

    ComputingResult extractionResult = generalExtractions(_traits,
                                                          _request,
                                                          pkModel,
                                                          intakeSeries,
                                                          covariateSeries,
                                                          parameterSeries);

    if (extractionResult != ComputingResult::Success) {
        return extractionResult;
    }

    // Now ready to do the real computing with all the extracted values

    ConcentrationPredictionPtr pPrediction = std::make_unique<ConcentrationPrediction>();
    ComputationResult computationResult = computePopulation(
                pPrediction,
                false,
                _traits->getStart(),
                _traits->getEnd(),
                intakeSeries,
                parameterSeries);

    if (computationResult == ComputationResult::Success &&
            intakeSeries.size() == pPrediction->getTimes().size() &&
            intakeSeries.size() == pPrediction->getValues().size())
    {
        std::unique_ptr<SinglePredictionResponse> resp = std::make_unique<SinglePredictionResponse>();

        for (size_t i = 0; i < intakeSeries.size(); i++) {
            TimeOffsets times = pPrediction->getTimes().at(i);
            DateTime start = intakeSeries.at(i).getEventTime();
            DateTime end = start + std::chrono::milliseconds(static_cast<int>(times.at(times.size() - 1)) * 1000);
            CycleData cycle(start, end, Unit("ug/l"));
            cycle.addData(times, pPrediction->getValues().at(i), 0);
            resp->addCycleData(cycle);
        }

        _response->addResponse(std::move(resp));
        return ComputingResult::Success;
    }
    else {
        return ComputingResult::Error;
    }

}

ComputingResult ComputingComponent::compute(
        const ComputingTraitPercentiles *_traits,
        const ComputingRequest &_request,
        std::unique_ptr<ComputingResponse> &_response)
{
    if (_traits == nullptr) {
        m_logger.error("The computing traits sent for computation are nullptr");
        return ComputingResult::Error;
    }

    std::shared_ptr<PkModel> pkModel;

    IntakeSeries intakeSeries;
    CovariateSeries covariateSeries;
    ParameterSetSeries parameterSeries;

    ComputingResult extractionResult = generalExtractions(_traits,
                                                          _request,
                                                          pkModel,
                                                          intakeSeries,
                                                          covariateSeries,
                                                          parameterSeries);

    if (extractionResult != ComputingResult::Success) {
        return extractionResult;
    }

    // Now ready to do the real computing with all the extracted values


    std::unique_ptr<Tucuxi::Core::IAprioriPercentileCalculator> calculator(new Tucuxi::Core::AprioriMonteCarloPercentileCalculator());



    Tucuxi::Core::PercentilesPrediction percentiles;

    Tucuxi::Core::PercentileRanks percentileRanks;
    Tucuxi::Core::OmegaMatrix omega;
    Tucuxi::Core::SigmaResidualErrorModel residualErrorModel;
    Tucuxi::Core::Etas etas;
    Tucuxi::Core::ComputingAborter *aborter = nullptr;

    //percentileRanks = {5, 10, 25, 50, 75, 90, 95};
    //percentileRanks = {10, 25, 75, 90};
    percentileRanks = {5, 25, 75, 95};
    percentileRanks = _traits->getRanks();

    Sigma sigma(1);
    sigma(0) = 0.3138;
    residualErrorModel.setErrorModel(SigmaResidualErrorModel::ResidualErrorType::PROPORTIONAL);
    residualErrorModel.setSigma(sigma);


    omega = Tucuxi::Core::OmegaMatrix(2,2);
    omega(0,0) = 0.356 * 0.356; // Variance of CL
    omega(0,1) = 0.798 * 0.356 * 0.629; // Covariance of CL and V
    omega(1,1) = 0.629 * 0.629; // Variance of V
    omega(1,0) = 0.798 * 0.356 * 0.629; // Covariance of CL and V

    // Set initial etas to 0 for CL and V
    etas.push_back(0.0);
    etas.push_back(0.0);

    Tucuxi::Core::IPercentileCalculator::ComputingResult computationResult;

    Tucuxi::Core::ConcentrationCalculator concentrationCalculator;
    computationResult = calculator->calculate(
                percentiles,
                intakeSeries,
                parameterSeries,
                omega,
                residualErrorModel,
                etas,
                percentileRanks,
                concentrationCalculator,
                aborter);

    // We use a single prediction to get back time offsets. Could be optimized
    ConcentrationPredictionPtr pPrediction = std::make_unique<ConcentrationPrediction>();
    ComputationResult predictionComputationResult = computePopulation(
                pPrediction,
                false,
                _traits->getStart(),
                _traits->getEnd(),
                intakeSeries,
                parameterSeries);

    TMP_UNUSED_PARAMETER(predictionComputationResult);

    if (computationResult == IPercentileCalculator::ComputingResult::Success &&
            intakeSeries.size() == pPrediction->getTimes().size() &&
            intakeSeries.size() == pPrediction->getValues().size())
    {
        std::unique_ptr<PercentilesResponse> resp = std::make_unique<PercentilesResponse>();

        const std::vector<std::vector<std::vector<Value> > > allValues = percentiles.getValues();

        for (unsigned int p = 0; p < percentiles.getRanks().size(); p++) {
            std::vector<CycleData> percData;

            for (unsigned int cycle = 0; cycle < allValues[p].size(); cycle ++) {

                TimeOffsets times = pPrediction->getTimes().at(cycle);
                DateTime start = intakeSeries.at(cycle).getEventTime();
                DateTime end = start + std::chrono::milliseconds(static_cast<int>(times.at(times.size() - 1)) * 1000);

                CycleData cycleData(start, end, Unit("ug/l"));
                cycleData.addData(times, allValues[p].at(cycle), 0);
                percData.push_back(cycleData);
            }
            resp->addPercentileData(percData);
        }
        resp->setRanks(percentileRanks);

        _response->addResponse(std::move(resp));
        return ComputingResult::Success;
    }
    else {
        return ComputingResult::Error;
    }
}

typedef struct {
    Value m_dose;
    Duration m_interval;
    Duration m_infusionTime;
} AdjustmentCandidate;


DosageTimeRange *createDosage(
        AdjustmentCandidate &_candidate,
        DateTime _startTime,
        DateTime _endTime,
        FormulationAndRoute _routeOfAdministration)
{
    unsigned int nbTimes;
    // TODO : Think about this: ceil or floor?
    nbTimes = (_endTime - _startTime) / _candidate.m_interval;
    LastingDose *lastingDose = new LastingDose(_candidate.m_dose, _routeOfAdministration, _candidate.m_infusionTime, _candidate.m_interval);
    DosageRepeat *repeat = new DosageRepeat(*lastingDose, nbTimes);
    DosageTimeRange *newTimeRange = new DosageTimeRange(_startTime, _endTime, *repeat);
    return newTimeRange;
}

bool compareCandidates(const FullDosage &a, const FullDosage &b)
{
    return a.getGlobalScore() < b.getGlobalScore();
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

        // We get the last formulation and retrieve the corresponding full spec from the available ones
        FormulationAndRoute formulation = _dosageHistory.getLastFormulationAndRoute();
        const FullFormulationAndRoute *selectedFormulationAndRoute = _availableFormulationAndRoutes.get(formulation);
        result.push_back(selectedFormulationAndRoute);
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

ComputingResult ComputingComponent::compute(
        const ComputingTraitAdjustment *_traits,
        const ComputingRequest &_request,
        std::unique_ptr<ComputingResponse> &_response)
{

    if (_traits == nullptr) {
        m_logger.error("The computing traits sent for computation are nullptr");
        return ComputingResult::Error;
    }

    std::shared_ptr<PkModel> pkModel;

    IntakeSeries intakeSeries;
    CovariateSeries covariateSeries;
    ParameterSetSeries parameterSeries;

    ComputingResult extractionResult = generalExtractions(_traits,
                                                          _request,
                                                          pkModel,
                                                          intakeSeries,
                                                          covariateSeries,
                                                          parameterSeries);

    if (extractionResult != ComputingResult::Success) {
        return extractionResult;
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
        return ComputingResult::Error;
    }


    int index = 0;

    // We iterate over the potential formulation and routes
    for (auto const *selectedFormulationAndRoute : selectedFormulationAndRoutes) {

        if (selectedFormulationAndRoute == nullptr) {
            m_logger.error("Can not find a formulation and route for adjustment");
            return ComputingResult::Error;
        }


        std::vector<Value> doseValues;
        std::vector<Duration> intervalValues;
        std::vector<Duration> infusionTimes;

        const ValidDoses * doses = selectedFormulationAndRoute->getValidDoses();
        if (doses) {
            doseValues = doses->getValues();
        }

        const ValidDurations * intervals = selectedFormulationAndRoute->getValidIntervals();
        if (intervals) {
            intervalValues = intervals->getDurations();
        }

        const ValidDurations * infusions = selectedFormulationAndRoute->getValidInfusionTimes();
        if (infusions) {
            infusionTimes = infusions->getDurations();
        }

        if (doseValues.size() == 0) {
            m_logger.error("No available potential dose");
            return ComputingResult::Error;
        }

        if (intervalValues.size() == 0) {
            m_logger.error("No available interval");
            return ComputingResult::Error;
        }

        if (infusionTimes.size() == 0) {
            if (selectedFormulationAndRoute->getFormulationAndRoute().getAbsorptionModel() == AbsorptionModel::INFUSION) {
                m_logger.error("Infusion selected, but no potential infusion time");
                return ComputingResult::Error;
            }
            infusionTimes.push_back(Duration(0h));
        }

        std::vector<AdjustmentCandidate> candidates;

        // Creation of all candidates
        for(auto dose : doseValues) {
            for(auto interval : intervalValues) {
                for(auto infusion : infusionTimes) {
                    candidates.push_back({dose, interval, infusion});
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

        TargetExtractor targetExtractor;
        TargetSeries targetSeries;

        TargetExtractionOption targetExtractionOption = TargetExtractionOption::PopulationValues;

        targetExtractor.extract(covariateSeries, _request.getDrugModel().getActiveMoieties().at(0).get()->getTargetDefinitions(),
                                _request.getDrugTreatment().getTargets(), _traits->getStart(), _traits->getEnd(),
                                targetExtractionOption, targetSeries);

        // A vector of vector because each adjustment candidate can have various targets
        std::vector< std::vector< TargetEvaluationResult> > evaluationResults;


        TargetEvaluator targetEvaluator;
        for (auto candidate : candidates) {
            DosageHistory *newHistory = _request.getDrugTreatment().getDosageHistory().clone();
            DateTime newEndTime = _traits->getEnd();
            DosageTimeRange *newDosage = createDosage(candidate, _traits->getAdjustmentTime(), newEndTime,
                                                      selectedFormulationAndRoute->getFormulationAndRoute());
            //        newHistory = new DosageHistory();
            //      newHistory->addTimeRange(*newDosage);
            newHistory->mergeDosage(newDosage);


            IntakeSeries intakeSeries;
            IntakeExtractor intakeExtractor;
            int nIntakes = intakeExtractor.extract(*newHistory, _traits->getStart(), _traits->getEnd(),
                                                   intakeSeries, _traits->getCycleSize());
            TMP_UNUSED_PARAMETER(nIntakes);

            IntakeToCalculatorAssociator::Result intakeExtractionResult =
                    IntakeToCalculatorAssociator::associate(intakeSeries, *pkModel.get());

            if (intakeExtractionResult != IntakeToCalculatorAssociator::Result::Ok) {
                m_logger.error("Can not associate intake calculators for the specified route");
                return ComputingResult::Error;
            }


            // We use a single prediction to get back time offsets. Could be optimized
            ConcentrationPredictionPtr pPrediction = std::make_unique<ConcentrationPrediction>();
            ComputationResult predictionComputationResult = computePopulation(
                        pPrediction,
                        false,
                        _traits->getStart(),
                        _traits->getEnd(),
                        intakeSeries,
                        parameterSeries);

            index ++;

            if (predictionComputationResult != ComputationResult::Success) {
                m_logger.error("Error with the computation of a single adjustment candidate");
                return ComputingResult::Error;
            }

            std::vector< TargetEvaluationResult> candidateResults;
            bool isValidCandidate = true;

            if (targetSeries.size() == 0) {
                isValidCandidate = false;
            }

            for(const auto & target : targetSeries) {
                TargetEvaluationResult localResult;

                // Now the score calculation
                TargetEvaluator::Result evaluationResult = targetEvaluator.evaluate(*pPrediction.get(), intakeSeries, target, localResult);

                // Here we do not compare to Result::Ok, because the result can also be
                // Result::InvalidCandidate
                if (evaluationResult == TargetEvaluator::Result::EvaluationError) {
                    m_logger.error("Error in the evaluation of targets");
                    return ComputingResult::Error;
                }

                // If the candidate is valid:
                if (evaluationResult == TargetEvaluator::Result::Ok) {
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

                FullDosage dosage;
                dosage.m_targetsEvaluation = candidateResults;
                dosage.m_history.addTimeRange(*newDosage);


                for (size_t i = 0; i < intakeSeries.size(); i++) {
                    TimeOffsets times = pPrediction->getTimes().at(i);
                    DateTime start = intakeSeries.at(i).getEventTime();
                    DateTime end = start + std::chrono::milliseconds(static_cast<int>(times.at(times.size() - 1)) * 1000);
                    if (start >= _traits->getAdjustmentTime()) {
                        CycleData cycle(start, end, Unit("ug/l"));
                        cycle.addData(times, pPrediction->getValues().at(i), 0);
                        dosage.m_data.push_back(cycle);
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



    // Sort in reverse order. The highest score will be the first element
    std::sort(dosageCandidates.rbegin(), dosageCandidates.rend(), compareCandidates);

#if 0
    std::cout << "Sorted..." << std::endl;
    // For debugging purpose only
    for (const auto & candidates : dosageCandidates)
    {
            std::cout << "Evaluation. Score : " << candidates.getGlobalScore()  << std::endl;
    }
#endif // 0

    // Now we have adjustments, predictions, and target evaluation results, let's build the response
    std::unique_ptr<AdjustmentResponse> resp = std::make_unique<AdjustmentResponse>();


    if (_traits->getBestCandidatesOption() == BestCandidatesOption::AllDosages) {
        resp->setAdjustments(dosageCandidates);
    }
    else {
        std::vector<FullDosage> bestDosage;
        if (dosageCandidates.size() != 0) {
            bestDosage.push_back(dosageCandidates.at(0));
        }
        resp->setAdjustments(bestDosage);
    }

    // Finally add the response to the set of responses
    _response->addResponse(std::move(resp));

    return ComputingResult::Success;
}


ComputingResult ComputingComponent::compute(
        const ComputingTraitAtMeasures *_traits,
        const ComputingRequest &_request,
        std::unique_ptr<ComputingResponse> &_response)
{
    TMP_UNUSED_PARAMETER(_traits);
    TMP_UNUSED_PARAMETER(_request);
    TMP_UNUSED_PARAMETER(_response);
    return ComputingResult::Error;
}

ComputingResult ComputingComponent::compute(
        const ComputingTraitSinglePoints *_traits,
        const ComputingRequest &_request,
        std::unique_ptr<ComputingResponse> &_response)
{
    TMP_UNUSED_PARAMETER(_traits);
    TMP_UNUSED_PARAMETER(_request);
    TMP_UNUSED_PARAMETER(_response);
    return ComputingResult::Error;
}


std::string ComputingComponent::getErrorString() const
{
    return "";
}

Tucuxi::Common::Interface* ComputingComponent::getInterface(const std::string &_name)
{
    return Tucuxi::Common::Component::getInterfaceImpl(_name);
}



ComputationResult ComputingComponent::computeConcentrations(
        ConcentrationPredictionPtr &_prediction,
        bool _isAll,
        const DateTime &_recordFrom,
        const DateTime &_recordTo,
        const IntakeSeries &_intakes,
        const ParameterSetSeries &_parameterSets,
        const Etas &_etas,
        const IResidualErrorModel &_residualErrorModel,
        const Deviations& _eps,
        const bool _isFixedDensity)
{
    // TODO : Use a factory for the calculator
    ConcentrationCalculator calculator;
    calculator.computeConcentrations(
                _prediction,
                _isAll,
                _recordFrom,
                _recordTo,
                _intakes,
                _parameterSets,
                _etas,
                _residualErrorModel,
                _eps,
                _isFixedDensity);
    return ComputationResult::Success;
}


} // namespace Core
} // namespace Tucuxi
