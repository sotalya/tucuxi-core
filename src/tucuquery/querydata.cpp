//@@license@@

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
