//@@lisence@@

#ifndef TUCUXI_CORE_SAMPLEEXTRACTOR_H
#define TUCUXI_CORE_SAMPLEEXTRACTOR_H

#include "tucucore/computingservice/computingresult.h"
#include "tucucore/drugtreatment/sample.h"
#include "tucucore/sampleevent.h"

namespace Tucuxi {
namespace Core {

class SampleExtractor
{
public:
    ///
    /// \brief extract relevant samples
    /// \param _samples The list of available samples
    /// \param _analyteGroup The analyte group in which the sample analyte has to be
    /// \param _start The start time of the range of interest
    /// \param _end The end time of the range of interest
    /// \param _toUnit The unit into which the sample value shall be converted
    /// \param _series The output series containing the relevant samples
    /// \return Result::Ok if everything went well
    ///
    /// This function also converts the samples to ug/l.
    ///
    ComputingStatus extract(
            const Samples& _samples,
            const AnalyteSet* _analyteGroup,
            const DateTime& _start,
            const DateTime& _end,
            const TucuUnit& _toUnit,
            SampleSeries& _series);


    ///
    /// \brief extract all relevant samples
    /// \param _samples The list of available samples
    /// \param _start The start time of the range of interest
    /// \param _end The end time of the range of interest
    /// \param _toUnit The unit into which the sample value shall be converted
    /// \param _series The output series containing the relevant samples
    /// \return Result::Ok if everything went well
    ///
    /// This function also converts the samples to ug/l.
    ///
    ComputingStatus extract(
            const Samples& _samples,
            const DateTime& _start,
            const DateTime& _end,
            const TucuUnit& _toUnit,
            SampleSeries& _series);
};

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_SAMPLEEXTRACTOR_H
