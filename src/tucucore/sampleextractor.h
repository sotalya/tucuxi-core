/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_SAMPLEEXTRACTOR_H
#define TUCUXI_CORE_SAMPLEEXTRACTOR_H

#include "tucucore/drugtreatment/sample.h"
#include "tucucore/sampleevent.h"

namespace Tucuxi {
namespace Core {

class SampleExtractor
{
public:
    static int extract(const Samples &_samples, const DateTime &_start, const DateTime &_end, SampleSeries &_series);
};

}
}

#endif // TUCUXI_CORE_SAMPLEEXTRACTOR_H
