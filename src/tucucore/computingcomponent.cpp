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
#include "tucucore/generalextractor.h"
#include "tucucore/cyclestatisticscalculator.h"
#include "tucucore/computingservice/computingresult.h"
#include "tucucore/treatmentdrugmodelcompatibilitychecker.h"
#include "tucucore/computingadjustments.h"
#include "tucucore/computingutils.h"

namespace Tucuxi {
namespace Core {


CovariateEvent getCovariateAtTime(const DateTime &_date, const CovariateSeries &_covariates)
{
    // Find the lasted change that occured before the given date
    std::vector<CovariateEvent>::const_iterator it = _covariates.begin();
    if (it != _covariates.end()) {
        std::vector<CovariateEvent>::const_iterator itNext = it;
        while (++itNext != _covariates.end() && _date > itNext->getEventTime()) {
            it++;
        }
    }

    // Did we find something?
    if (it != _covariates.end())
    {
        // Make a copy to hold values with applied etas
        return CovariateEvent(*it);
    }

    // Should not happen
    std::cout << "Something bad is currently happening" << std::endl;
    return CovariateEvent(*it);
}


Tucuxi::Common::Interface* ComputingComponent::createComponent()
{
    ComputingComponent *cmp = new ComputingComponent();

    cmp->initialize();

    return dynamic_cast<IComputingService*>(cmp);
}

Tucuxi::Common::Interface* ComputingComponent::getInterface(const std::string &_name)
{
    return Tucuxi::Common::Component::getInterfaceImpl(_name);
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
    m_utils = std::make_unique<ComputingUtils>();

    m_utils->m_models = std::make_shared<PkModelCollection>();
    if (!defaultPopulate(*m_utils->m_models)) {
        m_logger.error("Could not populate the Pk models collection. No model will be available");
        return false;
    }
    m_utils->m_generalExtractor = std::make_unique<GeneralExtractor>();
    return true;
}

void ComputingComponent::setPkModelCollection(std::shared_ptr<PkModelCollection> _collection) {
    m_utils->m_models = _collection;
}


std::string ComputingComponent::getErrorString() const
{
    return "Error message function not yet implemented";
}


ComputingResult ComputingComponent::compute(const ComputingRequest &_request, std::unique_ptr<ComputingResponse> &_response)
{
//    try {
        if (m_utils == nullptr) {
            m_logger.error("The Computing Component has not been initialized");
            return ComputingResult::ComputingComponentNotInitialized;
        }
        if (m_utils->m_models == nullptr) {
            m_logger.error("No Pk Model loaded. Impossible to perform computation");
            return ComputingResult::NoPkModels;
        }
        // First ensure there is at least a Pk Model available
        if (m_utils->m_models->getPkModelList().size() == 0) {
            m_logger.error("No Pk Model loaded. Impossible to perform computation");
            return ComputingResult::NoPkModels;
        }

        TreatmentDrugModelCompatibilityChecker checker;
        if (!checker.checkCompatibility(&_request.getDrugTreatment(), &_request.getDrugModel())) {
            return ComputingResult::IncompatibleTreatmentModel;
        }

        // A simple iteration on the ComputingTraits. Each one is responsible to fill the _response object with
        // a new computing response
        ComputingTraits::Iterator it = _request.getComputingTraits().begin();
        ComputingResult result = ComputingResult::Ok;
        while (it != _request.getComputingTraits().end()) {
            ComputingResult internalResult = it->get()->compute(*this, _request, _response);
            if (internalResult != ComputingResult::Ok) {
                result = internalResult;
            }
            it++;
        }

        return result;

//    } catch (...) {
//        return ComputingResult::ComputingComponentExceptionError;
//    }
}



ComputingResult ComputingComponent::compute(
        const ComputingTraitConcentration *_traits,
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

    // Now ready to do the real computing with all the extracted values

    std::vector<ConcentrationPredictionPtr> analytesPredictions;

    ComputingResult computingResult;

    std::map<AnalyteGroupId, Etas> allEtas;

    for(const auto &analyteGroup : _request.getDrugModel().getAnalyteSets()) {
        AnalyteGroupId analyteGroupId = analyteGroup->getId();

        ConcentrationPredictionPtr pPrediction = std::make_unique<ConcentrationPrediction>();

        Etas etas;

        if (_traits->getComputingOption().getParametersType() == PredictionParameterType::Aposteriori) {
            ComputingResult extractionResult = m_utils->m_generalExtractor->extractAposterioriEtas(etas, _request, analyteGroupId, intakeSeries[analyteGroupId], parameterSeries[analyteGroupId], covariateSeries, calculationStartTime, _traits->getEnd());
            if (extractionResult != ComputingResult::Ok) {
                return extractionResult;
            }
        }

        allEtas[analyteGroupId] = etas;


        Tucuxi::Core::ConcentrationCalculator concentrationCalculator;
        computingResult = concentrationCalculator.computeConcentrations(
                    pPrediction,
                    false,
                    _traits->getStart(),
                    _traits->getEnd(),
                    intakeSeries[analyteGroupId],
                    parameterSeries[analyteGroupId],
                    etas);


        if (computingResult == ComputingResult::Ok)
        {
            analytesPredictions.push_back(std::move(pPrediction));
        }
        else {
            return computingResult;
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

    std::unique_ptr<SinglePredictionResponse> resp = std::make_unique<SinglePredictionResponse>(_traits->getId());

    IntakeSeries recordedIntakes;
    selectRecordedIntakes(recordedIntakes, intakeSeries[_request.getDrugModel().getAnalyteSets()[0]->getId()], _traits->getStart(), _traits->getEnd());

    if ((recordedIntakes.size() != analytesPredictions[0]->getTimes().size()) ||
            (recordedIntakes.size() != analytesPredictions[0]->getValues().size())) {
        return ComputingResult::RecordedIntakesSizeError;
    }


    ConcentrationPredictionPtr pPrediction = std::make_unique<ConcentrationPrediction>();

    // std::cout << "Start Time : " << _traits->getStart() << std::endl;
    for (size_t i = 0; i < recordedIntakes.size(); i++) {

        TimeOffsets times = analytesPredictions[0]->getTimes()[i];
        DateTime start = recordedIntakes[i].getEventTime();
        // std::cout << "Time index " << i << " : " << start << std::endl;
        // times values are in hours
        std::chrono::milliseconds ms = std::chrono::milliseconds(static_cast<int>(times.back() * 3600 * 1000));
        Duration ds(ms);
        DateTime end = start + ds;
        // std::cout << "End Time index " << i << " : " << end << std::endl;

        if (end > _traits->getStart()) {
            // std::cout << "Selected Time index " << i << " : " << start << std::endl;
            CycleData cycle(start, end, Unit("ug/l"));

            if (!_request.getDrugModel().isSingleAnalyte()) {
                for (const auto &activeMoiety : activeMoietiesPredictions) {
                    cycle.addData(times, activeMoiety->getValues()[i]);
                }
            }

            size_t index = 0;
            for(const auto &analyteGroup : _request.getDrugModel().getAnalyteSets()) {
                AnalyteGroupId analyteGroupId = analyteGroup->getId();

                cycle.addData(times, analytesPredictions[index]->getValues()[i]);
                index ++;
            }

            AnalyteGroupId analyteGroupId = _request.getDrugModel().getAnalyteSets()[0]->getId();
            ParameterSetEventPtr params = parameterSeries[analyteGroupId].getAtTime(start, allEtas[analyteGroupId]);

            for (auto p = params.get()->begin() ; p < params.get()->end() ; p++) {
                cycle.m_parameters.push_back({(*p).getParameterId(), (*p).getValue()});
            }

            for (const auto &cov : params->m_covariates) {
                cycle.m_covariates.push_back({cov.m_id, cov.m_value});
            }

            std::sort(cycle.m_parameters.begin(), cycle.m_parameters.end(),
                      [&] (const ParameterValue &_v1, const ParameterValue &_v2) { return _v1.m_parameterId < _v2.m_parameterId; });

            resp->addCycleData(cycle);
        }
    }

    if (!_request.getDrugModel().isSingleAnalyte()) {
        for (const auto & activeMoiety : _request.getDrugModel().getActiveMoieties()) {
            resp->addAnalyteId(activeMoiety->getActiveMoietyId().toString());
        }
    }

    for(const auto &analyteGroup : _request.getDrugModel().getAnalyteSets()) {
        AnalyteId analyteId = analyteGroup->getAnalytes()[0]->getAnalyteId();
        resp->addAnalyteId(analyteId.toString());
    }

    if (_traits->getComputingOption().getWithStatistics()) {
        CycleStatisticsCalculator c;
        c.calculate(resp->getModifiableData());
    }

    _response->addResponse(std::move(resp));
    return ComputingResult::Ok;
}

ComputingResult ComputingComponent::compute(
        const ComputingTraitPercentiles *_traits,
        const ComputingRequest &_request,
        std::unique_ptr<ComputingResponse> &_response)
{
#ifdef NO_PERCENTILES
    return ComputingResult::NoPercentilesCalculation;
#endif
    if (_request.getDrugModel().getAnalyteSets().size() > 1) {
        return computePercentilesMulti(_traits, _request, _response);
    }
    else {
        return computePercentilesSimple(_traits, _request, _response);
    }
}

ComputingResult ComputingComponent::computePercentilesMulti(
        const ComputingTraitPercentiles *_traits,
        const ComputingRequest &_request,
        std::unique_ptr<ComputingResponse> &_response)
{
#ifdef NO_PERCENTILES
    return ComputingResult::NoPercentilesCalculation;
#endif
    if (_traits == nullptr) {
        m_logger.error("The computing traits sent for computation are nullptr");
        return ComputingResult::NoComputingTraits;
    }

    std::map<AnalyteGroupId, std::shared_ptr<PkModel> > pkModel;

    GroupsIntakeSeries intakeSeries;
    CovariateSeries covariateSeries;
    GroupsParameterSetSeries parameterSeries;
    DateTime calculationStartTime;

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

    // Now ready to do the real computing with all the extracted values


    Tucuxi::Core::PercentilesPrediction percentiles;

    Tucuxi::Core::PercentileRanks percentileRanks;
    std::map<AnalyteGroupId, Tucuxi::Core::OmegaMatrix> omega;
    Tucuxi::Core::ComputingAborter *aborter = _traits->getAborter();

    percentileRanks = _traits->getRanks();


    ResidualErrorModelExtractor errorModelExtractor;
    std::map<AnalyteGroupId, std::unique_ptr<IResidualErrorModel> > residualErrorModel;

    for(const auto &analyteGroup : _request.getDrugModel().getAnalyteSets()) {
        AnalyteGroupId analyteGroupId = analyteGroup->getId();

        std::unique_ptr<IResidualErrorModel> errorModel;

        ComputingResult errorModelExtractionResult = errorModelExtractor.extract(analyteGroup->getAnalytes()[0]->getResidualErrorModel(),
                                                                                 analyteGroup->getAnalytes()[0]->getUnit(),
                                                                                 covariateSeries, errorModel);
        residualErrorModel[analyteGroupId] = std::move(errorModel);

        if (errorModelExtractionResult != ComputingResult::Ok) {
            return errorModelExtractionResult;
        }
    }


    for(const auto &analyteGroup : _request.getDrugModel().getAnalyteSets()) {
        AnalyteGroupId analyteGroupId = analyteGroup->getId();
        std::vector<const FullFormulationAndRoute *> fullFormulationAndRoutes = m_utils->m_generalExtractor->extractFormulationAndRoutes(_request.getDrugModel(), intakeSeries[analyteGroupId]);

        ComputingResult omegaComputingResult = m_utils->m_generalExtractor->extractOmega(_request.getDrugModel(), analyteGroupId, fullFormulationAndRoutes, omega[analyteGroupId]);
        if (omegaComputingResult != ComputingResult::Ok) {
            return omegaComputingResult;
        }
    }


    std::map<AnalyteGroupId, Etas> etas;


    Tucuxi::Core::ComputingResult computingResult;

    Tucuxi::Core::ConcentrationCalculator concentrationCalculator;

    if (_traits->getComputingOption().getParametersType() == PredictionParameterType::Aposteriori)   {

        for(const auto &analyteGroup : _request.getDrugModel().getAnalyteSets()) {
            AnalyteGroupId analyteGroupId = analyteGroup->getId();

            ComputingResult aposterioriEtasExtractionResult = m_utils->m_generalExtractor->extractAposterioriEtas(etas[analyteGroupId], _request, analyteGroupId, intakeSeries[analyteGroupId], parameterSeries[analyteGroupId], covariateSeries, calculationStartTime, _traits->getEnd());
            if (aposterioriEtasExtractionResult != ComputingResult::Ok) {
                return aposterioriEtasExtractionResult;
            }
        }


        // This extraction is already done in extractAposterioriEtas... Could be optimized
        std::map<AnalyteGroupId, SampleSeries> sampleSeries;

        for(const auto &analyteGroup : _request.getDrugModel().getAnalyteSets()) {
            AnalyteGroupId analyteGroupId = analyteGroup->getId();


            SampleExtractor sampleExtractor;
            ComputingResult sampleExtractionResult =
                    sampleExtractor.extract(_request.getDrugTreatment().getSamples(), _request.getDrugModel().getAnalyteSet(analyteGroupId), calculationStartTime, _traits->getEnd(), sampleSeries[analyteGroupId]);

            if (sampleExtractionResult != ComputingResult::Ok) {
                return sampleExtractionResult;
            }
        }

        std::unique_ptr<Tucuxi::Core::IAposterioriPercentileCalculatorMulti> calculator(new Tucuxi::Core::AposterioriMonteCarloPercentileCalculatorMulti());
        computingResult = calculator->calculate(
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
                    _request.getDrugModel().getActiveMoieties()[0].get(),
                    aborter);

    }
    else {
        for(const auto &analyteGroup : _request.getDrugModel().getAnalyteSets()) {
            AnalyteGroupId analyteGroupId = analyteGroup->getId();
            etas[analyteGroupId] = Etas(0);
        }

        std::unique_ptr<Tucuxi::Core::IAprioriPercentileCalculatorMulti> calculator(new Tucuxi::Core::AprioriPercentileCalculatorMulti());
        computingResult = calculator->calculate(
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
                    _request.getDrugModel().getActiveMoieties()[0].get(),
                    aborter);
    }

    // We use a single prediction to get back time offsets. Could be optimized
    // YTA: I do not think this is relevant. It could be deleted I guess
    ConcentrationPredictionPtr pPrediction = std::make_unique<ConcentrationPrediction>();

    AnalyteGroupId analyteGroupId = _request.getDrugModel().getAnalyteSets()[0]->getId();

    ComputingResult predictionResult = concentrationCalculator.computeConcentrations(
                pPrediction,
                false,
                _traits->getStart(),
                _traits->getEnd(),
                intakeSeries[analyteGroupId],
                parameterSeries[analyteGroupId]);

    if (predictionResult != ComputingResult::Ok) {
        return predictionResult;
    }

    IntakeSeries selectedIntakes;
    selectRecordedIntakes(selectedIntakes, intakeSeries[analyteGroupId], _traits->getStart(), _traits->getEnd());

    if (computingResult == ComputingResult::Ok &&
            selectedIntakes.size() == pPrediction->getTimes().size() &&
            selectedIntakes.size() == pPrediction->getValues().size())
    {
        std::unique_ptr<PercentilesResponse> resp = std::make_unique<PercentilesResponse>(_traits->getId());

        const std::vector<std::vector<std::vector<Value> > > allValues = percentiles.getValues();



        for (unsigned int p = 0; p < percentiles.getRanks().size(); p++) {
            std::vector<CycleData> percData;

            if (selectedIntakes.size() != allValues[p].size()) {
                return ComputingResult::SelectedIntakesSizeError;
            }
            if (selectedIntakes.size() != pPrediction->getTimes().size()) {
                return ComputingResult::SelectedIntakesSizeError;
            }

            for (unsigned int cycle = 0; cycle < allValues[p].size(); cycle ++) {

                TimeOffsets times = pPrediction->getTimes()[cycle];
                DateTime start = selectedIntakes[cycle].getEventTime();

                std::chrono::milliseconds ms = std::chrono::milliseconds(static_cast<int>(times.back())) * 3600 * 1000;
                Duration ds(ms);
                DateTime end = start + ds;

                if (end > _traits->getStart()) {
                    CycleData cycleData(start, end, Unit("ug/l"));
                    cycleData.addData(times, allValues[p][cycle]);
                    percData.push_back(cycleData);
                }
            }

            if (_traits->getComputingOption().getWithStatistics()) {
                CycleStatisticsCalculator c;
                c.calculate(percData);
            }

            resp->addPercentileData(percData);
        }
        resp->setRanks(percentileRanks);

        _response->addResponse(std::move(resp));
        return ComputingResult::Ok;
    }
    else {
        if (computingResult != ComputingResult::Ok) {
            return computingResult;
        }
        else {
            return ComputingResult::SelectedIntakesSizeError;
        }
    }
}

ComputingResult ComputingComponent::computePercentilesSimple(
        const ComputingTraitPercentiles *_traits,
        const ComputingRequest &_request,
        std::unique_ptr<ComputingResponse> &_response)
{
#ifdef NO_PERCENTILES
    return ComputingResult::NoPercentilesCalculation;
#endif
    if (_traits == nullptr) {
        m_logger.error("The computing traits sent for computation are nullptr");
        return ComputingResult::NoComputingTraits;
    }

    std::map<AnalyteGroupId, std::shared_ptr<PkModel> > pkModel;

    GroupsIntakeSeries intakeSeries;
    CovariateSeries covariateSeries;
    GroupsParameterSetSeries parameterSeries;
    DateTime calculationStartTime;

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

    // Now ready to do the real computing with all the extracted values



    // TODO : Change this analyte group
    AnalyteGroupId analyteGroupId = _request.getDrugModel().getAnalyteSets()[0]->getId();


    Tucuxi::Core::PercentilesPrediction percentiles;

    Tucuxi::Core::PercentileRanks percentileRanks;
    Tucuxi::Core::OmegaMatrix omega;
    Tucuxi::Core::ComputingAborter *aborter = _traits->getAborter();

    percentileRanks = _traits->getRanks();


    ResidualErrorModelExtractor errorModelExtractor;
    std::unique_ptr<IResidualErrorModel> residualErrorModel;
    ComputingResult errorModelExtractionResult = errorModelExtractor.extract(_request.getDrugModel().getAnalyteSet()->getAnalytes()[0]->getResidualErrorModel(),
                                                                             _request.getDrugModel().getAnalyteSet()->getAnalytes()[0]->getUnit(),
                                                                             covariateSeries, residualErrorModel);
    if (errorModelExtractionResult != ComputingResult::Ok) {
        return errorModelExtractionResult;
    }

    std::vector<const FullFormulationAndRoute *> fullFormulationAndRoutes = m_utils->m_generalExtractor->extractFormulationAndRoutes(_request.getDrugModel(), intakeSeries[analyteGroupId]);

    ComputingResult omegaComputingResult = m_utils->m_generalExtractor->extractOmega(_request.getDrugModel(), analyteGroupId, fullFormulationAndRoutes, omega);
    if (omegaComputingResult != ComputingResult::Ok) {
        return omegaComputingResult;
    }


    // Set initial etas to 0 for all variable parameters
    Tucuxi::Core::Etas etas(static_cast<size_t>(omega.cols()), 0.0);

    Tucuxi::Core::ComputingResult computingResult;

    Tucuxi::Core::ConcentrationCalculator concentrationCalculator;

    if (_traits->getComputingOption().getParametersType() == PredictionParameterType::Aposteriori)   {
        ComputingResult aposterioriEtasExtractionResult = m_utils->m_generalExtractor->extractAposterioriEtas(etas, _request, analyteGroupId, intakeSeries[analyteGroupId], parameterSeries[analyteGroupId], covariateSeries, calculationStartTime, _traits->getEnd());
        if (aposterioriEtasExtractionResult != ComputingResult::Ok) {
            return aposterioriEtasExtractionResult;
        }

        // This extraction is already done in extractAposterioriEtas... Could be optimized
        SampleSeries sampleSeries;
        SampleExtractor sampleExtractor;
        ComputingResult sampleExtractionResult =
                sampleExtractor.extract(_request.getDrugTreatment().getSamples(), _request.getDrugModel().getAnalyteSet(analyteGroupId), calculationStartTime, _traits->getEnd(), sampleSeries);

        if (sampleExtractionResult != ComputingResult::Ok) {
            return sampleExtractionResult;
        }

        std::unique_ptr<Tucuxi::Core::IAposterioriPercentileCalculator> calculator(new Tucuxi::Core::AposterioriMonteCarloPercentileCalculator());
        computingResult = calculator->calculate(
                    percentiles,
                    _traits->getStart(),
                    _traits->getEnd(),
                    intakeSeries[analyteGroupId],
                    parameterSeries[analyteGroupId],
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
        computingResult = calculator->calculate(
                    percentiles,
                    _traits->getStart(),
                    _traits->getEnd(),
                    intakeSeries[analyteGroupId],
                    parameterSeries[analyteGroupId],
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

    ComputingResult predictionResult = concentrationCalculator.computeConcentrations(
                pPrediction,
                false,
                _traits->getStart(),
                _traits->getEnd(),
                intakeSeries[analyteGroupId],
                parameterSeries[analyteGroupId]);

    if (predictionResult != ComputingResult::Ok) {
        return predictionResult;
    }

    IntakeSeries selectedIntakes;
    selectRecordedIntakes(selectedIntakes, intakeSeries[analyteGroupId], _traits->getStart(), _traits->getEnd());

    if (computingResult == ComputingResult::Ok &&
            selectedIntakes.size() == pPrediction->getTimes().size() &&
            selectedIntakes.size() == pPrediction->getValues().size())
    {
        std::unique_ptr<PercentilesResponse> resp = std::make_unique<PercentilesResponse>(_traits->getId());

        const std::vector<std::vector<std::vector<Value> > > allValues = percentiles.getValues();



        for (unsigned int p = 0; p < percentiles.getRanks().size(); p++) {
            std::vector<CycleData> percData;

            if (selectedIntakes.size() != allValues[p].size()) {
                return ComputingResult::SelectedIntakesSizeError;
            }
            if (selectedIntakes.size() != pPrediction->getTimes().size()) {
                return ComputingResult::SelectedIntakesSizeError;
            }

            for (unsigned int cycle = 0; cycle < allValues[p].size(); cycle ++) {

                TimeOffsets times = pPrediction->getTimes()[cycle];
                DateTime start = selectedIntakes[cycle].getEventTime();

                std::chrono::milliseconds ms = std::chrono::milliseconds(static_cast<int>(times.back())) * 3600 * 1000;
                Duration ds(ms);
                DateTime end = start + ds;

                if (end > _traits->getStart()) {
                    CycleData cycleData(start, end, Unit("ug/l"));
                    cycleData.addData(times, allValues[p][cycle]);
                    percData.push_back(cycleData);
                }
            }

            if (_traits->getComputingOption().getWithStatistics()) {
                CycleStatisticsCalculator c;
                c.calculate(percData);
            }

            resp->addPercentileData(percData);
        }
        resp->setRanks(percentileRanks);

        _response->addResponse(std::move(resp));
        return ComputingResult::Ok;
    }
    else {
        if (computingResult != ComputingResult::Ok) {
            return computingResult;
        }
        else {
            return ComputingResult::SelectedIntakesSizeError;
        }
    }
}


ComputingResult ComputingComponent::compute(
        const ComputingTraitAdjustment *_traits,
        const ComputingRequest &_request,
        std::unique_ptr<ComputingResponse> &_response)
{

    if (_traits == nullptr) {
        m_logger.error("The computing traits sent for computation are nullptr");
        return ComputingResult::NoComputingTraits;
    }
   ComputingAdjustments computer(m_utils.get());
   return computer.compute(_traits, _request, _response);
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
        return ComputingResult::NoComputingTraits;
    }

    if (_traits->getTimes().size() == 0) {
        // No time given, so we return an empty response
        std::unique_ptr<SinglePointsResponse> resp = std::make_unique<SinglePointsResponse>(_traits->getId());
        _response->addResponse(std::move(resp));
        return ComputingResult::Ok;
    }

    std::map<AnalyteGroupId, std::shared_ptr<PkModel> > pkModel;

    GroupsIntakeSeries intakeSeries;
    CovariateSeries covariateSeries;
    GroupsParameterSetSeries parameterSeries;
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

    // TODO : Check if 3 is necessary or not
    ComputingTraitStandard standardTraits(_traits->getId(), firstDate, lastDate, 3, _traits->getComputingOption());

    ComputingResult extractionResult = m_utils->m_generalExtractor->generalExtractions(&standardTraits,
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

    if (_request.getDrugModel().getAnalyteSets().size() == 0) {
        return ComputingResult::NoAnalytesGroup;
    }

    for (const auto & analyteGroup : _request.getDrugModel().getAnalyteSets()) {
        AnalyteGroupId analyteGroupId = analyteGroup->getId();

        // Now ready to do the real computing with all the extracted values

        ConcentrationPredictionPtr pPrediction = std::make_unique<ConcentrationPrediction>();

        ComputingResult computingResult;

        Etas etas;

        if (_traits->getComputingOption().getParametersType() == PredictionParameterType::Aposteriori) {
            ComputingResult aposterioriEtasExtractionResult = m_utils->m_generalExtractor->extractAposterioriEtas(etas, _request, analyteGroupId, intakeSeries[analyteGroupId], parameterSeries[analyteGroupId], covariateSeries, calculationStartTime, lastDate);
            if (aposterioriEtasExtractionResult != ComputingResult::Ok) {
                return aposterioriEtasExtractionResult;
            }
        }

        Concentrations concentrations;

        SampleSeries timesSeries;

        for (const auto &times : _traits->getTimes()) {
            timesSeries.push_back(SampleEvent(times));
        }

        // Sort the times in chronological order
        std::sort(timesSeries.begin(), timesSeries.end(), [](const SampleEvent &_a, const SampleEvent &_b)
        {
            return _a.getEventTime() < _b.getEventTime();
        });

        ConcentrationCalculator calculator;
        computingResult = calculator.computeConcentrationsAtTimes(
                    concentrations,
                    false,
                    intakeSeries[analyteGroupId],
                    parameterSeries[analyteGroupId],
                    timesSeries,
                    etas,
                    true);


        if (computingResult == ComputingResult::Ok)
        {
            std::unique_ptr<SinglePointsResponse> resp = std::make_unique<SinglePointsResponse>(_traits->getId());

            if (concentrations.size() != timesSeries.size()) {
                // Something went wrong
                m_logger.error("The number of concentrations calculated is not the same as the number of points asked");
                return ComputingResult::ConcentrationSizeError;
            }

            size_t nbTimes = concentrations.size();
            Concentrations c;
            for(size_t i = 0; i < nbTimes; i++) {
                c.push_back(concentrations[i]);
                resp->m_times.push_back(timesSeries[i].getEventTime());
            }
            resp->m_concentrations.push_back(c);
            resp->m_unit = Unit("ug/l");

            _response->addResponse(std::move(resp));
        }
        else {
            return computingResult;
        }

    }
    return ComputingResult::Ok;
}


} // namespace Core
} // namespace Tucuxi
