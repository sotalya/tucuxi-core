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


#ifndef TUCUXI_CORE_TARGET_H
#define TUCUXI_CORE_TARGET_H

#include <vector>

#include "tucucommon/duration.h"

#include "tucucore/drugdefinitions.h"
#include "tucucore/drugmodel/activemoiety.h"
#include "tucucore/drugmodel/targetdefinition.h"
#include "tucucore/timedevent.h"

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

    // clang-format off
    Target(ActiveMoietyId _activeMoietyId,
           TargetType _type, 
           Value _min, 
           Value _best, 
           Value _max);
    // clang-format on

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

    // clang-format off
    Target(ActiveMoietyId _activeMoietyId,
           TargetType _type,
           Value _min,
           Value _best,
           Value _max,
           Value _mic);
    // clang-format on

    Target(ActiveMoietyId _activeMoietyId,
           TargetType _type,
           TucuUnit _unit,
           Value _vmin,
           Value _vbest,
           Value _vmax,
           Value _mic,
           TucuUnit _micUnit,
           Tucuxi::Common::Duration _tmin,
           Tucuxi::Common::Duration _tbest,
           Tucuxi::Common::Duration _tmax);

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
           Tucuxi::Common::Duration _tmin,
           Tucuxi::Common::Duration _tbest,
           Tucuxi::Common::Duration _tmax);


    ActiveMoietyId getActiveMoietyId() const
    {
        return m_activeMoietyId;
    }

    TargetType getTargetType() const
    {
        return m_targetType;
    }

    TucuUnit getUnit() const
    {
        return m_unit;
    }

    Value getValueMin() const
    {
        return m_valueMin;
    }

    Value getValueBest() const
    {
        return m_valueBest;
    }

    Value getValueMax() const
    {
        return m_valueMax;
    }

    Value getInefficacyAlarm() const
    {
        return m_inefficacyAlarm;
    }

    Value getToxicityAlarm() const
    {
        return m_toxicityAlarm;
    }

    Value getMicValue() const
    {
        return m_mic;
    }

    TucuUnit getMicUnit() const
    {
        return m_micUnit;
    }

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
    Value m_mic{};

    /// Unit of the MIC
    TucuUnit m_micUnit;

    /// Value under which the drug is inefficient
    Value m_inefficacyAlarm{};

    /// Value over which the drug can be toxic
    Value m_toxicityAlarm{};

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
};

///
/// \brief Targets A simple vector of targets
///
typedef std::vector<std::unique_ptr<Target> > Targets;

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_TARGET_H
