/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "montecarlopercentilecalculator.h"

#include "tucucore/definitions.h"

namespace Tucuxi {
namespace Core {


MonteCarloPercentileCalculator::MonteCarloPercentileCalculator()
{

}


MonteCarloPercentileCalculator::ProcessingResult MonteCarloPercentileCalculator::computePredictionsAndSortPercentiles(
        PercentilesPrediction _percentiles,
        const int _nbPoints,
        const IntakeSeries &_intakes,
        const ParameterSetSeries &_parameters,
        const ResidualErrorModel &_residualErrorModel,
        const PercentileRanks &_percentileRanks,
        int _nbPatients,
        const std::vector<Etas> _etas,
        const std::vector<Deviation> _epsilons,
        int _curvelength,
        ProcessingAborter *_aborter)
{

    return ProcessingResult::Failure;
}

MonteCarloPercentileCalculator::ProcessingResult AprioriMonteCarloPercentileCalculator::calculate(
        PercentilesPrediction _percentiles,
        const int _nbPoints,
        const IntakeSeries &_intakes,
        const ParameterSetSeries &_parameters,
        const Omega& _omega,
        const ResidualErrorModel &_residualErrorModel,
        const Etas& _initialEta,
        const PercentileRanks &_percentileRanks,
        ProcessingAborter *_aborter)
{


    return ProcessingResult::Failure;
}




MonteCarloPercentileCalculator::ProcessingResult AposterioriMonteCarloPercentileCalculator::calculate(
        PercentilesPrediction _percentiles,
        const int _nbPoints,
        const IntakeSeries &_intakes,
        const ParameterSetSeries &_parameters,
        const Omega& _omega,
        const ResidualErrorModel &_residualErrorModel,
        const Etas& _Eta,
        const SampleSeries &_samples,
        const PercentileRanks &_percentileRanks,
        ProcessingAborter *_aborter)
{


    return ProcessingResult::Failure;
}


}
}
