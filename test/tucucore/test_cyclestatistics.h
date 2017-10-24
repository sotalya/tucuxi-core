/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TEST_CYCLESTATISTICS_H
#define TEST_CYCLESTATISTICS_H

#include <iostream>
#include <memory>

#include "fructose/fructose.h"

#include "tucucommon/general.h"

#include "tucucore/residualerrormodel.h"
#include "tucucore/onecompartmentextra.h"
#include "tucucore/cyclestatisticscalculator.h"

struct TestCycleStatistics : public fructose::test_base<TestCycleStatistics>
{

    TestCycleStatistics() { }

    void test1(const std::string& /* _testName */)
    {
	// set cycleData
        DateTime now;
	int nbCompartments = 1, nbPoints = 10;
	Tucuxi::Core::CycleData cycleData;
	std::vector< std::vector<Tucuxi::Core::CycleStatistic> > stats;
	
	cycleData.m_start = now;

	Value concentrations[nbCompartments][nbPoints] = {{0, 10.2, 20.5, 15.0, 9.7, 10, 12.7, 15.2, 13.2, 14}};
	Value times[nbCompartments][nbPoints] = {{0, 10, 20, 30, 40, 50, 60, 70, 80, 90}};

	// reserve vectors of concentrations and times
	cycleData.m_concentrations.reserve(nbCompartments);
	cycleData.m_times.reserve(nbCompartments);
	stats.reserve(nbCompartments);
	for (int compartment = 0; compartment < nbCompartments; compartment++) {
	    cycleData.m_concentrations[compartment].reserve(nbPoints);
	    cycleData.m_times[compartment].reserve(nbPoints);

	    // assign data of concentrations and times
	    cycleData.m_concentrations[compartment].assign(concentrations[compartment][0], concentrations[compartment][nbPoints-1]);
	    cycleData.m_times[compartment].assign(times[compartment][0], times[compartment][nbPoints-1]);

	    stats[compartment].reserve(static_cast<int>(CycleStatisticType::CYCLE_STATISTIC_TYPE_SIZE));
	}

	// calculate cycle statistics
	DateTime dateTime;
	Value value;
	Tucuxi::Core::CycleStatistics cycleStatistics(cycleData);
	for (int compartment = 0; compartment < nbCompartments; compartment++) {
	}
	
	cycleStatistics.getStatistics(CycleStatisticType::AUC, stats);
	stats[0][static_cast<int>(CycleStatisticType::AUC)].getValue(dateTime, value, 0);
	//std::cout << "AUC: dataTime: " << dateTime << ", value" << value << std::endl;

	cycleStatistics.getStatistics(CycleStatisticType::Mean, stats);
	stats[0][static_cast<int>(CycleStatisticType::Mean)].getValue(dateTime, value, 0);
	//std::cout << "Mean: " << dateTime << ", value" << value << std::endl;

	cycleStatistics.getStatistics(CycleStatisticType::Peak, stats);
	stats[0][static_cast<int>(CycleStatisticType::Peak)].getValue(dateTime, value, 0);
	//std::cout << "Peak: " << dateTime << ", value" << value << std::endl;

	cycleStatistics.getStatistics(CycleStatisticType::Maximum, stats);
	stats[0][static_cast<int>(CycleStatisticType::Maximum)].getValue(dateTime, value, 0);
	//std::cout << "Maximum: " << dateTime << ", value" << value << std::endl;

	cycleStatistics.getStatistics(CycleStatisticType::Minimum, stats);
	stats[0][static_cast<int>(CycleStatisticType::Minimum)].getValue(dateTime, value, 0);
	//std::cout << "Minimum: " << dateTime << ", value" << value << std::endl;
    }
};

#endif // TEST_CYCLESTATISTICS_H
