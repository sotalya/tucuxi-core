/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "tucucommon/general.h"

#include "tucucore/sampleextractor.h"

namespace Tucuxi {
namespace Core {

int SampleExtractor::extract(const Samples &_samples, const DateTime &_start, const DateTime &_end, SampleSeries &_series)
{
    int nbRelevantSamples = 0;
    for (const auto & sample : _samples) {
        if ((sample->getDate() > _start) && (sample->getDate() < _end)) {
            _series.push_back(SampleEvent(sample->getDate(), translateToUnit(sample->getValue(), sample->getUnit(), Unit("ug/l"))));
            nbRelevantSamples ++;
        }
    }
    return nbRelevantSamples;
}


} // namespace Core
} // namespace Tucuxi

