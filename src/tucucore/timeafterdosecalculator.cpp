#include <typeinfo>

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

    // Sanity check: ensure that all the doses in each time range are within the
    // time range limits.
    Tucuxi::Common::LoggerHelper logHelper;
    auto const& trs = _dosageHistory.getDosageTimeRanges();
    for (auto const& tr : trs) {
        Dosage const * const dosage = tr->getDosage();
        auto startDate = tr->getStartDate();
        auto endDate = tr->getEndDate();
        if (typeid(*dosage) == typeid(SimpleDoseList)) {
            SimpleDoseList const * const dosagePtr =
                static_cast<SimpleDoseList const * const>(dosage);

            if (startDate > dosagePtr->getFirstDosageDate()) {
                logHelper.error(std::string("Time range start date is ") +
                                startDate.str() +
                                std::string(", but first dosage in the range is at ") +
                                dosagePtr->getFirstDosageDate().str());
                throw std::runtime_error("Time range start date error");
            }
            if (endDate < dosagePtr->getLastDosageDate()) {
                logHelper.error(std::string("Time range end date is ") +
                                endDate.str() +
                                std::string(", but last dosage in the range is at ") +
                                dosagePtr->getLastDosageDate().str());
                throw std::runtime_error("Time range end date error");
            }
        }
        if (typeid(*dosage) == typeid(SingleDoseAtTimeList)) {
            SingleDoseAtTimeList const * const dosagePtr =
                static_cast<SingleDoseAtTimeList const * const>(dosage);

            if (startDate > dosagePtr->getFirstDosageDate()) {
                logHelper.error(std::string("Time range start date is ") +
                                startDate.str() +
                                std::string(", but first dosage in the range is at ") +
                                dosagePtr->getFirstDosageDate().str());
                throw std::runtime_error("Time range start date error");
            }
            if (endDate < dosagePtr->getLastDosageDate()) {
                logHelper.error(std::string("Time range end date is ") +
                                endDate.str() +
                                std::string(", but last dosage in the range is at ") +
                                dosagePtr->getLastDosageDate().str());
                throw std::runtime_error("Time range end date error");
            }
        }
    }

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
