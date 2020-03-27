#include "clicomputer.h"

#include "tucucore/computingservice/icomputingservice.h"
#include "tucucore/computingcomponent.h"
#include "tucucore/computingservice/computingrequest.h"
#include "tucucore/computingresponseexport.h"
#include "tucucore/computingresponsexmlexport.h"
#include "tucucore/overloadevaluator.h"
#include "tucuquery/query.h"
#include "tucuquery/queryimport.h"
#include "tucuquery/querytocoreextractor.h"

using namespace Tucuxi::Core;
using namespace Tucuxi::Query;

CliComputer::CliComputer()
{

}


int CliComputer::compute(std::string _drugPath,
                          std::string _inputFileName,
                          std::string _outputPath)
{
    TMP_UNUSED_PARAMETER(_drugPath);

    // Change the settings for the tests
    Tucuxi::Core::SingleOverloadEvaluator::getInstance()->setValues(100000, 5000, 10000);


    Tucuxi::Common::LoggerHelper logHelper;


    RequestResponseId requestResponseID;

    DrugModel *drugModel = nullptr;
    DrugTreatment *drugTreatment = nullptr;


    Query *query = nullptr;
    QueryImport importer;

    QueryImport::Result importResult = importer.importFromFile(query, _inputFileName);


    if (importResult != QueryImport::Result::Ok) {
        if (importResult == QueryImport::Result::CantOpenFile) {
            logHelper.error("Error with the import of query file. Unable to open \"{}\"", _inputFileName);
        }
        else {
            logHelper.error("Error with the import of query file \"{}\"", _inputFileName);
        }
        return 1;
    }

    requestResponseID = query->getQueryID();

    QueryToCoreExtractor extractor;

    drugTreatment = extractor.extractDrugTreatment(*query);

    if (drugTreatment == nullptr) {
        logHelper.error("Error with the drug treatment import");
        return 1;
    }



    drugModel = extractor.extractDrugModel(*query, drugTreatment);

    if (drugModel == nullptr) {
        logHelper.error("Could not find a suitable drug model");
        return 1;
    }

    logHelper.info("Performing computation with drug model : {}", drugModel->getDrugModelId());


    ComputingTraits* computingTraits = extractor.extractComputingTraits(*query);

    ComputingRequest request(requestResponseID, *drugModel, *drugTreatment, std::unique_ptr<ComputingTraits>(computingTraits));

    std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseID);


    IComputingService* computingComponent = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

    ComputingResult result = computingComponent->compute(request, response);

    if (result != ComputingResult::Ok) {
        logHelper.error("Computing failed");
        return 1;
    }

    ComputingResponseExport exporter;

    if (!exporter.exportToFiles(*response.get(), _outputPath)) {
        logHelper.error("Could not export the response file");
        return 1;
    }
    else {
        logHelper.info("The response files were successfully generated");
    }

    ComputingResponseXmlExport xmlExporter;

    if (!xmlExporter.exportToFile(*response.get(), _outputPath + "/" + query->getQueryID() + ".xml")) {
        logHelper.error("Could not export the response XML file");
        return 1;
    }
    else {
        logHelper.info("The response XML file was successfully generated");
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

    return 0;
}
