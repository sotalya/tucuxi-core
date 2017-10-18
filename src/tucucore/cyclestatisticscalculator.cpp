/*
* Copyright (C) 2017 Tucuxi SA
*/

#include <vector>
#include <numeric>
#include <iterator>
#include "tucucommon/duration.h"
#include "tucucore/cyclestatisticscalculator.h"

namespace Tucuxi {
namespace Core {


CycleStatistic::CycleStatistic(const DateTime &_cycleStartDate, const CycleStatisticType _type)
    : m_cycleStartDate(_cycleStartDate),
      m_type(_type)
{
}


void CycleStatistics::calculateMean(const std::vector<Concentrations> &_concentrations)
{
    // Outer vetor size is equal to number of compartments
    for (unsigned int compartment = 0; compartment < _concentrations.size(); compartment++) {

	// if no concentratiosn, set mean to '0'
	if (!(_concentrations[compartment]).size()) {
	    m_stats[compartment][static_cast<int>(CycleStatisticType::Mean)].addValue(Duration(), 0);
	    return;
	}
	
	// sum up all of concentrations
	Value sum = 0.0;
	for (auto& concentration : _concentrations[compartment])
	    sum += concentration;
	
	// add mean value with time 0
	m_stats[compartment][static_cast<int>(CycleStatisticType::Mean)].addValue(Duration(), (sum /_concentrations[compartment].size()));
    }
}

void CycleStatistics::searchPeak(const std::vector<Concentrations> &_concentrations, const std::vector<TimeOffsets> &_times)
{
    // Outer vetor size is equal to number of compartments
    for (unsigned int compartment = 0; compartment < _concentrations.size(); compartment++) {

	// search the position of the max concentrations
	std::vector<Concentration>::const_iterator it = std::max_element(_concentrations[compartment].begin(),
	_concentrations[compartment].end());
	int nbPoints =  std::distance(_concentrations[compartment].begin(), it);

	// add peak value with duration (change offset to duration)
	m_stats[compartment][static_cast<int>(CycleStatisticType::Peak)].addValue(Duration(std::chrono::seconds((int)(_times[compartment][nbPoints])*60*60)), _concentrations[compartment][nbPoints]);
    }
}


void CycleStatistics::searchMaximumsMinimums(const std::vector<Concentrations> &_concentrations, const std::vector<TimeOffsets> &_times)
{
    // Outer vetor size is equal to number of compartments
    for (unsigned int compartment = 0; compartment < _concentrations.size(); compartment++) {

	// if no concentratiosn, set AUC to '0'
	if (!(_concentrations[compartment]).size()) {
	    m_stats[compartment][static_cast<int>(CycleStatisticType::Maximum)].addValue(Duration(), 0);
	    m_stats[compartment][static_cast<int>(CycleStatisticType::Minimum)].addValue(Duration(), 0);
	    return;
	}
	
	// calculate area under curve
	Value prevGradient = 0.0, gradient = 0.0;
	for (unsigned int nbPoints = 0; nbPoints < (_concentrations[compartment].size() - 1); nbPoints++) {

	    // compute current gradient
	    gradient = _concentrations[compartment][nbPoints+1] - _concentrations[compartment][nbPoints];

	    // max: compare whether the gradient is changed from positive to negative
	    if ((prevGradient > 0) && (gradient < 0)) {
		m_stats[compartment][static_cast<int>(CycleStatisticType::Maximum)].addValue(Duration(std::chrono::seconds((int)(_times[compartment][nbPoints])*60*60)), _concentrations[compartment][nbPoints]);
	    }
	    // min: compare whether the gradient is changed from negative to positive
	    else if ((prevGradient > 0) && (gradient < 0)) {
		m_stats[compartment][static_cast<int>(CycleStatisticType::Minimum)].addValue(Duration(std::chrono::seconds((int)(_times[compartment][nbPoints])*60*60)), _concentrations[compartment][nbPoints]);
	    }

	    // store current gradient to previous gradient for next comparison
	    prevGradient = gradient;
	}
	
    }
}

void CycleStatistics::calculateAUC(const std::vector<Concentrations> &_concentrations, const
std::vector<TimeOffsets> &_times)
{
    // Outer vetor size is equal to number of compartments
    for (unsigned int compartment = 0; compartment < _concentrations.size(); compartment++) {

	// if no concentratiosn, set AUC to '0'
	if (!(_concentrations[compartment]).size()) {
	    m_stats[compartment][static_cast<int>(CycleStatisticType::AUC)].addValue(Duration(), 0);
	    return;
	}
	
	// calculate area under curve
	Value auc = 0.0;
	for (unsigned int nbPoints = 0; nbPoints < (_concentrations[compartment].size() - 1); nbPoints++) {
	    auc += (_concentrations[compartment][nbPoints] + _concentrations[compartment][nbPoints+1])
		    / 2.0 * (_times[compartment][nbPoints+1] - _times[compartment][nbPoints]);
	}
	
	// add AUC value with time 0
	m_stats[compartment][static_cast<int>(CycleStatisticType::AUC)].addValue(Duration(), auc);
    }
}


CycleStatistics::CycleStatistics(const CycleData &_data)
{
    //
    // Build a new statistics with the given type
    //

    // mean
    CycleStatistic(_data.m_start, CycleStatisticType::Mean);
    calculateMean(_data.m_concentrations);

    // peak
    CycleStatistic(_data.m_start, CycleStatisticType::Peak);
    searchPeak(_data.m_concentrations, _data.m_times);

    // maximum
    CycleStatistic(_data.m_start, CycleStatisticType::Maximum);
    // minimum
    CycleStatistic(_data.m_start, CycleStatisticType::Minimum);

    searchMaximumsMinimums(_data.m_concentrations, _data.m_times);

    // AUC: The read under of curve
    CycleStatistic(_data.m_start, CycleStatisticType::AUC);
    calculateAUC(_data.m_concentrations, _data.m_times);
}

} // Core
} // Tucuxi
