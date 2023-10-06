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


#ifndef TUCUXI_CORE_CYCLESTATISTICSCALCULATOR_H
#define TUCUXI_CORE_CYCLESTATISTICSCALCULATOR_H

#include "tucucore/computingservice/computingresponse.h"
#include "tucucore/definitions.h"

namespace Tucuxi {
namespace Core {


///
/// \brief The CycleStatistics class
/// Contains all statistics for a cycle
///
class CycleStatistics
{
public:
    ///
    /// \brief Constructor. Computes statistics about the given cycle
    /// \param _data Cycle Data
    /// \param _cumulativeAuc Cumulative Auc of the previous cycle, modified in the computation
    /// When returning, _cumulativeAuc corresponds to its previous value, with the addition
    /// of the current cycle Auc.
    ///
    CycleStatistics(const CycleData& _data, std::vector<Value>& _cumulativeAuc);

    ///
    /// \brief Get the statistic for the specified compartment and type
    /// \param _compartment 0: first, 1: second or 2: third compartment
    /// \param _type mean, peak, maximum, minimum or AUC
    /// \return The list of statistics
    ///
    CycleStatistic getStatistic(size_t _compartment, CycleStatisticType _type)
    {
        return m_stats[_compartment][static_cast<size_t>(_type)];
    }

    ///
    /// \brief Get the statistics of all compartments for the specified type
    /// \param _type mean, peak, maximum, minimum or AUC
    /// \return The list of statistics
    ///
    bool getStatistics(CycleStatisticType _type, std::vector<std::vector<CycleStatistic> >& _stats)
    {
        if (_type >= CycleStatisticType::CYCLE_STATISTIC_TYPE_SIZE) {
            return false;
        }

        for (unsigned int compartment = 0; compartment < m_stats.size(); compartment++) {
            _stats[compartment][static_cast<size_t>(_type)] = m_stats[compartment][static_cast<size_t>(_type)];
        }

        return true;
    }

private:
    // The list of statistics for each compartments (e.g m_stats[0][]: 1st compartment, m_stats[1][]: 2nd compartment etc)
    std::vector<std::vector<CycleStatistic> > m_stats;
    void calculate(
            const MultiCompConcentrations& _concentrations,
            const std::vector<TimeOffsets>& _times,
            std::vector<Value>& _cumulativeAuc);
};


class CycleStatisticsCalculator
{
public:
    CycleStatisticsCalculator() {}

    void calculate(std::vector<CycleData>& _cycles);
};

} // namespace Core
} // namespace Tucuxi
#endif // TUCUXI_CORE_CYCLESTATISTICSCALCULATOR_H
