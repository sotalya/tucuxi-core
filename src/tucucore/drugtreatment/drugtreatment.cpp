/*
* Copyright (C) 2017 Tucuxi SA
*/

#include <memory>

#include "tucucore/drugtreatment/drugtreatment.h"

namespace Tucuxi {
namespace Core {

DrugTreatment::DrugTreatment()
{
}


std::unique_ptr<const DosageHistory> DrugTreatment::getDosageHistory(bool _useAdjustments) const
{
    std::unique_ptr<const DosageHistory> dosageHistory;

    if (!_useAdjustments || m_adjustmentDate.isUndefined()) {
        dosageHistory = std::unique_ptr<const DosageHistory>(&m_dosageHistory);
    }
    else {
        dosageHistory = std::make_unique<DosageHistory>();
        // Build the dosage history based on the history as well as the selected adjustment
        // Use the adjustment date to switch between history and adjustment.
    }

    return dosageHistory;
}


const PatientVariates& DrugTreatment::getCovariates() const
{
    return m_covariates;
}


const Samples& DrugTreatment::getSamples() const
{
    return m_samples;
}


const Targets& DrugTreatment::getTargets() const
{
    return m_targets;
}


void addCovariate(std::unique_ptr<PatientCovariate> _covariate)
{
}


void addDosageTimeRange(std::unique_ptr<DosageTimeRange> _timeRange)
{
}


void addTarget(std::unique_ptr<Target> _target)
{
}


void addSample(std::unique_ptr<Sample> _sample)
{
}


}
}
