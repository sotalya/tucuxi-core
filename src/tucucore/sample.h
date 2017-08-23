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
    Value getValue() const { return 0.0;}
};

typedef std::vector<SampleEvent> SampleSeries;

}
}

#endif // TUCUXI_CORE_SAMPLE_H
