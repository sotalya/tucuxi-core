#include "timeafterdosecalculator.h"

#include "intakeextractor.h"

namespace Tucuxi {
namespace Core {

TimeAfterDoseCalculator::TimeAfterDoseCalculator() {}


std::vector<Duration> TimeAfterDoseCalculator::calculateDurations(
        const Samples& _samples, const DosageHistory& _dosageHistory) const
{
    IntakeSeries intakes;
    IntakeExtractor extractor;
    std::vector<Duration> durations;

    durations.reserve(_samples.size());

    DateTime firstDate = _dosageHistory.getDosageTimeRanges().front()->getStartDate();
    DateTime lastDate = _dosageHistory.getDosageTimeRanges().back()->getEndDate();

    ComputingStatus result = extractor.extract(
            _dosageHistory, firstDate, lastDate, 1, TucuUnit("mg"), intakes, ExtractionOption::ForceCycle);


    for (const auto& sample : _samples) {
        DateTime sampleTime = sample->getDate();
        DateTime closestIntake;
        Duration diff;
        bool found = false;

        for (const auto& intake : intakes) {
            DateTime intakeTime = intake.getEventTime();
            if (intakeTime <= sampleTime) {
                closestIntake = intakeTime;
                found = true;
            }
            else {
                break; //If we pass the sampleTime stop looping.
            }
        }

        if (found) { // If found we substract
            diff = sampleTime - closestIntake;
        }
        else { // This case should be triggerd if the sample time is before all intakeTime. So we can use the first intakeTime.
            diff = sampleTime - intakes.front().getEventTime();
        }

        durations.push_back(diff);
    }

    return durations;
}

} // namespace Core
} // namespace Tucuxi
