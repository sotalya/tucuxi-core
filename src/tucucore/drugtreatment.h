/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_DRUGTREATMENT_H
#define TUCUXI_CORE_DRUGTREATMENT_H

#include "tucucore/dosage.h"
#include "tucucore/covariate.h"
#include "tucucore/sample.h"

namespace Tucuxi {
namespace Core {

class DrugTreatment
{
public:
    DrugTreatment();

    const DosageHistory& getDosageHistory() const;
    const Covariates& getCovariates() const;
    const Samples& getSamples() const;

private:
    DosageHistory m_dosageHistory;
    Covariates m_covariates;
    Samples m_samples;
};

}
}

#endif // TUCUXI_CORE_DRUGTREATMENT_H