//@@license@@

#include "clicomputer.h"

#include "tucucore/computingcomponent.h"
#include "tucucore/computingservice/computingrequest.h"
#include "tucucore/computingservice/icomputingservice.h"
#include "tucucore/overloadevaluator.h"

#include "tucuquery/computingqueryresponsexmlexport.h"
#include "tucuquery/computingresponseexport.h"
#include "tucuquery/querycomputer.h"
#include "tucuquery/querydata.h"
#include "tucuquery/queryimport.h"
#include "tucuquery/querytocoreextractor.h"

using namespace Tucuxi::Core;
using namespace Tucuxi::Query;

std::string fileAndDirFromPath(std::string& _outputPath);

CliComputer::CliComputer() = default;


QueryStatus CliComputer::compute(
        const std::string& _inputFileName,
        const std::string& _outputFileName,
        const std::string& _dataFilePath,
        const std::string& _tqfOutputFileName)
{

    // Change the settings for the tests
    Tucuxi::Core::SingleOverloadEvaluator::getInstance()->setValues(100000, 5000, 10000);

    // This method can be totally rewritten to take advantage of QueryComputer.
    // QueryComputer will be also used by the REST server, so all the common code should
    // be in QueryComputer.
    // One difference is that the input is a file for CliComputer, and a POST request for
    // the RestServer. So, the raw string extraction shall be the responsibility of CliComputer
    // and the RestServer. The raw string can then be used by QueryComputer.
    // The output is also managed differently, so the QueryResponse from QueryComputer shall
    // be used by CliComputer and the REST server to generate the useful output.


    Tucuxi::Common::LoggerHelper logHelper;

    ComputingQueryResponse computingQueryResponse;

    auto queryComputer = std::make_unique<QueryComputer>();

    std::ifstream ifs(_inputFileName);
    if (ifs.fail()) {
        logHelper.error("Could not open the input file: " + _inputFileName);
        computingQueryResponse.setQueryStatus(
                QueryStatus::ImportError, "Could not open the input file: " + _inputFileName);
        return computingQueryResponse.getQueryStatus();
    }
    std::string xmlString((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));

    queryComputer->compute(xmlString, computingQueryResponse, _tqfOutputFileName);


    if (!_dataFilePath.empty()
        && (computingQueryResponse.getQueryStatus() != QueryStatus::ImportError
            || computingQueryResponse.getQueryStatus() != QueryStatus::BadFormat)) {
        ComputingResponseExport exporter;

        if (!exporter.exportToFiles(computingQueryResponse, _dataFilePath)) {
            logHelper.error("Could not export the response file");
            return computingQueryResponse.getQueryStatus();
        }

        logHelper.info("The response files were successfully generated");
    }

    ComputingQueryResponseXmlExport xmlExporter;


    if (!xmlExporter.exportToFile(computingQueryResponse, _outputFileName)) {
        logHelper.error("Could not export the response XML file");
        return computingQueryResponse.getQueryStatus();
    }

    logHelper.info("The response XML file was successfully generated");

    return computingQueryResponse.getQueryStatus();
}
