/*
* Copyright (C) 2017 Tucuxi SA
*/

#include <chrono> // for high_resolution_clock

#include "computingcomponent.h"

#include "tucucommon/general.h"

#include "tucucore/aposteriorietascalculator.h"
#include "tucucore/computingcomponent.h"
#include "tucucore/computingservice/computingrequest.h"
#include "tucucore/computingservice/computingresponse.h"
#include "tucucore/computingservice/computingtrait.h"
#include "tucucore/computingservice/icomputingservice.h"
#include "tucucore/concentrationcalculator.h"
#include "tucucore/drugmodel/drugmodel.h"
#include "tucucore/drugtreatment/drugtreatment.h"
#include "tucucore/intaketocalculatorassociator.h"
#include "tucucore/montecarlopercentilecalculator.h"
#include "tucucore/multimontecarlopercentilecalculator.h"
#include "tucucore/percentilesprediction.h"
#include "tucucore/pkmodel.h"
#include "tucucore/sampleextractor.h"
//#include "tucucore/overloadevaluator.h"
#include "tucucore/computingadjustments.h"
#include "tucucore/computingservice/computingresult.h"
#include "tucucore/computingutils.h"
#include "tucucore/cyclestatisticscalculator.h"
#include "tucucore/generalextractor.h"
#include "tucucore/residualerrormodelextractor.h"
#include "tucucore/treatmentdrugmodelcompatibilitychecker.h"

namespace Tucuxi {
namespace Core {


Tucuxi::Common::Interface* ComputingComponent::createComponent()
{
    ComputingComponent* cmp = new ComputingComponent();

    cmp->initialize();

    return dynamic_cast<IComputingService*>(cmp);
}

Tucuxi::Common::Interface* ComputingComponent::getInterface(const std::string& _name)
{
    return Tucuxi::Common::Component::getInterfaceImpl(_name);
}

ComputingComponent::ComputingComponent()
{
    registerInterface(dynamic_cast<IComputingService*>(this));
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

void ComputingComponent::setPkModelCollection(std::shared_ptr<PkModelCollection> _collection)
{
    m_utils->m_models = std::move(_collection);
}


std::string ComputingComponent::getErrorString() const
{
    return "Error message function not yet implemented";
}


ComputingStatus ComputingComponent::compute(
        const ComputingRequest& _request, std::unique_ptr<ComputingResponse>& _response)
{
    // Record start time
    auto start = std::chrono::high_resolution_clock::now();
    TUCU_TRY
    {
        if (m_utils == nullptr) {
            m_logger.error("The Computing Component has not been initialized");
            _response->setComputingStatus(ComputingStatus::ComputingComponentNotInitialized);
            return ComputingStatus::ComputingComponentNotInitialized;
        }
        if (m_utils->m_models == nullptr) {
            m_logger.error("No Pk Model loaded. Impossible to perform computation");
            _response->setComputingStatus(ComputingStatus::NoPkModels);
            return ComputingStatus::NoPkModels;
        }
        // First ensure there is at least a Pk Model available
        if (m_utils->m_models->getPkModelList().empty()) {
            m_logger.error("No Pk Model loaded. Impossible to perform computation");
            _response->setComputingStatus(ComputingStatus::NoPkModels);
            return ComputingStatus::NoPkModels;
        }

        TreatmentDrugModelCompatibilityChecker checker;
        if (!checker.checkCompatibility(&_request.getDrugTreatment(), &_request.getDrugModel())) {
            _response->setComputingStatus(ComputingStatus::IncompatibleTreatmentModel);
            return ComputingStatus::IncompatibleTreatmentModel;
        }

        // A simple iteration on the ComputingTraits. Each one is responsible to fill the _response object with
        // a new computing response
        ComputingTraits::Iterator it = _request.getComputingTraits().begin();
        ComputingStatus result = ComputingStatus::Ok;
        while (it != _request.getComputingTraits().end()) {
            ComputingStatus internalResult = it->get()->compute(*this, _request, _response);
            if (internalResult != ComputingStatus::Ok) {
                result = internalResult;
            }
            it++;
        }

        // Record end time
        auto finish = std::chrono::high_resolution_clock::now();
        // Store the computing time in the response
        _response->setComputingTimeInSeconds(finish - start);

        _response->setComputingStatus(result);
        return result;
    }
    TUCU_CATCH(...)
    {
        _response->setComputingStatus(ComputingStatus::ComputingComponentExceptionError);
        return ComputingStatus::ComputingComponentExceptionError;
    }
}


ComputingStatus ComputingComponent::recordCycle(
        const ComputingTraitStandard* _traits,
        const ComputingRequest& _request,
        ConcentrationData& _concentrationData,
        DateTime _start,
        DateTime _end,
        const TimeOffsets& _times,
        const std::vector<ConcentrationPredictionPtr>& _activeMoietiesPredictions,
        const std::vector<ConcentrationPredictionPtr>& _analytesPredictions,
        size_t _valueIndex,
        const std::map<AnalyteGroupId, Etas>& _etas,
        GroupsParameterSetSeries& _parameterSeries)
{
    // The final unit depends on the computing options
    TucuUnit unit("ug/l");
    if (_traits->getComputingOption().forceUgPerLiter() == ForceUgPerLiterOption::DoNotForce) {
        unit = _request.getDrugModel().getActiveMoieties()[0]->getUnit();
    }

    CycleData cycle(_start, _end, unit);

    if (!_request.getDrugModel().isSingleAnalyte()) {
        for (const auto& activeMoiety : _activeMoietiesPredictions) {
            cycle.addData(
                    _times,
                    Tucuxi::Common::UnitManager::convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                            activeMoiety->getValues()[_valueIndex],
                            _request.getDrugModel().getActiveMoieties()[0]->getUnit(),
                            unit));
        }
    }

    size_t index = 0;
    for (const auto& analyteGroup : _request.getDrugModel().getAnalyteSets()) {
        AnalyteGroupId analyteGroupId = analyteGroup->getId();

        cycle.addData(
                _times,
                Tucuxi::Common::UnitManager::convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                        _analytesPredictions[index]->getValues()[_valueIndex],
                        _request.getDrugModel().getActiveMoieties()[0]->getUnit(),
                        unit));
        index++;
    }

    AnalyteGroupId analyteGroupId = _request.getDrugModel().getAnalyteSets()[0]->getId();
    ParameterSetEventPtr params = _parameterSeries[analyteGroupId].getAtTime(_start, _etas.at(analyteGroupId));

    if (_traits->getComputingOption().retrieveParameters() == RetrieveParametersOption::RetrieveParameters) {
        for (auto p = params->begin(); p < params->end(); p++) {
            cycle.m_parameters.push_back({(*p).getParameterId(), (*p).getValue()});
        }
        std::sort(
                cycle.m_parameters.begin(),
                cycle.m_parameters.end(),
                [&](const ParameterValue& _v1, const ParameterValue& _v2) {
                    return _v1.m_parameterId < _v2.m_parameterId;
                });
    }

    if (_traits->getComputingOption().retrieveCovariates() == RetrieveCovariatesOption::RetrieveCovariates) {
        for (const auto& cov : params->m_covariates) {
            cycle.m_covariates.push_back({cov.m_id, cov.m_value});
        }
    }

    _concentrationData.addCycleData(cycle);

    return ComputingStatus::Ok;
}

void ComputingComponent::endRecord(
        const ComputingTraitStandard* _traits, const ComputingRequest& _request, ConcentrationData& _concentrationData)
{
    if (_request.getDrugModel().isSingleAnalyte()) {
        AnalyteId analyteId = _request.getDrugModel().getAnalyteSets()[0]->getAnalytes()[0]->getAnalyteId();
        _concentrationData.addCompartmentInfo(
                {CompartmentInfo::CompartmentType::ActiveMoietyAndAnalyte, analyteId.toString()});
    }
    if (!_request.getDrugModel().isSingleAnalyte()) {
        for (const auto& activeMoiety : _request.getDrugModel().getActiveMoieties()) {
            _concentrationData.addCompartmentInfo(
                    {CompartmentInfo::CompartmentType::ActiveMoiety, activeMoiety->getActiveMoietyId().toString()});
        }

        for (const auto& analyteGroup : _request.getDrugModel().getAnalyteSets()) {
            AnalyteId analyteId = analyteGroup->getAnalytes()[0]->getAnalyteId();
            _concentrationData.addCompartmentInfo({CompartmentInfo::CompartmentType::Analyte, analyteId.toString()});
        }
    }

    if (_traits->getComputingOption().retrieveStatistics() == RetrieveStatisticsOption::RetrieveStatistics) {
        CycleStatisticsCalculator c;
        c.calculate(_concentrationData.getModifiableData());
    }
}


ComputingStatus ComputingComponent::compute(
        const ComputingTraitConcentration* _traits,
        const ComputingRequest& _request,
        std::unique_ptr<ComputingResponse>& _response)
{

    if (_traits == nullptr) {
        m_logger.error("The computing traits sent for computation are nullptr");
        return ComputingStatus::NoComputingTraits;
    }

    std::map<AnalyteGroupId, std::shared_ptr<PkModel> > pkModel;

    GroupsIntakeSeries intakeSeries;
    CovariateSeries covariateSeries;
    GroupsParameterSetSeries parameterSeries;
    DateTime calculationStartTime;

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

    // Now ready to do the real computing with all the extracted values

    std::vector<ConcentrationPredictionPtr> analytesPredictions;

    ComputingStatus computingResult;

    std::map<AnalyteGroupId, Etas> allEtas;

    for (const auto& analyteGroup : _request.getDrugModel().getAnalyteSets()) {
        AnalyteGroupId analyteGroupId = analyteGroup->getId();

        ConcentrationPredictionPtr pPrediction = std::make_unique<ConcentrationPrediction>();

        Etas etas;

        if (_traits->getComputingOption().getParametersType() == PredictionParameterType::Aposteriori) {
            ComputingStatus extractionResult = m_utils->m_generalExtractor->extractAposterioriEtas(
                    etas,
                    _request,
                    analyteGroupId,
                    intakeSeries[analyteGroupId],
                    parameterSeries[analyteGroupId],
                    covariateSeries,
                    calculationStartTime,
                    _traits->getEnd());
            if (extractionResult != ComputingStatus::Ok) {
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


        if (computingResult == ComputingStatus::Ok) {
            analytesPredictions.push_back(std::move(pPrediction));
        }
        else {
            return computingResult;
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

    std::unique_ptr<SinglePredictionData> resp = std::make_unique<SinglePredictionData>(_request.getId());

    IntakeSeries recordedIntakes;
    selectRecordedIntakes(
            recordedIntakes,
            intakeSeries[_request.getDrugModel().getAnalyteSets()[0]->getId()],
            _traits->getStart(),
            _traits->getEnd());

    if ((recordedIntakes.size() != analytesPredictions[0]->getTimes().size())
        || (recordedIntakes.size() != analytesPredictions[0]->getValues().size())) {
        return ComputingStatus::RecordedIntakesSizeError;
    }

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
            ComputingStatus recordStatus = recordCycle(
                    _traits,
                    _request,
                    *resp,
                    start,
                    end,
                    times,
                    activeMoietiesPredictions,
                    analytesPredictions,
                    i,
                    allEtas,
                    parameterSeries);
            if (recordStatus != ComputingStatus::Ok) {
                return recordStatus;
            }
        }
    }

    endRecord(_traits, _request, *resp);

    _response->addResponse(std::move(resp));
    return ComputingStatus::Ok;
}

ComputingStatus ComputingComponent::compute(
        const ComputingTraitPercentiles* _traits,
        const ComputingRequest& _request,
        std::unique_ptr<ComputingResponse>& _response)
{
#ifdef NO_PERCENTILES
    return ComputingStatus::NoPercentilesCalculation;
#endif
    if (_request.getDrugModel().getAnalyteSets().size() > 1) {
        return computePercentilesMulti(_traits, _request, _response);
    }
    return computePercentilesSimple(_traits, _request, _response);
}

ComputingStatus ComputingComponent::computePercentilesMulti(
        const ComputingTraitPercentiles* _traits,
        const ComputingRequest& _request,
        std::unique_ptr<ComputingResponse>& _response)
{
#ifdef NO_PERCENTILES
    return ComputingStatus::NoPercentilesCalculation;
#endif
    if (_traits == nullptr) {
        m_logger.error("The computing traits sent for computation are nullptr");
        return ComputingStatus::NoComputingTraits;
    }

    std::map<AnalyteGroupId, std::shared_ptr<PkModel> > pkModel;

    GroupsIntakeSeries intakeSeries;
    CovariateSeries covariateSeries;
    GroupsParameterSetSeries parameterSeries;
    DateTime calculationStartTime;

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

    // Now ready to do the real computing with all the extracted values


    Tucuxi::Core::PercentilesPrediction percentiles;

    Tucuxi::Core::PercentileRanks percentileRanks;
    std::map<AnalyteGroupId, Tucuxi::Core::OmegaMatrix> omega;
    Tucuxi::Core::ComputingAborter* aborter = _traits->getAborter();

    percentileRanks = _traits->getRanks();


    ResidualErrorModelExtractor errorModelExtractor;
    std::map<AnalyteGroupId, std::unique_ptr<IResidualErrorModel> > residualErrorModel;

    for (const auto& analyteGroup : _request.getDrugModel().getAnalyteSets()) {
        AnalyteGroupId analyteGroupId = analyteGroup->getId();

        std::unique_ptr<IResidualErrorModel> errorModel;

        ComputingStatus errorModelExtractionResult = errorModelExtractor.extract(
                analyteGroup->getAnalytes()[0]->getResidualErrorModel(),
                analyteGroup->getAnalytes()[0]->getUnit(),
                analyteGroup->getAnalytes()[0]->getUnit(),
                covariateSeries,
                errorModel);
        residualErrorModel[analyteGroupId] = std::move(errorModel);

        if (errorModelExtractionResult != ComputingStatus::Ok) {
            return errorModelExtractionResult;
        }
    }


    for (const auto& analyteGroup : _request.getDrugModel().getAnalyteSets()) {
        AnalyteGroupId analyteGroupId = analyteGroup->getId();
        std::vector<const FullFormulationAndRoute*> fullFormulationAndRoutes =
                m_utils->m_generalExtractor->extractFormulationAndRoutes(
                        _request.getDrugModel(), intakeSeries[analyteGroupId]);

        ComputingStatus omegaComputingResult = m_utils->m_generalExtractor->extractOmega(
                _request.getDrugModel(), analyteGroupId, fullFormulationAndRoutes, omega[analyteGroupId]);
        if (omegaComputingResult != ComputingStatus::Ok) {
            return omegaComputingResult;
        }
    }


    std::map<AnalyteGroupId, Etas> etas;


    Tucuxi::Core::ComputingStatus computingResult;

    Tucuxi::Core::ConcentrationCalculator concentrationCalculator;

    if (_traits->getComputingOption().getParametersType() == PredictionParameterType::Aposteriori) {

        for (const auto& analyteGroup : _request.getDrugModel().getAnalyteSets()) {
            AnalyteGroupId analyteGroupId = analyteGroup->getId();

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


        // This extraction is already done in extractAposterioriEtas... Could be optimized
        std::map<AnalyteGroupId, SampleSeries> sampleSeries;

        for (const auto& analyteGroup : _request.getDrugModel().getAnalyteSets()) {
            AnalyteGroupId analyteGroupId = analyteGroup->getId();


            SampleExtractor sampleExtractor;
            ComputingStatus sampleExtractionResult = sampleExtractor.extract(
                    _request.getDrugTreatment().getSamples(),
                    _request.getDrugModel().getAnalyteSet(analyteGroupId),
                    calculationStartTime,
                    _traits->getEnd(),
                    _request.getDrugModel().getAnalyteSet(analyteGroupId)->getConcentrationUnit(),
                    sampleSeries[analyteGroupId]);

            if (sampleExtractionResult != ComputingStatus::Ok) {
                return sampleExtractionResult;
            }
        }

        std::unique_ptr<Tucuxi::Core::IAposterioriPercentileCalculatorMulti> calculator(
                new Tucuxi::Core::AposterioriMonteCarloPercentileCalculatorMulti());
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
        for (const auto& analyteGroup : _request.getDrugModel().getAnalyteSets()) {
            AnalyteGroupId analyteGroupId = analyteGroup->getId();
            etas[analyteGroupId] = Etas(0);
        }

        std::unique_ptr<Tucuxi::Core::IAprioriPercentileCalculatorMulti> calculator =
                std::make_unique<Tucuxi::Core::AprioriPercentileCalculatorMulti>();
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

    if (computingResult != ComputingStatus::Ok) {
        return computingResult;
    }

    // We use a single prediction to get back time offsets. Could be optimized
    // YTA: I do not think this is relevant. It could be deleted I guess
    ConcentrationPredictionPtr pPrediction = std::make_unique<ConcentrationPrediction>();

    AnalyteGroupId analyteGroupId = _request.getDrugModel().getAnalyteSets()[0]->getId();

    ComputingStatus predictionResult = concentrationCalculator.computeConcentrations(
            pPrediction,
            false,
            _traits->getStart(),
            _traits->getEnd(),
            intakeSeries[analyteGroupId],
            parameterSeries[analyteGroupId]);

    if (predictionResult != ComputingStatus::Ok) {
        return predictionResult;
    }

    return preparePercentilesResponse(
            _traits, _request, _response, intakeSeries, pPrediction, percentiles, percentileRanks);
}

ComputingStatus ComputingComponent::computePercentilesSimple(
        const ComputingTraitPercentiles* _traits,
        const ComputingRequest& _request,
        std::unique_ptr<ComputingResponse>& _response)
{
#ifdef NO_PERCENTILES
    return ComputingStatus::NoPercentilesCalculation;
#endif
    if (_traits == nullptr) {
        m_logger.error("The computing traits sent for computation are nullptr");
        return ComputingStatus::NoComputingTraits;
    }

    std::map<AnalyteGroupId, std::shared_ptr<PkModel> > pkModel;

    GroupsIntakeSeries intakeSeries;
    CovariateSeries covariateSeries;
    GroupsParameterSetSeries parameterSeries;
    DateTime calculationStartTime;

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

    // Now ready to do the real computing with all the extracted values



    // TODO : Change this analyte group
    AnalyteGroupId analyteGroupId = _request.getDrugModel().getAnalyteSets()[0]->getId();


    Tucuxi::Core::PercentilesPrediction percentiles;

    Tucuxi::Core::PercentileRanks percentileRanks;
    Tucuxi::Core::OmegaMatrix omega;
    Tucuxi::Core::ComputingAborter* aborter = _traits->getAborter();

    percentileRanks = _traits->getRanks();


    ResidualErrorModelExtractor errorModelExtractor;
    std::unique_ptr<IResidualErrorModel> residualErrorModel;
    ComputingStatus errorModelExtractionResult = errorModelExtractor.extract(
            _request.getDrugModel().getAnalyteSet()->getAnalytes()[0]->getResidualErrorModel(),
            _request.getDrugModel().getAnalyteSet()->getAnalytes()[0]->getUnit(),
            _request.getDrugModel().getAnalyteSet()->getAnalytes()[0]->getUnit(),
            covariateSeries,
            residualErrorModel);
    if (errorModelExtractionResult != ComputingStatus::Ok) {
        return errorModelExtractionResult;
    }

    std::vector<const FullFormulationAndRoute*> fullFormulationAndRoutes =
            m_utils->m_generalExtractor->extractFormulationAndRoutes(
                    _request.getDrugModel(), intakeSeries[analyteGroupId]);

    ComputingStatus omegaComputingResult = m_utils->m_generalExtractor->extractOmega(
            _request.getDrugModel(), analyteGroupId, fullFormulationAndRoutes, omega);
    if (omegaComputingResult != ComputingStatus::Ok) {
        return omegaComputingResult;
    }


    // Set initial etas to 0 for all variable parameters
    Tucuxi::Core::Etas etas(static_cast<size_t>(omega.cols()), 0.0);

    Tucuxi::Core::ComputingStatus computingResult;

    Tucuxi::Core::ConcentrationCalculator concentrationCalculator;

    if (_traits->getComputingOption().getParametersType() == PredictionParameterType::Aposteriori) {
        ComputingStatus aposterioriEtasExtractionResult = m_utils->m_generalExtractor->extractAposterioriEtas(
                etas,
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

        if (_request.getDrugTreatment().getSamples().empty()) {
            return ComputingStatus::AposterioriPercentilesNoSamplesError;
        }

        // This extraction is already done in extractAposterioriEtas... Could be optimized
        SampleSeries sampleSeries;
        SampleExtractor sampleExtractor;
        // We take all samples
        ComputingStatus sampleExtractionResult = sampleExtractor.extract(
                _request.getDrugTreatment().getSamples(),
                _request.getDrugModel().getAnalyteSet(analyteGroupId),
                DateTime::min(),
                DateTime::max(),
                _request.getDrugModel().getAnalyteSet(analyteGroupId)->getConcentrationUnit(),
                sampleSeries);
        //sampleExtractor.extract(_request.getDrugTreatment().getSamples(), _request.getDrugModel().getAnalyteSet(analyteGroupId), calculationStartTime, _traits->getEnd(), _request.getDrugModel().getAnalyteSet(analyteGroupId)->getConcentrationUnit(), sampleSeries);

        if (sampleExtractionResult != ComputingStatus::Ok) {
            return sampleExtractionResult;
        }

        if (sampleSeries.empty()) {
            return ComputingStatus::AposterioriPercentilesOutOfScopeSamplesError;
        }

        std::unique_ptr<Tucuxi::Core::IAposterioriPercentileCalculator> calculator =
                std::make_unique<Tucuxi::Core::AposterioriMonteCarloPercentileCalculator>();
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
        std::unique_ptr<Tucuxi::Core::IAprioriPercentileCalculator> calculator =
                std::make_unique<Tucuxi::Core::AprioriMonteCarloPercentileCalculator>();
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

    if (computingResult != ComputingStatus::Ok) {
        return computingResult;
    }

    // We use a single prediction to get back time offsets. Could be optimized
    // YTA: I do not think this is relevant. It could be deleted I guess
    ConcentrationPredictionPtr pPrediction = std::make_unique<ConcentrationPrediction>();

    ComputingStatus predictionResult = concentrationCalculator.computeConcentrations(
            pPrediction,
            false,
            _traits->getStart(),
            _traits->getEnd(),
            intakeSeries[analyteGroupId],
            parameterSeries[analyteGroupId]);

    if (predictionResult != ComputingStatus::Ok) {
        return predictionResult;
    }

    return preparePercentilesResponse(
            _traits, _request, _response, intakeSeries, pPrediction, percentiles, percentileRanks);
}

ComputingStatus ComputingComponent::preparePercentilesResponse(
        const ComputingTraitPercentiles* _traits,
        const ComputingRequest& _request,
        std::unique_ptr<ComputingResponse>& _response,
        GroupsIntakeSeries& _intakeSeries,
        const ConcentrationPredictionPtr& _pPrediction,
        const Tucuxi::Core::PercentilesPrediction& _percentiles,
        const Tucuxi::Core::PercentileRanks& _percentileRanks)
{

    auto defaultAnalyteGroupId = _request.getDrugModel().getAnalyteSets()[0]->getId();
    IntakeSeries selectedIntakes;
    selectRecordedIntakes(
            selectedIntakes, _intakeSeries[defaultAnalyteGroupId], _traits->getStart(), _traits->getEnd());

    if (selectedIntakes.size() == _pPrediction->getTimes().size()
        && selectedIntakes.size() == _pPrediction->getValues().size()) {
        std::unique_ptr<PercentilesData> resp = std::make_unique<PercentilesData>(_request.getId());
        resp->setNbPointsPerHour(_traits->getNbPointsPerHour());

        const std::vector<std::vector<std::vector<Value> > >& allValues = _percentiles.getValues();


        // The final unit depends on the computing options
        TucuUnit finalUnit("ug/l");
        if (_traits->getComputingOption().forceUgPerLiter() == ForceUgPerLiterOption::DoNotForce) {
            finalUnit = _request.getDrugModel().getActiveMoieties()[0]->getUnit();
        }

        for (unsigned int p = 0; p < _percentiles.getRanks().size(); p++) {
            std::vector<CycleData> percData;

            if (selectedIntakes.size() != allValues[p].size()) {
                return ComputingStatus::SelectedIntakesSizeError;
            }
            if (selectedIntakes.size() != _pPrediction->getTimes().size()) {
                return ComputingStatus::SelectedIntakesSizeError;
            }

            for (unsigned int cycle = 0; cycle < allValues[p].size(); cycle++) {

                TimeOffsets times = _pPrediction->getTimes()[cycle];
                DateTime start = selectedIntakes[cycle].getEventTime();

                std::chrono::milliseconds ms = std::chrono::milliseconds(static_cast<int>(times.back())) * 3600 * 1000;
                Duration ds(ms);
                DateTime end = start + ds;

                if (end > _traits->getStart()) {
                    CycleData cycleData(start, end, finalUnit);
                    //                    cycleData.addData(times, allValues[p][cycle]);
                    cycleData.addData(
                            times,
                            Tucuxi::Common::UnitManager::convertToUnit<
                                    Tucuxi::Common::UnitManager::UnitType::Concentration>(
                                    allValues[p][cycle],
                                    _request.getDrugModel().getActiveMoieties()[0]->getUnit(),
                                    finalUnit));
                    percData.push_back(cycleData);
                }
            }

            if (_traits->getComputingOption().retrieveStatistics() == RetrieveStatisticsOption::RetrieveStatistics) {
                CycleStatisticsCalculator c;
                c.calculate(percData);
            }

            resp->addPercentileData(percData);
        }
        resp->setRanks(_percentileRanks);

        _response->addResponse(std::move(resp));
        return ComputingStatus::Ok;
    }
    return ComputingStatus::SelectedIntakesSizeError;
}


ComputingStatus ComputingComponent::compute(
        const ComputingTraitAdjustment* _traits,
        const ComputingRequest& _request,
        std::unique_ptr<ComputingResponse>& _response)
{

    if (_traits == nullptr) {
        m_logger.error("The computing traits sent for computation are nullptr");
        return ComputingStatus::NoComputingTraits;
    }
    ComputingAdjustments computer(m_utils.get());
    return computer.compute(_traits, _request, _response);
}



ComputingStatus ComputingComponent::compute(
        const ComputingTraitAtMeasures* _traits,
        const ComputingRequest& _request,
        std::unique_ptr<ComputingResponse>& _response)
{

    // Simply extract the sample dates
    std::vector<Tucuxi::Common::DateTime> sampleTimes;
    for (const auto& sample : _request.getDrugTreatment().getSamples()) {
        sampleTimes.push_back(sample->getDate());
    }

    // Create the corresponding object for single points calculation
    ComputingTraitSinglePoints traits(_request.getId(), sampleTimes, _traits->getComputingOption());

    // And start the calculation
    return compute(&traits, _request, _response);
}

ComputingStatus ComputingComponent::compute(
        const ComputingTraitSinglePoints* _traits,
        const ComputingRequest& _request,
        std::unique_ptr<ComputingResponse>& _response)
{

    if (_traits == nullptr) {
        m_logger.error("The computing traits sent for computation are nullptr");
        return ComputingStatus::NoComputingTraits;
    }

    if (_traits->getTimes().empty()) {
        // No time given, so we return an empty response
        std::unique_ptr<SinglePointsData> resp = std::make_unique<SinglePointsData>(_request.getId());
        _response->addResponse(std::move(resp));
        return ComputingStatus::Ok;
    }

    std::map<AnalyteGroupId, std::shared_ptr<PkModel> > pkModel;

    GroupsIntakeSeries intakeSeries;
    CovariateSeries covariateSeries;
    GroupsParameterSetSeries parameterSeries;
    DateTime calculationStartTime;

    DateTime firstDate;
    DateTime lastDate;

    for (const auto& singleTime : _traits->getTimes()) {
        if (firstDate.isUndefined() || (singleTime < firstDate)) {
            firstDate = singleTime;
        }
        if (lastDate.isUndefined() || (singleTime > lastDate)) {
            lastDate = singleTime;
        }
    }

    lastDate = lastDate + Duration(std::chrono::hours(1));

    // TODO : Check if 3 is necessary or not
    ComputingTraitStandard standardTraits(_request.getId(), firstDate, lastDate, 3, _traits->getComputingOption());

    ComputingStatus extractionResult = m_utils->m_generalExtractor->generalExtractions(
            &standardTraits,
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

    if (_request.getDrugModel().getAnalyteSets().empty()) {
        return ComputingStatus::NoAnalytesGroup;
    }

    for (const auto& analyteGroup : _request.getDrugModel().getAnalyteSets()) {
        AnalyteGroupId analyteGroupId = analyteGroup->getId();

        // Now ready to do the real computing with all the extracted values

        ConcentrationPredictionPtr pPrediction = std::make_unique<ConcentrationPrediction>();

        ComputingStatus computingResult;

        Etas etas;

        if (_traits->getComputingOption().getParametersType() == PredictionParameterType::Aposteriori) {
            ComputingStatus aposterioriEtasExtractionResult = m_utils->m_generalExtractor->extractAposterioriEtas(
                    etas,
                    _request,
                    analyteGroupId,
                    intakeSeries[analyteGroupId],
                    parameterSeries[analyteGroupId],
                    covariateSeries,
                    calculationStartTime,
                    lastDate);
            if (aposterioriEtasExtractionResult != ComputingStatus::Ok) {
                return aposterioriEtasExtractionResult;
            }
        }

        Concentrations concentrations;

        SampleSeries timesSeries;

        for (const auto& times : _traits->getTimes()) {
            timesSeries.push_back(SampleEvent(times));
        }

        // Sort the times in chronological order
        std::sort(timesSeries.begin(), timesSeries.end(), [](const SampleEvent& _a, const SampleEvent& _b) {
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


        if (computingResult == ComputingStatus::Ok) {
            std::unique_ptr<SinglePointsData> resp = std::make_unique<SinglePointsData>(_request.getId());

            if (concentrations.size() != timesSeries.size()) {
                // Something went wrong
                m_logger.error("The number of concentrations calculated is not the same as the number of points asked");
                return ComputingStatus::ConcentrationSizeError;
            }

            size_t nbTimes = concentrations.size();
            Concentrations c;
            for (size_t i = 0; i < nbTimes; i++) {
                c.push_back(concentrations[i]);
                resp->m_times.push_back(timesSeries[i].getEventTime());
            }
            resp->m_concentrations.push_back(c);
            resp->m_unit = TucuUnit("ug/l");

            _response->addResponse(std::move(resp));
        }
        else {
            return computingResult;
        }
    }
    return ComputingStatus::Ok;
}


} // namespace Core
} // namespace Tucuxi
