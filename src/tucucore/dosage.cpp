
#include "tucucore/dosage.h"

#include "tucucore/intakeextractor.h"


namespace Tucuxi {
namespace Core {

#define DOSAGE_UTILS_IMPL(className) \
    void className::extract(IntakeExtractor *extractor, Date start, Date end, IntakeSeries &series) \
    { \
        extractor->extract(this, start, end, series); \
    }

DOSAGE_UTILS_IMPL(Dosage)
DOSAGE_UTILS_IMPL(DosageUnbounded)
DOSAGE_UTILS_IMPL(DosageBounded)
DOSAGE_UTILS_IMPL(DosageLoop)
DOSAGE_UTILS_IMPL(DosageRepeat)
DOSAGE_UTILS_IMPL(DosageSequence)
DOSAGE_UTILS_IMPL(SingleDose)
DOSAGE_UTILS_IMPL(LastingDose)
DOSAGE_UTILS_IMPL(DailyDose)
DOSAGE_UTILS_IMPL(WeeklyDose)

}
}
