#include "querylogger.h"
#include "tucucommon/datetime.h"
#include "ctime"
#include "fstream"
#include "iostream"

#ifdef WIN32
#include "Windows.h"
#include "direct.h"
const char PATH_SEPARATOR = '\\';
//rc(filename) = _wmkdir(filename);
#else
const char PATH_SEPARATOR = '/';
#include <sys/stat.h>
#endif

using namespace std;

namespace Tucuxi {
namespace Query {

static const std::string DATE_FORMAT = "%Y-%m-%dT%H:%M:%S";


Tucuxi::Common::Interface* QueryLogger::createComponent(std::string _folderPath)
{
    QueryLogger *cmp = new QueryLogger(_folderPath);

    return dynamic_cast<IQueryLogger*>(cmp);
}


QueryLogger::QueryLogger(std::string _folderPath)
{
    m_folderPath = _folderPath;
    registerInterface(dynamic_cast<IQueryLogger*>(this));
}

QueryLogger::~QueryLogger()
{

}

Tucuxi::Common::Interface* QueryLogger::getInterface(const std::string &_name)
{
    return Tucuxi::Common::Component::getInterfaceImpl(_name);
}

std::string QueryLogger::getFolderPath(void)
{
    return m_folderPath;
}

void QueryLogger::saveQuery(std::string _queryString, std::string _queryID)
{
        Tucuxi::Common::DateTime date;

    time_t rawtime;
    struct tm * timeinfo;
    char buffer[20];

    time (&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer,sizeof(buffer),"%Y-%m-%d",timeinfo);
    std::string folderName(buffer);

    std::string directoryPath = getFolderPath() + PATH_SEPARATOR +  folderName;

    strftime(buffer,sizeof(buffer),"%Y-%m-%dT%H-%M-%S",timeinfo);
    std::string filedate(buffer);
    std::string fileName = filedate + "_" + _queryID + ".tqf";


    #ifdef WIN32
        _mkdir(directoryPath.c_str());
    #else
        mkdir(directoryPath.c_str(), 0755);
    #endif

    std::string filePath = directoryPath + PATH_SEPARATOR + fileName;

    ofstream queryFile;
    queryFile.open(filePath);
    queryFile << _queryString;
    queryFile.close();

}

} // namespace Query
} // namespace Tucuxi
