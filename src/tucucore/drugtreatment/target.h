/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_TARGET_H
#define TUCUXI_CORE_TARGET_H

#include <vector>

#include "tucucore/timedevent.h"
#include "tucucore/drugdefinitions.h"
#include "tucucore/drugmodel/targetdefinition.h"
#include "tucucommon/duration.h"

namespace Tucuxi {
namespace Core {


class Target
{
    TargetType m_targetType;
    Value m_valueMin;
    Value m_valueMax;
    Value m_valueBest;
    Tucuxi::Common::Duration m_tMin;
    Tucuxi::Common::Duration m_tMax;
    Tucuxi::Common::Duration m_tBest;

};


typedef std::vector<Target*> Targets;

}
}

#endif // TUCUXI_CORE_TARGET_H
