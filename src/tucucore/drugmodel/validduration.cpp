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


#include <chrono>
#include <utility>

#include "validduration.h"

#include "tucucore/drugdefinitions.h"

namespace Tucuxi {
namespace Core {

using Tucuxi::Common::Duration;

using namespace std::chrono_literals;

ValidDurations::ValidDurations(TucuUnit _unit, std::unique_ptr<PopulationValue> _defaultValue)
    : ValidValues(std::move(_unit), std::move(_defaultValue))
{
}


ValidDurations::~ValidDurations() = default;

/*
void ValidDurations::setDefaultDuration(Tucuxi::Common::Duration _duration)
{
    m_defaultDuration = _duration;
}
*/

Tucuxi::Common::Duration ValidDurations::getDefaultDuration() const
{
    // TODO : Manage covariates
    return valueToDuration(m_defaultValue->getValue());
}


std::vector<Tucuxi::Common::Duration> ValidDurations::getDurations() const
{
    std::vector<Value> values = getValues();
    std::vector<Tucuxi::Common::Duration> durations(values.size());
    for (size_t i = 0; i < values.size(); i++) {
        durations[i] = valueToDuration(values[i]);
    }
    return durations;
}

Tucuxi::Common::Duration ValidDurations::valueToDuration(Value _value) const
{
    if (m_unit == TucuUnit("y")) {
        return Duration(365 * 7 * 24h) * _value;
    }
    if (m_unit == TucuUnit("w")) {
        return Duration(7 * 24h) * _value;
    }
    if (m_unit == TucuUnit("d")) {
        return Duration(24h) * _value;
    }
    if (m_unit == TucuUnit("h")) {
        return Duration(1h) * _value;
    }
    if (m_unit == TucuUnit("min")) {
        return Duration(1min) * _value;
    }
    if (m_unit == TucuUnit("s")) {
        return Duration(1s) * _value;
    }
    throw std::runtime_error("Internal error. valueToDuration() with invalid unit");
}


} // namespace Core
} // namespace Tucuxi
