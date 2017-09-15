/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_TARGETEVENT_H
#define TUCUXI_CORE_TARGETEVENT_H

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



class SubTarget : public IndividualValue<SubTargetDefinition>
{
public:

    SubTarget(const SubTargetDefinition &_def) :
        IndividualValue<SubTargetDefinition>(_def)
    {}

};


class TargetEvent : public TimedEvent
{

public:
    TargetEvent(const TargetDefinition& _targetDef, const DateTime& _date) :
        TimedEvent(_date),
        m_targetDefinition(_targetDef),
        m_valueMin(_targetDef.getCMin()),
        m_valueMax(_targetDef.getCMin()),
        m_valueBest(_targetDef.getCMin()),
        m_tMin(_targetDef.getCMin()),
        m_tMax(_targetDef.getCMin()),
        m_tBest(_targetDef.getCMin())
    {}

protected:

    const TargetDefinition& m_targetDefinition;

    SubTarget m_valueMin;
    SubTarget m_valueMax;
    SubTarget m_valueBest;
    SubTarget m_tMin;
    SubTarget m_tMax;
    SubTarget m_tBest;

};

typedef std::vector<TargetEvent> TargetSeries;
}
}

#endif // TUCUXI_CORE_TARGETEVENT_H
