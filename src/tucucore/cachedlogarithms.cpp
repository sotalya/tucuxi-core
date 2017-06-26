
#include <boost/functional/hash.hpp>

#include "tucucore/cachedlogarithms.h"

namespace Tucuxi {
namespace Math {

CachedLogarithms::CachedLogarithms()
{
}

bool CachedLogarithms::get(Float _cycleDuration, const ParameterList& _parameters, CycleSize _nbPoints, PrecomputedLogarithms& _logarithms)
{
	std::size_t h = hash(_cycleDuration, _parameters, _nbPoints);
	if (m_cache.end() != m_cache.find(h)) {
		_logarithms = m_cache.at(h);
		return true;
	}
	return false;
}

void CachedLogarithms::set(Float _cycleDuration, const ParameterList& _parameters, CycleSize _nbPoints, const PrecomputedLogarithms& _logarithms)
{
	m_cache[hash(_cycleDuration, _parameters, _nbPoints)] = _logarithms;
}

std::size_t CachedLogarithms::hash(Float _cycleDuration, const ParameterList& _parameters, CycleSize _nbPoints) const
{
	std::size_t seed = 0;
	boost::hash<double> hasher;
	boost::hash_combine(seed, hasher(_cycleDuration));
	boost::hash_combine(seed, hasher(_nbPoints));
	for (ParameterList::const_iterator it = _parameters.begin(); it != _parameters.end(); ++it) {
		boost::hash_combine(seed, hasher(it->getValue()));
	}
	return seed;
}

}
}
