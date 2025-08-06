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


#include <iostream>

#include "tucucommon/loggerhelper.h"
#include "tucucommon/utils.h"

#include "tucucore/drugmodelrepository.h"

#include "tucuquery/querylogger.h"

#include "clicomputer.h"
#include "cxxopts.hpp"

#ifdef CONFIG_SIGN
#include "tucusign/signparser.h"
#include "tucusign/signvalidator.h"
#endif // CONFIG_SIGN

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
                "t,tqfoutput", "Path of a copy of the query file, for testing purpose", cxxopts::value<std::string>())(
                "csv", "Data file (.dat) folder path", cxxopts::value<std::string>())
#ifdef CONFIG_SIGN
                ("s,signature", "Signed drug file path", cxxopts::value<std::string>())
#endif // CONFIG_SIGN
                        ("help", "Print help");


        auto result = options.parse(_argc, _argv);

#ifdef CONFIG_SIGN
        if (result.count("signature") > 0) {
            std::string signedDrugfilePath = result["signature"].as<std::string>();
            Tucuxi::Sign::Signature signature;
            // load signature from drug file
            Tucuxi::Sign::ParsingError parsingResponse =
                    Tucuxi::Sign::SignParser::loadSignature(signedDrugfilePath, signature);

            if (parsingResponse == Tucuxi::Sign::ParsingError::SIGNATURE_OK) {
                // validate signature
                Tucuxi::Sign::SignatureError signatureResponse =
                        Tucuxi::Sign::SignValidator::validateSignature(signature);

                if (signatureResponse == Tucuxi::Sign::SignatureError::SIGNATURE_VALID) {
                    // print signer info
                    std::cout << "\nThe drug file has been signed by: \n"
                              << Tucuxi::Sign::SignValidator::loadSigner(signature.getUserCert()) << std::endl;
                }
            }
            exit(0);
        }
#endif // CONFIG_SIGN

        if (result.count("help") > 0) {
            std::cout << options.help({"", "Group"}) << '\n';
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
            std::cout << "The input file is mandatory" << '\n' << '\n';
            std::cout << options.help({"", "Group"}) << '\n';
            exit(-2);
        }

        std::string outputFileName;
        if (result.count("output") > 0) {
            outputFileName = result["output"].as<std::string>();
        }
        else {
            std::cout << "The output file is mandatory" << '\n' << '\n';
            std::cout << options.help({"", "Group"}) << '\n';
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

        std::string tqfoutputfilepath;
        if (result.count("tqfoutput") > 0) {
            tqfoutputfilepath = result["tqfoutput"].as<std::string>();
        }

        logHelper.info("Drugs directory : {}", drugPath);
        logHelper.info("Input file : {}", inputFileName);
        logHelper.info("Output file name : {}", outputFileName);
        logHelper.info("QueryLogs directory : {}", queryLogPath);
        logHelper.info("Data file directory : {}", datafilepath);
        logHelper.info("Tqf copy output file : {}", tqfoutputfilepath);

        Tucuxi::Common::ComponentManager* pCmpMgr = Tucuxi::Common::ComponentManager::getInstance();



        auto drugModelRepository =
                dynamic_cast<Tucuxi::Core::DrugModelRepository*>(Tucuxi::Core::DrugModelRepository::createComponent());

        pCmpMgr->registerComponent("DrugModelRepository", drugModelRepository);

        drugModelRepository->addFolderPath(drugPath);

        auto* queryLogger =
                dynamic_cast<Tucuxi::Query::QueryLogger*>(Tucuxi::Query::QueryLogger::createComponent(queryLogPath));

        pCmpMgr->registerComponent("QueryLogger", queryLogger);

        CliComputer computer;
        auto queryStatus = computer.compute(inputFileName, outputFileName, datafilepath, tqfoutputfilepath);


        pCmpMgr->unregisterComponent("DrugModelRepository");
        pCmpMgr->unregisterComponent("QueryLogger");


        delete drugModelRepository;

        delete queryLogger;

        return static_cast<int>(queryStatus);
    }
    catch (const cxxopts::exceptions::exception& e) {
        logHelper.error("error parsing options: {}", e.what());
        return -1;
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
