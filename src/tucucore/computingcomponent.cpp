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
#include "tucucore/overloadevaluator.h"
#include "tucucore/residualerrormodelextractor.h"

namespace Tucuxi {
namespace Core {

Tucuxi::Common::Interface* ComputingComponent::createComponent()
{
    ComputingComponent *cmp = new ComputingComponent();

    cmp->initialize();

    return dynamic_cast<IComputingService*>(cmp);
}


ComputingComponent::ComputingComponent() : m_models(nullptr)
{
    registerInterface(dynamic_cast<IComputingService*>(this));
}


ComputingComponent::~ComputingComponent()
{
    if (m_models != nullptr) {
        delete m_models;
    }
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
    try {
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

            if (it->get()->compute(*this, _request, _response) != ComputingResult::Success) {
                result = ComputingResult::Error;
            }
            it++;
        }

        return result;

    } catch (...) {
        return ComputingResult::Error;
    }
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

    Tucuxi::Common::DateTime firstEvent = _traits->getStart();

    if ((_traits->getComputingOption().getParametersType() == PredictionParameterType::Aposteriori)
            && (_request.getDrugTreatment().getSamples().size() > 0)) {
        for (const auto &sample : _request.getDrugTreatment().getSamples()) {
            if (sample->getDate() < firstEvent) {
                firstEvent = sample->getDate();
            }
        }
    }

    Tucuxi::Common::DateTime fantomStart = firstEvent - fantomDuration;

    _calculationStartTime = fantomStart;

    IntakeExtractor intakeExtractor;
    double nbPointsPerHour = _traits->getNbPointsPerHour();
    IntakeExtractor::Result intakeExtractionResult = intakeExtractor.extract(_request.getDrugTreatment().getDosageHistory(), fantomStart /*_traits->getStart()*/, _traits->getEnd() /* + Duration(24h)*/, nbPointsPerHour, _intakeSeries);

    if (intakeExtractionResult != IntakeExtractor::Result::Ok) {
        m_logger.error("Error with the intakes extraction.");
        return ComputingResult::Error;
    }

    int nIntakes = _intakeSeries.size();

    for (int i = 0; i < nIntakes; i++) {
        if (_intakeSeries.at(i).getEventTime() + _intakeSeries.at(i).getInterval() < _traits->getStart()) {
            _intakeSeries[i].setNbPoints(2);
        }
    }

    if (nIntakes > 0) {

        // Ensure that time ranges are correctly handled. We set again the interval based on the start of
        // next intake
        for (int i = 0; i < _intakeSeries.size() - 1; i++) {
            Duration interval = _intakeSeries.at(i+1).getEventTime() - _intakeSeries.at(i).getEventTime();
            _intakeSeries.at(i).setNbPoints(interval.toHours() * nbPointsPerHour);
            _intakeSeries.at(i).setInterval(interval);
        }

        const DosageTimeRangeList& timeRanges = _request.getDrugTreatment().getDosageHistory().getDosageTimeRanges();


        IntakeEvent *lastIntake = &(_intakeSeries.at(nIntakes - 1));

        // If the treatement end is before the last point we want to get, then we add an empty dose to get points

//        if (_traits->getEnd() > timeRanges.at(timeRanges.size() - 1)->getEndDate()) {
            if (_traits->getEnd() > lastIntake->getEventTime() + lastIntake->getInterval()) {

            DateTime start = lastIntake->getEventTime() + lastIntake->getInterval();
            Value dose = 0.0;
            Duration interval = _traits->getEnd() - timeRanges.at(timeRanges.size() - 1)->getEndDate();
            auto absorptionModel = lastIntake->getRoute();

            Duration infusionTime;
            if (absorptionModel == AbsorptionModel::INFUSION) {
                // We do this because the infusion calculators do not support infusionTime = 0
                infusionTime = Duration(std::chrono::hours(1));
            }
            int nbPoints = static_cast<int>(nbPointsPerHour * interval.toHours());

            IntakeEvent intake(start, Duration(), dose, interval, absorptionModel, infusionTime, nbPoints);
            _intakeSeries.push_back(intake);
        }

    }

    OverloadEvaluator overloadEvaluator;
    if (!overloadEvaluator.isAcceptable(_intakeSeries, _traits)) {
        return ComputingResult::TooBig;
    }

/*
    // TODO : Specific to busulfan here. Should be handled differently
    if (_request.getDrugModel().getAnalyteSet()->getAnalytes().at(0)->getAnalyteId() == "busulfan") {
        if (nIntakes > 0) {
            IntakeEvent *lastIntake = &(_intakeSeries.at(nIntakes - 1));
            DateTime start = lastIntake->getEventTime() + lastIntake->getInterval();
            Value dose = 0.0;
            Duration interval = Duration(std::chrono::hours(72));
            auto absorptionModel = lastIntake->getRoute();
            Duration infusionTime = Duration(std::chrono::hours(1));
            int nbPoints = nbPointsPerHour * 72;

            IntakeEvent intake(start, Duration(), dose, interval, absorptionModel, infusionTime, nbPoints);
            _intakeSeries.push_back(intake);


            //_intakeSeries.at(nIntakes - 1).setInterval(72h);
            //_intakeSeries.at(nIntakes - 1).setNbPoints(250*10);
        }
    }
*/

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


    IntakeToCalculatorAssociator::Result intakeAssociationResult = IntakeToCalculatorAssociator::associate(_intakeSeries, *_pkModel.get());

    if (intakeAssociationResult != IntakeToCalculatorAssociator::Result::Ok) {
        m_logger.error("Can not associate intake calculators for the specified route");
        return ComputingResult::Error;
    }

    if (_traits->getComputingOption().getParametersType() == PredictionParameterType::Population) {
        PatientVariates emptyPatientVariates;
        CovariateExtractor covariateExtractor(_request.getDrugModel().getCovariates(),
                                              emptyPatientVariates,
                                              fantomStart,
                                              _traits->getEnd());
        CovariateExtractor::Result covariateExtractionResult = covariateExtractor.extract(_covariatesSeries);

        if (covariateExtractionResult != CovariateExtractor::Result::Ok) {
            m_logger.error("Can not extract covariates");
            return ComputingResult::Error;
        }
    }
    else {
        CovariateExtractor covariateExtractor(_request.getDrugModel().getCovariates(),
                                              _request.getDrugTreatment().getCovariates(),
                                              fantomStart,
                                              _traits->getEnd());
        CovariateExtractor::Result covariateExtractionResult = covariateExtractor.extract(_covariatesSeries);

        if (covariateExtractionResult != CovariateExtractor::Result::Ok) {
            m_logger.error("Can not extract covariates");
            return ComputingResult::Error;
        }
    }

    ParameterDefinitionIterator it = _request.getDrugModel().getParameterDefinitions(analyteId, formulation, route);

    ParametersExtractor parameterExtractor(_covariatesSeries,
                                           it,
                                           fantomStart,
                                           _traits->getEnd());

    ParametersExtractor::Result parametersExtractionResult;


    if (_traits->getComputingOption().getParametersType() == PredictionParameterType::Population) {
#ifdef POPPARAMETERSFROMDEFAULTVALUES
        parametersExtractionResult = parameterExtractor.extractPopulation(_parameterSeries);
#else
        //parametersExtractionResult = parameterExtractor.extract(_parameterSeries);

        ParameterSetSeries intermediateParameterSeries;

        parametersExtractionResult = parameterExtractor.extract(intermediateParameterSeries);

        if (parametersExtractionResult != ParametersExtractor::Result::Ok) {
            m_logger.error("Can not extract parameters");
            return ComputingResult::Error;
        }

        // The intermediateParameterSeries contains changes of parameters, so we build a full set of parameter
        // for each event.
        parametersExtractionResult = parameterExtractor.buildFullSet(intermediateParameterSeries, _parameterSeries);
        if (parametersExtractionResult != ParametersExtractor::Result::Ok) {
            m_logger.error("Can not consolidate parameters");
            return ComputingResult::Error;
        }

#endif // POPPARAMETERSFROMDEFAULTVALUES

        if (parametersExtractionResult != ParametersExtractor::Result::Ok) {
            m_logger.error("Can not extract parameters");
            return ComputingResult::Error;
        }

    }
    else {
        ParameterSetSeries intermediateParameterSeries;

        parametersExtractionResult = parameterExtractor.extract(intermediateParameterSeries);

        if (parametersExtractionResult != ParametersExtractor::Result::Ok) {
            m_logger.error("Can not extract parameters");
            return ComputingResult::Error;
        }

        // The intermediateParameterSeries contains changes of parameters, so we build a full set of parameter
        // for each event.
        parametersExtractionResult = parameterExtractor.buildFullSet(intermediateParameterSeries, _parameterSeries);
        if (parametersExtractionResult != ParametersExtractor::Result::Ok) {
            m_logger.error("Can not consolidate parameters");
            return ComputingResult::Error;
        }

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

    Etas etas;

    if (_traits->getComputingOption().getParametersType() == PredictionParameterType::Aposteriori) {


        Tucuxi::Core::OmegaMatrix omega;
        ComputationResult omegaComputationResult = extractOmega(_request.getDrugModel(), omega);
        if (omegaComputationResult != ComputationResult::Success) {
            return ComputingResult::Error;
        }

        SampleSeries sampleSeries;
        SampleExtractor sampleExtractor;
        sampleExtractor.extract(_request.getDrugTreatment().getSamples(), calculationStartTime, _traits->getEnd(), sampleSeries);

        if (sampleSeries.size() == 0) {
            // Surprising. Something maybe wrong with the sample extractor

        }
        else {
            ResidualErrorModelExtractor errorModelExtractor;
            IResidualErrorModel *residualErrorModel = nullptr;
            if (errorModelExtractor.extract(_request.getDrugModel().getAnalyteSet()->getAnalytes().at(0)->getResidualErrorModel(),
                                            _request.getDrugModel().getAnalyteSet()->getAnalytes().at(0)->getUnit(),
                                            covariateSeries, &residualErrorModel)
                != ResidualErrorModelExtractor::Result::Ok) {
                return ComputingResult::Error;
            }

            APosterioriEtasCalculator etasCalculator;
            etasCalculator.computeAposterioriEtas(intakeSeries, parameterSeries, omega, *residualErrorModel, sampleSeries, etas);

            delete residualErrorModel;
        }
    }

    computationResult = computeConcentrations(
                pPrediction,
                false,
                 _traits->getStart(),
//                   calculationStartTime,
                _traits->getEnd(),
                intakeSeries,
                parameterSeries,
                etas);


    if (computationResult == ComputationResult::Success)
    {
        std::unique_ptr<SinglePredictionResponse> resp = std::make_unique<SinglePredictionResponse>(_traits->getId());

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


                ParameterSetEventPtr params = parameterSeries.getAtTime(start, etas);

                for (auto p = params.get()->begin() ; p < params.get()->end() ; p++) {
                    cycle.m_parameters.push_back({(*p).getParameterId(), (*p).getValue()});
                }

                std::sort(cycle.m_parameters.begin(), cycle.m_parameters.end(),
                           [&] (const ParameterValue &v1, const ParameterValue &v2) { return v1.m_parameterId < v2.m_parameterId; });

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


    ResidualErrorModelExtractor errorModelExtractor;
    std::unique_ptr<IResidualErrorModel> residualErrorModel = nullptr;
    IResidualErrorModel *residual = nullptr;
    if (errorModelExtractor.extract(_request.getDrugModel().getAnalyteSet()->getAnalytes().at(0)->getResidualErrorModel(),
                                    _request.getDrugModel().getAnalyteSet()->getAnalytes().at(0)->getUnit(),
                                    covariateSeries, &residual)
        != ResidualErrorModelExtractor::Result::Ok) {
        if (residual != nullptr) {
            delete residual;
        }
        return ComputingResult::Error;
    }

    residualErrorModel = std::unique_ptr<IResidualErrorModel>(residual);

//    const Tucuxi::Core::IResidualErrorModel &residualErrorModel =_request.getDrugModel().getAnalyteSet()->getAnalytes().at(0)->getResidualErrorModel();


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
        sampleExtractor.extract(_request.getDrugTreatment().getSamples(), calculationStartTime, _traits->getEnd(), sampleSeries);

        APosterioriEtasCalculator etasCalculator;
        etasCalculator.computeAposterioriEtas(intakeSeries, parameterSeries, omega, *residualErrorModel, sampleSeries, etas);

        std::unique_ptr<Tucuxi::Core::IAposterioriPercentileCalculator> calculator(new Tucuxi::Core::AposterioriMonteCarloPercentileCalculator());
        computationResult = calculator->calculate(
                    percentiles,
                    _traits->getStart(),
                    _traits->getEnd(),
                    intakeSeries,
                    parameterSeries,
                    omega,
                    *residualErrorModel,
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
                    *residualErrorModel,
                    etas,
                    percentileRanks,
                    concentrationCalculator,
                    aborter);
    }

    // We use a single prediction to get back time offsets. Could be optimized
    // YTA: I do not think this is relevant. It could be deleted I guess
    ConcentrationPredictionPtr pPrediction = std::make_unique<ConcentrationPrediction>();

    ComputationResult predictionComputationResult = computeConcentrations(
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
        std::unique_ptr<PercentilesResponse> resp = std::make_unique<PercentilesResponse>(_traits->getId());

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



DosageTimeRange *ComputingComponent::createDosage(
        ComputingComponent::AdjustmentCandidate &_candidate,
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

ComputingResult ComputingComponent::buildCandidates(const FullFormulationAndRoute* _formulationAndRoute, std::vector<ComputingComponent::AdjustmentCandidate> &_candidates)
{
    std::vector<Value> doseValues;
    std::vector<Duration> intervalValues;
    std::vector<Duration> infusionTimes;

    const ValidDoses * doses = _formulationAndRoute->getValidDoses();
    if (doses) {
        doseValues = doses->getValues();
    }

    const ValidDurations * intervals = _formulationAndRoute->getValidIntervals();
    if (intervals) {
        intervalValues = intervals->getDurations();
    }

    const ValidDurations * infusions = _formulationAndRoute->getValidInfusionTimes();
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
        if (_formulationAndRoute->getFormulationAndRoute().getAbsorptionModel() == AbsorptionModel::INFUSION) {
            m_logger.error("Infusion selected, but no potential infusion time");
            return ComputingResult::Error;
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
    return ComputingResult::Success;
}

bool compareCandidates(const FullDosage &a, const FullDosage &b)
{
    return a.getGlobalScore() < b.getGlobalScore();
}


std::vector<FullDosage> ComputingComponent::sortAndFilterCandidates(std::vector<FullDosage> &_candidates, BestCandidatesOption _option)
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
    } break;
    case BestCandidatesOption::BestDosage : {
        std::vector<FullDosage> bestDosage;
        if (_candidates.size() != 0) {
            bestDosage.push_back(_candidates.at(0));
        }
        return bestDosage;
    } break;
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
                                    _candidates.erase(_candidates.begin() + j);
                                    j = j - 1;
                                }
                            }
                        }

                    }
                }
            }
        }
        return _candidates;
    } break;
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

//        const Tucuxi::Core::IResidualErrorModel &residualErrorModel =_request.getDrugModel().getAnalyteSet()->getAnalytes().at(0)->getResidualErrorModel();


        ResidualErrorModelExtractor errorModelExtractor;
        IResidualErrorModel *residualErrorModel = nullptr;
        if (errorModelExtractor.extract(_request.getDrugModel().getAnalyteSet()->getAnalytes().at(0)->getResidualErrorModel(),
                                        _request.getDrugModel().getAnalyteSet()->getAnalytes().at(0)->getUnit(),
                                        covariateSeries, &residualErrorModel)
            != ResidualErrorModelExtractor::Result::Ok) {
            return ComputingResult::Error;
        }


        SampleSeries sampleSeries;
        SampleExtractor sampleExtractor;
        sampleExtractor.extract(_request.getDrugTreatment().getSamples(), calculationStartTime, _traits->getEnd(), sampleSeries);

        if (sampleSeries.size() == 0) {
            // Something strange here
        }
        else {
            APosterioriEtasCalculator etasCalculator;
            etasCalculator.computeAposterioriEtas(intakeSeries, parameterSeries, omega, *residualErrorModel, sampleSeries, etas);
        }

        delete residualErrorModel;
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

        std::vector<AdjustmentCandidate> candidates;

        ComputingResult buildingResult = buildCandidates(selectedFormulationAndRoute, candidates);

        if (buildingResult != ComputingResult::Success) {
            m_logger.error("Can not build adjustment candidates");
            return buildingResult;
        }

        TargetExtractor targetExtractor;
        TargetSeries targetSeries;
        TargetExtractionOption targetExtractionOption = _traits->getTargetExtractionOption();

        TargetExtractor::Result targetExtractionResult =
        targetExtractor.extract(covariateSeries, _request.getDrugModel().getActiveMoieties().at(0).get()->getTargetDefinitions(),
                                _request.getDrugTreatment().getTargets(), _traits->getStart(), _traits->getEnd(),
                                targetExtractionOption, targetSeries);

        if (targetExtractionResult != TargetExtractor::Result::Ok) {
            return ComputingResult::Error;
        }

        // A vector of vector because each adjustment candidate can have various targets
        std::vector< std::vector< TargetEvaluationResult> > evaluationResults;


        TargetEvaluator targetEvaluator;
        for (auto candidate : candidates) {
            DosageHistory *newHistory = _request.getDrugTreatment().getDosageHistory().clone();
            DateTime newEndTime;

            // If in steady state mode, then calculate the real end time
            if (_traits->getSteadyStateTargetOption() == SteadyStateTargetOption::AtSteadyState) {

                const HalfLife &halfLife = _request.getDrugModel().getTimeConsiderations().getHalfLife();

                // Use the same function as for the start date of calculation when in steady state mode
                Duration newDuration = secureStartDuration(halfLife);

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
            double nbPointsPerHour = _traits->getNbPointsPerHour();
            IntakeExtractor::Result intakeExtractionResult = intakeExtractor.extract(*newHistory, calculationStartTime, newEndTime,
                                                   nbPointsPerHour, intakeSeries);

            delete newHistory;
            newHistory = nullptr;

            if (intakeExtractionResult != IntakeExtractor::Result::Ok) {
                return ComputingResult::Error;
            }

            IntakeToCalculatorAssociator::Result intakeAssociationResult =
                    IntakeToCalculatorAssociator::associate(intakeSeries, *pkModel.get());

            if (intakeAssociationResult != IntakeToCalculatorAssociator::Result::Ok) {
                m_logger.error("Can not associate intake calculators for the specified route");
                return ComputingResult::Error;
            }


            ConcentrationPredictionPtr pPrediction = std::make_unique<ConcentrationPrediction>();

            ComputationResult predictionComputationResult;

            predictionComputationResult = computeConcentrations(
                        pPrediction,
                        false,
                        //                            _traits->getStart(),
                        calculationStartTime,
                        newEndTime,
                        intakeSeries,
                        parameterSeries,
                        etas);

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

            delete newDosage;

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

    return ComputingResult::Success;
}


ComputingResult ComputingComponent::compute(
        const ComputingTraitAtMeasures *_traits,
        const ComputingRequest &_request,
        std::unique_ptr<ComputingResponse> &_response)
{

    // Simply extract the sample dates
    std::vector<Tucuxi::Common::DateTime> sampleTimes;
    for (const auto &sample : _request.getDrugTreatment().getSamples())
    {
        sampleTimes.push_back(sample->getDate());
    }

    // Create the corresponding object for single points calculation
    ComputingTraitSinglePoints traits(_traits->getId(), sampleTimes, _traits->getComputingOption());

    // And start the calculation
    return compute(&traits, _request, _response);
}

ComputingResult ComputingComponent::compute(
        const ComputingTraitSinglePoints *_traits,
        const ComputingRequest &_request,
        std::unique_ptr<ComputingResponse> &_response)
{

    if (_traits == nullptr) {
        m_logger.error("The computing traits sent for computation are nullptr");
        return ComputingResult::Error;
    }

    if (_traits->getTimes().size() == 0) {
        // No time given, so we return an empty response
        std::unique_ptr<SinglePointsResponse> resp = std::make_unique<SinglePointsResponse>(_traits->getId());
        _response->addResponse(std::move(resp));
        return ComputingResult::Success;
    }

    std::shared_ptr<PkModel> pkModel;

    IntakeSeries intakeSeries;
    CovariateSeries covariateSeries;
    ParameterSetSeries parameterSeries;
    DateTime calculationStartTime;

    DateTime firstDate;
    DateTime lastDate;

    for (const auto &singleTime : _traits->getTimes()) {
        if (firstDate.isUndefined() || (singleTime < firstDate)) {
            firstDate = singleTime;
        }
        if (lastDate.isUndefined() || (singleTime > lastDate)) {
            lastDate = singleTime;
        }
    }

    lastDate = lastDate + Duration(std::chrono::hours(1));

    // TODO : Check if 2 is necessary or not
    ComputingTraitStandard standardTraits(_traits->getId(), firstDate, lastDate, 2, _traits->getComputingOption());

    ComputingResult extractionResult = generalExtractions(&standardTraits,
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

    // TODO : Check what happens if etas is 0
    Etas etas;


    if (_traits->getComputingOption().getParametersType() == PredictionParameterType::Aposteriori) {


        Tucuxi::Core::OmegaMatrix omega;
        ComputationResult omegaComputationResult = extractOmega(_request.getDrugModel(), omega);
        if (omegaComputationResult != ComputationResult::Success) {
            return ComputingResult::Error;
        }

        SampleSeries sampleSeries;
        SampleExtractor sampleExtractor;
        sampleExtractor.extract(_request.getDrugTreatment().getSamples(), calculationStartTime, lastDate, sampleSeries);

        if (sampleSeries.size() == 0) {
            // Surprising. Something maybe wrong with the sample extractor

        }
        else {
            ResidualErrorModelExtractor errorModelExtractor;
            IResidualErrorModel *residualErrorModel = nullptr;
            if (errorModelExtractor.extract(_request.getDrugModel().getAnalyteSet()->getAnalytes().at(0)->getResidualErrorModel(),
                                            _request.getDrugModel().getAnalyteSet()->getAnalytes().at(0)->getUnit(),
                                            covariateSeries, &residualErrorModel)
                != ResidualErrorModelExtractor::Result::Ok) {
                return ComputingResult::Error;
            }

            APosterioriEtasCalculator etasCalculator;
            etasCalculator.computeAposterioriEtas(intakeSeries, parameterSeries, omega, *residualErrorModel, sampleSeries, etas);

            delete residualErrorModel;
        }
    }

    Concentrations concentrations;

    SampleSeries timesSeries;

    for (const auto &times : _traits->getTimes()) {
        timesSeries.push_back(SampleEvent(times));
    }

    ConcentrationCalculator calculator;
    computationResult = calculator.computeConcentrationsAtTimes(
                concentrations,
                false,
                intakeSeries,
                parameterSeries,
                timesSeries,
                etas,
                true);


    if (computationResult == ComputationResult::Success)
    {
        std::unique_ptr<SinglePointsResponse> resp = std::make_unique<SinglePointsResponse>(_traits->getId());

        if (concentrations.size() != timesSeries.size()) {
            // Something went wrong
            m_logger.error("The number of concentrations calculated is not the same as the number of points asked");
            return ComputingResult::Error;
        }

        // TODO : Check size of oncentration with respect to number of points

        size_t nbTimes = concentrations.size();
        Concentrations c;
        for(size_t i = 0; i < nbTimes; i++) {
            c.push_back(concentrations[i]);
            resp->m_times.push_back(timesSeries[i].getEventTime());
        }
        resp->m_concentrations.push_back(c);
        resp->m_unit = Unit("ug/l");

        _response->addResponse(std::move(resp));
        return ComputingResult::Success;
    }
    else {
        return ComputingResult::Error;
    }

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
