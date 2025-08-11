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


#ifndef TUCUXI_CORE_CYCLESTATISTICS_H
#define TUCUXI_CORE_CYCLESTATISTICS_H

#include "tucucommon/datetime.h"
#include "tucucommon/duration.h"

#include "tucucore/definitions.h"

namespace Tucuxi {
namespace Core {

enum class CycleStatisticType : int
{
    Mean = 0,
    Peak,
    Maximum,
    Minimum,
    AUC,
    AUC24,
    CumulativeAuc,
    Residual,
    CycleInterval,
    CYCLE_STATISTIC_TYPE_SIZE
};

///
/// \brief The CycleStatistic class
/// A single statistic with a type (mean, peak, maximum, minimum and AUC),
/// a time (offset from start of cycle), a value, a unit.
/// The time is not relevant in the case of Mean and AUC.
///
class CycleStatistic
{
public:
    ///
    /// \brief Build a new statistic with the given type
    /// \param _cycleStartDate Absolute cycle start date
    /// \param _type
    ///
    CycleStatistic(Tucuxi::Common::DateTime _cycleStartDate, CycleStatisticType _type);

    ///
    /// \brief Add a new value to the statistic
    /// \param _time offset from the start of the corresponding cycle (m_cycleStartDate)
    /// \param _value new value (mean, peak, maximum, minimum or AUC)
    ///
    void addValue(const Tucuxi::Common::Duration _time, const Value _value)
    {
        Data newData;
        newData.m_offset = _time;
        newData.m_value = _value;
        m_data.push_back(newData);
    }

    ///
    /// \brief Return the number of values
    /// \return number of values
    ///
    size_t getNbValue() const
    {
        return m_data.size();
    }

    ///
    /// \brief Return cycle start date
    /// \return cycle start date
    ///
    Tucuxi::Common::DateTime getCycleStartDate() const
    {
        return m_cycleStartDate;
    }

    ///
    /// \brief Find one of the values with index and return the value and its dateTime
    /// \param _dateTime Absolute time
    /// \param _value stored value
    /// \param _iindex
    /// \return True if there is a value at given index. otherwise False
    ///
    bool getValue(Tucuxi::Common::DateTime& _dateTime, Value& _value, size_t _index = 0) const
    {
        // if the required index is bigger than the stored value...
        if (_index >= getNbValue()) {
            return false;
        }
        _dateTime = m_cycleStartDate + m_data[_index].m_offset;
        _value = m_data[_index].m_value;
        return true;
    }

private:
    Tucuxi::Common::DateTime m_cycleStartDate; // Date and time of the start of the corresponding cycle
    // CycleStatisticType m_type; // The type of statistic

    struct Data
    {
        Tucuxi::Common::Duration m_offset;
        Value m_value{0.0};
    };

    std::vector<Data> m_data; // The list of values (in case of maximum and minimum, we can have a list)
};

class CycleStats
{
public:
    ///
    /// \brief Get the statistic for the specified compartment and type
    /// \param _compartment 0: first, 1: second or 2: third compartment
    /// \param _type mean, peak, maximum, minimum or AUC
    /// \return The list of statistics
    ///
    CycleStatistic getStatistic(size_t _compartment, CycleStatisticType _type) const
    {
        return m_stats[_compartment][static_cast<size_t>(_type)];
    }

    void setStatistics(size_t _compartment, CycleStatisticType _type, CycleStatistic _statistic)
    {
        while (m_stats.size() <= _compartment) {
            m_stats.push_back(std::vector<CycleStatistic>(
                    static_cast<int>(CycleStatisticType::CYCLE_STATISTIC_TYPE_SIZE),
                    CycleStatistic(Tucuxi::Common::DateTime::now(), CycleStatisticType::CYCLE_STATISTIC_TYPE_SIZE)));
        }
        m_stats[_compartment][static_cast<size_t>(_type)] = std::move(_statistic);
    }

    std::vector<std::vector<CycleStatistic> > getStats() const
    {
        return m_stats;
    }

protected:
    // The list of statistics for each compartments (e.g m_stats[0][]: 1st compartment, m_stats[1][]: 2nd compartment etc)
    std::vector<std::vector<CycleStatistic> > m_stats;
};


} // namespace Core
} // namespace Tucuxi


#endif // TUCUXI_CORE_CYCLESTATISTICS_H
