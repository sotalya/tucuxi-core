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

    Tucuxi::Core::DrugTreatment *extractDrugTreatment(const QueryData &_query) const;

    Tucuxi::Core::ComputingTraits *extractComputingTraits(const QueryData &_query) const;

    Tucuxi::Core::DrugModel *extractDrugModel(const QueryData &_query, const Tucuxi::Core::DrugTreatment *_drugTreatment) const;

    Tucuxi::Core::PredictionParameterType extractPredictionParameterType(const QueryData &_query, const RequestData &_request) const;

    std::vector<Tucuxi::Core::ComputingTrait *> extractPredictionTraits(const QueryData &_query, const RequestData &_request) const;

    std::vector<Tucuxi::Core::ComputingTrait *> extractPredictionAtSampleTimeTraits(const QueryData &_query, const RequestData &_request) const;

    std::vector<Tucuxi::Core::ComputingTrait *> extractPredictionAtTimesTraits(const QueryData &_query, const RequestData &_request) const;

    std::vector<Tucuxi::Core::ComputingTrait *> extractFirstDosageTraits(const QueryData &_query, const RequestData &_request) const;

    std::vector<Tucuxi::Core::ComputingTrait *> extractAdaptationTraits(const QueryData &_query, const RequestData &_request) const;
};

} // namespace Query
} // namespace Tucuxi

#endif // QUERYTOCOREEXTRACTOR_H
