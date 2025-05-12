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


#include "tucucore/dosage.h"

#include "tucucore/intakeextractor.h"

namespace Tucuxi {
namespace Core {

// Virtual destructor whose implementation is required for well-formed C++.
// SingleDose::~SingleDose() { }

/// \brief Visitor function's implementation.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define DOSAGE_UTILS_IMPL(className)                                                                 \
    int className::extract(                                                                          \
            IntakeExtractor& _extractor,                                                             \
            const DateTime& _start,                                                                  \
            const DateTime& _end,                                                                    \
            double _nbPointsPerHour,                                                                 \
            const TucuUnit& _toUnit,                                                                 \
            IntakeSeries& _series,                                                                   \
            ExtractionOption _option) const                                                          \
    {                                                                                                \
        return _extractor.extract(*this, _start, _end, _nbPointsPerHour, _toUnit, _series, _option); \
    }


DOSAGE_UTILS_IMPL(DosageBounded)
DOSAGE_UTILS_IMPL(DosageLoop)
DOSAGE_UTILS_IMPL(DosageSteadyState)
DOSAGE_UTILS_IMPL(DosageRepeat)
DOSAGE_UTILS_IMPL(DosageSequence)
DOSAGE_UTILS_IMPL(ParallelDosageSequence)
DOSAGE_UTILS_IMPL(LastingDose)
DOSAGE_UTILS_IMPL(DailyDose)
DOSAGE_UTILS_IMPL(WeeklyDose)

bool timeRangesOverlap(const DosageTimeRange& _timeRange, const DosageTimeRangeList& _timeRangeList)
{
    for (auto&& tr : _timeRangeList) {
        if (timeRangesOverlap(_timeRange, *tr)) {
            return true;
        }
    }
    return false;
}

bool timeRangesOverlap(const DosageTimeRange& _first, const DosageTimeRange& _second)
{
    if (_first.m_endDate.isUndefined() && _second.m_endDate.isUndefined()) {
        return true;
    }
    if (_first.m_endDate.isUndefined()) {
        return (_first.m_startDate < _second.m_endDate);
    }
    if (_second.m_endDate.isUndefined()) {
        return (_second.m_startDate < _first.m_endDate);
    }
    return (_first.m_endDate < _second.m_endDate && _first.m_endDate > _second.m_startDate)
           || (_first.m_startDate > _second.m_startDate && _first.m_startDate < _second.m_endDate)
           || (_first.m_startDate < _second.m_startDate && _first.m_endDate > _second.m_endDate)
           || (_first.m_startDate > _second.m_startDate && _first.m_endDate < _second.m_endDate);
}



void DosageHistory::mergeDosage(const DosageTimeRange* _newDosage)
{
    // First remove the existing time ranges that are replaced because of
    // the new dosage

    DateTime newStart = _newDosage->getStartDate();

    auto iterator =
            std::remove_if(m_history.begin(), m_history.end(), [newStart](std::unique_ptr<DosageTimeRange>& _val) {
                return (_val->m_startDate >= newStart);
            });

    m_history.erase(iterator, m_history.end());

    for (const auto& existing : m_history) {
        if (existing->getEndDate().isUndefined() || (existing->getEndDate() > _newDosage->getStartDate())) {
            existing->m_endDate = _newDosage->getStartDate();
        }
    }

    // We fill the gap by extending the last dosage of the initial history
    // No, that's not OK because it would add new doses instead of empty doses

    if (!m_history.empty()) {
        if (m_history.back()->m_endDate < _newDosage->getStartDate()) {

            // At least a number of intervals allowing to fill the interval asked
            Duration duration = _newDosage->getStartDate() - m_history.back()->m_endDate;
            unsigned int nbTimes = 1;
            LastingDose lastingDose(
                    0.0,
                    TucuUnit("mg"),
                    m_history.back()->getDosage()->getLastFormulationAndRoute(),
                    Duration(),
                    duration);
            DosageRepeat repeat(lastingDose, nbTimes);
            DosageTimeRange gapFiller =
                    DosageTimeRange(m_history.back()->m_endDate, m_history.back()->m_endDate + duration, repeat);
            addTimeRange(gapFiller);
        }
    }
    addTimeRange(*_newDosage);
}

FormulationAndRoute DosageHistory::getLastFormulationAndRoute() const
{
    if (m_history.empty()) {
        return FormulationAndRoute(Formulation::Undefined, AdministrationRoute::Undefined);
    }
    return m_history.back()->m_dosage->getLastFormulationAndRoute();
}


std::vector<FormulationAndRoute> DosageHistory::getFormulationAndRouteList() const
{
    std::vector<FormulationAndRoute> result;
    for (const auto& timeRange : m_history) {
        result = mergeFormulationAndRouteList(result, timeRange->getDosage()->getFormulationAndRouteList());
    }
    return result;
}

} // namespace Core
} // namespace Tucuxi
