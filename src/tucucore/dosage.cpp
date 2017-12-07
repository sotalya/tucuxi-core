/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "tucucore/dosage.h"
#include "tucucore/intakeextractor.h"

namespace Tucuxi {
namespace Core {



void cloneIntakeSeries(const std::vector<IntakeEvent> &_input, std::vector<IntakeEvent> &_output)
{
    for (auto intake : _input) {
        IntakeEvent newIntakeEvent = intake;
        newIntakeEvent.setCalculator(intake.getCalculator()->getLightClone());
        _output.push_back(newIntakeEvent);
    }
}

// Virtual destructor whose implementation is required for well-formed C++.
SingleDose::~SingleDose() { }

/// \brief Visitor function's implementation.
#define DOSAGE_UTILS_IMPL(className) \
int className::extract(IntakeExtractor &_extractor, const DateTime &_start, const DateTime &_end, IntakeSeries &_series) const \
{ \
    return _extractor.extract(*this, _start, _end, _series); \
}

DOSAGE_UTILS_IMPL(DosageBounded)
DOSAGE_UTILS_IMPL(DosageLoop)
DOSAGE_UTILS_IMPL(DosageRepeat)
DOSAGE_UTILS_IMPL(DosageSequence)
DOSAGE_UTILS_IMPL(ParallelDosageSequence)
DOSAGE_UTILS_IMPL(LastingDose)
DOSAGE_UTILS_IMPL(DailyDose)
DOSAGE_UTILS_IMPL(WeeklyDose)

bool timeRangesOverlap(const DosageTimeRange &_timeRange, const DosageTimeRangeList &_timeRangeList)
{
    for (auto&& tr : _timeRangeList) {
        if (timeRangesOverlap(_timeRange, *tr)) {
            return true;
        }
    }
    return false;
}

bool timeRangesOverlap(const DosageTimeRange &_first, const DosageTimeRange &_second)
{
    if (_first.m_endDate.isUndefined() && _second.m_endDate.isUndefined()) {
        return true;
    } else if (_first.m_endDate.isUndefined()) {
        return (_first.m_startDate  < _second.m_endDate);
    } else if (_second.m_endDate.isUndefined()) {
        return (_second.m_startDate < _first.m_endDate );
    } else {
        return  (_first.m_endDate   < _second.m_endDate   && _first.m_endDate   > _second.m_startDate) ||
                (_first.m_startDate > _second.m_startDate && _first.m_startDate < _second.m_endDate  ) ||
                (_first.m_startDate < _second.m_startDate && _first.m_endDate   > _second.m_endDate  ) ||
                (_first.m_startDate > _second.m_startDate && _first.m_endDate   < _second.m_endDate  );
    }
}

}
}
