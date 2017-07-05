
#include <sstream>
#include <string>
#include <iostream>
#include <stdio.h>
#include <cstdio>
#include <regex>

#include "fructose/fructose.h"

#include "tucucommon/loggerhelper.h"
#include "tucucommon/componentmanager.h"

struct TestLogger : public fructose::test_base<TestLogger> 
{
    std::string m_path;

    TestLogger(std::string& _path)
    {
        m_path = _path + std::string("/MyLogFile.txt");

        // Make sure to remove the existing rotating file...
        std::remove(m_path.c_str());

        Tucuxi::Common::LoggerHelper::init(m_path);
    }

    void basic(const std::string& _testName)
    {
        std::cout << _testName << std::endl;

        Tucuxi::Common::LoggerHelper logger;
        logger.debug("Tcho les topiots");
        logger.info("Tcho les {}", "topiots");
        logger.warn("{} + {} = {}", 2, 2, 4);
        logger.error("Tcho les topiots");
        logger.critical("Tcho les topiots");

        // Check content of log file :
        std::ifstream infile(m_path);
        checkLogfileLine(infile, "debug", "Tcho les topiots");
        checkLogfileLine(infile, "info", "Tcho les topiots");
        checkLogfileLine(infile, "warning", "2 + 2 = 4");
        checkLogfileLine(infile, "error", "Tcho les topiots");
        checkLogfileLine(infile, "critical", "Tcho les topiots");
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

private:
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
