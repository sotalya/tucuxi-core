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


#ifndef TUCUXI_CORE_TREATMENTDRUGMODELCOMPATIBILITYCHECKER_H
#define TUCUXI_CORE_TREATMENTDRUGMODELCOMPATIBILITYCHECKER_H

namespace Tucuxi {
namespace Core {

class DrugModel;
class DrugTreatment;
class DosageHistory;
class PkModel;

///
/// \brief The TreatmentDrugModelCompatibilityChecker class
/// The aim of this class is to offer a compatibility check function
/// between a drug treatment and a drug model.
/// It is used by the ComputingComponent as a first check
///
class TreatmentDrugModelCompatibilityChecker
{
public:
    ///
    /// \brief Empty constructor
    ///
    TreatmentDrugModelCompatibilityChecker();

    ///
    /// \brief Checks compatibility between a drug treatment and a drug model
    /// \param _drugTreatment The drug treatment
    /// \param _drugModel The drug model
    /// \return true if the model is compatible with the treatment, false else
    ///
    /// First if one of the input parameters is nullptr, the function returns false.
    /// Second, it checks that each formulation and route of the drug treatment
    /// is available in the drug model. If this is not the case, false is returned.
    ///
    bool checkCompatibility(const DrugTreatment* _drugTreatment, const DrugModel* _drugModel);

    ///
    /// \brief Checks compatibility between a dosage history and a Pk model
    /// \param _dosageHistory The dosage history
    /// \param _pkModel The PK model
    /// \return true if the PK model is compatible with the dosage history, false else
    ///
    /// This function should be called after the drug model compatibility has
    /// been assessed, typically just before starting a computation.
    ///
    bool checkPkModelCompatibility(const Tucuxi::Core::DosageHistory* _dosageHistory, const PkModel* _pkModel);
};

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_TREATMENTDRUGMODELCOMPATIBILITYCHECKER_H
