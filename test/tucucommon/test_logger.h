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


#include <cstdio>
#include <iostream>
#include <regex>
#include <sstream>
#include <stdio.h>
#include <string>

#include "tucucommon/componentmanager.h"
#include "tucucommon/loggerhelper.h"
#include "tucucommon/utils.h"

#include "fructose/fructose.h"

struct TestLogger : public fructose::test_base<TestLogger>
{
    std::string m_path;

    TestLogger(std::string& _path)
    {
        m_path = _path;
    }

    void basic(const std::string& _testName)
    {
        std::cout << _testName << std::endl;

        reset("LogTest2.log");
        Tucuxi::Common::LoggerHelper logger;
        logger.debug("Tcho les topiots");
        logger.info("Tcho les {}", "topiots");
        logger.warn("{} + {} = {}", 2, 2, 4);
        logger.error("Tcho les topiots");
        logger.critical("Tcho les topiots");
        logger.critical("{}", std::string("Tcho les topiots"));

        // Check content of log file :
        std::ifstream infile(m_path + "/LogTest2.log");
        checkLogfileLine(infile, "debug", "Tcho les topiots");
        checkLogfileLine(infile, "info", "Tcho les topiots");
        checkLogfileLine(infile, "warning", "2 + 2 = 4");
        checkLogfileLine(infile, "error", "Tcho les topiots");
        checkLogfileLine(infile, "critical", "Tcho les topiots");
        checkLogfileLine(infile, "critical", "Tcho les topiots");
    }

    void crashes(const std::string& /*_testName*/)
    {
        // Work with an uninitialized logger
        Tucuxi::Common::LoggerHelper logger;
        logger.info("asdfa");

        // Initialize with a bad file path
        Tucuxi::Common::LoggerHelper::init("");
        Tucuxi::Common::LoggerHelper logger2;
        logger2.info("asdfa");


        // Try to make our LoggerHelper crash
        reset("LogTest1.log");
        Tucuxi::Common::LoggerHelper logger3;
        logger3.info("asdfa {}");
        logger3.info("asdfa {}", 12, 22);
        logger3.info("asdfa {}", (static_cast<char*>(0x00)));
    }

private:
    void reset(const char* _fileName)
    {
        // Remove the current logger (hack for test only)
        Tucuxi::Common::ComponentManager::getInstance()->unregisterComponent("Logger");

        // Make sure to remove the existing rotating file...
        const std::string& fileName = Tucuxi::Common::Utils::strFormat("%s/%s", m_path.c_str(), _fileName);
        std::remove(fileName.c_str());

        // Initialize the logger correctly
        Tucuxi::Common::LoggerHelper::init(fileName);
    }

    void checkLogfileLine(std::ifstream& _file, const char* _level, const char* _msg)
    {
        static const char* patternFormat = "\\[[0-9\\-\\:\\.\\ ]*\\] [0-9]* ([a-z]*): (.*)";
        std::string line;
        std::getline(_file, line);
        //std::cout << line << std::endl;

        std::string pattern = Tucuxi::Common::Utils::strFormat(patternFormat, _level, _msg);
        std::regex rgx(pattern);
        std::smatch match;

        bool isMatching = std::regex_search(line, match, rgx);
        fructose_assert(isMatching);
        if (isMatching) {
            fructose_assert(match[1].str() == _level);
            fructose_assert(match[2].str() == _msg);
        }
    }
};
