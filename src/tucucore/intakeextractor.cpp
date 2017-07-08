
#include "tucucore/intakeextractor.h"

#include "tucucore/dosage.h"


namespace Tucuxi {
namespace Core {

// To be managed with the dateTime type to be choosen
#define minDate(d1, d2) d2

void IntakeExtractor::extract(const DosageHistory *_dosageHistory, Date _start, Date _end, IntakeSeries &_series)
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


void IntakeExtractor::extract(const DosageTimeRange *_dosage, Date _start, Date _end, IntakeSeries &_series)
{

}


void IntakeExtractor::extract(const DosageList _dosage, Date _start, Date _end, IntakeSeries &_series)
{
    for(std::vector<Dosage *>::const_iterator dosage = _dosage.begin(); dosage != _dosage.end(); ++dosage)
    {
        // Here we exploit the visitor pattern
        // The dosage will call the associated extract function of the IntakeExtractor
        (*dosage)->extract(this, _start, _end, _series);
    }

}

void IntakeExtractor::extract(const Dosage *_dosage, Date _start, Date _end, IntakeSeries &_series)
{

}

void IntakeExtractor::extract(const DosageUnbounded *_dosage, Date _start, Date _end, IntakeSeries &_series)
{

}

void IntakeExtractor::extract(const DosageLoop *_dosage, Date _start, Date _end, IntakeSeries &_series)
{

}

void IntakeExtractor::extract(const DosageBounded *_dosage, Date _start, Date _end, IntakeSeries &_series)
{

}

void IntakeExtractor::extract(const DosageRepeat *_dosage, Date _start, Date _end, IntakeSeries &_series)
{

}

void IntakeExtractor::extract(const DosageSequence *_dosage, Date _start, Date _end, IntakeSeries &_series)
{

}

void IntakeExtractor::extract(const SingleDose *_dosage, Date _start, Date _end, IntakeSeries &_series)
{

}

void IntakeExtractor::extract(const LastingDose *_dosage, Date _start, Date _end, IntakeSeries &_series)
{

}

void IntakeExtractor::extract(const DailyDose *_dosage, Date _start, Date _end, IntakeSeries &_series)
{

}

void IntakeExtractor::extract(const WeeklyDose *_dosage, Date _start, Date _end, IntakeSeries &_series)
{

}

}
}
