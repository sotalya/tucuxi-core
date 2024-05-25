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


#include <utility>

#include "targetevent.h"

namespace Tucuxi {
namespace Core {

TargetEvent::TargetEvent(
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
        Tucuxi::Common::Duration _tmax)
    : m_activeMoietyId(std::move(_activeMoietyId)), m_targetType(_type), m_valueMin(_vmin), m_valueMax(_vmax),
      m_valueBest(_vbest), m_mic(_mic), m_micUnit(std::move(_micUnit)), m_tMin(_tmin), m_tMax(_tmax), m_tBest(_tbest),
      m_unit(std::move(_unit)), m_finalUnit(std::move(_finalUnit)){};

TargetEvent TargetEvent::createTargetEventWithTime(
        ActiveMoietyId _activeMoietyId,
        TargetType _type,
        TucuUnit _unit,
        TucuUnit _finalUnit,
        Value _vmin,
        Value _vbest,
        Value _vmax,
        const Tucuxi::Common::Duration& _tmin,
        const Tucuxi::Common::Duration& _tbest,
        const Tucuxi::Common::Duration& _tmax)
{
    return TargetEvent(
            std::move(_activeMoietyId),
            _type,
            std::move(_unit),
            std::move(_finalUnit),
            _vmin,
            _vbest,
            _vmax,
            0.0,
            TucuUnit(""),
            _tmin,
            _tbest,
            _tmax);
}

TargetEvent TargetEvent::createTargetEventWithMic(
        ActiveMoietyId _activeMoietyId,
        TargetType _type,
        TucuUnit _unit,
        TucuUnit _finalUnit,
        Value _vmin,
        Value _vbest,
        Value _vmax,
        Value _mic,
        TucuUnit _micUnit)
{
    return TargetEvent(
            std::move(_activeMoietyId),
            _type,
            std::move(_unit),
            std::move(_finalUnit),
            _vmin,
            _vbest,
            _vmax,
            _mic,
            std::move(_micUnit),
            Tucuxi::Common::Duration(),
            Tucuxi::Common::Duration(),
            Tucuxi::Common::Duration());
}

TargetEvent TargetEvent::createTargetEventWithoutTimeAndMic(
        ActiveMoietyId _activeMoietyId,
        TargetType _type,
        TucuUnit _unit,
        TucuUnit _finalUnit,
        Value _vmin,
        Value _vbest,
        Value _vmax)
{
    return TargetEvent(
            std::move(_activeMoietyId),
            _type,
            std::move(_unit),
            std::move(_finalUnit),
            _vmin,
            _vbest,
            _vmax,
            0.0,
            TucuUnit(""),
            Tucuxi::Common::Duration(),
            Tucuxi::Common::Duration(),
            Tucuxi::Common::Duration());
}

TargetEvent TargetEvent::createTargetEventWithMicAndTime(
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
        const Tucuxi::Common::Duration& _tmax)
{
    return TargetEvent(
            std::move(_activeMoietyId),
            _type,
            std::move(_unit),
            std::move(_finalUnit),
            _vmin,
            _vbest,
            _vmax,
            _mic,
            std::move(_micUnit),
            _tmin,
            _tbest,
            _tmax);
}




} // namespace Core
} // namespace Tucuxi
