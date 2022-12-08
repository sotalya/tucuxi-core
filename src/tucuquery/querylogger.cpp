//@@license@@

#include <array>
#include <ctime>
#include <fstream>
#include <iostream>

#include "querylogger.h"

#include "tucucommon/datetime.h"
#include "tucucommon/timeofday.h"

#ifdef WIN32
#include "Windows.h"
#include "direct.h"
static const char PATH_SEPARATOR = '\\';
template<typename T>
void MKDIR(T _x)
{
    _mkdir(_x);
}
//#define MKDIR(x) _mkdir(x)
#else
static const char PATH_SEPARATOR = '/';
#include <sys/stat.h>
template<typename T>
void MKDIR(T _x)
{
    mkdir(_x, 0755);
}
//#define MKDIR(x) mkdir(x, 0755)
#endif

using namespace std;

namespace Tucuxi {
namespace Query {

static const std::string DATE_FORMAT = "%Y-%m-%dT%H:%M:%S";


Tucuxi::Common::Interface* QueryLogger::createComponent(const std::string& _folderPath)
{
    QueryLogger* cmp = new QueryLogger(_folderPath);


    return dynamic_cast<IQueryLogger*>(cmp);
}


QueryLogger::QueryLogger(const std::string& _folderPath)
{
    m_folderPath = _folderPath;
    registerInterface(dynamic_cast<IQueryLogger*>(this));
}

QueryLogger::~QueryLogger() = default;

Tucuxi::Common::Interface* QueryLogger::getInterface(const std::string& _name)
{
    return Tucuxi::Common::Component::getInterfaceImpl(_name);
}

std::string QueryLogger::getFolderPath()
{
    return m_folderPath;
}

void QueryLogger::saveQuery(const std::string& _queryString, const std::string& _queryID)
{

    time_t rawtime;
    struct tm* timeinfo;
    std::array<char, 20> buffer;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer.data(), sizeof(buffer), "%Y-%m-%d", timeinfo);
    std::string directoryPath = getFolderPath() + PATH_SEPARATOR + string(buffer.data());

    strftime(buffer.data(), sizeof(buffer), "%Y-%m-%dT%H-%M-%S", timeinfo);
    std::string fileName = string(buffer.data()) + "_" + _queryID + ".tqf";

    MKDIR(directoryPath.c_str());

    ofstream queryFile;
    queryFile.open(directoryPath + PATH_SEPARATOR + fileName);
    queryFile << _queryString;
    queryFile.close();
}

} // namespace Query
} // namespace Tucuxi
