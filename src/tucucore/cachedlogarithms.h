/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_MATH_CACHEDLOGARITHMS_H
#define TUCUXI_MATH_CACHEDLOGARITHMS_H

#include "tucucore/definitions.h"
#include "tucucore/parameter.h"

namespace Tucuxi {
namespace Core {

/// \ingroup TucuCore
/// \brief This class manages a cache of precomputated logarithms
/// Equations for the different algorithms uses series of precomputed logarithms. These series of logarithms depends
/// on the following three factors:
///     - the duration of a cycle, 
///     - the number of points,
///     - parameters computed from the intake, patient data as well as measured drug concentration in the blood.
/// The idea of CachedLogarithms is to try to avoid computing logarithms when possible. I.e. as long as the three factors  
/// listed above stay the same...
/// The cache works with a hash values computed from these three factors.
class CachedLogarithms
{
public:
    /// \brief Constructor
    CachedLogarithms();
    
    /// \brief Get cached logarithms corresponding to the specified factors
    /// @param _cycleDuration Duration of a cycle
    /// @param _parameters The list of parameters computed from the intake, patient's data and measured drug concentrations
    /// @param _nbPoints The number of points to compute within the cycle
    /// @param _logarithms Cached logarithms if found
    /// @return Returns true if something has been found in the cache
    bool get(DeltaTime _cycleDuration, const ParameterList& _parameters, CycleSize _nbPoints, PrecomputedLogarithms& _logarithms);
    
    /// \brief Store logarithms into the cache for the specified factors
    /// @param _cycleDuration Duration of a cycle
    /// @param _parameters The list of parameters computed from the intake, patient's data and measured drug concentrations
    /// @param _nbPoints The number of points to compute within the cycle
    /// @param _logarithms Logarithms to be cached
    void set(DeltaTime _cycleDuration, const ParameterList& _parameters, CycleSize _nbPoints, const PrecomputedLogarithms& _logarithms);
    
private:
    /// \brief Compute a hash based on the specified factors
    /// @param _cycleDuration Duration of a cycle
    /// @param _parameters The list of parameters computed from the intake, patient's data and measured drug concentrations
    /// @param _nbPoints The number of points to compute within the cycle
    /// @return Returns the computed hash
    std::size_t hash(DeltaTime _cycleDuration, const ParameterList& _parameters, CycleSize _nbPoints) const;

private:
    std::map<std::size_t, PrecomputedLogarithms> m_cache; /// The actual cache of logarithms
};


}
}

#endif // #ifndef TUCUXI_MATH_CACHEDLOGARITHMS_H
