
#include "tucucore/intakeextractor.h"

#include "tucucore/dosage.h"


namespace Tucuxi {
namespace Core {

// To be managed with the dateTime type to be choosen
#define minDate(d1, d2) d2

void IntakeExtractor::extract(DosageHistory *_dosageHistory, Date _start, Date _end, IntakeSeries &_series)
{
    for(std::vector<DosageTimeRange *>::const_iterator timeRange = _dosageHistory->m_history.begin(); timeRange != _dosageHistory->m_history.end(); ++timeRange)
    {
        Date starting;
        Date ending;
        starting = minDate(_start, (*timeRange)->m_startDate);
        ending = minDate(_end, (*timeRange)->m_endDate);
        extract((*timeRange)->m_dosages, starting, ending, _series);
    }
}


void IntakeExtractor::extract(DosageTimeRange *_dosage, Date _start, Date _end, IntakeSeries &_series)
{

}


void IntakeExtractor::extract(DosageList _dosage, Date _start, Date _end, IntakeSeries &_series)
{

}

void IntakeExtractor::extract(Dosage *_dosage, Date _start, Date _end, IntakeSeries &_series)
{

}

void IntakeExtractor::extract(DosageUnbounded *_dosage, Date _start, Date _end, IntakeSeries &_series)
{

}

void IntakeExtractor::extract(DosageLoop *_dosage, Date _start, Date _end, IntakeSeries &_series)
{

}

void IntakeExtractor::extract(DosageBounded *_dosage, Date _start, Date _end, IntakeSeries &_series)
{

}

void IntakeExtractor::extract(DosageRepeat *_dosage, Date _start, Date _end, IntakeSeries &_series)
{

}

void IntakeExtractor::extract(DosageSequence *_dosage, Date _start, Date _end, IntakeSeries &_series)
{

}

void IntakeExtractor::extract(SingleDose *_dosage, Date _start, Date _end, IntakeSeries &_series)
{

}

void IntakeExtractor::extract(LastingDose *_dosage, Date _start, Date _end, IntakeSeries &_series)
{

}

void IntakeExtractor::extract(DailyDose *_dosage, Date _start, Date _end, IntakeSeries &_series)
{

}

void IntakeExtractor::extract(WeeklyDose *_dosage, Date _start, Date _end, IntakeSeries &_series)
{

}

}
}
