/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_SAMPLEEXTRACTOR_H
#define TUCUXI_CORE_SAMPLEEXTRACTOR_H

#include "tucucore/drugtreatment/sample.h"
#include "tucucore/sampleevent.h"
#include "tucucore/computingservice/computingresult.h"

namespace Tucuxi {
namespace Core {

class SampleExtractor
{
public:

    ///
    /// \brief extract relevant samples
    /// \param _samples The list of available samples
    /// \param _start The start time of the range of interest
    /// \param _end The end time of the range of interest
    /// \param _series The output series containing the relevant samples
    /// \return Result::Ok if everything went well
    ///
    /// This function also converts the samples to ug/l.
    ///
    ComputingResult extract(const Samples &_samples, const DateTime &_start, const DateTime &_end, SampleSeries &_series);
};

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_SAMPLEEXTRACTOR_H
