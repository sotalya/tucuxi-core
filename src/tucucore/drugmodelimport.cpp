#include "drugmodelimport.h"

#include "tucucommon/xmldocument.h"
#include "tucucommon/xmlnode.h"
#include "tucucommon/xmliterator.h"

#include "tucucore/drugmodel/drugmodel.h"
#include "tucucore/drugdefinitions.h"
#include "tucucore/drugmodel/drugmodeldomain.h"

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

    return operation;
}

PopulationValue* DrugModelImport::extractPopulationValue(Tucuxi::Common::XmlNodeIterator _node)
{
    PopulationValue *populationValue;
    Value value;
    Operation *operation;
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

    populationValue = new PopulationValue(id, value, operation);

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
            drugModel->setDrugId(it->getValue());
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
    PopulationValue *value;

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

    HalfLife *halfLife = new HalfLife(id, value->getValue(), unit, multiplier, &value->getOperation());
    return halfLife;
}

OutdatedMeasure* DrugModelImport::extractOutdatedMeasure(Tucuxi::Common::XmlNodeIterator _node)
{
    OutdatedMeasure *outdatedMeasure;
    Unit unit;
    PopulationValue *value;
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

    outdatedMeasure = new OutdatedMeasure(id, value->getValue(), unit, &value->getOperation());
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

} // namespace Core
} // namespace Tucuxi

