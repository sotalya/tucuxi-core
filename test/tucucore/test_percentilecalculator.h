/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TEST_PERCENTILECALCULATOR_H
#define TEST_PERCENTILECALCULATOR_H

#include <iostream>
#include <memory>

#include "fructose/fructose.h"

#include "tucucommon/general.h"

#include "tucucore/montecarlopercentilecalculator.h"
#include "tucucore/residualerrormodel.h"

struct TestPercentileCalculator : public fructose::test_base<TestPercentileCalculator>
{

    TestPercentileCalculator() { }

    void test1(const std::string& /* _testName */)
    {
        // Simple test with imatinib values

        Tucuxi::Core::PercentilesPrediction percentiles;
        int nbPoints;
        Tucuxi::Core::IntakeSeries intakeSeries;
        Tucuxi::Core::ParameterSetSeries parameters;
        Tucuxi::Core::OmegaMatrix omega;
        Tucuxi::Core::SigmaResidualErrorModel residualErrorModel;
        Tucuxi::Core::Etas etas;
        Tucuxi::Core::PercentileRanks percentileRanks;
        Tucuxi::Core::ProcessingAborter *aborter = nullptr;



        Tucuxi::Core::AprioriMonteCarloPercentileCalculator calculator;

        calculator.calculate(
                    percentiles,
                    nbPoints,
                    intakeSeries,
                    parameters,
                    omega,
                    residualErrorModel,
                    etas,
                    percentileRanks,
                    aborter);
    }


};

#endif // TEST_PERCENTILECALCULATOR_H
