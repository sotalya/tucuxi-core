#ifndef DRUGMODELIMPORT_H
#define DRUGMODELIMPORT_H

#include <string>
#include <vector>

#include "tucucommon/xmlnode.h"
#include "tucucore/definitions.h"


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
class CovariateDefinition;
enum class CovariateType;
enum class DataType;
enum class InterpolationType;

class LightPopulationValue
{
public:
    LightPopulationValue() {}

    Operation* getOperation() { return m_operation;}
    Value getValue() { return m_value;}

    Operation *m_operation;
    Value m_value;
};


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

    std::vector<CovariateDefinition*> extractCovariates(Tucuxi::Common::XmlNodeIterator _node);
    CovariateDefinition* extractCovariate(Tucuxi::Common::XmlNodeIterator _node);

    LightPopulationValue* extractPopulationValue(Tucuxi::Common::XmlNodeIterator _node);
    Unit extractUnit(Tucuxi::Common::XmlNodeIterator _node);
    double extractDouble(Tucuxi::Common::XmlNodeIterator _node);
    Operation* extractOperation(Tucuxi::Common::XmlNodeIterator _node);
    JSOperation* extractJSOperation(Tucuxi::Common::XmlNodeIterator _node);


    CovariateType extractCovariateType(Tucuxi::Common::XmlNodeIterator _node);
    DataType extractDataType(Tucuxi::Common::XmlNodeIterator _node);
    InterpolationType extractInterpolationType(Tucuxi::Common::XmlNodeIterator _node);


};


} // namespace Core
} // namespace Tucuxi

#endif // DRUGMODELIMPORT_H
