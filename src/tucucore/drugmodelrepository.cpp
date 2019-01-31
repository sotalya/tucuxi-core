#include "drugmodelrepository.h"

#include "tucucore/drugmodel/drugmodel.h"
#include "tucucore/drugmodelimport.h"

#include "tucucommon/loggerhelper.h"

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

void DrugModelRepository::loadFolder(std::string _folder)
{
    Tucuxi::Common::LoggerHelper logHelper;

    loadFile(_folder + "/ch.tucuxi.imatinib.tdd");
    loadFile(_folder + "/ch.tucuxi.vancomycin.tdd");
    loadFile(_folder + "/ch.tucuxi.busulfan_children.tdd");
    loadFile(_folder + "/ch.tucuxi.vancomycin_adults.tdd");
    loadFile(_folder + "/ch.tucuxi.vancomycin_adults_1CP.tdd");
    loadFile(_folder + "/ch.tucuxi.tobramycin.tdd");

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
