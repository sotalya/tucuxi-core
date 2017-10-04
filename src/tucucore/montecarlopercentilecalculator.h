/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_MONTECARLOPERCENTILECALCULATOR_H
#define TUCUXI_CORE_MONTECARLOPERCENTILECALCULATOR_H

#include "likelihood.h"

#include "tucucore/parameter.h"
#include "tucucore/sampleevent.h"
#include "tucucore/dosage.h"
#include "tucucore/iprocessingservices.h"

namespace Tucuxi {
namespace Core {

class IConcentrationCalculator;

class IResidualErrorModel;

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

class IPercentileCalculator
{
public:

    enum class ProcessingResult
    {
        Success,
        Failure,
        Aborted
    };

};


class IAprioriPercentileCalculator : public IPercentileCalculator
{
public:

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
    virtual ProcessingResult calculate(
            PercentilesPrediction &_percentiles,
            const int _nbPoints,
            const IntakeSeries &_intakes,
            const ParameterSetSeries &_parameters,
            const OmegaMatrix& _omega,
            const IResidualErrorModel &_residualErrorModel,
            const Etas& _etas,
            const PercentileRanks &_percentileRanks,
	    IConcentrationCalculator &_concentrationCalculator,
            ProcessingAborter *_aborter) = 0;
};

class IAposterioriPercentileCalculator : public IPercentileCalculator
{
public:

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
    virtual ProcessingResult calculate(
            PercentilesPrediction &_percentiles,
            const int _nbPoints,
            const IntakeSeries &_intakes,
            const ParameterSetSeries &_parameters,
            const OmegaMatrix& _omega,
            const IResidualErrorModel &_residualErrorModel,
            const Etas& _etas,
            const SampleSeries &_samples,
            const PercentileRanks &_percentileRanks,
	    IConcentrationCalculator &_concentrationCalculator,
            ProcessingAborter *_aborter) = 0;
};



class MonteCarloPercentileCalculatorBase : public IPercentileCalculator
{

public:


    MonteCarloPercentileCalculatorBase();

    void setNumberPatients(const unsigned int _nbPatients) { m_nbPatients = _nbPatients; };

    unsigned int getNumberPatients() { return m_nbPatients; };



protected:

    ///
    /// \brief calculate
    /// \param _intakes Intake series
    /// \param _parameters Initial parameters series
    /// \param _omega covariance matrix for inter-individual variability
    /// \param _residualErrorModel Residual error model
    /// \param _etas Etas pre-calculated by the aposteriori calculator
    /// \param _samples List of samples
    /// \param _subomega Result of this function, non-negative hessian matrix
    void calculateSubomega(
	    const OmegaMatrix& _omega,
	    const Etas& _etas,
	    Likelihood &_logLikelihood,
	    EigenMatrix &_subomega);

protected:

    ///
    /// \brief computePredictionsAndSortPercentiles
    /// \param _percentiles percentiles calculated within the method
    /// \param _nbPoints Number of points asked for each cycle
    /// \param _intakes Intake series
    /// \param _parameters Initial parameters series
    /// \param _residualErrorModel Residual error model
    /// \param _percentileRanks List of percentiles ranks
    /// \param _etas Set of Etas to apply for each specific patient
    /// \param _epsilons Set of epsilons, one vector per patient
    /// \param _aborter An aborter object allowing to abort the calculation
    /// \return The status of calculation
    ///
    ProcessingResult computePredictionsAndSortPercentiles(
            PercentilesPrediction &_percentiles,
            const int _nbPoints,
            const IntakeSeries &_intakes,
            const ParameterSetSeries &_parameters,
            const IResidualErrorModel &_residualErrorModel,
            const PercentileRanks &_percentileRanks,
            const std::vector<Etas> _etas,
	    const std::vector<Deviations> _epsilons,
	    IConcentrationCalculator &_concentrationCalculator,
            ProcessingAborter *_aborter);

private:
    unsigned int m_nbPatients;

};


/*
* Used to calculated Monte Carlo of population or apriori curves
* The number of simulated curves is hardcoded to be 10,000
*/
class AprioriMonteCarloPercentileCalculator : public IAprioriPercentileCalculator,
        public MonteCarloPercentileCalculatorBase {

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
    ProcessingResult calculate(
            PercentilesPrediction &_percentiles,
            const int _nbPoints,
            const IntakeSeries &_intakes,
            const ParameterSetSeries &_parameters,
            const OmegaMatrix& _omega,
            const IResidualErrorModel &_residualErrorModel,
            const Etas& _initialEtas,
            const PercentileRanks &_percentileRanks,
	    IConcentrationCalculator &_concentrationCalculator,
            ProcessingAborter *_aborter) override;


};


class AposterioriMonteCarloPercentileCalculator : public MonteCarloPercentileCalculatorBase {

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
    ProcessingResult calculate(
            PercentilesPrediction &_percentiles,
            const int _nbPoints,
            const IntakeSeries &_intakes,
            const ParameterSetSeries &_parameters,
            const OmegaMatrix& _omega,
            const IResidualErrorModel &_residualErrorModel,
            const Etas& _etas,
            const SampleSeries &_samples,
            const PercentileRanks &_percentileRanks,
	    IConcentrationCalculator &_concentrationCalculator,
            ProcessingAborter *_aborter);


};


/// \brief The AposterioriNormalApproximationMonteCarloPercentileCalculator class
/// This class implements the normal approximation of the posterior, and
/// only apply steps 1 and 2 of the Annex A of Aziz document (posteriori2.pdf)
class AposterioriNormalApproximationMonteCarloPercentileCalculator : public MonteCarloPercentileCalculatorBase {

public:

    AposterioriNormalApproximationMonteCarloPercentileCalculator();

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
    ProcessingResult calculate(
            PercentilesPrediction &_percentiles,
            const int _nbPoints,
            const IntakeSeries &_intakes,
            const ParameterSetSeries &_parameters,
            const OmegaMatrix& _omega,
            const IResidualErrorModel &_residualErrorModel,
            const Etas& _etas,
            const SampleSeries &_samples,
            const PercentileRanks &_percentileRanks,
	    IConcentrationCalculator &_concentrationCalculator,
            ProcessingAborter *_aborter);


};


}
}

#endif // TUCUXI_CORE_MONTECARLOPERCENTILECALCULATOR_H
