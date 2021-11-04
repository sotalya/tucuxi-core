#ifndef DRUGMODELIMPORT_H
#define DRUGMODELIMPORT_H

#include <string>
#include <vector>
#include <mutex>

#include "tucucommon/xmlnode.h"
#include "tucucommon/xmldocument.h"
#include "tucucommon/translatablestring.h"
#include "tucucommon/xmlimporter.h"

#include "tucucore/definitions.h"
#include "tucucore/drugmodel/errormodel.h"
#include "tucucore/drugmodel/parameterdefinition.h"
#include "tucucore/drugmodel/formulationandroute.h"


namespace Tucuxi {
namespace Core {

enum class CovariateType;
enum class DataType;
enum class InterpolationType;
enum class TargetType;

class DrugModel;
class TimeConsiderations;
class HalfLife;
class OutdatedMeasure;
class PopulationValue;
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
class ErrorModel;
class ParameterSetDefinition;
class ParameterDefinition;
class Correlation;
class FullFormulationAndRoute;
class FormulationAndRoutes;
class DrugModelMetadata;

class LightPopulationValue
{
public:
    LightPopulationValue() : m_operation(nullptr), m_value(0.0) {}

    Operation* getOperation() { return m_operation;}
    Value getValue() { return m_value;}

    Operation *m_operation;
    Value m_value;
};


class DrugModelImport : public Tucuxi::Common::XMLImporter
{
public:


    ///
    /// \brief DrugModelImport empty constructor
    ///
    DrugModelImport();

    virtual ~DrugModelImport() = default;

    ///
    /// \brief importFromFile
    /// \param _drugModel A reference to a DrugModel pointer that will be allocated within the function
    /// \param _fileName The name of the file in which the drug model is stored
    /// \return Result::Ok if the import went well, another Result else.
    /// This function is reentrant.
    ///
    Status importFromFile(Tucuxi::Core::DrugModel *&_drugModel, const std::string &_fileName);

    ///
    /// \brief importFromString
    /// \param _drugModel A reference to a DrugModel pointer that will be allocated within the function
    /// \param _xml A string in which the drug model is stored
    /// \return Result::Ok if the import went well, another Result else.
    /// This function is reentrant.
    ///
    Status importFromString(Tucuxi::Core::DrugModel *&_drugModel, const std::string &_xml);

protected:

    const std::vector<std::string> &ignoredTags() const override;

    /// A mutex to ensure the public methods are reentrant
    std::mutex m_mutex;

    ///
    /// \brief importFromString
    /// \param _drugModel A reference to a DrugModel pointer that will be allocated within the function
    /// \param _document An XmlDocument in which the drug model is stored
    /// \return Result::Ok if the import went well, another Result else.
    /// This function is reentrant.
    ///
    Status importDocument(Tucuxi::Core::DrugModel *&_drugModel, Tucuxi::Common::XmlDocument & _document);


    ///////////////////////////////////////////////////////////////////////////////
    /// enum class imports
    ///////////////////////////////////////////////////////////////////////////////

    CovariateType extractCovariateType(Tucuxi::Common::XmlNodeIterator _node);
    DataType extractDataType(Tucuxi::Common::XmlNodeIterator _node);
    InterpolationType extractInterpolationType(Tucuxi::Common::XmlNodeIterator _node);
    TargetType extractTargetType(Tucuxi::Common::XmlNodeIterator _node);
    ResidualErrorType extractResidualErrorType(Tucuxi::Common::XmlNodeIterator _node);
    ParameterVariabilityType extractParameterVariabilityType(Tucuxi::Common::XmlNodeIterator _node);
    Formulation extractFormulation(Tucuxi::Common::XmlNodeIterator _node);
    AdministrationRoute extractAdministrationRoute(Tucuxi::Common::XmlNodeIterator _node);
    AbsorptionModel extractAbsorptionModel(Tucuxi::Common::XmlNodeIterator _node);


    ///////////////////////////////////////////////////////////////////////////////
    /// objects imports
    ///////////////////////////////////////////////////////////////////////////////

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
    ErrorModel* extractErrorModel(Tucuxi::Common::XmlNodeIterator _node);
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
    Operation* extractOperation(Tucuxi::Common::XmlNodeIterator _node);
    JSOperation* extractJSOperation(Tucuxi::Common::XmlNodeIterator _node);
    Tucuxi::Common::TranslatableString extractTranslatableString(Tucuxi::Common::XmlNodeIterator _node, const std::string &_insideName);
    DrugModelMetadata* extractHead(Tucuxi::Common::XmlNodeIterator _node);

};


} // namespace Core
} // namespace Tucuxi

#endif // DRUGMODELIMPORT_H
