/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_DRUGTREATMENT_H
#define TUCUXI_CORE_DRUGTREATMENT_H

#include "tucucore/dosage.h"
#include "tucucore/covariate.h"
#include "tucucore/sample.h"
#include "tucucore/target.h"

namespace Tucuxi {
namespace Core {

class DrugTreatment
{
public:
    DrugTreatment();

    std::unique_ptr<const DosageHistory> getDosageHistory(bool _useAdjustments) const;
    const PatientVariates& getCovariates() const;
    const Samples& getSamples() const;
    const Targets& getTargets() const;

private:
    DosageHistory m_dosageHistory;

    PatientVariates m_covariates;
    Targets m_targets;
    Samples m_samples;

    DosageHistory m_adjustment;
    DateTime m_adjustmentDate;
};

}
}

#endif // TUCUXI_CORE_DRUGTREATMENT_H
