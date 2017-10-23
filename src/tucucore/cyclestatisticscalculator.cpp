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


void CycleStatistics::calculateCycleStatistics(const std::vector<Concentrations> &_concentrations, const
std::vector<TimeOffsets> &_times)
{
    // Outer vetor size is equal to number of compartments
    for (unsigned int compartment = 0; compartment < _concentrations.size(); compartment++) {

	//
	// Calculate AUC and mean, Find mean and max
	//

	// if no concentratiosn, set AUC to '0'
	if (!(_concentrations[compartment]).size()) {
	    return;
	}
	
	Value prevGradient = 0.0, gradient = 0.0, auc = 0.0;
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

	    // auc: calculate area under curve
	    auc += (_concentrations[compartment][nbPoints] + _concentrations[compartment][nbPoints+1])
		    / 2.0 * (_times[compartment][nbPoints+1] - _times[compartment][nbPoints]);
	}
	
	// add AUC value with time 0
	m_stats[compartment][static_cast<int>(CycleStatisticType::AUC)].addValue(Duration(), auc);
	// add mean value with time 0
	m_stats[compartment][static_cast<int>(CycleStatisticType::AUC)].addValue(Duration(), (auc /(_times[compartment][_concentrations[compartment].size()] - _times[compartment][0])));

	//
	// Find Peak
	//

	// search the position of the max concentrations
	std::vector<Concentration>::const_iterator it = std::max_element(_concentrations[compartment].begin(),
	_concentrations[compartment].end());
	int nbPoints =  std::distance(_concentrations[compartment].begin(), it);

	// add peak value with duration (change offset to duration)
	m_stats[compartment][static_cast<int>(CycleStatisticType::Peak)].addValue(Duration(std::chrono::seconds((int)(_times[compartment][nbPoints])*60*60)), _concentrations[compartment][nbPoints]);
    }
}


CycleStatistics::CycleStatistics(const CycleData &_data)
{
    //
    // Build a new statistics with the given type
    //
    unsigned int NbCompartment = _data.m_concentrations.size();

    m_stats.reserve(NbCompartment);
    for (unsigned int compartment = 0; compartment < NbCompartment; compartment++) {
	for (unsigned int type= 0; type< static_cast<int>(CycleStatisticType::CYCLE_STATISTIC_TYPE_SIZE); type++) {
	    m_stats[compartment].push_back(CycleStatistic(_data.m_start, (CycleStatisticType)type));
	}
    }

    //
    // calcualte cylcle statistics and add values
    //
    calculateCycleStatistics(_data.m_concentrations, _data.m_times);
}

} // Core
} // Tucuxi
