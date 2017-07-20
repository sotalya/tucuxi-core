#include "tucucore/intakeextractor.h"

#define DBC_VERIFICATION

namespace Tucuxi {
namespace Core {

#define EXTRACT_PRECONDITIONS(start, end, series) \
    IntakeSeries in_series = series; \
    assert (!start.isUndefined()); \
    assert (end.isUndefined() || start < end);

#define EXTRACT_POSTCONDITIONS(start, end, series) \
    assert (series.size() >= in_series.size()); \
    for (size_t i = 0; i < in_series.size(); ++i) { \
        assert (series.at(i) == in_series.at(i)); \
    } \
    for (size_t i = in_series.size(); i < series.size(); ++i) { \
        assert (series.at(i).getEventTime() >= start); \
        assert (end.isUndefined() || series.at(i).getEventTime() < end); \
    }


int IntakeExtractor::extract(const DosageHistory &_dosageHistory, const DateTime &_start, const DateTime &_end, IntakeSeries &_series)
{
    EXTRACT_PRECONDITIONS(_start, _end, _series);

    // Iterate on elements in dosage history that are in the appropriate time interval and add each of them to the
    // intake series
    for (auto&& timeRange : _dosageHistory.m_history)
    {
        extract(*timeRange, _start, _end, _series);
    }

    EXTRACT_POSTCONDITIONS(_start, _end, _series);

    return _series.size();
}


int IntakeExtractor::extract(const DosageTimeRange &_timeRange, const DateTime &_start, const DateTime &_end, IntakeSeries &_series)
{
    EXTRACT_PRECONDITIONS(_start, _end, _series);

    int nbIntakes = 0;
    nbIntakes = extract(_timeRange.m_dosages, _start, _end, _series);

    EXTRACT_POSTCONDITIONS(_start, _end, _series);

    return nbIntakes;
}


int IntakeExtractor::extract(const DosageList &_dosageList, const DateTime &_start, const DateTime &_end, IntakeSeries &_series)
{
    EXTRACT_PRECONDITIONS(_start, _end, _series);

    int nbIntakes = 0;
    for (auto&& dosage : _dosageList)
    {
        nbIntakes += dosage->extract(*this, _start, _end, _series);
    }

    EXTRACT_POSTCONDITIONS(_start, _end, _series);

    return nbIntakes;
}


int IntakeExtractor::extract(const DosageBounded & /*_dosageBounded*/, const DateTime & /*_start*/,
                             const DateTime & /*_end*/, IntakeSeries & /*_series*/)
{
    // This function should never be called directly (only derived functions should).
    return -1;
}


int IntakeExtractor::extract(const DosageLoop &_dosageLoop, const DateTime &_start, const DateTime &_end, IntakeSeries &_series)
{
    EXTRACT_PRECONDITIONS(_start, _end, _series);

    int nbIntakes = 0;

    DateTime currentTime = _dosageLoop.m_dosage->getFirstIntakeInterval(_start);
    while (currentTime < _end) {
        nbIntakes += extract(*(_dosageLoop.m_dosage), currentTime, _end, _series);
        currentTime += _dosageLoop.m_dosage->getTimeStep();
    }

    EXTRACT_POSTCONDITIONS(_start, _end, _series);

    return nbIntakes;
}


int IntakeExtractor::extract(const DosageRepeat &_dosageRepeat, const DateTime &_start, const DateTime &_end, IntakeSeries &_series)
{
    EXTRACT_PRECONDITIONS(_start, _end, _series);

    int nbIntakes = 0;

    DateTime currentTime = _dosageRepeat.m_dosage->getFirstIntakeInterval(_start);
    for (unsigned int nbIt = 0; nbIt < _dosageRepeat.m_nbTimes; ++nbIt) {
        nbIntakes += extract(*(_dosageRepeat.m_dosage), currentTime, _end, _series);
        currentTime += _dosageRepeat.m_dosage->getTimeStep();
    }

    EXTRACT_POSTCONDITIONS(_start, _end, _series);

    return nbIntakes;
}


int IntakeExtractor::extract(const DosageSequence &_dosageSequence, const DateTime &_start, const DateTime &_end, IntakeSeries &_series)
{
    EXTRACT_PRECONDITIONS(_start, _end, _series);

    int nbIntakes = 0;
    DateTime currentTime = _dosageSequence.m_dosages.at(0)->getFirstIntakeInterval(_start);
    for (auto&& dosage : _dosageSequence.m_dosages)
    {
        nbIntakes += dosage->extract(*this, currentTime, _end, _series);
        currentTime += dosage->getTimeStep();
    }

    EXTRACT_POSTCONDITIONS(_start, _end, _series);

    return nbIntakes;
}


int IntakeExtractor::extract(const LastingDose &_dosage, const DateTime &_start, const DateTime & /*_end*/, IntakeSeries &_series)
{
    IntakeEvent intake(_start, Duration(), _dosage.m_dose, _dosage.getTimeStep(), _dosage.m_routeOfAdministration,
                       _dosage.m_infusionTime, 0);
    _series.push_back(intake);
    return 1;
}


int IntakeExtractor::extract(const DailyDose &_dosage, const DateTime &_start, const DateTime & /*_end*/, IntakeSeries &_series)
{
    IntakeEvent intake(_start, Duration(), _dosage.m_dose, _dosage.getTimeStep(), _dosage.m_routeOfAdministration,
                       _dosage.m_infusionTime, 0);
    _series.push_back(intake);
    return 1;
}


int IntakeExtractor::extract(const WeeklyDose &_dosage, const DateTime &_start, const DateTime & /*_end*/, IntakeSeries &_series)
{
    IntakeEvent intake(_start, Duration(), _dosage.m_dose, _dosage.getTimeStep(), _dosage.m_routeOfAdministration,
                       _dosage.m_infusionTime, 0);
    _series.push_back(intake);
    return 1;
}

}
}
