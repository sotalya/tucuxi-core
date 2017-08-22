/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_MONTECARLOPERCENTILECALCULATOR_H
#define TUCUXI_CORE_MONTECARLOPERCENTILECALCULATOR_H


#include "tucucore/parameter.h"
#include "tucucore/sample.h"
#include "tucucore/dosage.h"
#include "tucucore/iprocessingservices.h"

namespace Tucuxi {
namespace Core {

class ResidualErrorModel;


///
/// \brief The ProcessingAborter class
/// This class is passed to methods that require a huge processing time.
/// It only supply a method to test if the processing should be aborted or not.
/// If it returns true, the processing should be aborted and all resources freed.
/// If it returns true a second call would return false, so be carful with that.
class ProcessingAborter
{
public:

    ///
    /// \brief Indicates if the calculation should be aborted
    /// \return true if the calculation should be aborted, false else
    /// This function should be called at some points in calculation in order to abort as soon as possible
    /// when asked by an external component
    ///
    virtual bool shouldAbort() { return false;}
};



class MonteCarloPercentileCalculator {

public:

    enum class ProcessingResult
    {
        Success,
        Failure,
        Aborted
    };

    MonteCarloPercentileCalculator();

protected:

    ///
    /// \brief computePredictionsAndSortPercentiles
    /// \param _percentiles percentiles calculated within the method
    /// \param _nbPoints Number of points asked for each cycle
    /// \param _intakes Intake series
    /// \param _parameters Initial parameters series
    /// \param _residualErrorModel Residual error model
    /// \param _percentileRanks List of percentiles ranks
    /// \param _nbPatients Number of patients generated by the caller
    /// \param _etas Set of Etas to apply for each specific patient
    /// \param _epsilons Set of epsilons, one per patient
    /// \param _curvelength Total number of points to be returned
    /// \param _aborter An aborter object allowing to abort the calculation
    /// \return The status of calculation
    ///
    ProcessingResult computePredictionsAndSortPercentiles(
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
            ProcessingAborter *_aborter);
};


/*
* Used to calculated Monte Carlo of population or apriori curves
* The number of simulated curves is hardcoded to be 10,000
*/
class AprioriMonteCarloPercentileCalculator : public MonteCarloPercentileCalculator {

public:

    AprioriMonteCarloPercentileCalculator();

    ///
    /// \brief calculate
    /// \param _percentiles percentiles calculated within the method
    /// \param _nbPoints Number of points asked for each cycle
    /// \param _intakes Intake series
    /// \param _parameters Initial parameters series
    /// \param _omega covariance matrix for inter-individual variability
    /// \param _residualErrorModel Residual error model
    /// \param _initialEtas Set of initial Etas, used in case of a posteriori
    /// \param _percentileRanks List of percentiles ranks
    /// \param _aborter An aborter object allowing to abort the calculation
    /// \return The status of calculation
    ///
    ProcessingResult calculate(PercentilesPrediction _percentiles,
            const int _nbPoints,
            const IntakeSeries &_intakes,
            const ParameterSetSeries &_parameters,
            const OmegaMatrix& _omega,
            const ResidualErrorModel &_residualErrorModel,
            const Etas& _initialEtas,
            const PercentileRanks &_percentileRanks,
            ProcessingAborter *_aborter);


};


class AposterioriMonteCarloPercentileCalculator : public MonteCarloPercentileCalculator {

public:

    AposterioriMonteCarloPercentileCalculator();

    ///
    /// \brief calculate
    /// \param _percentiles percentiles calculated within the method
    /// \param _nbPoints Number of points asked for each cycle
    /// \param _intakes Intake series
    /// \param _parameters Initial parameters series
    /// \param _omega covariance matrix for inter-individual variability
    /// \param _residualErrorModel Residual error model
    /// \param _etas Etas pre-calculated by the aposteriori calculator
    /// \param _samples List of samples
    /// \param _percentileRanks List of percentiles ranks
    /// \param _aborter An aborter object allowing to abort the calculation
    /// \return The status of calculation
    ProcessingResult calculate(PercentilesPrediction _percentiles,
            const int _nbPoints,
            const IntakeSeries &_intakes,
            const ParameterSetSeries &_parameters,
            const OmegaMatrix& _omega,
            const ResidualErrorModel &_residualErrorModel,
            const Etas& _etas,
            const SampleSeries &_samples,
            const PercentileRanks &_percentileRanks,
            ProcessingAborter *_aborter);


};


}
}

#endif // TUCUXI_CORE_MONTECARLOPERCENTILECALCULATOR_H
