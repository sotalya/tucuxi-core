#include "treatmentdrugmodelcompatibilitychecker.h"

#include "tucucore/drugtreatment/drugtreatment.h"
#include "tucucore/drugmodel/drugmodel.h"
#include "tucucore/drugmodel/formulationandroute.h"

namespace Tucuxi {
namespace Core {

TreatmentDrugModelCompatibilityChecker::TreatmentDrugModelCompatibilityChecker()
{

}

bool TreatmentDrugModelCompatibilityChecker::checkCompatibility(const DrugTreatment *_drugTreatment,
                        const DrugModel *_drugModel)
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

    for (const auto & formulationAndRoute : treatmentList) {
        bool found = false;
        // Try to get a formulation and route compabible within the drug model
        for (const auto & f : _drugModel->getFormulationAndRoutes().getList()) {
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

} // namespace Core
} // namespace Tucuxi
