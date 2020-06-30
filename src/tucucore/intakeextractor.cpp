/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "tucucore/intakeextractor.h"

#define DBC_VERIFICATION

namespace Tucuxi {
namespace Core {

template <typename Enumeration>
auto as_integer(Enumeration const _value)
    -> typename std::underlying_type<Enumeration>::type
{
    return static_cast<typename std::underlying_type<Enumeration>::type>(_value);
}

#define EXTRACT_PRECONDITIONS(start, end, series) \
    if (start.isUndefined()) { \
        throw std::runtime_error("[IntakeExtractor] Start time is undefined"); \
    } \
    if (!(end.isUndefined() || start < end)) { \
        throw std::runtime_error("[IntakeExtractor] start is greater than end and end is defined"); \
    }


ComputingStatus IntakeExtractor::extract(const DosageHistory& _dosageHistory, const DateTime &_start, const DateTime &_end, double _nbPointsPerHour, IntakeSeries &_series,
                                         ExtractionOption _option)
{
    try {
        EXTRACT_PRECONDITIONS(_start, _end, _series);

        // Iterate on elements in dosage history that are in the appropriate time interval and add each of them to the
        // intake series
        size_t index = 0;
        for (auto&& timeRange : _dosageHistory.m_history)
        {
            index ++;
            if (index == _dosageHistory.m_history.size()) {
                extract(*timeRange, _start, _end, _nbPointsPerHour, _series, _option);
            }
            else {
                if (_option == ExtractionOption::ForceCycle) {
                    extract(*timeRange, _start, _end, _nbPointsPerHour, _series, _option);
                }
                else {
                    extract(*timeRange, _start, _end, _nbPointsPerHour, _series, ExtractionOption::EndofDate);
                }
            }
        }

        std::sort(_series.begin(), _series.end());

    }
    catch (...) {
        return ComputingStatus::IntakeExtractionError;
    }

    return ComputingStatus::Ok;
}


int IntakeExtractor::extract(const DosageTimeRange &_timeRange, const DateTime &_start, const DateTime &_end, double _nbPointsPerHour, IntakeSeries &_series,
                             ExtractionOption _option)
{
    EXTRACT_PRECONDITIONS(_start, _end, _series);

    int nbIntakes = 0;
//    DateTime iStart = std::max(_start, _timeRange.m_startDate);

    // By default we maybe need the start time of the timeRange
    DateTime iStart = _timeRange.m_startDate;
    // Or not, if it contains a steady state dosage.
    // In that context we use the _start value instead of the on of the timeRange
    // because it is irrelevant at steady state
    if (dynamic_cast<DosageSteadyState*>(_timeRange.m_dosage.get()) != nullptr) {
        iStart = _start;
    }

    DateTime iEnd;
    if (_end.isUndefined()) {
        iEnd = _timeRange.m_endDate;
    } else if (_timeRange.m_endDate.isUndefined()) {
        iEnd = _end;
    } else {
        iEnd = std::min(_end, _timeRange.m_endDate);
    }

    // We check the extraction range
    if (iEnd >= iStart) {
        nbIntakes = _timeRange.m_dosage->extract(*this, iStart, iEnd, _nbPointsPerHour, _series, _option);
    }

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
                             const DateTime &_end, double _nbPointsPerHour, IntakeSeries &_series, ExtractionOption _option)
{
    return _dosageBounded.extract(*this, _start, _end, _nbPointsPerHour, _series, _option);
}


int IntakeExtractor::extract(const DosageLoop &_dosageLoop, const DateTime &_start, const DateTime &_end, double _nbPointsPerHour, IntakeSeries &_series,
                             ExtractionOption _option)
{
    EXTRACT_PRECONDITIONS(_start, _end, _series);

    int nbIntakes = 0;

    DateTime currentTime = _dosageLoop.m_dosage->getFirstIntakeInterval(_start);
    // If the end time is undefined, then take the current instant, otherwise take the specified end time.
    DateTime iEnd = _end.isUndefined() ? DateTime() : _end;

    while (currentTime < iEnd) {
        nbIntakes += extract(*(_dosageLoop.m_dosage), currentTime, iEnd, _nbPointsPerHour, _series, _option);
        currentTime += _dosageLoop.m_dosage->getTimeStep();
    }

    std::sort(_series.begin(), _series.end());

    return nbIntakes;
}


int IntakeExtractor::extract(const DosageSteadyState &_dosageSteadyState, const DateTime &_start, const DateTime &_end, double _nbPointsPerHour, IntakeSeries &_series,
                             ExtractionOption _option)
{
    EXTRACT_PRECONDITIONS(_start, _end, _series);

    int nbIntakes = 0;

    DateTime currentTime = _dosageSteadyState.getFirstIntakeInterval(_start);

    while (currentTime < _end) {
        nbIntakes += extract(*(_dosageSteadyState.m_dosage), currentTime, _end, _nbPointsPerHour, _series, _option);
        currentTime += _dosageSteadyState.m_dosage->getTimeStep();
    }

    std::sort(_series.begin(), _series.end());

    return nbIntakes;
}



int IntakeExtractor::extract(const DosageRepeat &_dosageRepeat, const DateTime &_start, const DateTime &_end, double _nbPointsPerHour, IntakeSeries &_series,
                             ExtractionOption _option)
{
    EXTRACT_PRECONDITIONS(_start, _end, _series);

    int nbIntakes = 0;

    DateTime currentTime = _dosageRepeat.m_dosage->getFirstIntakeInterval(_start);
    for (unsigned int nbIt = 0; nbIt < _dosageRepeat.m_nbTimes && currentTime < _end; ++nbIt) {
        nbIntakes += extract(*(_dosageRepeat.m_dosage), currentTime, _end, _nbPointsPerHour, _series, _option);
        currentTime += _dosageRepeat.m_dosage->getTimeStep();
    }

    std::sort(_series.begin(), _series.end());

    return nbIntakes;
}


int IntakeExtractor::extract(const DosageSequence &_dosageSequence, const DateTime &_start, const DateTime &_end, double _nbPointsPerHour, IntakeSeries &_series,
                             ExtractionOption _option)
{
    EXTRACT_PRECONDITIONS(_start, _end, _series);

    int nbIntakes = 0;
    DateTime currentTime = _dosageSequence.m_dosages[0]->getFirstIntakeInterval(_start);
    for (auto&& dosage : _dosageSequence.m_dosages)
    {
        nbIntakes += dosage->extract(*this, currentTime, _end, _nbPointsPerHour, _series, _option);
        currentTime += dosage->getTimeStep();
        if (currentTime > _end) {
            break;
        }
    }

    std::sort(_series.begin(), _series.end());

    return nbIntakes;
}


int IntakeExtractor::extract(const ParallelDosageSequence &_parallelDosageSequence, const DateTime &_start, const DateTime &_end, double _nbPointsPerHour, IntakeSeries &_series,
                             ExtractionOption _option)
{
    EXTRACT_PRECONDITIONS(_start, _end, _series);

    int nbIntakes = 0;

    for (size_t i = 0; i < _parallelDosageSequence.m_dosages.size(); ++i) {
        DateTime newIntervalStart = _parallelDosageSequence.m_dosages[i]->getFirstIntakeInterval(_start + _parallelDosageSequence.m_offsets[i]);
        if (newIntervalStart < _end) {
            nbIntakes += _parallelDosageSequence.m_dosages[i]->extract(*this, newIntervalStart, _end, _nbPointsPerHour, _series, _option);
        }
    }

    std::sort(_series.begin(), _series.end());

    return nbIntakes;
}


int IntakeExtractor::extract(const LastingDose &_dosage, const DateTime &_start, const DateTime & _end, double _nbPointsPerHour, IntakeSeries &_series,
                             ExtractionOption _option)
{
    Duration interval;
    if (_option == ExtractionOption::EndofDate) {
        interval = std::min(_dosage.getTimeStep(), _end - _start);
    }
    else {
        interval = _dosage.getTimeStep();
    }

    // If the absorption model is INFUSION but the infusion time is 0, then use INTRAVASCULAR instead
    if ((_dosage.m_routeOfAdministration.getAbsorptionModel() == AbsorptionModel::Infusion) &&
            (_dosage.m_infusionTime.isEmpty())) {
        // TODO : Be careful here, the formulation and route is not in line the AbsorptionModel::Intravascular
        IntakeEvent intake(_start, Duration(), UnitManager::translateToUnit(_dosage.m_dose, _dosage.m_doseUnit, Unit("mg")), _dosage.m_doseUnit, interval, _dosage.getLastFormulationAndRoute(), AbsorptionModel::Intravascular,
                           _dosage.m_infusionTime, static_cast<int>(interval.toHours() * _nbPointsPerHour) + 1);
        _series.push_back(intake);
    }
    else {
        IntakeEvent intake(_start, Duration(), UnitManager::translateToUnit(_dosage.m_dose, _dosage.m_doseUnit, Unit("mg")), _dosage.m_doseUnit, interval, _dosage.getLastFormulationAndRoute(), _dosage.m_routeOfAdministration.getAbsorptionModel(),
                           _dosage.m_infusionTime, static_cast<int>(interval.toHours() * _nbPointsPerHour) + 1);
        _series.push_back(intake);
    }
    return 1;
}


int IntakeExtractor::extract(const DailyDose &_dosage, const DateTime &_start, const DateTime & _end, double _nbPointsPerHour, IntakeSeries &_series,
                             ExtractionOption _option)
{
    Duration interval;
    if (_option == ExtractionOption::EndofDate) {
        interval = std::min(_dosage.getTimeStep(), _end - _start);
    }
    else {
        interval = _dosage.getTimeStep();
    }

    // If the absorption model is INFUSION but the infusion time is 0, then use INTRAVASCULAR instead
    if ((_dosage.m_routeOfAdministration.getAbsorptionModel() == AbsorptionModel::Infusion) &&
            (_dosage.m_infusionTime.isEmpty())) {

        // TODO : Be careful here, the formulation and route is not in line the AbsorptionModel::Intravascular
        IntakeEvent intake(_start, Duration(), UnitManager::translateToUnit(_dosage.m_dose, _dosage.m_doseUnit, Unit("mg")), _dosage.m_doseUnit, interval, _dosage.getLastFormulationAndRoute(),  AbsorptionModel::Intravascular,
                           _dosage.m_infusionTime, static_cast<int>(interval.toHours() * _nbPointsPerHour) + 1);
        _series.push_back(intake);
    }
    else {

        IntakeEvent intake(_start, Duration(),UnitManager::translateToUnit(_dosage.m_dose, _dosage.m_doseUnit, Unit("mg")), _dosage.m_doseUnit, interval, _dosage.getLastFormulationAndRoute(),  _dosage.m_routeOfAdministration.getAbsorptionModel(),
                           _dosage.m_infusionTime, static_cast<int>(interval.toHours() * _nbPointsPerHour) + 1);
        _series.push_back(intake);
    }
    return 1;
}


int IntakeExtractor::extract(const WeeklyDose &_dosage, const DateTime &_start, const DateTime & _end, double _nbPointsPerHour, IntakeSeries &_series,
                             ExtractionOption _option)
{
    Duration interval;
    if (_option == ExtractionOption::EndofDate) {
        interval = std::min(_dosage.getTimeStep(), _end - _start);
    }
    else {
        interval = _dosage.getTimeStep();
    }

    // If the absorption model is INFUSION but the infusion time is 0, then use INTRAVASCULAR instead
    if ((_dosage.m_routeOfAdministration.getAbsorptionModel() == AbsorptionModel::Infusion) &&
            (_dosage.m_infusionTime.isEmpty())) {

        // TODO : Be careful here, the formulation and route is not in line the AbsorptionModel::Intravascular
        IntakeEvent intake(_start, Duration(), UnitManager::translateToUnit(_dosage.m_dose, _dosage.m_doseUnit, Unit("mg")), _dosage.m_doseUnit, interval, _dosage.getLastFormulationAndRoute(),  AbsorptionModel::Intravascular,
                           _dosage.m_infusionTime, static_cast<int>(interval.toHours() * _nbPointsPerHour) + 1);
        _series.push_back(intake);
    }
    else {

        IntakeEvent intake(_start, Duration(), UnitManager::translateToUnit(_dosage.m_dose, _dosage.m_doseUnit, Unit("mg")), _dosage.m_doseUnit, interval, _dosage.getLastFormulationAndRoute(),  _dosage.m_routeOfAdministration.getAbsorptionModel(),
                           _dosage.m_infusionTime, static_cast<int>(interval.toHours() * _nbPointsPerHour) + 1);
        _series.push_back(intake);
    }
    return 1;
}

} // namespace Core
} // namespace Tucuxi
