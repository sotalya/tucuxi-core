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


#include "errormodel.h"

#include "tucucore/drugdefinitions.h"
#include "tucucore/operation.h"

namespace Tucuxi {
namespace Core {

ErrorModel::ErrorModel() = default;


void ErrorModel::setApplyFormula(std::unique_ptr<Operation> _applyFormula)
{
    m_applyFormula = std::move(_applyFormula);
}
void ErrorModel::setLikelyhoodFormula(std::unique_ptr<Operation> _likelyhoodFormula)
{
    m_likelyhoodFormula = std::move(_likelyhoodFormula);
}

void ErrorModel::addOriginalSigma(std::unique_ptr<PopulationValue> _sigma)
{
    m_originalSigmas.push_back(std::move(_sigma));
}
void ErrorModel::setErrorModel(ResidualErrorType _errorModel)
{
    m_errorModel = _errorModel;
}


} // namespace Core
} // namespace Tucuxi
