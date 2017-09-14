#include "validduration.h"


namespace Tucuxi {
namespace Core {


void ValidDurations::setDefaultDuration(Tucuxi::Common::Duration _duration)
{
    m_defaultDuration = _duration;
}

Tucuxi::Common::Duration ValidDurations::getDefaultDuration() const
{
    return m_defaultDuration;
}


AnyDurations::AnyDurations(
        Tucuxi::Common::Duration _from,
        Tucuxi::Common::Duration _to,
        Tucuxi::Common::Duration _step) :
    m_from(_from), m_to(_to), m_step(_step)
{}

std::vector<Tucuxi::Common::Duration> AnyDurations::getDurations() const
{
    Tucuxi::Common::Duration currentDuration = m_from;
    std::vector<Tucuxi::Common::Duration> result;

    while (currentDuration <= m_to) {
        result.push_back(currentDuration);
        currentDuration += m_step;
    }
    return result;
}


std::vector<Tucuxi::Common::Duration> SpecificDurations::getDurations() const
{
    return m_durations;
}

void SpecificDurations::addDuration(Common::Duration _duration)
{
    m_durations.push_back(_duration);
}



}
}
