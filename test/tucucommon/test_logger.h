
#include "fructose/fructose.h"
#include "tucucommon/logger.h"

#include <sstream>
#include <string>
#include <iostream>
#include <stdio.h>
#include <regex>

class some_class {};
std::ostream& operator<<(std::ostream& os, const some_class& c)
{ 
    return os << "some_class"; 
}

struct TestLogger : public fructose::test_base<TestLogger> 
{
    void basic(const std::string& _testName)
    {
        std::cout << _testName << std::endl;

        Tucuxi::Common::Logger logger;
        logger.debug("Tcho les topiots");
//        logger.info("Tcho les {}", "topiots");
//        logger.warn("{} + {} = {}", 2, 2, 4);
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

    void logMyClass(const std::string& _testName)
    {
        std::cout << _testName << std::endl;

        Tucuxi::Common::Logger logger;
        some_class c;
//        logger.info("Logging a custom class with operator<<: {}", c);

        // Todo: Check content of log file...
    }
 };
