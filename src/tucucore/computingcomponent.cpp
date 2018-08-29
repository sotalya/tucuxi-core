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

#include "tucucore/pkmodels/onecompartmentbolus.h"
#include "tucucore/pkmodels/onecompartmentextra.h"
#include "tucucore/pkmodels/onecompartmentinfusion.h"
#include "tucucore/pkmodels/twocompartmentbolus.h"
#include "tucucore/pkmodels/twocompartmentextra.h"
#include "tucucore/pkmodels/twocompartmentinfusion.h"
#include "tucucore/pkmodels/threecompartmentbolus.h"
#include "tucucore/pkmodels/threecompartmentextra.h"
#include "tucucore/pkmodels/threecompartmentinfusion.h"

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


ComputingResult ComputingComponent::compute(
        const ComputingTraitConcentration *_traits,
        const ComputingRequest &_request,
        std::unique_ptr<ComputingResponse> &_response)
{

    if (_traits == nullptr) {
        m_logger.error("The computing traits sent for computation are nullptr");
        return ComputingResult::Error;
    }


    IntakeSeries intakeSeries;
    int nIntakes = IntakeExtractor::extract(_request.getDrugTreatment().getDosageHistory(false), _traits->getStart(), _traits->getEnd(), intakeSeries, _traits->getCycleSize());
    TMP_UNUSED_PARAMETER(nIntakes);

    static const std::string analyteId = "imatinib";
    static const std::string pkModelId = _request.getDrugModel().getPkModelId();
    static const std::string formulation = "???";
    static const Route route = Route::IntravenousBolus; // ???

    std::shared_ptr<PkModel> pkModel = m_models->getPkModelFromId(pkModelId);

    if (pkModel == nullptr) {
        m_logger.error("Can not find a Pk Model for the calculation");
        return ComputingResult::Error;
    }


    IntakeToCalculatorAssociator::Result intakeExtractionResult = IntakeToCalculatorAssociator::associate(intakeSeries, *pkModel.get());

    if (intakeExtractionResult != IntakeToCalculatorAssociator::Result::Ok) {
        m_logger.error("Can not associate intake calculators for the specified route");
        return ComputingResult::Error;
    }

    CovariateSeries covariateSeries;
    CovariateExtractor covariateExtractor(_request.getDrugModel().getCovariates(),
                                          _request.getDrugTreatment().getCovariates(),
                                          _traits->getStart(),
                                          _traits->getEnd());
    CovariateExtractor::Result covariateExtractionResult = covariateExtractor.extract(covariateSeries);

    if (covariateExtractionResult != CovariateExtractor::Result::Ok) {
        m_logger.error("Can not extract covariates");
        return ComputingResult::Error;
    }

    ParameterDefinitionIterator it = _request.getDrugModel().getParameterDefinitions(analyteId, formulation, route);

    ParameterSetSeries parameterSeries;
    ParametersExtractor parameterExtractor(covariateSeries,
                                           it,
                                           _traits->getStart(),
                                           _traits->getEnd());

    ParametersExtractor::Result parametersExtractionResult = parameterExtractor.extract(parameterSeries);

    if (parametersExtractionResult != ParametersExtractor::Result::Ok) {
        m_logger.error("Can not extract parameters");
        return ComputingResult::Error;
    }

    ConcentrationPredictionPtr pPrediction = std::make_unique<ConcentrationPrediction>();
    ComputationResult result = computePopulation(
                pPrediction,
                false,
                _traits->getStart(),
                _traits->getEnd(),
                intakeSeries,
                parameterSeries);

    if (result == ComputationResult::Success &&
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
    }

    return ComputingResult::Success;
}

ComputingResult ComputingComponent::compute(
        const ComputingTraitPercentiles *_traits,
        const ComputingRequest &_request,
        std::unique_ptr<ComputingResponse> &_response)
{
    TMP_UNUSED_PARAMETER(_traits);
    TMP_UNUSED_PARAMETER(_request);
    TMP_UNUSED_PARAMETER(_response);
    return ComputingResult::Error;
}

ComputingResult ComputingComponent::compute(
        const ComputingTraitAdjustment *_traits,
        const ComputingRequest &_request,
        std::unique_ptr<ComputingResponse> &_response)
{
    TMP_UNUSED_PARAMETER(_traits);
    TMP_UNUSED_PARAMETER(_request);
    TMP_UNUSED_PARAMETER(_response);
    return ComputingResult::Error;
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
    const bool _isAll,
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


} // Core
} // Tucuxi
