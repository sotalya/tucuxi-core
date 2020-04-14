/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "cxxopts/include/cxxopts.hpp"

#include "tucucommon/utils.h"
#include "tucucommon/loggerhelper.h"
#include "tucucommon/general.h"

#include "tucucore/drugmodelchecker.h"
#include "tucucore/pkmodel.h"
#include "tucucore/drugmodelimport.h"

cxxopts::ParseResult
parse(int _argc, char* _argv[])
{
    // Get application folder
    std::string appFolder = Tucuxi::Common::Utils::getAppFolder(_argv);


    try
    {

        cxxopts::Options options(_argv[0], " - Tucuxi drug file checker");
        options
                .positional_help("[optional args]")
                .show_positional_help();

        options
                .allow_unrecognised_options()
                .add_options()
                ("d,drugfile", "Drug file content", cxxopts::value<std::string>())
                ("help", "Print help")
                ;


        auto result = options.parse(_argc, _argv);

        if (result.count("help") > 0)
        {
            std::cout << options.help({"", "Group"}) << std::endl;
            exit(0);
        }

        std::string drugFileContent;
        if (result.count("drugfile") > 0) {
            drugFileContent = result["drugfile"].as<std::string>();
        }
        else {
            std::cout << "The drug file content is mandatory" << std::endl << std::endl;
            std::cout << options.help({"", "Group"}) << std::endl;
            exit(0);
        }


//        Tucuxi::Common::LoggerHelper::init(logFileName);
//        Tucuxi::Common::LoggerHelper logHelper;

//        logHelper.info("Drug file : {}", drugFileName);
//        logHelper.info("Test file : {}", testsFileName);
//        logHelper.info("Log file  : {}", logFileName);

        //Scan the drug

        Tucuxi::Core::DrugModel *dModel;

        Tucuxi::Core::DrugModelImport importer;
        if (importer.importFromString(dModel, drugFileContent) != Tucuxi::Core::DrugModelImport::Result::Ok) {
            std::cout << "Can not import the drug file.\n\n" << importer.getErrorMessage() << std::endl;
            exit(3);
        }


        Tucuxi::Core::DrugModelChecker checker;

        Tucuxi::Core::PkModelCollection pkCollection;

        if (!defaultPopulate(pkCollection)) {
            std::cout << "Could not populate the Pk models collection. No model will be available" << std::endl;
            exit(2);
        }

        Tucuxi::Core::DrugModelChecker::CheckerResult_t checkerResult = checker.checkDrugModel(dModel, &pkCollection);
        if (!checkerResult.m_ok) {
            std::cout << checkerResult.m_errorMessage << std::endl;
            exit(4);
        }
        else {
            std::cout << "The drug file seems valid, and should at least give some results in Tucuxi." << std::endl;
        }

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

int main(int _argc, char** _argv)
{
    // Get application folder
    std::string appFolder = Tucuxi::Common::Utils::getAppFolder(_argv);


    auto result = parse(_argc, _argv);

    Tucuxi::Common::LoggerHelper::beforeExit();

    return 0;
}
