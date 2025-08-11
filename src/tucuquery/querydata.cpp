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

#include "querydata.h"

using namespace std;
using namespace Tucuxi;

namespace Tucuxi {
namespace Query {

QueryData::QueryData(
        string _queryID,
        string _clientID,
        Common::DateTime _pQueryDate,
        string _language,
        unique_ptr<DrugTreatmentData> _pParameters,
        vector<unique_ptr<RequestData> >& _requests)
    : m_queryID(std::move(_queryID)), m_clientID(std::move(_clientID)), m_pQueryDate(std::move(_pQueryDate)),
      m_language(std::move(_language)), m_pParameters(move(_pParameters)), m_requests(move(_requests))
{
}

string QueryData::getQueryID() const
{
    return m_queryID;
}

string QueryData::getClientID() const
{
    return m_clientID;
}

Common::DateTime QueryData::getpQueryDate() const
{
    return m_pQueryDate;
}

string QueryData::getLanguage() const
{
    return m_language;
}

const DrugTreatmentData& QueryData::getpParameters() const
{
    return *m_pParameters;
}

const vector<unique_ptr<RequestData> >& QueryData::getRequests() const
{
    return m_requests;
}

} // namespace Query
} // namespace Tucuxi
