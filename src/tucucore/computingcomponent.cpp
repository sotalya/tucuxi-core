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

namespace Tucuxi {
namespace Core {

Tucuxi::Common::Interface* ComputingComponent::createComponent()
{
    ComputingComponent *cmp = new ComputingComponent();

    cmp->initialize();

    return dynamic_cast<IComputingService*>(cmp);
}


ComputingComponent::ComputingComponent() : m_models(nullptr), m_generalExtractor(nullptr)
{
    registerInterface(dynamic_cast<IComputingService*>(this));
}


ComputingComponent::~ComputingComponent()
{
    if (m_models != nullptr) {
        delete m_models;
    }
    if (m_generalExtractor != nullptr) {
        delete m_generalExtractor;
    }
}

bool ComputingComponent::initialize()
{
    m_models = new PkModelCollection();
    if (!defaultPopulate(*m_models)) {
        m_logger.error("Could not populate the Pk models collection. No model will be available");
        return false;
    }
    m_generalExtractor = new GeneralExtractor();
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



ComputingResult ComputingComponent::compute(
        const ComputingTraitConcentration *_traits,
        const ComputingRequest &_request,
        std::unique_ptr<ComputingResponse> &_response)
{

    if (_traits == nullptr) {
        m_logger.error("The computing traits sent for computation are nullptr");
        return ComputingResult::Error;
    }

    std::map<AnalyteGroupId, std::shared_ptr<PkModel> > pkModel;

    GroupsIntakeSeries intakeSeries;
    CovariateSeries covariateSeries;
    GroupsParameterSetSeries parameterSeries;
    DateTime calculationStartTime;

    ComputingResult extractionResult = m_generalExtractor->generalExtractions(_traits,
                                                                           _request,
                                                                           m_models,
                                                                           pkModel,
                                                                           intakeSeries,
                                                                           covariateSeries,
                                                                           parameterSeries,
                                                                           calculationStartTime);

    if (extractionResult != ComputingResult::Success) {
        return extractionResult;
    }

    // Now ready to do the real computing with all the extracted values

    std::vector<ConcentrationPredictionPtr> analytesPredictions;

    ComputationResult computationResult;

    std::map<AnalyteGroupId, Etas> allEtas;

    for(const auto &analyteGroup : _request.getDrugModel().getAnalyteSets()) {
        AnalyteGroupId analyteGroupId = analyteGroup->getId();

        ConcentrationPredictionPtr pPrediction = std::make_unique<ConcentrationPrediction>();

        Etas etas;

        if (_traits->getComputingOption().getParametersType() == PredictionParameterType::Aposteriori) {
            if (m_generalExtractor->extractAposterioriEtas(etas, _request, analyteGroupId, intakeSeries[analyteGroupId], parameterSeries[analyteGroupId], covariateSeries, calculationStartTime, _traits->getEnd())
                    != ComputingResult::Success) {
                return ComputingResult::Error;
            }
        }

        allEtas[analyteGroupId] = etas;

        computationResult = computeConcentrations(
                    pPrediction,
                    false,
                     _traits->getStart(),
    //                   calculationStartTime,
                    _traits->getEnd(),
                    intakeSeries[analyteGroupId],
                    parameterSeries[analyteGroupId],
                    etas);


        if (computationResult == ComputationResult::Success)
        {
            analytesPredictions.push_back(std::move(pPrediction));
        }
        else {
            return ComputingResult::Error;
        }
    }
    std::vector<ConcentrationPredictionPtr> activeMoietiesPredictions;

    if (!_request.getDrugModel().isSingleAnalyte()) {

    for (const auto & activeMoiety : _request.getDrugModel().getActiveMoieties()) {
        ConcentrationPredictionPtr activeMoietyPrediction = std::make_unique<ConcentrationPrediction>();
        ComputingResult activeMoietyComputationResult = computeActiveMoiety(_request.getDrugModel(), activeMoiety.get(), analytesPredictions, activeMoietyPrediction);
        if (activeMoietyComputationResult != ComputingResult::Success) {
            return ComputingResult::Error;
        }
        activeMoietiesPredictions.push_back(std::move(activeMoietyPrediction));
    }
    }

    std::unique_ptr<SinglePredictionResponse> resp = std::make_unique<SinglePredictionResponse>(_traits->getId());

    IntakeSeries recordedIntakes;
    selectRecordedIntakes(recordedIntakes, intakeSeries[_request.getDrugModel().getAnalyteSets()[0]->getId()], _traits->getStart(), _traits->getEnd());

    if ((recordedIntakes.size() != analytesPredictions[0]->getTimes().size()) ||
            (recordedIntakes.size() != analytesPredictions[0]->getValues().size())) {
        return ComputingResult::Error;
    }


    ConcentrationPredictionPtr pPrediction = std::make_unique<ConcentrationPrediction>();

    // std::cout << "Start Time : " << _traits->getStart() << std::endl;
    for (size_t i = 0; i < recordedIntakes.size(); i++) {

        TimeOffsets times = analytesPredictions[0]->getTimes().at(i);
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

            int index = 0;
            for(const auto &analyteGroup : _request.getDrugModel().getAnalyteSets()) {
                AnalyteGroupId analyteGroupId = analyteGroup->getId();

                cycle.addData(times, analytesPredictions[index]->getValues().at(i), 0);
                index ++;
            }

            if (!_request.getDrugModel().isSingleAnalyte()) {
                for (const auto &activeMoiety : activeMoietiesPredictions) {
                    cycle.addData(times, activeMoiety->getValues().at(i), 0);
                }
            }

            AnalyteGroupId analyteGroupId = _request.getDrugModel().getAnalyteSets()[0]->getId();
            ParameterSetEventPtr params = parameterSeries[analyteGroupId].getAtTime(start, allEtas[analyteGroupId]);

            for (auto p = params.get()->begin() ; p < params.get()->end() ; p++) {
                cycle.m_parameters.push_back({(*p).getParameterId(), (*p).getValue()});
            }

            std::sort(cycle.m_parameters.begin(), cycle.m_parameters.end(),
                      [&] (const ParameterValue &_v1, const ParameterValue &_v2) { return _v1.m_parameterId < _v2.m_parameterId; });

            resp->addCycleData(cycle);
        }
    }

    for(const auto &analyteGroup : _request.getDrugModel().getAnalyteSets()) {
        AnalyteGroupId analyteGroupId = analyteGroup->getId();
        resp->addAnalyteId(analyteGroupId.toString());
    }

    if (!_request.getDrugModel().isSingleAnalyte()) {
        for (const auto & activeMoiety : _request.getDrugModel().getActiveMoieties()) {
            resp->addAnalyteId(activeMoiety->getActiveMoietyId().toString());
        }
    }

    if (_traits->getComputingOption().getWithStatistics()) {
        CycleStatisticsCalculator c;
        c.calculate(resp->getModifiableData());
    }

    _response->addResponse(std::move(resp));
    return ComputingResult::Success;
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

    std::map<AnalyteGroupId, std::shared_ptr<PkModel> > pkModel;

    GroupsIntakeSeries intakeSeries;
    CovariateSeries covariateSeries;
    GroupsParameterSetSeries parameterSeries;
    DateTime calculationStartTime;

    ComputingResult extractionResult = m_generalExtractor->generalExtractions(_traits,
                                                                              _request,
                                                                              m_models,
                                                                              pkModel,
                                                                              intakeSeries,
                                                                              covariateSeries,
                                                                              parameterSeries,
                                                                              calculationStartTime);

    if (extractionResult != ComputingResult::Success) {
        return extractionResult;
    }

    // Now ready to do the real computing with all the extracted values



    // TODO : Change this analyte group
    AnalyteGroupId analyteGroupId = _request.getDrugModel().getAnalyteSets().at(0)->getId();


    Tucuxi::Core::PercentilesPrediction percentiles;

    Tucuxi::Core::PercentileRanks percentileRanks;
    Tucuxi::Core::OmegaMatrix omega;
    Tucuxi::Core::ComputingAborter *aborter = _traits->getAborter();

    percentileRanks = _traits->getRanks();


    ResidualErrorModelExtractor errorModelExtractor;
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


    std::unique_ptr<IResidualErrorModel> residualErrorModel = std::unique_ptr<IResidualErrorModel>(residual);

    std::vector<const FullFormulationAndRoute *> fullFormulationAndRoutes = m_generalExtractor->extractFormulationAndRoutes(_request.getDrugModel(), intakeSeries[analyteGroupId]);

    ComputationResult omegaComputationResult = m_generalExtractor->extractOmega(_request.getDrugModel(), analyteGroupId, fullFormulationAndRoutes, omega);
    if (omegaComputationResult != ComputationResult::Success) {
        return ComputingResult::Error;
    }


    // Set initial etas to 0 for all variable parameters
    Tucuxi::Core::Etas etas(omega.cols(), 0.0);

    Tucuxi::Core::IPercentileCalculator::ComputingResult computationResult;

    Tucuxi::Core::ConcentrationCalculator concentrationCalculator;

    if (_traits->getComputingOption().getParametersType() == PredictionParameterType::Aposteriori)   {

        if (m_generalExtractor->extractAposterioriEtas(etas, _request, analyteGroupId, intakeSeries[analyteGroupId], parameterSeries[analyteGroupId], covariateSeries, calculationStartTime, _traits->getEnd())
                != ComputingResult::Success) {
            return ComputingResult::Error;
        }

        // This extraction is already done in extractAposterioriEtas... Could be optimized
        SampleSeries sampleSeries;
        SampleExtractor sampleExtractor;
        SampleExtractor::Result sampleExtractionResult =
        sampleExtractor.extract(_request.getDrugTreatment().getSamples(), calculationStartTime, _traits->getEnd(), sampleSeries);

        if (sampleExtractionResult != SampleExtractor::Result::Ok) {
            return ComputingResult::Error;
        }

        std::unique_ptr<Tucuxi::Core::IAposterioriPercentileCalculator> calculator(new Tucuxi::Core::AposterioriMonteCarloPercentileCalculator());
        computationResult = calculator->calculate(
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
        computationResult = calculator->calculate(
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

    ComputationResult predictionComputationResult = computeConcentrations(
                pPrediction,
                false,
                _traits->getStart(),
                _traits->getEnd(),
                intakeSeries[analyteGroupId],
                parameterSeries[analyteGroupId]);

    if (predictionComputationResult != ComputationResult::Success) {
        return ComputingResult::Error;
    }

    IntakeSeries selectedIntakes;
    selectRecordedIntakes(selectedIntakes, intakeSeries[analyteGroupId], _traits->getStart(), _traits->getEnd());

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

            if (_traits->getComputingOption().getWithStatistics()) {
                CycleStatisticsCalculator c;
                c.calculate(percData);
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
        return ComputingResult::Error;
    }

    if (intervalValues.size() == 0) {
        m_logger.error("No available interval");
        return ComputingResult::Error;
    }

    if (infusionTimes.size() == 0) {
        if (_formulationAndRoute->getFormulationAndRoute().getAbsorptionModel() == AbsorptionModel::Infusion) {
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

bool compareCandidates(const FullDosage &_a, const FullDosage &_b)
{
    return _a.getGlobalScore() < _b.getGlobalScore();
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

    std::map<AnalyteGroupId, std::shared_ptr<PkModel> > pkModel;

    GroupsIntakeSeries intakeSeries;
    CovariateSeries covariateSeries;
    GroupsParameterSetSeries parameterSeries;
    DateTime calculationStartTime;

    // Be carefull here, as the endTime could be different...
    ComputingResult extractionResult = m_generalExtractor->generalExtractions(_traits,
                                                                              _request,
                                                                              m_models,
                                                                              pkModel,
                                                                              intakeSeries,
                                                                              covariateSeries,
                                                                              parameterSeries,
                                                                              calculationStartTime);

    if (extractionResult != ComputingResult::Success) {
        return extractionResult;
    }


    std::map<AnalyteGroupId, Etas> etas;

    std::vector<AnalyteGroupId> allGroupIds;
    for(const auto &analyteGroup : _request.getDrugModel().getAnalyteSets()) {
        allGroupIds.push_back(analyteGroup->getId());
    }

    for(auto analyteGroupId : allGroupIds) {
        if (_traits->getComputingOption().getParametersType() == PredictionParameterType::Aposteriori) {
            if (m_generalExtractor->extractAposterioriEtas(etas[analyteGroupId], _request, analyteGroupId, intakeSeries[analyteGroupId], parameterSeries[analyteGroupId], covariateSeries, calculationStartTime, _traits->getEnd())
                    != ComputingResult::Success) {
                return ComputingResult::Error;
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
                Duration newDuration = m_generalExtractor->secureStartDuration(halfLife);

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

            GroupsIntakeSeries intakeSeriesPerGroup;

            // TODO : There should be one object per analyte
            ConcentrationPredictionPtr pPrediction = std::make_unique<ConcentrationPrediction>();

            for (auto analyteGroupId : allGroupIds) {

                IntakeExtractor intakeExtractor;
                double nbPointsPerHour = _traits->getNbPointsPerHour();
                IntakeExtractor::Result intakeExtractionResult = intakeExtractor.extract(*newHistory, calculationStartTime, newEndTime,
                                                                                         nbPointsPerHour, intakeSeriesPerGroup[analyteGroupId]);

                delete newHistory;
                newHistory = nullptr;

                if (intakeExtractionResult != IntakeExtractor::Result::Ok) {
                    return ComputingResult::Error;
                }

                IntakeToCalculatorAssociator::Result intakeAssociationResult =
                        IntakeToCalculatorAssociator::associate(intakeSeriesPerGroup[analyteGroupId], *pkModel[analyteGroupId]);

                if (intakeAssociationResult != IntakeToCalculatorAssociator::Result::Ok) {
                    m_logger.error("Can not associate intake calculators for the specified route");
                    return ComputingResult::Error;
                }



                ComputationResult predictionComputationResult;

                predictionComputationResult = computeConcentrations(
                            pPrediction,
                            false,
                            //                            _traits->getStart(),
                            calculationStartTime,
                            newEndTime,
                            intakeSeriesPerGroup[analyteGroupId],
                            parameterSeries[analyteGroupId],
                            etas[analyteGroupId]);

                if (predictionComputationResult != ComputationResult::Success) {
                    m_logger.error("Error with the computation of a single adjustment candidate");
                    return ComputingResult::Error;
                }
            }

            std::vector< TargetEvaluationResult> candidateResults;
            bool isValidCandidate = true;

            if (targetSeries.size() == 0) {
                isValidCandidate = false;
            }

            for(const auto & target : targetSeries) {
                TargetEvaluationResult localResult;

                // Now the score calculation
                TargetEvaluator::Result evaluationResult = targetEvaluator.evaluate(*pPrediction.get(), intakeSeriesPerGroup[allGroupIds[0]], target, localResult);

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


                for (auto analyteGroupId : allGroupIds) {
                    for (size_t i = 0; i < intakeSeriesPerGroup[analyteGroupId].size(); i++) {
                        TimeOffsets times = pPrediction->getTimes().at(i);
                        DateTime start = intakeSeriesPerGroup[analyteGroupId].at(i).getEventTime();
                        DateTime end = start + std::chrono::milliseconds(static_cast<int>(times.at(times.size() - 1)) * 1000);
                        if (start >= _traits->getAdjustmentTime()) {
                            CycleData cycle(start, end, Unit("ug/l"));
                            cycle.addData(times, pPrediction->getValues().at(i), 0);
                            dosage.m_data.push_back(cycle);
                        }
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

    ComputingResult extractionResult = m_generalExtractor->generalExtractions(&standardTraits,
                                                                              _request,
                                                                              m_models,
                                                                              pkModel,
                                                                              intakeSeries,
                                                                              covariateSeries,
                                                                              parameterSeries,
                                                                              calculationStartTime);

    if (extractionResult != ComputingResult::Success) {
        return extractionResult;
    }

    for (const auto & analyteGroup : _request.getDrugModel().getAnalyteSets()) {
        AnalyteGroupId analyteGroupId = analyteGroup->getId();

        // Now ready to do the real computing with all the extracted values

        ConcentrationPredictionPtr pPrediction = std::make_unique<ConcentrationPrediction>();

        ComputationResult computationResult;

        Etas etas;

        if (_traits->getComputingOption().getParametersType() == PredictionParameterType::Aposteriori) {
            if (m_generalExtractor->extractAposterioriEtas(etas, _request, analyteGroupId, intakeSeries[analyteGroupId], parameterSeries[analyteGroupId], covariateSeries, calculationStartTime, lastDate)
                    != ComputingResult::Success) {
                return ComputingResult::Error;
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
                    intakeSeries[analyteGroupId],
                    parameterSeries[analyteGroupId],
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
        const ParameterSetSeries &_parameters,
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
                _parameters,
                _etas,
                _residualErrorModel,
                _eps,
                _isFixedDensity);
}


ComputingResult ComputingComponent::computeActiveMoiety(
        const DrugModel &_drugModel,
        const ActiveMoiety *_activeMoiety,
        const std::vector<ConcentrationPredictionPtr> &_analytesPredictions,
        ConcentrationPredictionPtr &_activeMoietyPredictions)
{
    TMP_UNUSED_PARAMETER(_drugModel);

    Operation *op = _activeMoiety->getFormula();

    size_t fullSize = _analytesPredictions[0]->getValues().size();

    Concentrations concentration;
    for (size_t i = 0; i < fullSize; i++) {
        TimeOffsets times = _analytesPredictions[0]->getTimes()[i];
        Concentrations analyteC = _analytesPredictions[0]->getValues()[i];

        size_t nbConcentrations = analyteC.size();
        Concentrations concentration(nbConcentrations);

        if (op == nullptr) {
            for(size_t c = 0; c < nbConcentrations; c++) {
                concentration[c] = analyteC[c];
            }
        }
        else {

            for(size_t c = 0; c < nbConcentrations; c++) {
                OperationInputList inputList;
                inputList.push_back(OperationInput("input0", analyteC[c]));
                double result;
                if (!op->evaluate(inputList, result)) {
                    // Something went wrong
                    return ComputingResult::Error;
                }
                concentration[c] = result;
            }
        }
        _activeMoietyPredictions->appendConcentrations(times, concentration);
    }

    return ComputingResult::Success;
}

} // namespace Core
} // namespace Tucuxi
