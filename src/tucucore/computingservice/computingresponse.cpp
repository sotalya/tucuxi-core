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

#include "computingresponse.h"


namespace Tucuxi {
namespace Core {


ComputingResponse::ComputingResponse(RequestResponseId _id) : m_id(std::move(_id)) {}

RequestResponseId ComputingResponse::getId() const
{
    return m_id;
}

void ComputingResponse::addResponse(std::unique_ptr<ComputedData> _response)
{
    m_data = std::move(_response);
}

void ComputingResponse::setComputingTimeInSeconds(std::chrono::duration<double> _computingTime)
{
    m_computingTimeInSeconds = _computingTime;
}

std::chrono::duration<double> ComputingResponse::getComputingTimeInSeconds() const
{
    return m_computingTimeInSeconds;
}


void ComputingResponse::setComputingStatus(ComputingStatus _result)
{
    m_computingResult = _result;
}

ComputingStatus ComputingResponse::getComputingStatus() const
{
    return m_computingResult;
}


ComputedData::ComputedData(RequestResponseId _id) : m_id(std::move(_id)) {}

ComputedData::~ComputedData() = default;

RequestResponseId ComputedData::getId() const
{
    return m_id;
}

} // namespace Core
} // namespace Tucuxi
