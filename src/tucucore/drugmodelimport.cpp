#include "drugmodelimport.h"

#include <iostream>

#include "tucucommon/xmldocument.h"
#include "tucucommon/xmlnode.h"
#include "tucucommon/xmlattribute.h"
#include "tucucommon/xmliterator.h"

#include "tucucore/drugmodel/drugmodel.h"
#include "tucucore/drugdefinitions.h"
#include "tucucore/drugmodel/drugmodeldomain.h"
#include "tucucore/definitions.h"
#include "tucucore/hardcodedoperation.h"
#include "tucucore/residualerrormodel.h"


using namespace Tucuxi::Common;
using namespace Tucuxi::Core;

namespace Tucuxi {
namespace Core {


/// Macro to delete a pointer if it is not nullptr
#define DELETE_IF_NON_NULL(p) {if (p != nullptr) { delete p;}}

/// Macro to delete a vector of pointers.
/// It deletes every pointed value and emptied the vector.
#define DELETE_PVECTOR(v) { \
    while (v.size() != 0) { \
    delete v.at(v.size() - 1); \
    v.pop_back(); \
} \
}


///////////////////////////////////////////////////////////////////////////////
/// public methods
///////////////////////////////////////////////////////////////////////////////

DrugModelImport::DrugModelImport()
{

}



DrugModelImport::Result DrugModelImport::importFromFile(Tucuxi::Core::DrugModel *&_drugModel, std::string _fileName)
{
    // Ensure the function is reentrant
    std::lock_guard<std::mutex> lock(m_mutex);

    setResult(Result::Ok);
    _drugModel = nullptr;

    XmlDocument document;
    if (!document.open(_fileName)) {
        return Result::CantOpenFile;
    }

    return importDocument(_drugModel, document);
}


DrugModelImport::Result DrugModelImport::importFromString(Tucuxi::Core::DrugModel *&_drugModel, std::string _xml)
{
    // Ensure the function is reentrant
    std::lock_guard<std::mutex> lock(m_mutex);

    setResult(Result::Ok);
    _drugModel = nullptr;

    XmlDocument document;

    if (!document.fromString(_xml)) {
        return Result::Error;
    }

    return importDocument(_drugModel, document);
}


///////////////////////////////////////////////////////////////////////////////
/// General methods
///////////////////////////////////////////////////////////////////////////////

DrugModelImport::Result DrugModelImport::importDocument(
        Tucuxi::Core::DrugModel *&_drugModel,
        Tucuxi::Common::XmlDocument & _document)
{
    XmlNode root = _document.getRoot();

    XmlNodeIterator drugModelIterator = root.getChildren("drugModel");

    if (drugModelIterator == XmlNodeIterator::none()) {
        return Result::Error;
    }

    _drugModel = extractDrugModel(drugModelIterator);

    return getResult();

}



const std::vector<std::string>& DrugModelImport::ignoredTags() const {
    static std::vector<std::string> ignoredTags =
    {"comments", "description", "errorMessage", "name", "activeMoietyName"};
    return ignoredTags;
}

///////////////////////////////////////////////////////////////////////////////
/// basic types imports
///////////////////////////////////////////////////////////////////////////////


Unit DrugModelImport::extractUnit(Tucuxi::Common::XmlNodeIterator _node)
{
    std::string unitString = _node->getValue();
    if (unitString == "min")
        unitString = "m";
    Unit result(unitString);
    return result;
}

double DrugModelImport::extractDouble(Tucuxi::Common::XmlNodeIterator _node)
{
    double result;
    try {
        result = std::stod(_node->getValue());
    } catch (...) {
        setResult(Result::Error);
        result = 0.0;
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
        //{"sex", CovariateType::Sex},
        {"ageInYears", CovariateType::AgeInYears},
        {"ageInDays", CovariateType::AgeInDays},
        {"ageInMonths", CovariateType::AgeInMonths}
    };


    auto it = m.find(_node->getValue());
    if (it != m.end())
        return it->second;

    setResult(Result::Error);
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
    if (it != m.end())
        return it->second;

    setResult(Result::Error);
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
        {"cumulativeAuc", TargetType::CumulativeAuc},
        {"aucOverMic", TargetType::AucOverMic},
        {"timeOverMic", TargetType::TimeOverMic},
        {"aucDividedByMic", TargetType::AucDividedByMic},
        {"peakDividedByMic", TargetType::PeakDividedByMic}
    };

    auto it = m.find(_node->getValue());
    if (it != m.end())
        return it->second;

    setResult(Result::Error);
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
    if (it != m.end())
        return it->second;

    setResult(Result::Error);
    return InterpolationType::Direct;
}


SigmaResidualErrorModel::ResidualErrorType DrugModelImport::extractResidualErrorType(Tucuxi::Common::XmlNodeIterator _node)
{
    static std::map<std::string, SigmaResidualErrorModel::ResidualErrorType> m =
    {
        {"additive", SigmaResidualErrorModel::ResidualErrorType::ADDITIVE},
        {"proportional", SigmaResidualErrorModel::ResidualErrorType::PROPORTIONAL},
        {"exponential", SigmaResidualErrorModel::ResidualErrorType::EXPONENTIAL},
        {"mixed", SigmaResidualErrorModel::ResidualErrorType::MIXED},
        {"softcoded", SigmaResidualErrorModel::ResidualErrorType::SOFTCODED},
        {"none", SigmaResidualErrorModel::ResidualErrorType::NONE}
    };

    auto it = m.find(_node->getValue());
    if (it != m.end())
        return it->second;

    setResult(Result::Error);
    return SigmaResidualErrorModel::ResidualErrorType::NONE;
}


ParameterVariabilityType DrugModelImport::extractParameterVariabilityType(Tucuxi::Common::XmlNodeIterator _node)
{
    static std::map<std::string, ParameterVariabilityType> m =
    {
        {"normal", ParameterVariabilityType::Normal},
        {"lognormal", ParameterVariabilityType::LogNormal},
        {"proportional", ParameterVariabilityType::Proportional},
        {"additive", ParameterVariabilityType::Additive},
        {"none", ParameterVariabilityType::None}
    };

    auto it = m.find(_node->getValue());
    if (it != m.end())
        return it->second;

    setResult(Result::Error);
    return ParameterVariabilityType::None;
}

Formulation DrugModelImport::extractFormulation(Tucuxi::Common::XmlNodeIterator _node)
{

    static std::map<std::string, Formulation> m =
    {
        {"undefined", Formulation::Undefined},
        {"parenteral solution", Formulation::ParenteralSolution},
        {"oral solution", Formulation::OralSolution},
        {"test", Formulation::Test}
    };

    auto it = m.find(_node->getValue());
    if (it != m.end())
        return it->second;

    setResult(Result::Error);
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
    if (it != m.end())
        return it->second;

    setResult(Result::Error);
    return AdministrationRoute::Undefined;
}


RouteModel DrugModelImport::extractRouteModel(Tucuxi::Common::XmlNodeIterator _node)
{

    static std::map<std::string, RouteModel> m =
    {
        {"undefined", RouteModel::UNDEFINED},
        {"bolus", RouteModel::INTRAVASCULAR},
        {"extra", RouteModel::EXTRAVASCULAR},
        {"infusion", RouteModel::INFUSION}
    };

    auto it = m.find(_node->getValue());
    if (it != m.end())
        return it->second;

    setResult(Result::Error);
    return RouteModel::UNDEFINED;

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
        if (nodeName == "formula") {
            XmlNodeIterator cdataIt = it->getChildren();
            if (cdataIt == XmlNodeIterator::none()) {
                setResult(Result::Error);
            }
            else if (cdataIt->getType() != EXmlNodeType::CData) {
                setResult(Result::Error);
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
                                setResult(Result::Error);
                            }
                        }
                        else {
                            unexpectedTag(inputItName);
                        }
                        inputIt ++;
                    }

                    if (getResult() != Result::Error) {
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

    if (getResult() != Result::Ok) {
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
            operation = new GFR_MDRD();
        }
        else if (nodeName == "multiFormula") {
            // TODO : Implement multi formula parsing
        }
        else {
            unexpectedTag(nodeName);
        }
        it ++;
    }

    if (getResult() != Result::Ok) {
        DELETE_IF_NON_NULL(operation);
        return nullptr;
    }

    if (operation == nullptr) {
        setResult(Result::Error);
    }

    return operation;
}

LightPopulationValue *DrugModelImport::extractPopulationValue(Tucuxi::Common::XmlNodeIterator _node)
{
    LightPopulationValue *populationValue;
    Value value = 0.0;
    Operation *operation = nullptr;
    std::string id = "";

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
    DrugModel *drugModel;
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

    if (getResult() != Result::Ok) {
        DELETE_PVECTOR(covariates);
        DELETE_PVECTOR(activeMoieties);
        DELETE_PVECTOR(analyteSets);
        DELETE_IF_NON_NULL(formulationAndRoutes);
        DELETE_IF_NON_NULL(timeConsiderations);
        DELETE_IF_NON_NULL(domain);
        return nullptr;
    }

    drugModel = new DrugModel();
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
    Unit unit;
    double multiplier = 1.0;
    //    Operation *operation;
    LightPopulationValue *value = nullptr;

    XmlNodeIterator it = _node->getChildren();

    while (it != XmlNodeIterator::none()) {
        std::string nodeName = it->getName();
        if (nodeName == "unit") {
            unit = extractUnit(it);
        }
        else if (nodeName == "value") {
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

    HalfLife *halfLife = new HalfLife(id, value->getValue(), unit, multiplier, value->getOperation());

    DELETE_IF_NON_NULL(value);

    return halfLife;
}

OutdatedMeasure* DrugModelImport::extractOutdatedMeasure(Tucuxi::Common::XmlNodeIterator _node)
{
    OutdatedMeasure *outdatedMeasure = nullptr;
    Unit unit;
    LightPopulationValue *value = nullptr;
    std::string id;

    XmlNodeIterator it = _node->getChildren();

    while (it != XmlNodeIterator::none()) {
        std::string nodeName = it->getName();
        if (nodeName == "unit") {
            unit = extractUnit(it);
        }
        else if (nodeName == "value") {
            value = extractPopulationValue(it);
        }
        else {
            unexpectedTag(nodeName);
        }
        it ++;
    }

    if (getResult() != Result::Ok) {
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

    domain = new DrugModelDomain();

    XmlNodeIterator it = _node->getChildren();

    while (it != XmlNodeIterator::none()) {
        std::string nodeName = it->getName();
        if (nodeName == "constraints") {
            constraints = extractConstraints(it);
        }
        else {
            unexpectedTag(nodeName);
        }
        it ++;
    }

    if (getResult() != Result::Ok) {
        DELETE_IF_NON_NULL(domain);
        return nullptr;
    }

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
            if (constraint) {
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
    Operation *operation;

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
                setResult(Result::Error);
            }
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

    if (getResult() != Result::Ok) {
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
            if (covariate) {
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

CovariateDefinition* DrugModelImport::extractCovariate(Tucuxi::Common::XmlNodeIterator _node)
{
    CovariateDefinition* covariate;

    std::string id;
    CovariateType type = CovariateType::Standard;
    DataType dataType = DataType::Int;
    Unit unit;
    InterpolationType interpolationType = InterpolationType::Direct;
    LightPopulationValue *value = nullptr;
    Operation *validation = nullptr;

    XmlNodeIterator it = _node->getChildren();

    while (it != XmlNodeIterator::none()) {
        std::string nodeName = it->getName();
        if (nodeName == "id") {
            id = it->getValue();
        }
        else if (nodeName == "unit") {
            unit = extractUnit(it);
        }
        else if (nodeName == "type") {
            type = extractCovariateType(it);
        }
        else if (nodeName == "dataType") {
            dataType = extractDataType(it);
        }
        else if (nodeName == "interpolationType") {
            interpolationType = extractInterpolationType(it);
        }
        else if (nodeName == "value") {
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

    if (getResult() != Result::Ok) {
        DELETE_IF_NON_NULL(validation);
        DELETE_IF_NON_NULL(value);
        return nullptr;
    }

    // TODO : Try to find a way to allow flexible covariate values. This is odd
    std::string valueString = std::to_string(value->getValue());

    covariate = new CovariateDefinition(id, valueString, value->getOperation(), type, dataType);
    //    covariate = new CovariateDefinition(id, valueString, nullptr, type, dataType);
    covariate->setInterpolationType(interpolationType);
    covariate->setUnit(unit);
    covariate->setValidation(std::unique_ptr<Operation>(validation));

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
            if (activeMoiety) {
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

    ActiveMoiety *activeMoiety;
    std::string activeMoietyId;
    Unit unit;
    std::vector<std::string> analyteIdList;
    Operation *formula = nullptr;
    std::vector<TargetDefinition *> targets;

    XmlNodeIterator it = _node->getChildren();

    while (it != XmlNodeIterator::none()) {
        std::string nodeName = it->getName();
        if (nodeName == "activeMoietyId") {
            activeMoietyId = it->getValue();
        }
        else if (nodeName == "unit") {
            unit = extractUnit(it);
        }
        else if (nodeName == "analyteIdList") {

            XmlNodeIterator analyteIt = it->getChildren();

            while (analyteIt != XmlNodeIterator::none()) {
                if (analyteIt->getName() == "analyteId") {
                    analyteIdList.push_back(analyteIt->getValue());
                }
                else {
                    unexpectedTag(analyteIt->getName());
                }
                analyteIt ++;
            }
        }
        else if (nodeName == "formula") {
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

    if (getResult() != Result::Ok) {
        if (formula != nullptr) {
            delete formula;
        }
        DELETE_PVECTOR(targets);
        return nullptr;
    }

    activeMoiety = new ActiveMoiety(activeMoietyId, unit, analyteIdList, std::unique_ptr<Operation>(formula));
    for (const auto & target : targets) {
        activeMoiety->addTarget(std::unique_ptr<TargetDefinition>(target));
    }

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
            if (target) {
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
    Unit unit("ug/l");
    TargetType type = TargetType::UnknownTarget;
    LightPopulationValue *minValue = nullptr;
    LightPopulationValue *maxValue = nullptr;
    LightPopulationValue *bestValue = nullptr;
    LightPopulationValue *tMin = nullptr;
    LightPopulationValue *tMax = nullptr;
    LightPopulationValue *tBest = nullptr;
    LightPopulationValue *toxicityAlarm = nullptr;
    LightPopulationValue *inefficacyAlarm = nullptr;

    XmlNodeIterator it = _node->getChildren();

    while (it != XmlNodeIterator::none()) {
        std::string nodeName = it->getName();
        if (nodeName == "targetType") {
            type = extractTargetType(it);
        }
        else if (nodeName == "unit") {
            unit = extractUnit(it);
        }
        else if (nodeName == "targetValues") {

            XmlNodeIterator targetValuesIt = it->getChildren();

            while (targetValuesIt != XmlNodeIterator::none()) {
                std::string valueName = targetValuesIt->getName();

                if (valueName == "min") {
                    minValue = extractPopulationValue(targetValuesIt);
                }
                else if (valueName == "max") {
                    maxValue = extractPopulationValue(targetValuesIt);
                }
                else if (valueName == "best") {
                    bestValue = extractPopulationValue(targetValuesIt);
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
            TargetType extractTargetType(Tucuxi::Common::XmlNodeIterator _node);
        }
        else if (nodeName == "times") {

            XmlNodeIterator timesIt = _node->getChildren();

            while (timesIt != XmlNodeIterator::none()) {
                std::string valueName = timesIt->getName();

                if (valueName == "min") {
                    tMin = extractPopulationValue(it);
                }
                else if (valueName == "max") {
                    tMax = extractPopulationValue(it);
                }
                else if (valueName == "best") {
                    tBest = extractPopulationValue(it);
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

    if (getResult() != Result::Ok) {
        DELETE_IF_NON_NULL(minValue);
        DELETE_IF_NON_NULL(maxValue);
        DELETE_IF_NON_NULL(bestValue);
        DELETE_IF_NON_NULL(tMin);
        DELETE_IF_NON_NULL(tMax);
        DELETE_IF_NON_NULL(tBest);
        DELETE_IF_NON_NULL(toxicityAlarm);
        DELETE_IF_NON_NULL(inefficacyAlarm);
        return nullptr;
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

    target = new TargetDefinition(type, unit, "",
                                  std::make_unique<SubTargetDefinition>("cMin", minValue->getValue(), minValue->getOperation()),
                                  std::make_unique<SubTargetDefinition>("cMax", maxValue->getValue(), maxValue->getOperation()),
                                  std::make_unique<SubTargetDefinition>("cBest", bestValue->getValue(), bestValue->getOperation()),
                                  std::make_unique<SubTargetDefinition>("tMin", tMin->getValue(), tMin->getOperation()),
                                  std::make_unique<SubTargetDefinition>("tMax", tMax->getValue(), tMax->getOperation()),
                                  std::make_unique<SubTargetDefinition>("tBest", tBest->getValue(), tBest->getOperation()),
                                  std::make_unique<SubTargetDefinition>("toxicity", toxicityAlarm->getValue(), toxicityAlarm->getOperation()),
                                  std::make_unique<SubTargetDefinition>("inefficacy", inefficacyAlarm->getValue(), inefficacyAlarm->getOperation())
                                  );


    DELETE_IF_NON_NULL(minValue);
    DELETE_IF_NON_NULL(maxValue);
    DELETE_IF_NON_NULL(bestValue);
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
            if (analyteGroup) {
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

    if (getResult() != Result::Ok) {
        DELETE_PVECTOR(analytes);
        DELETE_IF_NON_NULL(parameters);
        return nullptr;
    }

    analyteGroup = new AnalyteSet();
    analyteGroup->setId(groupId);
    analyteGroup->setPkModelId(pkModelId);
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
            if (analyte) {
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
    Unit unit;
    MolarMass *molarMass = nullptr;
    IResidualErrorModel *errorModel = nullptr;

    XmlNodeIterator it = _node->getChildren();

    while (it != XmlNodeIterator::none()) {
        std::string nodeName = it->getName();
        if (nodeName == "analyteId") {
            analyteId = it->getValue();
        }
        else if (nodeName == "unit") {
            unit = extractUnit(it);
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

    if (getResult() != Result::Ok) {
        DELETE_IF_NON_NULL(molarMass);
        DELETE_IF_NON_NULL(errorModel);
        return nullptr;
    }

    analyte = new Analyte(analyteId, unit, *molarMass);
    analyte->setResidualErrorModel(std::unique_ptr<IResidualErrorModel>(errorModel));

    DELETE_IF_NON_NULL(molarMass);

    return analyte;
}

MolarMass* DrugModelImport::extractMolarMass(Tucuxi::Common::XmlNodeIterator _node)
{
    MolarMass *molarMass = nullptr;
    Value value = 0.0;
    Unit unit;

    XmlNodeIterator it = _node->getChildren();

    while (it != XmlNodeIterator::none()) {
        std::string nodeName = it->getName();
        if (nodeName == "value") {
            value = extractDouble(it);
        }
        else if (nodeName == "unit") {
            unit = extractUnit(it);
        }
        else {
            unexpectedTag(nodeName);
        }

        it ++;
    }

    if (getResult() != Result::Ok) {
        return nullptr;
    }

    molarMass = new MolarMass(value, unit);
    return molarMass;
}

IResidualErrorModel* DrugModelImport::extractErrorModel(Tucuxi::Common::XmlNodeIterator _node)
{
    SigmaResidualErrorModel *errorModel = nullptr;
    SigmaResidualErrorModel::ResidualErrorType type = SigmaResidualErrorModel::ResidualErrorType::NONE;
    Operation *formula = nullptr;
    std::vector<LightPopulationValue*> sigmas;

    XmlNodeIterator it = _node->getChildren();

    while (it != XmlNodeIterator::none()) {
        std::string nodeName = it->getName();
        if (nodeName == "errorModelType") {
            type = extractResidualErrorType(it);
        }
        else if (nodeName == "formula") {
            formula = extractOperation(it);
        }
        else if (nodeName == "sigmas") {
            XmlNodeIterator sigmaIt = it->getChildren();

            while (sigmaIt != XmlNodeIterator::none()) {
                if (sigmaIt->getName() == "sigma") {
                    LightPopulationValue* value = extractPopulationValue(sigmaIt);
                    if (getResult() == Result::Ok) {
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

    if (getResult() != Result::Ok) {
        DELETE_PVECTOR(sigmas);
        return nullptr;
    }

    errorModel = new SigmaResidualErrorModel();
    errorModel->setErrorModel(type);
    for (const auto & sigma : sigmas) {
        errorModel->addOriginalSigma(std::make_unique<PopulationValue>("", sigma->getValue(), sigma->getOperation()));
    }
    errorModel->setFormula(std::unique_ptr<Operation>(formula));
    errorModel->generatePopulationSigma();

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

    if (getResult() != Result::Ok) {
        DELETE_PVECTOR(parameters);
        DELETE_PVECTOR(correlations);

        return nullptr;
    }

    parameterSet = new ParameterSetDefinition();

    for (const auto & correlation : correlations) {
        parameterSet->addCorrelation(*correlation);
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
            if (parameter) {
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
    Unit unit;
    LightPopulationValue *value = nullptr;
    ParameterVariability *variability = nullptr;
    Operation *validation = nullptr;

    XmlNodeIterator it = _node->getChildren();

    while (it != XmlNodeIterator::none()) {
        std::string nodeName = it->getName();
        if (nodeName == "id") {
            id = it->getValue();
        }
        else if (nodeName == "unit") {
            unit = extractUnit(it);
        }
        else if (nodeName == "value") {
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

    if (getResult() != Result::Ok) {
        DELETE_IF_NON_NULL(value);
        DELETE_IF_NON_NULL(variability);
        DELETE_IF_NON_NULL(validation);
        return nullptr;
    }

    parameter = new ParameterDefinition(id, value->getValue(), value->getOperation(), std::unique_ptr<ParameterVariability>(variability));
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
            if (correlation) {
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

    if (getResult() != Result::Ok) {
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
                    if (getResult() == Result::Ok) {
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

    if (getResult() != Result::Ok) {
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
            if (formulationAndRoute) {
                formulationAndRoutesVector.push_back(formulationAndRoute);
            }
        }
        else {
            unexpectedTag(nodeName);
        }
        it ++;
    }

    if (getResult() != Result::Ok) {
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
    std::string formulationAndRouteId = "";
    Formulation formulation = Formulation::Undefined;
    std::string administrationName = "";
    AdministrationRoute administrationRoute = AdministrationRoute::Undefined;
    RouteModel routeModelId = RouteModel::UNDEFINED;
    ParameterSetDefinition *absorptionParameters = nullptr;
    std::vector<AnalyteSetToAbsorptionAssociation *> associations;

    ValidDoses *availableDoses = nullptr;
    ValidDurations *intervals = nullptr;
    ValidDurations *infusions = nullptr;

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
        else if (nodeName == "routeModelId") {
            routeModelId = extractRouteModel(it);
        }
        else if (nodeName == "dosages") {
            XmlNodeIterator dosageIt = it->getChildren();

            while (dosageIt != XmlNodeIterator::none()) {
                std::string nName = dosageIt->getName();
                if (nName == "analyteConversions") {

                }
                else if (nName == "availableDoses") {
                    availableDoses = extractValidDoses(dosageIt);
                }
                else if (nName == "intervals") {
                    intervals = extractValidDurations(dosageIt);
                }
                else if (nName == "infusions") {
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

            std::string analyteGroupId = "";
            AnalyteSet *selectedAnalyteSet = nullptr;
            RouteModel absorptionModelId = RouteModel::UNDEFINED;
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
                        }
                        else if (pName == "routeModelId") {
                            absorptionModelId = extractRouteModel(pIt);
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
            }

            if ((getResult() == Result::Ok) && (absorptionParameters != nullptr)) {
                AnalyteSetToAbsorptionAssociation *association = new AnalyteSetToAbsorptionAssociation(*selectedAnalyteSet);
                association->setAbsorptionParameters(std::unique_ptr<ParameterSetDefinition>(absorptionParameters));
                association->setRouteModel(absorptionModelId);
                associations.push_back(association);
            }

        }
        else {
            unexpectedTag(nodeName);
        }

        it ++;
    }

    if (getResult() != Result::Ok) {
        DELETE_IF_NON_NULL(absorptionParameters);
        DELETE_IF_NON_NULL(availableDoses);
        DELETE_IF_NON_NULL(intervals);
        DELETE_IF_NON_NULL(infusions);
        return nullptr;
    }

    FormulationAndRoute spec(formulation, administrationRoute, routeModelId, administrationName);

    formulationAndRoute = new FullFormulationAndRoute(spec,formulationAndRouteId);
    formulationAndRoute->setValidDoses(std::unique_ptr<ValidDoses>(availableDoses));
    formulationAndRoute->setValidIntervals(std::unique_ptr<ValidDurations>(intervals));
    formulationAndRoute->setValidInfusionTimes(std::unique_ptr<ValidDurations>(infusions));
    for(const auto & association : associations) {
        formulationAndRoute->addAssociation(std::unique_ptr<AnalyteSetToAbsorptionAssociation>(association));
    }

    return formulationAndRoute;
}


ValidDurations* DrugModelImport::extractValidDurations(Tucuxi::Common::XmlNodeIterator _node)
{

    ValidDurations *validDurations;
    Unit unit;
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
            unit = extractUnit(it);
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

    if (getResult() != Result::Ok) {
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
    Unit unit;
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
            unit = extractUnit(it);
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

    if (getResult() != Result::Ok) {
        DELETE_PVECTOR(formulationAndRoutesVector);
        DELETE_IF_NON_NULL(defaultValue);
        DELETE_IF_NON_NULL(valuesRange);
        DELETE_IF_NON_NULL(valuesFixed);
        return nullptr;
    }

    validDoses = new ValidDoses(unit, std::make_unique<PopulationValue>("default", defaultValue->getValue(), defaultValue->getOperation()));

    if (valuesRange != nullptr) {
        validDoses->addValues(std::unique_ptr<IValidValues>(valuesRange));
    }
    if (valuesFixed != nullptr) {
        validDoses->addValues(std::unique_ptr<IValidValues>(valuesFixed));
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
        setResult(Result::Error);
    }

    if (getResult() != Result::Ok){
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
            if (getResult() != Result::Error) {
                values.push_back(v);
            }
        }
        else {
            unexpectedTag(nodeName);
        }

        it ++;
    }

    if (getResult() != Result::Ok){
        DELETE_IF_NON_NULL(valuesFixed);
        return nullptr;
    }

    valuesFixed = new ValidValuesFixed();
    for (const auto & v : values) {
        valuesFixed->addValue(v);
    }

    return valuesFixed;
}


} // namespace Core
} // namespace Tucuxi

