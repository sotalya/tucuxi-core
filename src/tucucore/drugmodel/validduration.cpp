#include <chrono>

#include "validduration.h"

#include "tucucore/drugdefinitions.h"

namespace Tucuxi {
namespace Core {

using Tucuxi::Common::Duration;

using namespace std::chrono_literals;

ValidDurations::ValidDurations(TucuUnit _unit, std::unique_ptr<PopulationValue> _defaultValue)
    : ValidValues(_unit, std::move(_defaultValue))
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
    if (m_unit == TucuUnit("d")) {
        return Duration(24h) * _value;
    }
    else if (m_unit == TucuUnit("h")) {
        return Duration(1h) * _value;
    }
    else if (m_unit == TucuUnit("min")) {
        return Duration(1min) * _value;
    }
    else if (m_unit == TucuUnit("s")) {
        return Duration(1s) * _value;
    }
    throw std::runtime_error("Internal error. valueToDuration() with invalid unit");
}


} // namespace Core
} // namespace Tucuxi
