/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "tucucommon/general.h"
#include "tucucore/covariateextractor.h"

namespace Tucuxi {
namespace Core {

int CovariateExtractor::extract(const CovariateDefinitions &_defaults, const Covariates &_patientCovariates, const DateTime &_start, const DateTime &_end, CovariateSeries &_series)
{
    TMP_UNUSED_PARAMETER(_defaults);
    TMP_UNUSED_PARAMETER(_patientCovariates);
    TMP_UNUSED_PARAMETER(_start);
    TMP_UNUSED_PARAMETER(_end);
    TMP_UNUSED_PARAMETER(_series);
    return 0;
}

}
}

