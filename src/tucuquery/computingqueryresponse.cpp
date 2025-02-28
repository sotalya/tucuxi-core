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

#include "computingqueryresponse.h"

#include "tucucore/computingservice/computingresponse.h"

namespace Tucuxi {
namespace Query {

SingleResponseData::SingleResponseData(
        std::unique_ptr<Tucuxi::Core::ComputingResponse> _computingResponse,
        std::unique_ptr<ComputingResponseMetaData> _metaData)
    : m_computingResponse(std::move(_computingResponse)), m_metaData(std::move(_metaData))
{
}


ComputingQueryResponse::ComputingQueryResponse() = default;

Tucuxi::Core::RequestResponseId ComputingQueryResponse::getQueryId() const
{
    return m_queryId;
}

const std::vector<SingleResponseData>& ComputingQueryResponse::getRequestResponses() const
{
    return m_requestResponses;
}

QueryStatus ComputingQueryResponse::getQueryStatus() const
{
    return m_queryStatus;
}

std::string ComputingQueryResponse::getErrorMessage() const
{
    return m_errorMessage;
}

void ComputingQueryResponse::setQueryStatus(QueryStatus _queryStatus)
{
    m_queryStatus = _queryStatus;
}

void ComputingQueryResponse::setQueryStatus(QueryStatus _queryStatus, std::string _errorMessage)
{
    m_queryStatus = _queryStatus;
    m_errorMessage = std::move(_errorMessage);
}

void ComputingQueryResponse::addRequestResponse(
        std::unique_ptr<Core::ComputingResponse> _computingResponse,
        std::unique_ptr<ComputingResponseMetaData> _metaData)
{
    m_requestResponses.emplace_back(std::move(_computingResponse), std::move(_metaData));
}

void ComputingQueryResponse::setRequestResponseId(Tucuxi::Core::RequestResponseId _requestResponseId)
{
    m_queryId = std::move(_requestResponseId);
}

void ComputingQueryResponse::setComputingTimeInSeconds(std::chrono::duration<double> _computingTime)
{
    m_computingTimeInSeconds = _computingTime;
}

std::chrono::duration<double> ComputingQueryResponse::getComputingTimeInSeconds() const
{
    return m_computingTimeInSeconds;
}

ComputingResponseMetaData::ComputingResponseMetaData(std::string _drugModelID) : m_drugModelId(std::move(_drugModelID))
{
}

} // namespace Query
} // namespace Tucuxi
