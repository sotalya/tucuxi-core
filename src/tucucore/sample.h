/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_SAMPLE_H
#define TUCUXI_CORE_SAMPLE_H

#include <vector>

#include "tucucore/timedevent.h"
#include "tucucore/definitions.h"

namespace Tucuxi {
namespace Core {

class Sample
{
};

typedef std::vector<Sample> Samples;

class SampleEvent : public TimedEvent
{
public:

    /// \brief Constructor defining the time of the sample event and its value.
    /// \param _time Time of the event to set.
    /// \param _value Value of the sample.
    /// \pre _time.isValid() == true
    /// \post m_time == _time
    /// \post m_value == _value
    SampleEvent(DateTime _time, Value _value = 0) : TimedEvent(_time), m_value(_value)
    {
    }

    Value getValue() const { return m_value;}

protected:

    Value m_value;
};

typedef std::vector<SampleEvent> SampleSeries;

}
}

#endif // TUCUXI_CORE_SAMPLE_H
