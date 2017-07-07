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

    void extract(DosageHistory *_dosageHistory, Date _start, Date _end, IntakeSeries &_series);
private:
    void extract(Dosage *_dosage, Date _start, Date _end, IntakeSeries &_series);
    void extract(DosageUnbounded *_dosage, Date _start, Date _end, IntakeSeries &_series);
    void extract(DosageLoop *_dosage, Date _start, Date _end, IntakeSeries &_series);
    void extract(DosageBounded *_dosage, Date _start, Date _end, IntakeSeries &_series);
    void extract(DosageRepeat *_dosage, Date _start, Date _end, IntakeSeries &_series);
    void extract(DosageSequence *_dosage, Date _start, Date _end, IntakeSeries &_series);
    void extract(SingleDose *_dosage, Date _start, Date _end, IntakeSeries &_series);
    void extract(LastingDose *_dosage, Date _start, Date _end, IntakeSeries &_series);
    void extract(DailyDose *_dosage, Date _start, Date _end, IntakeSeries &_series);
    void extract(WeeklyDose *_dosage, Date _start, Date _end, IntakeSeries &_series);
};

}
}

#endif // TUCUXI_MATH_INTAKEEXTRACTOR_H
