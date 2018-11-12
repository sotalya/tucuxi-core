/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "tucucore/intakeextractor.h"

#define DBC_VERIFICATION

namespace Tucuxi {
namespace Core {

template <typename Enumeration>
auto as_integer(Enumeration const value)
    -> typename std::underlying_type<Enumeration>::type
{
    return static_cast<typename std::underlying_type<Enumeration>::type>(value);
}

#define EXTRACT_PRECONDITIONS(start, end, series) \
    assert (!start.isUndefined()); \
    assert (end.isUndefined() || start < end);


int IntakeExtractor::extract(const DosageHistory& _dosageHistory, const DateTime &_start, const DateTime &_end, double _nbPointsPerHour, IntakeSeries &_series)
{
    EXTRACT_PRECONDITIONS(_start, _end, _series);

    // Iterate on elements in dosage history that are in the appropriate time interval and add each of them to the
    // intake series
    for (auto&& timeRange : _dosageHistory.m_history)
    {
        extract(*timeRange, _start, _end, _nbPointsPerHour, _series);
    }

    std::sort(_series.begin(), _series.end());

    return static_cast<int>(_series.size());
}


int IntakeExtractor::extract(const DosageTimeRange &_timeRange, const DateTime &_start, const DateTime &_end, double _nbPointsPerHour, IntakeSeries &_series)
{
    EXTRACT_PRECONDITIONS(_start, _end, _series);

    int nbIntakes = 0;
//    DateTime iStart = std::max(_start, _timeRange.m_startDate);
    DateTime iStart = _timeRange.m_startDate;
    DateTime iEnd;
    if (_end.isUndefined()) {
        iEnd = _timeRange.m_endDate;
    } else if (_timeRange.m_endDate.isUndefined()) {
        iEnd = _end;
    } else {
        iEnd = std::min(_end, _timeRange.m_endDate);
    }

    nbIntakes = _timeRange.m_dosage->extract(*this, iStart, iEnd, _nbPointsPerHour, _series);

    // Add unplanned intakes that fall in the desired interval
    for (auto& intake: _timeRange.m_addedIntakes) {
        if (intake.getEventTime() >= _start && intake.getEventTime() < _end) {
            _series.push_back(intake);
        }
    }

    // Drop the skipped planned intakes that fall in the desired interval
    for (auto& intake: _timeRange.m_skippedIntakes) {
        if (intake.getEventTime() >= _start && intake.getEventTime() < _end) {
            auto intakeToRemove = std::find_if(_series.begin(), _series.end(), [&intake](const IntakeEvent &_ev) -> bool { return intake == _ev; });
            if (intakeToRemove != _series.end()) {
                std::swap(*intakeToRemove, _series.back());
                _series.pop_back();
            }
        }
    }


    for (auto it = _series.begin(); it != _series.end(); ) {
        if ((*it).getEventTime() + (*it).getInterval() < _start) {
            it = _series.erase(it);
        } else {
            ++it;
        }
    }

    std::sort(_series.begin(), _series.end());

    return nbIntakes;
}


int IntakeExtractor::extract(const DosageBounded &_dosageBounded, const DateTime &_start,
                             const DateTime &_end, double _nbPointsPerHour, IntakeSeries &_series)
{
    return _dosageBounded.extract(*this, _start, _end, _nbPointsPerHour, _series);
}


int IntakeExtractor::extract(const DosageLoop &_dosageLoop, const DateTime &_start, const DateTime &_end, double _nbPointsPerHour, IntakeSeries &_series)
{
    EXTRACT_PRECONDITIONS(_start, _end, _series);

    int nbIntakes = 0;

    DateTime currentTime = _dosageLoop.m_dosage->getFirstIntakeInterval(_start);
    // If the end time is undefined, then take the current instant, otherwise take the specified end time.
    DateTime iEnd = _end.isUndefined() ? DateTime() : _end;

    while (currentTime < iEnd) {
        nbIntakes += extract(*(_dosageLoop.m_dosage), currentTime, iEnd, _nbPointsPerHour, _series);
        currentTime += _dosageLoop.m_dosage->getTimeStep();
    }

    std::sort(_series.begin(), _series.end());

    return nbIntakes;
}


int IntakeExtractor::extract(const DosageRepeat &_dosageRepeat, const DateTime &_start, const DateTime &_end, double _nbPointsPerHour, IntakeSeries &_series)
{
    EXTRACT_PRECONDITIONS(_start, _end, _series);

    int nbIntakes = 0;

    DateTime currentTime = _dosageRepeat.m_dosage->getFirstIntakeInterval(_start);
    for (unsigned int nbIt = 0; nbIt < _dosageRepeat.m_nbTimes && currentTime < _end; ++nbIt) {
        nbIntakes += extract(*(_dosageRepeat.m_dosage), currentTime, _end, _nbPointsPerHour, _series);
        currentTime += _dosageRepeat.m_dosage->getTimeStep();
    }

    std::sort(_series.begin(), _series.end());

    return nbIntakes;
}


int IntakeExtractor::extract(const DosageSequence &_dosageSequence, const DateTime &_start, const DateTime &_end, double _nbPointsPerHour, IntakeSeries &_series)
{
    EXTRACT_PRECONDITIONS(_start, _end, _series);

    int nbIntakes = 0;
    DateTime currentTime = _dosageSequence.m_dosages.at(0)->getFirstIntakeInterval(_start);
    for (auto&& dosage : _dosageSequence.m_dosages)
    {
        nbIntakes += dosage->extract(*this, currentTime, _end, _nbPointsPerHour, _series);
        currentTime += dosage->getTimeStep();
        if (currentTime > _end) {
            break;
        }
    }

    std::sort(_series.begin(), _series.end());

    return nbIntakes;
}


int IntakeExtractor::extract(const ParallelDosageSequence &_parallelDosageSequence, const DateTime &_start, const DateTime &_end, double _nbPointsPerHour, IntakeSeries &_series)
{
    EXTRACT_PRECONDITIONS(_start, _end, _series);

    int nbIntakes = 0;

    for (size_t i = 0; i < _parallelDosageSequence.m_dosages.size(); ++i) {
        DateTime newIntervalStart = _parallelDosageSequence.m_dosages.at(i)->getFirstIntakeInterval(_start + _parallelDosageSequence.m_offsets.at(i));
        if (newIntervalStart < _end) {
            nbIntakes += _parallelDosageSequence.m_dosages.at(i)->extract(*this, newIntervalStart, _end, _nbPointsPerHour, _series);
        }
    }

    std::sort(_series.begin(), _series.end());

    return nbIntakes;
}


int IntakeExtractor::extract(const LastingDose &_dosage, const DateTime &_start, const DateTime & /*_end*/, double _nbPointsPerHour, IntakeSeries &_series)
{
    IntakeEvent intake(_start, Duration(), _dosage.m_dose, _dosage.getTimeStep(), _dosage.m_routeOfAdministration.getAbsorptionModel(),
                       _dosage.m_infusionTime, _dosage.getTimeStep().toHours() * _nbPointsPerHour);
    _series.push_back(intake);
    return 1;
}


int IntakeExtractor::extract(const DailyDose &_dosage, const DateTime &_start, const DateTime & /*_end*/, double _nbPointsPerHour, IntakeSeries &_series)
{
    IntakeEvent intake(_start, Duration(), _dosage.m_dose, _dosage.getTimeStep(), _dosage.m_routeOfAdministration.getAbsorptionModel(),
                       _dosage.m_infusionTime, _dosage.getTimeStep().toHours() * _nbPointsPerHour);
    _series.push_back(intake);
    return 1;
}


int IntakeExtractor::extract(const WeeklyDose &_dosage, const DateTime &_start, const DateTime & /*_end*/, double _nbPointsPerHour, IntakeSeries &_series)
{
    IntakeEvent intake(_start, Duration(), _dosage.m_dose, _dosage.getTimeStep(), _dosage.m_routeOfAdministration.getAbsorptionModel(),
                       _dosage.m_infusionTime, _dosage.getTimeStep().toHours() * _nbPointsPerHour);
    _series.push_back(intake);
    return 1;
}

} // namespace Core
} // namespace Tucuxi
