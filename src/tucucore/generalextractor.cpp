#include "generalextractor.h"

#include "tucucore/computingservice/computingrequest.h"
#include "tucucore/computingservice/icomputingservice.h"
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

using namespace Tucuxi::Common;

namespace Tucuxi {
namespace Core {

GeneralExtractor::GeneralExtractor()
{

}

Duration GeneralExtractor::secureStartDuration(const HalfLife &_halfLife)
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

ComputingResult GeneralExtractor::extractAposterioriEtas(Etas &_etas, const ComputingRequest &_request, const IntakeSeries &_intakeSeries, const ParameterSetSeries &_parameterSeries, const CovariateSeries &_covariateSeries, DateTime _calculationStartTime, DateTime _endTime)
{
    Tucuxi::Core::OmegaMatrix omega;
    ComputationResult omegaComputationResult = extractOmega(_request.getDrugModel(), omega);
    if (omegaComputationResult != ComputationResult::Success) {
        return ComputingResult::Error;
    }

    SampleSeries sampleSeries;
    SampleExtractor sampleExtractor;
    SampleExtractor::Result sampleExtractionResult =
    sampleExtractor.extract(_request.getDrugTreatment().getSamples(), _calculationStartTime, _endTime, sampleSeries);

    if (sampleExtractionResult != SampleExtractor::Result::Ok) {
        return ComputingResult::Error;
    }

    if (sampleSeries.size() == 0) {
        // Surprising. Something maybe wrong with the sample extractor

    }
    else {
        ResidualErrorModelExtractor errorModelExtractor;
        IResidualErrorModel *residualErrorModel = nullptr;
        if (errorModelExtractor.extract(_request.getDrugModel().getAnalyteSet()->getAnalytes().at(0)->getResidualErrorModel(),
                                        _request.getDrugModel().getAnalyteSet()->getAnalytes().at(0)->getUnit(),
                                        _covariateSeries, &residualErrorModel)
                != ResidualErrorModelExtractor::Result::Ok) {
            return ComputingResult::Error;
        }

        APosterioriEtasCalculator etasCalculator;
        etasCalculator.computeAposterioriEtas(_intakeSeries, _parameterSeries, omega, *residualErrorModel, sampleSeries, _etas);

        delete residualErrorModel;
    }
    return ComputingResult::Success;
}

ComputationResult GeneralExtractor::extractOmega(
    const DrugModel &_drugModel,
    OmegaMatrix &_omega)
{

    // TODO : This should not necessarily be the default formulation and route
    // Should get rid of the next 3 lines
    const AnalyteGroupId analyteId = _drugModel.getAnalyteSet()->getId();
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


ComputingResult GeneralExtractor::generalExtractions(const ComputingTraitStandard *_traits,
        const ComputingRequest &_request,
        const PkModelCollection *_modelCollection,
        std::map<AnalyteGroupId, std::shared_ptr<PkModel> > &_pkModel,
        GroupsIntakeSeries &_intakeSeries,
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

    IntakeSeries intakeSeries;
    IntakeExtractor::Result intakeExtractionResult = intakeExtractor.extract(_request.getDrugTreatment().getDosageHistory(), fantomStart /*_traits->getStart()*/, _traits->getEnd() /* + Duration(24h)*/, nbPointsPerHour, intakeSeries);

    if (intakeExtractionResult != IntakeExtractor::Result::Ok) {
        m_logger.error("Error with the intakes extraction.");
        return ComputingResult::Error;
    }

    size_t nIntakes = intakeSeries.size();


    if (nIntakes > 0) {

        // Ensure that time ranges are correctly handled. We set again the interval based on the start of
        // next intake
        for (size_t i = 0; i < nIntakes - 1; i++) {
            Duration interval = intakeSeries.at(i+1).getEventTime() - intakeSeries.at(i).getEventTime();
            intakeSeries.at(i).setNbPoints(static_cast<int>(interval.toHours() * nbPointsPerHour) + 1);
            intakeSeries.at(i).setInterval(interval);
        }

        for (size_t i = 0; i < nIntakes; i++) {
            if (intakeSeries.at(i).getEventTime() + intakeSeries.at(i).getInterval() < _traits->getStart()) {
                intakeSeries[i].setNbPoints(2);
            }
        }

        const DosageTimeRangeList& timeRanges = _request.getDrugTreatment().getDosageHistory().getDosageTimeRanges();


        IntakeEvent *lastIntake = &(intakeSeries.at(nIntakes - 1));

        // If the treatement end is before the last point we want to get, then we add an empty dose to get points

//        if (_traits->getEnd() > timeRanges.at(timeRanges.size() - 1)->getEndDate()) {
            if (_traits->getEnd() > lastIntake->getEventTime() + lastIntake->getInterval()) {

            DateTime start = lastIntake->getEventTime() + lastIntake->getInterval();
            Value dose = 0.0;
            Duration interval = _traits->getEnd() - timeRanges.at(timeRanges.size() - 1)->getEndDate();
            auto absorptionModel = lastIntake->getRoute();

            Duration infusionTime;
            if (absorptionModel == AbsorptionModel::Infusion) {
                // We do this because the infusion calculators do not support infusionTime = 0
                infusionTime = Duration(std::chrono::hours(1));
            }
            int nbPoints = static_cast<int>(nbPointsPerHour * interval.toHours());

            IntakeEvent intake(start, Duration(), dose, interval, lastIntake->getFormulationAndRoute(), absorptionModel, infusionTime, nbPoints);
            intakeSeries.push_back(intake);
        }

    }

    OverloadEvaluator overloadEvaluator;
    if (!overloadEvaluator.isAcceptable(intakeSeries, _traits)) {
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

    std::vector<FormulationAndRoute> allFormulationAndRoutes;
    for (const auto & intake : intakeSeries) {
        FormulationAndRoute f = intake.getFormulationAndRoute();
        if (std::find(allFormulationAndRoutes.begin(), allFormulationAndRoutes.end(), f) == allFormulationAndRoutes.end()) {
            allFormulationAndRoutes.push_back(f);
        }
    }

    std::map<FormulationAndRoute, const FullFormulationAndRoute *> fullFormulationAndRoutes;

    // Only look for formulation and routes if there is an existing treatment
    if (intakeSeries.size() > 0) {
        if (!findFormulationAndRoutes(allFormulationAndRoutes, _request.getDrugModel().getFormulationAndRoutes(), fullFormulationAndRoutes)) {
            m_logger.error("Could not find a suitable formulation and route in the drug model");
            return ComputingResult::Error;
        }
    }

    // TODO : This should not necessarily be the default formulation and route
    // Should get rid of the next 4 lines
    const std::string pkModelId = _request.getDrugModel().getAnalyteSet()->getPkModelId();
    const AnalyteGroupId analyteGroupId = _request.getDrugModel().getAnalyteSet()->getId();
    const Formulation formulation = _request.getDrugModel().getFormulationAndRoutes().getDefault()->getFormulationAndRoute().getFormulation();
    const AdministrationRoute route = _request.getDrugModel().getFormulationAndRoutes().getDefault()->getFormulationAndRoute().getAdministrationRoute();

    for (const auto &analyteSet : _request.getDrugModel().getAnalyteSets()) {
        _pkModel[analyteSet->getId()] = _modelCollection->getPkModelFromId(analyteSet->getPkModelId());
        if (_pkModel[analyteSet->getId()] == nullptr) {
            m_logger.error("Can not find a Pk Model for the calculation");
            return ComputingResult::Error;
        }
    }

    for (const auto &analyteSet : _request.getDrugModel().getAnalyteSets()) {
        cloneIntakeSeries(intakeSeries, _intakeSeries[analyteSet->getId()]);

        IntakeToCalculatorAssociator::Result intakeAssociationResult = IntakeToCalculatorAssociator::associate(_intakeSeries[analyteSet->getId()], *_pkModel[analyteSet->getId()]);

        if (intakeAssociationResult != IntakeToCalculatorAssociator::Result::Ok) {
            m_logger.error("Can not associate intake calculators for the specified route");
            return ComputingResult::Error;
        }
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

    ParameterDefinitionIterator it = _request.getDrugModel().getParameterDefinitions(analyteGroupId, formulation, route);

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

bool GeneralExtractor::findFormulationAndRoutes(std::vector<FormulationAndRoute> &_treatmentFandR, const FormulationAndRoutes &_drugModelFandR, std::map<FormulationAndRoute, const FullFormulationAndRoute *> &_result)
{
    if (_treatmentFandR.size() > 1) {
        m_logger.error("The computing component does not support multi-formulation and routes treatments");
        return false;
    }
    for (const auto & f : _drugModelFandR) {
        if (f->getFormulationAndRoute().getAbsorptionModel() == _treatmentFandR[0].getAbsorptionModel()) {
            _result[_treatmentFandR[0]] = f.get();
            return true;
        }
    }

    m_logger.error("Could not find a formulation and route compatible with the treatment");
    return false;
}

} // namespace Core
} // namespace Tucuxi
