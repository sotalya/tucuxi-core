#ifndef DRUGMODELIMPORT_H
#define DRUGMODELIMPORT_H

#include <string>
#include <vector>

#include "tucucommon/xmlnode.h"
#include "tucucore/definitions.h"
#include "tucucore/residualerrormodel.h"
#include "tucucore/drugmodel/parameterdefinition.h"
#include "tucucore/drugmodel/formulationandroute.h"


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
class ActiveMoiety;
class TargetDefinition;
class AnalyteSet;
class Analyte;
class MolarMass;
class IResidualErrorModel;
class ParameterSetDefinition;
class ParameterDefinition;
class Correlation;
class FullFormulationAndRoute;
class FormulationAndRoutes;

enum class CovariateType;
enum class DataType;
enum class InterpolationType;
enum class TargetType;

class LightPopulationValue
{
public:
    LightPopulationValue() : m_operation(nullptr), m_value(0.0) {}

    Operation* getOperation() { return m_operation;}
    Value getValue() { return m_value;}

    Operation *m_operation;
    Value m_value;
};

class IImport
{
public:
    enum class Result {
        Ok = 0,
        Error,
        CantOpenFile
    };

protected:

    void setResult(Result _result) {
        // Totally unuseful test, bug good to add a breakpoint in the else during debugging
        if (_result == Result::Ok) {
            m_result = _result;
        }
        else {
            m_result = _result;
        }
    }

    void unexpectedTag(std::string tagName) {
        std::vector<std::string> unused = {"comments", "description", "errorMessage", "name", "activeMoietyName"};
        for(const auto & s : unused) {
            if (s == tagName) {
                return;
            }
        }
        std::cout << "Unexpected tag" << std::endl;
    }

    Result getResult() const { return m_result;}

private:

    Result m_result;
};

class DrugModelImport : public IImport
{
public:


    DrugModelImport();

    Result import(Tucuxi::Core::DrugModel *&_drugModel, std::string _fileName);

protected:


    DrugModel* extractDrugModel(Tucuxi::Common::XmlNodeIterator _node);
    TimeConsiderations* extractTimeConsiderations(Tucuxi::Common::XmlNodeIterator _node);
    HalfLife* extractHalfLife(Tucuxi::Common::XmlNodeIterator _node);
    OutdatedMeasure* extractOutdatedMeasure(Tucuxi::Common::XmlNodeIterator _node);
    DrugModelDomain* extractDrugDomain(Tucuxi::Common::XmlNodeIterator _node);
    std::vector<Constraint*> extractConstraints(Tucuxi::Common::XmlNodeIterator _node);
    Constraint* extractConstraint(Tucuxi::Common::XmlNodeIterator _node);
    std::vector<ActiveMoiety*> extractActiveMoieties(Tucuxi::Common::XmlNodeIterator _node);
    ActiveMoiety* extractActiveMoiety(Tucuxi::Common::XmlNodeIterator _node);
    std::vector<TargetDefinition*> extractTargets(Tucuxi::Common::XmlNodeIterator _node);
    TargetDefinition* extractTarget(Tucuxi::Common::XmlNodeIterator _node);
    std::vector<AnalyteSet*> extractAnalyteGroups(Tucuxi::Common::XmlNodeIterator _node);
    AnalyteSet* extractAnalyteGroup(Tucuxi::Common::XmlNodeIterator _node);
    std::vector<Analyte*> extractAnalytes(Tucuxi::Common::XmlNodeIterator _node);
    Analyte* extractAnalyte(Tucuxi::Common::XmlNodeIterator _node);
    MolarMass* extractMolarMass(Tucuxi::Common::XmlNodeIterator _node);
    IResidualErrorModel* extractErrorModel(Tucuxi::Common::XmlNodeIterator _node);
    ParameterSetDefinition* extractParameterSet(Tucuxi::Common::XmlNodeIterator _node);
    std::vector<ParameterDefinition*> extractParameters(Tucuxi::Common::XmlNodeIterator _node);
    ParameterDefinition* extractParameter(Tucuxi::Common::XmlNodeIterator _node);
    std::vector<Correlation*> extractCorrelations(Tucuxi::Common::XmlNodeIterator _node);
    Correlation* extractCorrelation(Tucuxi::Common::XmlNodeIterator _node);
    ParameterVariability* extractVariability(Tucuxi::Common::XmlNodeIterator _node);
    FormulationAndRoutes* extractFullFormulationAndRoutes(Tucuxi::Common::XmlNodeIterator _node, const std::vector<AnalyteSet *> &_analyteSets);
    FullFormulationAndRoute* extractFullFormulationAndRoute(Tucuxi::Common::XmlNodeIterator _node, const std::vector<AnalyteSet *> &_analyteSets);
    ValidDurations* extractValidDurations(Tucuxi::Common::XmlNodeIterator _node);
    ValidDoses* extractValidDoses(Tucuxi::Common::XmlNodeIterator _node);
    ValidValuesRange* extractValuesRange(Tucuxi::Common::XmlNodeIterator _node);
    ValidValuesFixed* extractValuesFixed(Tucuxi::Common::XmlNodeIterator _node);

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
    TargetType extractTargetType(Tucuxi::Common::XmlNodeIterator _node);
    SigmaResidualErrorModel::ResidualErrorType extractResidualErrorType(Tucuxi::Common::XmlNodeIterator _node);
    ParameterVariabilityType extractParameterVariabilityType(Tucuxi::Common::XmlNodeIterator _node);

    Formulation extractFormulation(Tucuxi::Common::XmlNodeIterator _node);
    AdministrationRoute extractAdministrationRoute(Tucuxi::Common::XmlNodeIterator _node);
    AbsorptionModel extractAbsorptionModel(Tucuxi::Common::XmlNodeIterator _node);



};


} // namespace Core
} // namespace Tucuxi

#endif // DRUGMODELIMPORT_H
