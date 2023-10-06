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

#include "sample.h"


namespace Tucuxi {
namespace Core {


Sample::Sample(DateTime _date, AnalyteId _analyteId, Value _value, TucuUnit _unit, Value _weight)
    : m_date(std::move(_date)), m_analyteID(std::move(_analyteId)), m_value(_value), m_unit(std::move(_unit)),
      m_weight(_weight)
{
}

Sample::~Sample() = default;

DateTime Sample::getDate() const
{
    return m_date;
}

//std::string Sample::getSampleId() const
//{
//    return m_sampleID;
//}

TucuUnit Sample::getUnit() const
{
    return m_unit;
}

AnalyteId Sample::getAnalyteID() const
{
    return m_analyteID;
}

Value Sample::getValue() const
{
    return m_value;
}

Value Sample::getWeight() const
{
    return m_weight;
}



///// Concentrations in every sample
//ConcentrationData::ConcentrationData(AnalyteId _analyteID,
//                                     Value _value,
//                                     Unit _unit) :
//    m_analyteID(_analyteID), m_value(_value), m_unit(_unit)
//{}



} // namespace Core
} // namespace Tucuxi
