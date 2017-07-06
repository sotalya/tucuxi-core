
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

        // Check content of log file :
        std::ifstream infile(m_path + "/LogTest2.log");
        checkLogfileLine(infile, "debug", "Tcho les topiots");
        checkLogfileLine(infile, "info", "Tcho les topiots");
        checkLogfileLine(infile, "warning", "2 + 2 = 4");
        checkLogfileLine(infile, "error", "Tcho les topiots");
        checkLogfileLine(infile, "critical", "Tcho les topiots");
    }

    void crashes(const std::string& _testName)
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
        logger3.info("asdfa {}", ((char*)0x00));
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
