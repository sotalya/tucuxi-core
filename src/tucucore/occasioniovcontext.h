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


#ifndef TUCUXI_CORE_OCCASIONIOVCONTEXT_H
#define TUCUXI_CORE_OCCASIONIOVCONTEXT_H

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>

#include "tucucommon/datetime.h"

namespace Tucuxi {
namespace Core {

///
/// \brief One inter-occasion variability (IOV) occasion.
///
/// An occasion is a dosing interval that carries at least one sample. Intervals
/// without a sample are not occasions and get no IOV eta.
///
struct IovOccasion
{
    /// \brief Start time of the occasion (the dose intake time).
    Tucuxi::Common::DateTime m_start;

    /// \brief End time of the occasion (start plus the dosing interval).
    Tucuxi::Common::DateTime m_end;
};

///
/// \brief Read-only description of the IOV eta layout for one a-posteriori fit.
///
/// The flat eta vector handed to the minimizer is laid out as the BSV etas
/// first, then one IOV block per occasion. Each block holds one IOV eta per
/// IOV parameter. This object lets the parameter machinery find, for a given
/// time, which occasion block to add on top of the BSV etas. It is attached to
/// a ParameterSetSeries only on the IOV fitting path, so the no-IOV path never
/// sees it and pays nothing.
///
class OccasionIovContext
{
public:
    ///
    /// \brief Constructor.
    /// \param[in] _bsvCount : Number of BSV etas (the head of the eta vector).
    /// \param[in] _iovCountPerOccasion : Number of IOV etas in one occasion
    ///            block.
    /// \param[in] _occasions : Occasions carrying a sample, in time order.
    ///
    OccasionIovContext(std::size_t _bsvCount, std::size_t _iovCountPerOccasion, std::vector<IovOccasion> _occasions)
        : m_bsvCount(_bsvCount), m_iovCountPerOccasion(_iovCountPerOccasion), m_occasions(std::move(_occasions))
    {
        // Each IOV parameter has a BSV (IOV implies BSV) and adds one eta, so
        // the IOV etas per occasion never outnumber the BSV etas.
        assert(_iovCountPerOccasion <= _bsvCount);
    }

    ///
    /// \brief Get the number of BSV etas.
    /// \return Number of BSV etas.
    ///
    std::size_t getBsvCount() const
    {
        return m_bsvCount;
    }

    ///
    /// \brief Get the number of IOV etas per occasion.
    /// \return Number of IOV etas per occasion.
    ///
    std::size_t getIovCountPerOccasion() const
    {
        return m_iovCountPerOccasion;
    }

    ///
    /// \brief Get the number of occasions carrying a sample.
    /// \return Number of occasions.
    ///
    std::size_t getOccasionCount() const
    {
        return m_occasions.size();
    }

    ///
    /// \brief Find the occasion block index for a given time.
    /// \param[in] _time : Time to look up.
    /// \return Occasion index in [0, getOccasionCount()), or -1 when the time
    ///         lies outside every occasion, so no IOV applies.
    ///
    int32_t occasionIndexAtTime(Tucuxi::Common::DateTime const& _time) const
    {
        for (std::size_t i = 0; i < m_occasions.size(); ++i) {
            if ((_time >= m_occasions[i].m_start) && (_time <= m_occasions[i].m_end)) {
                return static_cast<int32_t>(i);
            }
        }
        return -1;
    }

    ///
    /// \brief Offset, in the flat eta vector, of an occasion's IOV block.
    /// \param[in] _occasionIndex : Occasion index.
    /// \return Index of the first IOV eta of that occasion.
    ///
    std::size_t occasionBlockOffset(std::size_t _occasionIndex) const
    {
        assert(_occasionIndex < m_occasions.size());
        return m_bsvCount + (_occasionIndex * m_iovCountPerOccasion);
    }

    ///
    /// \brief Total number of etas (BSV head plus all occasion blocks).
    /// \return Total eta count (equals the Omega matrix size).
    ///
    std::size_t getTotalEtaCount() const
    {
        return m_bsvCount + (m_occasions.size() * m_iovCountPerOccasion);
    }

private:
    /// \brief Number of BSV etas (the head of the flat eta vector).
    std::size_t m_bsvCount;

    /// \brief Number of IOV etas in one occasion block.
    std::size_t m_iovCountPerOccasion;

    /// \brief Occasions carrying a sample, in time order.
    std::vector<IovOccasion> m_occasions;
};

///
/// \brief Select the dosing intervals that carry a sample, in time order.
///
/// A sample is assigned to the first interval whose end is at or after the
/// sample time (matches how the concentration calculator assigns a sample to a
/// cycle --- a closed upper bound).
///
/// \param[in] _intervals : All dosing intervals, in time order.
/// \param[in] _sampleTimes : Sample times.
/// \return Intervals owning at least one sample, in time order.
///
inline std::vector<IovOccasion> computeSampledOccasions(
        std::vector<IovOccasion> const& _intervals, std::vector<Tucuxi::Common::DateTime> const& _sampleTimes)
{
    std::vector<IovOccasion> occasions;
    for (std::size_t i = 0; i < _intervals.size(); ++i) {
        bool hasSample = false;
        for (auto const& sampleTime : _sampleTimes) {
            bool const afterPreviousEnd = (i == 0) || (sampleTime > _intervals[i - 1].m_end);
            if (afterPreviousEnd && (sampleTime <= _intervals[i].m_end)) {
                hasSample = true;
                break;
            }
        }
        if (hasSample) {
            occasions.push_back(_intervals[i]);
        }
    }
    return occasions;
}

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_OCCASIONIOVCONTEXT_H
