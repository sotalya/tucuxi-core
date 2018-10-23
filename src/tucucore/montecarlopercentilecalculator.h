/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_MONTECARLOPERCENTILECALCULATOR_H
#define TUCUXI_CORE_MONTECARLOPERCENTILECALCULATOR_H

#include "likelihood.h"

#include "tucucore/parameter.h"
#include "tucucore/sampleevent.h"
#include "tucucore/dosage.h"

namespace Tucuxi {
namespace Core {

class IConcentrationCalculator;
class IResidualErrorModel;
class PercentilesPrediction;

///
/// \brief The ComputingAborter class
/// This class is passed to methods that require a huge processing time.
/// It only supply a method to test if the processing should be aborted or not.
/// If it returns true, the processing should be aborted and all resources freed.
/// If it returns true a second call would return false, so be carful with that.
class ComputingAborter
{
public:
    ///
    /// \brief Indicates if the calculation should be aborted
    /// \return true if the calculation should be aborted, false else
    /// This function should be called at some points in calculation in order to abort as soon as possible
    /// when asked by an external component
    ///
    virtual bool shouldAbort() { return false;}

    virtual ~ComputingAborter();
};

class IPercentileCalculator
{
public:
    enum class ComputingResult
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
    /// \param _recordFrom Date from which we start recording the concentration
    /// \param _recordTo Date until which we record the concentration
    /// \param _intakes Intake series
    /// \param _parameters Initial parameters series
    /// \param _omega covariance matrix for inter-individual variability
    /// \param _residualErrorModel Residual error model
    /// \param _etas Etas pre-calculated by the aposteriori calculator
    /// \param _percentileRanks List of percentiles ranks
    /// \param _aborter An aborter object allowing to abort the calculation
    /// \return The status of calculation
    virtual ComputingResult calculate(
            PercentilesPrediction &_percentiles,
            const DateTime &_recordFrom,
            const DateTime &_recordTo,
            const IntakeSeries &_intakes,
            const ParameterSetSeries &_parameters,
            const OmegaMatrix& _omega,
            const IResidualErrorModel &_residualErrorModel,
            const Etas& _etas,
            const PercentileRanks &_percentileRanks,
            IConcentrationCalculator &_concentrationCalculator,
            ComputingAborter *_aborter) = 0;

    virtual ~IAprioriPercentileCalculator() {};
};

class IAposterioriPercentileCalculator : public IPercentileCalculator
{
public:

    ///
    /// \brief calculate
    /// \param _percentiles percentiles calculated within the method
    /// \param _recordFrom Date from which we start recording the concentration
    /// \param _recordTo Date until which we record the concentration
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
    virtual ComputingResult calculate(
            PercentilesPrediction &_percentiles,
            const DateTime &_recordFrom,
            const DateTime &_recordTo,
            const IntakeSeries &_intakes,
            const ParameterSetSeries &_parameters,
            const OmegaMatrix& _omega,
            const IResidualErrorModel &_residualErrorModel,
            const Etas& _etas,
            const SampleSeries &_samples,
            const PercentileRanks &_percentileRanks,
            IConcentrationCalculator &_concentrationCalculator,
            ComputingAborter *_aborter) = 0;

    virtual ~IAposterioriPercentileCalculator() {};
};

class IAposterioriNormalApproximationMonteCarloPercentileCalculator : public IPercentileCalculator
{
public:

    ///
    /// \brief calculate
    /// \param _percentiles percentiles calculated within the method
    /// \param _recordFrom Date from which we start recording the concentration
    /// \param _recordTo Date until which we record the concentration
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
    virtual ComputingResult calculate(
            PercentilesPrediction &_percentiles,
            const DateTime &_recordFrom,
            const DateTime &_recordTo,
            const IntakeSeries &_intakes,
            const ParameterSetSeries &_parameters,
            const OmegaMatrix& _omega,
            const IResidualErrorModel &_residualErrorModel,
            const Etas& _etas,
            const SampleSeries &_samples,
            const PercentileRanks &_percentileRanks,
            IConcentrationCalculator &_concentrationCalculator,
            ComputingAborter *_aborter) = 0;

    virtual ~IAposterioriNormalApproximationMonteCarloPercentileCalculator() {};
};



class MonteCarloPercentileCalculatorBase : public IPercentileCalculator
{
public:
    MonteCarloPercentileCalculatorBase();

    static void setStaticNumberPatients(const unsigned int _nbPatients) {
        sm_nbPatients = _nbPatients;
    }

    void setNumberPatients(const unsigned int _nbPatients) { m_nbPatients = _nbPatients; }
    unsigned int getNumberPatients() { return m_nbPatients; }

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
    /// \param _recordFrom Date from which we start recording the concentration
    /// \param _recordTo Date until which we record the concentration
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
    ComputingResult computePredictionsAndSortPercentiles(
            PercentilesPrediction &_percentiles,
            const DateTime &_recordFrom,
            const DateTime &_recordTo,
            const IntakeSeries &_intakes,
            const ParameterSetSeries &_parameters,
            const IResidualErrorModel &_residualErrorModel,
            const PercentileRanks &_percentileRanks,
            const std::vector<Etas> &_etas,
            const std::vector<Deviations> &_epsilons,
            IConcentrationCalculator &_concentrationCalculator,
            ComputingAborter *_aborter);

private:
    unsigned int m_nbPatients;

    static unsigned int sm_nbPatients;
};


/*
* Used to calculated Monte Carlo of population or apriori curves
* The number of simulated curves is hardcoded to be 10,000
*/
class AprioriMonteCarloPercentileCalculator : public IAprioriPercentileCalculator,
        public MonteCarloPercentileCalculatorBase
{
public:
    AprioriMonteCarloPercentileCalculator();

    ///
    /// \brief calculate
    /// \param _percentiles percentiles calculated within the method
    /// \param _recordFrom Date from which we start recording the concentration
    /// \param _recordTo Date until which we record the concentration
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
    ComputingResult calculate(
            PercentilesPrediction &_percentiles,
            const DateTime &_recordFrom,
            const DateTime &_recordTo,
            const IntakeSeries &_intakes,
            const ParameterSetSeries &_parameters,
            const OmegaMatrix& _omega,
            const IResidualErrorModel &_residualErrorModel,
            const Etas& _initialEtas,
            const PercentileRanks &_percentileRanks,
            IConcentrationCalculator &_concentrationCalculator,
            ComputingAborter *_aborter) override;
};


class AposterioriMonteCarloPercentileCalculator : public IAposterioriPercentileCalculator, public MonteCarloPercentileCalculatorBase 
{
public:
    AposterioriMonteCarloPercentileCalculator();

    ///
    /// \brief calculate
    /// \param _percentiles percentiles calculated within the method
    /// \param _recordFrom Date from which we start recording the concentration
    /// \param _recordTo Date until which we record the concentration
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
    ComputingResult calculate(
            PercentilesPrediction &_percentiles,
            const DateTime &_recordFrom,
            const DateTime &_recordTo,
            const IntakeSeries &_intakes,
            const ParameterSetSeries &_parameters,
            const OmegaMatrix& _omega,
            const IResidualErrorModel &_residualErrorModel,
            const Etas& _etas,
            const SampleSeries &_samples,
            const PercentileRanks &_percentileRanks,
            IConcentrationCalculator &_concentrationCalculator,
            ComputingAborter *_aborter) override;
};


/// \brief The AposterioriNormalApproximationMonteCarloPercentileCalculator class
/// This class implements the normal approximation of the posterior, and
/// only apply steps 1 and 2 of the Annex A of Aziz document (posteriori2.pdf)
class AposterioriNormalApproximationMonteCarloPercentileCalculator : public IAposterioriNormalApproximationMonteCarloPercentileCalculator, public MonteCarloPercentileCalculatorBase {

public:

    AposterioriNormalApproximationMonteCarloPercentileCalculator();

    ///
    /// \brief calculate
    /// \param _percentiles percentiles calculated within the method
    /// \param _recordFrom Date from which we start recording the concentration
    /// \param _recordTo Date until which we record the concentration
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
    ComputingResult calculate(
            PercentilesPrediction &_percentiles,
            const DateTime &_recordFrom,
            const DateTime &_recordTo,
            const IntakeSeries &_intakes,
            const ParameterSetSeries &_parameters,
            const OmegaMatrix& _omega,
            const IResidualErrorModel &_residualErrorModel,
            const Etas& _etas,
            const SampleSeries &_samples,
            const PercentileRanks &_percentileRanks,
            IConcentrationCalculator &_concentrationCalculator,
            ComputingAborter *_aborter) override;
};


} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_MONTECARLOPERCENTILECALCULATOR_H
