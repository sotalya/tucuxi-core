//@@license@@

#ifndef TUCUXI_CORE_SAMPLEEVENT_H
#define TUCUXI_CORE_SAMPLEEVENT_H

#include <vector>

#include "tucucore/definitions.h"
#include "tucucore/drugdefinitions.h"
#include "tucucore/timedevent.h"

namespace Tucuxi {
namespace Core {


class SampleEvent : public TimedEvent
{
public:
    /// \brief Constructor defining the time of the sample event and its value.
    /// \param _time Time of the event to set.
    /// \param _value Value of the sample.
    /// \pre _time.isValid() == true
    /// \post m_time == _time
    /// \post m_value == _value
    SampleEvent(DateTime _time, Value _value = 0) : TimedEvent(_time), m_value(_value) {}

    Value getValue() const
    {
        return m_value;
    }

protected:
    Value m_value;
};

typedef std::vector<SampleEvent> SampleSeries;

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_SAMPLEEVENT_H
