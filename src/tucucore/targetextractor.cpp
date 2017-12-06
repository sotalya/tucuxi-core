/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "tucucommon/general.h"

#include "tucucore/targetextractor.h"

namespace Tucuxi {
namespace Core {

int TargetExtractor::extract(const CovariateSeries &_covariates, const TargetDefinitions& _targetDefinitions, const Targets &_targets, const DateTime &_start, const DateTime &_end, TargetSeries &_series)
{
    TMP_UNUSED_PARAMETER(_covariates);
    TMP_UNUSED_PARAMETER(_targetDefinitions);
    TMP_UNUSED_PARAMETER(_start);
    TMP_UNUSED_PARAMETER(_end);
    TMP_UNUSED_PARAMETER(_series);

    return 0;
}

}
}
