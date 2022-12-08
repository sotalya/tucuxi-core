//@@license@@

#ifndef FULLSAMPLE_H
#define FULLSAMPLE_H

#include <string>

#include "tucucore/drugtreatment/sample.h"

namespace Tucuxi {
namespace Query {

/// \brief Subclass of Sample that is more administration-oriented because
///        it includes the sample identifier.
class FullSample : public Core::Sample
{
public:
    ///
    /// \brief Sample constructor
    /// \param _id Unique identifier
    /// \param _date Date of measures
    /// \param _analyteId ID of the measured analyte
    /// \param _value Value of concentration
    /// \param _unit Unit of the value
    /// \param _weight The sample weight
    ///
    FullSample(
            std::string _id,
            Common::DateTime _date,
            Core::AnalyteId _analyteId,
            Core::Value _value,
            TucuUnit _unit,
            Core::Value _weight = 1.0);

    /// \brief Returns the sample ID
    /// \return Sample ID
    std::string getSampleId() const;

protected:
    /// Sample ID
    const std::string m_sampleID;
};

/// A vector of unique_ptr on FullSample objects
typedef std::vector<std::unique_ptr<FullSample> > FullSamples;

} // namespace Query
} // namespace Tucuxi

#endif // FULLSAMPLE_H
