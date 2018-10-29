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


Duration ComputingComponent::secureStartDuration(const HalfLife &_halfLife)
{
    Duration duration;
    if (_halfLife.getUnit() == Unit("d")) {
        duration = Duration(24h) * _halfLife.getValue() * _halfLife.getMultiplier();
    }
    else if (_halfLife.getUnit() == Unit("h")) {
        duration = Duration(1h) * _halfLife.getValue() * _halfLife.getMultiplier();
    }
    else if (_halfLife.getUnit() == Unit("m")) {
        duration = Duration(1min) * _halfLife.getValue() * _halfLife.getMultiplier();
    }
    else if (_halfLife.getUnit() == Unit("s")) {
        duration = Duration(1s) * _halfLife.getValue() * _halfLife.getMultiplier();
    }
    return duration;
}

ComputingResult ComputingComponent::generalExtractions(
        const ComputingTraitStandard *_traits,
        const ComputingRequest &_request,
        std::shared_ptr<PkModel> &_pkModel,
        IntakeSeries &_intakeSeries,
        CovariateSeries &_covariatesSeries,
        ParameterSetSeries &_parameterSeries,
        DateTime &_calculationStartTime
        )
{


    const HalfLife &halfLife = _request.getDrugModel().getTimeConsiderations().getHalfLife();

    Duration fantomDuration = secureStartDuration(halfLife);

    Tucuxi::Common::DateTime fantomStart = _traits->getStart() - fantomDuration;

    _calculationStartTime = fantomStart;

    IntakeExtractor intakeExtractor;
    int nIntakes = intakeExtractor.extract(_request.getDrugTreatment().getDosageHistory(false), fantomStart /*_traits->getStart()*/, _traits->getEnd() + Duration(24h), _intakeSeries, _traits->getCycleSize());

    for (int i = 0; i < nIntakes; i++) {
        if (_intakeSeries.at(i).getEventTime() + _intakeSeries.at(i).getInterval() < _traits->getStart()) {
            _intakeSeries[i].setNbPoints(2);
        }
    }

    // TODO : Specific to busulfan here. Should be handled differently
    if (_request.getDrugModel().getAnalyteSet()->getAnalytes().at(0)->getAnalyteId() == "busulfan") {
        if (nIntakes > 0) {
            _intakeSeries.at(nIntakes - 1).setInterval(72h);
            _intakeSeries.at(nIntakes - 1).setNbPoints(250*10);
        }
    }

    const std::string pkModelId = _request.getDrugModel().getPkModelId();

    // TODO : This should not necessarily be the default formulation and route
    // Should get rid of the next 3 lines
    const std::string analyteId = _request.getDrugModel().getAnalyteSet()->getId();
    const Formulation formulation = _request.getDrugModel().getFormulationAndRoutes().getDefault()->getFormulationAndRoute().getFormulation();
    const AdministrationRoute route = _request.getDrugModel().getFormulationAndRoutes().getDefault()->getFormulationAndRoute().getAdministrationRoute();

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
                                          fantomStart,
                                          _traits->getEnd());
    CovariateExtractor::Result covariateExtractionResult = covariateExtractor.extract(_covariatesSeries);

    if (covariateExtractionResult != CovariateExtractor::Result::Ok) {
        m_logger.error("Can not extract covariates");
        return ComputingResult::Error;
    }

    ParameterDefinitionIterator it = _request.getDrugModel().getParameterDefinitions(analyteId, formulation, route);

    ParametersExtractor parameterExtractor(_covariatesSeries,
                                           it,
                                           fantomStart,
                                           _traits->getEnd());

    ParametersExtractor::Result parametersExtractionResult;

    if (_traits->getComputingOption().getParametersType() == PredictionParameterType::Population) {
        parametersExtractionResult = parameterExtractor.extractPopulation(_parameterSeries);
    }
    else {
        parametersExtractionResult = parameterExtractor.extract(_parameterSeries);
    }

    if (parametersExtractionResult != ParametersExtractor::Result::Ok) {
        m_logger.error("Can not extract parameters");
        return ComputingResult::Error;
    }

    return ComputingResult::Success;

}

ComputationResult ComputingComponent::extractOmega(
    const DrugModel &_drugModel,
    OmegaMatrix &_omega)
{

    // TODO : This should not necessarily be the default formulation and route
    // Should get rid of the next 3 lines
    const std::string analyteId = _drugModel.getAnalyteSet()->getId();
    const Formulation formulation = _drugModel.getFormulationAndRoutes().getDefault()->getFormulationAndRoute().getFormulation();
    const AdministrationRoute route = _drugModel.getFormulationAndRoutes().getDefault()->getFormulationAndRoute().getAdministrationRoute();

    ParameterDefinitionIterator it = _drugModel.getParameterDefinitions(analyteId, formulation, route);


    int nbVariableParameters = 0;

    it.reset();
    while (!it.isDone()) {
        if ((*it)->getVariability().getType() != ParameterVariabilityType::None) {
            nbVariableParameters ++;
        }
        it.next();
    }

    _omega = Tucuxi::Core::OmegaMatrix(nbVariableParameters, nbVariableParameters);

    for(int x = 0; x < nbVariableParameters; x++) {
        for(int y = 0; y < nbVariableParameters; y++) {
            _omega(x,y) = 0.0;
        }
    }

    std::map<std::string, int> paramMap;
    int nbParameter = 0;
    it.reset();
    while (!it.isDone()) {
        if ((*it)->getVariability().getType() != ParameterVariabilityType::None) {
            // const ParameterDefinition *p = (*it);
            // Value v = (*it)->getVariability().getValue() ;
            _omega(nbParameter, nbParameter) = (*it)->getVariability().getValue() * (*it)->getVariability().getValue();
            paramMap[(*it)->getId()] = nbParameter;
            nbParameter ++;
        }
        it.next();
    }

    const AnalyteSet *analyteSet = _drugModel.getAnalyteSet(analyteId);
    Correlations correlations = analyteSet->getDispositionParameters().getCorrelations();
    for(size_t i = 0; i < correlations.size(); i++) {
        std::string p1 = correlations[i].getParamId1();
        std::string p2 = correlations[i].getParamId2();
        Value correlation = correlations[i].getValue();

        int index1 = paramMap[p1];
        int index2 = paramMap[p2];

        Value covariance = correlation * (std::sqrt(_omega(index1, index1) * _omega(index2, index2)));
        _omega(index1, index2) = covariance;
        _omega(index2, index1) = covariance;
    }




    /*
    _omega(0,0) = 0.356 * 0.356; // Variance of CL
    _omega(0,1) = 0.798 * 0.356 * 0.629; // Covariance of CL and V
    _omega(1,1) = 0.629 * 0.629; // Variance of V
    _omega(1,0) = 0.798 * 0.356 * 0.629; // Covariance of CL and V
*/
    return ComputationResult::Success;
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
    DateTime calculationStartTime;

    ComputingResult extractionResult = generalExtractions(_traits,
                                                          _request,
                                                          pkModel,
                                                          intakeSeries,
                                                          covariateSeries,
                                                          parameterSeries,
                                                          calculationStartTime);

    if (extractionResult != ComputingResult::Success) {
        return extractionResult;
    }

    // Now ready to do the real computing with all the extracted values

    ConcentrationPredictionPtr pPrediction = std::make_unique<ConcentrationPrediction>();

    ComputationResult computationResult;

    if (_traits->getComputingOption().getParametersType() == PredictionParameterType::Aposteriori) {

        Etas etas;

        Tucuxi::Core::OmegaMatrix omega;
        ComputationResult omegaComputationResult = extractOmega(_request.getDrugModel(), omega);
        if (omegaComputationResult != ComputationResult::Success) {
            return ComputingResult::Error;
        }

        const Tucuxi::Core::IResidualErrorModel &residualErrorModel =_request.getDrugModel().getAnalyteSet()->getAnalytes().at(0)->getResidualErrorModel();


        SampleSeries sampleSeries;
        SampleExtractor sampleExtractor;
        sampleExtractor.extract(_request.getDrugTreatment().getSamples(), _traits->getStart(), _traits->getEnd(), sampleSeries);

        APosterioriEtasCalculator etasCalculator;
        etasCalculator.computeAposterioriEtas(intakeSeries, parameterSeries, omega, residualErrorModel, sampleSeries, etas);
        computationResult = computeAposteriori(
                    pPrediction,
                    false,
                     _traits->getStart(),
 //                   calculationStartTime,
                    _traits->getEnd(),
                    intakeSeries,
                    parameterSeries,
                    etas,
                    residualErrorModel);
    }
    else {
        computationResult = computePopulation(
                    pPrediction,
                    false,
                    _traits->getStart(),
//                    calculationStartTime,
                    _traits->getEnd(),
                    intakeSeries,
                    parameterSeries);
    }

    if (computationResult == ComputationResult::Success)
    {
        std::unique_ptr<SinglePredictionResponse> resp = std::make_unique<SinglePredictionResponse>();

        IntakeSeries recordedIntakes;
        selectRecordedIntakes(recordedIntakes, intakeSeries, _traits->getStart(), _traits->getEnd());

        if ((recordedIntakes.size() != pPrediction->getTimes().size()) ||
                (recordedIntakes.size() != pPrediction->getValues().size())) {
            return ComputingResult::Error;
        }

        // std::cout << "Start Time : " << _traits->getStart() << std::endl;
        for (size_t i = 0; i < recordedIntakes.size(); i++) {

            TimeOffsets times = pPrediction->getTimes().at(i);
            DateTime start = recordedIntakes.at(i).getEventTime();
            // std::cout << "Time index " << i << " : " << start << std::endl;
            // times values are in hours
            std::chrono::milliseconds ms = std::chrono::milliseconds(static_cast<int>(times.at(times.size() - 1))) * 3600 * 1000;
            Duration ds(ms);
            DateTime end = start + ds;
            // std::cout << "End Time index " << i << " : " << end << std::endl;

            if (end > _traits->getStart()) {
                // std::cout << "Selected Time index " << i << " : " << start << std::endl;
                CycleData cycle(start, end, Unit("ug/l"));
                cycle.addData(times, pPrediction->getValues().at(i), 0);
                resp->addCycleData(cycle);
            }
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
#ifdef NO_PERCENTILES
    return ComputingResult::Error;
#endif
    if (_traits == nullptr) {
        m_logger.error("The computing traits sent for computation are nullptr");
        return ComputingResult::Error;
    }

    std::shared_ptr<PkModel> pkModel;

    IntakeSeries intakeSeries;
    CovariateSeries covariateSeries;
    ParameterSetSeries parameterSeries;
    DateTime calculationStartTime;

    ComputingResult extractionResult = generalExtractions(_traits,
                                                          _request,
                                                          pkModel,
                                                          intakeSeries,
                                                          covariateSeries,
                                                          parameterSeries,
                                                          calculationStartTime);

    if (extractionResult != ComputingResult::Success) {
        return extractionResult;
    }

    // Now ready to do the real computing with all the extracted values





    Tucuxi::Core::PercentilesPrediction percentiles;

    Tucuxi::Core::PercentileRanks percentileRanks;
    Tucuxi::Core::OmegaMatrix omega;
    Tucuxi::Core::Etas etas;
    Tucuxi::Core::ComputingAborter *aborter = _traits->getAborter();

    percentileRanks = _traits->getRanks();

    const Tucuxi::Core::IResidualErrorModel &residualErrorModel =_request.getDrugModel().getAnalyteSet()->getAnalytes().at(0)->getResidualErrorModel();


    ComputationResult omegaComputationResult = extractOmega(_request.getDrugModel(), omega);
    if (omegaComputationResult != ComputationResult::Success) {
        return ComputingResult::Error;
    }


    // Set initial etas to 0 for all variable parameters
    int etaSize = omega.cols();
    for (int i = 0; i < etaSize; i++) {
        etas.push_back(0.0);
    }

    Tucuxi::Core::IPercentileCalculator::ComputingResult computationResult;

    Tucuxi::Core::ConcentrationCalculator concentrationCalculator;

    if (_traits->getComputingOption().getParametersType() == PredictionParameterType::Aposteriori)   {

        SampleSeries sampleSeries;
        SampleExtractor sampleExtractor;
        sampleExtractor.extract(_request.getDrugTreatment().getSamples(), _traits->getStart(), _traits->getEnd(), sampleSeries);

        APosterioriEtasCalculator etasCalculator;
        etasCalculator.computeAposterioriEtas(intakeSeries, parameterSeries, omega, residualErrorModel, sampleSeries, etas);

        std::unique_ptr<Tucuxi::Core::IAposterioriPercentileCalculator> calculator(new Tucuxi::Core::AposterioriMonteCarloPercentileCalculator());
        computationResult = calculator->calculate(
                    percentiles,
                    _traits->getStart(),
                    _traits->getEnd(),
                    intakeSeries,
                    parameterSeries,
                    omega,
                    residualErrorModel,
                    etas,
                    sampleSeries,
                    percentileRanks,
                    concentrationCalculator,
                    aborter);

    }
    else {
        std::unique_ptr<Tucuxi::Core::IAprioriPercentileCalculator> calculator(new Tucuxi::Core::AprioriMonteCarloPercentileCalculator());
        computationResult = calculator->calculate(
                    percentiles,
                    _traits->getStart(),
                    _traits->getEnd(),
                    intakeSeries,
                    parameterSeries,
                    omega,
                    residualErrorModel,
                    etas,
                    percentileRanks,
                    concentrationCalculator,
                    aborter);
    }

    // We use a single prediction to get back time offsets. Could be optimized
    // YTA: I do not think this is relevant. It could be deleted I guess
    ConcentrationPredictionPtr pPrediction = std::make_unique<ConcentrationPrediction>();
    ComputationResult predictionComputationResult = computePopulation(
                pPrediction,
                false,
                _traits->getStart(),
                _traits->getEnd(),
                intakeSeries,
                parameterSeries);

    if (predictionComputationResult != ComputationResult::Success) {
        return ComputingResult::Error;
    }

    IntakeSeries selectedIntakes;
    selectRecordedIntakes(selectedIntakes, intakeSeries, _traits->getStart(), _traits->getEnd());

    if (computationResult == IPercentileCalculator::ComputingResult::Success &&
            selectedIntakes.size() == pPrediction->getTimes().size() &&
            selectedIntakes.size() == pPrediction->getValues().size())
    {
        std::unique_ptr<PercentilesResponse> resp = std::make_unique<PercentilesResponse>();

        const std::vector<std::vector<std::vector<Value> > > allValues = percentiles.getValues();



        for (unsigned int p = 0; p < percentiles.getRanks().size(); p++) {
            std::vector<CycleData> percData;

            if (selectedIntakes.size() != allValues[p].size()) {
                return ComputingResult::Error;
            }
            if (selectedIntakes.size() != pPrediction->getTimes().size()) {
                return ComputingResult::Error;
            }

            for (unsigned int cycle = 0; cycle < allValues[p].size(); cycle ++) {

                TimeOffsets times = pPrediction->getTimes().at(cycle);
                DateTime start = selectedIntakes.at(cycle).getEventTime();

                std::chrono::milliseconds ms = std::chrono::milliseconds(static_cast<int>(times.at(times.size() - 1))) * 3600 * 1000;
                Duration ds(ms);
                DateTime end = start + ds;

                if (end > _traits->getStart()) {
                    CycleData cycleData(start, end, Unit("ug/l"));
                    cycleData.addData(times, allValues[p].at(cycle), 0);
                    percData.push_back(cycleData);
                }
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

    // At least a number of intervals allowing to fill the interval asked
    nbTimes = static_cast<unsigned int>(std::ceil((_endTime - _startTime) / _candidate.m_interval));
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
    DateTime calculationStartTime;

    // Be carefull here, as the endTime could be different...
    ComputingResult extractionResult = generalExtractions(_traits,
                                                          _request,
                                                          pkModel,
                                                          intakeSeries,
                                                          covariateSeries,
                                                          parameterSeries,
                                                          calculationStartTime);

    if (extractionResult != ComputingResult::Success) {
        return extractionResult;
    }


    Etas etas;

    if (_traits->getComputingOption().getParametersType() == PredictionParameterType::Aposteriori) {

        Tucuxi::Core::OmegaMatrix omega;
        ComputationResult omegaComputationResult = extractOmega(_request.getDrugModel(), omega);
        if (omegaComputationResult != ComputationResult::Success) {
            return ComputingResult::Error;
        }

        const Tucuxi::Core::IResidualErrorModel &residualErrorModel =_request.getDrugModel().getAnalyteSet()->getAnalytes().at(0)->getResidualErrorModel();


        SampleSeries sampleSeries;
        SampleExtractor sampleExtractor;
        sampleExtractor.extract(_request.getDrugTreatment().getSamples(), _traits->getStart(), _traits->getEnd(), sampleSeries);

        APosterioriEtasCalculator etasCalculator;
        etasCalculator.computeAposterioriEtas(intakeSeries, parameterSeries, omega, residualErrorModel, sampleSeries, etas);
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
            if (selectedFormulationAndRoute->getFormulationAndRoute().getRouteModel() == RouteModel::INFUSION) {
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

        TargetExtractionOption targetExtractionOption = _traits->getTargetExtractionOption();

        targetExtractor.extract(covariateSeries, _request.getDrugModel().getActiveMoieties().at(0).get()->getTargetDefinitions(),
                                _request.getDrugTreatment().getTargets(), _traits->getStart(), _traits->getEnd(),
                                targetExtractionOption, targetSeries);

        // A vector of vector because each adjustment candidate can have various targets
        std::vector< std::vector< TargetEvaluationResult> > evaluationResults;


        TargetEvaluator targetEvaluator;
        for (auto candidate : candidates) {
            DosageHistory *newHistory = _request.getDrugTreatment().getDosageHistory().clone();
            DateTime newEndTime;

            // If in steady state mode, then calculate the real end time
            if (_traits->getSteadyStateTargetOption() == SteadyStateTargetOption::AtSteadyState) {

                const HalfLife &halfLife = _request.getDrugModel().getTimeConsiderations().getHalfLife();

                Duration newDuration;
                if (halfLife.getUnit() == Unit("d")) {
                    newDuration = Duration(24h) * halfLife.getValue() * halfLife.getMultiplier();
                }
                else if (halfLife.getUnit() == Unit("h")) {
                    newDuration = Duration(1h) * halfLife.getValue() * halfLife.getMultiplier();
                }
                else if (halfLife.getUnit() == Unit("m")) {
                    newDuration = Duration(1min) * halfLife.getValue() * halfLife.getMultiplier();
                }
                else if (halfLife.getUnit() == Unit("s")) {
                    newDuration = Duration(1s) * halfLife.getValue() * halfLife.getMultiplier();
                }

                // Rounding the new duration to be a multiple of the new interval
                int nbIntervals = static_cast<int>(newDuration / candidate.m_interval);
                Duration roundedNewDuration = candidate.m_interval * nbIntervals;
                newEndTime = _traits->getAdjustmentTime() + roundedNewDuration;
            }
            else {
                newEndTime = _traits->getEnd();
            }

            DosageTimeRange *newDosage = createDosage(candidate, _traits->getAdjustmentTime(), newEndTime,
                                                      selectedFormulationAndRoute->getFormulationAndRoute());
            newHistory->mergeDosage(newDosage);


            IntakeSeries intakeSeries;
            IntakeExtractor intakeExtractor;
            int nIntakes = intakeExtractor.extract(*newHistory, calculationStartTime, newEndTime,
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

            ComputationResult predictionComputationResult;

            if (_traits->getComputingOption().getParametersType() == PredictionParameterType::Aposteriori) {

                const Tucuxi::Core::IResidualErrorModel &residualErrorModel =_request.getDrugModel().getAnalyteSet()->getAnalytes().at(0)->getResidualErrorModel();

                predictionComputationResult = computeAposteriori(
                            pPrediction,
                            false,
         //                    _traits->getStart(),
                            calculationStartTime,
                            newEndTime,
                            intakeSeries,
                            parameterSeries,
                            etas,
                            residualErrorModel);
            }
            else {
                predictionComputationResult = computePopulation(
                            pPrediction,
                            false,
        //                    _traits->getStart(),
                            calculationStartTime,
                            newEndTime,
                            intakeSeries,
                            parameterSeries);
            }

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
                index ++;

                FullDosage dosage;
                dosage.m_targetsEvaluation = candidateResults;
//                dosage.m_history = *newHistory->clone();
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
    // There is an issue with DosageHistory as the copy don't work correctly
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
    return calculator.computeConcentrations(
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
}


} // namespace Core
} // namespace Tucuxi
