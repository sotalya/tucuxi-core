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


#ifndef TUCUXI_QUERY_QUERY_H
#define TUCUXI_QUERY_QUERY_H

#include <memory>
#include <string>
#include <vector>

#include "tucucommon/datetime.h"

#include "administrativedata.h"
#include "parametersdata.h"
#include "requestdata.h"


namespace Tucuxi {
namespace Query {

///
/// \brief The Query class
/// This class contains all of the informations received from the client.
///
class QueryData
{
public:
    // Constructors

    ///
    /// \brief Default constructor is not supported.
    ///
    QueryData() = delete;

    ///
    /// \brief Constructor of a query.
    /// \param _queryID ID of the query.
    /// \param _clientID ID of the client.
    /// \param _pQueryDate A pointer to the date at which the query is sent.
    /// \param _language The language supported by the client.
    /// \param _pAdmin A pointer to the administrative data.
    /// \param _pParameters A pointer to the medical data used for computation.
    /// \param _requests The requests that the server must process.
    ///
    QueryData(
            std::string _queryID,
            std::string _clientID,
            Tucuxi::Common::DateTime _pQueryDate,
            std::string _language,
            std::unique_ptr<DrugTreatmentData> _pParameters,
            std::vector<std::unique_ptr<RequestData> >& _requests);

    ///
    /// \brief Copy constructor is not supported.
    ///  The copy constructor is not supported because of the use of
    ///  unique_ptr wich can't be copied.
    ///
    QueryData(QueryData& _other) = delete;

    // Getters
    std::string getQueryID() const;
    std::string getClientID() const;
    Tucuxi::Common::DateTime getpQueryDate() const;
    std::string getLanguage() const;
    const DrugTreatmentData& getpParameters() const;
    const std::vector<std::unique_ptr<RequestData> >& getRequests() const;

protected:
    /// The unique identifier of the query.
    const std::string m_queryID;

    /// The unique identifier of the client.
    const std::string m_clientID;

    /// The date at which the query is sent.
    const Tucuxi::Common::DateTime m_pQueryDate;

    /// The language supported by the client.
    const std::string m_language;

    /// The data of the patient used for the computations.
    std::unique_ptr<DrugTreatmentData> m_pParameters;

    /// The requests the client wants the server to process.
    std::vector<std::unique_ptr<RequestData> > m_requests;
};

} // namespace Query
} // namespace Tucuxi

#endif // TUCUXI_QUERY_QUERY_H
