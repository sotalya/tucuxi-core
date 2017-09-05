/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_TARGETEVENT_H
#define TUCUXI_CORE_TARGETEVENT_H

#include <vector>

#include "tucucore/timedevent.h"
#include "tucucore/drugdefinitions.h"

namespace Tucuxi {
namespace Core {


class SubTargetDefinition : public PopulationValue
{
public:

    SubTargetDefinition() : PopulationValue("",0, nullptr) {}

};


class TargetDefinition
{
public:

    TargetDefinition()
    {m_cMin = SubTargetDefinition();}

    enum class TargetType {
        Residual,
        Peak,
        Mean,
        Auc
    };

    TargetType getTargetType() const { return m_targetType;}

    const SubTargetDefinition & getCMin() const { return m_cMin;}
    const SubTargetDefinition & getCMax() const { return m_cMax;}
    const SubTargetDefinition & getCBest() const { return m_cBest;}
    const SubTargetDefinition & getTMin() const { return m_tMin;}
    const SubTargetDefinition & getTMax() const { return m_tMax;}
    const SubTargetDefinition & getTBest() const { return m_tBest;}

//protected:
    TargetType m_targetType;

    SubTargetDefinition m_cMin;
    SubTargetDefinition m_cMax;
    SubTargetDefinition m_cBest;
    SubTargetDefinition m_tMin;
    SubTargetDefinition m_tMax;
    SubTargetDefinition m_tBest;
};

class SubTarget : public IndividualValue<SubTargetDefinition>
{
public:

    SubTarget(const SubTargetDefinition *_def) :
        IndividualValue<SubTargetDefinition>(*_def)
    {}

};

class Target
{
public:
    Target(const TargetDefinition& _targetDef) :
        m_targetDefinition(_targetDef),
        m_cMin(&_targetDef.m_cMin),
        m_cMax(&_targetDef.m_cMax),
        m_cBest(&_targetDef.m_cBest),
        m_tMin(&_targetDef.m_tMin),
        m_tMax(&_targetDef.m_tMax),
        m_tBest(&_targetDef.m_tBest)
    {}

protected:

    const TargetDefinition& m_targetDefinition;

    SubTarget m_cMin;
    SubTarget m_cMax;
    SubTarget m_cBest;
    SubTarget m_tMin;
    SubTarget m_tMax;
    SubTarget m_tBest;

};

typedef std::vector<Target*> Targets;

class TargetEvent : public TimedEvent
{
};

typedef std::vector<TargetEvent> TargetSeries;
}
}

#endif // TUCUXI_CORE_TARGETEVENT_H
