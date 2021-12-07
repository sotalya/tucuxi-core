#ifndef DRUGMODELBUILDER_H
#define DRUGMODELBUILDER_H

#include <memory>

namespace Tucuxi {
namespace Core {

class DrugModel;

} // namespace Core
} // namespace Tucuxi


class DrugModelBuilder
{
public:
    virtual std::unique_ptr<Tucuxi::Core::DrugModel> buildDrugModel() = 0;
};

#endif // DRUGMODELBUILDER_H
