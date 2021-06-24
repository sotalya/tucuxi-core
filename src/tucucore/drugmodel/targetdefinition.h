/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TARGETDEFINITION_H
#define TARGETDEFINITION_H



#include <vector>

#include "tucucore/timedevent.h"
#include "tucucore/drugdefinitions.h"
#include "tucucore/invariants.h"


namespace Tucuxi {
namespace Core {

class TargetExtractor;

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
    Auc24,
    CumulativeAuc,
    AucOverMic,
    Auc24OverMic,
    TimeOverMic,
    AucDividedByMic,
    Auc24DividedByMic,
    PeakDividedByMic,
    ResidualDividedByMic
};

std::string toString(TargetType _type);

///
/// \brief The TargetDefinition class
///
/// Time is in minutes
class TargetDefinition
{
public:

    TargetDefinition() : m_activeMoietyId("NoActiveMoietyId")
    {}

    TargetDefinition(TargetType _type,
                     TucuUnit _unit,
                     ActiveMoietyId _activeMoietyId,
                     std::unique_ptr<SubTargetDefinition> _valueMin,
                     std::unique_ptr<SubTargetDefinition> _valueMax,
                     std::unique_ptr<SubTargetDefinition> _valueBest,
                     std::unique_ptr<SubTargetDefinition> _mic,
                     std::unique_ptr<SubTargetDefinition> _tMin,
                     std::unique_ptr<SubTargetDefinition> _tMax,
                     std::unique_ptr<SubTargetDefinition> _tBest,
                     std::unique_ptr<SubTargetDefinition> _toxicityAlarm,
                     std::unique_ptr<SubTargetDefinition> _inefficacyAlarm,
                     TucuUnit _micUnit = TucuUnit(),
                     TucuUnit _timeUnit = TucuUnit("h")) :
        m_targetType(_type),
        m_unit(_unit),
        m_micUnit(_micUnit),
        m_timeUnit(_timeUnit),
        m_activeMoietyId(_activeMoietyId),
        m_valueMin(std::move(_valueMin)),
        m_valueMax(std::move(_valueMax)),
        m_valueBest(std::move(_valueBest)),
        m_mic(std::move(_mic)),
        m_tMin(std::move(_tMin)),
        m_tMax(std::move(_tMax)),
        m_tBest(std::move(_tBest)),
        m_toxicityAlarm(std::move(_toxicityAlarm)),
        m_inefficacyAlarm(std::move(_inefficacyAlarm))
    {

    }

    void setActiveMoietyId(ActiveMoietyId _activeMoietyId) { m_activeMoietyId = _activeMoietyId;}
    TargetType getTargetType() const { return m_targetType;}
    ActiveMoietyId getActiveMoietyId() const { return m_activeMoietyId;}
    TucuUnit getUnit() const { return m_unit;}
    TucuUnit getMicUnit() const { return m_micUnit;}
    TucuUnit getTimeUnit() const { return m_timeUnit;}

    const SubTargetDefinition & getCMin() const { return *m_valueMin;}
    const SubTargetDefinition & getCMax() const { return *m_valueMax;}
    const SubTargetDefinition & getCBest() const { return *m_valueBest;}
    const SubTargetDefinition & getMic() const { return *m_mic;}
    const SubTargetDefinition & getTMin() const { return *m_tMin;}
    const SubTargetDefinition & getTMax() const { return *m_tMax;}
    const SubTargetDefinition & getTBest() const { return *m_tBest;}
    const SubTargetDefinition & getToxicityAlarm() const { return *m_toxicityAlarm;}
    const SubTargetDefinition & getInefficacyAlarm() const { return *m_inefficacyAlarm;}

    INVARIANTS(
            INVARIANT(Invariants::INV_TARGETDEFINITION_0001, (m_activeMoietyId.size() > 0), "a target has no active moiety Id")
            INVARIANT(Invariants::INV_TARGETDEFINITION_0002, ((m_targetType != TargetType::AucOverMic) || (m_mic != nullptr)), "a target of type AucOverMic requires a MIC field")
            INVARIANT(Invariants::INV_TARGETDEFINITION_0003, ((m_targetType != TargetType::Auc24OverMic) || (m_mic != nullptr)), "a target of type Auc24OverMic requires a MIC field")
            INVARIANT(Invariants::INV_TARGETDEFINITION_0004, ((m_targetType != TargetType::TimeOverMic) || (m_mic != nullptr)), "a target of type TimeOverMic requires a MIC field")
            INVARIANT(Invariants::INV_TARGETDEFINITION_0005, ((m_targetType != TargetType::AucDividedByMic) || (m_mic != nullptr)), "a target of type AucDividedByMic requires a MIC field")
            INVARIANT(Invariants::INV_TARGETDEFINITION_0006, ((m_targetType != TargetType::Auc24DividedByMic) || (m_mic != nullptr)), "a target of type Auc24DividedByMic requires a MIC field")
            INVARIANT(Invariants::INV_TARGETDEFINITION_0007, ((m_targetType != TargetType::PeakDividedByMic) || (m_mic != nullptr)), "a target of type PeakDividedByMic requires a MIC field")
            INVARIANT(Invariants::INV_TARGETDEFINITION_0008, ((m_targetType != TargetType::ResidualDividedByMic) || (m_mic != nullptr)), "a target of type ResidualDividedByMic requires a MIC field")
            )

protected:
    TargetType m_targetType;
    TucuUnit m_unit;
    TucuUnit m_micUnit;
    TucuUnit m_timeUnit;
    ActiveMoietyId m_activeMoietyId;

    std::unique_ptr<SubTargetDefinition> m_valueMin;
    std::unique_ptr<SubTargetDefinition> m_valueMax;
    std::unique_ptr<SubTargetDefinition> m_valueBest;
    std::unique_ptr<SubTargetDefinition> m_mic;
    std::unique_ptr<SubTargetDefinition> m_tMin;
    std::unique_ptr<SubTargetDefinition> m_tMax;
    std::unique_ptr<SubTargetDefinition> m_tBest;
    std::unique_ptr<SubTargetDefinition> m_toxicityAlarm;
    std::unique_ptr<SubTargetDefinition> m_inefficacyAlarm;

    friend TargetExtractor;
};


typedef std::vector<std::unique_ptr<TargetDefinition> > TargetDefinitions;

} // namespace Core
} // namespace Tucuxi


#endif // TARGETDEFINITION_H
