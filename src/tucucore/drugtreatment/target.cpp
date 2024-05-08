//@@license@@

#include <utility>

#include "target.h"

#include "tucucommon/duration.h"

namespace Tucuxi {
namespace Core {

Target::Target(
        ActiveMoietyId _activeMoietyId, const TargetType _type, const Value _min, const Value _best, const Value _max)
    : m_activeMoietyId(std::move(_activeMoietyId)), m_targetType(_type), m_valueMin(_min), m_valueMax(_max),
      m_valueBest(_best)
{
}

Target::Target(
        ActiveMoietyId _activeMoietyId,
        const TargetType _type,
        const Value _min,
        const Value _best,
        const Value _max,
        const Value _inefficacyAlarm,
        const Value _toxicityAlarm)
    : m_activeMoietyId(std::move(_activeMoietyId)), m_targetType(_type), m_valueMin(_min), m_valueMax(_max),
      m_valueBest(_best), m_inefficacyAlarm(_inefficacyAlarm), m_toxicityAlarm(_toxicityAlarm)
{
}

Target::Target(
        ActiveMoietyId _activeMoietyId,
        const TargetType _type,
        TucuUnit _unit,
        const Value _min,
        const Value _best,
        const Value _max,
        const Value _inefficacyAlarm,
        const Value _toxicityAlarm)
    : m_activeMoietyId(std::move(_activeMoietyId)), m_targetType(_type), m_valueMin(_min), m_valueMax(_max),
      m_valueBest(_best), m_inefficacyAlarm(_inefficacyAlarm), m_toxicityAlarm(_toxicityAlarm), m_unit(std::move(_unit))
{
}

Target::Target(ActiveMoietyId _activeMoietyId, TargetType _type, Value _min, Value _best, Value _max, Value _mic)
    : m_activeMoietyId(std::move(_activeMoietyId)), m_targetType(_type), m_valueMin(_min), m_valueMax(_max),
      m_valueBest(_best), m_mic(_mic)
{
}

Target::Target(
        ActiveMoietyId _activeMoietyId,
        TargetType _type,
        TucuUnit _unit,
        Value _vmin,
        Value _vbest,
        Value _vmax,
        Value _mic,
        TucuUnit _micUnit,
        Tucuxi::Common::Duration _tmin,
        Tucuxi::Common::Duration _tbest,
        Tucuxi::Common::Duration _tmax)
    : m_activeMoietyId(std::move(_activeMoietyId)), m_targetType(_type), m_valueMin(_vmin), m_valueMax(_vmax),
      m_valueBest(_vbest), m_mic(_mic), m_micUnit(std::move(_micUnit)), m_tMin(_tmin), m_tMax(_tmax), m_tBest(_tbest),
      m_unit(std::move(_unit))
{
}

Target::Target(
        ActiveMoietyId _activeMoietyId,
        TargetType _type,
        TucuUnit _unit,
        Value _vmin,
        Value _vbest,
        Value _vmax,
        Value _inefficacyAlarm,
        Value _toxicityAlarm,
        Value _mic,
        TucuUnit _micUnit)
    : m_activeMoietyId(std::move(_activeMoietyId)), m_targetType(_type), m_valueMin(_vmin), m_valueMax(_vmax),
      m_valueBest(_vbest), m_mic(_mic), m_micUnit(std::move(_micUnit)), m_inefficacyAlarm(_inefficacyAlarm),
      m_toxicityAlarm(_toxicityAlarm), m_unit(std::move(_unit))
{
}


Target::Target(
        ActiveMoietyId _activeMoietyId,
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
        Tucuxi::Common::Duration _tmax)
    : m_activeMoietyId(std::move(_activeMoietyId)), m_targetType(_type), m_valueMin(_vmin), m_valueMax(_vmax),
      m_valueBest(_vbest), m_mic(_mic), m_micUnit(std::move(_micUnit)), m_inefficacyAlarm(_inefficacyAlarm),
      m_toxicityAlarm(_toxicityAlarm), m_tMin(_tmin), m_tMax(_tmax), m_tBest(_tbest), m_unit(std::move(_unit))
{
}

} // namespace Core
} // namespace Tucuxi
