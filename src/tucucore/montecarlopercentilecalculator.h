/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_MONTECARLOPERCENTILECALCULATOR_H
#define TUCUXI_CORE_MONTECARLOPERCENTILECALCULATOR_H

#include "likelihood.h"

#include "tucucore/parameter.h"
#include "tucucore/sampleevent.h"
#include "tucucore/dosage.h"
#include "tucucore/computingservice/computingaborter.h"
#include "tucucore/computingservice/computingresult.h"

namespace Tucuxi {
namespace Core {

class IConcentrationCalculator;
class IResidualErrorModel;
class PercentilesPrediction;
class ActiveMoiety;


class IPercentileCalculator
{
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
    virtual ComputingStatus calculate(
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

    virtual ~IAprioriPercentileCalculator() {}
};

class IAposterioriPercentileCalculator : public IPercentileCalculator
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
    /// \param _samples List of samples
    /// \param _percentileRanks List of percentiles ranks
    /// \param _aborter An aborter object allowing to abort the calculation
    /// \return The status of calculation
    virtual ComputingStatus calculate(
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

    virtual ~IAposterioriPercentileCalculator() {}
};

class IAposterioriNormalApproximationMonteCarloPercentileCalculator : public IPercentileCalculator
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
    /// \param _samples List of samples
    /// \param _percentileRanks List of percentiles ranks
    /// \param _aborter An aborter object allowing to abort the calculation
    /// \return The status of calculation
    virtual ComputingStatus calculate(
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

    virtual ~IAposterioriNormalApproximationMonteCarloPercentileCalculator() {}
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

public:

    ///
    /// \brief computePredictionsAndSortPercentiles
    /// \param _percentiles percentiles calculated within the method
    /// \param _recordFrom Date from which we start recording the concentration
    /// \param _recordTo Date until which we record the concentration
    /// \param _intakes Intake series
    /// \param _parameters Initial parameters series
    /// \param _residualErrorModel Residual error model
    /// \param _percentileRanks List of percentiles ranks
    /// \param _etas Set of Etas to apply for each specific patient
    /// \param _epsilons Set of epsilons, one vector per patient
    /// \param _aborter An aborter object allowing to abort the calculation
    /// \return The status of calculation
    ///
    ComputingStatus computePredictionsAndSortPercentiles(
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

    ComputingStatus computePredictions(const DateTime &_recordFrom,
            const DateTime &_recordTo,
            const IntakeSeries &_intakes,
            const ParameterSetSeries &_parameters,
            const IResidualErrorModel &_residualErrorModel,
            const std::vector<Etas> &_etas,
            const std::vector<Deviations> &_epsilons,
            IConcentrationCalculator &_concentrationCalculator,
            unsigned int _nbPatients,
            std::vector<TimeOffsets> &_times,
            IntakeSeries &_recordedIntakes,
            std::vector< std::vector< std::vector<Concentration> > > &_concentrations,
            ComputingAborter *_aborter);

    ComputingStatus sortAndExtractPercentiles(PercentilesPrediction &_percentiles,
            const PercentileRanks &_percentileRanks,
            unsigned int _nbPatients,
            std::vector<TimeOffsets> _times,
            IntakeSeries &_recordedIntakes,
            std::vector< std::vector< std::vector<Concentration> > > &_concentrations);

private:
    unsigned int m_nbPatients;

    static unsigned int sm_nbPatients; // NOLINT(readability-identifier-naming)
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
    /// \param _intakes Intake series
    /// \param _parameters Initial parameters series
    /// \param _omega covariance matrix for inter-individual variability
    /// \param _residualErrorModel Residual error model
    /// \param _initialEtas Set of initial Etas, used in case of a posteriori
    /// \param _percentileRanks List of percentiles ranks
    /// \param _aborter An aborter object allowing to abort the calculation
    /// \return The status of calculation
    ///
    ComputingStatus calculate(
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


    ///
    /// \brief calculateEtasAndEpsilons
    /// \param _etas Etas calculated within the function
    /// \param _epsilons Epsilons calculated within the function
    /// \param _omega covariance matrix for inter-individual variability
    /// \param _residualErrorModel Residual error model
    /// \param _initialEtas Set of initial Etas, used in case of a posteriori
    /// \return The status of calculation
    ///
    ComputingStatus calculateEtasAndEpsilons(std::vector<Etas> &_etas,
            std::vector<Deviations> &_epsilons,
            const OmegaMatrix& _omega,
            const IResidualErrorModel &_residualErrorModel,
            const Etas& _initialEtas);
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
    /// \param _intakes Intake series
    /// \param _parameters Initial parameters series
    /// \param _omega covariance matrix for inter-individual variability
    /// \param _residualErrorModel Residual error model
    /// \param _etas Etas pre-calculated by the aposteriori calculator
    /// \param _samples List of samples
    /// \param _percentileRanks List of percentiles ranks
    /// \param _aborter An aborter object allowing to abort the calculation
    /// \return The status of calculation
    ComputingStatus calculate(
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


    ///
    /// \brief calculate
    /// \param _percentiles percentiles calculated within the method
    /// \param _intakes Intake series
    /// \param _parameters Initial parameters series
    /// \param _omega covariance matrix for inter-individual variability
    /// \param _residualErrorModel Residual error model
    /// \param _etas Etas pre-calculated by the aposteriori calculator
    /// \param _samples List of samples
    /// \param _aborter An aborter object allowing to abort the calculation
    /// \return The status of calculation
    ComputingStatus calculateEtasAndEpsilons(std::vector<Etas> &_fullEtas,
            std::vector<Deviations> &_epsilons,
            const IntakeSeries &_intakes,
            const ParameterSetSeries &_parameters,
            const OmegaMatrix& _omega,
            const IResidualErrorModel &_residualErrorModel,
            const Etas& _etas,
            const SampleSeries &_samples,
            IConcentrationCalculator &_concentrationCalculator,
            ComputingAborter *_aborter);
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
    /// \param _intakes Intake series
    /// \param _parameters Initial parameters series
    /// \param _omega covariance matrix for inter-individual variability
    /// \param _residualErrorModel Residual error model
    /// \param _etas Etas pre-calculated by the aposteriori calculator
    /// \param _samples List of samples
    /// \param _percentileRanks List of percentiles ranks
    /// \param _aborter An aborter object allowing to abort the calculation
    /// \return The status of calculation
    ComputingStatus calculate(
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

///
/// \brief The AposterioriMatrixCache class
/// This class embeds a map of matrices with a matrix
/// for each pair of nbSamples and nbEtas. When creating the matrices,
/// it populates them with random samples. If a matrix already exists for pair
/// <nbSamples,nbEtas>, then it is reused.
/// The method for getting a matrix is reentrant.
///
/// Tested by TestPercentileCalculator
///
class AposterioriMatrixCache {
public:

    ///
    /// \brief get a matrix
    /// \param _nbSamples Number of samples of the matrix
    /// \param _nbEtas Number of etas of the matrix
    /// \return A reference to the matrix
    ///
    /// This function only creates a matrix if it does not exist yet.
    /// When the matrix is created it is filled with random values.
    ///
    /// Be careful to use it correctly:
    /// const EigenMatrix &m = matrixCache.getAvecs(x,y);
    /// If not, then the matrix would be copied and so the system would be less
    /// efficient.
    ///
    const EigenMatrix &getAvecs(int _nbSamples, int _nbEtas);
protected:

    ///! The map of matrices
    std::map<std::pair<int, int>, EigenMatrix> m_matrices;
};




class IAprioriPercentileCalculatorMulti : public IPercentileCalculator
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
    virtual ComputingStatus calculate(
            PercentilesPrediction &_percentiles,
            const DateTime &_recordFrom,
            const DateTime &_recordTo,
            const GroupsIntakeSeries &_intakes,
            const GroupsParameterSetSeries &_parameters,
            const std::map<AnalyteGroupId, OmegaMatrix>& _omega,
            const std::map<AnalyteGroupId, std::unique_ptr<IResidualErrorModel> > &_residualErrorModel,
            const std::map<AnalyteGroupId, Etas>& _etas,
            const PercentileRanks &_percentileRanks,
            IConcentrationCalculator &_concentrationCalculator,
            const ActiveMoiety *_activeMoiety,
            ComputingAborter *_aborter) = 0;

    virtual ~IAprioriPercentileCalculatorMulti() {}
};

class IAposterioriPercentileCalculatorMulti : public IPercentileCalculator
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
    /// \param _samples List of samples
    /// \param _percentileRanks List of percentiles ranks
    /// \param _aborter An aborter object allowing to abort the calculation
    /// \return The status of calculation
    virtual ComputingStatus calculate(
            PercentilesPrediction &_percentiles,
            const DateTime &_recordFrom,
            const DateTime &_recordTo,
            const GroupsIntakeSeries &_intakes,
            const GroupsParameterSetSeries &_parameters,
            const std::map<AnalyteGroupId, OmegaMatrix>& _omega,
            const std::map<AnalyteGroupId, std::unique_ptr<IResidualErrorModel> > &_residualErrorModel,
            const std::map<AnalyteGroupId, Etas>& _etas,
            const std::map<AnalyteGroupId, SampleSeries> &_samples,
            const PercentileRanks &_percentileRanks,
            IConcentrationCalculator &_concentrationCalculator,
            const ActiveMoiety *_activeMoiety,
            ComputingAborter *_aborter) = 0;

    virtual ~IAposterioriPercentileCalculatorMulti() {}
};

class PercentileCalculatorMultiBase
{
protected:

    ///
    /// \brief calculateActiveMoietyAndSort
    /// \param _percentiles percentiles calculated within the method
    /// \param _percentileRanks List of percentiles ranks
    /// \param _activeMoiety Active moiety that has to be calculated
    /// \param _analyteGroupIds Analyte group Ids for the active moiety
    /// \param _recordedIntakes List of intakes that have to be saved
    /// \param _nbPatients Number of patients during Monte Carlo
    /// \param _concentrations Concentrations calculated during Monte Carlo
    /// \param _simpleCalculator A single-analytegroup calculator
    /// \param _times The times calculated
    /// \return ComputingResult::Ok if everything went well
    ///
    ComputingStatus calculateActiveMoietyAndSort(PercentilesPrediction &_percentiles,
            const PercentileRanks &_percentileRanks,
            const ActiveMoiety *_activeMoiety,
            std::vector<AnalyteGroupId> _analyteGroupIds,
            IntakeSeries _recordedIntakes,
            unsigned int _nbPatients,
            std::vector<std::vector<std::vector<std::vector<Concentration> > > > &_concentrations,
            MonteCarloPercentileCalculatorBase &_simpleCalculator, std::vector<TimeOffsets> _times);

};

class AprioriPercentileCalculatorMulti : public IAprioriPercentileCalculatorMulti, public MonteCarloPercentileCalculatorBase, public PercentileCalculatorMultiBase
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
    /// \param _activeMoiety Active moiety that has to be calculated
    /// \param _aborter An aborter object allowing to abort the calculation
    /// \return The status of calculation
    ComputingStatus calculate(
            PercentilesPrediction &_percentiles,
            const DateTime &_recordFrom,
            const DateTime &_recordTo,
            const GroupsIntakeSeries &_intakes,
            const GroupsParameterSetSeries &_parameters,
            const std::map<AnalyteGroupId, OmegaMatrix>& _omega,
            const std::map<AnalyteGroupId, std::unique_ptr<IResidualErrorModel> > &_residualErrorModel,
            const std::map<AnalyteGroupId, Etas>& _etas,
            const PercentileRanks &_percentileRanks,
            IConcentrationCalculator &_concentrationCalculator,
            const ActiveMoiety *_activeMoiety,
            ComputingAborter *_aborter) override;

    ~AprioriPercentileCalculatorMulti() override {}
};


class AposterioriMonteCarloPercentileCalculatorMulti : public IAposterioriPercentileCalculatorMulti, public MonteCarloPercentileCalculatorBase, public PercentileCalculatorMultiBase
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
    /// \param _samples List of samples
    /// \param _percentileRanks List of percentiles ranks
    /// \param _aborter An aborter object allowing to abort the calculation
    /// \return The status of calculation
    ComputingStatus calculate(
            PercentilesPrediction &_percentiles,
            const DateTime &_recordFrom,
            const DateTime &_recordTo,
            const GroupsIntakeSeries &_intakes,
            const GroupsParameterSetSeries &_parameters,
            const std::map<AnalyteGroupId, OmegaMatrix>& _omega,
            const std::map<AnalyteGroupId, std::unique_ptr<IResidualErrorModel> > &_residualErrorModel,
            const std::map<AnalyteGroupId, Etas>& _etas,
            const std::map<AnalyteGroupId, SampleSeries> &_samples,
            const PercentileRanks &_percentileRanks,
            IConcentrationCalculator &_concentrationCalculator,
            const ActiveMoiety *_activeMoiety,
            ComputingAborter *_aborter) override;

    ~AposterioriMonteCarloPercentileCalculatorMulti() override {}
};

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_MONTECARLOPERCENTILECALCULATOR_H
