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


#include <utility>

#include "activemoiety.h"

namespace Tucuxi {
namespace Core {


ActiveMoiety::ActiveMoiety() : m_id("") {}

ActiveMoiety::ActiveMoiety(
        ActiveMoietyId _id, TucuUnit _unit, std::vector<AnalyteId> _analyteIds, std::unique_ptr<Operation> _formula)
    : m_id(std::move(_id)), m_unit(std::move(_unit)), m_analyteIds(std::move(_analyteIds)),
      m_formula(std::move(_formula))
{
}

} // namespace Core
} // namespace Tucuxi
