#ifndef DRUGMODELIMPORT_H
#define DRUGMODELIMPORT_H

#include <string>
#include <vector>

#include "tucucommon/xmlnode.h"


namespace Tucuxi {
namespace Core {

class DrugModel;
class TimeConsiderations;
class HalfLife;
class OutdatedMeasure;
class PopulationValue;
class Unit;
class Operation;
class DrugModelDomain;
class Constraint;
class JSOperation;

class DrugModelImport
{
public:

    enum class Result {
        Ok = 0,
        Error,
        CantOpenFile
    };

    DrugModelImport();

    Result import(Tucuxi::Core::DrugModel *&_drugModel, std::string _fileName);

protected:

    Result m_result;

    DrugModel* extractDrugModel(Tucuxi::Common::XmlNodeIterator _node);
    TimeConsiderations* extractTimeConsiderations(Tucuxi::Common::XmlNodeIterator _node);
    HalfLife* extractHalfLife(Tucuxi::Common::XmlNodeIterator _node);
    OutdatedMeasure* extractOutdatedMeasure(Tucuxi::Common::XmlNodeIterator _node);
    DrugModelDomain* extractDrugDomain(Tucuxi::Common::XmlNodeIterator _node);
    std::vector<Constraint*> extractConstraints(Tucuxi::Common::XmlNodeIterator _node);
    Constraint* extractConstraint(Tucuxi::Common::XmlNodeIterator _node);

    PopulationValue* extractPopulationValue(Tucuxi::Common::XmlNodeIterator _node);
    Unit extractUnit(Tucuxi::Common::XmlNodeIterator _node);
    double extractDouble(Tucuxi::Common::XmlNodeIterator _node);
    Operation* extractOperation(Tucuxi::Common::XmlNodeIterator _node);
    JSOperation* extractJSOperation(Tucuxi::Common::XmlNodeIterator _node);


};


} // namespace Core
} // namespace Tucuxi

#endif // DRUGMODELIMPORT_H
