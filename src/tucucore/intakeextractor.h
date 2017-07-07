#ifndef TUCUXI_MATH_INTAKEEXTRACTOR_H
#define TUCUXI_MATH_INTAKEEXTRACTOR_H

#include <vector>

#include "tucucore/definitions.h"
#include "tucucore/dosage.h"

namespace Tucuxi {
namespace Core {

class IntakeExtractor
{
    friend Dosage;
    friend DosageUnbounded;
    friend DosageLoop;
    friend DosageBounded;
    friend DosageRepeat;
    friend DosageSequence;
    friend SingleDose;
    friend LastingDose;
    friend DailyDose;
    friend WeeklyDose;

public:

    void extract(const DosageHistory *_dosageHistory, Date _start, Date _end, IntakeSeries &_series);

private:
    void extract(const DosageTimeRange *_dosage, Date _start, Date _end, IntakeSeries &_series);
    void extract(const DosageList _dosage, Date _start, Date _end, IntakeSeries &_series);
    void extract(const Dosage *_dosage, Date _start, Date _end, IntakeSeries &_series);
    void extract(const DosageUnbounded *_dosage, Date _start, Date _end, IntakeSeries &_series);
    void extract(const DosageLoop *_dosage, Date _start, Date _end, IntakeSeries &_series);
    void extract(const DosageBounded *_dosage, Date _start, Date _end, IntakeSeries &_series);
    void extract(const DosageRepeat *_dosage, Date _start, Date _end, IntakeSeries &_series);
    void extract(const DosageSequence *_dosage, Date _start, Date _end, IntakeSeries &_series);
    void extract(const SingleDose *_dosage, Date _start, Date _end, IntakeSeries &_series);
    void extract(const LastingDose *_dosage, Date _start, Date _end, IntakeSeries &_series);
    void extract(const DailyDose *_dosage, Date _start, Date _end, IntakeSeries &_series);
    void extract(const WeeklyDose *_dosage, Date _start, Date _end, IntakeSeries &_series);
};

}
}

#endif // TUCUXI_MATH_INTAKEEXTRACTOR_H
