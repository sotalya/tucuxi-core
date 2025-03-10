/* 
 * Tucuxi - Tucuxi-core library and command line tool. 
 * This code allows to perform prediction of drug concentration in blood 
 * and to propose dosage adaptations.
 * It has been developed by HEIG-VD, in close collaboration with CHUV. 
 * Copyright (C) 2023 HEIG-VD, maintained by Yann Thoma  <yann.thoma@heig-vd.ch>
 * 
 * This program is free software: you can redistribute it and/or modify 
 * it under the terms of the GNU Affero General Public License as 
 * published by the Free Software Foundation, either version 3 of the 
 * License, or any later version. 
 * 
 * This program is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU Affero General Public License for more details. 
 * 
 * You should have received a copy of the GNU Affero General Public License 
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */


#ifndef TARGETDEFINITION_H
#define TARGETDEFINITION_H



#include <vector>

#include "tucucore/drugdefinitions.h"
#include "tucucore/invariants.h"
#include "tucucore/timedevent.h"


namespace Tucuxi {
namespace Core {

class TargetExtractor;

class SubTargetDefinition : public PopulationValue
{
public:
    SubTargetDefinition(const std::string& _id, Value _value, std::unique_ptr<Operation> _operation)
        : PopulationValue(_id, _value, std::move(_operation))
    {
    }

    SubTargetDefinition() : PopulationValue("", 0, nullptr) {}
};


enum class TargetType
{
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
    ResidualDividedByMic,
    FractionTimeOverMic,
};

std::string toString(TargetType _type);

///
/// \brief The TargetDefinition class
///
/// Time is in minutes
class TargetDefinition
{
public:
    TargetDefinition() : m_activeMoietyId("NoActiveMoietyId") {}

    TargetDefinition(
            TargetType _type,
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
            TucuUnit _timeUnit = TucuUnit("h"))
        : m_targetType(_type), m_unit(std::move(_unit)), m_micUnit(std::move(_micUnit)),
          m_timeUnit(std::move(_timeUnit)), m_activeMoietyId(std::move(_activeMoietyId)),
          m_valueMin(std::move(_valueMin)), m_valueMax(std::move(_valueMax)), m_valueBest(std::move(_valueBest)),
          m_mic(std::move(_mic)), m_tMin(std::move(_tMin)), m_tMax(std::move(_tMax)), m_tBest(std::move(_tBest)),
          m_toxicityAlarm(std::move(_toxicityAlarm)), m_inefficacyAlarm(std::move(_inefficacyAlarm))
    {
    }

    void setActiveMoietyId(ActiveMoietyId _activeMoietyId)
    {
        m_activeMoietyId = std::move(_activeMoietyId);
    }
    TargetType getTargetType() const
    {
        return m_targetType;
    }
    ActiveMoietyId getActiveMoietyId() const
    {
        return m_activeMoietyId;
    }
    TucuUnit getUnit() const
    {
        return m_unit;
    }
    TucuUnit getMicUnit() const
    {
        return m_micUnit;
    }
    TucuUnit getTimeUnit() const
    {
        return m_timeUnit;
    }

    const SubTargetDefinition& getCMin() const
    {
        return *m_valueMin;
    }
    const SubTargetDefinition& getCMax() const
    {
        return *m_valueMax;
    }
    const SubTargetDefinition& getCBest() const
    {
        return *m_valueBest;
    }
    const SubTargetDefinition& getMic() const
    {
        return *m_mic;
    }
    const SubTargetDefinition& getTMin() const
    {
        return *m_tMin;
    }
    const SubTargetDefinition& getTMax() const
    {
        return *m_tMax;
    }
    const SubTargetDefinition& getTBest() const
    {
        return *m_tBest;
    }
    const SubTargetDefinition& getToxicityAlarm() const
    {
        return *m_toxicityAlarm;
    }
    const SubTargetDefinition& getInefficacyAlarm() const
    {
        return *m_inefficacyAlarm;
    }

    // For now we do not have invariants on the target values, but we could add some,
    // related to the DFS specifications. It would however maybe invalidate drug models
    // where the toxicity and inefficacy alarms are wrong.
    INVARIANTS(INVARIANT(
                       Invariants::INV_TARGETDEFINITION_0001,
                       (!m_activeMoietyId.empty()),
                       "a target has no active moiety Id");
               INVARIANT(
                       Invariants::INV_TARGETDEFINITION_0002,
                       ((m_targetType != TargetType::AucOverMic) || (m_mic != nullptr)),
                       "a target of type AucOverMic requires a MIC field");
               INVARIANT(
                       Invariants::INV_TARGETDEFINITION_0003,
                       ((m_targetType != TargetType::Auc24OverMic) || (m_mic != nullptr)),
                       "a target of type Auc24OverMic requires a MIC field");
               INVARIANT(
                       Invariants::INV_TARGETDEFINITION_0004,
                       ((m_targetType != TargetType::TimeOverMic) || (m_mic != nullptr)),
                       "a target of type TimeOverMic requires a MIC field");
               INVARIANT(
                       Invariants::INV_TARGETDEFINITION_0005,
                       ((m_targetType != TargetType::AucDividedByMic) || (m_mic != nullptr)),
                       "a target of type AucDividedByMic requires a MIC field");
               INVARIANT(
                       Invariants::INV_TARGETDEFINITION_0006,
                       ((m_targetType != TargetType::Auc24DividedByMic) || (m_mic != nullptr)),
                       "a target of type Auc24DividedByMic requires a MIC field");
               INVARIANT(
                       Invariants::INV_TARGETDEFINITION_0007,
                       ((m_targetType != TargetType::PeakDividedByMic) || (m_mic != nullptr)),
                       "a target of type PeakDividedByMic requires a MIC field");
               INVARIANT(
                       Invariants::INV_TARGETDEFINITION_0008,
                       ((m_targetType != TargetType::ResidualDividedByMic) || (m_mic != nullptr)),
                       "a target of type ResidualDividedByMic requires a MIC field");)

protected:
    TargetType m_targetType{TargetType::UnknownTarget};
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
