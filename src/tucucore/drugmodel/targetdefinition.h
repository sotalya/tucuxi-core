/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TARGETDEFINITION_H
#define TARGETDEFINITION_H



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

    SubTargetDefinition() : PopulationValue("",0, nullptr) {}

};


enum class TargetType {
    UnknownTarget = 0,
    Residual,
    Peak,
    Mean,
    Auc,
    CumulativeAuc,
    AucOverMic,
    TimeOverMic,
    AucDividedByMic,
    PeakDividedByMic
};


class TargetDefinition
{
public:

    TargetDefinition()
    {}

    TargetDefinition(TargetType _type,
                     Unit _unit,
                     std::string _activeMoietyId,
                     std::unique_ptr<SubTargetDefinition> _valueMin,
                     std::unique_ptr<SubTargetDefinition> _valueMax,
                     std::unique_ptr<SubTargetDefinition> _valueBest,
                     std::unique_ptr<SubTargetDefinition> _tMin,
                     std::unique_ptr<SubTargetDefinition> _tMax,
                     std::unique_ptr<SubTargetDefinition> _tBest,
                     std::unique_ptr<SubTargetDefinition> _toxicityAlarm,
                     std::unique_ptr<SubTargetDefinition> _inefficacyAlarm) :
        m_targetType(_type),
        m_unit(_unit),
        m_activeMoietyId(_activeMoietyId),
        m_valueMin(std::move(_valueMin)),
        m_valueMax(std::move(_valueMax)),
        m_valueBest(std::move(_valueBest)),
        m_tMin(std::move(_tMin)),
        m_tMax(std::move(_tMax)),
        m_tBest(std::move(_tBest)),
        m_toxicityAlarm(std::move(_toxicityAlarm)),
        m_inefficacyAlarm(std::move(_inefficacyAlarm))
    {

    }

    TargetType getTargetType() const { return m_targetType;}
    std::string getActiveMoietyId() const { return m_activeMoietyId;}
    Unit getUnit() const { return m_unit;}

    const SubTargetDefinition & getCMin() const { return *m_valueMin;}
    const SubTargetDefinition & getCMax() const { return *m_valueMax;}
    const SubTargetDefinition & getCBest() const { return *m_valueBest;}
    const SubTargetDefinition & getTMin() const { return *m_tMin;}
    const SubTargetDefinition & getTMax() const { return *m_tMax;}
    const SubTargetDefinition & getTBest() const { return *m_tBest;}
    const SubTargetDefinition & getToxicityAlarm() const { return *m_toxicityAlarm;}
    const SubTargetDefinition & getInefficacyAlarm() const { return *m_inefficacyAlarm;}

//protected:
    TargetType m_targetType;
    Unit m_unit;
    std::string m_activeMoietyId;

    std::unique_ptr<SubTargetDefinition> m_valueMin;
    std::unique_ptr<SubTargetDefinition> m_valueMax;
    std::unique_ptr<SubTargetDefinition> m_valueBest;
    std::unique_ptr<SubTargetDefinition> m_tMin;
    std::unique_ptr<SubTargetDefinition> m_tMax;
    std::unique_ptr<SubTargetDefinition> m_tBest;
    std::unique_ptr<SubTargetDefinition> m_toxicityAlarm;
    std::unique_ptr<SubTargetDefinition> m_inefficacyAlarm;
};


typedef std::vector<std::unique_ptr<TargetDefinition> > TargetDefinitions;

} // namespace Core
} // namespace Tucuxi


#endif // TARGETDEFINITION_H
