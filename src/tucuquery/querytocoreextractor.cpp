#include "querytocoreextractor.h"

#include "tucucore/drugtreatment/drugtreatment.h"
#include "tucucore/computingservice/computingtrait.h"
#include "tucucore/drugmodelimport.h"
#include "tucucore/drugmodelrepository.h"
#include "tucucore/treatmentdrugmodelcompatibilitychecker.h"

#include "tucucommon/componentmanager.h"
#include "tucucommon/loggerhelper.h"

namespace Tucuxi {
namespace Query {



QueryToCoreExtractor::QueryToCoreExtractor()
{

}


Tucuxi::Core::PatientVariates QueryToCoreExtractor::extractPatientVariates(const Query &_query) const
{
    const std::vector< std::unique_ptr<CovariateData> >& covariateData = _query.getpParameters()
            .getpPatient()
            .getCovariates();

    Tucuxi::Core::PatientVariates patientVariates;
    for (const std::unique_ptr<CovariateData>& covariate : covariateData) {
        patientVariates.push_back(
                    std::make_unique<Tucuxi::Core::PatientCovariate>(
                        covariate->getCovariateId(),
                        covariate->getValue(),
                        covariate->getDatatype(),
                        Core::Unit(covariate->getUnit()),
                        covariate->getpDate()
                        )
                    );
    }

    return patientVariates;
}

Tucuxi::Core::Targets QueryToCoreExtractor::extractTargets(const Query &_query, size_t _drugPosition) const
{
    Tucuxi::Core::Targets targets;

    const std::vector< std::unique_ptr<TargetData> >& targetsData = _query.getpParameters()
            .getDrugs().at(_drugPosition)
            ->getTargets();

    for (const std::unique_ptr<TargetData>& td : targetsData) {
        targets.push_back(
                    std::make_unique<Tucuxi::Core::Target>(
                        td->getActiveMoietyID(),
                        td->getTargetType(),
                        td->getUnit(),
                        td->getMin(),
                        td->getBest(),
                        td->getMax(),
                        td->getInefficacyAlarm(),
                        td->getToxicityAlarm()
                        )
                    );
    }

    return targets;
}


Tucuxi::Core::Samples QueryToCoreExtractor::extractSamples(const Query &_query, size_t _drugPosition) const
{
    Tucuxi::Core::Samples samples;

    const std::vector< std::unique_ptr<SampleData> >& samplesData = _query.getpParameters()
            .getDrugs().at(_drugPosition)
            ->getSamples();

    for (const std::unique_ptr<SampleData>& sd : samplesData) {
        for (const std::unique_ptr<ConcentrationData>& cd : sd->getConcentrations()) {
            samples.push_back(
                        std::make_unique<Tucuxi::Core::Sample>(
                            sd->getpSampleDate(),
                            cd->getAnalyteID(),
                            cd->getValue(),
                            Core::Unit(cd->getUnit())
                            )
                        );
        }
    }

    return samples;
}


Tucuxi::Core::DrugTreatment *QueryToCoreExtractor::extractDrugTreatment(const Query &_query) const
{
    Tucuxi::Core::DrugTreatment *drugTreatment = nullptr;

    drugTreatment = new Tucuxi::Core::DrugTreatment();

    // Getting the drug related to the request
    std::string drugID = _query.getRequests().at(0)->getDrugID();
    const std::vector< std::unique_ptr<DrugData> >& drugs = _query.getpParameters().getDrugs();
    size_t drugPosition = 0;
    for (; drugPosition < drugs.size(); ++drugPosition) {
        if (drugs.at(drugPosition)->getDrugID() == drugID) {
            break;
        }
    }

    if (drugPosition == drugs.size()) {
        Tucuxi::Common::LoggerHelper logHelper;
        logHelper.warn("Issue with the drug Id");
        return nullptr;
    }

    // Getting the dosage history for the drug treatment
    const Tucuxi::Core::DosageTimeRangeList& dosageTimeRangeList = drugs.at(0)
            ->getpTreatment()
            .getpDosageHistory()
            .getDosageTimeRanges();

    for (const auto& dosageTimeRange : dosageTimeRangeList) {
        //drugTreatment.addDosageTimeRange(make_unique<DosageTimeRange>(*dosageTimeRange));
        drugTreatment->getModifiableDosageHistory().addTimeRange(*dosageTimeRange);
    }

    // Getting the patient's covariates for the drug treatment
    Tucuxi::Core::PatientVariates patientVariates = extractPatientVariates(_query);
    for (auto& pc : patientVariates) {
        drugTreatment->addCovariate(move(pc));
    }

    // Getting the samples for the drug treatment
    Tucuxi::Core::Samples samples = extractSamples(_query, drugPosition);
    for (auto& sample : samples) {
        drugTreatment->addSample(move(sample));
    }

    // Getting the targets for the drug treatment
    Tucuxi::Core::Targets targets = extractTargets(_query, drugPosition);
    for (auto& target : targets) {
        drugTreatment->addTarget(move(target));
    }

    return drugTreatment;
}


Tucuxi::Core::ComputingTraits *QueryToCoreExtractor::extractComputingTraits(const Query &_query) const
{
    Tucuxi::Core::ComputingTraits *traits = nullptr;

    traits = new Tucuxi::Core::ComputingTraits();

    for (auto &request : _query.getRequests()) {
        std::string requestType = request->getRequestType();

        if (requestType == "prediction") {
            std::vector<Tucuxi::Core::ComputingTrait *> traitsList = extractPredictionTraits(_query, *request.get());
            for (auto trait : traitsList) {
                traits->addTrait(std::unique_ptr<Tucuxi::Core::ComputingTrait>(trait));
            }
        } else if (requestType == "predictionAtSampleTime") {
            std::vector<Tucuxi::Core::ComputingTrait *> traitsList = extractPredictionAtSampleTimeTraits(_query, *request.get());
            for (auto trait : traitsList) {
                traits->addTrait(std::unique_ptr<Tucuxi::Core::ComputingTrait>(trait));
            }
        } else if (requestType == "predictionAtTimes") {
            std::vector<Tucuxi::Core::ComputingTrait *> traitsList = extractPredictionAtTimesTraits(_query, *request.get());
            for (auto trait : traitsList) {
                traits->addTrait(std::unique_ptr<Tucuxi::Core::ComputingTrait>(trait));
            }
        } else if (requestType == "adjustment") {
            std::vector<Tucuxi::Core::ComputingTrait *> traitsList = extractAdaptationTraits(_query, *request.get());
            for (auto trait : traitsList) {
                traits->addTrait(std::unique_ptr<Tucuxi::Core::ComputingTrait>(trait));
            }
        } else if (requestType == "firstDosage") {
            std::vector<Tucuxi::Core::ComputingTrait *> traitsList = extractFirstDosageTraits(_query, *request.get());
            for (auto trait : traitsList) {
                traits->addTrait(std::unique_ptr<Tucuxi::Core::ComputingTrait>(trait));
            }
        } else {
        }


    }



    return traits;
}

Tucuxi::Core::PredictionParameterType QueryToCoreExtractor::extractPredictionParameterType(const Query &_query, const RequestData &_request) const
{
    std::string parametersType =_request.getParametersType();
    Tucuxi::Core::PredictionParameterType predictionParameterType = Tucuxi::Core::PredictionParameterType::Population;
    if (parametersType == "population") {
        predictionParameterType = Tucuxi::Core::PredictionParameterType::Population;
    } else if (parametersType == "apriori") {
        predictionParameterType = Tucuxi::Core::PredictionParameterType::Apriori;
    } else if (parametersType == "aposteriori") {
        predictionParameterType = Tucuxi::Core::PredictionParameterType::Aposteriori;
    }
    else if (parametersType == "best") {

        bool covariatesExist = _query.getpParameters().getpPatient().getCovariates().empty() ? false : true;
        bool samplesExist = _query.getpParameters().getDrugs().at(0)->getSamples().empty() ? false : true;

        if (covariatesExist && samplesExist) {
            predictionParameterType = Tucuxi::Core::PredictionParameterType::Aposteriori;
        } else if (covariatesExist && !samplesExist) {
            predictionParameterType = Tucuxi::Core::PredictionParameterType::Apriori;
        } else {
            predictionParameterType = Tucuxi::Core::PredictionParameterType::Population;
        }

    } else {
//        apiResponse->addWarning("UnexpetedPredicitionType", m_query.getLanguage(), "Unexpected prediction type!");
//        predictionParameterType = PredictionParameterType::Population;
    }
    return predictionParameterType;
}


std::vector<Tucuxi::Core::ComputingTrait *> QueryToCoreExtractor::extractFirstDosageTraits(const Query &_query, const RequestData &_request) const
{
    std::vector<Tucuxi::Core::ComputingTrait * > traits;

    TMP_UNUSED_PARAMETER(_query);
    TMP_UNUSED_PARAMETER(_request);

    return traits;
}


std::vector<Tucuxi::Core::ComputingTrait *> QueryToCoreExtractor::extractAdaptationTraits(const Query &_query, const RequestData &_request) const
{
    std::vector<Tucuxi::Core::ComputingTrait * > traits;

    Tucuxi::Core::PredictionParameterType predictionParameterType = extractPredictionParameterType(_query, _request);


    Tucuxi::Core::ComputingOption computingOption(predictionParameterType, Tucuxi::Core::CompartmentsOption::MainCompartment);

    int nbPointsPerHour = 10;
    if (_request.getNbPointsPerHour() > 0) {
        nbPointsPerHour = _request.getNbPointsPerHour();
    }


    Tucuxi::Core::BestCandidatesOption adjustmentOption(Tucuxi::Core::BestCandidatesOption::BestDosagePerInterval);
    Tucuxi::Core::LoadingOption loadingOption(Tucuxi::Core::LoadingOption::NoLoadingDose);
    Tucuxi::Core::RestPeriodOption restPeriodOption(Tucuxi::Core::RestPeriodOption::NoRestPeriod);
    Tucuxi::Core::SteadyStateTargetOption steadyStateTargetOption(Tucuxi::Core::SteadyStateTargetOption::AtSteadyState);
    Tucuxi::Core::TargetExtractionOption targetExtractionOption(Tucuxi::Core::TargetExtractionOption::DefinitionIfNoIndividualTarget);
    Tucuxi::Core::FormulationAndRouteSelectionOption formulationAndRouteSelectionOption(Tucuxi::Core::FormulationAndRouteSelectionOption::LastFormulationAndRoute);

    Tucuxi::Common::DateTime adjustmentTime;

    adjustmentTime = _request.getpDateInterval().getStart();

    Tucuxi::Core::ComputingTraitAdjustment * trait = new Tucuxi::Core::ComputingTraitAdjustment(
                _request.getRequestID(),
                _request.getpDateInterval().getStart(),
                _request.getpDateInterval().getEnd(),
                nbPointsPerHour,
                computingOption,
                adjustmentTime,
                adjustmentOption,
                loadingOption,
                restPeriodOption,
                steadyStateTargetOption,
                targetExtractionOption,
                formulationAndRouteSelectionOption);

    traits.push_back(trait);

    return traits;
}


std::vector<Tucuxi::Core::ComputingTrait * > QueryToCoreExtractor::extractPredictionTraits(const Query &_query, const RequestData &_request) const
{
    std::vector<Tucuxi::Core::ComputingTrait * > traits;


    Tucuxi::Core::PredictionParameterType predictionParameterType = extractPredictionParameterType(_query, _request);

    // Do it with statistics... Could come from a spec somewhere
    Tucuxi::Core::ComputingOption computingOption(predictionParameterType, Tucuxi::Core::CompartmentsOption::MainCompartment, true);

    int nbPointsPerHour = 10;
    if (_request.getNbPointsPerHour() > 0) {
        nbPointsPerHour = _request.getNbPointsPerHour();
    }

    Tucuxi::Core::ComputingTraitConcentration *trait = new Tucuxi::Core::ComputingTraitConcentration(
                _request.getRequestID(),
                _request.getpDateInterval().getStart(),
                _request.getpDateInterval().getEnd(),
                nbPointsPerHour,
                computingOption);

    traits.push_back(trait);

    if (_request.getPercentiles().size() != 0) {

        Tucuxi::Core::ComputingTraitPercentiles *trait = new Tucuxi::Core::ComputingTraitPercentiles(
                    _request.getRequestID() + "_p",
                    _request.getpDateInterval().getStart(),
                    _request.getpDateInterval().getEnd(),
                    _request.getPercentiles(),
                    nbPointsPerHour,
                    computingOption);

        traits.push_back(trait);
    }


    return traits;
}

std::vector<Tucuxi::Core::ComputingTrait * > QueryToCoreExtractor::extractPredictionAtSampleTimeTraits(const Query &_query, const RequestData &_request) const
{
    std::vector<Tucuxi::Core::ComputingTrait * > traits;


    Tucuxi::Core::PredictionParameterType predictionParameterType = extractPredictionParameterType(_query, _request);

    Tucuxi::Core::ComputingOption computingOption(predictionParameterType, Tucuxi::Core::CompartmentsOption::MainCompartment);

    std::vector<Tucuxi::Common::DateTime> times;

    std::string drugId = _request.getDrugID();

    DrugData *drugData = nullptr;

    for (const auto &drug : _query.getpParameters().getDrugs()) {
        if (drug->getDrugID() == drugId) {
            drugData = drug.get();
        }
    }

    if (drugData == nullptr) {
        // Error
    }

#if 0 // The following code could be useful for single points calculations
    for (const auto &sample : drugData->getSamples()) {
        times.push_back(sample->getpSampleDate());
    }

    Tucuxi::Core::ComputingTraitSinglePoints *trait = new Tucuxi::Core::ComputingTraitSinglePoints(
                _request.getRequestID(),
                times,
                computingOption);

    traits.push_back(trait);
#endif // 0

    Tucuxi::Core::ComputingTraitAtMeasures *trait = new Tucuxi::Core::ComputingTraitAtMeasures(
                _request.getRequestID(),
                computingOption);

    traits.push_back(trait);

    return traits;
}


std::vector<Tucuxi::Core::ComputingTrait * > QueryToCoreExtractor::extractPredictionAtTimesTraits(const Query &_query, const RequestData &_request) const
{
    std::vector<Tucuxi::Core::ComputingTrait * > traits;


    Tucuxi::Core::PredictionParameterType predictionParameterType = extractPredictionParameterType(_query, _request);

    Tucuxi::Core::ComputingOption computingOption(predictionParameterType, Tucuxi::Core::CompartmentsOption::MainCompartment);

    std::vector<Tucuxi::Common::DateTime> times;

    std::string drugId = _request.getDrugID();

    DrugData *drugData = nullptr;

    for (const auto &drug : _query.getpParameters().getDrugs()) {
        if (drug->getDrugID() == drugId) {
            drugData = drug.get();
        }
    }

    if (drugData == nullptr) {
        // Error
    }

    for (const auto &t : _request.getPointsInTime()) {
        times.push_back(t);
    }

    Tucuxi::Core::ComputingTraitSinglePoints *trait = new Tucuxi::Core::ComputingTraitSinglePoints(
                _request.getRequestID(),
                times,
                computingOption);

    traits.push_back(trait);

    return traits;
}


Tucuxi::Core::DrugModel *QueryToCoreExtractor::extractDrugModel(const Query &_query, const Tucuxi::Core::DrugTreatment *_drugTreatment) const
{
    Tucuxi::Core::DrugModel *drugModel = nullptr;

    // Getting the drug model
    Tucuxi::Core::IDrugModelRepository *drugModelRepository;

    // Get the "DrugModelRepository" component from the component manager
    Tucuxi::Common::ComponentManager* pCmpMgr = Tucuxi::Common::ComponentManager::getInstance();
    if (pCmpMgr != nullptr) {
        drugModelRepository = pCmpMgr->getComponent<Tucuxi::Core::IDrugModelRepository>("DrugModelRepository");
    }
    else {
        return nullptr;
    }

    if (_query.getpParameters().getDrugs()[0]->getDrugModelID() != "") {
        drugModel = drugModelRepository->getDrugModelById(_query.getpParameters().getDrugs()[0]->getDrugModelID());

        Tucuxi::Core::TreatmentDrugModelCompatibilityChecker checker;
        if (!checker.checkCompatibility(_drugTreatment, drugModel)) {
            return nullptr;
        }

    }
    else {
        std::vector<Tucuxi::Core::DrugModel *> drugModels = drugModelRepository->getDrugModelsByDrugId(_query.getpParameters().getDrugs()[0]->getDrugID());

        for (const auto dM : drugModels) {
            Tucuxi::Core::TreatmentDrugModelCompatibilityChecker checker;
            if (checker.checkCompatibility(_drugTreatment, dM)) {
                return dM;
            }
        }
    }

    return drugModel;
}

} // namespace Query
} // namespace Tucuxi
