#include "drugmodelimport.h"

#include <iostream>

#include "tucucommon/xmldocument.h"
#include "tucucommon/xmlnode.h"
#include "tucucommon/xmlattribute.h"
#include "tucucommon/xmliterator.h"
#include "tucucommon/translatablestring.h"
#include "tucucommon/xmlimporter.h"

#include "tucucore/drugmodel/drugmodel.h"
#include "tucucore/drugdefinitions.h"
#include "tucucore/drugmodel/drugmodeldomain.h"
#include "tucucore/definitions.h"
#include "tucucore/hardcodedoperation.h"
#include "tucucore/residualerrormodel.h"
#include "tucucore/pkmodel.h"
#include "tucucore/iresidualerrormodel.h"


using namespace Tucuxi::Common;
using namespace Tucuxi::Core;

namespace Tucuxi {
namespace Core {


/// Function to delete a pointer if it is not nullptr
template<typename T> inline void DELETE_IF_NON_NULL(T _p)
{
    if (_p != nullptr) {
        delete _p;
    }
}

/// Function to delete a vector of pointers.
/// It deletes every pointed value and emptied the vector.
template<typename T> inline void DELETE_PVECTOR(T _v) {
    while (!_v.empty()) {
        delete _v.back();
        _v.pop_back();
    }
}


///////////////////////////////////////////////////////////////////////////////
/// public methods
///////////////////////////////////////////////////////////////////////////////

DrugModelImport::DrugModelImport() = default;



DrugModelImport::Status DrugModelImport::importFromFile(Tucuxi::Core::DrugModel *&_drugModel, std::string _fileName)
{
    // Ensure the function is reentrant
    std::lock_guard<std::mutex> lock(m_mutex);

    setStatus(Status::Ok);
    _drugModel = nullptr;

    XmlDocument document;
    if (!document.open(_fileName)) {
        setStatus(Status::CantOpenFile, "Can not open file");
        return Status::CantOpenFile;
    }

    return importDocument(_drugModel, document);
}


DrugModelImport::Status DrugModelImport::importFromString(Tucuxi::Core::DrugModel *&_drugModel, std::string _xml)
{
    // Ensure the function is reentrant
    std::lock_guard<std::mutex> lock(m_mutex);

    setStatus(Status::Ok);
    _drugModel = nullptr;

    XmlDocument document;

    if (!document.fromString(_xml)) {
        setStatus(Status::Error, "The XML is not valid.");
        return Status::Error;
    }

    return importDocument(_drugModel, document);
}


///////////////////////////////////////////////////////////////////////////////
/// General methods
///////////////////////////////////////////////////////////////////////////////

DrugModelImport::Status DrugModelImport::importDocument(
        Tucuxi::Core::DrugModel *&_drugModel,
        Tucuxi::Common::XmlDocument & _document)
{
    XmlNode root = _document.getRoot();

    XmlNodeIterator drugModelIterator = root.getChildren("drugModel");

    checkNodeIterator(drugModelIterator, "drugModel");

    _drugModel = extractDrugModel(drugModelIterator);

    if (getStatus() != Status::Ok) {
        return getStatus();
    }

    DrugModelMetadata *metaData = nullptr;

    XmlNodeIterator metadataIterator = root.getChildren("head");

    if (metadataIterator == XmlNodeIterator::none()) {
        return Status::Error;
    }

    metaData = extractHead(metadataIterator);


    std::unique_ptr<PkModelCollection> models = std::make_unique<PkModelCollection>();
    if (!defaultPopulate(*models)) {
        setStatus(Status::Error);
    }

    if (_drugModel->getAnalyteSets().size() == 1) {
        std::shared_ptr<PkModel> model = models->getPkModelFromId(_drugModel->getAnalyteSet()->getPkModelId());
        if (model == nullptr) {
            setStatus(Status::Error, "The PK model ID is not a valid one supported by Tucuxi.");
            return getStatus();
        }
        metaData->setDistribution(model->getDistribution());
        metaData->setElimination(model->getElimination());
    }
    else {
        // TODO : Define that for multi-analytes
    }
    if ((_drugModel != nullptr) && (metaData != nullptr)) {
        _drugModel->setMetadata(std::unique_ptr<DrugModelMetadata>(metaData));
    }

    return getStatus();
}



const std::vector<std::string>& DrugModelImport::ignoredTags() const {
    static std::vector<std::string> ignoredTags =
    {"comments", "description", "errorMessage", "name"};
    return ignoredTags;
}


Tucuxi::Common::TranslatableString DrugModelImport::extractTranslatableString(Tucuxi::Common::XmlNodeIterator _node, std::string _insideName)
{
    Tucuxi::Common::TranslatableString result;

    XmlNodeIterator it = _node->getChildren();

    while (it != XmlNodeIterator::none()) {
        std::string nodeName = it->getName();
        if (nodeName == _insideName) {
            XmlAttribute langAttribute = it->getAttribute("lang");
            if (!langAttribute.isValid()) {
                setNodeError(it);
                return result;
            }
            std::string lang = langAttribute.getValue();
            std::string value = it->getValue();
            result.setString(value,lang);
        }
        else {
            unexpectedTag(nodeName);
        }
        it ++;
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////
/// enum class imports
///////////////////////////////////////////////////////////////////////////////


CovariateType DrugModelImport::extractCovariateType(Tucuxi::Common::XmlNodeIterator _node)
{
    static std::map<std::string, CovariateType> m =
    {
        {"standard", CovariateType::Standard},
        {"sex", CovariateType::Sex},
        {"ageInYears", CovariateType::AgeInYears},
        {"ageInDays", CovariateType::AgeInDays},
        {"ageInWeeks", CovariateType::AgeInWeeks},
        {"ageInMonths", CovariateType::AgeInMonths},
        {"dose", CovariateType::Dose}
    };


    auto it = m.find(_node->getValue());
    if (it != m.end()) {
        return it->second;
    }

    setNodeError(_node);
    return CovariateType::Standard;
}

DataType DrugModelImport::extractDataType(Tucuxi::Common::XmlNodeIterator _node)
{

    static std::map<std::string, DataType> m =
    {
        {"int", DataType::Int},
        {"double", DataType::Double},
        {"bool", DataType::Bool},
        {"date", DataType::Date}
    };


    auto it = m.find(_node->getValue());
    if (it != m.end()) {
        return it->second;
    }

    setNodeError(_node);
    return DataType::Double;
}

TargetType DrugModelImport::extractTargetType(Tucuxi::Common::XmlNodeIterator _node)
{
    static std::map<std::string, TargetType> m =
    {
        {"peak", TargetType::Peak},
        {"residual", TargetType::Residual},
        {"mean", TargetType::Mean},
        {"auc", TargetType::Auc},
        {"auc24", TargetType::Auc24},
        {"cumulativeAuc", TargetType::CumulativeAuc},
        {"aucOverMic", TargetType::AucOverMic},
        {"auc24OverMic", TargetType::Auc24OverMic},
        {"timeOverMic", TargetType::TimeOverMic},
        {"aucDividedByMic", TargetType::AucDividedByMic},
        {"auc24DividedByMic", TargetType::Auc24DividedByMic},
        {"peakDividedByMic", TargetType::PeakDividedByMic},
        {"residualDividedByMic", TargetType::ResidualDividedByMic}
    };

    auto it = m.find(_node->getValue());
    if (it != m.end()) {
        return it->second;
    }

    setNodeError(_node);
    return TargetType::UnknownTarget;
}

InterpolationType DrugModelImport::extractInterpolationType(Tucuxi::Common::XmlNodeIterator _node)
{

    static std::map<std::string, InterpolationType> m =
    {
        {"direct", InterpolationType::Direct},
        {"linear", InterpolationType::Linear},
        {"sigmoid", InterpolationType::Sigmoid},
        {"tanh", InterpolationType::Tanh}
    };

    auto it = m.find(_node->getValue());
    if (it != m.end()) {
        return it->second;
    }

    setNodeError(_node);
    return InterpolationType::Direct;
}


ResidualErrorType DrugModelImport::extractResidualErrorType(Tucuxi::Common::XmlNodeIterator _node)
{
    static std::map<std::string, ResidualErrorType> m =
    {
        {"additive", ResidualErrorType::ADDITIVE},
        {"proportional", ResidualErrorType::PROPORTIONAL},
        {"exponential", ResidualErrorType::EXPONENTIAL},
        {"propexp", ResidualErrorType::PROPEXP},
        {"mixed", ResidualErrorType::MIXED},
        {"softcoded", ResidualErrorType::SOFTCODED},
        {"none", ResidualErrorType::NONE}
    };

    auto it = m.find(_node->getValue());
    if (it != m.end()) {
        return it->second;
    }

    setNodeError(_node);
    return ResidualErrorType::NONE;
}


ParameterVariabilityType DrugModelImport::extractParameterVariabilityType(Tucuxi::Common::XmlNodeIterator _node)
{
    static std::map<std::string, ParameterVariabilityType> m =
    {
        {"normal", ParameterVariabilityType::Normal},
        {"lognormal", ParameterVariabilityType::LogNormal},
        {"proportional", ParameterVariabilityType::Proportional},
        {"exponential", ParameterVariabilityType::Exponential},
        {"additive", ParameterVariabilityType::Additive},
        {"logit", ParameterVariabilityType::Logit},
        {"none", ParameterVariabilityType::None}
    };

    auto it = m.find(_node->getValue());
    if (it != m.end()) {
        return it->second;
    }

    setNodeError(_node);
    return ParameterVariabilityType::None;
}

Formulation DrugModelImport::extractFormulation(Tucuxi::Common::XmlNodeIterator _node)
{

    static std::map<std::string, Formulation> m =
    {
        {"undefined", Formulation::Undefined},
        // We should get rid of the space to prefer the next option
        {"parenteral solution", Formulation::ParenteralSolution},
        {"parenteralSolution", Formulation::ParenteralSolution},
        // We should get rid of the space to prefer the next option
        {"oral solution", Formulation::OralSolution},
        {"oralSolution", Formulation::OralSolution},
        {"test", Formulation::Test}
    };

    auto it = m.find(_node->getValue());
    if (it != m.end()) {
        return it->second;
    }

    setNodeError(_node);
    return Formulation::Undefined;
}


AdministrationRoute DrugModelImport::extractAdministrationRoute(Tucuxi::Common::XmlNodeIterator _node)
{

    static std::map<std::string, AdministrationRoute> m =
    {
        {"undefined", AdministrationRoute::Undefined},
        {"intramuscular", AdministrationRoute::Intramuscular},
        {"intravenousBolus", AdministrationRoute::IntravenousBolus},
        {"intravenousDrip", AdministrationRoute::IntravenousDrip},
        {"nasal", AdministrationRoute::Nasal},
        {"oral", AdministrationRoute::Oral},
        {"rectal", AdministrationRoute::Rectal},
        {"subcutaneous", AdministrationRoute::Subcutaneous},
        {"sublingual", AdministrationRoute::Sublingual},
        {"transdermal", AdministrationRoute::Transdermal},
        {"vaginal", AdministrationRoute::Vaginal}
    };

    auto it = m.find(_node->getValue());
    if (it != m.end()) {
        return it->second;
    }

    setNodeError(_node);
    return AdministrationRoute::Undefined;
}


AbsorptionModel DrugModelImport::extractAbsorptionModel(Tucuxi::Common::XmlNodeIterator _node)
{

    static std::map<std::string, AbsorptionModel> m =
    {
        {"undefined", AbsorptionModel::Undefined},
        {"bolus", AbsorptionModel::Intravascular},
        {"extra", AbsorptionModel::Extravascular},
        {"extra.lag", AbsorptionModel::ExtravascularLag},
        {"infusion", AbsorptionModel::Infusion}
    };

    auto it = m.find(_node->getValue());
    if (it != m.end()) {
        return it->second;
    }

    setNodeError(_node);
    return AbsorptionModel::Undefined;

}



///////////////////////////////////////////////////////////////////////////////
/// objects imports
///////////////////////////////////////////////////////////////////////////////



JSOperation* DrugModelImport::extractJSOperation(Tucuxi::Common::XmlNodeIterator _node)
{
    JSOperation *operation = nullptr;
    std::string formula;
    OperationInputList operationInputList;

    XmlNodeIterator it = _node->getChildren();

    while (it != XmlNodeIterator::none()) {
        std::string nodeName = it->getName();
        if (nodeName == "code") {
            XmlNodeIterator cdataIt = it->getChildren();
            if (cdataIt->getType() != EXmlNodeType::CData) {
                setNodeError(cdataIt);
            }
            else {
                formula = cdataIt->getValue();
            }

        }
        else if (nodeName == "inputs") {
            XmlNodeIterator inputsIt = it->getChildren();

            while (inputsIt != XmlNodeIterator::none()) {
                if (inputsIt->getName() == "input") {

                    InputType inputType;
                    std::string id;

                    XmlNodeIterator inputIt = inputsIt->getChildren();
                    while (inputIt != XmlNodeIterator::none()) {
                        std::string inputItName = inputIt->getName();
                        if (inputItName == "id") {
                            id = inputIt->getValue();
                        }
                        else if (inputItName == "type") {
                            std::string t = inputIt->getValue();
                            if (t == "int") {
                                inputType = InputType::INTEGER;
                            }
                            else if (t == "double") {
                                inputType = InputType::DOUBLE;
                            }
                            else if (t == "bool") {
                                inputType = InputType::BOOL;
                            }
                            else {
                                setNodeError(inputIt);
                            }
                        }
                        else {
                            unexpectedTag(inputItName);
                        }
                        inputIt ++;
                    }

                    if (getStatus() != Status::Error) {
                        operationInputList.push_back( {id, inputType});
                    }

                }
                else {
                    unexpectedTag(inputsIt->getName());
                }

                inputsIt ++;
            }
            // We should access a repository to get an existing hardcoded operation, based on a Id
            // operation = hardcodedOperationRepository->getOperationById(it->getValue());
        }
        else if (nodeName == "multiFormula") {
            // TODO : Implement multi formula parsing
        }
        else {
            unexpectedTag(nodeName);
        }
        it ++;
    }

    if (getStatus() != Status::Ok) {
        return nullptr;
    }

    operation = new JSOperation(formula, operationInputList);

    return operation;
}


Operation* DrugModelImport::extractOperation(Tucuxi::Common::XmlNodeIterator _node)
{
    Operation *operation = nullptr;

    XmlNodeIterator it = _node->getChildren();

    while (it != XmlNodeIterator::none()) {
        std::string nodeName = it->getName();
        if (nodeName == "softFormula") {
            operation = extractJSOperation(it);
        }
        else if (nodeName == "hardFormula") {
            // We should access a repository to get an existing hardcoded operation, based on a Id
            // operation = hardcodedOperationRepository->getOperationById(it->getValue());
            OperationCollection collection;
            collection.populate();
            std::shared_ptr<Operation> sharedOperation = collection.getOperationFromId(it->getValue());
            if (sharedOperation != nullptr) {
                operation = sharedOperation.get()->clone().release();
            }
        }
        else if (nodeName == "multiFormula") {
            // TODO : Implement multi formula parsing
        }
        else {
            unexpectedTag(nodeName);
        }
        it ++;
    }

    if (getStatus() != Status::Ok) {
        DELETE_IF_NON_NULL(operation);
        return nullptr;
    }

    if (operation == nullptr) {
        setNodeError(_node);
    }

    return operation;
}

LightPopulationValue *DrugModelImport::extractPopulationValue(Tucuxi::Common::XmlNodeIterator _node)
{
    LightPopulationValue *populationValue;
    Value value = 0.0;
    Operation *operation = nullptr;
    std::string id;

    XmlNodeIterator it = _node->getChildren();

    while (it != XmlNodeIterator::none()) {
        std::string nodeName = it->getName();
        if (nodeName == "standardValue") {
            value = extractDouble(it);
        }
        else if (nodeName == "aprioriComputation") {
            operation = extractOperation(it);
        }
        else {
            unexpectedTag(nodeName);
        }
        it ++;
    }

    // No check for operation, as it is not a mandatory field.
    // It is simply nullptr by default

    populationValue = new LightPopulationValue();
    populationValue->m_value = value;
    populationValue->m_operation = operation;

    return populationValue;
}



DrugModel* DrugModelImport::extractDrugModel(Tucuxi::Common::XmlNodeIterator _node)
{
    XmlNodeIterator it = _node->getChildren();

    TimeConsiderations *timeConsiderations = nullptr;
    DrugModelDomain *domain = nullptr;
    std::vector<CovariateDefinition *> covariates;
    std::vector<ActiveMoiety *> activeMoieties;
    std::vector<AnalyteSet *> analyteSets;
    FormulationAndRoutes *formulationAndRoutes = nullptr;


    std::string drugId;
    std::string drugModelId;
    while (it != XmlNodeIterator::none()) {
        std::string nodeName = it->getName();
        if (nodeName == "drugId") {
            drugId = it->getValue();
        }
        else if (nodeName == "drugModelId") {
            drugModelId = it->getValue();
        }
        else if (nodeName == "domain") {
            domain = extractDrugDomain(it);
        }
        else if (nodeName == "covariates") {
            covariates = extractCovariates(it);
        }
        else if (nodeName == "activeMoieties") {
            activeMoieties = extractActiveMoieties(it);
        }
        else if (nodeName == "analyteGroups") {
            analyteSets = extractAnalyteGroups(it);
        }
        else if (nodeName == "formulationAndRoutes") {
            formulationAndRoutes = extractFullFormulationAndRoutes(it, analyteSets);
        }
        else if (nodeName == "timeConsiderations") {
            timeConsiderations = extractTimeConsiderations(it);
        }
        else {
            unexpectedTag(nodeName);
        }
        it ++;
    }

    if (getStatus() != Status::Ok) {
        DELETE_PVECTOR(covariates);
        DELETE_PVECTOR(activeMoieties);
        DELETE_PVECTOR(analyteSets);
        DELETE_IF_NON_NULL(formulationAndRoutes);
        DELETE_IF_NON_NULL(timeConsiderations);
        DELETE_IF_NON_NULL(domain);
        return nullptr;
    }

    DrugModel *drugModel = new DrugModel();
    drugModel->setDrugId(drugId);
    drugModel->setDrugModelId(drugModelId);
    drugModel->setFormulationAndRoutes(std::unique_ptr<FormulationAndRoutes>(formulationAndRoutes));


    drugModel->setTimeConsiderations(std::unique_ptr<TimeConsiderations>(timeConsiderations));
    drugModel->setDomain(std::unique_ptr<DrugModelDomain>(domain));
    for (const auto & covariate : covariates) {
        drugModel->addCovariate(std::unique_ptr<CovariateDefinition>(covariate));
    }
    for (const auto & activeMoiety : activeMoieties) {
        drugModel->addActiveMoiety(std::unique_ptr<ActiveMoiety>(activeMoiety));
    }
    for (const auto & analyteSet : analyteSets) {
        drugModel->addAnalyteSet(std::unique_ptr<AnalyteSet>(analyteSet));
    }

    return drugModel;

}


TimeConsiderations* DrugModelImport::extractTimeConsiderations(Tucuxi::Common::XmlNodeIterator _node)
{
    TimeConsiderations *timeConsiderations = new TimeConsiderations();
    HalfLife *halfLife = nullptr;
    OutdatedMeasure *outdatedMeasure = nullptr;

    XmlNodeIterator it = _node->getChildren();

    while (it != XmlNodeIterator::none()) {
        std::string nodeName = it->getName();
        if (nodeName == "halfLife") {
            halfLife = extractHalfLife(it);
        }
        else if (nodeName == "outdatedMeasure") {
            outdatedMeasure = extractOutdatedMeasure(it);
        }
        else {
            unexpectedTag(nodeName);
        }
        it ++;
    }

    timeConsiderations->setHalfLife(std::unique_ptr<HalfLife>(halfLife));
    timeConsiderations->setOutdatedMeasure(std::unique_ptr<OutdatedMeasure>(outdatedMeasure));
    return timeConsiderations;
}


HalfLife* DrugModelImport::extractHalfLife(Tucuxi::Common::XmlNodeIterator _node)
{
    std::string id;
    //    Value value;
    TucuUnit unit;
    double multiplier = 1.0;
    //    Operation *operation;
    LightPopulationValue *value = nullptr;

    XmlNodeIterator it = _node->getChildren();

    while (it != XmlNodeIterator::none()) {
        std::string nodeName = it->getName();
        if (nodeName == "unit") {
            unit = extractUnit(it, CheckUnit::Check);
        }
        else if (nodeName == "duration") {
            value = extractPopulationValue(it);
        }
        else if (nodeName == "multiplier") {
            multiplier = extractDouble(it);
        }
        else {
            unexpectedTag(nodeName);
        }
        it ++;
    }

    if (value == nullptr) {
        setStatus(Status::Error, "No duration value in half life.");
        return nullptr;
    }

    HalfLife *halfLife = new HalfLife(id, value->getValue(), unit, multiplier, value->getOperation());

    DELETE_IF_NON_NULL(value);

    return halfLife;
}

OutdatedMeasure* DrugModelImport::extractOutdatedMeasure(Tucuxi::Common::XmlNodeIterator _node)
{
    OutdatedMeasure *outdatedMeasure = nullptr;
    TucuUnit unit;
    LightPopulationValue *value = nullptr;
    std::string id;

    XmlNodeIterator it = _node->getChildren();

    while (it != XmlNodeIterator::none()) {
        std::string nodeName = it->getName();
        if (nodeName == "unit") {
            unit = extractUnit(it, CheckUnit::Check);
        }
        else if (nodeName == "duration") {
            value = extractPopulationValue(it);
        }
        else {
            unexpectedTag(nodeName);
        }
        it ++;
    }

    if (getStatus() != Status::Ok) {
        DELETE_IF_NON_NULL(value);
        return nullptr;
    }

    if (value == nullptr) {
        setStatus(Status::Error, "No value in outdate measure");
        return nullptr;
    }

    outdatedMeasure = new OutdatedMeasure(id, value->getValue(), unit, value->getOperation());

    DELETE_IF_NON_NULL(value);

    return outdatedMeasure;
}

DrugModelDomain* DrugModelImport::extractDrugDomain(Tucuxi::Common::XmlNodeIterator _node)
{
    DrugModelDomain *domain;
    std::vector<Constraint*> constraints;
    TranslatableString domainDescription;

    domain = new DrugModelDomain();

    XmlNodeIterator it = _node->getChildren();

    while (it != XmlNodeIterator::none()) {
        std::string nodeName = it->getName();
        if (nodeName == "description") {
            domainDescription = extractTranslatableString(it, "desc");
        }
        else if (nodeName == "constraints") {
            constraints = extractConstraints(it);
        }
        else {
            unexpectedTag(nodeName);
        }
        it ++;
    }

    if (getStatus() != Status::Ok) {
        DELETE_IF_NON_NULL(domain);
        return nullptr;
    }

    domain->setDescription(domainDescription);

    for(const auto & constraint : constraints) {
        domain->addConstraint(std::unique_ptr<Constraint>(constraint));
    }

    return domain;
}


std::vector<Constraint *> DrugModelImport::extractConstraints(Tucuxi::Common::XmlNodeIterator _node)
{
    std::vector<Constraint *> constraints;

    XmlNodeIterator it = _node->getChildren();

    while (it != XmlNodeIterator::none()) {
        std::string nodeName = it->getName();
        if (nodeName == "constraint") {
            Constraint *constraint;
            constraint = extractConstraint(it);
            if (constraint != nullptr) {
                constraints.push_back(constraint);
            }
        }
        else {
            unexpectedTag(nodeName);
        }
        it ++;
    }

    return constraints;
}

Constraint* DrugModelImport::extractConstraint(Tucuxi::Common::XmlNodeIterator _node)
{
    Constraint* constraint;
    constraint = new Constraint();
    Operation *operation = nullptr;

    XmlNodeIterator it = _node->getChildren();

    while (it != XmlNodeIterator::none()) {
        std::string nodeName = it->getName();
        if (nodeName == "constraintType") {
            std::string nodeValue = it->getValue();
            if (nodeValue == "hard") {
                constraint->setType(ConstraintType::HARD);
            }
            else if (nodeValue == "soft") {
                constraint->setType(ConstraintType::SOFT);
            }
            else {
                setNodeError(it);
            }
        }
        else if (nodeName == "errorMessage") {
            constraint->setErrorMessage(extractTranslatableString(it, "text"));
        }
        else if (nodeName == "requiredCovariates") {

            XmlNodeIterator reqCovariatesIt = it->getChildren();
            while (reqCovariatesIt != XmlNodeIterator::none()) {
                if (reqCovariatesIt->getName() == "covariateId") {
                    constraint->addRequiredCovariateId(reqCovariatesIt->getValue());
                }
                else {
                    unexpectedTag(reqCovariatesIt->getName());
                }
                reqCovariatesIt ++;
            }

        }
        else if (nodeName == "checkOperation") {
            operation = extractOperation(it);
        }
        else {
            unexpectedTag(nodeName);
        }
        it ++;
    }

    if (getStatus() != Status::Ok) {
        DELETE_IF_NON_NULL(constraint);
        return nullptr;
    }

    constraint->setCheckOperation(std::unique_ptr<Operation>(operation));

    return constraint;
}

std::vector<CovariateDefinition*> DrugModelImport::extractCovariates(Tucuxi::Common::XmlNodeIterator _node)
{

    std::vector<CovariateDefinition *> covariates;

    XmlNodeIterator it = _node->getChildren();

    while (it != XmlNodeIterator::none()) {
        std::string nodeName = it->getName();
        if (nodeName == "covariate") {
            CovariateDefinition *covariate;
            covariate = extractCovariate(it);
            if (covariate != nullptr) {
                covariates.push_back(covariate);
            }
        }
        else {
            unexpectedTag(nodeName);
        }
        it ++;
    }

    return covariates;
}

Duration castDuration(double _duration, TucuUnit _unit)
{
    if (_unit.toString() == "d") {
        return Duration(std::chrono::seconds(static_cast<long>(_duration * 3600.0 * 24.0)));
    }
    if (_unit.toString() == "min") {
        return Duration(std::chrono::seconds(static_cast<long>(_duration * 60.0)));
    }
    if (_unit.toString() == "m") {
        return Duration(std::chrono::seconds(static_cast<long>(_duration * 60.0)));
    }
    if (_unit.toString() == "h") {
        return Duration(std::chrono::seconds(static_cast<long>(_duration * 3600.0)));
    }
    if (_unit.toString() == "s") {
        return Duration(std::chrono::seconds(static_cast<long>(_duration)));
    }
    return Duration();
}

CovariateDefinition* DrugModelImport::extractCovariate(Tucuxi::Common::XmlNodeIterator _node)
{
    CovariateDefinition* covariate;

    std::string id;
    CovariateType type = CovariateType::Standard;
    DataType dataType = DataType::Int;
    TucuUnit unit;
    InterpolationType interpolationType = InterpolationType::Direct;
    LightPopulationValue *value = nullptr;
    Operation *validation = nullptr;
    TranslatableString name;
    TucuUnit refreshPeriodUnit;
    double refreshPeriodValue = 0.0;

    XmlNodeIterator it = _node->getChildren();

    while (it != XmlNodeIterator::none()) {
        std::string nodeName = it->getName();
        if (nodeName == "covariateId") {
            id = it->getValue();
        }
        else if (nodeName == "unit") {
            unit = extractUnit(it, CheckUnit::Check);
        }
        else if (nodeName == "covariateName") {
            name = extractTranslatableString(it,"name");
        }
        else if (nodeName == "covariateType") {
            type = extractCovariateType(it);
        }
        else if (nodeName == "dataType") {
            dataType = extractDataType(it);
        }
        else if (nodeName == "interpolationType") {
            interpolationType = extractInterpolationType(it);
        }
        else if (nodeName == "refreshPeriod") {

            XmlNodeIterator refreshIt = it->getChildren();
            while (refreshIt != XmlNodeIterator::none()) {
                if (refreshIt->getName() == "unit") {
                    // As a covariate can be of any type, the value could lead to a conversion issue
                    refreshPeriodUnit = extractUnit(refreshIt, CheckUnit::Check);
                }
                else if (refreshIt->getName() == "value") {
                    refreshPeriodValue = extractDouble(refreshIt);
                }
                else {
                    unexpectedTag(refreshIt->getName());
                }
                refreshIt ++;
            }
        }
        else if (nodeName == "covariateValue") {
            value = extractPopulationValue(it);
        }
        else if (nodeName == "validation") {

            XmlNodeIterator validationIt = it->getChildren();

            while (validationIt != XmlNodeIterator::none()) {
                if (validationIt->getName() == "operation") {
                    validation = extractOperation(validationIt);
                }
                else {
                    unexpectedTag(validationIt->getName());
                }
                validationIt ++;
            }
        }
        else {
            unexpectedTag(nodeName);
        }

        it ++;
    }

    if (getStatus() != Status::Ok) {
        DELETE_IF_NON_NULL(validation);
        DELETE_IF_NON_NULL(value);
        return nullptr;
    }

    if (value == nullptr) {
        DELETE_IF_NON_NULL(validation);
        setStatus(Status::Error, "no value in a covariate");
        return nullptr;
    }

    // TODO : Try to find a way to allow flexible covariate values. This is odd
    std::string valueString = std::to_string(value->getValue());

    covariate = new CovariateDefinition(id, valueString, value->getOperation(), type, dataType);
    //    covariate = new CovariateDefinition(id, valueString, nullptr, type, dataType);
    covariate->setInterpolationType(interpolationType);
    covariate->setUnit(unit);
    covariate->setValidation(std::unique_ptr<Operation>(validation));
    covariate->setName(name);
    covariate->setRefreshPeriod(castDuration(refreshPeriodValue, refreshPeriodUnit));

    DELETE_IF_NON_NULL(value);

    return covariate;

}



std::vector<ActiveMoiety*> DrugModelImport::extractActiveMoieties(Tucuxi::Common::XmlNodeIterator _node)
{

    std::vector<ActiveMoiety *> activeMoieties;

    XmlNodeIterator it = _node->getChildren();

    while (it != XmlNodeIterator::none()) {
        std::string nodeName = it->getName();
        if (nodeName == "activeMoiety") {
            ActiveMoiety *activeMoiety;
            activeMoiety = extractActiveMoiety(it);
            if (activeMoiety != nullptr) {
                activeMoieties.push_back(activeMoiety);
            }
        }
        else {
            unexpectedTag(nodeName);
        }
        it ++;
    }

    return activeMoieties;

}

ActiveMoiety* DrugModelImport::extractActiveMoiety(Tucuxi::Common::XmlNodeIterator _node)
{

    ActiveMoiety *activeMoiety = nullptr;
    std::string activeMoietyId;
    TucuUnit unit;
    std::vector<AnalyteId> analyteIdList;
    Operation *formula = nullptr;
    std::vector<TargetDefinition *> targets;
    TranslatableString name;

    XmlNodeIterator it = _node->getChildren();

    while (it != XmlNodeIterator::none()) {
        std::string nodeName = it->getName();
        if (nodeName == "activeMoietyId") {
            activeMoietyId = it->getValue();
        }
        else if (nodeName == "unit") {
            unit = extractUnit(it, CheckUnit::Check);
        }
        else if (nodeName == "activeMoietyName") {
            name = extractTranslatableString(it, "name");
        }
        else if (nodeName == "analyteIdList") {

            XmlNodeIterator analyteIt = it->getChildren();

            while (analyteIt != XmlNodeIterator::none()) {
                if (analyteIt->getName() == "analyteId") {
                    analyteIdList.push_back(AnalyteId(analyteIt->getValue()));
                }
                else {
                    unexpectedTag(analyteIt->getName());
                }
                analyteIt ++;
            }
        }
        else if (nodeName == "analytesToMoietyFormula") {
            formula = extractOperation(it);
        }
        else if (nodeName == "targets") {
            targets = extractTargets(it);
        }
        else {
            unexpectedTag(nodeName);
        }

        it ++;
    }

    if (getStatus() != Status::Ok) {
        DELETE_IF_NON_NULL(formula);
        DELETE_PVECTOR(targets);
        return nullptr;
    }

    activeMoiety = new ActiveMoiety(ActiveMoietyId(activeMoietyId), unit, analyteIdList, std::unique_ptr<Operation>(formula));
    for (const auto & target : targets) {
        target->setActiveMoietyId(ActiveMoietyId(activeMoietyId));
        activeMoiety->addTarget(std::unique_ptr<TargetDefinition>(target));
    }
    activeMoiety->setName(name);

    return activeMoiety;
}


std::vector<TargetDefinition*> DrugModelImport::extractTargets(Tucuxi::Common::XmlNodeIterator _node)
{

    std::vector<TargetDefinition *> targets;

    XmlNodeIterator it = _node->getChildren();

    while (it != XmlNodeIterator::none()) {
        std::string nodeName = it->getName();
        if (nodeName == "target") {
            TargetDefinition *target;
            target = extractTarget(it);
            if (target != nullptr) {
                targets.push_back(target);
            }
        }
        else {
            unexpectedTag(nodeName);
        }

        it ++;
    }

    return targets;
}


TargetDefinition* DrugModelImport::extractTarget(Tucuxi::Common::XmlNodeIterator _node)
{

    TargetDefinition *target = nullptr;
    TucuUnit unit("ug/l");
    TargetType type = TargetType::UnknownTarget;
    LightPopulationValue *minValue = nullptr;
    LightPopulationValue *maxValue = nullptr;
    LightPopulationValue *bestValue = nullptr;
    LightPopulationValue *mic = nullptr;
    LightPopulationValue *tMin = nullptr;
    LightPopulationValue *tMax = nullptr;
    LightPopulationValue *tBest = nullptr;
    LightPopulationValue *toxicityAlarm = nullptr;
    LightPopulationValue *inefficacyAlarm = nullptr;
    TucuUnit micUnit("ug/l");

    TucuUnit tUnit = TucuUnit("h");

    XmlNodeIterator it = _node->getChildren();

    while (it != XmlNodeIterator::none()) {
        std::string nodeName = it->getName();
        if (nodeName == "targetType") {
            type = extractTargetType(it);
        }
        else if (nodeName == "targetValues") {

            XmlNodeIterator targetValuesIt = it->getChildren();

            while (targetValuesIt != XmlNodeIterator::none()) {
                std::string valueName = targetValuesIt->getName();

                if (valueName == "unit") {
                    unit = extractUnit(targetValuesIt, CheckUnit::Check);
                }
                else if (valueName == "min") {
                    minValue = extractPopulationValue(targetValuesIt);
                }
                else if (valueName == "max") {
                    maxValue = extractPopulationValue(targetValuesIt);
                }
                else if (valueName == "best") {
                    bestValue = extractPopulationValue(targetValuesIt);
                }
                else if (valueName == "mic") {

                    XmlNodeIterator micIt = targetValuesIt->getChildren();

                    while (micIt != XmlNodeIterator::none()) {
                        std::string micName = micIt->getName();
                        if (micName == "unit") {
                            micUnit = extractUnit(micIt, CheckUnit::Check);
                        }
                        else if (micName == "micValue") {
                            mic = extractPopulationValue(micIt);
                        }
                        else {
                            unexpectedTag(micName);
                        }
                        micIt ++;
                    }
                }
                else if (valueName == "toxicityAlarm") {
                    toxicityAlarm = extractPopulationValue(targetValuesIt);
                }
                else if (valueName == "inefficacyAlarm") {
                    inefficacyAlarm = extractPopulationValue(targetValuesIt);
                }
                else {
                    unexpectedTag(valueName);
                }


                targetValuesIt ++;
            }
        }
        else if (nodeName == "times") {

            XmlNodeIterator timesIt = it->getChildren();

            while (timesIt != XmlNodeIterator::none()) {
                std::string valueName = timesIt->getName();

                if (valueName == "unit") {
                    tUnit = extractUnit(timesIt, CheckUnit::Check);
                }
                else if (valueName == "min") {
                    tMin = extractPopulationValue(timesIt);
                }
                else if (valueName == "max") {
                    tMax = extractPopulationValue(timesIt);
                }
                else if (valueName == "best") {
                    tBest = extractPopulationValue(timesIt);
                }
                else {
                    unexpectedTag(valueName);
                }


                timesIt ++;
            }
        }
        else {
            unexpectedTag(nodeName);
        }

        it ++;
    }

    if ((type == TargetType::AucOverMic) ||
            (type == TargetType::TimeOverMic) ||
            (type == TargetType::Auc24OverMic) ||
            (type == TargetType::AucDividedByMic) ||
            (type == TargetType::PeakDividedByMic) ||
            (type == TargetType::ResidualDividedByMic) ||
            (type == TargetType::Auc24DividedByMic)) {
        if (mic == nullptr) {
            setStatus(Status::Error, "A target is using MIC, but no MIC tag is found in the target");
        }
    }


    if ((getStatus() != Status::Ok) ||
        (minValue == nullptr) || (maxValue == nullptr) || (bestValue == nullptr)) {

        if (minValue == nullptr) {
            setStatus(Status::Error, "No min value in a target");
        }

        if (maxValue == nullptr) {
            setStatus(Status::Error, "No max value in a target");
        }

        if (bestValue == nullptr) {
            setStatus(Status::Error, "No best value in a target");
        }
        DELETE_IF_NON_NULL(minValue);
        DELETE_IF_NON_NULL(maxValue);
        DELETE_IF_NON_NULL(bestValue);
        DELETE_IF_NON_NULL(mic);
        DELETE_IF_NON_NULL(tMin);
        DELETE_IF_NON_NULL(tMax);
        DELETE_IF_NON_NULL(tBest);
        DELETE_IF_NON_NULL(toxicityAlarm);
        DELETE_IF_NON_NULL(inefficacyAlarm);
        return nullptr;
    }

    if (mic == nullptr) {
        mic = new LightPopulationValue();
    }
    if (tMin == nullptr) {
        tMin = new LightPopulationValue();
    }
    if (tMax == nullptr) {
        tMax = new LightPopulationValue();
    }
    if (tBest == nullptr) {
        tBest = new LightPopulationValue();
    }
    if (toxicityAlarm == nullptr) {
        toxicityAlarm = new LightPopulationValue();
    }
    if (inefficacyAlarm == nullptr) {
        inefficacyAlarm = new LightPopulationValue();
    }

    target = new TargetDefinition(type, unit, ActiveMoietyId(""),
                                  std::make_unique<SubTargetDefinition>("cMin", minValue->getValue(), minValue->getOperation()),
                                  std::make_unique<SubTargetDefinition>("cMax", maxValue->getValue(), maxValue->getOperation()),
                                  std::make_unique<SubTargetDefinition>("cBest", bestValue->getValue(), bestValue->getOperation()),
                                  std::make_unique<SubTargetDefinition>("mic", mic->getValue(), mic->getOperation()),
                                  std::make_unique<SubTargetDefinition>("tMin", UnitManager::convertToUnit<UnitManager::UnitType::Time>(tMin->getValue(), tUnit, TucuUnit("min")), tMin->getOperation()),
                                  std::make_unique<SubTargetDefinition>("tMax", UnitManager::convertToUnit<UnitManager::UnitType::Time>(tMax->getValue(), tUnit, TucuUnit("min")), tMax->getOperation()),
                                  std::make_unique<SubTargetDefinition>("tBest", UnitManager::convertToUnit<UnitManager::UnitType::Time>(tBest->getValue(), tUnit, TucuUnit("min")), tBest->getOperation()),
                                  std::make_unique<SubTargetDefinition>("toxicity", toxicityAlarm->getValue(), toxicityAlarm->getOperation()),
                                  std::make_unique<SubTargetDefinition>("inefficacy", inefficacyAlarm->getValue(), inefficacyAlarm->getOperation()),
                                  micUnit,
                                  tUnit
                                  );

    DELETE_IF_NON_NULL(minValue);
    DELETE_IF_NON_NULL(maxValue);
    DELETE_IF_NON_NULL(bestValue);
    DELETE_IF_NON_NULL(mic);
    DELETE_IF_NON_NULL(tMin);
    DELETE_IF_NON_NULL(tMax);
    DELETE_IF_NON_NULL(tBest);
    DELETE_IF_NON_NULL(toxicityAlarm);
    DELETE_IF_NON_NULL(inefficacyAlarm);

    return target;
}


std::vector<AnalyteSet*> DrugModelImport::extractAnalyteGroups(Tucuxi::Common::XmlNodeIterator _node)
{

    std::vector<AnalyteSet *> analyteGroups;

    XmlNodeIterator it = _node->getChildren();

    while (it != XmlNodeIterator::none()) {
        std::string nodeName = it->getName();
        if (nodeName == "analyteGroup") {
            AnalyteSet *analyteGroup;
            analyteGroup = extractAnalyteGroup(it);
            if (analyteGroup != nullptr) {
                analyteGroups.push_back(analyteGroup);
            }
        }
        else {
            unexpectedTag(nodeName);
        }
        it ++;
    }

    return analyteGroups;
}

AnalyteSet* DrugModelImport::extractAnalyteGroup(Tucuxi::Common::XmlNodeIterator _node)
{
    AnalyteSet *analyteGroup = nullptr;
    std::string groupId;
    std::string pkModelId;
    // Use a default value for the concentration unit, for backward compatibility purpose
    TucuUnit concentrationUnit{"ug/l"};
    std::vector<Analyte*> analytes;
    ParameterSetDefinition* parameters = nullptr;
    std::vector<TargetDefinition *> targets;

    XmlNodeIterator it = _node->getChildren();

    while (it != XmlNodeIterator::none()) {
        std::string nodeName = it->getName();
        if (nodeName == "groupId") {
            groupId = it->getValue();
        }
        else if (nodeName == "pkModelId") {
            pkModelId = it->getValue();
        }
        else if (nodeName == "analytes") {
            analytes = extractAnalytes(it);
        }
        else if (nodeName == "dispositionParameters") {
            parameters = extractParameterSet(it);
        }
        else {
            unexpectedTag(nodeName);
        }
        it ++;
    }

    if (getStatus() != Status::Ok) {
        DELETE_PVECTOR(analytes);
        DELETE_IF_NON_NULL(parameters);
        return nullptr;
    }

    analyteGroup = new AnalyteSet();
    analyteGroup->setId(groupId);
    analyteGroup->setPkModelId(pkModelId);
    if (analytes.size() > 0) {
        // Here we assume all analytes will share the same unit
        analyteGroup->setDoseUnit(Tucuxi::Common::UnitManager::getWeightFromConcentration(analytes[0]->getUnit()));
    }
    for (const auto & analyte : analytes) {
        analyteGroup->addAnalyte(std::unique_ptr<Analyte>(analyte));
    }
    analyteGroup->setDispositionParameters(std::unique_ptr<ParameterSetDefinition>(parameters));

    return analyteGroup;

}


std::vector<Analyte*> DrugModelImport::extractAnalytes(Tucuxi::Common::XmlNodeIterator _node)
{

    std::vector<Analyte *> analytes;

    XmlNodeIterator it = _node->getChildren();

    while (it != XmlNodeIterator::none()) {
        std::string nodeName = it->getName();
        if (nodeName == "analyte") {
            Analyte *analyte;
            analyte = extractAnalyte(it);
            if (analyte != nullptr) {
                analytes.push_back(analyte);
            }
        }
        else {
            unexpectedTag(nodeName);
        }
        it ++;
    }

    return analytes;
}

Analyte* DrugModelImport::extractAnalyte(Tucuxi::Common::XmlNodeIterator _node)
{

    Analyte *analyte;
    std::string analyteId;
    TucuUnit unit;
    MolarMass *molarMass = nullptr;
    ErrorModel *errorModel = nullptr;

    XmlNodeIterator it = _node->getChildren();

    while (it != XmlNodeIterator::none()) {
        std::string nodeName = it->getName();
        if (nodeName == "analyteId") {
            analyteId = it->getValue();
        }
        else if (nodeName == "unit") {
            unit = extractUnit(it, CheckUnit::Check);
        }
        else if (nodeName == "molarMass") {
            molarMass = extractMolarMass(it);
        }
        else if (nodeName == "errorModel") {
            errorModel = extractErrorModel(it);
        }
        else {
            unexpectedTag(nodeName);
        }

        it ++;
    }

    if (getStatus() != Status::Ok) {
        DELETE_IF_NON_NULL(molarMass);
        DELETE_IF_NON_NULL(errorModel);
        return nullptr;
    }

    if (molarMass == nullptr) {
        setStatus(Status::Error, "No molar mass in an analyte");
        DELETE_IF_NON_NULL(molarMass);
        DELETE_IF_NON_NULL(errorModel);
        return nullptr;
    }

    analyte = new Analyte(analyteId, unit, *molarMass);
    analyte->setResidualErrorModel(std::unique_ptr<ErrorModel>(errorModel));

    DELETE_IF_NON_NULL(molarMass);

    return analyte;
}

MolarMass* DrugModelImport::extractMolarMass(Tucuxi::Common::XmlNodeIterator _node)
{
    MolarMass *molarMass = nullptr;
    Value value = 0.0;
    TucuUnit unit;

    XmlNodeIterator it = _node->getChildren();

    while (it != XmlNodeIterator::none()) {
        std::string nodeName = it->getName();
        if (nodeName == "value") {
            value = extractDouble(it);
        }
        else if (nodeName == "unit") {
            unit = extractUnit(it, CheckUnit::Check);
        }
        else {
            unexpectedTag(nodeName);
        }

        it ++;
    }

    if (getStatus() != Status::Ok) {
        return nullptr;
    }

    molarMass = new MolarMass(value, unit);
    return molarMass;
}

ErrorModel *DrugModelImport::extractErrorModel(Tucuxi::Common::XmlNodeIterator _node)
{
    ErrorModel *errorModel = nullptr;
    ResidualErrorType type = ResidualErrorType::NONE;
    Operation *applyFormula = nullptr;
    Operation *likelyhoodFormula = nullptr;
    std::vector<LightPopulationValue*> sigmas;

    XmlNodeIterator it = _node->getChildren();

    while (it != XmlNodeIterator::none()) {
        std::string nodeName = it->getName();
        if (nodeName == "errorModelType") {
            type = extractResidualErrorType(it);
        }
        else if (nodeName == "applyFormula") {
            applyFormula = extractOperation(it);
        }
        else if (nodeName == "likelyHoodFormula") {
            likelyhoodFormula = extractOperation(it);
        }
        else if (nodeName == "sigmas") {
            XmlNodeIterator sigmaIt = it->getChildren();

            while (sigmaIt != XmlNodeIterator::none()) {
                if (sigmaIt->getName() == "sigma") {
                    LightPopulationValue* value = extractPopulationValue(sigmaIt);
                    if (getStatus() == Status::Ok) {
                        sigmas.push_back(value);
                    }
                }
                else {
                    unexpectedTag(sigmaIt->getName());
                }
                sigmaIt ++;
            }

        }
        else {
            unexpectedTag(nodeName);
        }

        it ++;
    }

    if (getStatus() != Status::Ok) {
        DELETE_PVECTOR(sigmas);
        return nullptr;
    }

    errorModel = new ErrorModel();
    errorModel->setErrorModel(type);
    for (const auto & sigma : sigmas) {
        errorModel->addOriginalSigma(std::make_unique<PopulationValue>("", sigma->getValue(), sigma->getOperation()));
    }
    errorModel->setApplyFormula(std::unique_ptr<Operation>(applyFormula));
    errorModel->setLikelyhoodFormula(std::unique_ptr<Operation>(likelyhoodFormula));

    DELETE_PVECTOR(sigmas);


    return errorModel;
}

ParameterSetDefinition* DrugModelImport::extractParameterSet(Tucuxi::Common::XmlNodeIterator _node)
{
    ParameterSetDefinition *parameterSet;
    std::vector<ParameterDefinition *> parameters;
    std::vector<Correlation *> correlations;

    XmlNodeIterator it = _node->getChildren();

    while (it != XmlNodeIterator::none()) {
        std::string nodeName = it->getName();
        if (nodeName == "parameters") {
            parameters = extractParameters(it);
        }
        else if (nodeName == "correlations") {
            correlations = extractCorrelations(it);
        }
        else {
            unexpectedTag(nodeName);
        }

        it ++;
    }

    if (getStatus() != Status::Ok) {
        DELETE_PVECTOR(parameters);
        DELETE_PVECTOR(correlations);

        return nullptr;
    }

    parameterSet = new ParameterSetDefinition();

    for (const auto & correlation : correlations) {
        parameterSet->addCorrelation(*correlation);
        DELETE_IF_NON_NULL(correlation);
    }
    for (const auto & parameter : parameters) {
        parameterSet->addParameter(std::unique_ptr<ParameterDefinition>(parameter));
    }

    return parameterSet;
}

std::vector<ParameterDefinition*> DrugModelImport::extractParameters(Tucuxi::Common::XmlNodeIterator _node)
{

    std::vector<ParameterDefinition *> parameters;

    XmlNodeIterator it = _node->getChildren();

    while (it != XmlNodeIterator::none()) {
        std::string nodeName = it->getName();
        if (nodeName == "parameter") {
            ParameterDefinition *parameter;
            parameter = extractParameter(it);
            if (parameter != nullptr) {
                parameters.push_back(parameter);
            }
        }
        else {
            unexpectedTag(nodeName);
        }
        it ++;
    }

    return parameters;
}

ParameterDefinition* DrugModelImport::extractParameter(Tucuxi::Common::XmlNodeIterator _node)
{

    ParameterDefinition *parameter;
    std::string id;
    TucuUnit unit;
    LightPopulationValue *value = nullptr;
    ParameterVariability *variability = nullptr;
    Operation *validation = nullptr;

    XmlNodeIterator it = _node->getChildren();

    while (it != XmlNodeIterator::none()) {
        std::string nodeName = it->getName();
        if (nodeName == "parameterId") {
            id = it->getValue();
        }
        else if (nodeName == "unit") {
            unit = extractUnit(it, CheckUnit::DoNotCheck);
        }
        else if (nodeName == "parameterValue") {
            value = extractPopulationValue(it);
        }
        else if (nodeName == "bsv") {
            variability = extractVariability(it);
        }
        else if (nodeName == "validation") {

            // TODO : To be refactor to be the same as in Covariates
            XmlNodeIterator validationIt = it->getChildren();

            while (validationIt != XmlNodeIterator::none()) {
                if (validationIt->getName() == "operation") {
                    validation = extractOperation(validationIt);
                }
                else {
                    unexpectedTag(validationIt->getName());
                }
                validationIt ++;
            }
        }
        else {
            unexpectedTag(nodeName);
        }

        it ++;
    }

    if (getStatus() != Status::Ok) {
        DELETE_IF_NON_NULL(value);
        DELETE_IF_NON_NULL(variability);
        DELETE_IF_NON_NULL(validation);
        return nullptr;
    }


    if (value == nullptr) {
        setStatus(Status::Error, "No value in a PK parameter");
        DELETE_IF_NON_NULL(value);
        DELETE_IF_NON_NULL(variability);
        DELETE_IF_NON_NULL(validation);
        return nullptr;
    }

    parameter = new ParameterDefinition(id, value->getValue(), value->getOperation(), std::unique_ptr<ParameterVariability>(variability));
    parameter->setUnit(unit);
    parameter->setValidation(std::unique_ptr<Operation>(validation));

    DELETE_IF_NON_NULL(value);

    return parameter;
}

std::vector<Correlation*> DrugModelImport::extractCorrelations(Tucuxi::Common::XmlNodeIterator _node)
{

    std::vector<Correlation *> correlations;

    XmlNodeIterator it = _node->getChildren();

    while (it != XmlNodeIterator::none()) {
        std::string nodeName = it->getName();
        if (nodeName == "correlation") {
            Correlation *correlation;
            correlation = extractCorrelation(it);
            if (correlation != nullptr) {
                correlations.push_back(correlation);
            }
        }
        else {
            unexpectedTag(nodeName);
        }
        it ++;
    }

    return correlations;
}

Correlation* DrugModelImport::extractCorrelation(Tucuxi::Common::XmlNodeIterator _node)
{

    Correlation *correlation;
    std::string param1;
    std::string param2;
    Value value = 0.0;

    XmlNodeIterator it = _node->getChildren();

    while (it != XmlNodeIterator::none()) {
        std::string nodeName = it->getName();
        if (nodeName == "param1") {
            param1 = it->getValue();
        }
        else if (nodeName == "param2") {
            param2 = it->getValue();
        }
        else if (nodeName == "value") {
            value = extractDouble(it);
        }
        else {
            unexpectedTag(nodeName);
        }

        it ++;
    }

    if (getStatus() != Status::Ok) {
        return nullptr;
    }

    correlation = new Correlation(param1, param2, value);

    return correlation;
}

ParameterVariability* DrugModelImport::extractVariability(Tucuxi::Common::XmlNodeIterator _node)
{
    ParameterVariability *variability = nullptr;
    ParameterVariabilityType type = ParameterVariabilityType::None;
    std::vector<Value> stdDevs;

    XmlNodeIterator it = _node->getChildren();

    while (it != XmlNodeIterator::none()) {
        std::string nodeName = it->getName();
        if (nodeName == "bsvType") {
            type = extractParameterVariabilityType(it);
        }
        else if (nodeName == "stdDevs") {

            XmlNodeIterator stdDevIt = it->getChildren();

            while (stdDevIt != XmlNodeIterator::none()) {
                if (stdDevIt->getName() == "stdDev") {
                    Value value = extractDouble(stdDevIt);
                    if (getStatus() == Status::Ok) {
                        stdDevs.push_back(value);
                    }
                }
                else {
                    unexpectedTag(stdDevIt->getName());
                }

                stdDevIt ++;
            }
        }
        else {
            unexpectedTag(nodeName);
        }

        it ++;
    }

    if (getStatus() != Status::Ok) {
        return nullptr;
    }

    variability = new ParameterVariability(type, stdDevs);

    return variability;
}


FormulationAndRoutes* DrugModelImport::extractFullFormulationAndRoutes(
        Tucuxi::Common::XmlNodeIterator _node,
        const std::vector<AnalyteSet *>& _analyteSets)
{

    FormulationAndRoutes *formulationAndRoutes;
    std::vector<FullFormulationAndRoute *> formulationAndRoutesVector;

    std::string defaultId;
    XmlAttribute attribute = _node->getAttribute("default");
    defaultId = attribute.getValue();

    XmlNodeIterator it = _node->getChildren();

    while (it != XmlNodeIterator::none()) {
        std::string nodeName = it->getName();
        if (nodeName == "formulationAndRoute") {
            FullFormulationAndRoute *formulationAndRoute;
            formulationAndRoute = extractFullFormulationAndRoute(it, _analyteSets);
            if (formulationAndRoute != nullptr) {
                formulationAndRoutesVector.push_back(formulationAndRoute);
            }
        }
        else {
            unexpectedTag(nodeName);
        }
        it ++;
    }

    if (getStatus() != Status::Ok) {
        DELETE_PVECTOR(formulationAndRoutesVector);
        return nullptr;
    }

    formulationAndRoutes = new FormulationAndRoutes();

    for (const auto & f : formulationAndRoutesVector) {
        // We insert the formulation and route and calculate if it is the default one
        formulationAndRoutes->add(std::unique_ptr<FullFormulationAndRoute>(f), (f->getId() == defaultId));
    }

    return formulationAndRoutes;
}

FullFormulationAndRoute* DrugModelImport::extractFullFormulationAndRoute(
        Tucuxi::Common::XmlNodeIterator _node,
        const std::vector<AnalyteSet *>& _analyteSets)
{

    FullFormulationAndRoute *formulationAndRoute = nullptr;
    std::string formulationAndRouteId;
    Formulation formulation = Formulation::Undefined;
    std::string administrationName;
    AdministrationRoute administrationRoute = AdministrationRoute::Undefined;
    AbsorptionModel absorptionModelId = AbsorptionModel::Undefined;
    ParameterSetDefinition *absorptionParameters = nullptr;
    std::vector<AnalyteSetToAbsorptionAssociation *> associations;
    std::vector<AnalyteConversion *> analyteConversions;

    ValidDoses *availableDoses = nullptr;
    ValidDurations *intervals = nullptr;
    ValidDurations *infusions = nullptr;
    StandardTreatment *standardTreatment = nullptr;

    XmlNodeIterator it = _node->getChildren();

    while (it != XmlNodeIterator::none()) {
        std::string nodeName = it->getName();
        if (nodeName == "formulationAndRouteId") {
            formulationAndRouteId = it->getValue();
        }
        else if (nodeName == "formulation") {
            formulation =extractFormulation(it);
        }
        else if (nodeName == "administrationName") {
            administrationName = it->getValue();
        }
        else if (nodeName == "administrationRoute") {
            administrationRoute = extractAdministrationRoute(it);
        }
        else if (nodeName == "absorptionModelId") {
            absorptionModelId = extractAbsorptionModel(it);
        }
        else if (nodeName == "dosages") {
            XmlNodeIterator dosageIt = it->getChildren();

            while (dosageIt != XmlNodeIterator::none()) {
                std::string nName = dosageIt->getName();
                if (nName == "standardTreatment") {
                    XmlNodeIterator stdIt = dosageIt->getChildren();
                    bool isFixedDuration = false;
                    TucuUnit unit;
                    double value = 0.0;
                    while (stdIt != XmlNodeIterator::none()) {
                        std::string stdName = stdIt->getName();
                        if (stdName == "isFixedDuration"){
                            isFixedDuration = extractBool(stdIt);
                        }
                        else if (stdName == "timeValue") {
                            XmlNodeIterator timeIt = stdIt->getChildren();
                            while (timeIt != XmlNodeIterator::none()) {
                                std::string timeName = timeIt->getName();
                                if (timeName == "unit") {
                                    unit = extractUnit(timeIt, CheckUnit::Check);
                                }
                                else if (timeName == "value") {
                                    value = extractDouble(timeIt);
                                }
                                else {
                                    unexpectedTag(timeName);
                                }
                                timeIt ++;
                            }
                        }
                        else {
                            unexpectedTag(stdName);
                        }

                        stdIt ++;
                    }

                    standardTreatment = new StandardTreatment(isFixedDuration, value, unit);

                }
                else if (nName == "analyteConversions") {
                    XmlNodeIterator analyteConversionsIt = dosageIt->getChildren();

                    while (analyteConversionsIt != XmlNodeIterator::none()) {
                        std::string analyteId;
                        Value factor = 0.0;

                        if (analyteConversionsIt->getName() == "analyteConversion") {
                            XmlNodeIterator analyteConversionIt = analyteConversionsIt->getChildren();
                            while (analyteConversionIt != XmlNodeIterator::none()) {
                                std::string n = analyteConversionIt->getName();

                                if (n == "analyteId") {
                                    analyteId = analyteConversionIt->getValue();
                                }
                                else if (n == "factor") {
                                    factor = extractDouble(analyteConversionIt);
                                }
                                analyteConversionIt ++;
                            }

                        }

                        if (getStatus() == Status::Ok) {
                            analyteConversions.push_back(new AnalyteConversion(analyteId, factor));
                        }

                        analyteConversionsIt ++;
                    }

                }
                else if (nName == "availableDoses") {
                    availableDoses = extractValidDoses(dosageIt);
                }
                else if ((nName == "availableIntervals") || (nName == "intervals")) {
                    intervals = extractValidDurations(dosageIt);
                }
                else if ((nName == "availableInfusions") || (nName == "infusions")) {
                    infusions = extractValidDurations(dosageIt);
                }
                else {
                    unexpectedTag(nName);
                }
                dosageIt ++;
            }

        }
        else if (nodeName == "absorptionParameters") {

            XmlNodeIterator absorptionIt = it->getChildren();

            std::string analyteGroupId;
            AnalyteSet *selectedAnalyteSet = nullptr;
            AbsorptionModel absorptionModelId = AbsorptionModel::Undefined;
            absorptionParameters = nullptr;

            while (absorptionIt != XmlNodeIterator::none()) {

                std::string nName = absorptionIt->getName();
                if (nName == "parameterSetAnalyteGroup") {

                    XmlNodeIterator pIt = absorptionIt->getChildren();

                    while (pIt != XmlNodeIterator::none()) {
                        std::string pName = pIt->getName();
                        if (pName == "analyteGroupId") {
                            analyteGroupId = pIt->getValue();
                            for (const auto & analyteSet : _analyteSets) {
                                if (analyteSet->getId() == analyteGroupId) {
                                    selectedAnalyteSet = analyteSet;
                                }
                            }
                            if (selectedAnalyteSet == nullptr) {
                                setNodeError(pIt);
                            }
                        }
                        else if (pName == "absorptionModelId") {
                            absorptionModelId = extractAbsorptionModel(pIt);
                        }
                        else if (pName == "parameterSet") {
                            absorptionParameters = extractParameterSet(pIt);
                        }
                        pIt++;
                    }
                }
                else {
                    unexpectedTag(nName);
                }
                absorptionIt ++;

                if ((getStatus() == Status::Ok) && (absorptionParameters != nullptr)) {
                    if (selectedAnalyteSet != nullptr) {
                        AnalyteSetToAbsorptionAssociation *association = new AnalyteSetToAbsorptionAssociation(*selectedAnalyteSet);
                        association->setAbsorptionParameters(std::unique_ptr<ParameterSetDefinition>(absorptionParameters));
                        association->setAbsorptionModel(absorptionModelId);
                        associations.push_back(association);
                    }
                }

            }


        }
        else {
            unexpectedTag(nodeName);
        }

        it ++;
    }

    if (getStatus() != Status::Ok) {
        DELETE_IF_NON_NULL(absorptionParameters);
        DELETE_IF_NON_NULL(availableDoses);
        DELETE_IF_NON_NULL(intervals);
        DELETE_IF_NON_NULL(infusions);
        DELETE_IF_NON_NULL(standardTreatment);
        DELETE_PVECTOR(analyteConversions);
        DELETE_PVECTOR(associations);
        return nullptr;
    }

    FormulationAndRoute spec(formulation, administrationRoute, absorptionModelId, administrationName);

    formulationAndRoute = new FullFormulationAndRoute(spec,formulationAndRouteId);
    formulationAndRoute->setValidDoses(std::unique_ptr<ValidDoses>(availableDoses));
    formulationAndRoute->setValidIntervals(std::unique_ptr<ValidDurations>(intervals));
    formulationAndRoute->setValidInfusionTimes(std::unique_ptr<ValidDurations>(infusions));
    for (const auto & analyteConversion : analyteConversions) {
        formulationAndRoute->addAnalyteConversion(std::unique_ptr<AnalyteConversion>(analyteConversion));
    }
    for(const auto & association : associations) {
        formulationAndRoute->addAssociation(std::unique_ptr<AnalyteSetToAbsorptionAssociation>(association));
    }
    if (standardTreatment != nullptr) {
        formulationAndRoute->setStandardTreatment(std::unique_ptr<StandardTreatment>(standardTreatment));
    }

    return formulationAndRoute;
}


ValidDurations* DrugModelImport::extractValidDurations(Tucuxi::Common::XmlNodeIterator _node)
{

    ValidDurations *validDurations;
    TucuUnit unit;
    LightPopulationValue *defaultValue = nullptr;
    ValidValuesRange *valuesRange = nullptr;
    ValidValuesFixed *valuesFixed = nullptr;

    std::vector<FullFormulationAndRoute *> formulationAndRoutesVector;

    std::string defaultId;
    XmlAttribute attribute = _node->getAttribute("default");
    defaultId = attribute.getValue();

    XmlNodeIterator it = _node->getChildren();

    while (it != XmlNodeIterator::none()) {
        std::string nodeName = it->getName();
        if (nodeName == "unit") {
            unit = extractUnit(it, CheckUnit::Check);
        }
        else if (nodeName =="default") {
            defaultValue = extractPopulationValue(it);
        }
        else if (nodeName == "rangeValues") {
            valuesRange = extractValuesRange(it);
        }
        else if (nodeName == "fixedValues") {
            valuesFixed = extractValuesFixed(it);
        }
        else {
            unexpectedTag(nodeName);
        }

        it ++;
    }

    if (getStatus() != Status::Ok) {
        DELETE_PVECTOR(formulationAndRoutesVector);
        DELETE_IF_NON_NULL(defaultValue);
        DELETE_IF_NON_NULL(valuesRange);
        DELETE_IF_NON_NULL(valuesFixed);
        return nullptr;
    }

    if (defaultValue == nullptr) {
        setStatus(Status::Error, "No default value in valid durations.");
        DELETE_PVECTOR(formulationAndRoutesVector);
        DELETE_IF_NON_NULL(defaultValue);
        DELETE_IF_NON_NULL(valuesRange);
        DELETE_IF_NON_NULL(valuesFixed);
        return nullptr;
    }

    validDurations = new ValidDurations(unit, std::make_unique<PopulationValue>("default", defaultValue->getValue(), defaultValue->getOperation()));

    if (valuesRange != nullptr) {
        validDurations->addValues(std::unique_ptr<IValidValues>(valuesRange));
    }
    if (valuesFixed != nullptr) {
        validDurations->addValues(std::unique_ptr<IValidValues>(valuesFixed));
    }

    DELETE_IF_NON_NULL(defaultValue);

    return validDurations;

}

ValidDoses* DrugModelImport::extractValidDoses(Tucuxi::Common::XmlNodeIterator _node)
{

    ValidDoses *validDoses;
    TucuUnit unit;
    LightPopulationValue *defaultValue = nullptr;
    std::vector<ValidValuesRange *> valuesRange;
    std::vector<ValidValuesFixed *> valuesFixed;

    std::vector<FullFormulationAndRoute *> formulationAndRoutesVector;

    std::string defaultId;
    XmlAttribute attribute = _node->getAttribute("default");
    defaultId = attribute.getValue();

    XmlNodeIterator it = _node->getChildren();

    while (it != XmlNodeIterator::none()) {
        std::string nodeName = it->getName();
        if (nodeName == "unit") {
            unit = extractUnit(it, CheckUnit::Check);
        }
        else if (nodeName =="default") {
            defaultValue = extractPopulationValue(it);
        }
        else if (nodeName == "rangeValues") {
            ValidValuesRange *v = extractValuesRange(it);
            if (v != nullptr) {
                valuesRange.push_back(v);
            }
        }
        else if (nodeName == "fixedValues") {
            ValidValuesFixed *v = extractValuesFixed(it);
            if (v != nullptr) {
                valuesFixed.push_back(v);
            }
        }
        else {
            unexpectedTag(nodeName);
        }

        it ++;
    }

    if (getStatus() != Status::Ok) {
        DELETE_PVECTOR(formulationAndRoutesVector);
        DELETE_IF_NON_NULL(defaultValue);
        DELETE_PVECTOR(valuesRange);
        DELETE_PVECTOR(valuesFixed);
        return nullptr;
    }

    if (defaultValue == nullptr) {
        setStatus(Status::Error, "No default value in valid doses.");
        DELETE_PVECTOR(formulationAndRoutesVector);
        DELETE_IF_NON_NULL(defaultValue);
        DELETE_PVECTOR(valuesRange);
        DELETE_PVECTOR(valuesFixed);
        return nullptr;
    }

    validDoses = new ValidDoses(unit, std::make_unique<PopulationValue>("default", defaultValue->getValue(), defaultValue->getOperation()));

    for (const auto value : valuesRange) {
        validDoses->addValues(std::unique_ptr<IValidValues>(value));
    }
    for (const auto value : valuesFixed) {
        validDoses->addValues(std::unique_ptr<IValidValues>(value));
    }

    DELETE_IF_NON_NULL(defaultValue);

    return validDoses;
}

ValidValuesRange* DrugModelImport::extractValuesRange(Tucuxi::Common::XmlNodeIterator _node)
{
    ValidValuesRange *valuesRange;
    LightPopulationValue *from = nullptr;
    LightPopulationValue *to = nullptr;
    LightPopulationValue *step = nullptr;

    XmlNodeIterator it = _node->getChildren();

    while (it != XmlNodeIterator::none()) {
        std::string nodeName = it->getName();
        if (nodeName == "from") {
            from = extractPopulationValue(it);
        }
        else if (nodeName =="to") {
            to = extractPopulationValue(it);
        }
        else if (nodeName == "step") {
            step = extractPopulationValue(it);
        }
        else {
            unexpectedTag(nodeName);
        }

        it ++;
    }

    if ((from == nullptr) || (to == nullptr) || (step == nullptr)){
        setNodeError(_node);
        DELETE_IF_NON_NULL(from);
        DELETE_IF_NON_NULL(to);
        DELETE_IF_NON_NULL(step);
        return nullptr;
    }

    if (getStatus() != Status::Ok){
        DELETE_IF_NON_NULL(from);
        DELETE_IF_NON_NULL(to);
        DELETE_IF_NON_NULL(step);
        return nullptr;
    }

    valuesRange = new ValidValuesRange(
                std::make_unique<PopulationValue>("from", from->getValue(), from->getOperation()),
                std::make_unique<PopulationValue>("to", to->getValue(), to->getOperation()),
                std::make_unique<PopulationValue>("step", step->getValue(), step->getOperation())
                );

    DELETE_IF_NON_NULL(from);
    DELETE_IF_NON_NULL(to);
    DELETE_IF_NON_NULL(step);

    return valuesRange;
}

ValidValuesFixed* DrugModelImport::extractValuesFixed(Tucuxi::Common::XmlNodeIterator _node)
{
    ValidValuesFixed *valuesFixed = nullptr;
    std::vector<Value> values;

    XmlNodeIterator it = _node->getChildren();

    while (it != XmlNodeIterator::none()) {
        std::string nodeName = it->getName();
        if (nodeName == "value") {
            Value v = extractDouble(it);
            if (getStatus() != Status::Error) {
                values.push_back(v);
            }
        }
        else {
            unexpectedTag(nodeName);
        }

        it ++;
    }

    if (getStatus() != Status::Ok){
        DELETE_IF_NON_NULL(valuesFixed);
        return nullptr;
    }

    valuesFixed = new ValidValuesFixed();
    for (const auto & v : values) {
        valuesFixed->addValue(v);
    }

    return valuesFixed;
}


DrugModelMetadata* DrugModelImport::extractHead(Tucuxi::Common::XmlNodeIterator _node)
{
    TranslatableString drugName;
    TranslatableString drugDescription;
    TranslatableString studyName;
    TranslatableString distribution;
    TranslatableString elimination;
    TranslatableString description;
    std::string authorName;
    std::vector<std::string> atcs;

    XmlNodeIterator it = _node->getChildren();

    while (it != XmlNodeIterator::none()) {
        std::string nodeName = it->getName();
        if (nodeName == "drug") {
            XmlNodeIterator drugIterator = it->getChildren();

            while (drugIterator != XmlNodeIterator::none()) {

                std::string insideName = drugIterator->getName();

                if (insideName == "drugName") {
                    drugName = extractTranslatableString(drugIterator, "name");
                }
                else if (insideName == "drugDescription") {
                    drugDescription = extractTranslatableString(drugIterator, "desc");
                }
                else if (insideName == "atcs") {
                    XmlNodeIterator atcIterator = drugIterator->getChildren();
                    while (atcIterator != XmlNodeIterator::none()) {

                        std::string atcName = atcIterator->getName();

                        if (atcName == "atc") {
                            atcs.push_back(atcIterator->getValue());
                        }

                        atcIterator ++;
                    }
                }

                drugIterator ++;

            }
        }
        else if (nodeName == "study") {

            XmlNodeIterator studyIterator = it->getChildren();

            while (studyIterator != XmlNodeIterator::none()) {

                std::string insideName = studyIterator->getName();

                if (insideName == "studyName") {
                    studyName = extractTranslatableString(studyIterator, "name");
                }
                else if (insideName == "studyAuthors") {
                    authorName = studyIterator->getValue();
                }
                else if (insideName == "description") {
                    description = extractTranslatableString(studyIterator, "desc");
                }

                studyIterator ++;

            }
        }
        else if (nodeName == "modelDescription") {

            XmlNodeIterator modelIterator = it->getChildren();

            while (modelIterator != XmlNodeIterator::none()) {

                std::string insideName = modelIterator->getName();

                if (insideName == "distribution") {
                    distribution = extractTranslatableString(modelIterator, "desc");
                }
                else if (insideName == "elimination") {
                    elimination = extractTranslatableString(modelIterator, "desc");
                }

                modelIterator ++;

            }
        }
        else {
            unexpectedTag(nodeName);
        }

        it ++;
    }

    if (getStatus() != Status::Ok){
        return nullptr;
    }

    DrugModelMetadata *metaData = new DrugModelMetadata();
    metaData->setAuthorName(authorName);
    metaData->setDrugName(drugName);
    metaData->setStudyName(studyName);
    metaData->setDrugDescription(drugDescription);
    metaData->setAtcs(atcs);
    metaData->setElimination(elimination);
    metaData->setDistribution(distribution);
    metaData->setDescription(description);

    return metaData;

}


} // namespace Core
} // namespace Tucuxi

