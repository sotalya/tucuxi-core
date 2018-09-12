#include "drugmodelimport.h"

#include "tucucommon/xmldocument.h"
#include "tucucommon/xmlnode.h"
#include "tucucommon/xmliterator.h"

#include "tucucore/drugmodel/drugmodel.h"
#include "tucucore/drugdefinitions.h"
#include "tucucore/drugmodel/drugmodeldomain.h"
#include "tucucore/definitions.h"

#include <iostream>

using namespace Tucuxi::Common;
using namespace Tucuxi::Core;

namespace Tucuxi {
namespace Core {


DrugModelImport::DrugModelImport()
{

}

JSOperation* DrugModelImport::extractJSOperation(Tucuxi::Common::XmlNodeIterator _node)
{
    JSOperation *operation = nullptr;
    std::string formula;
    OperationInputList operationInputList;

    XmlNodeIterator it = _node->getChildren();

    while (it != XmlNodeIterator::none()) {
        std::cout << it->getName()  << " : "  << it->getValue() << std::endl;
        std::string nodeName = it->getName();
        if (nodeName == "formula") {
            XmlNodeIterator cdataIt = it->getChildren();
            if (cdataIt == XmlNodeIterator::none()) {
                m_result = Result::Error;
            }
            else if (cdataIt->getType() != EXmlNodeType::CData) {
                m_result = Result::Error;
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
                                m_result = Result::Error;
                            }
                        }
                        inputIt ++;
                    }

                    if (m_result != Result::Error) {
                        operationInputList.push_back( {id, inputType});
                    }

                }

                inputsIt ++;
            }
            // We should access a repository to get an existing hardcoded operation, based on a Id
            // operation = hardcodedOperationRepository->getOperationById(it->getValue());
        }
        else if (nodeName == "multiFormula") {
            // TODO : Implement multi formula parsing
        }
        it ++;
    }

    if (m_result != Result::Ok) {
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
        std::cout << it->getName()  << " : "  << it->getValue() << std::endl;
        std::string nodeName = it->getName();
        if (nodeName == "softFormula") {
            operation = extractJSOperation(it);
        }
        else if (nodeName == "hardFormula") {
            // We should access a repository to get an existing hardcoded operation, based on a Id
            // operation = hardcodedOperationRepository->getOperationById(it->getValue());
        }
        else if (nodeName == "multiFormula") {
            // TODO : Implement multi formula parsing
        }
        it ++;
    }

    if (m_result != Result::Ok) {
        if (operation != nullptr) {
            delete operation;
            return nullptr;
        }
    }

    if (operation == nullptr) {
        m_result = Result::Error;
    }

    return operation;
}

LightPopulationValue *DrugModelImport::extractPopulationValue(Tucuxi::Common::XmlNodeIterator _node)
{
    LightPopulationValue *populationValue;
    Value value;
    Operation *operation = nullptr;
    std::string id = "";

    XmlNodeIterator it = _node->getChildren();

    while (it != XmlNodeIterator::none()) {
        std::cout << it->getName()  << " : "  << it->getValue() << std::endl;
        std::string nodeName = it->getName();
        if (nodeName == "standardValue") {
            value = extractDouble(it);
        }
        else if (nodeName == "aprioriComputation") {
            operation = extractOperation(it);
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

Unit DrugModelImport::extractUnit(Tucuxi::Common::XmlNodeIterator _node)
{
    Unit result(_node->getValue());
    return result;
}

double DrugModelImport::extractDouble(Tucuxi::Common::XmlNodeIterator _node)
{
    double result;
    try {
        result = std::stod(_node->getValue());
    } catch (...) {
        m_result = Result::Error;
        result = 0.0;
    }
    return result;
}

DrugModelImport::Result DrugModelImport::import(Tucuxi::Core::DrugModel *&_drugModel, std::string _fileName)
{
    m_result = Result::Ok;
    _drugModel = nullptr;

    XmlDocument document;
    if (!document.open(_fileName)) {
        return Result::CantOpenFile;
    }

    XmlNode root = document.getRoot();

    XmlNodeIterator drugModelIterator = root.getChildren("drugModel");

    if (drugModelIterator == XmlNodeIterator::none()) {
        return Result::Error;
    }

    _drugModel = extractDrugModel(drugModelIterator);

    XmlNodeIterator it = root.getChildren();


    while (it != XmlNodeIterator::none()) {
        std::cout << it->getName()  << " : "  << it->getValue() << std::endl;
        it ++;
    }



    return m_result;
}


DrugModel* DrugModelImport::extractDrugModel(Tucuxi::Common::XmlNodeIterator _node)
{
    XmlNodeIterator it = _node->getChildren();

    TimeConsiderations *timeConsiderations;
    DrugModelDomain *domain;
    std::vector<CovariateDefinition *> covariates;

    DrugModel *drugModel = new DrugModel();
    while (it != XmlNodeIterator::none()) {
        std::cout << it->getName()  << " : "  << it->getValue() << std::endl;
        std::string nodeName = it->getName();
        if (nodeName == "drugId") {
            drugModel->setDrugId(it->getValue());
        }
        else if (nodeName == "drugModelId") {
            drugModel->setDrugModelId(it->getValue());
        }
        else if (nodeName == "domain") {
            domain = extractDrugDomain(it);
        }
        else if (nodeName == "covariates") {
            covariates = extractCovariates(it);
        }
        else if (nodeName == "activeMoieties") {
            drugModel->setDrugId(it->getValue());
        }
        else if (nodeName == "analyteGroups") {
            drugModel->setDrugId(it->getValue());
        }
        else if (nodeName == "formulationAndRoutes") {
            drugModel->setDrugId(it->getValue());
        }
        else if (nodeName == "timeConsiderations") {
            timeConsiderations = extractTimeConsiderations(it);
        }
        it ++;
    }

    if (m_result != Result::Ok) {
        delete drugModel;
        return nullptr;
    }

    drugModel->setTimeConsiderations(std::unique_ptr<TimeConsiderations>(timeConsiderations));
    drugModel->setDomain(std::unique_ptr<DrugModelDomain>(domain));
    for (const auto & covariate : covariates) {
        drugModel->addCovariate(std::unique_ptr<CovariateDefinition>(covariate));
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
        std::cout << it->getName()  << " : "  << it->getValue() << std::endl;
        std::string nodeName = it->getName();
        if (nodeName == "halfLife") {
            halfLife = extractHalfLife(it);
        }
        else if (nodeName == "outdatedMeasure") {
            outdatedMeasure = extractOutdatedMeasure(it);
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
    double multiplier;
//    Operation *operation;
    LightPopulationValue *value;

    XmlNodeIterator it = _node->getChildren();

    while (it != XmlNodeIterator::none()) {
        std::cout << it->getName()  << " : "  << it->getValue() << std::endl;
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
        it ++;
    }

    HalfLife *halfLife = new HalfLife(id, value->getValue(), unit, multiplier, value->getOperation());
    return halfLife;
}

OutdatedMeasure* DrugModelImport::extractOutdatedMeasure(Tucuxi::Common::XmlNodeIterator _node)
{
    OutdatedMeasure *outdatedMeasure;
    Unit unit;
    LightPopulationValue *value;
    std::string id;

    XmlNodeIterator it = _node->getChildren();

    while (it != XmlNodeIterator::none()) {
        std::cout << it->getName()  << " : "  << it->getValue() << std::endl;
        std::string nodeName = it->getName();
        if (nodeName == "unit") {
            unit = extractUnit(it);
        }
        else if (nodeName == "value") {
            value = extractPopulationValue(it);
        }
        it ++;
    }

    if (m_result != Result::Ok) {
        return nullptr;
    }

    outdatedMeasure = new OutdatedMeasure(id, value->getValue(), unit, value->getOperation());
    return outdatedMeasure;
}

DrugModelDomain* DrugModelImport::extractDrugDomain(Tucuxi::Common::XmlNodeIterator _node)
{
    DrugModelDomain *domain;
    std::vector<Constraint*> constraints;

    domain = new DrugModelDomain();

    XmlNodeIterator it = _node->getChildren();

    while (it != XmlNodeIterator::none()) {
        std::cout << it->getName()  << " : "  << it->getValue() << std::endl;
        std::string nodeName = it->getName();
        if (nodeName == "constraints") {
            constraints = extractConstraints(it);
        }
        it ++;
    }

    if (m_result != Result::Ok) {
        delete domain;
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
        std::cout << it->getName()  << " : "  << it->getValue() << std::endl;
        std::string nodeName = it->getName();
        if (nodeName == "constraint") {
            Constraint *constraint;
            constraint = extractConstraint(it);
            if (constraint) {
                constraints.push_back(constraint);
            }
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
        std::cout << it->getName()  << " : "  << it->getValue() << std::endl;
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
                m_result = Result::Error;
            }
        }
        else if (nodeName == "requiredCovariates") {

            XmlNodeIterator reqCovariatesIt = it->getChildren();
            while (reqCovariatesIt != XmlNodeIterator::none()) {
                if (reqCovariatesIt->getName() == "covariateId") {
                    constraint->addRequiredCovariateId(reqCovariatesIt->getValue());
                }
                reqCovariatesIt ++;
            }

        }
        else if (nodeName == "checkOperation") {
            operation = extractOperation(it);
        }
        it ++;
    }

    if (m_result != Result::Ok) {
        delete constraint;
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
        std::cout << it->getName()  << " : "  << it->getValue() << std::endl;
        std::string nodeName = it->getName();
        if (nodeName == "covariate") {
            CovariateDefinition *covariate;
            covariate = extractCovariate(it);
            if (covariate) {
                covariates.push_back(covariate);
            }
        }
        it ++;
    }

    return covariates;
}

CovariateDefinition* DrugModelImport::extractCovariate(Tucuxi::Common::XmlNodeIterator _node)
{
    CovariateDefinition* covariate;

    std::string id;
    CovariateType type;
    DataType dataType;
    Unit unit;
    InterpolationType interpolationType;
    LightPopulationValue *value = nullptr;
    Operation *validation = nullptr;

    XmlNodeIterator it = _node->getChildren();

    while (it != XmlNodeIterator::none()) {
        std::cout << it->getName()  << " : "  << it->getValue() << std::endl;
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
                validationIt ++;
            }
        }

        it ++;
    }

    if (m_result != Result::Ok) {
        if (validation != nullptr) {
            delete validation;
        }
        if (value != nullptr) {
            delete value;
        }
        return nullptr;
    }

    // TODO : Try to find a way to allow flexible covariate values. This is odd
    std::string valueString = std::to_string(value->getValue());

    covariate = new CovariateDefinition(id, valueString, value->getOperation(), type, dataType);
//    covariate = new CovariateDefinition(id, valueString, nullptr, type, dataType);
    covariate->setInterpolationType(interpolationType);
    covariate->setUnit(unit);
    covariate->setValidation(std::unique_ptr<Operation>(validation));

    return covariate;

}


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

    m_result = Result::Error;
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

    m_result = Result::Error;
    return DataType::Double;
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

    m_result = Result::Error;
    return InterpolationType::Direct;
}


} // namespace Core
} // namespace Tucuxi

