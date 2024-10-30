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


#include "residualerrormodelextractor.h"

#include "tucucore/drugmodel/errormodel.h"
#include "tucucore/residualerrormodel.h"

using Tucuxi::Common::UnitManager;

namespace Tucuxi {
namespace Core {

ResidualErrorModelExtractor::ResidualErrorModelExtractor() = default;


ComputingStatus ResidualErrorModelExtractor::extract(
        const ErrorModel& _errorModel,
        const TucuUnit& _fromUnit,
        const TucuUnit& _toUnit,
        const CovariateSeries& _covariateSeries,
        std::unique_ptr<IResidualErrorModel>& _residualErrorModel)
{
    // At some stage the covariates could influence the error model. Maybe...
    TMP_UNUSED_PARAMETER(_covariateSeries);

    if (_errorModel.m_errorModel == ResidualErrorType::SOFTCODED) {
        // Not supported yet
        return ComputingStatus::ErrorModelExtractionError;
    }

    auto newErrorModel = std::make_unique<SigmaResidualErrorModel>();
    newErrorModel->setErrorModel(_errorModel.m_errorModel);

    Sigma sigma = Sigma(_errorModel.m_originalSigmas.size());
    for (std::size_t i = 0; i < _errorModel.m_originalSigmas.size(); i++) {
        if (((_errorModel.m_errorModel == ResidualErrorType::MIXED) && (i == 0))
            || (_errorModel.m_errorModel == ResidualErrorType::ADDITIVE)) {
            sigma[static_cast<Eigen::Index>(i)] = UnitManager::convertToUnit<UnitManager::UnitType::Concentration>(
                    _errorModel.m_originalSigmas[i]->getValue(), _fromUnit, _toUnit);
        }
        else {
            sigma[static_cast<Eigen::Index>(i)] = _errorModel.m_originalSigmas[i]->getValue();
        }
    }

    newErrorModel->setSigma(sigma);

    _residualErrorModel = std::move(newErrorModel);

    return ComputingStatus::Ok;
}

} // namespace Core
} // namespace Tucuxi
