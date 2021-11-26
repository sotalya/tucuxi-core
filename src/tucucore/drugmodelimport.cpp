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

/*
/// Function to delete a pointer if it is not nullptr
template<typename T> inline void DELETE_IF_NON_NULL(T _p)
{
    // Removed the if (_p == nullptr) because of readability-delete-null-pointer
    delete _p;
}

/// Function to delete a vector of pointers.
/// It deletes every pointed value and emptied the vector.
template<typename T> inline void DELETE_PVECTOR(T _v) {
    while (!_v.empty()) {
        delete _v.back();
        _v.pop_back();
    }
}
*/


///////////////////////////////////////////////////////////////////////////////
/// public methods
///////////////////////////////////////////////////////////////////////////////

DrugModelImport::DrugModelImport() = default;



DrugModelImport::Status DrugModelImport::importFromFile(Tucuxi::Core::DrugModel *&_drugModel, const std::string& _fileName)
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


DrugModelImport::Status DrugModelImport::importFromString(Tucuxi::Core::DrugModel *&_drugModel, const std::string& _xml)
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

    _drugModel = extractDrugModel(drugModelIterator).release();

    if (getStatus() != Status::Ok) {
        return getStatus();
    }

    XmlNodeIterator metadataIterator = root.getChildren("head");

    if (metadataIterator == XmlNodeIterator::none()) {
        return Status::Error;
    }

    auto metaData = extractHead(metadataIterator);


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
        _drugModel->setMetadata(std::move(metaData));
    }

    return getStatus();
}



const std::vector<std::string>& DrugModelImport::ignoredTags() const {
    static std::vector<std::string> ignoredTags =
    {"comments", "description", "errorMessage", "name"};
    return ignoredTags;
}


Tucuxi::Common::TranslatableString DrugModelImport::extractTranslatableString(Tucuxi::Common::XmlNodeIterator _node, const std::string& _insideName)
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



std::unique_ptr<JSOperation> DrugModelImport::extractJSOperation(Tucuxi::Common::XmlNodeIterator _node)
{
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

    auto operation = std::make_unique<JSOperation>(formula, operationInputList);

    return operation;
}


std::unique_ptr<Operation> DrugModelImport::extractOperation(Tucuxi::Common::XmlNodeIterator _node)
{
    std::unique_ptr<Operation> operation;

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
                operation = std::unique_ptr<Operation>(sharedOperation->clone().release());
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
        return nullptr;
    }

    if (operation == nullptr) {
        setNodeError(_node);
    }

    return operation;
}

std::unique_ptr<LightPopulationValue> DrugModelImport::extractPopulationValue(Tucuxi::Common::XmlNodeIterator _node)
{
    Value value = 0.0;
    std::unique_ptr<Operation> operation;
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

    auto populationValue = std::make_unique<LightPopulationValue>();
    populationValue->m_value = value;
    populationValue->m_operation = operation.release();

    return populationValue;
}



std::unique_ptr<DrugModel> DrugModelImport::extractDrugModel(Tucuxi::Common::XmlNodeIterator _node)
{
    XmlNodeIterator it = _node->getChildren();

    std::unique_ptr<TimeConsiderations> timeConsiderations = nullptr;
    std::unique_ptr<DrugModelDomain> domain = nullptr;
    std::vector<std::unique_ptr<CovariateDefinition> > covariates;
    std::vector<std::unique_ptr<ActiveMoiety> > activeMoieties;
    std::vector<std::unique_ptr<AnalyteSet> > analyteSets;
    std::unique_ptr<FormulationAndRoutes> formulationAndRoutes = nullptr;


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
        return nullptr;
    }

    auto drugModel = std::make_unique<DrugModel>();
    drugModel->setDrugId(drugId);
    drugModel->setDrugModelId(drugModelId);
    drugModel->setFormulationAndRoutes(std::move(formulationAndRoutes));


    drugModel->setTimeConsiderations(std::move(timeConsiderations));
    drugModel->setDomain(std::move(domain));
    for (auto & covariate : covariates) {
        drugModel->addCovariate(std::move(covariate));
    }
    for (auto & activeMoiety : activeMoieties) {
        drugModel->addActiveMoiety(std::move(activeMoiety));
    }
    for (auto & analyteSet : analyteSets) {
        drugModel->addAnalyteSet(std::move(analyteSet));
    }

    return drugModel;

}


std::unique_ptr<TimeConsiderations> DrugModelImport::extractTimeConsiderations(Tucuxi::Common::XmlNodeIterator _node)
{
    auto timeConsiderations = std::make_unique<TimeConsiderations>();
    std::unique_ptr<HalfLife> halfLife = nullptr;
    std::unique_ptr<OutdatedMeasure> outdatedMeasure = nullptr;

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

    timeConsiderations->setHalfLife(std::move(halfLife));
    timeConsiderations->setOutdatedMeasure(std::move(outdatedMeasure));
    return timeConsiderations;
}


std::unique_ptr<HalfLife> DrugModelImport::extractHalfLife(Tucuxi::Common::XmlNodeIterator _node)
{
    std::string id;
    //    Value value;
    TucuUnit unit;
    double multiplier = 1.0;
    //    Operation *operation;
    std::unique_ptr<LightPopulationValue> value = nullptr;

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

    auto halfLife = std::make_unique<HalfLife>(id, value->getValue(), unit, multiplier, value->getOperation());

    return halfLife;
}

std::unique_ptr<OutdatedMeasure> DrugModelImport::extractOutdatedMeasure(Tucuxi::Common::XmlNodeIterator _node)
{
    TucuUnit unit;
    std::unique_ptr<LightPopulationValue> value = nullptr;
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
        return nullptr;
    }

    if (value == nullptr) {
        setStatus(Status::Error, "No value in outdate measure");
        return nullptr;
    }

    auto outdatedMeasure = std::make_unique<OutdatedMeasure>(id, value->getValue(), unit, value->getOperation());

    return outdatedMeasure;
}

std::unique_ptr<DrugModelDomain> DrugModelImport::extractDrugDomain(Tucuxi::Common::XmlNodeIterator _node)
{
    std::vector<std::unique_ptr<Constraint> > constraints;
    TranslatableString domainDescription;

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
        return nullptr;
    }


    auto domain = std::make_unique<DrugModelDomain>();
    domain->setDescription(domainDescription);

    for(auto & constraint : constraints) {
        domain->addConstraint(std::move(constraint));
    }

    return domain;
}


std::vector<std::unique_ptr<Constraint> > DrugModelImport::extractConstraints(Tucuxi::Common::XmlNodeIterator _node)
{
    std::vector<std::unique_ptr<Constraint> > constraints;

    XmlNodeIterator it = _node->getChildren();

    while (it != XmlNodeIterator::none()) {
        std::string nodeName = it->getName();
        if (nodeName == "constraint") {
            auto constraint = extractConstraint(it);
            if (constraint != nullptr) {
                constraints.push_back(std::move(constraint));
            }
        }
        else {
            unexpectedTag(nodeName);
        }
        it ++;
    }

    return constraints;
}

std::unique_ptr<Constraint> DrugModelImport::extractConstraint(Tucuxi::Common::XmlNodeIterator _node)
{
    auto constraint = std::make_unique<Constraint>();
    std::unique_ptr<Operation> operation = nullptr;

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
        return nullptr;
    }

    constraint->setCheckOperation(std::move(operation));

    return constraint;
}

std::vector<std::unique_ptr<CovariateDefinition> > DrugModelImport::extractCovariates(Tucuxi::Common::XmlNodeIterator _node)
{

    std::vector<std::unique_ptr<CovariateDefinition> > covariates;

    XmlNodeIterator it = _node->getChildren();

    while (it != XmlNodeIterator::none()) {
        std::string nodeName = it->getName();
        if (nodeName == "covariate") {
            auto covariate = extractCovariate(it);
            if (covariate != nullptr) {
                covariates.push_back(std::move(covariate));
            }
        }
        else {
            unexpectedTag(nodeName);
        }
        it ++;
    }

    return covariates;
}

Duration castDuration(double _duration, const TucuUnit& _unit)
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

std::unique_ptr<CovariateDefinition> DrugModelImport::extractCovariate(Tucuxi::Common::XmlNodeIterator _node)
{
    std::string id;
    CovariateType type = CovariateType::Standard;
    DataType dataType = DataType::Int;
    TucuUnit unit;
    InterpolationType interpolationType = InterpolationType::Direct;
    std::unique_ptr<LightPopulationValue> value = nullptr;
    std::unique_ptr<Operation> validation = nullptr;
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
        return nullptr;
    }

    if (value == nullptr) {
        setStatus(Status::Error, "no value in a covariate");
        return nullptr;
    }

    // TODO : Try to find a way to allow flexible covariate values. This is odd
    std::string valueString = std::to_string(value->getValue());

    auto covariate = std::make_unique<CovariateDefinition>(id, valueString, value->getOperation(), type, dataType);
    covariate->setInterpolationType(interpolationType);
    covariate->setUnit(unit);
    covariate->setValidation(std::move(validation));
    covariate->setName(name);
    covariate->setRefreshPeriod(castDuration(refreshPeriodValue, refreshPeriodUnit));

    return covariate;

}



std::vector<std::unique_ptr<ActiveMoiety> > DrugModelImport::extractActiveMoieties(Tucuxi::Common::XmlNodeIterator _node)
{

    std::vector<std::unique_ptr<ActiveMoiety> > activeMoieties;

    XmlNodeIterator it = _node->getChildren();

    while (it != XmlNodeIterator::none()) {
        std::string nodeName = it->getName();
        if (nodeName == "activeMoiety") {
            auto activeMoiety = extractActiveMoiety(it);
            if (activeMoiety != nullptr) {
                activeMoieties.push_back(std::move(activeMoiety));
            }
        }
        else {
            unexpectedTag(nodeName);
        }
        it ++;
    }

    return activeMoieties;

}

std::unique_ptr<ActiveMoiety> DrugModelImport::extractActiveMoiety(Tucuxi::Common::XmlNodeIterator _node)
{
    std::string activeMoietyId;
    TucuUnit unit;
    std::vector<AnalyteId> analyteIdList;
    std::unique_ptr<Operation> formula = nullptr;
    std::vector<std::unique_ptr<TargetDefinition> > targets;
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
        return nullptr;
    }

    auto activeMoiety = std::make_unique<ActiveMoiety>(ActiveMoietyId(activeMoietyId), unit, analyteIdList, std::move(formula));
    for (auto & target : targets) {
        target->setActiveMoietyId(ActiveMoietyId(activeMoietyId));
        activeMoiety->addTarget(std::move(target));
    }
    activeMoiety->setName(name);

    return activeMoiety;
}


std::vector<std::unique_ptr<TargetDefinition> > DrugModelImport::extractTargets(Tucuxi::Common::XmlNodeIterator _node)
{

    std::vector<std::unique_ptr<TargetDefinition> > targets;

    XmlNodeIterator it = _node->getChildren();

    while (it != XmlNodeIterator::none()) {
        std::string nodeName = it->getName();
        if (nodeName == "target") {
            auto target = extractTarget(it);
            if (target != nullptr) {
                targets.push_back(std::move(target));
            }
        }
        else {
            unexpectedTag(nodeName);
        }

        it ++;
    }

    return targets;
}


std::unique_ptr<TargetDefinition> DrugModelImport::extractTarget(Tucuxi::Common::XmlNodeIterator _node)
{
    TucuUnit unit("ug/l");
    TargetType type = TargetType::UnknownTarget;
    std::unique_ptr<LightPopulationValue> minValue = nullptr;
    std::unique_ptr<LightPopulationValue> maxValue = nullptr;
    std::unique_ptr<LightPopulationValue> bestValue = nullptr;
    std::unique_ptr<LightPopulationValue> mic = nullptr;
    std::unique_ptr<LightPopulationValue> tMin = nullptr;
    std::unique_ptr<LightPopulationValue> tMax = nullptr;
    std::unique_ptr<LightPopulationValue> tBest = nullptr;
    std::unique_ptr<LightPopulationValue> toxicityAlarm = nullptr;
    std::unique_ptr<LightPopulationValue> inefficacyAlarm = nullptr;
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
        return nullptr;
    }

    if (mic == nullptr) {
        mic = std::make_unique<LightPopulationValue> ();
    }
    if (tMin == nullptr) {
        tMin = std::make_unique<LightPopulationValue> ();
    }
    if (tMax == nullptr) {
        tMax = std::make_unique<LightPopulationValue> ();
    }
    if (tBest == nullptr) {
        tBest = std::make_unique<LightPopulationValue> ();
    }
    if (toxicityAlarm == nullptr) {
        toxicityAlarm = std::make_unique<LightPopulationValue> ();
    }
    if (inefficacyAlarm == nullptr) {
        inefficacyAlarm = std::make_unique<LightPopulationValue> ();
    }

    auto target = std::make_unique<TargetDefinition>(type, unit, ActiveMoietyId(""),
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

    return target;
}


std::vector<std::unique_ptr<AnalyteSet> > DrugModelImport::extractAnalyteGroups(Tucuxi::Common::XmlNodeIterator _node)
{

    std::vector<std::unique_ptr<AnalyteSet> > analyteGroups;

    XmlNodeIterator it = _node->getChildren();

    while (it != XmlNodeIterator::none()) {
        std::string nodeName = it->getName();
        if (nodeName == "analyteGroup") {
            auto analyteGroup = extractAnalyteGroup(it);
            if (analyteGroup != nullptr) {
                analyteGroups.push_back(std::move(analyteGroup));
            }
        }
        else {
            unexpectedTag(nodeName);
        }
        it ++;
    }

    return analyteGroups;
}

std::unique_ptr<AnalyteSet> DrugModelImport::extractAnalyteGroup(Tucuxi::Common::XmlNodeIterator _node)
{
    std::string groupId;
    std::string pkModelId;
    // Use a default value for the concentration unit, for backward compatibility purpose
    TucuUnit concentrationUnit{"ug/l"};
    std::vector<std::unique_ptr<Analyte> > analytes;
    std::unique_ptr<ParameterSetDefinition> parameters = nullptr;
    std::vector<std::unique_ptr<TargetDefinition> > targets;

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
        return nullptr;
    }

    auto analyteGroup = std::make_unique<AnalyteSet>();
    analyteGroup->setId(groupId);
    analyteGroup->setPkModelId(pkModelId);
    if (!analytes.empty()) {
        // Here we assume all analytes will share the same unit
        analyteGroup->setDoseUnit(Tucuxi::Common::UnitManager::getWeightFromConcentration(analytes[0]->getUnit()));
    }
    for (auto & analyte : analytes) {
        analyteGroup->addAnalyte(std::move(analyte));
    }
    analyteGroup->setDispositionParameters(std::move(parameters));

    return analyteGroup;

}


std::vector<std::unique_ptr<Analyte> > DrugModelImport::extractAnalytes(Tucuxi::Common::XmlNodeIterator _node)
{

    std::vector<std::unique_ptr<Analyte> > analytes;

    XmlNodeIterator it = _node->getChildren();

    while (it != XmlNodeIterator::none()) {
        std::string nodeName = it->getName();
        if (nodeName == "analyte") {
            auto analyte = extractAnalyte(it);
            if (analyte != nullptr) {
                analytes.push_back(std::move(analyte));
            }
        }
        else {
            unexpectedTag(nodeName);
        }
        it ++;
    }

    return analytes;
}

std::unique_ptr<Analyte> DrugModelImport::extractAnalyte(Tucuxi::Common::XmlNodeIterator _node)
{
    std::string analyteId;
    TucuUnit unit;
    std::unique_ptr<MolarMass> molarMass = nullptr;
    std::unique_ptr<ErrorModel> errorModel = nullptr;

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
        return nullptr;
    }

    if (molarMass == nullptr) {
        setStatus(Status::Error, "No molar mass in an analyte");
        return nullptr;
    }

    auto analyte = std::make_unique<Analyte>(analyteId, unit, std::move(molarMass));
    analyte->setResidualErrorModel(std::move(errorModel));

    return analyte;
}

std::unique_ptr<MolarMass> DrugModelImport::extractMolarMass(Tucuxi::Common::XmlNodeIterator _node)
{
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

    auto molarMass = std::make_unique<MolarMass>(value, unit);
    return molarMass;
}

std::unique_ptr<ErrorModel> DrugModelImport::extractErrorModel(Tucuxi::Common::XmlNodeIterator _node)
{
    ResidualErrorType type = ResidualErrorType::NONE;
    std::unique_ptr<Operation> applyFormula = nullptr;
    std::unique_ptr<Operation> likelyhoodFormula = nullptr;
    std::vector<std::unique_ptr<LightPopulationValue> > sigmas;

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
                    std::unique_ptr<LightPopulationValue> value = extractPopulationValue(sigmaIt);
                    if (getStatus() == Status::Ok) {
                        sigmas.push_back(std::move(value));
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
        return nullptr;
    }

    auto errorModel = std::make_unique<ErrorModel>();
    errorModel->setErrorModel(type);
    for (const auto & sigma : sigmas) {
        errorModel->addOriginalSigma(std::make_unique<PopulationValue>("", sigma->getValue(), sigma->getOperation()));
    }
    errorModel->setApplyFormula(std::move(applyFormula));
    errorModel->setLikelyhoodFormula(std::move(likelyhoodFormula));

    return errorModel;
}

std::unique_ptr<ParameterSetDefinition> DrugModelImport::extractParameterSet(Tucuxi::Common::XmlNodeIterator _node)
{
    std::vector<std::unique_ptr<ParameterDefinition> > parameters;
    std::vector<std::unique_ptr<Correlation> > correlations;

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
        return nullptr;
    }

    auto parameterSet = std::make_unique<ParameterSetDefinition>();

    for (auto & correlation : correlations) {
        parameterSet->addCorrelation(std::move(correlation));
    }
    for (auto & parameter : parameters) {
        parameterSet->addParameter(std::move(parameter));
    }

    return parameterSet;
}

std::vector<std::unique_ptr<ParameterDefinition> > DrugModelImport::extractParameters(Tucuxi::Common::XmlNodeIterator _node)
{

    std::vector<std::unique_ptr<ParameterDefinition> > parameters;

    XmlNodeIterator it = _node->getChildren();

    while (it != XmlNodeIterator::none()) {
        std::string nodeName = it->getName();
        if (nodeName == "parameter") {
            auto parameter = extractParameter(it);
            if (parameter != nullptr) {
                parameters.push_back(std::move(parameter));
            }
        }
        else {
            unexpectedTag(nodeName);
        }
        it ++;
    }

    return parameters;
}

std::unique_ptr<ParameterDefinition> DrugModelImport::extractParameter(Tucuxi::Common::XmlNodeIterator _node)
{
    std::string id;
    TucuUnit unit;
    std::unique_ptr<LightPopulationValue> value = nullptr;
    std::unique_ptr<ParameterVariability> variability = nullptr;
    std::unique_ptr<Operation> validation = nullptr;

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
        return nullptr;
    }


    if (value == nullptr) {
        setStatus(Status::Error, "No value in a PK parameter");
        return nullptr;
    }

    auto parameter = std::make_unique<ParameterDefinition>(id, value->getValue(), value->getOperation(), std::move(variability));
    parameter->setUnit(unit);
    parameter->setValidation(std::move(validation));

    return parameter;
}

std::vector<std::unique_ptr<Correlation> > DrugModelImport::extractCorrelations(Tucuxi::Common::XmlNodeIterator _node)
{

    std::vector<std::unique_ptr<Correlation> > correlations;

    XmlNodeIterator it = _node->getChildren();

    while (it != XmlNodeIterator::none()) {
        std::string nodeName = it->getName();
        if (nodeName == "correlation") {
            auto correlation = extractCorrelation(it);
            if (correlation != nullptr) {
                correlations.push_back(std::move(correlation));
            }
        }
        else {
            unexpectedTag(nodeName);
        }
        it ++;
    }

    return correlations;
}

std::unique_ptr<Correlation> DrugModelImport::extractCorrelation(Tucuxi::Common::XmlNodeIterator _node)
{
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

    auto correlation = std::make_unique<Correlation>(param1, param2, value);

    return correlation;
}

std::unique_ptr<ParameterVariability> DrugModelImport::extractVariability(Tucuxi::Common::XmlNodeIterator _node)
{
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

    return std::make_unique<ParameterVariability>(type, stdDevs);
}


std::unique_ptr<FormulationAndRoutes> DrugModelImport::extractFullFormulationAndRoutes(
        Tucuxi::Common::XmlNodeIterator _node,
        const std::vector<std::unique_ptr<AnalyteSet> >& _analyteSets)
{

    std::vector<std::unique_ptr<FullFormulationAndRoute> > formulationAndRoutesVector;

    std::string defaultId;
    XmlAttribute attribute = _node->getAttribute("default");
    defaultId = attribute.getValue();

    XmlNodeIterator it = _node->getChildren();

    while (it != XmlNodeIterator::none()) {
        std::string nodeName = it->getName();
        if (nodeName == "formulationAndRoute") {
            auto formulationAndRoute = extractFullFormulationAndRoute(it, _analyteSets);
            if (formulationAndRoute != nullptr) {
                formulationAndRoutesVector.push_back(std::move(formulationAndRoute));
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

    auto formulationAndRoutes = std::make_unique<FormulationAndRoutes>();

    for (auto & f : formulationAndRoutesVector) {
        // We insert the formulation and route and calculate if it is the default one
        bool isDefault = (f->getId() == defaultId);
        formulationAndRoutes->add(std::move(f), isDefault);
    }

    return formulationAndRoutes;
}

std::unique_ptr<FullFormulationAndRoute> DrugModelImport::extractFullFormulationAndRoute(
        Tucuxi::Common::XmlNodeIterator _node,
        const std::vector<std::unique_ptr<AnalyteSet> >& _analyteSets)
{
    std::string formulationAndRouteId;
    Formulation formulation = Formulation::Undefined;
    std::string administrationName;
    AdministrationRoute administrationRoute = AdministrationRoute::Undefined;
    AbsorptionModel absorptionModel = AbsorptionModel::Undefined;
    std::unique_ptr<ParameterSetDefinition> absorptionParameters = nullptr;
    std::vector<std::unique_ptr<AnalyteSetToAbsorptionAssociation> > associations;
    std::vector<std::unique_ptr<AnalyteConversion> > analyteConversions;

    std::unique_ptr<ValidDoses> availableDoses = nullptr;
    std::unique_ptr<ValidDurations> intervals = nullptr;
    std::unique_ptr<ValidDurations> infusions = nullptr;
    std::unique_ptr<StandardTreatment> standardTreatment = nullptr;

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
        else if (nodeName == "absorptionModel") {
            absorptionModel = extractAbsorptionModel(it);
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

                    standardTreatment = std::make_unique<StandardTreatment>(isFixedDuration, value, unit);

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
                            analyteConversions.push_back(std::make_unique<AnalyteConversion>(AnalyteId(analyteId), factor));
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
            // Yes, that's normal it is not a unique_ptr. It will point on a selected analyte set,
            // but won't own the object
            AnalyteSet *selectedAnalyteSet = nullptr;
            AbsorptionModel absorptionModel = AbsorptionModel::Undefined;
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
                                    selectedAnalyteSet = analyteSet.get();
                                }
                            }
                            if (selectedAnalyteSet == nullptr) {
                                setNodeError(pIt);
                            }
                        }
                        else if (pName == "absorptionModel") {
                            absorptionModel = extractAbsorptionModel(pIt);
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
                        auto association = std::make_unique<AnalyteSetToAbsorptionAssociation>(*selectedAnalyteSet);
                        association->setAbsorptionParameters(std::move(absorptionParameters));
                        association->setAbsorptionModel(absorptionModel);
                        associations.push_back(std::move(association));
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
        return nullptr;
    }

    FormulationAndRoute spec(formulation, administrationRoute, absorptionModel, administrationName);

    auto formulationAndRoute = std::make_unique<FullFormulationAndRoute>(spec,formulationAndRouteId);
    formulationAndRoute->setValidDoses(std::move(availableDoses));
    formulationAndRoute->setValidIntervals(std::move(intervals));
    formulationAndRoute->setValidInfusionTimes(std::move(infusions));
    for (auto & analyteConversion : analyteConversions) {
        formulationAndRoute->addAnalyteConversion(std::move(analyteConversion));
    }
    for(auto & association : associations) {
        formulationAndRoute->addAssociation(std::move(association));
    }
    if (standardTreatment != nullptr) {
        formulationAndRoute->setStandardTreatment(std::move(standardTreatment));
    }

    return formulationAndRoute;
}


std::unique_ptr<ValidDurations> DrugModelImport::extractValidDurations(Tucuxi::Common::XmlNodeIterator _node)
{
    TucuUnit unit;
    std::unique_ptr<LightPopulationValue> defaultValue = nullptr;
    std::unique_ptr<ValidValuesRange> valuesRange = nullptr;
    std::unique_ptr<ValidValuesFixed> valuesFixed = nullptr;

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
        return nullptr;
    }

    if (defaultValue == nullptr) {
        setStatus(Status::Error, "No default value in valid durations.");
        return nullptr;
    }

    auto validDurations = std::make_unique<ValidDurations>(unit, std::make_unique<PopulationValue>("default", defaultValue->getValue(), defaultValue->getOperation()));

    if (valuesRange != nullptr) {
        validDurations->addValues(std::move(valuesRange));
    }
    if (valuesFixed != nullptr) {
        validDurations->addValues(std::move(valuesFixed));
    }

    return validDurations;

}

std::unique_ptr<ValidDoses> DrugModelImport::extractValidDoses(Tucuxi::Common::XmlNodeIterator _node)
{
    TucuUnit unit;
    std::unique_ptr<LightPopulationValue> defaultValue = nullptr;
    std::vector<std::unique_ptr<ValidValuesRange> > valuesRange;
    std::vector<std::unique_ptr<ValidValuesFixed> > valuesFixed;

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
            auto v = extractValuesRange(it);
            if (v != nullptr) {
                valuesRange.push_back(std::move(v));
            }
        }
        else if (nodeName == "fixedValues") {
            auto v = extractValuesFixed(it);
            if (v != nullptr) {
                valuesFixed.push_back(std::move(v));
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

    if (defaultValue == nullptr) {
        setStatus(Status::Error, "No default value in valid doses.");
        return nullptr;
    }

    auto validDoses = std::make_unique<ValidDoses>(unit, std::make_unique<PopulationValue>("default", defaultValue->getValue(), defaultValue->getOperation()));

    for (auto &value : valuesRange) {
        validDoses->addValues(std::move(value));
    }
    for (auto &value : valuesFixed) {
        validDoses->addValues(std::move(value));
    }

    return validDoses;
}

std::unique_ptr<ValidValuesRange> DrugModelImport::extractValuesRange(Tucuxi::Common::XmlNodeIterator _node)
{
    std::unique_ptr<LightPopulationValue> from = nullptr;
    std::unique_ptr<LightPopulationValue> to = nullptr;
    std::unique_ptr<LightPopulationValue> step = nullptr;

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
        return nullptr;
    }

    if (getStatus() != Status::Ok){
        return nullptr;
    }

    auto valuesRange = std::make_unique<ValidValuesRange>(
                std::make_unique<PopulationValue>("from", from->getValue(), from->getOperation()),
                std::make_unique<PopulationValue>("to", to->getValue(), to->getOperation()),
                std::make_unique<PopulationValue>("step", step->getValue(), step->getOperation())
                );

    return valuesRange;
}

std::unique_ptr<ValidValuesFixed> DrugModelImport::extractValuesFixed(Tucuxi::Common::XmlNodeIterator _node)
{
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
        return nullptr;
    }

    auto valuesFixed = std::make_unique<ValidValuesFixed>();
    for (const auto & v : values) {
        valuesFixed->addValue(v);
    }

    return valuesFixed;
}


std::unique_ptr<DrugModelMetadata> DrugModelImport::extractHead(Tucuxi::Common::XmlNodeIterator _node)
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

    auto metaData = std::make_unique<DrugModelMetadata>();
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

