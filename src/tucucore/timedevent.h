/*
* Copyright (C) 2017 Tucuxi SA
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
    DateTime m_time;    /// The time of the event
};

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_TIMEDEVENT_H
