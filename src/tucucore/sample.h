/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_SAMPLE_H
#define TUCUXI_CORE_SAMPLE_H

#include <vector>

#include "tucucore/timedevent.h"

namespace Tucuxi {
namespace Core {

class Sample
{
};

typedef std::vector<Sample> Samples;

class SampleEvent : public TimedEvent
{
};

typedef std::vector<SampleEvent> SampleSeries;

}
}

#endif // TUCUXI_CORE_SAMPLE_H