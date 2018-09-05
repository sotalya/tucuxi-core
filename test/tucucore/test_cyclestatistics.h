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
#include "tucucore/pkmodels/onecompartmentextra.h"
#include "tucucore/cyclestatisticscalculator.h"
#include "tucucore/concentrationprediction.h"
#include "tucucore/definitions.h"

// TODO : Add a test for residual value CycleStatisticType::Residual

struct TestCycleStatistics : public fructose::test_base<TestCycleStatistics>
{
    TestCycleStatistics() { }

    void test1CycleStatistics(const std::string& /* _testName */)
    {
        // set cycleData
        DateTime now;
        int nbCompartments = 1;
        int nbPoints = 10;
        Tucuxi::Core::CycleData cycleData;
        std::vector< std::vector<Tucuxi::Core::CycleStatistic> > stats;

        cycleData.m_start = now;

        std::vector<Concentrations> concentrations{{0, 10.2, 20.5, 15.0, 9.7, 10, 12.7, 15.2, 13.2, 14}};
        std::vector<TimeOffsets> times{{0, 0.5, 1, 1.5, 2, 2.5, 3, 3.5, 4, 4.5}};

        for (int compartment = 0; compartment < nbCompartments; compartment++) {
            // assign concentration data
            cycleData.m_concentrations.push_back(Concentrations ());
            cycleData.m_concentrations[compartment].reserve(nbPoints);
            cycleData.m_concentrations[compartment].assign(concentrations[compartment].begin(),concentrations[compartment].end());

            // assign time data
            cycleData.m_times.push_back(TimeOffsets ());
            cycleData.m_times[compartment].reserve(nbPoints);
            cycleData.m_times[compartment].assign(times[compartment].begin(),times[compartment].end());

            // allocate memory for local variable stats
            stats.push_back(std::vector<Tucuxi::Core::CycleStatistic> ());
            for (unsigned int type= 0; type< static_cast<int>(CycleStatisticType::CYCLE_STATISTIC_TYPE_SIZE); type++) {
                stats[compartment].push_back(CycleStatistic(cycleData.m_start, (CycleStatisticType)type));
            }
        }

        // calculate cycle statistics
        DateTime dateTime;
        Value value;
        Tucuxi::Core::CycleStatistics cycleStatistics(cycleData);

        // AUC
        cycleStatistics.getStatistics(CycleStatisticType::AUC, stats);
        stats[0][static_cast<int>(CycleStatisticType::AUC)].getValue(dateTime, value, 0);
        //std::cout << "[AUC] dataTime: " << dateTime << ", value: " << value << std::endl;
        fructose_assert_double_eq_rel_abs(56.75, value, 0.01, 0.01);

        // Mean
        cycleStatistics.getStatistics(CycleStatisticType::Mean, stats);
        stats[0][static_cast<int>(CycleStatisticType::Mean)].getValue(dateTime, value, 0);
        //std::cout << "[Mean] dateTime: " << dateTime << ", value: " << value << std::endl;
        fructose_assert_double_eq_rel_abs(12.61, value, 0.01, 0.01);

        // Peak
        cycleStatistics.getStatistics(CycleStatisticType::Peak, stats);
        stats[0][static_cast<int>(CycleStatisticType::Peak)].getValue(dateTime, value, 0);
        //std::cout << "[Peak] dateTime: " << dateTime << ", value: " << value << std::endl;
        fructose_assert_double_eq(20.5, value);

        // Max
        size_t nbValue;
        cycleStatistics.getStatistics(CycleStatisticType::Maximum, stats);
        for (nbValue=0; nbValue<stats[0][static_cast<int>(CycleStatisticType::Maximum)].getNbValue(); nbValue++) {
            stats[0][static_cast<int>(CycleStatisticType::Maximum)].getValue(dateTime, value, nbValue);
            //std::cout << "[Maximum] dateTime: " << dateTime << ", value: " << value << std::endl;
        }
        fructose_assert_eq(nbValue, 2);

        // Min
        cycleStatistics.getStatistics(CycleStatisticType::Minimum, stats);
        for (size_t nbValue=0; nbValue<stats[0][static_cast<int>(CycleStatisticType::Minimum)].getNbValue(); nbValue++) {
            stats[0][static_cast<int>(CycleStatisticType::Minimum)].getValue(dateTime, value, nbValue);
            //std::cout << "[Minimum] dateTime: " << dateTime << ", value: " << value << std::endl;
        }
        fructose_assert_eq(nbValue, 2);
    }
};

#endif // TEST_CYCLESTATISTICS_H
