/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "target.h"

#include "tucucommon/duration.h"

namespace Tucuxi {
namespace Core {

Target::Target(ActiveMoietyId _activeMoietyId,
               const TargetType _type, 
               const Value _min, 
               const Value _best, 
               const Value _max)
    : m_activeMoietyId(_activeMoietyId),
      m_targetType(_type),
      m_valueMin(_min),
      m_valueMax(_max),
      m_valueBest(_best),
      m_inefficacyAlarm(0.0),
      m_toxicityAlarm(0.0)
{
}

Target::Target(ActiveMoietyId _activeMoietyId,
               const TargetType _type,
               const Value _min,
               const Value _best,
               const Value _max,
               const Value _inefficacyAlarm,
               const Value _toxicityAlarm)
    : m_activeMoietyId(_activeMoietyId),
      m_targetType(_type),
      m_valueMin(_min),
      m_valueMax(_max),
      m_valueBest(_best),
      m_inefficacyAlarm(_inefficacyAlarm),
      m_toxicityAlarm(_toxicityAlarm)
{
}

Target::Target(ActiveMoietyId _activeMoietyId,
               const TargetType _type,
               const Unit _unit,
               const Value _min,
               const Value _best,
               const Value _max,
               const Value _inefficacyAlarm,
               const Value _toxicityAlarm)
    : m_activeMoietyId(_activeMoietyId),
      m_targetType(_type),
      m_valueMin(_min),
      m_valueMax(_max),
      m_valueBest(_best),
      m_inefficacyAlarm(_inefficacyAlarm),
      m_toxicityAlarm(_toxicityAlarm),
      m_unit(_unit)
{
}

Target::Target(ActiveMoietyId _activeMoietyId,
       TargetType _type,
       Value _min,
       Value _best,
       Value _max,
       Value _mic)
    : m_activeMoietyId(_activeMoietyId),
      m_targetType(_type),
      m_valueMin(_min),
      m_valueMax(_max),
      m_valueBest(_best),
      m_mic(_mic)
{

}

Target::Target(ActiveMoietyId _activeMoietyId,
               TargetType _type,
               Unit _unit,
               Value _vmin,
               Value _vbest,
               Value _vmax,
               Value _mic,
               Unit _micUnit,
               const Tucuxi::Common::Duration &_tmin,
               const Tucuxi::Common::Duration &_tbest,
               const Tucuxi::Common::Duration &_tmax)
    : m_activeMoietyId(_activeMoietyId),
      m_targetType(_type),
      m_valueMin(_vmin),
      m_valueMax(_vmax),
      m_valueBest(_vbest),
      m_mic(_mic),
      m_micUnit(_micUnit),
      m_tMin(_tmin),
      m_tMax(_tmax),
      m_tBest(_tbest),
      m_unit(_unit)
{
}

Target::Target(ActiveMoietyId _activeMoietyId,
               TargetType _type,
               Unit _unit,
               Value _min,
               Value _best,
               Value _max,
               Value _inefficacyAlarm,
               Value _toxicityAlarm,
               Value _mic,
               Unit _micUnit)
    : m_activeMoietyId(_activeMoietyId),
      m_targetType(_type),
      m_valueMin(_min),
      m_valueMax(_max),
      m_valueBest(_best),
      m_mic(_mic),
      m_micUnit(_micUnit),
      m_inefficacyAlarm(_inefficacyAlarm),
      m_toxicityAlarm(_toxicityAlarm),
      m_unit(_unit)
{
}

}
}
