/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_TARGET_H
#define TUCUXI_CORE_TARGET_H

#include <vector>

#include "tucucore/timedevent.h"
#include "tucucore/drugdefinitions.h"
#include "tucucore/drugmodel/activemoiety.h"
#include "tucucore/drugmodel/targetdefinition.h"
#include "tucucommon/duration.h"
#include "tucucore/targetevent.h"

struct TestTargetExtractor;

namespace Tucuxi {
namespace Core {

class TargetExtractor;
class TargetEvaluator;

///
/// \brief A target defined within a DrugTreatment
/// It should be used to override the default DrugModel targets
///
class Target
{
public:
    Target() = delete;

    Target(ActiveMoietyId _activeMoietyId,
           TargetType _type, 
           Value _min, 
           Value _best, 
           Value _max);

    Target(ActiveMoietyId _activeMoietyId,
           TargetType _type,
           Value _min,
           Value _best,
           Value _max,
           Value _inefficacyAlarm,
           Value _toxicityAlarm);

    Target(ActiveMoietyId _activeMoietyId,
           TargetType _type,
           TucuUnit _unit,
           Value _min,
           Value _best,
           Value _max,
           Value _inefficacyAlarm,
           Value _toxicityAlarm);

    Target(ActiveMoietyId _activeMoietyId,
           TargetType _type,
           Value _min,
           Value _best,
           Value _max,
           Value _mic);

    Target(ActiveMoietyId _activeMoietyId,
           TargetType _type,
           TucuUnit _unit,
           Value _vmin,
           Value _vbest,
           Value _vmax,
           Value _mic,
           TucuUnit _micUnit,
           const Tucuxi::Common::Duration &_tmin,
           const Tucuxi::Common::Duration &_tbest,
           const Tucuxi::Common::Duration &_tmax);

    Target(ActiveMoietyId _activeMoietyId,
           TargetType _type,
           TucuUnit _unit,
           Value _vmin,
           Value _vbest,
           Value _vmax,
           Value _inefficacyAlarm,
           Value _toxicityAlarm,
           Value _mic,
           TucuUnit _micUnit);

    Target(ActiveMoietyId _activeMoietyId,
           TargetType _type,
           TucuUnit _unit,
           Value _vmin,
           Value _vbest,
           Value _vmax,
           Value _inefficacyAlarm,
           Value _toxicityAlarm,
           Value _mic,
           TucuUnit _micUnit,
           const Tucuxi::Common::Duration &_tmin,
           const Tucuxi::Common::Duration &_tbest,
           const Tucuxi::Common::Duration &_tmax);


    ActiveMoietyId getActiveMoietyId() const { return m_activeMoietyId;}

    TargetType getTargetType() const {return m_targetType;}

    TucuUnit getUnit() const {return m_unit;}

    Value getValueMin() const {return m_valueMin;}

    Value getValueBest() const {return m_valueBest;}

    Value getValueMax() const {return m_valueMax;}

    Value getInefficacyAlarm() const {return m_inefficacyAlarm;}

    Value getToxicityAlarm() const {return m_toxicityAlarm;}

    Value getMicValue() const {return m_mic;}

    TucuUnit getMicUnit() const {return m_micUnit;}

private:


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
    TucuUnit m_micUnit;

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
    TucuUnit m_unit;

    friend TargetExtractor;
    friend TargetEvaluator;

    friend TestTargetExtractor;
};

///
/// \brief Targets A simple vector of targets
///
typedef std::vector<std::unique_ptr<Target> > Targets;

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_TARGET_H
