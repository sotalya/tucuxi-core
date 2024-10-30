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


#ifndef TUCUXI_CORE_TARGETEVENT_H
#define TUCUXI_CORE_TARGETEVENT_H

#include <vector>

#include "tucucore/drugdefinitions.h"
#include "tucucore/drugmodel/targetdefinition.h"
#include "tucucore/drugtreatment/target.h"


class TestTargetExtractor;

namespace Tucuxi {
namespace Core {

class TargetEvaluator;
class Target;

class TargetEvent
{
public:
    TargetEvent() = default;
    //TargetEvent() = delete;
    //TargetEvent(TargetEvent&) = default;
    //TargetEvent(TargetEvent&&) = default;

    ActiveMoietyId getActiveMoietyId() const
    {
        return m_activeMoietyId;
    }

    Value getValueMin() const
    {
        return m_valueMin;
    }

    Value getValueMax() const
    {
        return m_valueMax;
    }

    Value getValueBest() const
    {
        return m_valueBest;
    }

    Value getInefficacyAlarm() const
    {
        return m_inefficacyAlarm;
    }

    Value getToxicityAlarm() const
    {
        return m_toxicityAlarm;
    }

    static TargetEvent createTargetEventWithMic(
            ActiveMoietyId _activeMoietyId,
            TargetType _type,
            TucuUnit _unit,
            TucuUnit _finalUnit,
            Value _vmin,
            Value _vbest,
            Value _vmax,
            Value _mic,
            TucuUnit _micUnit);

    static TargetEvent createTargetEventWithTime(
            ActiveMoietyId _activeMoietyId,
            TargetType _type,
            TucuUnit _unit,
            TucuUnit _finalUnit,
            Value _vmin,
            Value _vbest,
            Value _vmax,
            const Tucuxi::Common::Duration& _tmin,
            const Tucuxi::Common::Duration& _tbest,
            const Tucuxi::Common::Duration& _tmax);

    static TargetEvent createTargetEventWithoutTimeAndMic(
            ActiveMoietyId _activeMoietyId,
            TargetType _type,
            TucuUnit _unit,
            TucuUnit _finalUnit,
            Value _vmin,
            Value _vbest,
            Value _vmax);

    static TargetEvent createTargetEventWithMicAndTime(
            ActiveMoietyId _activeMoietyId,
            TargetType _type,
            TucuUnit _unit,
            TucuUnit _finalUnit,
            Value _vmin,
            Value _vbest,
            Value _vmax,
            Value _mic,
            TucuUnit _micUnit,
            const Tucuxi::Common::Duration& _tmin,
            const Tucuxi::Common::Duration& _tbest,
            const Tucuxi::Common::Duration& _tmax);



private:
    TargetEvent(
            ActiveMoietyId _activeMoietyId,
            TargetType _type,
            TucuUnit _unit,
            TucuUnit _finalUnit,
            Value _vmin,
            Value _vbest,
            Value _vmax,
            Value _mic,
            TucuUnit _micUnit,
            Tucuxi::Common::Duration _tmin,
            Tucuxi::Common::Duration _tbest,
            Tucuxi::Common::Duration _tmax);


    /// Id of the active moiety on which applying the target
    ActiveMoietyId m_activeMoietyId{""};

    /// Type of target
    TargetType m_targetType{TargetType::UnknownTarget};

    /// Target minimum acceptable value
    Value m_valueMin{0.0};

    /// Target maximum acceptable value
    Value m_valueMax{0.0};

    /// Target best value
    Value m_valueBest{0.0};

    /// Minimum inhibitory concentration (for antibiotics)
    Value m_mic{0.0};

    /// Unit of the MIC
    TucuUnit m_micUnit{};

    /// Value under which the drug is inefficient
    Value m_inefficacyAlarm{0.0};

    /// Value over which the drug can be toxic
    Value m_toxicityAlarm{0.0};

    /// Target minimum time from last intake (for peak targets)
    Tucuxi::Common::Duration m_tMin;

    /// Target maximum time from last intake (for peak targets)
    Tucuxi::Common::Duration m_tMax;

    /// Target best time from last intake (for peak targets)
    Tucuxi::Common::Duration m_tBest;

    /// Unit of the target
    TucuUnit m_unit;

    /// Unit of the result we are interesting in
    /// For instance, the target, internally could be ug/l, while we want the evaluation
    /// to be in mg/l
    TucuUnit m_finalUnit;

    friend TargetEvaluator;

    friend TestTargetExtractor;
};


//typedef Target TargetEvent;

typedef std::vector<TargetEvent> TargetSeries;
} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_TARGETEVENT_H
