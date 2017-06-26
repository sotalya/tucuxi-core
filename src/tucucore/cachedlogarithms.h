#ifndef TUCUXI_MATH_CACHEDLOGARITHMS_H
#define TUCUXI_MATH_CACHEDLOGARITHMS_H

#include "tucucore/definitions.h"
#include "tucucore/parameter.h"

namespace Tucuxi {
namespace Math {


// Used as the values in the log hash tables 
// Enough information to avoid calculating
// the log values for calculation of cycle residuals
class CachedLogarithms
{
public:
	CachedLogarithms();
	
	bool get(Float cycleduration, const ParameterList &parameters, CycleSize nbPoints, PrecomputedLogarithms& _logarithms);
	void set(Float cycleduration, const ParameterList &parameters, CycleSize nbPoints, const PrecomputedLogarithms& _logarithms);

private:
	std::size_t hash(Float _cycleDuration, const ParameterList& _parameters, CycleSize _nbPoints) const;

private:
	std::map<std::size_t, PrecomputedLogarithms> m_cache;
};



}
}

#endif // #ifndef TUCUXI_MATH_CACHEDLOGARITHMS_H
