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

    SubTargetDefinition(std::string _id, Value _value, std::shared_ptr<const Operation> _operation) :
        PopulationValue(_id, _value, _operation) {}

    SubTargetDefinition() : PopulationValue("",0, nullptr) {}

};


enum class TargetType {
    Residual,
    Peak,
    Mean,
    Auc,
    FullAuc
};


class TargetDefinition
{
public:

    TargetDefinition()
    {}

    TargetDefinition(TargetType _type,
                     SubTargetDefinition *_valueMin,
                     SubTargetDefinition *_valueMax,
                     SubTargetDefinition *_valueBest,
                     SubTargetDefinition *_tMin,
                     SubTargetDefinition *_tMax,
                     SubTargetDefinition *_tBest) :
        m_targetType(_type),
        m_valueMin(_valueMin),
        m_valueMax(_valueMax),
        m_valueBest(_valueBest),
        m_tMin(_tMin),
        m_tMax(_tMax),
        m_tBest(_tBest)
    {

    }

    TargetType getTargetType() const { return m_targetType;}

    const SubTargetDefinition & getCMin() const { return *m_valueMin;}
    const SubTargetDefinition & getCMax() const { return *m_valueMax;}
    const SubTargetDefinition & getCBest() const { return *m_valueBest;}
    const SubTargetDefinition & getTMin() const { return *m_tMin;}
    const SubTargetDefinition & getTMax() const { return *m_tMax;}
    const SubTargetDefinition & getTBest() const { return *m_tBest;}

//protected:
    TargetType m_targetType;

    std::unique_ptr<SubTargetDefinition> m_valueMin;
    std::unique_ptr<SubTargetDefinition> m_valueMax;
    std::unique_ptr<SubTargetDefinition> m_valueBest;
    std::unique_ptr<SubTargetDefinition> m_tMin;
    std::unique_ptr<SubTargetDefinition> m_tMax;
    std::unique_ptr<SubTargetDefinition> m_tBest;
};


typedef std::vector<std::unique_ptr<TargetDefinition>> TargetDefinitions;

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
    Target(const TargetDefinition& _targetDef) :
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


typedef std::vector<Target*> Targets;

class TargetEvent : public TimedEvent
{
};

typedef std::vector<TargetEvent> TargetSeries;
}
}

#endif // TUCUXI_CORE_TARGETEVENT_H
