#include "tdacalculator.h"


namespace Tucuxi {
namespace Core {

TDACalculator::TDACalculator(){}


std::vector<Duration> TDACalculator::calculateDurations(
        const Samples& samples, const DosageHistory& dosageHistory) const {
    IntakeSeries intakes;
    IntakeExtractor extractor;
    std::vector<Duration> durations;

    durations.reserve(samples.size());

    DateTime firstDate = dosageHistory.getDosageTimeRanges().front()->getStartDate();
    DateTime lastDate = dosageHistory.getDosageTimeRanges().back()->getEndDate();

    ComputingStatus result = extractor.extract(
                              dosageHistory,
                              firstDate,
                              lastDate,
                              1,
                              TucuUnit("mg/l"),
                              intakes,
                              ExtractionOption::EndofDate);

    return durations;
}

} // namespace Core
} // namespace Tucuxi
