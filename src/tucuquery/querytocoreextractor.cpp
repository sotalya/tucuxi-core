#include "querytocoreextractor.h"

#include "tucucore/drugtreatment/drugtreatment.h"
#include "tucucore/computingservice/computingtrait.h"
#include "tucucore/drugmodelimport.h"
#include "tucucore/drugmodelrepository.h"
#include "tucucore/treatmentdrugmodelcompatibilitychecker.h"
#include "tucucore/drugdefinitions.h"
#include "tucucore/computingservice/computingrequest.h"

#include "tucucommon/componentmanager.h"
#include "tucucommon/loggerhelper.h"

#include "tucuquery/computingquery.h"

namespace Tucuxi {
namespace Query {



QueryToCoreExtractor::QueryToCoreExtractor()
= default;


QueryStatus QueryToCoreExtractor::extractComputingQuery(const QueryData &_query, ComputingQuery &_computingQuery)
{
    // In this method we have to build all the ComputingRequest objects,
    // and add them to the ComputingQuery.
    // It can obviously use the extractor method of QueryToCoreExtractor for the various fields

    Tucuxi::Common::LoggerHelper logHelper;


    Tucuxi::Core::DrugTreatment *drugTreatment = nullptr;


    for(const std::unique_ptr<RequestData>& requestData : _query.getRequests())
    {
         drugTreatment = extractDrugTreatment(_query, *requestData);
        if (drugTreatment == nullptr) {
            logHelper.error("Error during drug treatment import. Drug Id issue");
            setErrorMessage("Error during drug treatment import. Drug Id issue");
            return QueryStatus::ImportError;
        }

        Tucuxi::Core::DrugModel *drugModel = extractDrugModel(*requestData, drugTreatment);

        if (drugModel == nullptr) {
            logHelper.error("Could not find a suitable drug model");
            setErrorMessage("Could not find a suitable drug model");
            return QueryStatus::ImportError;
        }
        logHelper.info("Performing computation with drug model : {}, Request ID : {}", drugModel->getDrugModelId(), requestData->getRequestID());

        std::unique_ptr<Tucuxi::Core::ComputingRequest> computingRequest = std::make_unique<Tucuxi::Core::ComputingRequest>(requestData->getRequestID(), *drugModel, *drugTreatment, std::move(requestData->m_pComputingTrait));
        _computingQuery.addComputingRequest(std::move(computingRequest));
    }

    return QueryStatus::Ok;
}

Tucuxi::Core::PatientVariates QueryToCoreExtractor::extractPatientVariates(const QueryData &_query) const
{
    const std::vector< std::unique_ptr<Tucuxi::Core::PatientCovariate> >& covariates = _query.getpParameters()
               .getpPatient()
               .getCovariates();

       Tucuxi::Core::PatientVariates patientVariates;
       for (const std::unique_ptr<Tucuxi::Core::PatientCovariate>& covariate : covariates) {
           patientVariates.push_back(
                       std::make_unique<Tucuxi::Core::PatientCovariate>(
                           covariate->getId(),
                           covariate->getValue(),
                           covariate->getDataType(),
                           Common::Unit(covariate->getUnit()),
                           covariate->getValueAsDate()
                           )
                       );
       }

       return patientVariates;

}

Tucuxi::Core::Targets QueryToCoreExtractor::extractTargets(const QueryData &_query, size_t _drugPosition) const
{
    Tucuxi::Core::Targets targets;

    const std::vector< std::unique_ptr<Tucuxi::Core::Target> >& targetData = _query.getpParameters()
            .getDrugs().at(_drugPosition)
            ->getTargets();

    for (const auto& td : targetData) {
        targets.push_back(
                    std::make_unique<Tucuxi::Core::Target>(
                        td->getActiveMoietyId(),
                        td->getTargetType(),
                        td->getUnit(),
                        td->getValueMin(),
                        td->getValueBest(),
                        td->getValueMax(),
                        td->getInefficacyAlarm(),
                        td->getToxicityAlarm(),
                        td->getMicValue(),
                        td->getMicUnit()
                        )
                    );
    }

    return targets;
}


Tucuxi::Core::Samples QueryToCoreExtractor::extractSamples(const QueryData &_query, size_t _drugPosition) const
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
                            Common::Unit(cd->getUnit())
                            )
                        );
        }
    }

    return samples;
}

Tucuxi::Core::DrugTreatment *QueryToCoreExtractor::extractDrugTreatment(const QueryData &_query, const RequestData &_requestData) const
{
    Tucuxi::Core::DrugTreatment *drugTreatment = nullptr;

    drugTreatment = new Tucuxi::Core::DrugTreatment();

    // Getting the drug related to the request
    std::string drugID = _requestData.getDrugID();
    const std::vector< std::unique_ptr<DrugData> >& drugs = _query.getpParameters().getDrugs();
    size_t drugPosition = 0;
    for (; drugPosition < drugs.size(); ++drugPosition) {
        if (drugs.at(drugPosition)->getDrugID() == drugID) {
            break;
        }
    }

    if (drugPosition == drugs.size()) {
        return nullptr;
    }

    // Getting the dosage history for the drug treatment
    const Tucuxi::Core::DosageTimeRangeList& dosageTimeRangeList = drugs.at(drugPosition)
            ->getpTreatment()
            .getpDosageHistory()
            .getDosageTimeRanges();

    for (const auto& dosageTimeRange : dosageTimeRangeList) {
        //drugTreatment.addDosageTimeRange(make_unique<DosageTimeRange>(*dosageTimeRange));
        drugTreatment->getModifiableDosageHistory().addTimeRange(*dosageTimeRange);
    }

    // Getting the patient's covariates for the drug treatment
    Tucuxi::Core::PatientVariates patientVariates = extractPatientVariates(_query);
//    Tucuxi::Core::PatientVariates patientVariates = _query.getpParameters().getpPatient().getCovariates();
    for (auto& cov : patientVariates) {
        drugTreatment->addCovariate(move(cov));
    }

    // Getting the samples for the drug treatment
    Tucuxi::Core::Samples samples = extractSamples(_query, drugPosition);
    for (auto& sample : samples) {
        drugTreatment->addSample(move(sample));
    }

    // Getting the targets for the drug treatment
    Tucuxi::Core::Targets targets = extractTargets(_query, drugPosition);
//    Tucuxi::Core::Targets targets = _query.getpParameters().getDrugs().at(drugPosition)->getTargets();
    for (auto& target : targets) {
        drugTreatment->addTarget(move(target));
    }

    return drugTreatment;
}

Tucuxi::Core::DrugModel *QueryToCoreExtractor::extractDrugModel(const RequestData &_requestData, const Tucuxi::Core::DrugTreatment *_drugTreatment) const
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


    //TODO : MUST FIND A MODEL
    drugModel = drugModelRepository->getDrugModelById(_requestData.getDrugModelID());


    Tucuxi::Core::TreatmentDrugModelCompatibilityChecker checker;
    if (!checker.checkCompatibility(_drugTreatment, drugModel)) {
        return nullptr;
    }

    return drugModel;
}


} // namespace Query
} // namespace Tucuxi
