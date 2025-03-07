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
    /// \brief Default constructor for the TDACalculator class.
    ///
    /// Initializes a TDACalculator object without any specific configuration.
    explicit TDACalculator();

    /// \brief Calculates the durations between each sample and the closest preceding intake.
    ///
    /// This function iterates over the list of samples (_samples) and, for each one,
    /// it finds the most recent intake that occurred before the sample in time.
    /// Then computes the duration between the two and stores the result in a vector of durations.
    /// If no intake exist before a sample it will calculate a negative time with the closest intake.
    ///
    /// \param samples List of samples to analyze.
    /// \param dosageHistory Medication intake history.
    /// \return A vector containing the durations between each sample and its closest preceding intake.
    std::vector<Duration> calculateDurations(const Samples& samples, const DosageHistory& dosageHistory) const;

private:

};


} // namespace Core
} // namespace Tucuxi

#endif // TDACALCULATOR_H
