
#include <sstream>
#include <string>
#include <iostream>
#include <stdio.h>
#include <regex>

#include "fructose/fructose.h"

#include "tucucommon/loggerhelper.h"
#include "tucucommon/componentmanager.h"

struct TestLogger : public fructose::test_base<TestLogger> 
{
    void basic(const std::string& _testName)
    {
        std::cout << _testName << std::endl;

        Tucuxi::Common::LoggerHelper::init("/home/mylag/reds/Tucuxi/tucuxi/bin/MyLogFile.txt");

        Tucuxi::Common::LoggerHelper logger;
        logger.debug("Tcho les topiots");
        logger.info("Tcho les {}", "topiots");
        logger.warn("{} + {} = {}", 2, 2, 4);
        logger.error("Tcho les topiots");
        logger.critical("Tcho les topiots");

        // Check content of log file :
        std::ifstream infile("/home/mylag/reds/Tucuxi/tucuxi/bin/MyLogFile.txt");

        std::string str;

        int diff = 0;

        std::getline(infile, str);
        diff = std::regex_match (str, std::regex("\\[[0-9\\-\\:\\.\\ ]*\\] \\[m_logger\\] \\[debug\\] Tcho les topiots"));
        fructose_assert(diff != 0);

        std::getline(infile, str);
        diff = std::regex_match (str, std::regex("\\[[0-9\\-\\:\\.\\ ]*\\] \\[m_logger\\] \\[error\\] Tcho les topiots"));
        fructose_assert(diff != 0);

        std::getline(infile, str);
        diff = std::regex_match (str, std::regex("\\[[0-9\\-\\:\\.\\ ]*\\] \\[m_logger\\] \\[critical\\] Tcho les topiots"));
        fructose_assert(diff != 0);
    }

    void crashes(const std::string& _testName)
    {
        // Try to make our LoggerHelper crash
        Tucuxi::Common::LoggerHelper::init("c:/temp/test.log");
        Tucuxi::Common::LoggerHelper logger;
        logger.info("asdfa {}");
        logger.info("asdfa {}", 12, 22);
        logger.info("asdfa {}", ((char*)0x00));

        // Re-initialize our Logger component
        Tucuxi::Common::ComponentManager::getInstance()->unregisterComponent("Logger");
        Tucuxi::Common::LoggerHelper::init("");
        logger.info("asdfa");
    }
 };
