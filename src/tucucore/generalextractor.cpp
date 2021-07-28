#include "generalextractor.h"

#include "tucucore/computingservice/computingrequest.h"
#include "tucucore/computingservice/icomputingservice.h"
#include "tucucore/drugtreatment/sample.h"
#include "tucucore/sampleevent.h"
#include "tucucore/sampleextractor.h"
#include "tucucore/residualerrormodelextractor.h"
#include "tucucore/aposteriorietascalculator.h"
#include "tucucore/dosage.h"
#include "tucucore/parameter.h"
#include "tucucore/intakeextractor.h"
#include "tucucore/parametersextractor.h"
#include "tucucore/covariateextractor.h"
#include "tucucore/intaketocalculatorassociator.h"
#include "tucucore/overloadevaluator.h"
#include "tucucore/drugmodel/timeconsiderations.h"
#include "tucucore/drugdomainconstraintsevaluator.h"

using namespace Tucuxi::Common;

namespace Tucuxi {
namespace Core {

GeneralExtractor::GeneralExtractor()
= default;

Duration GeneralExtractor::secureStartDuration(const HalfLife &_halfLife)
{
    Duration duration;
    if (_halfLife.getUnit() == TucuUnit("d")) {
        duration = Duration(24h) * _halfLife.getValue() * _halfLife.getMultiplier();
    }
    else if (_halfLife.getUnit() == TucuUnit("h")) {
        duration = Duration(1h) * _halfLife.getValue() * _halfLife.getMultiplier();
    }
    else if (_halfLife.getUnit() == TucuUnit("min")) {
        duration = Duration(1min) * _halfLife.getValue() * _halfLife.getMultiplier();
    }
    else if (_halfLife.getUnit() == TucuUnit("s")) {
        duration = Duration(1s) * _halfLife.getValue() * _halfLife.getMultiplier();
    }
    return duration;
}

ComputingStatus GeneralExtractor::extractAposterioriEtas(
        Etas &_etas,
        const ComputingRequest &_request,
        const AnalyteGroupId& _analyteGroupId,
        const IntakeSeries &_intakeSeries,
        const ParameterSetSeries &_parameterSeries,
        const CovariateSeries &_covariateSeries,
        DateTime _calculationStartTime,
        DateTime _endTime)
{
    Tucuxi::Core::OmegaMatrix omega;

    std::vector<const FullFormulationAndRoute *> formulationAndRoutes = extractFormulationAndRoutes(_request.getDrugModel(), _intakeSeries);

    ComputingStatus omegaComputingResult = extractOmega(_request.getDrugModel(), _analyteGroupId, formulationAndRoutes, omega);
    if (omegaComputingResult != ComputingStatus::Ok) {
        return omegaComputingResult;
    }

    SampleSeries sampleSeries;
    SampleExtractor sampleExtractor;
    ComputingStatus sampleExtractionResult =
    sampleExtractor.extract(_request.getDrugTreatment().getSamples(), _request.getDrugModel().getAnalyteSet(_analyteGroupId),
                            _calculationStartTime,
                            _endTime,
                            _request.getDrugModel().getAnalyteSet(_analyteGroupId)->getConcentrationUnit(), sampleSeries);

    if (sampleExtractionResult != ComputingStatus::Ok) {
        return sampleExtractionResult;
    }

    if (sampleSeries.empty()) {
        // Surprising. Something maybe wrong with the sample extractor

    }
    else {
        ResidualErrorModelExtractor errorModelExtractor;
        std::unique_ptr<IResidualErrorModel> residualErrorModel;
        ComputingStatus errorModelExtractionResult = errorModelExtractor.extract(_request.getDrugModel().getAnalyteSet(_analyteGroupId)->getAnalytes()[0]->getResidualErrorModel(),
                _request.getDrugModel().getAnalyteSet(_analyteGroupId)->getAnalytes()[0]->getUnit(),
                _request.getDrugModel().getAnalyteSet(_analyteGroupId)->getAnalytes()[0]->getUnit(),
                _covariateSeries, residualErrorModel);
        if (errorModelExtractionResult != ComputingStatus::Ok) {
            return errorModelExtractionResult;
        }

        APosterioriEtasCalculator etasCalculator;
        auto status = etasCalculator.computeAposterioriEtas(_intakeSeries, _parameterSeries, omega, *residualErrorModel, sampleSeries, _etas);
        if (status != ComputingStatus::Ok) {
            return status;
        }

    }
    return ComputingStatus::Ok;
}

ComputingStatus GeneralExtractor::extractOmega(
        const DrugModel &_drugModel,
        const AnalyteGroupId& _analyteGroupId,
        std::vector<const FullFormulationAndRoute*> &_formulationAndRoutes,
        OmegaMatrix &_omega)
{
    ParameterDefinitionIterator it = _drugModel.getParameterDefinitions(_analyteGroupId, _formulationAndRoutes);

    int nbVariableParameters = 0;
    std::size_t nbEtas = 0;

    it.reset();
    while (!it.isDone()) {
        if ((*it)->getVariability().getType() != ParameterVariabilityType::None) {
            nbVariableParameters ++;
            nbEtas += (*it)->getVariability().getValues().size();
        }
        it.next();
    }

    _omega = Tucuxi::Core::OmegaMatrix(nbEtas, nbEtas);

    for(std::size_t x = 0; x < nbEtas; x++) {
        for(std::size_t y = 0; y < nbEtas; y++) {
            _omega(static_cast<Eigen::Index>(x),static_cast<Eigen::Index>(y)) = 0.0;
        }
    }

    std::map<std::string, int> paramMap;
    int etaNumber = 0;
    it.reset();
    while (!it.isDone()) {
        if ((*it)->getVariability().getType() != ParameterVariabilityType::None) {
            // const ParameterDefinition *p = (*it);
            // Value v = (*it)->getVariability().getValue() ;
            _omega(etaNumber, etaNumber) = (*it)->getVariability().getValue() * (*it)->getVariability().getValue();
            paramMap[(*it)->getId()] = etaNumber;
            etaNumber ++;
            if ((*it)->getVariability().getValues().size() > 1) {
                for (std::size_t i = 1; i < (*it)->getVariability().getValues().size(); i++) {
                    _omega(etaNumber, etaNumber) = (*it)->getVariability().getValues()[i] * (*it)->getVariability().getValues()[i];
                    etaNumber ++;
                }

            }
        }
        it.next();
    }

    const AnalyteSet *analyteSet = _drugModel.getAnalyteSet(_analyteGroupId);
    Correlations correlations = analyteSet->getDispositionParameters().getCorrelations();
    for(auto & correlation : correlations) {
        std::string p1 = correlation.getParamId1();
        std::string p2 = correlation.getParamId2();
        Value correlationValue = correlation.getValue();

        int index1 = paramMap[p1];
        int index2 = paramMap[p2];

        Value covariance = correlationValue * (std::sqrt(_omega(index1, index1) * _omega(index2, index2)));
        _omega(index1, index2) = covariance;
        _omega(index2, index1) = covariance;
    }

    return ComputingStatus::Ok;
}

std::vector<const FullFormulationAndRoute*> GeneralExtractor::extractFormulationAndRoutes(const DrugModel &_drugModel, const IntakeSeries &_intakeSeries)
{
    std::vector<const FullFormulationAndRoute*> result;
    std::vector<FormulationAndRoute> allFormulationAndRoutes;
    for (const auto & intake : _intakeSeries) {
        FormulationAndRoute f = intake.getFormulationAndRoute();
        if (std::find(allFormulationAndRoutes.begin(), allFormulationAndRoutes.end(), f) == allFormulationAndRoutes.end()) {
            allFormulationAndRoutes.push_back(f);
        }
    }
    result.reserve(allFormulationAndRoutes.size());
    for (const auto & f : allFormulationAndRoutes) {
        result.push_back(_drugModel.getFormulationAndRoutes().get(f));
    }
    return result;
}


ComputingStatus GeneralExtractor::generalExtractions(const ComputingTraitStandard *_traits,
                                                     const DrugModel &_drugModel,
                                                     const DosageHistory &_dosageHistory,
                                                     const Samples &_samples,
                                                     const PatientVariates &_patientVariates,
        const PkModelCollection *_modelCollection,
        std::map<AnalyteGroupId, std::shared_ptr<PkModel> > &_pkModel,
        GroupsIntakeSeries &_intakeSeries,
        CovariateSeries &_covariatesSeries,
        GroupsParameterSetSeries &_parameterSeries,
        DateTime &_calculationStartTime
        )
{


    const HalfLife &halfLife = _drugModel.getTimeConsiderations().getHalfLife();

    Duration fantomDuration = secureStartDuration(halfLife);

    Tucuxi::Common::DateTime firstEvent = _traits->getStart();

    if ((_traits->getComputingOption().getParametersType() == PredictionParameterType::Aposteriori)
            && (!_samples.empty())) {
        for (const auto &sample : _samples) {
            if (sample->getDate() < firstEvent) {
                firstEvent = sample->getDate();
            }
        }
    }

    Tucuxi::Common::DateTime fantomStart = firstEvent - fantomDuration;

    _calculationStartTime = fantomStart;

    IntakeExtractor intakeExtractor;
    double nbPointsPerHour = _traits->getNbPointsPerHour();

    IntakeSeries intakeSeries;
    TUCU_TRY {
        ComputingStatus intakeExtractionResult = intakeExtractor.extract(_dosageHistory, fantomStart /*_traits->getStart()*/,
                                                                         _traits->getEnd() /* + Duration(24h)*/,
                                                                         nbPointsPerHour,
                                                                         // TODO : This is a code smell. Does not work if 2 analyte sets do not share the same concentration unit
                                                                         _drugModel.getAnalyteSets()[0]->getDoseUnit(), intakeSeries);
        if (intakeExtractionResult != ComputingStatus::Ok) {
            m_logger.error("Error with the intakes extraction.");
            return intakeExtractionResult;
        }

    } TUCU_CATCH (std::runtime_error &e) TUCU_ONEXCEPTION({
        m_logger.error(e.what());
        return ComputingStatus::IntakeExtractionError;
    })


    size_t nIntakes = intakeSeries.size();


    if (nIntakes > 0) {

        // Ensure that time ranges are correctly handled. We set again the interval based on the start of
        // next intake
        for (size_t i = 0; i < nIntakes - 1; i++) {
            Duration interval = intakeSeries[i + 1].getEventTime() - intakeSeries[i].getEventTime();
            intakeSeries[i].setNbPoints(static_cast<CycleSize>(interval.toHours() * nbPointsPerHour) + 1);
            intakeSeries[i].setInterval(interval);
        }

        for (size_t i = 0; i < nIntakes; i++) {
            if (intakeSeries[i].getEventTime() + intakeSeries[i].getInterval() < _traits->getStart()) {
                intakeSeries[i].setNbPoints(2);
            }
        }

        // const DosageTimeRangeList& timeRanges = _drugTreatment.getDosageHistory().getDosageTimeRanges();


        IntakeEvent *lastIntake = &(intakeSeries.back());

        // If the treatement end is before the last point we want to get, then we add an empty dose to get points

//        if (_traits->getEnd() > timeRanges.back()->getEndDate()) {
        Duration interval = _traits->getEnd() - (lastIntake->getEventTime() + lastIntake->getInterval());
        if (interval > Duration(std::chrono::hours(0))) {

            DateTime start = lastIntake->getEventTime() + lastIntake->getInterval();
            Value dose = 0.0;
            TucuUnit doseValue("-");

            auto absorptionModel = lastIntake->getRoute();

            Duration infusionTime;
            if (absorptionModel == AbsorptionModel::Infusion) {
                // We do this because the infusion calculators do not support infusionTime = 0
                infusionTime = Duration(std::chrono::hours(1));
            }
            // We need at least one point. It could be less if the interval is very very small
            size_t nbPoints = std::max(size_t{1}, static_cast<size_t>(nbPointsPerHour * interval.toHours()));

            IntakeEvent intake(start, Duration(), dose, doseValue,  interval, lastIntake->getFormulationAndRoute(), absorptionModel, infusionTime, nbPoints);
            intakeSeries.push_back(intake);
        }

    }

    {
        IntakeSeries recordedIntakes;
        selectRecordedIntakes(recordedIntakes, intakeSeries, _traits->getStart(), _traits->getEnd());

        OverloadEvaluator *overloadEvaluator = SingleOverloadEvaluator::getInstance();
        if (!overloadEvaluator->isAcceptable(recordedIntakes, _traits)) {
            m_logger.error("Computation is too big. Aborting to avoid a computer crash. {}", overloadEvaluator->getErrorMessage());
            return ComputingStatus::TooBig;
        }
    }

    std::vector<FormulationAndRoute> allFormulationAndRoutes;
    for (const auto & intake : intakeSeries) {
        FormulationAndRoute f = intake.getFormulationAndRoute();
        if (std::find(allFormulationAndRoutes.begin(), allFormulationAndRoutes.end(), f) == allFormulationAndRoutes.end()) {
            allFormulationAndRoutes.push_back(f);
        }
    }

    std::map<FormulationAndRoute, const FullFormulationAndRoute *> fullFormulationAndRoutes;

    // Only look for formulation and routes if there is an existing treatment
    if (!intakeSeries.empty()) {
        if (!findFormulationAndRoutes(allFormulationAndRoutes, _drugModel.getFormulationAndRoutes(), fullFormulationAndRoutes)) {
            m_logger.error("Could not find a suitable formulation and route in the drug model");
            return ComputingStatus::CouldNotFindSuitableFormulationAndRoute;
        }
    }

    /*
    if (fullFormulationAndRoutes.size() > 1) {
        m_logger.error("The computing engine does not support multiple formulations and routes");
        return ComputingStatus::MultipleFormulationAndRoutesNotSupported;
    }
    */

    // std::map<AnalyteGroupId, AbsorptionModel > absorptionModels;

    // Extraction of the PK model for each analyte group
    auto pkModelExtractionResult = extractPkModel(_drugModel, _modelCollection, _pkModel);
    if (pkModelExtractionResult != ComputingStatus::Ok) {
        return pkModelExtractionResult;
    }


    for (const auto &analyteSet : _drugModel.getAnalyteSets()) {
        cloneIntakeSeries(intakeSeries, _intakeSeries[analyteSet->getId()]);

        auto status = convertAnalytes(_intakeSeries[analyteSet->getId()], _drugModel, analyteSet.get());
        if (status != ComputingStatus::Ok) {
            return status;
        }

        ComputingStatus intakeAssociationResult = IntakeToCalculatorAssociator::associate(_intakeSeries[analyteSet->getId()], *_pkModel[analyteSet->getId()]);

        if (intakeAssociationResult != ComputingStatus::Ok) {
            m_logger.error("Can not associate intake calculators for the specified route");
            return intakeAssociationResult;
        }
    }

    TUCU_TRY {
        if (_traits->getComputingOption().getParametersType() == PredictionParameterType::Population) {
            PatientVariates emptyPatientVariates;
            CovariateExtractor covariateExtractor(_drugModel.getCovariates(),
                                                  emptyPatientVariates,
                                                  fantomStart,
                                                  _traits->getEnd());
            ComputingStatus covariateExtractionResult = covariateExtractor.extract(_covariatesSeries);

            if (covariateExtractionResult != ComputingStatus::Ok) {
                m_logger.error("Can not extract covariates");
                return covariateExtractionResult;
            }
        }
        else {
            CovariateExtractor covariateExtractor(_drugModel.getCovariates(),
                                                  _patientVariates,
                                                  fantomStart,
                                                  _traits->getEnd());
            ComputingStatus covariateExtractionResult = covariateExtractor.extract(_covariatesSeries);

            if (covariateExtractionResult != ComputingStatus::Ok) {
                m_logger.error("Can not extract covariates");
                return covariateExtractionResult;        }
        }
    } TUCU_CATCH(std::runtime_error &e) TUCU_ONEXCEPTION({
        m_logger.error(e.what());
        return ComputingStatus::CovariateExtractionError;
    })

    auto formulationsAndRoutes = _dosageHistory.getFormulationAndRouteList();

    for (const auto &analyteSet : _drugModel.getAnalyteSets()) {
        const AnalyteGroupId analyteGroupId =analyteSet->getId();

        // If we are dealing with a treatment mixing different intakes
        ParameterDefinitionIterator itMix = _drugModel.getParameterDefinitions(analyteGroupId, formulationsAndRoutes);

        /*
        // Or a single type of intake

        Formulation formulation = Formulation::Undefined;
        AdministrationRoute route = AdministrationRoute::Undefined;
        if (!allFormulationAndRoutes.empty()) {
            formulation = allFormulationAndRoutes[0].getFormulation();
            route = allFormulationAndRoutes[0].getAdministrationRoute();
        }
        else {
            formulation = _drugModel.getFormulationAndRoutes().getDefault()->getFormulationAndRoute().getFormulation();
            route = _drugModel.getFormulationAndRoutes().getDefault()->getFormulationAndRoute().getAdministrationRoute();
        }

        ParameterDefinitionIterator itSingle = _drugModel.getParameterDefinitions(analyteGroupId, formulation, route);

        // Here we choose the Mix iterator or the single intake operator
        ParameterDefinitionIterator it = (formulationsAndRoutes.size() > 1) ? itMix : itSingle;
*/
        ParametersExtractor parameterExtractor(_covariatesSeries,
                                               itMix,
                                               fantomStart,
                                               _traits->getEnd());

        ComputingStatus parametersExtractionResult;


        if (_traits->getComputingOption().getParametersType() == PredictionParameterType::Population) {
#ifdef POPPARAMETERSFROMDEFAULTVALUES
            parametersExtractionResult = parameterExtractor.extractPopulation(_parameterSeries);
#else
            //parametersExtractionResult = parameterExtractor.extract(_parameterSeries);

            ParameterSetSeries intermediateParameterSeries;

            parametersExtractionResult = parameterExtractor.extract(intermediateParameterSeries);

            if (parametersExtractionResult != ComputingStatus::Ok) {
                m_logger.error("Can not extract parameters");
                return parametersExtractionResult;
            }

            // The intermediateParameterSeries contains changes of parameters, so we build a full set of parameter
            // for each event.
            parametersExtractionResult = parameterExtractor.buildFullSet(intermediateParameterSeries, _parameterSeries[analyteGroupId]);
            if (parametersExtractionResult != ComputingStatus::Ok) {
                m_logger.error("Can not consolidate parameters");
                return parametersExtractionResult;
            }

#endif // POPPARAMETERSFROMDEFAULTVALUES

            if (parametersExtractionResult != ComputingStatus::Ok) {
                m_logger.error("Can not extract parameters");
                return parametersExtractionResult;
            }

        }
        else {
            ParameterSetSeries intermediateParameterSeries;

            parametersExtractionResult = parameterExtractor.extract(intermediateParameterSeries);

            if (parametersExtractionResult != ComputingStatus::Ok) {
                m_logger.error("Can not extract parameters");
                return parametersExtractionResult;
            }

            // The intermediateParameterSeries contains changes of parameters, so we build a full set of parameter
            // for each event.
            parametersExtractionResult = parameterExtractor.buildFullSet(intermediateParameterSeries, _parameterSeries[analyteGroupId]);
            if (parametersExtractionResult != ComputingStatus::Ok) {
                m_logger.error("Can not consolidate parameters");
                return parametersExtractionResult;
            }

        }
    }


    return ComputingStatus::Ok;

}



ComputingStatus GeneralExtractor::generalExtractions(const ComputingTraitStandard *_traits,
        const ComputingRequest &_request,
        const PkModelCollection *_modelCollection,
        std::map<AnalyteGroupId, std::shared_ptr<PkModel> > &_pkModel,
        GroupsIntakeSeries &_intakeSeries,
        CovariateSeries &_covariatesSeries,
        GroupsParameterSetSeries &_parameterSeries,
        DateTime &_calculationStartTime
        )
{
    return generalExtractions(_traits,
                              _request.getDrugModel(),
                              _request.getDrugTreatment().getDosageHistory(),
                              _request.getDrugTreatment().getSamples(),
                              _request.getDrugTreatment().getCovariates(),
                              _modelCollection,
                              _pkModel,
                              _intakeSeries,
                              _covariatesSeries,
                              _parameterSeries,
                              _calculationStartTime);
}



ComputingStatus GeneralExtractor::extractPkModel(
        const DrugModel &_drugModel,
        const PkModelCollection *_modelCollection,
        std::map<AnalyteGroupId, std::shared_ptr<PkModel> > &_pkModel
        )
{

    for (const auto &analyteSet : _drugModel.getAnalyteSets()) {
        //if (!allFormulationAndRoutes.empty()) {
        //    const FullFormulationAndRoute *singleFormulationAndRoute = fullFormulationAndRoutes[allFormulationAndRoutes[0]];
        //    absorptionModels[analyteSet->getId()] = singleFormulationAndRoute->getAbsorptionModel(analyteSet->getId());
        //}

        _pkModel[analyteSet->getId()] = _modelCollection->getPkModelFromId(analyteSet->getPkModelId());
        if (_pkModel[analyteSet->getId()] == nullptr) {
            m_logger.error("Can not find a Pk Model for the calculation");
            return ComputingStatus::NoPkModelError;
        }
    }

    return ComputingStatus::Ok;
}


bool GeneralExtractor::findFormulationAndRoutes(std::vector<FormulationAndRoute> &_treatmentFandR, const FormulationAndRoutes &_drugModelFandR, std::map<FormulationAndRoute, const FullFormulationAndRoute *> &_result)
{
    /*
    if (_treatmentFandR.size() > 1) {
        m_logger.error("The computing component does not support multi-formulation and routes treatments");
        return false;
    }
    */
    for (const auto &fTreatment : _treatmentFandR) {
        bool found = false;
        for (const auto & f : _drugModelFandR) {
            if (f->getFormulationAndRoute().getAbsorptionModel() == fTreatment.getAbsorptionModel()) {
                _result[fTreatment] = f.get();
                found = true;
            }
        }
        if (!found) {
            m_logger.error("Could not find a formulation and route compatible with the treatment");
            return false;

        }
    }
    return true;
}

ComputingStatus GeneralExtractor::convertAnalytes(IntakeSeries &_intakeSeries, const Tucuxi::Core::DrugModel &_drugModel, const AnalyteSet *_analyteGroup)
{
    for (auto & intakeSerie : _intakeSeries) {
        const FullFormulationAndRoute *formulation = _drugModel.getFormulationAndRoutes().get(intakeSerie.getFormulationAndRoute());
        if (formulation == nullptr) {
            return ComputingStatus::AnalyteConversionError;
        }
        // TODO : Here we only support one analyte. To be modified once
        const AnalyteConversion *analyteConversion = formulation->getAnalyteConversion(_analyteGroup->getAnalytes()[0]->getAnalyteId());
        if (analyteConversion != nullptr) {
            double factor = analyteConversion->getFactor();
            DoseValue newDose = intakeSerie.getDose() * factor;
            intakeSerie.setDose(newDose);
        }
    }
    return ComputingStatus::Ok;
}


ComputingStatus GeneralExtractor::extractParameters(const std::vector<Tucuxi::Core::FormulationAndRoute> &_formulationsAndRoutes,
                                                    const AnalyteSets &_analyteSets,
                                                    const DrugModel &_drugModel,
                                                    const CovariateSeries &_covariatesSeries,
                                                    DateTime _start,
                                                    DateTime _end,
                                                    PredictionParameterType _parametersType,
                                                    GroupsParameterSetSeries &_parameterSeries)
{

    for (const auto &analyteSet : _analyteSets) {
        const AnalyteGroupId analyteGroupId =analyteSet->getId();

        // If we are dealing with a treatment mixing different intakes
        ParameterDefinitionIterator itMix = _drugModel.getParameterDefinitions(analyteGroupId, _formulationsAndRoutes);

        /*
    // Or a single type of intake

    Formulation formulation = Formulation::Undefined;
    AdministrationRoute route = AdministrationRoute::Undefined;
    if (!allFormulationAndRoutes.empty()) {
        formulation = allFormulationAndRoutes[0].getFormulation();
        route = allFormulationAndRoutes[0].getAdministrationRoute();
    }
    else {
        formulation = _request.getDrugModel().getFormulationAndRoutes().getDefault()->getFormulationAndRoute().getFormulation();
        route = _request.getDrugModel().getFormulationAndRoutes().getDefault()->getFormulationAndRoute().getAdministrationRoute();
    }

    ParameterDefinitionIterator itSingle = _request.getDrugModel().getParameterDefinitions(analyteGroupId, formulation, route);

    // Here we choose the Mix iterator or the single intake operator
    ParameterDefinitionIterator it = (_formulationsAndRoutes.size() > 1) ? itMix : itSingle;
*/
        ParametersExtractor parameterExtractor(_covariatesSeries,
                                               itMix,
                                               _start,
                                               _end);

        ComputingStatus parametersExtractionResult;


        if (_parametersType == PredictionParameterType::Population) {
#ifdef POPPARAMETERSFROMDEFAULTVALUES
            parametersExtractionResult = parameterExtractor.extractPopulation(_parameterSeries);
#else
            //parametersExtractionResult = parameterExtractor.extract(_parameterSeries);

            ParameterSetSeries intermediateParameterSeries;

            parametersExtractionResult = parameterExtractor.extract(intermediateParameterSeries);

            if (parametersExtractionResult != ComputingStatus::Ok) {
                m_logger.error("Can not extract parameters");
                return parametersExtractionResult;
            }

            // The intermediateParameterSeries contains changes of parameters, so we build a full set of parameter
            // for each event.
            parametersExtractionResult = parameterExtractor.buildFullSet(intermediateParameterSeries, _parameterSeries[analyteGroupId]);
            if (parametersExtractionResult != ComputingStatus::Ok) {
                m_logger.error("Can not consolidate parameters");
                return parametersExtractionResult;
            }

#endif // POPPARAMETERSFROMDEFAULTVALUES

            if (parametersExtractionResult != ComputingStatus::Ok) {
                m_logger.error("Can not extract parameters");
                return parametersExtractionResult;
            }

        }
        else {
            ParameterSetSeries intermediateParameterSeries;

            parametersExtractionResult = parameterExtractor.extract(intermediateParameterSeries);

            if (parametersExtractionResult != ComputingStatus::Ok) {
                m_logger.error("Can not extract parameters");
                return parametersExtractionResult;
            }

            // The intermediateParameterSeries contains changes of parameters, so we build a full set of parameter
            // for each event.
            parametersExtractionResult = parameterExtractor.buildFullSet(intermediateParameterSeries, _parameterSeries[analyteGroupId]);
            if (parametersExtractionResult != ComputingStatus::Ok) {
                m_logger.error("Can not consolidate parameters");
                return parametersExtractionResult;
            }

        }
    }
    return ComputingStatus::Ok;
}

} // namespace Core
} // namespace Tucuxi
