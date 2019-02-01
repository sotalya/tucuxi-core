#include "querytocoreextractor.h"

#include "tucucore/drugtreatment/drugtreatment.h"
#include "tucucore/computingservice/computingtrait.h"
#include "tucucore/drugmodelimport.h"
#include "tucucore/drugmodelrepository.h"
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
                        covariate->getName(),
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
            Tucuxi::Core::ComputingTrait *trait = extractPredictionTrait(*request.get());
            traits->addTrait(std::unique_ptr<Tucuxi::Core::ComputingTrait>(trait));
        } else if (requestType == "dosageAdaptation") {
        } else if (requestType == "firstDosage") {
        } else {
        }


    }



    return traits;
}

Tucuxi::Core::ComputingTrait *QueryToCoreExtractor::extractPredictionTrait(const RequestData &_request) const
{

    std::string predictionType =_request.getPredictionType();
    Tucuxi::Core::PredictionParameterType predictionParameterType = Tucuxi::Core::PredictionParameterType::Population;
    if (predictionType == "population") {
        predictionParameterType = Tucuxi::Core::PredictionParameterType::Population;
    } else if (predictionType == "a priori") {
        predictionParameterType = Tucuxi::Core::PredictionParameterType::Apriori;
    } else if (predictionType == "a posteriori") {
        predictionParameterType = Tucuxi::Core::PredictionParameterType::Aposteriori;
    }
    /*else if (predictionType == "best") {
        bool covariatesExist = drugTreatment.getCovariates().empty() ? false : true;
        bool samplesExist = drugTreatment.getSamples().empty() ? false : true;

        if (covariatesExist && samplesExist) {
            predictionParameterType = PredictionParameterType::Aposteriori;
        } else if (covariatesExist && !samplesExist) {
            predictionParameterType = PredictionParameterType::Apriori;
        } else {
            predictionParameterType = PredictionParameterType::Population;
        }

    } else {
        apiResponse->addWarning("UnexpetedPredicitionType", m_query.getLanguage(), "Unexpected prediction type!");
        predictionParameterType = PredictionParameterType::Population;
    }
    */
    Tucuxi::Core::ComputingOption computingOption(predictionParameterType, Tucuxi::Core::CompartmentsOption::MainCompartment);

    Tucuxi::Core::ComputingTraitConcentration *trait = new Tucuxi::Core::ComputingTraitConcentration(
                _request.getRequestID(),
                _request.getpDateInterval().getStart(),
                _request.getpDateInterval().getEnd(),
                100, // TODO : Arbitrary number here, should be changed
                computingOption);

    return trait;
}


Tucuxi::Core::DrugModel *QueryToCoreExtractor::extractDrugModel(const Query &_query) const
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
    }
    else {
        std::vector<Tucuxi::Core::DrugModel *> drugModels = drugModelRepository->getDrugModelsByDrugId(_query.getpParameters().getDrugs()[0]->getDrugID());
        if (drugModels.size() != 0) {
            drugModel = drugModels[0];
        }
    }

    return drugModel;
}

} // namespace Query
} // namespace Tucuxi
