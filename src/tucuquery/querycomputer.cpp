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


#include "querycomputer.h"

#include "tucucore/computingcomponent.h"
#include "tucucore/computingservice/computingrequest.h"
#include "tucucore/computingservice/icomputingservice.h"
#include "tucucore/definitions.h"
#include "tucucore/overloadevaluator.h"

#include "tucuquery/computingqueryresponsexmlexport.h"
#include "tucuquery/computingresponseexport.h"
#include "tucuquery/querycomputer.h"
#include "tucuquery/querydata.h"
#include "tucuquery/queryimport.h"
#include "tucuquery/querylogger.h"
#include "tucuquery/querytocoreextractor.h"


namespace Tucuxi {
namespace Query {

QueryComputer::QueryComputer() = default;

void QueryComputer::compute(ComputingQuery& _query, ComputingQueryResponse& _response)
{

    auto computingComponent = dynamic_cast<Core::IComputingService*>(Core::ComputingComponent::createComponent());

    unsigned int errorsCounter = 0;
    for (const std::unique_ptr<Core::ComputingRequest>& computingRequest : _query.m_computingRequests) {
        // A ComputingResponse local to the loop iteration. Moved to the _response at the end of the loop
        std::unique_ptr<Core::ComputingResponse> computingResponse =
                std::make_unique<Core::ComputingResponse>(computingRequest->getId());

        Core::ComputingStatus result = computingComponent->compute(*computingRequest, computingResponse);

        computingResponse->setComputingStatus(result);

        std::unique_ptr<ComputingResponseMetaData> computingResponseMetaData =
                std::make_unique<ComputingResponseMetaData>(computingRequest->getDrugModel().getDrugModelId());
        _response.setRequestResponseId(_query.m_queryId);
        _response.addRequestResponse(std::move(computingResponse), std::move(computingResponseMetaData));

        if (result != Core::ComputingStatus::Ok) {
            errorsCounter++;
        }
    }

    if (_response.getQueryStatus() == QueryStatus::Undefined) {
        // THERE WAS NO ERROR UNTIL NOW
        if (errorsCounter == _query.m_computingRequests.size()) {
            _response.setQueryStatus(QueryStatus::Error);
        }
        else if (errorsCounter == 0) {
            _response.setQueryStatus(QueryStatus::Ok);
        }
        else {
            _response.setQueryStatus(QueryStatus::PartiallyOk);
        }
    }

    delete computingComponent;
}

void QueryComputer::compute(const std::string& _queryString, ComputingQueryResponse& _response)
{
    Tucuxi::Common::LoggerHelper logHelper;

    std::unique_ptr<QueryData> query = nullptr;

    QueryImport importer;
    QueryImport::Status importResult = importer.importFromString(query, _queryString);


    if (importResult != QueryImport::Status::Ok) {

        if ((importResult == QueryImport::Status::CantOpenFile)
            || (importResult == QueryImport::Status::CantCreateXmlDocument)) {
            logHelper.error("Error, see details : {}", importer.getErrorMessage());
            _response.setRequestResponseId("bad.format");
            _response.setQueryStatus(QueryStatus::BadFormat, importer.getErrorMessage());
        }
        else {
            logHelper.error("Error, see details : {}", importer.getErrorMessage());
            _response.setRequestResponseId(query->getQueryID());
            _response.setQueryStatus(QueryStatus::ImportError, importer.getErrorMessage());
        }


        return;
    }

    Tucuxi::Common::ComponentManager* pCmpMgr = Tucuxi::Common::ComponentManager::getInstance();
    if (pCmpMgr != nullptr) {
        auto queryLogger = pCmpMgr->getComponent<Tucuxi::Query::IQueryLogger>("QueryLogger");
        if (!queryLogger->getFolderPath().empty()) {
            queryLogger->saveQuery(_queryString, query->getQueryID());
        }
    }

    QueryToCoreExtractor extractor;
    auto computingQuery = std::make_unique<ComputingQuery>(query->getQueryID());

    std::vector<std::unique_ptr<Tucuxi::Core::DrugTreatment> > drugTreatments;
    QueryStatus extractResult = extractor.extractComputingQuery(*query, *computingQuery, drugTreatments);
    if (extractResult != QueryStatus::Ok) {
        _response.setRequestResponseId(query->getQueryID());
        _response.setQueryStatus(QueryStatus::ImportError, extractor.getErrorMessage());
        return;
    }

    compute(*computingQuery, _response);
}


} // namespace Query
} // namespace Tucuxi
