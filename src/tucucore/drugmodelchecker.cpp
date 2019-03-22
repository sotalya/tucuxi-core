#include "drugmodelchecker.h"

#include "tucucore/drugmodel/drugmodel.h"
#include "tucucore/pkmodel.h"

namespace Tucuxi {
namespace Core {

DrugModelChecker::DrugModelChecker()
{

}

DrugModelChecker::CheckerResult_t DrugModelChecker::checkDrugModel(const DrugModel *_drugModel, const PkModelCollection *_pkCollection)
{
    if (_drugModel == nullptr) {
        return {false, "The Error Model is nullptr"};
    }
    if (_pkCollection == nullptr) {
        return {false, "The Pk models collection is nullptr"};
    }

    // The invariants are basic checks implemented within the drug model classes
    if (!_drugModel->checkInvariants()) {
        return {false, "There is at least one drug model invariant that is not satisfied"};
    }

    // We need a PK model for each analyte group
    for (const auto &analyteGroup : _drugModel->getAnalyteSets()) {
        std::shared_ptr<PkModel> pkModel = _pkCollection->getPkModelFromId(analyteGroup->getPkModelId());
        if (pkModel == nullptr) {
            return {false, "There is no PK model corresponding to the one defined in the drug model (" + analyteGroup->getPkModelId() + ")"};
        }
    }


    // The analytes are used in various places in the drug model. Check coherency.
    DrugModelChecker::CheckerResult_t analyteResult = checkAnalytes(_drugModel);
    if (!analyteResult.ok) {
        return analyteResult;
    }
/*
    // Checks that the parameters correspond to the PK model parameters
    DrugModelChecker::CheckerResult_t parametersResult = checkParameters(_drugModel, pkModel.get());
    if (!parametersResult.ok) {
        return parametersResult;
    }
*/

    // Checks that the inputs of every formula corresponds to a covariate of the model
    DrugModelChecker::CheckerResult_t formulaInputsResult = checkFormulaInputs(_drugModel);
    if (!formulaInputsResult.ok) {
        return formulaInputsResult;
    }

    return {true, ""};
}

bool contains(std::vector<std::string> _vector, std::string _s) {
    if(std::find(_vector.begin(), _vector.end(), _s) != _vector.end()) {
        return true;
    } else {
        return false;
    }
}
template<class T>
bool contains(std::vector<T> _vector, T _s) {
    if(std::find(_vector.begin(), _vector.end(), _s) != _vector.end()) {
        return true;
    } else {
        return false;
    }
}

DrugModelChecker::CheckerResult_t DrugModelChecker::checkAnalytes(const DrugModel *_drugModel)
{
    std::vector<AnalyteId> activeMoietiesAnalytes;

    // 1. Check that there is no overlap amongst moieties
    for (const auto &activeMoiety : _drugModel->getActiveMoieties()) {
        for (const auto &analyteId : activeMoiety->getAnalyteIds()) {
            if (contains<AnalyteId>(activeMoietiesAnalytes, analyteId)) {
                return {false, "The analyte " + analyteId.toString() + " is present at least twice in the active moieties"};
            }
            else {
                activeMoietiesAnalytes.push_back(analyteId);
            }
        }
    }

    // Sort the list for further comparisons
    std::sort(activeMoietiesAnalytes.begin(), activeMoietiesAnalytes.end());


    // 2. Check that there is no overlap amongst the analyte groups
    std::vector<AnalyteId> allAnalytes;
    for (const auto &analyteSet : _drugModel->m_analyteSets) {
        for (const auto &analyte : analyteSet->getAnalytes()) {
            if (contains<AnalyteId>(allAnalytes, analyte->getAnalyteId())) {
                return {false, "The analyte " + analyte->getAnalyteId().toString() + " is present at least twice in the analyte groups"};
            }
            allAnalytes.push_back(analyte->getAnalyteId());
        }
    }

    std::sort(allAnalytes.begin(), allAnalytes.end());

    // 3. Check that there is an exact match between the moieties and analyte groups
    if (allAnalytes != activeMoietiesAnalytes) {
        return {false, "The analytes in the active moieties and the analyte groupes are not the same"};
    }

    // 4. Check that there is an exact match between the conversions and the moieties
    for (const auto &formulationAndRoute : _drugModel->m_formulationAndRoutes->m_fars) {

        std::vector<AnalyteId> analytes;
        for (const auto &analyteConversion : formulationAndRoute->getAnalyteConversions()) {
            analytes.push_back(analyteConversion->getAnalyteId());
        }
        std::sort(analytes.begin(), analytes.end());
        if (allAnalytes != analytes) {
            return {false, "The analytes in the analyte conversions of a formulation and routes are not the same as the full set of analytes"};
        }
    }

    return {true, ""};
}

DrugModelChecker::CheckerResult_t DrugModelChecker::checkFormulaInputs(const DrugModel *_drugModel)
{
    // First get the list of covariate Ids
    std::vector<std::string> validInputs;
    for(const auto &covariate : _drugModel->getCovariates()) {
        validInputs.push_back(covariate->getId());
    }

    // Second the disposition parameters
    for (const auto &analyteGroup : _drugModel->getAnalyteSets()) {
        for (const auto &parameter : analyteGroup->getDispositionParameters().m_parameters) {
            validInputs.push_back(parameter->getId() + "_population");
        }
    }


    // Second add the parameters
//    for (const auto &parameter : _drugModel->getAbsorptionParameters())


    std::vector<Operation *> operations;

    getAllOperations(_drugModel, operations);

    for(const Operation *operation : operations) {
        if (operation != nullptr) {
            for(const auto &input : operation->getInputs()) {
                if (!contains(validInputs, input.getName())) {
                    return {false, "The input " + input.getName() + " is not a valid formula input"};
                }
            }
        }
    }
    return {true, ""};
}


DrugModelChecker::CheckerResult_t DrugModelChecker::checkParameters(const DrugModel *_drugModel, const PkModel *_pkModel)
{
    TMP_UNUSED_PARAMETER(_drugModel);
    TMP_UNUSED_PARAMETER(_pkModel);

    return {true, ""};
}


void DrugModelChecker::getAllOperations(const DrugModel *_drugModel, std::vector<Operation *> &_operations)
{

    for (const auto &analyteGroup : _drugModel->getAnalyteSets()) {
        for (const auto &parameter : analyteGroup->getDispositionParameters().m_parameters) {
            if (parameter->isVariable())
                _operations.push_back(&parameter->getOperation());
        }
    }
}

} // namespace Core
} // namespace Tucuxi
