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
{

}


Tucuxi::Query::ComputingQuery* QueryToCoreExtractor::extractComputingQuery(const QueryData &_query) const
{
    // In this method we have to build all the ComputingRequest objects,
    // and add them to the ComputingQuery.
    // It can obviously use the extractor method of QueryToCoreExtractor for the various fields

    Tucuxi::Common::LoggerHelper logHelper;

    Tucuxi::Core::RequestResponseId requestResponseID = _query.getQueryID();


    Tucuxi::Core::DrugTreatment *drugTreatment = nullptr;

    ComputingQuery *newQuery = new ComputingQuery(_query.getQueryID());


    for(const std::unique_ptr<RequestData>& requestData : _query.getRequests())
    {
         drugTreatment = extractDrugTreatment(_query, *requestData);
        if (drugTreatment == nullptr) {
            logHelper.error("Error with the drug treatment import");
            return nullptr;
        }

        Tucuxi::Core::DrugModel *drugModel = extractDrugModel(_query, *requestData, drugTreatment);

        if (drugModel == nullptr) {
            logHelper.error("Could not find a suitable drug model");
            return nullptr;
        }
        logHelper.info("Performing computation with drug model : {}, Request ID : {}", drugModel->getDrugModelId(), requestData->getRequestID());

        std::unique_ptr<Tucuxi::Core::ComputingRequest> computingRequest = std::make_unique<Tucuxi::Core::ComputingRequest>(requestResponseID, *drugModel, *drugTreatment, std::move(requestData->m_pComputingTrait));
        newQuery->addComputingRequest(std::move(computingRequest));
    }

    return newQuery;
}

Tucuxi::Core::PatientVariates QueryToCoreExtractor::extractPatientVariates(const QueryData &_query) const
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

Tucuxi::Core::Targets QueryToCoreExtractor::extractTargets(const QueryData &_query, size_t _drugPosition) const
{
    Tucuxi::Core::Targets targets;

    const std::vector< std::unique_ptr<TargetData> >& targetsData = _query.getpParameters()
            .getDrugs().at(_drugPosition)
            ->getTargets();

    for (const std::unique_ptr<TargetData>& td : targetsData) {
        targets.push_back(
                    std::make_unique<Tucuxi::Core::Target>(
                        Tucuxi::Core::ActiveMoietyId(td->getActiveMoietyID()),
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
                            Core::Unit(cd->getUnit())
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
        Tucuxi::Common::LoggerHelper logHelper;
        logHelper.warn("Issue with the drug Id");
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

Tucuxi::Core::DrugModel *QueryToCoreExtractor::extractDrugModel(const QueryData &_query, const RequestData &_requestData, const Tucuxi::Core::DrugTreatment *_drugTreatment) const
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
