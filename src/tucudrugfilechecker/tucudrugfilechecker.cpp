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


#include "tucucommon/loggerhelper.h"
#include "tucucommon/utils.h"

#include "tucucore/drugmodel/drugmodel.h"
#include "tucucore/drugmodelchecker.h"
#include "tucucore/drugmodelimport.h"
#include "tucucore/overloadevaluator.h"
#include "tucucore/pkmodel.h"

#include "cxxopts.hpp"

cxxopts::ParseResult parse(int _argc, char* _argv[])
{
    // Get application folder
    std::string appFolder = Tucuxi::Common::Utils::getAppFolder(_argv);

    // These values are identical as the ones used in Tucuxi-gui
    Tucuxi::Core::SingleOverloadEvaluator::getInstance()->setValues(200000, 10000, 10000);


    try {

        cxxopts::Options options(_argv[0], " - Tucuxi drug file checker");

        // clang-format off
        options
                .positional_help("[optional args]")
                .show_positional_help();

        options
                .allow_unrecognised_options()
                .add_options()
                ("d,drugfilecontent", "Drug file content", cxxopts::value<std::string>())
                ("f,drugfilepath", "Drug file path", cxxopts::value<std::string>())
                ("c,operationfilecontent", "Operation content", cxxopts::value<std::string>())
                ("p,operationfilepath", "Operation file path", cxxopts::value<std::string>())
                ("help", "Print help")
                ;
        // clang-format on

        auto result = options.parse(_argc, _argv);

        if (result.count("help") > 0) {
            std::cout << options.help({"", "Group"}) << '\n';
            exit(0);
        }

        std::string drugFileContent;
        std::string drugFilePath;
        std::string operationFileContent;
        std::string operationFilePath;
        bool checkDrugFile = false;
        if (result.count("drugfilecontent") > 0) {
            drugFileContent = result["drugfilecontent"].as<std::string>();
            checkDrugFile = true;
        }
        else if (result.count("drugfilepath") > 0) {
            drugFilePath = result["drugfilepath"].as<std::string>();
            checkDrugFile = true;
        }
        else if (result.count("operationfilecontent") > 0) {
            operationFileContent = result["operationfilecontent"].as<std::string>();
        }
        else if (result.count("operationfilepath") > 0) {
            operationFilePath = result["operationfilepath"].as<std::string>();
        }
        else {
            std::cout << "The drug file content of path is mandatory" << '\n' << '\n';
            std::cout << options.help({"", "Group"}) << '\n';
            exit(0);
        }




        //        Tucuxi::Common::LoggerHelper::init(logFileName);
        //        Tucuxi::Common::LoggerHelper logHelper;

        //        logHelper.info("Drug file : {}", drugFileName);
        //        logHelper.info("Test file : {}", testsFileName);
        //        logHelper.info("Log file  : {}", logFileName);

        if (checkDrugFile) {
            //Scan the drug

            std::unique_ptr<Tucuxi::Core::DrugModel> dModel;

            Tucuxi::Core::DrugModelImport importer;

            if (drugFileContent.empty()) {
                if (importer.importFromFile(dModel, drugFilePath) != Tucuxi::Core::DrugModelImport::Status::Ok) {
                    std::cout << "Can not import the drug file.\n\n" << importer.getErrorMessage() << '\n';
                    exit(3);
                }
            }
            else {
                if (importer.importFromString(dModel, drugFileContent) != Tucuxi::Core::DrugModelImport::Status::Ok) {
                    std::cout << "Can not import the drug file content.\n\n" << importer.getErrorMessage() << '\n';
                    exit(3);
                }
            }




            Tucuxi::Core::DrugModelChecker checker;

            Tucuxi::Core::PkModelCollection pkCollection;

            if (!defaultPopulate(pkCollection)) {
                std::cout << "Could not populate the Pk models collection. No model will be available" << '\n';
                exit(2);
            }

            Tucuxi::Core::DrugModelChecker::CheckerResult_t checkerResult =
                    checker.checkDrugModel(dModel.get(), &pkCollection);
            if (!checkerResult.m_ok) {
                std::cout << checkerResult.m_errorMessage << '\n';
                exit(4);
            }
            else {
                std::cout << "The drug file seems valid, and should at least give some results in Tucuxi." << '\n';
            }
        }
        else {
            // We deal with an operation

            Tucuxi::Core::DrugModelImport importer;
            std::unique_ptr<Tucuxi::Core::Operation> operation;
            if (operationFileContent.empty()) {
                if (importer.importOperationFromFile(operation, operationFilePath)
                    != Tucuxi::Core::DrugModelImport::Status::Ok) {
                    std::cout << "Can not import the operation file.\n\n" << importer.getErrorMessage() << '\n';
                    exit(5);
                }
            }
            else {
                if (importer.importOperationFromString(operation, operationFileContent)
                    != Tucuxi::Core::DrugModelImport::Status::Ok) {
                    std::cout << "Can not import the operation content.\n\n" << importer.getErrorMessage() << '\n';
                    exit(5);
                }
            }

            Tucuxi::Core::DrugModelChecker checker;
            Tucuxi::Core::DrugModelChecker::CheckerResult_t checkerResult = checker.checkOperation(operation.get());
            if (!checkerResult.m_ok) {
                std::cout << checkerResult.m_errorMessage << '\n';
                exit(6);
            }
            else {
                std::cout << "The operation file seems valid, and should at least give some results in Tucuxi." << '\n';
            }
        }

        return result;
    }
    catch (const cxxopts::exceptions::exception& e) {
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
