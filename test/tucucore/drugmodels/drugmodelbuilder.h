#ifndef DRUGMODELBUILDER_H
#define DRUGMODELBUILDER_H


namespace Tucuxi {
namespace Core {

class DrugModel;

}
}


class DrugModelBuilder
{
public:
    virtual Tucuxi::Core::DrugModel *buildDrugModel() = 0;
};

#endif // DRUGMODELBUILDER_H
