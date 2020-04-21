#include "querycomputer.h"

#include "tucuquery/queryimport.h"
#include "tucucore/computingservice/icomputingservice.h"
#include "tucucore/computingcomponent.h"
#include "tucucore/computingservice/computingrequest.h"
#include "tucuquery/computingresponseexport.h"
#include "tucuquery/computingqueryresponsexmlexport.h"
#include "tucucore/overloadevaluator.h"
#include "tucuquery/querydata.h"
#include "tucuquery/queryimport.h"
#include "tucuquery/querytocoreextractor.h"
#include "tucuquery/querycomputer.h"
#include "tucucore/definitions.h"


namespace Tucuxi {
namespace Query {

QueryComputer::QueryComputer()
{

}

void QueryComputer::compute(ComputingQuery& _query, ComputingQueryResponse& _response)
{
    Core::IComputingService* computingComponent = dynamic_cast<Core::IComputingService*>(Core::ComputingComponent::createComponent());

    for(const std::unique_ptr<Core::ComputingRequest>& computingRequest : _query.m_computingRequests)
    {
        // A ComputingResponse local to the loop iteration. Moved to the _response at the end of the loop
        std::unique_ptr<Core::ComputingResponse> computingResponse = std::make_unique<Core::ComputingResponse>(computingRequest->getId());

        Core::ComputingResult result = computingComponent->compute(*computingRequest, computingResponse);

        std::unique_ptr<ComputingResponseMetaData> computingResponseMetaData = std::make_unique<ComputingResponseMetaData>("DrugModelId");

        // This for loop will be replaced by a getter in the future, as there is only one response
        for(std::unique_ptr<Core::SingleComputingResponse>& single : computingResponse->getResponses())
        {
            _response.addRequestResponse(std::move(single), result, std::move(computingResponseMetaData));
        }
    }
}

int QueryComputer::compute(std::string _queryString, ComputingQueryResponse& _response)
{
    Tucuxi::Common::LoggerHelper logHelper;

    QueryData *query = nullptr;

    QueryImport importer;
    QueryImport::Result importResult = importer.importFromString(query, _queryString);


    if (importResult != QueryImport::Result::Ok) {
        if (importResult == QueryImport::Result::CantOpenFile) {
            logHelper.error("Error with the import of query file. Unable to open \"{}\"", _queryString);
        }
        else {
            logHelper.error("Error with the import of query file \"{}\"", _queryString);
        }
        return 1;
    }

    QueryToCoreExtractor extractor;

    ComputingQuery *computingQuery = extractor.extractComputingQuery(*query);

    compute(*computingQuery, _response);

    // First build the ComputingQuery object

    // 1. Build the QueryData from _queryString

    // 2. Build a ComputingQuery from QueryData
    //    This can be done thanks to the QueryToCoreExtractor

    // Then call compute(query, _response);
    return 0;

}


} // namespace Query
} // namespace Tucuxi

