#include "clicomputer.h"

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

using namespace Tucuxi::Core;
using namespace Tucuxi::Query;

CliComputer::CliComputer() = default;


QueryStatus CliComputer::compute(const std::string& _inputFileName,
                                 const std::string& _outputPath)
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
    std::string xmlString((std::istreambuf_iterator<char>(ifs)),(std::istreambuf_iterator<char>()));

    QueryStatus queryStatus = queryComputer->compute(xmlString, computingQueryResponse);

    if(queryStatus == QueryStatus::Ok)
    {
        ComputingResponseExport exporter;

        if (!exporter.exportToFiles(computingQueryResponse, _outputPath)) {
            logHelper.error("Could not export the response file");
            return computingQueryResponse.getQueryStatus();
        }

        logHelper.info("The response files were successfully generated");

    }

    ComputingQueryResponseXmlExport xmlExporter;

    std::string fileName = _outputPath + "/" + computingQueryResponse.getQueryId() + ".xml";

    if (!xmlExporter.exportToFile(computingQueryResponse, fileName)) {
        logHelper.error("Could not export the response XML file");
        return computingQueryResponse.getQueryStatus();
    }

    logHelper.info("The response XML file was successfully generated");

    return computingQueryResponse.getQueryStatus();



    return queryStatus;
}
