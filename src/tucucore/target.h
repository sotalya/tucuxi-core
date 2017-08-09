/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_TARGETEVENT_H
#define TUCUXI_CORE_TARGETEVENT_H

#include <vector>

#include "tucucore/timedevent.h"

namespace Tucuxi {
namespace Core {

class Target
{
};

typedef std::vector<Target> Targets;

class TargetEvent : public TimedEvent
{
};

typedef std::vector<TargetEvent> TargetSeries;
}
}

#endif // TUCUXI_CORE_TARGETEVENT_H