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

    SubTargetDefinition(std::string _id, Value _value, Operation *_operation) :
        PopulationValue(_id, _value, _operation) {}

//    SubTargetDefinition(SubTargetDefinition && other) : PopulationValue(std::move(other)) {}

//    SubTargetDefinition(const SubTargetDefinition & other) : PopulationValue(std::move(other)) {}

    SubTargetDefinition() : PopulationValue("",0, nullptr) {}

};

// TOCHECK : Mettre m_value au lieu de m_c
// TOCHECK : Sortir des enum class des autres classes

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
                     SubTargetDefinition *_cMin,
                     SubTargetDefinition *_cMax,
                     SubTargetDefinition *_cBest,
                     SubTargetDefinition *_tMin,
                     SubTargetDefinition *_tMax,
                     SubTargetDefinition *_tBest) :
        m_targetType(_type),
        m_cMin(_cMin),
        m_cMax(_cMax),
        m_cBest(_cBest),
        m_tMin(_tMin),
        m_tMax(_tMax),
        m_tBest(_tBest)
    {

    }

    TargetType getTargetType() const { return m_targetType;}

    const SubTargetDefinition & getCMin() const { return *m_cMin;}
    const SubTargetDefinition & getCMax() const { return *m_cMax;}
    const SubTargetDefinition & getCBest() const { return *m_cBest;}
    const SubTargetDefinition & getTMin() const { return *m_tMin;}
    const SubTargetDefinition & getTMax() const { return *m_tMax;}
    const SubTargetDefinition & getTBest() const { return *m_tBest;}

//protected:
    TargetType m_targetType;

    std::unique_ptr<SubTargetDefinition> m_cMin;
    std::unique_ptr<SubTargetDefinition> m_cMax;
    std::unique_ptr<SubTargetDefinition> m_cBest;
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
        m_cMin(_targetDef.getCMin()),
        m_cMax(_targetDef.getCMin()),
        m_cBest(_targetDef.getCMin()),
        m_tMin(_targetDef.getCMin()),
        m_tMax(_targetDef.getCMin()),
        m_tBest(_targetDef.getCMin())
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
