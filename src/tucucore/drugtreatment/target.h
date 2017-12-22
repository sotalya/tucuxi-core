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

namespace Tucuxi {
namespace Core {

///
/// \brief A target defined within a DrugTreatment
/// It should be used to override the default DrugModel targets
///
class Target
{
public:
    Target() = delete;

    Target(const std::string& _analyteId, 
           const TargetType _type, 
           const Value _min, 
           const Value _best, 
           const Value _max);

    Target(const std::string& _analyteId, 
           const TargetType _type, 
           const Value _vmin, 
           const Value _vbest, 
           const Value _vmax,
           const Tucuxi::Common::Duration &_tmin, 
           const Tucuxi::Common::Duration &_tbest, 
           const Tucuxi::Common::Duration &_tmax);

private:
    /// Id of the analyte on which applying the target
    std::string m_analyteId;

    /// Type of target
    TargetType m_targetType;

    /// Target minimum acceptable value
    Value m_valueMin;

    /// Target maximum acceptable value
    Value m_valueMax;

    /// Target best value
    Value m_valueBest;

    /// Target minimum time from last intake (for peak targets)
    Tucuxi::Common::Duration m_tMin;

    /// Target maximum time from last intake (for peak targets)
    Tucuxi::Common::Duration m_tMax;

    /// Target best time from last intake (for peak targets)
    Tucuxi::Common::Duration m_tBest;
};

///
/// \brief Targets A simple vector of targets
///
typedef std::vector<std::unique_ptr<Target> > Targets;

}
}

#endif // TUCUXI_CORE_TARGET_H
