#ifndef TUCUXI_CORE_DRUGMODELREPOSITORY_H
#define TUCUXI_CORE_DRUGMODELREPOSITORY_H

#include <string>
#include <vector>

#include "tucucommon/component.h"

namespace Tucuxi {
namespace Core {

class DrugModel;

class IDrugModelRepository : public Tucuxi::Common::Interface
{
public:

    virtual ~IDrugModelRepository() = default;

    virtual DrugModel *getDrugModelById(std::string _drugModelId) = 0;

    virtual std::vector<DrugModel *> getDrugModelsByDrugId(std::string _drugId) = 0;

};

class DrugModelRepository : public Tucuxi::Common::Component,
        public IDrugModelRepository
{
public:
    static Tucuxi::Common::Interface* createComponent();

    DrugModelRepository();

    //bool initialize() override;

    void loadFolder(std::string _folder);

    void loadFile(std::string _fileName);

    void addDrugModel(DrugModel *_drugModel);

    DrugModel *getDrugModelById(std::string _drugModelId) override;

    std::vector<DrugModel *> getDrugModelsByDrugId(std::string _drugId) override;

protected:

    std::vector<DrugModel *> m_drugModels;

    /// \brief Access other interfaces of the same component.
    Tucuxi::Common::Interface* getInterface(const std::string &_name) override;

};


} // namespace Core
} // namespace Tucuxi


#endif // TUCUXI_CORE_DRUGMODELREPOSITORY_H
