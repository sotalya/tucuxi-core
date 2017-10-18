/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_CYCLESTATISTICSCALCULATOR_H
#define TUCUXI_CORE_CYCLESTATISTICSCALCULATOR_H

#include "tucucore/definitions.h"
#include "tucucore/processingservice/processingresponse.h"

namespace Tucuxi {
namespace Core {

enum class CycleStatisticType : int { Mean, Peak, Maximum, Minimum, AUC };

///
/// \brief The CycleStatistic class
/// A siglle statistic with a type (mean, peak, maximum, minimum and AUC),
/// a time (offset from start of cycle), a value, a unit.
/// The time is not relaevant in the case of MEan and AUC.
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
    int getNbValue() const { return m_data.size(); }

    ///
    /// \brief Return cycle start date
    /// \return cycle start date
    ///
    DateTime getCycleStartDate() const { return m_cycleStartDate; }

    ///
    /// \brief Find one of the values with index and return the value and its dateTime
    /// \param _dateTime Absolute time
    /// \param _value stored value
    /// \param index
    /// \return True if there is a value at given index. otherwise False
    ///
    bool getValue(DateTime &_dateTime, Value &_value, int index = 0) const {
	// if the required index is bigger than the stored value...
	if (index >= getNbValue())
	    return false;

	_dateTime = m_cycleStartDate + m_data[index].offset;
	_value = m_data[index].value;

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
    ///
    CycleStatistics(const CycleData &_data);

    ///
    /// \brief Get the statistic for the specified type
    /// \param _type mean, peak, maximum, minimum or AUC
    /// \return The list of statistics
    ///
    CycleStatistic getStatistic(CycleStatisticType _type) { return m_stats[0][static_cast<int>(_type)]; }

    void calculateMean(const std::vector<Concentrations> &_concentrations);
    void calculateAUC(const std::vector<Concentrations> &_concentrations, const std::vector<TimeOffsets> &_times);
    void searchPeak(const std::vector<Concentrations> &_concentrations, const std::vector<TimeOffsets> &_times);
    void searchMaximumsMinimums(const std::vector<Concentrations> &_concentrations, const std::vector<TimeOffsets> &_times);

private:
    // TODO: currently it's only for first compartment, but need to change for all compartment
    // The list of statistics for each compartments (e.g m_stats[0][]: 1st compartment, m_stats[1][]: 2nd compartment etc)
    std::vector< std::vector<CycleStatistic> > m_stats;
};

}
}
#endif // TUCUXI_CORE_CYCLESTATISTICSCALCULATOR_H