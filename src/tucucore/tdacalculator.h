#ifndef TDACALCULATOR_H
#define TDACALCULATOR_H

#include <iostream>
#include <memory>
#include <vector>

#include "drugtreatment/sample.h"
#include "tucucommon/duration.h"
#include "intakeextractor.h"
#include "dosage.h" // For dosageHistory

using Tucuxi::Common::Duration;
using Tucuxi::Common::DateTime;

namespace Tucuxi {
namespace Core {

class TDACalculator
{
public:
    explicit TDACalculator();

    std::vector<Duration> calculateDurations(const Samples& samples, const DosageHistory& dosageHistory) const;

private:

};


} // namespace Core
} // namespace Tucuxi

#endif // TDACALCULATOR_H
