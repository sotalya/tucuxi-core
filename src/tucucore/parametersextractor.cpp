/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "tucucommon/general.h"
#include "tucucore/parametersextractor.h"

namespace Tucuxi {
namespace Core {

int ParametersExtractor::extract(const CovariateSeries &_covariates, const ParameterDefinitions& _drugParameters, const DateTime &_start, const DateTime &_end, ParameterSetSeries &_series)
{
    TMP_UNUSED_PARAMETER(_covariates);
    TMP_UNUSED_PARAMETER(_drugParameters);
    TMP_UNUSED_PARAMETER(_start);
    TMP_UNUSED_PARAMETER(_end);
    TMP_UNUSED_PARAMETER(_series);

    return 0;
}

}
}

