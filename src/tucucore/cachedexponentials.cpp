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


#include <boost/functional/hash.hpp>

#include "tucucore/cachedexponentials.h"

#include "tucucommon/duration.h"

namespace Tucuxi {
namespace Core {

CachedExponentials::CachedExponentials() = default;


bool CachedExponentials::get(
        const Tucuxi::Common::Duration& _cycleDuration,
        const ParameterSetEvent& _parameters,
        CycleSize _nbPoints,
        PrecomputedExponentials& _exponentials)
{
    std::size_t h = hash(_cycleDuration, _parameters, _nbPoints);
    if (m_cache.end() != m_cache.find(h)) {
        _exponentials = m_cache[h];
        return true;
    }
    return false;
}


void CachedExponentials::set(
        const Tucuxi::Common::Duration& _cycleDuration,
        const ParameterSetEvent& _parameters,
        CycleSize _nbPoints,
        const PrecomputedExponentials& _exponentials)
{
    m_cache[hash(_cycleDuration, _parameters, _nbPoints)] = _exponentials;
}


std::size_t CachedExponentials::hash(
        const Tucuxi::Common::Duration& _cycleDuration, const ParameterSetEvent& _parameters, CycleSize _nbPoints) const
{
    std::size_t seed = 0;
    boost::hash<double> hasher;

    boost::hash_combine(seed, hasher(static_cast<double>(_cycleDuration.toMilliseconds())));
    boost::hash_combine(seed, hasher(static_cast<double>(_nbPoints)));
    for (const auto& parameter : _parameters) {
        boost::hash_combine(seed, hasher(parameter.getValue()));
    }
    return seed;
}

} // namespace Core
} // namespace Tucuxi
