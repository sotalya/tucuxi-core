/* 
 * Tucuxi - Tucuxi-core library and command line tool. 
 * This code allows to perform prediction of drug concentration in blood 
 * and to propose dosage adaptations.
 * It has been developed by HEIG-VD, in close collaboration with CHUV. 
 * Copyright (C) 2023 HEIG-VD, maintained by Yann Thoma  <yann.thoma@heig-vd.ch>
 * 
 * This program is free software: you can redistribute it and/or modify 
 * it under the terms of the GNU Affero General Public License as 
 * published by the Free Software Foundation, either version 3 of the 
 * License, or any later version. 
 * 
 * This program is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU Affero General Public License for more details. 
 * 
 * You should have received a copy of the GNU Affero General Public License 
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */


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
        // Try to get a formulation and route compatible within the drug model
        for (const auto& f : _drugModel->getFormulationAndRoutes().getList()) {
            if (f->getFormulationAndRoute().isCompatible(formulationAndRoute)) {
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
    return ((treatmentList.size() <= 1) || (_pkModel->allowMultipleRoutes() != PkModel::AllowMultipleRoutes::No));
}

} // namespace Core
} // namespace Tucuxi
