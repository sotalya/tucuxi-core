//@@license@@

#include <regex>
#include <fstream>
#include <string>

#include <gtest/gtest.h>

#include "tucucommon/componentmanager.h"
#include "tucucommon/loggerhelper.h"
#include "tucucommon/utils.h"

extern std::string appFolder;

static void reset(const char* _fileName, std::string m_path)
{
    // Remove the current logger (hack for test only)
    Tucuxi::Common::ComponentManager::getInstance()->unregisterComponent("Logger");

    // Make sure to remove the existing rotating file...
    const std::string& fileName = Tucuxi::Common::Utils::strFormat("%s/%s", m_path.c_str(), _fileName);
    std::remove(fileName.c_str());

    // Initialize the logger correctly
    Tucuxi::Common::LoggerHelper::init(fileName);
}

static void checkLogfileLine(std::ifstream& _file, const char* _level, const char* _msg)
{
    static const char* patternFormat = "\\[[0-9\\-\\:\\.\\ ]*\\] [0-9]* ([a-z]*): (.*)";
    std::string line;
    std::getline(_file, line);
    //std::cout << line << std::endl;

    std::string pattern = Tucuxi::Common::Utils::strFormat(patternFormat, _level, _msg);
    std::regex rgx(pattern);
    std::smatch match;

    bool isMatching = std::regex_search(line, match, rgx);
    EXPECT_TRUE(isMatching);
    if (isMatching) {
        EXPECT_EQ(match[1].str(), _level);
        EXPECT_EQ(match[2].str(), _msg);
    }
}

TEST (Common_LoggerTest, Basic){
    reset("LogTest2.log", appFolder);
    Tucuxi::Common::LoggerHelper logger;
    logger.debug("Tcho les topiots");
    logger.info("Tcho les {}", "topiots");
    logger.warn("{} + {} = {}", 2, 2, 4);
    logger.error("Tcho les topiots");
    logger.critical("Tcho les topiots");
    logger.critical("{}", std::string("Tcho les topiots"));

    // Check content of log file :
    std::ifstream infile(appFolder + "/LogTest2.log");
    checkLogfileLine(infile, "debug", "Tcho les topiots");
    checkLogfileLine(infile, "info", "Tcho les topiots");
    checkLogfileLine(infile, "warning", "2 + 2 = 4");
    checkLogfileLine(infile, "error", "Tcho les topiots");
    checkLogfileLine(infile, "critical", "Tcho les topiots");
    checkLogfileLine(infile, "critical", "Tcho les topiots");
}

TEST (Common_LoggerTest, Crashes){
    // Work with an uninitialized logger
    Tucuxi::Common::LoggerHelper logger;
    logger.info("asdfa");

    // Initialize with a bad file path
    Tucuxi::Common::LoggerHelper::init("");
    Tucuxi::Common::LoggerHelper logger2;
    logger2.info("asdfa");


    // Try to make our LoggerHelper crash
    reset("LogTest1.log", appFolder);
    Tucuxi::Common::LoggerHelper logger3;
    logger3.info("asdfa {}");
    logger3.info("asdfa {}", 12, 22);
    logger3.info("asdfa {}", (static_cast<char*>(0x00)));
}
