/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "tucucommon/general.h"

#include "tucucore/sampleextractor.h"

namespace Tucuxi {
namespace Core {


// TODO : This function should be tested.
// TODO : Nothing about units here...
int SampleExtractor::extract(const Samples &_samples, const DateTime &_start, const DateTime &_end, SampleSeries &_series)
{
    int nbRelevantSamples = 0;
    for (const auto & sample : _samples) {
        if ((sample->getDate() > _start) && (sample->getDate() < _end)) {
            _series.push_back(SampleEvent(sample->getDate(), sample->getValue()));
            nbRelevantSamples ++;
        }
    }
    return nbRelevantSamples;
}


} // namespace Core
} // namespace Tucuxi

