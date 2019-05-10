#include "clicomputer.h"

#include "tucucore/computingservice/icomputingservice.h"
#include "tucucore/computingcomponent.h"
#include "tucucore/computingservice/computingrequest.h"
#include "tucucore/computingresponseexport.h"
#include "tucuquery/query.h"
#include "tucuquery/queryimport.h"
#include "tucuquery/querytocoreextractor.h"

using namespace Tucuxi::Core;
using namespace Tucuxi::Query;

CliComputer::CliComputer()
{

}


void CliComputer::compute(std::string _drugPath,
                          std::string _inputFileName,
                          std::string _outputPath)
{
    TMP_UNUSED_PARAMETER(_drugPath);

    Tucuxi::Common::LoggerHelper logHelper;


    RequestResponseId requestResponseID;

    DrugModel *drugModel = nullptr;
    DrugTreatment *drugTreatment = nullptr;


    Query *query = nullptr;
    QueryImport importer;

    QueryImport::Result importResult = importer.importFromFile(query, _inputFileName);


    if (importResult != QueryImport::Result::Ok) {
        logHelper.error("Error with the import of query file \"{}\"", _inputFileName);
        return;
    }

    requestResponseID = query->getQueryID();

    QueryToCoreExtractor extractor;

    drugTreatment = extractor.extractDrugTreatment(*query);

    if (drugTreatment == nullptr) {
        logHelper.error("Error with the drug treatment import");
        return;
    }



    drugModel = extractor.extractDrugModel(*query, drugTreatment);

    if (drugModel == nullptr) {
        logHelper.error("Could not find a suitable drug model");
        return;
    }

    logHelper.info("Performing computation with drug model : {}", drugModel->getDrugModelId());


    ComputingTraits* computingTraits = extractor.extractComputingTraits(*query);

    ComputingRequest request(requestResponseID, *drugModel, *drugTreatment, std::unique_ptr<ComputingTraits>(computingTraits));

    std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseID);


    IComputingService* computingComponent = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

    ComputingResult result = computingComponent->compute(request, response);

    if (result != ComputingResult::Ok) {
        logHelper.error("Computing failed");
        return;
    }

    ComputingResponseExport exporter;

    if (!exporter.exportToFiles(*response.get(), _outputPath)) {
        logHelper.error("Could not export the response file");
    }
    else {
        logHelper.info("The response files were successfully generated");
    }

    if (drugTreatment != nullptr) {
        delete drugTreatment;
    }
    if (query != nullptr) {
        delete query;
    }
    if (computingComponent != nullptr) {
        delete computingComponent;
    }
}
