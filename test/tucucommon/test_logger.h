
#include <sstream>
#include <string>
#include <iostream>
#include <stdio.h>
#include <regex>

#include "fructose/fructose.h"

#include "tucucommon/loggerhelper.h"

class some_class {};
std::ostream& operator<<(std::ostream& os, const some_class& c)
{ 
    return os << "some_class"; 
}

struct TestLogger : public fructose::test_base<TestLogger> 
{
    std::string m_path;

    TestLogger(std::string& _path)
    {
        m_path = _path + std::string("/MyLogFile.txt");
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

        std::string str;

        int diff = 0;

        std::getline(infile, str);
        std::cout << str << std::endl;
        diff = std::regex_match (str, std::regex("\\[[0-9\\-\\:\\.\\ ]*\\] \\[m_logger\\] \\[debug\\] Tcho les topiots"));
        fructose_assert(diff != 0);

        std::getline(infile, str);
        std::cout << str << std::endl;
        diff = std::regex_match (str, std::regex("\\[[0-9\\-\\:\\.\\ ]*\\] \\[m_logger\\] \\[info\\] Tcho les topiots"));
        fructose_assert(diff != 0);

        std::getline(infile, str);
        std::cout << str << std::endl;
        diff = std::regex_match (str, std::regex("\\[[0-9\\-\\:\\.\\ ]*\\] \\[m_logger\\] \\[warning\\] 2 \\+ 2 = 4"));
        fructose_assert(diff != 0);

        std::getline(infile, str);
        std::cout << str << std::endl;
        diff = std::regex_match (str, std::regex("\\[[0-9\\-\\:\\.\\ ]*\\] \\[m_logger\\] \\[error\\] Tcho les topiots"));
        fructose_assert(diff != 0);

        std::getline(infile, str);
        std::cout << str << std::endl;
        diff = std::regex_match (str, std::regex("\\[[0-9\\-\\:\\.\\ ]*\\] \\[m_logger\\] \\[critical\\] Tcho les topiots"));
        fructose_assert(diff != 0);
    }

    void logMyClass(const std::string& _testName)
    {
        std::cout << _testName << std::endl;

        //Tucuxi::Common::LoggerHelper logger;
        //some_class c;
        //logger.info("Logging a custom class with operator<<: {}", c);
        //logger.debug(c);

        // Todo: Check content of log file...
    }
 };
