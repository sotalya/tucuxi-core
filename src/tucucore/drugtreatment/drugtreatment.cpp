/*
 * Tucuxi - Tucuxi-core library and command line tool.
 * This code allows to perform prediction of drug concentration in blood
 * and to propose dosage adaptations.
 * It has been developed by HEIG-VD, in close collaboration with CHUV.
 * Copyright (C) 2023 HEIG-VD, maintained by Yann Thoma  <yann.thoma@heig-vd.ch>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */


#include <memory>

#include "tucucore/drugtreatment/drugtreatment.h"

namespace Tucuxi {
namespace Core {

DrugTreatment::DrugTreatment() = default;


const DosageHistory& DrugTreatment::getDosageHistory() const
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
    m_dosageHistory.addTimeRange(*_timeRange);
}

void DrugTreatment::mergeDosageTimeRange(std::unique_ptr<DosageTimeRange> _timeRange)
{
    m_dosageHistory.mergeDosage(_timeRange.get());
}


void DrugTreatment::addTarget(std::unique_ptr<Target> _target)
{
    m_targets.push_back(std::move(_target));
}


void DrugTreatment::addSample(std::unique_ptr<Sample> _sample)
{
    m_samples.push_back(std::move(_sample));
}


} // namespace Core
} // namespace Tucuxi
