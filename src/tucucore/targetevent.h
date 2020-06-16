/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_TARGETEVENT_H
#define TUCUXI_CORE_TARGETEVENT_H

#include <vector>

#include "tucucore/timedevent.h"
#include "tucucore/drugtreatment/target.h"
#include "tucucore/drugdefinitions.h"
#include "tucucore/drugmodel/targetdefinition.h"
#include "tucucommon/duration.h"


namespace Tucuxi {
namespace Core {

class TargetEvaluator;
class Target;

class TargetEvent
{
public:
    TargetEvent() = delete;

    ActiveMoietyId getActiveMoietyId() const { return m_activeMoietyId;}

    static TargetEvent createTargetEvent(ActiveMoietyId _activeMoietyId,
                                         TargetType _type,
                                         Unit _unit,
                                         Unit _finalUnit,
                                         Value _vmin,
                                         Value _vbest,
                                         Value _vmax,
                                         Value _mic,
                                         Unit _micUnit);

    static TargetEvent createTargetEvent(ActiveMoietyId _activeMoietyId,
                                         TargetType _type,
                                         Unit _unit,
                                         Unit _finalUnit,
                                         Value _vmin,
                                         Value _vbest,
                                         Value _vmax,
                                         const Tucuxi::Common::Duration &_tmin,
                                         const Tucuxi::Common::Duration &_tbest,
                                         const Tucuxi::Common::Duration &_tmax);

    static TargetEvent createTargetEvent(ActiveMoietyId _activeMoietyId,
                                         TargetType _type,
                                         Unit _unit,
                                         Unit _finalUnit,
                                         Value _vmin,
                                         Value _vbest,
                                         Value _vmax);

    static TargetEvent createTargetEvent(ActiveMoietyId _activeMoietyId,
                                         TargetType _type,
                                         Unit _unit,
                                         Unit _finalUnit,
                                         Value _vmin,
                                         Value _vbest,
                                         Value _vmax,
                                         Value _mic,
                                         Unit _micUnit,
                                         const Tucuxi::Common::Duration &_tmin,
                                         const Tucuxi::Common::Duration &_tbest,
                                         const Tucuxi::Common::Duration &_tmax);



private:

    TargetEvent(ActiveMoietyId _activeMoietyId,
           TargetType _type,
           Unit _unit,
           Unit _finalUnit,
           Value _vmin,
           Value _vbest,
           Value _vmax,
           Value _mic,
           Unit _micUnit,
           const Tucuxi::Common::Duration &_tmin,
           const Tucuxi::Common::Duration &_tbest,
           const Tucuxi::Common::Duration &_tmax);


    /// Id of the active moiety on which applying the target
    ActiveMoietyId m_activeMoietyId;

    /// Type of target
    TargetType m_targetType;

    /// Target minimum acceptable value
    Value m_valueMin;

    /// Target maximum acceptable value
    Value m_valueMax;

    /// Target best value
    Value m_valueBest;

    /// Minimum inhibitory concentration (for antibiotics)
    Value m_mic;

    /// Unit of the MIC
    Unit m_micUnit;

    /// Value under which the drug is inefficient
    Value m_inefficacyAlarm;

    /// Value over which the drug can be toxic
    Value m_toxicityAlarm;

    /// Target minimum time from last intake (for peak targets)
    Tucuxi::Common::Duration m_tMin;

    /// Target maximum time from last intake (for peak targets)
    Tucuxi::Common::Duration m_tMax;

    /// Target best time from last intake (for peak targets)
    Tucuxi::Common::Duration m_tBest;

    /// Unit of the target
    Unit m_unit;

    /// Unit of the result we are interesting in
    /// For instance, the target, internally could be ug/l, while we want the evaluation
    /// to be in mg/l
    Unit m_finalUnit;

    friend TargetEvaluator;

};


//typedef Target TargetEvent;

typedef std::vector<TargetEvent> TargetSeries;
} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_TARGETEVENT_H
