/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "tucucommon/utils.h"
#include "tucucommon/loggerhelper.h"
#include "tucucommon/general.h"

#include "tucucore/definitions.h"
#include "tucucore/intakeevent.h"
#include "tucucore/parameter.h"
#include "tucucore/pkmodels/onecompartmentextra.h"
#include "tucucore/drugmodelrepository.h"

#include "cxxopts/include/cxxopts.hpp"
#include "clicomputer.h"
#include "tucuquery/querylogger.h"

using namespace std::chrono_literals;

cxxopts::ParseResult
parse(int _argc, char* _argv[])
{
    // Get application folder
    std::string appFolder = Tucuxi::Common::Utils::getAppFolder(_argv);

    Tucuxi::Common::LoggerHelper logHelper;

    try
    {

        cxxopts::Options options(_argv[0], " - Tucuxi command line");
        options
                .positional_help("[optional args]")
                .show_positional_help();

        options
                .allow_unrecognised_options()
                .add_options()
                ("d,drugpath", "Drug files path", cxxopts::value<std::string>())
                ("i,input", "Input request file", cxxopts::value<std::string>())
                ("o,output", "Output response file", cxxopts::value<std::string>())
                ("q,querylogpath", "Query folder path", cxxopts::value<std::string>())
                ("help", "Print help")
                ;


        auto result = options.parse(_argc, _argv);

        if (result.count("help") > 0)
        {
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
            exit(0);
        }

        std::string outputPath;
        if (result.count("output") > 0) {
            outputPath = result["output"].as<std::string>();
        }
        else {
            std::cout << "The output file is mandatory" << std::endl << std::endl;
            std::cout << options.help({"", "Group"}) << std::endl;
            exit(0);
        }

        std::string queryLogPath;
        if (result.count("querylogpath") > 0) {
            queryLogPath = result["querylogpath"].as<std::string>();
        }

        logHelper.info("Drugs directory : {}", drugPath);
        logHelper.info("Input file : {}", inputFileName);
        logHelper.info("Output directory : {}", outputPath);
        logHelper.info("QueryLogs directory : {}", queryLogPath);

        Tucuxi::Common::ComponentManager* pCmpMgr = Tucuxi::Common::ComponentManager::getInstance();


        Tucuxi::Core::DrugModelRepository *drugModelRepository = dynamic_cast<Tucuxi::Core::DrugModelRepository*>(
                    Tucuxi::Core::DrugModelRepository::createComponent());

        pCmpMgr->registerComponent("DrugModelRepository", drugModelRepository);

        drugModelRepository->addFolderPath(drugPath);

        Tucuxi::Query::QueryLogger *queryLogger = dynamic_cast<Tucuxi::Query::QueryLogger*>(
                    Tucuxi::Query::QueryLogger::createComponent(queryLogPath));

        pCmpMgr->registerComponent("QueryLogger", queryLogger);

//        drugModelRepository->loadFolder(drugPath);


        CliComputer computer;
        int exitCode = computer.compute(drugPath, inputFileName, outputPath, queryLogPath);

        if (exitCode != 0) {
            exit(exitCode);
        }

        pCmpMgr->unregisterComponent("DrugModelRepository");
        pCmpMgr->unregisterComponent("QueryLogger");
        delete drugModelRepository;

        return result;

    } catch (const cxxopts::OptionException& e)
    {
        logHelper.error("error parsing options: {}", e.what());
        exit(1);
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

    auto result = parse(_argc, _argv);

    logHelper.info("Tucuxi console application is exiting...");

    Tucuxi::Common::LoggerHelper::beforeExit();

    return 0;
}
