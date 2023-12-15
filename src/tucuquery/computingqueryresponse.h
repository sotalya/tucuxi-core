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


#ifndef COMPUTINGQUERYRESPONSE_H
#define COMPUTINGQUERYRESPONSE_H

#include "tucucore/computingservice/computingresponse.h"
#include "tucucore/computingservice/computingresult.h"

#include "querystatus.h"

namespace Tucuxi {
namespace Query {

///
/// \brief The QueryResponseMetaData class
/// This class will embed information such as computation time for a ComputingQuery
///
class QueryResponseMetaData
{
};

///
/// \brief The ComputingResponseMetaData class
/// This class will embed information such as the drug model ID
/// used for computation, and the computation time, for a single ComputingRequest
///
class ComputingResponseMetaData
{
public:
    ComputingResponseMetaData(std::string _drugModelId);
    std::string getDrugModelId()
    {
        return m_drugModelId;
    }; // todo : document

private:
    std::string m_drugModelId;
};

class SingleResponseData
{

public:
    SingleResponseData(
            std::unique_ptr<Tucuxi::Core::ComputingResponse> _computingResponse,
            std::unique_ptr<ComputingResponseMetaData> _metaData);

    std::unique_ptr<Tucuxi::Core::ComputingResponse> m_computingResponse;
    std::unique_ptr<ComputingResponseMetaData> m_metaData;
};

///
/// \brief The ComputingQueryResponse class
/// This class contains all the responses of a ComputingQuery.
/// It can take advantage of ComputingResponse class, but should also embed
/// information about the drug model Id used (for instance)
///
class ComputingQueryResponse
{
public:
    ComputingQueryResponse();

    Tucuxi::Core::RequestResponseId getQueryId() const;
    const std::vector<SingleResponseData>& getRequestResponses() const;

    QueryStatus getQueryStatus() const;

    std::string getErrorMessage() const;

    void setQueryStatus(QueryStatus _queryStatus);

    void setQueryStatus(QueryStatus _queryStatus, std::string _errorMessage);

    void addRequestResponse(
            std::unique_ptr<Tucuxi::Core::ComputingResponse> _computingResponse,
            std::unique_ptr<ComputingResponseMetaData> _metaData);
    void setRequestResponseId(Tucuxi::Core::RequestResponseId _requestResponseId);

    Tucuxi::Core::RequestResponseId m_queryId;

    QueryStatus m_queryStatus{QueryStatus::Undefined};

    std::string m_errorMessage;

    std::vector<SingleResponseData> m_requestResponses;

    std::unique_ptr<QueryResponseMetaData> m_metaData;
};


} // namespace Query
} // namespace Tucuxi



#endif // COMPUTINGQUERYRESPONSE_H
