#include "drugmodelrepository.h"

#include "tucucore/drugmodel/drugmodel.h"
#include "tucucore/drugmodelimport.h"

#include "tucucommon/loggerhelper.h"


#ifdef _WIN32
#include <windows.h>
#else
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

Tucuxi::Common::Interface* DrugModelRepository::getInterface(const std::string &_name)
{
    return Tucuxi::Common::Component::getInterfaceImpl(_name);
}

void DrugModelRepository::loadFile(std::string _fileName)
{
    Tucuxi::Common::LoggerHelper logHelper;

    DrugModel *drugModel;
    Tucuxi::Core::DrugModelImport drugModelImport;

    if (drugModelImport.importFromFile(drugModel, _fileName) == DrugModelImport::Result::Ok) {
        logHelper.info("Successfully loaded drug model : {}", drugModel->getDrugModelId());
        addDrugModel(drugModel);
    }
    else {
        logHelper.error("Cannot import a drug file : {}", _fileName);
    }
}

// Solution taken here: http://www.martinbroadhurst.com/list-the-files-in-a-directory-in-c.html
#ifdef _WIN32
void read_directory(const std::string& name, std::vector<std::string>& v)
{
    std::string pattern(name);
    pattern.append("\\*");
    WIN32_FIND_DATA data;
    HANDLE hFind;
    if ((hFind = FindFirstFile(pattern.c_str(), &data)) != INVALID_HANDLE_VALUE) {
        do {
            v.push_back(data.cFileName);
        } while (FindNextFile(hFind, &data) != 0);
        FindClose(hFind);
    }
}
#else
void read_directory(const std::string& name, std::vector<std::string>& v)
{
    DIR* dirp = opendir(name.c_str());
    struct dirent * dp;
    while ((dp = readdir(dirp)) != NULL) {
        v.push_back(dp->d_name);
    }
    closedir(dirp);
}
#endif // _WIN32

void DrugModelRepository::loadFolder(std::string _folder)
{
    Tucuxi::Common::LoggerHelper logHelper;

    std::vector<std::string> list;

    read_directory(_folder, list);

    for (auto filename : list) {
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
    return nullptr;
}

std::vector<DrugModel *> DrugModelRepository::getDrugModelsByDrugId(std::string _drugId)
{
    std::vector<DrugModel* > drugModels;
    for (auto drugModel : m_drugModels) {
        if (drugModel->getDrugId() == _drugId) {
            drugModels.push_back(drugModel);
        }
    }
    return drugModels;
}

} // namespace Core
} // namespace Tucuxi
