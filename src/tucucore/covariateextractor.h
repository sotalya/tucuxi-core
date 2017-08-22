/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_COVARIATEEXTRACTOR_H
#define TUCUXI_CORE_COVARIATEEXTRACTOR_H

#include <vector>

#include "tucucommon/datetime.h"

#include "tucucore/definitions.h"
#include "tucucore/covariate.h"

namespace Tucuxi {
namespace Core {

class CovariateExtractor
{
public:
    static int extract(const Covariates &_defaults, const Covariates &_patientCovariates, const DateTime &_start, const DateTime &_end, CovariateSeries &_series);
};

}
}

#endif // TUCUXI_CORE_COVARIATEEXTRACTOR_H