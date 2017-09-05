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

    SubTarget(const SubTargetDefinition &_def) :
        IndividualValue<SubTargetDefinition>(_def)
    {}

};

class Target
{
public:
    Target(const TargetDefinition& _targetDef,
           const SubTargetDefinition& _cMinDef) :
        m_targetDefinition(_targetDef),
        m_cMin(_cMinDef)
  //      m_cMaxDefinition(_targetDef.m_cMax),
    //    m_cBestDefinition(_targetDef.m_cBest),
    //    m_tMinDefinition(_targetDef.m_tMin),
    //    m_tMaxDefinition(_targetDef.m_tMax),
    //    m_tBestDefinition(_targetDef.m_tBest)
    {}

protected:

    const TargetDefinition& m_targetDefinition;

    const SubTarget& m_cMin;
/*    const SubTarget& m_cMaxDefinition;
    const SubTarget& m_cBestDefinition;
    const SubTarget& m_tMinDefinition;
    const SubTarget& m_tMaxDefinition;
    const SubTarget& m_tBestDefinition;
*/
};

typedef std::vector<Target*> Targets;

class TargetEvent : public TimedEvent
{
};

typedef std::vector<TargetEvent> TargetSeries;
}
}

#endif // TUCUXI_CORE_TARGETEVENT_H
