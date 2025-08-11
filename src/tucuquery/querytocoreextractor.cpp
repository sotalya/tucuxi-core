/*
 * Tucuxi - Tucuxi-core library and command line tool.
 * This code allows to perform prediction of drug concentration in blood
 * and to propose dosage adaptations.
 * It has been developed by HEIG-VD, in close collaboration with CHUV.
 * Copyright (C) 2023 HEIG-VD, maintained by Yann Thoma  <yann.thoma@heig-vd.ch>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */


#include "querytocoreextractor.h"

#include "tucucommon/componentmanager.h"
#include "tucucommon/loggerhelper.h"

#include "tucucore/computingservice/computingrequest.h"
#include "tucucore/computingservice/computingtrait.h"
#include "tucucore/drugdefinitions.h"
#include "tucucore/drugmodelimport.h"
#include "tucucore/drugmodelrepository.h"
#include "tucucore/drugtreatment/drugtreatment.h"
#include "tucucore/treatmentdrugmodelcompatibilitychecker.h"

#include "tucuquery/computingquery.h"
#include "tucuquery/fullsample.h"

namespace Tucuxi {
namespace Query {

QueryToCoreExtractor::QueryToCoreExtractor() = default;


QueryStatus QueryToCoreExtractor::extractComputingQuery(
        const QueryData& _query,
        ComputingQuery& _computingQuery,
        std::vector<std::unique_ptr<Core::DrugTreatment> >& _drugTreatments)
{
    // In this method we have to build all the ComputingRequest objects,
    // and add them to the ComputingQuery.
    // It can obviously use the extractor method of QueryToCoreExtractor for the various fields

    Tucuxi::Common::LoggerHelper logHelper;

    for (const std::unique_ptr<RequestData>& requestData : _query.getRequests()) {

        auto drugTreatment = extractDrugTreatment(_query, *requestData);
        if (drugTreatment == nullptr) {
            logHelper.error("Error during drug treatment import. Drug Id issue");
            setErrorMessage("Error during drug treatment import. Drug Id issue");
            return QueryStatus::ImportError;
        }

        Tucuxi::Core::DrugModel* drugModel = extractDrugModel(*requestData, drugTreatment.get());

        if (drugModel == nullptr) {
            logHelper.error("Could not find a suitable drug model");
            setErrorMessage("Could not find a suitable drug model");
            return QueryStatus::ImportError;
        }
        logHelper.info(
                "Performing computation with drug model : {}, Request ID : {}",
                drugModel->getDrugModelId(),
                requestData->getRequestID());

        std::unique_ptr<Tucuxi::Core::ComputingRequest> computingRequest =
                std::make_unique<Tucuxi::Core::ComputingRequest>(
                        requestData->getRequestID(),
                        *drugModel,
                        *drugTreatment,
                        std::move(requestData->m_pComputingTrait));
        _computingQuery.addComputingRequest(std::move(computingRequest));

        _drugTreatments.push_back(std::move(drugTreatment));
    }

    return QueryStatus::Ok;
}

Tucuxi::Core::PatientVariates QueryToCoreExtractor::extractPatientVariates(const QueryData& _query) const
{
    const std::vector<std::unique_ptr<Tucuxi::Core::PatientCovariate> >& covariates =
            _query.getpParameters().getpPatient().getCovariates();

    Tucuxi::Core::PatientVariates patientVariates;
    for (const std::unique_ptr<Tucuxi::Core::PatientCovariate>& covariate : covariates) {
        patientVariates.push_back(std::make_unique<Tucuxi::Core::PatientCovariate>(
                covariate->getId(),
                covariate->getValue(),
                covariate->getDataType(),
                covariate->getUnit(),
                covariate->getEventTime()));
    }

    return patientVariates;
}

Tucuxi::Core::Targets QueryToCoreExtractor::extractTargets(const QueryData& _query, size_t _drugPosition) const
{
    Tucuxi::Core::Targets targets;

    const std::vector<std::unique_ptr<Tucuxi::Core::Target> >& targetData =
            _query.getpParameters().getDrugs().at(_drugPosition)->getTargets();

    for (const auto& td : targetData) {
        targets.push_back(std::make_unique<Tucuxi::Core::Target>(
                td->getActiveMoietyId(),
                td->getTargetType(),
                td->getUnit(),
                td->getValueMin(),
                td->getValueBest(),
                td->getValueMax(),
                td->getInefficacyAlarm(),
                td->getToxicityAlarm(),
                td->getMicValue(),
                td->getMicUnit()));
    }

    return targets;
}


Tucuxi::Query::FullSamples QueryToCoreExtractor::extractSamples(const QueryData& _query, size_t _drugPosition) const
{
    Tucuxi::Query::FullSamples samples;

    const std::vector<std::unique_ptr<Tucuxi::Query::FullSample> >& samplesData =
            _query.getpParameters().getDrugs().at(_drugPosition)->getSamples();

    for (const std::unique_ptr<Tucuxi::Query::FullSample>& sd : samplesData) {
        samples.push_back(std::make_unique<Tucuxi::Query::FullSample>(
                sd->getSampleId(), sd->getDate(), sd->getAnalyteID(), sd->getValue(), sd->getUnit()));
    }

    return samples;
}

std::unique_ptr<Tucuxi::Core::DrugTreatment> QueryToCoreExtractor::extractDrugTreatment(
        const QueryData& _query, const RequestData& _requestData) const
{

    // Getting the drug related to the request
    std::string drugID = _requestData.getDrugID();
    const std::vector<std::unique_ptr<DrugData> >& drugs = _query.getpParameters().getDrugs();
    size_t drugPosition = 0;
    for (; drugPosition < drugs.size(); ++drugPosition) {
        if (drugs.at(drugPosition)->getDrugID() == drugID) {
            break;
        }
    }

    if (drugPosition == drugs.size()) {
        return nullptr;
    }

    auto drugTreatment = std::make_unique<Tucuxi::Core::DrugTreatment>();

    // Getting the dosage history for the drug treatment
    const Tucuxi::Core::DosageTimeRangeList& dosageTimeRangeList =
            drugs.at(drugPosition)->getpTreatment().getpDosageHistory().getDosageTimeRanges();

    for (const auto& dosageTimeRange : dosageTimeRangeList) {
        drugTreatment->getModifiableDosageHistory().addTimeRange(*dosageTimeRange);
    }

    // Getting the patient's covariates for the drug treatment
    Tucuxi::Core::PatientVariates patientVariates = extractPatientVariates(_query);
    for (auto& cov : patientVariates) {
        drugTreatment->addCovariate(move(cov));
    }

    // Getting the samples for the drug treatment
    Tucuxi::Query::FullSamples samples = extractSamples(_query, drugPosition);
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

Tucuxi::Core::DrugModel* QueryToCoreExtractor::extractDrugModel(
        const RequestData& _requestData, const Tucuxi::Core::DrugTreatment* _drugTreatment) const
{
    Tucuxi::Core::DrugModel* drugModel = nullptr;

    // Getting the drug model
    Tucuxi::Core::IDrugModelRepository* drugModelRepository = nullptr;

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
