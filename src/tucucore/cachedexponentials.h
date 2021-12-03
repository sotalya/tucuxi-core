/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_CACHEDEXPONENTIALS_H
#define TUCUXI_CORE_CACHEDEXPONENTIALS_H

#include "tucucore/definitions.h"
#include "tucucore/parameter.h"

namespace Tucuxi {
namespace Core {

/// \ingroup TucuCore
/// \brief This class manages a cache of precomputated exponentials
/// Equations for the different algorithms uses series of precomputed exponentials. These series of exponentials depends
/// on the following three factors:
///     - the duration of a cycle, 
///     - the number of points,
///     - parameters computed from the intake, patient data as well as measured drug concentration in the blood.
/// The idea of CachedExponentials is to try to avoid computing exponentials when possible. I.e. as long as the three factors
/// listed above stay the same...
/// The cache works with a hash values computed from these three factors.
class CachedExponentials
{
public:
    /// \brief Constructor
    CachedExponentials();
    
    /// \brief Get cached exponentials corresponding to the specified factors
    /// \param _cycleDuration Duration of a cycle
    /// \param _parameters The list of parameters computed from the intake, patient's data and measured drug concentrations
    /// \param _nbPoints The number of points to compute within the cycle
    /// \param _exponentials Cached exponentials if found
    /// \return Returns true if something has been found in the cache
    bool get(const Tucuxi::Common::Duration& _cycleDuration, const ParameterSetEvent& _parameters, CycleSize _nbPoints, PrecomputedExponentials& _exponentials);
    
    /// \brief Store exponentials into the cache for the specified factors
    /// \param _cycleDuration Duration of a cycle
    /// \param _parameters The list of parameters computed from the intake, patient's data and measured drug concentrations
    /// \param _nbPoints The number of points to compute within the cycle
    /// \param _exponentials exponentials to be cached
    void set(const Tucuxi::Common::Duration& _cycleDuration, const ParameterSetEvent& _parameters, CycleSize _nbPoints, const PrecomputedExponentials& _exponentials);
    
private:
    /// \brief Compute a hash based on the specified factors
    /// \param _cycleDuration Duration of a cycle
    /// \param _parameters The list of parameters computed from the intake, patient's data and measured drug concentrations
    /// \param _nbPoints The number of points to compute within the cycle
    /// \return Returns the computed hash
    std::size_t hash(const Tucuxi::Common::Duration& _cycleDuration, const ParameterSetEvent& _parameters, CycleSize _nbPoints) const;

    std::map<std::size_t, PrecomputedExponentials> m_cache; /// The actual cache of exponentials
};


} // namespace Core
} // namespace Tucuxi

#endif // #ifndef TUCUXI_CORE_CACHEDEXPONENTIALS_H
