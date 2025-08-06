#ifndef TIMEAFTERDOSECALCULATOR_H
#define TIMEAFTERDOSECALCULATOR_H

#include <vector>

#include "tucucommon/duration.h"

#include "dosage.h"
#include "drugtreatment/sample.h"

namespace Tucuxi {
namespace Core {

class TimeAfterDoseCalculator
{
public:
    /// \brief Default constructor for the TimeAfterDoseCalculator class.
    ///
    /// Initializes a TimeAfterDoseCalculator object without any specific configuration.
    explicit TimeAfterDoseCalculator();

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

#endif // TIMEAFTERDOSECALCULATOR_H
