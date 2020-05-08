#ifndef TUCUXI_QUERY_QUERY_H
#define TUCUXI_QUERY_QUERY_H

#include <string>
#include <vector>
#include <memory>
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
            const std::string& _queryID,
            const std::string& _clientID,
            const Tucuxi::Common::DateTime& _pQueryDate,
            const std::string& _language,
            std::unique_ptr<AdministrativeData> _pAdmin,
            std::unique_ptr<DrugTreatmentData> _pParameters,
            std::vector< std::unique_ptr< RequestData > >& _requests
    );

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
    const AdministrativeData& getpAdmin() const;
    const DrugTreatmentData& getpParameters() const;
    const std::vector< std::unique_ptr< RequestData > >& getRequests() const;

protected:
    /// The unique identifier of the query.
    const std::string m_queryID;

    /// The unique identifier of the client.
    const std::string m_clientID;

    /// The date at which the query is sent.
    const Tucuxi::Common::DateTime m_pQueryDate;

    /// The language supported by the client.
    const std::string m_language;

    /// The administrative data used for the report generation.
    std::unique_ptr<AdministrativeData> m_pAdmin;

    /// The data of the patient used for the computations.
    std::unique_ptr<DrugTreatmentData> m_pParameters;

    /// The requests the client wants the server to process.
    std::vector< std::unique_ptr< RequestData > > m_requests;
};

} // namespace Query
} // namespace Tucuxi

#endif // TUCUXI_QUERY_QUERY_H
