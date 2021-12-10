#include "treatmentdrugmodelcompatibilitychecker.h"

#include "tucucore/drugmodel/drugmodel.h"
#include "tucucore/drugmodel/formulationandroute.h"
#include "tucucore/drugtreatment/drugtreatment.h"
#include "tucucore/pkmodel.h"

namespace Tucuxi {
namespace Core {

TreatmentDrugModelCompatibilityChecker::TreatmentDrugModelCompatibilityChecker() = default;

bool TreatmentDrugModelCompatibilityChecker::checkCompatibility(
        const DrugTreatment* _drugTreatment, const DrugModel* _drugModel)
{
    // Checks the input sanity
    if (_drugTreatment == nullptr) {
        return false;
    }
    if (_drugModel == nullptr) {
        return false;
    }

    // Get the formulation and routes of the drug treatment
    std::vector<FormulationAndRoute> treatmentList = _drugTreatment->getDosageHistory().getFormulationAndRouteList();

    for (const auto& formulationAndRoute : treatmentList) {
        bool found = false;
        // Try to get a formulation and route compabible within the drug model
        for (const auto& f : _drugModel->getFormulationAndRoutes().getList()) {
            if (f->getFormulationAndRoute() == formulationAndRoute) {
                found = true;
            }
        }
        if (!found) {
            return false;
        }
    }

    return true;
}

bool TreatmentDrugModelCompatibilityChecker::checkPkModelCompatibility(
        const DosageHistory* _dosageHistory, const PkModel* _pkModel)
{
    // Get the formulation and routes of the drug treatment
    std::vector<FormulationAndRoute> treatmentList = _dosageHistory->getFormulationAndRouteList();

    // Here we just check if the PK model allows for multiple routes and the treatment embeds multiple routes
    return (!((treatmentList.size() > 1) && (_pkModel->allowMultipleRoutes() == PkModel::AllowMultipleRoutes::No)));
}

} // namespace Core
} // namespace Tucuxi
