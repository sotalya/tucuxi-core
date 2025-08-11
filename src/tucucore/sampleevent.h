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


#ifndef TUCUXI_CORE_SAMPLEEVENT_H
#define TUCUXI_CORE_SAMPLEEVENT_H

#include <vector>

#include "tucucore/definitions.h"
#include "tucucore/timedevent.h"

namespace Tucuxi {
namespace Core {


class SampleEvent : public TimedEvent
{
public:
    /// \brief Constructor defining the time of the sample event and its value.
    /// \param _time Time of the event to set.
    /// \param _value Value of the sample.
    /// \pre _time.isValid() == true
    /// \post m_time == _time
    /// \post m_value == _value
    SampleEvent(DateTime _time, Value _value = 0, Value _weight = 1.0)
        : TimedEvent(_time), m_value(_value), m_weight(_weight)
    {
    }

    Value getValue() const
    {
        return m_value;
    }

    Value getWeight() const
    {
        return m_weight;
    }

protected:
    Value m_value;
    Value m_weight;
};

typedef std::vector<SampleEvent> SampleSeries;

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_SAMPLEEVENT_H
