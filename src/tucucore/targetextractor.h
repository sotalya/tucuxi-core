/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_TARGETEXTRACTOR_H
#define TUCUXI_CORE_TARGETEXTRACTOR_H

#include "tucucore/target.h"

namespace Tucuxi {
namespace Core {

class TargetExtractor
{
public:
    static int extract(const Targets &_samples, const DateTime &_start, const DateTime &_end, TargetSeries &_series);
};

}
}

#endif // TUCUXI_CORE_TARGETEXTRACTOR_H