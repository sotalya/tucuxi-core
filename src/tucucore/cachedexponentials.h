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
    bool get(
            const Tucuxi::Common::Duration& _cycleDuration,
            const ParameterSetEvent& _parameters,
            CycleSize _nbPoints,
            PrecomputedExponentials& _exponentials);

    /// \brief Store exponentials into the cache for the specified factors
    /// \param _cycleDuration Duration of a cycle
    /// \param _parameters The list of parameters computed from the intake, patient's data and measured drug concentrations
    /// \param _nbPoints The number of points to compute within the cycle
    /// \param _exponentials exponentials to be cached
    void set(
            const Tucuxi::Common::Duration& _cycleDuration,
            const ParameterSetEvent& _parameters,
            CycleSize _nbPoints,
            const PrecomputedExponentials& _exponentials);

private:
    /// \brief Compute a hash based on the specified factors
    /// \param _cycleDuration Duration of a cycle
    /// \param _parameters The list of parameters computed from the intake, patient's data and measured drug concentrations
    /// \param _nbPoints The number of points to compute within the cycle
    /// \return Returns the computed hash
    std::size_t hash(
            const Tucuxi::Common::Duration& _cycleDuration,
            const ParameterSetEvent& _parameters,
            CycleSize _nbPoints) const;

    std::map<std::size_t, PrecomputedExponentials> m_cache; /// The actual cache of exponentials
};


} // namespace Core
} // namespace Tucuxi

#endif // #ifndef TUCUXI_CORE_CACHEDEXPONENTIALS_H
