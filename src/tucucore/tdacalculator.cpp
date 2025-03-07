#include "tdacalculator.h"


namespace Tucuxi {
namespace Core {

TDACalculator::TDACalculator(){}


std::vector<Duration> TDACalculator::calculateDurations(
        const Samples& _samples, const DosageHistory& _dosageHistory) const {
    IntakeSeries intakes;
    IntakeExtractor extractor;
    std::vector<Duration> durations;

    durations.reserve(_samples.size());

    DateTime firstDate = _dosageHistory.getDosageTimeRanges().front()->getStartDate();
    DateTime lastDate = _dosageHistory.getDosageTimeRanges().back()->getEndDate();

    ComputingStatus result = extractor.extract(
                              _dosageHistory,
                              firstDate,
                              lastDate,
                              1,
                              TucuUnit("mg"),
                              intakes,
                              ExtractionOption::ForceCycle);

    auto dt1 = intakes.back().getEventTime();

    for (const auto& sample : _samples) {
        Duration diff = sample->getDate() - dt1;
        double hours = diff.toHours();
        durations.push_back(diff);
    }

    return durations;
}

} // namespace Core
} // namespace Tucuxi
