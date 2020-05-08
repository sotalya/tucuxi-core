#include "querydata.h"

using namespace std;
using namespace Tucuxi;

namespace Tucuxi {
namespace Query {

QueryData::QueryData(
        const string& _queryID,
        const string& _clientID,
        const Common::DateTime& _pQueryDate,
        const string& _language,
        unique_ptr<AdministrativeData> _pAdmin,
        unique_ptr<DrugTreatmentData> _pParameters,
        vector< unique_ptr< RequestData > >& _requests
) : m_queryID(_queryID), m_clientID(_clientID), m_pQueryDate(_pQueryDate),
    m_language(_language), m_pAdmin(move(_pAdmin)), m_pParameters(move(_pParameters)),
    m_requests(move(_requests))
{}

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

const AdministrativeData& QueryData::getpAdmin() const
{
    return *m_pAdmin;
}

const DrugTreatmentData& QueryData::getpParameters() const
{
    return *m_pParameters;
}

const vector< unique_ptr< RequestData > >& QueryData::getRequests() const
{
    return m_requests;
}

} // namespace Query
} // namespace Tucuxi
