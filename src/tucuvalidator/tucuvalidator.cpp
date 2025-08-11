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


#include "tucucommon/general.h"
#include "tucucommon/loggerhelper.h"
#include "tucucommon/utils.h"

#include "tucucore/drugfilevalidator.h"

#include "cxxopts.hpp"

cxxopts::ParseResult parse(int _argc, char* _argv[])
{
    // Get application folder
    std::string appFolder = Tucuxi::Common::Utils::getAppFolder(_argv);


    try {

        cxxopts::Options options(_argv[0], " - Tucuxi drug file validator");
        options.positional_help("[optional args]").show_positional_help();

        // clang-format off
        options.allow_unrecognised_options()
                .add_options()
                ("d,drugfile", "Drug file", cxxopts::value<std::string>())
                ("t,testfile", "Tests to be conducted", cxxopts::value<std::string>())
                ("l,logfile", "Log file", cxxopts::value<std::string>())
                ("help", "Print help")
                ;
        // clang-format on

        auto result = options.parse(_argc, _argv);

        if (result.count("help") > 0) {
            std::cout << options.help({"", "Group"}) << std::endl;
            exit(0);
        }

        std::string drugFileName;
        if (result.count("drugfile") > 0) {
            drugFileName = result["drugfile"].as<std::string>();
        }
        else {
            std::cout << "The drug file is mandatory" << std::endl << std::endl;
            std::cout << options.help({"", "Group"}) << std::endl;
            exit(0);
        }

        std::string testsFileName;
        if (result.count("testfile") > 0) {
            testsFileName = result["testfile"].as<std::string>();
        }
        else {
            std::cout << "The test file is mandatory" << std::endl << std::endl;
            std::cout << options.help({"", "Group"}) << std::endl;
            exit(0);
        }

        std::string logFileName;
        if (result.count("logfile") > 0) {
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
