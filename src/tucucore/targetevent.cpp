/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "targetevent.h"

namespace Tucuxi {
namespace Core {

TargetEvent::TargetEvent(ActiveMoietyId _activeMoietyId,
                         TargetType _type,
                         TucuUnit _unit,
                         TucuUnit _finalUnit,
                         Value _vmin,
                         Value _vbest,
                         Value _vmax,
                         Value _mic,
                         TucuUnit _micUnit,
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
          m_unit(_unit),
          m_finalUnit(_finalUnit)
{};

TargetEvent TargetEvent::createTargetEventWithTime(ActiveMoietyId _activeMoietyId,
                                           TargetType _type,
                                           TucuUnit _unit,
                                           TucuUnit _finalUnit,
                                           Value _vmin,
                                           Value _vbest,
                                           Value _vmax,
                                           const Tucuxi::Common::Duration &_tmin,
                                           const Tucuxi::Common::Duration &_tbest,
                                           const Tucuxi::Common::Duration &_tmax)
{
    return TargetEvent(_activeMoietyId,
                       _type,
                       _unit,
                       _finalUnit,
                       _vmin,
                       _vbest,
                       _vmax,
                       0.0,
                       TucuUnit(""),
                       _tmin,
                       _tbest,
                       _tmax);
}

TargetEvent TargetEvent::createTargetEventWithMic(ActiveMoietyId _activeMoietyId,
                                           TargetType _type,
                                           TucuUnit _unit,
                                           TucuUnit _finalUnit,
                                           Value _vmin,
                                           Value _vbest,
                                           Value _vmax,
                                           Value _mic,
                                           TucuUnit _micUnit)
{
    return TargetEvent(_activeMoietyId,
                       _type,
                       _unit,
                       _finalUnit,
                       _vmin,
                       _vbest,
                       _vmax,
                       _mic,
                       _micUnit,
                       Tucuxi::Common::Duration(),
                       Tucuxi::Common::Duration(),
                       Tucuxi::Common::Duration());
}

TargetEvent TargetEvent::createTargetEventWithoutTimeAndMic(ActiveMoietyId _activeMoietyId,
                                           TargetType _type,
                                           TucuUnit _unit,
                                           TucuUnit _finalUnit,
                                           Value _vmin,
                                           Value _vbest,
                                           Value _vmax)
{
    return TargetEvent(_activeMoietyId,
                       _type,
                       _unit,
                       _finalUnit,
                       _vmin,
                       _vbest,
                       _vmax,
                       0.0,
                       TucuUnit(""),
                       Tucuxi::Common::Duration(),
                       Tucuxi::Common::Duration(),
                       Tucuxi::Common::Duration());
}

TargetEvent TargetEvent::createTargetEventWithMicAndTime(ActiveMoietyId _activeMoietyId,
                                         TargetType _type,
                                         TucuUnit _unit,
                                         TucuUnit _finalUnit,
                                         Value _vmin,
                                         Value _vbest,
                                         Value _vmax,
                                         Value _mic,
                                         TucuUnit _micUnit,
                                         const Tucuxi::Common::Duration &_tmin,
                                         const Tucuxi::Common::Duration &_tbest,
                                         const Tucuxi::Common::Duration &_tmax)
{
    return TargetEvent(_activeMoietyId,
                       _type,
                       _unit,
                       _finalUnit,
                       _vmin,
                       _vbest,
                       _vmax,
                       _mic,
                       _micUnit,
                       _tmin,
                       _tbest,
                       _tmax);
}




} // namespace Core
} // namespace Tucuxi
