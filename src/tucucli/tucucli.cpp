//@@license@@
#include <fstream>
#include <iostream>

#include "tucucommon/general.h"
#include "tucucommon/loggerhelper.h"
#include "tucucommon/utils.h"

#include "tucucore/definitions.h"
#include "tucucore/drugmodelrepository.h"
#include "tucucore/intakeevent.h"
#include "tucucore/parameter.h"
#include "tucucore/pkmodels/onecompartmentextra.h"

#include "tucuquery/querylogger.h"
#include "tucuquery/querystatus.h"

#include "clicomputer.h"
#include "cxxopts/include/cxxopts.hpp"
#include "tucusign/signparser.h"
#include "tucusign/signvalidator.h"

using namespace std::chrono_literals;


int parse(int _argc, char* _argv[]) // NOLINT(cppcoreguidelines-avoid-c-arrays, modernize-avoid-c-arrays)
{
    // Get application folder
    std::string appFolder = Tucuxi::Common::Utils::getAppFolder(_argv);

    Tucuxi::Common::LoggerHelper logHelper;

    try {

        cxxopts::Options options(
                _argv[0], " - Tucuxi command line"); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        options.positional_help("[optional args]").show_positional_help();

        options.allow_unrecognised_options().add_options()(
                "d,drugpath", "Drug files path", cxxopts::value<std::string>())(
                "i,input", "Input request file", cxxopts::value<std::string>())(
                "o,output", "Output response file", cxxopts::value<std::string>())(
                "q,querylogpath", "Query folder path", cxxopts::value<std::string>())(
                "csv", "Data file (.dat) folder path", cxxopts::value<std::string>())(
                "s,signature", "Signed drug file path", cxxopts::value<std::string>())("help", "Print help");


        auto result = options.parse(_argc, _argv);

        if (result.count("signature") > 0) {
            std::string signedDrugfilePath = result["signature"].as<std::string>();
            Tucuxi::Common::Signature signature;
            Tucuxi::Common::ParsingError parsingResponse = Tucuxi::Common::SignParser::loadSignature(signedDrugfilePath, signature);

            if (parsingResponse == Tucuxi::Common::ParsingError::SIGNATURE_OK) {
                try {
                    Tucuxi::Common::SignatureError signatureResponse = Tucuxi::Common::SignValidator::validateSignature(signature);

                    if (signatureResponse == Tucuxi::Common::SignatureError::SIGNATURE_VALID) {
                        std::cout << "\nThe drug file has been signed by: \n"
                                  << Tucuxi::Common::SignValidator::loadSigner(signature.getUserCert()) << std::endl;
                    }
                } catch (...) {
                    logHelper.error("The drug file is not properly signed");
                    exit(1);
                }
            }
            exit(0);
        }

        if (result.count("help") > 0) {
            std::cout << options.help({"", "Group"}) << std::endl;
            exit(0);
        }

        std::string drugPath = appFolder + "/drugs2";
        if (result.count("drugpath") > 0) {
            drugPath = result["drugpath"].as<std::string>();
        }

        std::string inputFileName;
        if (result.count("input") > 0) {
            inputFileName = result["input"].as<std::string>();
        }
        else {
            std::cout << "The input file is mandatory" << std::endl << std::endl;
            std::cout << options.help({"", "Group"}) << std::endl;
            exit(-2);
        }

        std::string outputFileName;
        if (result.count("output") > 0) {
            outputFileName = result["output"].as<std::string>();
        }
        else {
            std::cout << "The output file is mandatory" << std::endl << std::endl;
            std::cout << options.help({"", "Group"}) << std::endl;
            exit(-2);
        }

        std::string queryLogPath;
        if (result.count("querylogpath") > 0) {
            queryLogPath = result["querylogpath"].as<std::string>();
        }

        std::string datafilepath;
        if (result.count("csv") > 0) {
            datafilepath = result["csv"].as<std::string>();
        }

        logHelper.info("Drugs directory : {}", drugPath);
        logHelper.info("Input file : {}", inputFileName);
        logHelper.info("Output file name : {}", outputFileName);
        logHelper.info("QueryLogs directory : {}", queryLogPath);
        logHelper.info("Data file directory : {}", datafilepath);

        Tucuxi::Common::ComponentManager* pCmpMgr = Tucuxi::Common::ComponentManager::getInstance();



        auto drugModelRepository =
                dynamic_cast<Tucuxi::Core::DrugModelRepository*>(Tucuxi::Core::DrugModelRepository::createComponent());

        pCmpMgr->registerComponent("DrugModelRepository", drugModelRepository);

        drugModelRepository->addFolderPath(drugPath);

        Tucuxi::Query::QueryLogger* queryLogger =
                dynamic_cast<Tucuxi::Query::QueryLogger*>(Tucuxi::Query::QueryLogger::createComponent(queryLogPath));

        pCmpMgr->registerComponent("QueryLogger", queryLogger);

        CliComputer computer;
        auto queryStatus = computer.compute(inputFileName, outputFileName, datafilepath);


        pCmpMgr->unregisterComponent("DrugModelRepository");
        pCmpMgr->unregisterComponent("QueryLogger");


        delete drugModelRepository;

        delete queryLogger;

        return static_cast<int>(queryStatus);
    }
    catch (const cxxopts::OptionException& e) {
        logHelper.error("error parsing options: {}", e.what());

        return -1;


        //retourner -1
    }
}

/// \defgroup TucuCli Tucuxi Console application
/// \brief The Tucuxi console application
///
/// The Tucuxi console application offers a simple command line interface to
/// launch prediction and suggestion algorithms.
///
/// This application is intended mainly to run automated test scripts

int main(int _argc, char** _argv)
{
    // Get application folder
    std::string appFolder = Tucuxi::Common::Utils::getAppFolder(_argv);

    Tucuxi::Common::LoggerHelper::init(appFolder + "/tucucli.log");
    Tucuxi::Common::LoggerHelper logHelper;

    logHelper.info("********************************************************");
    logHelper.info("Tucuxi console application is starting up...");

    int status = parse(_argc, _argv);

    logHelper.info("Tucuxi console application is exiting...");

    Tucuxi::Common::LoggerHelper::beforeExit();

    return status;
}
