/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_PARAMETERSEXTRACTOR_H
#define TUCUXI_CORE_PARAMETERSEXTRACTOR_H

#include "tucucore/parameter.h"
#include "tucucore/covariate.h"

namespace Tucuxi {
namespace Core {

class ParametersExtractor
{
public:
    static int extract(const CovariateSeries &_covariates, const DateTime &_start, const DateTime &_end, ParameterSetSeries &_series);
};

}
}

#endif // TUCUXI_CORE_PARAMETERSEXTRACTOR_H