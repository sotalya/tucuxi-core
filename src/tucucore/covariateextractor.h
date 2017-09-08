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

class ICovariateExtractor
{
public:

    virtual int extract(
            const CovariateDefinitions &_defaults,
            const PatientVariates &_patientCovariates,
            const DateTime &_start,
            const DateTime &_end,
            CovariateSeries &_series) = 0;

};

class CovariateExtractor : public ICovariateExtractor
{
public:

    int extract(
            const CovariateDefinitions &_defaults,
            const PatientVariates &_patientCovariates,
            const DateTime &_start,
            const DateTime &_end,
            CovariateSeries &_series) override;
};

}
}

#endif // TUCUXI_CORE_COVARIATEEXTRACTOR_H
