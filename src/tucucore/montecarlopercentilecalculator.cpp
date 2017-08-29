/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "montecarlopercentilecalculator.h"

#include "tucucore/definitions.h"

namespace Tucuxi {
namespace Core {


MonteCarloPercentileCalculatorBase::MonteCarloPercentileCalculatorBase()
{

}


IPercentileCalculator::ProcessingResult MonteCarloPercentileCalculatorBase::computePredictionsAndSortPercentiles(
        PercentilesPrediction _percentiles,
        const int _nbPoints,
        const IntakeSeries &_intakes,
        const ParameterSetSeries &_parameters,
        const IResidualErrorModel &_residualErrorModel,
        const PercentileRanks &_percentileRanks,
        int _nbPatients,
        const std::vector<Etas> _etas,
        const std::vector<Deviations> _epsilons,
        int _curvelength,
        ProcessingAborter *_aborter)
{
    TMP_UNUSED_PARAMETER(_percentiles);
    TMP_UNUSED_PARAMETER(_nbPoints);
    TMP_UNUSED_PARAMETER(_intakes);
    TMP_UNUSED_PARAMETER(_parameters);
    TMP_UNUSED_PARAMETER(_residualErrorModel);
    TMP_UNUSED_PARAMETER(_percentileRanks);
    TMP_UNUSED_PARAMETER(_nbPatients);
    TMP_UNUSED_PARAMETER(_etas);
    TMP_UNUSED_PARAMETER(_epsilons);
    TMP_UNUSED_PARAMETER(_curvelength);
    TMP_UNUSED_PARAMETER(_aborter);

    return ProcessingResult::Failure;
}

IPercentileCalculator::ProcessingResult AprioriMonteCarloPercentileCalculator::calculate(
        PercentilesPrediction _percentiles,
        const int _nbPoints,
        const IntakeSeries &_intakes,
        const ParameterSetSeries &_parameters,
        const OmegaMatrix& _omega,
        const IResidualErrorModel &_residualErrorModel,
        const Etas& _initialEtas,
        const PercentileRanks &_percentileRanks,
        ProcessingAborter *_aborter)
{
    TMP_UNUSED_PARAMETER(_percentiles);
    TMP_UNUSED_PARAMETER(_nbPoints);
    TMP_UNUSED_PARAMETER(_intakes);
    TMP_UNUSED_PARAMETER(_parameters);
    TMP_UNUSED_PARAMETER(_omega);
    TMP_UNUSED_PARAMETER(_residualErrorModel);
    TMP_UNUSED_PARAMETER(_initialEtas);
    TMP_UNUSED_PARAMETER(_percentileRanks);
    TMP_UNUSED_PARAMETER(_aborter);


    return ProcessingResult::Failure;
}




IPercentileCalculator::ProcessingResult AposterioriMonteCarloPercentileCalculator::calculate(
        PercentilesPrediction _percentiles,
        const int _nbPoints,
        const IntakeSeries &_intakes,
        const ParameterSetSeries &_parameters,
        const OmegaMatrix& _omega,
        const IResidualErrorModel &_residualErrorModel,
        const Etas& _etas,
        const SampleSeries &_samples,
        const PercentileRanks &_percentileRanks,
        ProcessingAborter *_aborter)
{
    TMP_UNUSED_PARAMETER(_percentiles);
    TMP_UNUSED_PARAMETER(_nbPoints);
    TMP_UNUSED_PARAMETER(_intakes);
    TMP_UNUSED_PARAMETER(_parameters);
    TMP_UNUSED_PARAMETER(_omega);
    TMP_UNUSED_PARAMETER(_residualErrorModel);
    TMP_UNUSED_PARAMETER(_etas);
    TMP_UNUSED_PARAMETER(_samples);
    TMP_UNUSED_PARAMETER(_percentileRanks);
    TMP_UNUSED_PARAMETER(_aborter);


    return ProcessingResult::Failure;
}


IPercentileCalculator::ProcessingResult AposterioriNormalApproximationMonteCarloPercentileCalculator::calculate(
        PercentilesPrediction _percentiles,
        const int _nbPoints,
        const IntakeSeries &_intakes,
        const ParameterSetSeries &_parameters,
        const OmegaMatrix& _omega,
        const IResidualErrorModel &_residualErrorModel,
        const Etas& _etas,
        const SampleSeries &_samples,
        const PercentileRanks &_percentileRanks,
        ProcessingAborter *_aborter)
{
    TMP_UNUSED_PARAMETER(_percentiles);
    TMP_UNUSED_PARAMETER(_nbPoints);
    TMP_UNUSED_PARAMETER(_intakes);
    TMP_UNUSED_PARAMETER(_parameters);
    TMP_UNUSED_PARAMETER(_omega);
    TMP_UNUSED_PARAMETER(_residualErrorModel);
    TMP_UNUSED_PARAMETER(_etas);
    TMP_UNUSED_PARAMETER(_samples);
    TMP_UNUSED_PARAMETER(_percentileRanks);
    TMP_UNUSED_PARAMETER(_aborter);


    return ProcessingResult::Failure;
}


}
}
