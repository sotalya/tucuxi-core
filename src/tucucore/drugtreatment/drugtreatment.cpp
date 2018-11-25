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


const DosageHistory &DrugTreatment::getDosageHistory() const
{
    return m_dosageHistory;
}


DosageHistory& DrugTreatment::getModifiableDosageHistory()
{
    return m_dosageHistory;
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


void DrugTreatment::addCovariate(std::unique_ptr<PatientCovariate> _covariate)
{
    m_covariates.push_back(std::move(_covariate));
}


void DrugTreatment::addDosageTimeRange(std::unique_ptr<DosageTimeRange> _timeRange)
{
    m_dosageHistory.addTimeRange(*_timeRange.get());
}


void DrugTreatment::addTarget(std::unique_ptr<Target> _target)
{
    m_targets.push_back(std::move(_target));
}


void DrugTreatment::addSample(std::unique_ptr<Sample> _sample)
{
    m_samples.push_back(std::move(_sample));
}


}
}
