//@@license@@

#include "intakeevent.h"



namespace Tucuxi {
namespace Core {


void cloneIntakeSeries(const std::vector<IntakeEvent>& _input, std::vector<IntakeEvent>& _output)
{
    for (const auto& intake : _input) {
        IntakeEvent newIntakeEvent = intake;
        if (intake.getCalculator() != nullptr) {
            newIntakeEvent.setCalculator(intake.getCalculator()->getLightClone());
        }
        _output.push_back(newIntakeEvent);
    }
}

void selectRecordedIntakes(
        IntakeSeries& _selectionSeries,
        const IntakeSeries& _intakeSeries,
        const DateTime& _recordFrom,
        const DateTime& _recordTo)
{
    for (const auto& intake : _intakeSeries) {
        DateTime eventTime = intake.getEventTime();
        if ((eventTime + intake.getInterval() > _recordFrom) && (eventTime < _recordTo)) {
            _selectionSeries.push_back(IntakeEvent(intake));
        }
    }
}


} // namespace Core
} // namespace Tucuxi
