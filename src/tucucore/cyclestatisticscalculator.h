/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_CYCLESTATISTICSCALCULATOR_H
#define TUCUXI_CORE_CYCLESTATISTICSCALCULATOR_H

#include "tucucore/definitions.h"
#include "tucucore/computingservice/computingresponse.h"

namespace Tucuxi {
namespace Core {

enum class CycleStatisticType : int { Mean = 0, Peak, Maximum, Minimum, AUC, CumulativeAuc, Residual, CycleInterval, CYCLE_STATISTIC_TYPE_SIZE };

///
/// \brief The CycleStatistic class
/// A single statistic with a type (mean, peak, maximum, minimum and AUC),
/// a time (offset from start of cycle), a value, a unit.
/// The time is not relaevant in the case of Mean and AUC.
/// 
class CycleStatistic
{
public:

    ///
    /// \brief Build a new statistic with the given type
    /// \param _cycleStartDate Absolute cycle start date
    /// \param _type
    ///
    CycleStatistic(const DateTime &_cycleStartDate, CycleStatisticType _type);

    ///
    /// \brief Add a new value to the statistic
    /// \param _time offset from the start of the corresponding cycle (m_cycleStartDate)
    /// \param _value new value (mean, peak, maximum, minimum or AUC)
    ///
    void addValue(const Duration _time, const Value _value) { 
        Data newData;
        newData.offset = _time;
        newData.value = _value;
        m_data.push_back(newData);
    }

    ///
    /// \brief Return the number of values
    /// \return number of values
    ///
    size_t getNbValue() const { return m_data.size(); }

    ///
    /// \brief Return cycle start date
    /// \return cycle start date
    ///
    DateTime getCycleStartDate() const { return m_cycleStartDate; }

    ///
    /// \brief Find one of the values with index and return the value and its dateTime
    /// \param _dateTime Absolute time
    /// \param _value stored value
    /// \param _iindex
    /// \return True if there is a value at given index. otherwise False
    ///
    bool getValue(DateTime &_dateTime, Value &_value, size_t _index = 0) const
    {
        // if the required index is bigger than the stored value...
        if (_index >= getNbValue()) {
            return false;
        }
        _dateTime = m_cycleStartDate + m_data[_index].offset;
        _value = m_data[_index].value;
        return true;
    }

private:
    DateTime m_cycleStartDate; // Date and time of the start of the corresponding cycle
    CycleStatisticType m_type; // The type of statistic
    
    struct Data {
        Duration offset;
        Value value;
    };
    std::vector<Data> m_data; // The list of values (in case of maximum and minimum, we can have a list)
};

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


} // namespace Core
} // namespace Tucuxi
#endif // TUCUXI_CORE_CYCLESTATISTICSCALCULATOR_H
