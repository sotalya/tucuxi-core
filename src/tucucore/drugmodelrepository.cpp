#include "drugmodelrepository.h"

#include "tucucore/drugmodel/drugmodel.h"
#include "tucucore/drugmodelimport.h"
#include "tucucore/drugmodelchecker.h"
#include "tucucore/pkmodel.h"

#include "tucucommon/loggerhelper.h"


#ifdef _WIN32
static const char PATH_SEPARATOR = '\\';
#include <windows.h>
#else
static const char PATH_SEPARATOR = '/';
#include <sys/types.h>
#include <dirent.h>
#endif

namespace Tucuxi {
namespace Core {



Tucuxi::Common::Interface* DrugModelRepository::createComponent()
{
    DrugModelRepository *cmp = new DrugModelRepository();

    return dynamic_cast<IDrugModelRepository*>(cmp);
}

DrugModelRepository::DrugModelRepository()
{
    registerInterface(dynamic_cast<IDrugModelRepository*>(this));
}

DrugModelRepository::~DrugModelRepository()
{
    for (auto drugModel : m_drugModels) {
        delete drugModel;
    }
}

Tucuxi::Common::Interface* DrugModelRepository::getInterface(const std::string &_name)
{
    return Tucuxi::Common::Component::getInterfaceImpl(_name);
}


void DrugModelRepository::addFolderPath(const std::string& _folderPath)
{

    m_drugPaths.push_back(_folderPath);

}


DrugModel* DrugModelRepository::loadFile(const std::string& _fileName)
{
    Tucuxi::Common::LoggerHelper logHelper;

    DrugModel *drugModel;
    Tucuxi::Core::DrugModelImport drugModelImport;

    if (drugModelImport.importFromFile(drugModel, _fileName) == DrugModelImport::Status::Ok) {

        PkModelCollection pkCollection;

        if (!defaultPopulate(pkCollection)) {
            logHelper.error("Could not populate the Pk models collection. No model will be available");
//            return;
        }

        DrugModelChecker checker;
        DrugModelChecker::CheckerResult_t checkerResult = checker.checkDrugModel(drugModel, &pkCollection);
        if (!checkerResult.m_ok) {
            logHelper.error("The drug file {} has internal errors : {}", _fileName, checkerResult.m_errorMessage);
            return nullptr;
        }
        else {
            logHelper.info("Successfully loaded drug model : {}", drugModel->getDrugModelId());
            addDrugModel(drugModel);
        }
    }
    else {
        logHelper.error("Cannot import a drug file : {}", _fileName);
        logHelper.error("{} ", drugModelImport.getErrorMessage());
        return nullptr;
    }
    return drugModel;
}

// Solution taken here: http://www.martinbroadhurst.com/list-the-files-in-a-directory-in-c.html
#ifdef _WIN32
void read_directory(const std::string& name, std::vector<std::string>& v)
{
    std::string pattern(name);
    pattern.append("\\*");
    wchar_t winPattern[1000];
    mbstowcs(winPattern, pattern.c_str(), strlen(pattern.c_str()) + 1);
    WIN32_FIND_DATA data;
    HANDLE hFind;
    if ((hFind = FindFirstFile(winPattern, &data)) != INVALID_HANDLE_VALUE) {
        do {
            char ch[260];
            char DefChar = ' ';
            WideCharToMultiByte(CP_ACP,0,data.cFileName,-1, ch,260,&DefChar, NULL);
            v.push_back(ch);
        } while (FindNextFile(hFind, &data) != 0);
        FindClose(hFind);
    }
}
#else
void read_directory(const std::string& _name, std::vector<std::string>& _v)
{
    DIR* dirp = opendir(_name.c_str());
    if (dirp == nullptr) {
        return;
    }
    struct dirent * dp;
    while ((dp = readdir(dirp)) != nullptr) {
        _v.push_back(dp->d_name);
    }
    closedir(dirp);
}
#endif // _WIN32

void DrugModelRepository::loadFolder(const std::string& _folder)
{
    Tucuxi::Common::LoggerHelper logHelper;

    std::vector<std::string> list;

    read_directory(_folder, list);

    for (const auto& filename : list) {
        if (filename.substr((filename.size() > 4) ? (filename.size() - 4) : 0, filename.size() - 1) == ".tdd") {
            loadFile(_folder + "/" + filename);
        }
    }
}

void DrugModelRepository::addDrugModel(DrugModel *_drugModel)
{
    m_drugModels.push_back(_drugModel);
}

DrugModel *DrugModelRepository::getDrugModelById(std::string _drugModelId)
{

    for (auto drugModel : m_drugModels) {
        if (drugModel->getDrugModelId() == _drugModelId) {
            return drugModel;
        }
    }

    for (const auto& drugpath : m_drugPaths){
        DrugModel *drugModel = loadFile(drugpath + PATH_SEPARATOR + _drugModelId + ".tdd");
        if(drugModel != nullptr)
        {
            return drugModel;
        }
    }

    for (const auto& drugpath : m_drugPaths){
        loadFolder(drugpath);
    }

    for (auto drugModel : m_drugModels) {
        if (drugModel->getDrugModelId() == _drugModelId) {
            return drugModel;
        }
    }
    return nullptr;
}

std::vector<DrugModel *> DrugModelRepository::getDrugModelsByDrugId(std::string _drugId)
{
    static const int INDEX_POSITION = 10;

    bool drugModelExist = false;
    std::vector<DrugModel* > drugModels;
    for (auto drugModel : m_drugModels) {
        if (drugModel->getDrugId() == _drugId) {
            drugModels.push_back(drugModel);
            drugModelExist = true;
        }
    }

    if(drugModelExist) {return drugModels;}

    std::vector<std::string> list;
    for (const auto& drugpath : m_drugPaths){
        read_directory(drugpath, list);
        for (const auto& filename : list) {
            if (filename.substr((filename.size() > 4) ? (filename.size() - 4) : 0, filename.size() - 1) == ".tdd") {

                if(filename.substr(INDEX_POSITION, _drugId.length()) == _drugId)
                {
                    drugModels.push_back(loadFile(drugpath + "/" + filename));
                }
            }
        }
    }
    return drugModels;

}

} // namespace Core
} // namespace Tucuxi
