/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "tucucommon/utils.h"
#include "tucucommon/loggerhelper.h"
#include "tucucommon/licensemanager.h"
#include "tucucommon/general.h"

#include "tucucore/drugfilevalidator.h"

#include "cxxopts/include/cxxopts.hpp"

cxxopts::ParseResult
parse(int argc, char* argv[])
{
    // Get application folder
    std::string appFolder = Tucuxi::Common::Utils::getAppFolder(argv);


    try
    {

        cxxopts::Options options(argv[0], " - Tucuxi drug file validator");
        options
                .positional_help("[optional args]")
                .show_positional_help();

        options
                .allow_unrecognised_options()
                .add_options()
                ("d,drugfile", "Drug file", cxxopts::value<std::string>())
                ("t,testfile", "Tests to be conducted", cxxopts::value<std::string>())
                ("l,logfile", "Log file", cxxopts::value<std::string>())
                ("help", "Print help")
                ;


        auto result = options.parse(argc, argv);

        if (result.count("help"))
        {
            std::cout << options.help({"", "Group"}) << std::endl;
            exit(0);
        }

        std::string drugFileName;
        if (result.count("drugfile")) {
            drugFileName = result["drugfile"].as<std::string>();
        }
        else {
            std::cout << "The drug file is mandatory" << std::endl << std::endl;
            std::cout << options.help({"", "Group"}) << std::endl;
            exit(0);
        }

        std::string testsFileName;
        if (result.count("testfile")) {
            testsFileName = result["testfile"].as<std::string>();
        }
        else {
            std::cout << "The test file is mandatory" << std::endl << std::endl;
            std::cout << options.help({"", "Group"}) << std::endl;
            exit(0);
        }

        std::string logFileName;
        if (result.count("logfile")) {
            logFileName = result["logfile"].as<std::string>();
        }
        else {
            logFileName = "./tucuvalidator.log";
        }


        Tucuxi::Common::LoggerHelper::init(logFileName);
        Tucuxi::Common::LoggerHelper logHelper;

        logHelper.info("Drug file : {}", drugFileName);
        logHelper.info("Test file : {}", testsFileName);
        logHelper.info("Log file  : {}", logFileName);


        Tucuxi::Core::DrugFileValidator validator;

        validator.validate(drugFileName, testsFileName);

        return result;

    } catch (const cxxopts::OptionException& e)
    {
        Tucuxi::Common::LoggerHelper logHelper;
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

int main(int argc, char** argv)
{
    // Get application folder
    std::string appFolder = Tucuxi::Common::Utils::getAppFolder(argv);


    auto result = parse(argc, argv);

    Tucuxi::Common::LoggerHelper::beforeExit();

    return 0;
}
