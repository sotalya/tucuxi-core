/*
* Copyright (C) 2017 Tucuxi SA
*/

#include <boost/functional/hash.hpp>

#include "tucucore/cachedexponentials.h"
#include "tucucommon/duration.h"

namespace Tucuxi {
namespace Core {

CachedExponentials::CachedExponentials()
= default;


bool CachedExponentials::get(const Tucuxi::Common::Duration& _cycleDuration, const ParameterSetEvent& _parameters, CycleSize _nbPoints, PrecomputedExponentials& _exponentials)
{
    std::size_t h = hash(_cycleDuration, _parameters, _nbPoints);
    if (m_cache.end() != m_cache.find(h)) {
        _exponentials = m_cache[h];
        return true;
    }
    return false;
}


void CachedExponentials::set(const Tucuxi::Common::Duration& _cycleDuration, const ParameterSetEvent& _parameters, CycleSize _nbPoints, const PrecomputedExponentials& _exponentials)
{
    m_cache[hash(_cycleDuration, _parameters, _nbPoints)] = _exponentials;
}


std::size_t CachedExponentials::hash(const Tucuxi::Common::Duration& _cycleDuration, const ParameterSetEvent& _parameters, CycleSize _nbPoints) const
{
    std::size_t seed = 0;
    boost::hash<double> hasher;

    boost::hash_combine(seed, hasher(static_cast<int>(_cycleDuration.toMilliseconds())));
    boost::hash_combine(seed, hasher(static_cast<double>(_nbPoints)));
    for (const auto & parameter : _parameters) {
        boost::hash_combine(seed, hasher(parameter.getValue()));
    }
    return seed;
}

} // namespace Core
} // namespace Tucuxi
