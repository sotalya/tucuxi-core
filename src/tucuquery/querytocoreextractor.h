#ifndef QUERYTOCOREEXTRACTOR_H
#define QUERYTOCOREEXTRACTOR_H


#include "tucucore/covariateevent.h"
#include "tucucore/drugtreatment/sample.h"
#include "tucucore/targetevent.h"
#include "tucucore/computingservice/computingtrait.h"

#include "tucuquery/querydata.h"

namespace Tucuxi {

namespace Core {
    class DrugTreatment;
    class ComputingTrait;
    class ComputingTraits;
    class DrugModel;
}

namespace Query {

class ComputingQuery;

///
/// \brief The QueryToCoreExtractor class
/// This class is responsible for extracting data from a query and translate
/// various fields to Tucuxi::Core objects.
/// For instance, patient variates, targets and samples
///
class QueryToCoreExtractor
{
public:
    QueryToCoreExtractor();

    ///
    /// \brief Extract a ComputingQuery from the XML pre-processed data
    /// \param _query The QueryData object built from XML
    /// \return A pointer to a ComputingQuery, nullptr if something went wrong
    ///
    /// This method should be the only public one.
    ///
    Tucuxi::Query::ComputingQuery* extractComputingQuery(const QueryData &_query) const;

    ///
    /// \brief extractPatientVariates
    /// \param _query
    /// \return
    ///
    Tucuxi::Core::PatientVariates extractPatientVariates(const QueryData &_query) const;

    ///
    /// \brief extractTargets
    /// \param _query
    /// \param _drugPosition
    /// \return
    ///
    Tucuxi::Core::Targets extractTargets(const QueryData &_query, size_t _drugPosition) const;

    ///
    /// \brief extractSamples
    /// \param _query
    /// \param _drugPosition
    /// \return
    ///
    Tucuxi::Core::Samples extractSamples(const QueryData &_query, size_t _drugPosition) const;

    Tucuxi::Core::DrugTreatment *extractDrugTreatment(const QueryData &_query, const RequestData &_requestData) const;

    Tucuxi::Core::DrugModel *extractDrugModel(const QueryData &_query, const RequestData &_requestData, const Tucuxi::Core::DrugTreatment *_drugTreatment) const;


};

} // namespace Query
} // namespace Tucuxi

#endif // QUERYTOCOREEXTRACTOR_H
