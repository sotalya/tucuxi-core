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


#ifndef TUCUXI_CORE_TIMEDEVENT_H
#define TUCUXI_CORE_TIMEDEVENT_H

#include "tucucommon/datetime.h"

using Tucuxi::Common::DateTime; // NOLINT(google-global-names-in-headers)

namespace Tucuxi {
namespace Core {

/// \ingroup TucuCore
/// \brief Base class for all types of "events"
/// An event is a change in a value that influences the computation of prediction. It could be
/// a new intake, a new measured concentration, a change of a covariate, ...
class TimedEvent
{
public:
    /// \brief Constructor, "deleted" because not necessary.
    TimedEvent() = delete;

    /// \brief Constructor defining the time of the event.
    /// \param _time Time of the event to set.
    /// \pre _time.isValid() == true
    /// \post m_time == _time
    TimedEvent(DateTime _time) : m_time(_time)
    {
        //assert (!_time.isUndefined());
    }

    /// \brief Get the time the event happened.
    /// \return Time of the event.
    /// \invariant UNALTERED(m_time)
    DateTime getEventTime() const
    {
        return m_time;
    }

protected:
    DateTime m_time; /// The time of the event
};

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_TIMEDEVENT_H
