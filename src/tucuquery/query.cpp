#include "query.h"

using namespace std;
using namespace Tucuxi;

namespace Tucuxi {
namespace Query {

Query::Query(
        const string& _queryID,
        const string& _clientID,
        const Common::DateTime& _pQueryDate,
        const string& _language,
        unique_ptr<AdministrativeData> _pAdmin,
        unique_ptr<ParametersData> _pParameters,
        vector< unique_ptr< RequestData > >& _requests
) : m_queryID(_queryID), m_clientID(_clientID), m_pQueryDate(_pQueryDate),
    m_language(_language), m_pAdmin(move(_pAdmin)), m_pParameters(move(_pParameters)),
    m_requests(move(_requests))
{}

const string Query::getQueryID() const
{
    return m_queryID;
}

const string Query::getClientID() const
{
    return m_clientID;
}

const Common::DateTime Query::getpQueryDate() const
{
    return m_pQueryDate;
}

const string Query::getLanguage() const
{
    return m_language;
}

const AdministrativeData& Query::getpAdmin() const
{
    return *m_pAdmin;
}

const ParametersData& Query::getpParameters() const
{
    return *m_pParameters;
}

const vector< unique_ptr< RequestData > >& Query::getRequests() const
{
    return m_requests;
}

} // namespace Query
} // namespace Tucuxi
