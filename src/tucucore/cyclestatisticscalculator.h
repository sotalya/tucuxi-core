/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_CYCLESTATISTICSCALCULATOR_H
#define TUCUXI_CORE_CYCLESTATISTICSCALCULATOR_H

#include "tucucore/definitions.h"
#include "tucucore/computingservice/computingresponse.h"

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
    CycleStatistics(const CycleData &_data, Value &_cumulativeAuc);

    ///
    /// \brief Get the statistic for the specified compartment and type
    /// \param _compartment 0: first, 1: second or 2: third compartment
    /// \param _type mean, peak, maximum, minimum or AUC
    /// \return The list of statistics
    ///
    CycleStatistic getStatistic(int _compartment, CycleStatisticType _type) { return m_stats[_compartment][static_cast<int>(_type)]; }

    ///
    /// \brief Get the statistics of all compartments for the specified type
    /// \param _type mean, peak, maximum, minimum or AUC
    /// \return The list of statistics
    ///
    bool getStatistics(CycleStatisticType _type, std::vector< std::vector<CycleStatistic> > &_stats) 
    { 
        if (_type >= CycleStatisticType::CYCLE_STATISTIC_TYPE_SIZE) {
            return false;
        }

        for (unsigned int compartment = 0; compartment < m_stats.size(); compartment++) {
            _stats[compartment][static_cast<int>(_type)] = m_stats[compartment][static_cast<int>(_type)];
        }

        return true;
    }

private:
    // The list of statistics for each compartments (e.g m_stats[0][]: 1st compartment, m_stats[1][]: 2nd compartment etc)
    std::vector< std::vector<CycleStatistic> > m_stats;
    void calculate(const std::vector<Concentrations> &_concentrations,
                   const std::vector<TimeOffsets> &_times,
                   Value &_cumulativeAuc);
};


class CycleStatisticsCalculator
{
public:

    CycleStatisticsCalculator() {}

    void calculate(std::vector<CycleData> & _cycles);
};

} // namespace Core
} // namespace Tucuxi
#endif // TUCUXI_CORE_CYCLESTATISTICSCALCULATOR_H
