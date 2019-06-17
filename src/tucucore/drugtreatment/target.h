/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_TARGET_H
#define TUCUXI_CORE_TARGET_H

#include <vector>

#include "tucucore/timedevent.h"
#include "tucucore/drugdefinitions.h"
#include "tucucore/drugmodel/targetdefinition.h"
#include "tucucommon/duration.h"

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

    Target(const std::string& _activeMoietyId,
           TargetType _type, 
           Value _min, 
           Value _best, 
           Value _max);

    Target(const std::string& _activeMoietyId,
           TargetType _type,
           Value _min,
           Value _best,
           Value _max,
           Value _inefficacyAlarm,
           Value _toxicityAlarm);

    Target(const std::string& _activeMoietyId,
           TargetType _type,
           Unit _unit,
           Value _min,
           Value _best,
           Value _max,
           Value _inefficacyAlarm,
           Value _toxicityAlarm);

    Target(const std::string& _activeMoietyId,
           TargetType _type,
           Value _min,
           Value _best,
           Value _max,
           Value _mic);

    Target(const std::string& _activeMoietyId,
           TargetType _type,
           Unit _unit,
           Unit _finalUnit,
           Value _vmin,
           Value _vbest,
           Value _vmax,
           Value _mic,
           const Tucuxi::Common::Duration &_tmin,
           const Tucuxi::Common::Duration &_tbest,
           const Tucuxi::Common::Duration &_tmax);

    std::string getActiveMoietyId() const { return m_activeMoietyId;}

private:
    /// Id of the active moiety on which applying the target
    std::string m_activeMoietyId;

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
