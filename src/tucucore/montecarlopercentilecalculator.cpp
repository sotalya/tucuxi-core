/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "montecarlopercentilecalculator.h"
#include "likelihood.h"
#include "concentrationcalculator.h"
#include "definitions.h"

#include <thread>
#include <Eigen/Cholesky>
//#include <boost/math/distributions/normal.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/variate_generator.hpp>

namespace Tucuxi {
namespace Core {


MonteCarloPercentileCalculatorBase::MonteCarloPercentileCalculatorBase()
{
    /*
     * Here, hardcoded number of simulated patients
     * Aziz says this is an approximate number to assure a reasonable result
     * for most cases
     */
    setNumberPatients(10000);
}

IPercentileCalculator::ProcessingResult MonteCarloPercentileCalculatorBase::computePredictionsAndSortPercentiles(
        PercentilesPrediction &_percentiles,
        const int _nbPoints,
        const IntakeSeries &_intakes,
        const ParameterSetSeries &_parameters,
        const IResidualErrorModel &_residualErrorModel,
        const PercentileRanks &_percentileRanks,
        const std::vector<Etas> _etas,
        const std::vector<Deviations> _epsilons,
        int _curvelength,
        ProcessingAborter *_aborter)
{
    bool abort = false;
    unsigned int nbPatients = Tucuxi::Core::MonteCarloPercentileCalculatorBase::getNumberPatients();

    std::vector<Concentrations> concentrations;

    // TODO: check whether reset is needed or not
#if 0
    (_percentiles.getValues()).clear();
    for (int i = 0; i < _curvelength; ++i) {
        concentrations.push_back(std::vector<Concentrations>(nbPatients));
        (_percentiles.getValue()).push_back(PercentileRanks());
    }
#endif

    /*
     * Parallelize this for loop with some shared and some copied-to-each-thread-with-current-state (firstprivate) variables
     */
    int nbThreads = std::max(std::thread::hardware_concurrency(), (unsigned int)1);

    std::vector<std::thread> workers;
    for(int thread = 0; thread < nbThreads; thread++) {

    /* TODO: after modifying header file, need to activate following code */
#if 0
        workers.push_back(std::thread([thread, nbPatients, &abort, _aborter, _etas, _curvelength, _epsilons, _parameters, _intakes, _nbPoints, _residualErrorModel, &concentrations, nbThreads]()
#endif
        {
	    /* get concentrations for each patients */
	    ConcentrationPredictionPtr predictionPtr;
            std::vector<Time> times;

            (predictionPtr->getValues()).reserve(_curvelength);
            times.reserve(_curvelength);

            int start = thread * (nbPatients / nbThreads);
            int end = (thread + 1) * (nbPatients / nbThreads);
            for (int i = start; i < end; ++i) {
                if (!abort) {
                    if ((_aborter != nullptr) && (_aborter->shouldAbort())) {
                        abort = true;
                    }

                    (predictionPtr->getValues()).clear();
                    times.clear();

                    /*
                     * Call to apriori becomes population as its determined earlier in the parametersExtractor
                     */
		    /* TODO: after modifying header file, need to activate following code */
		    #if 0
                    ProcessingResult processingResult = 
			Tucuxi::Core::IConcentrationCalculator::computeConcentrations(
						predictionPtr, 
						false, /* fix to "false": */
						_nbPoints,
						_intakes,
						_parameters,
						_etas[i],
						_residualErrorModel,
						_epsilons[i],
						false);
		    #else
                    ProcessingResult processingResult = ProcessingResult::Success;
		    #endif

                    if (processingResult == ProcessingResult::Success) {
                        for (int k = 0; k < _curvelength; ++k) {
			    /* TODO: temporary code. figugre out whether it is correct or not */
#if 0
                            concentrations[k][i] = (predictionPtr->getValues())[k];
#else
                            concentrations[k][i] = (predictionPtr->getValues())[k][i];
#endif
                        }
                    }
                }
            }
        }
    /* TODO: after modifying header file, need to activate following code */
#if 0
	));
#endif

    }
    std::for_each(workers.begin(), workers.end(), [](std::thread &t)
    {
        t.join();
    });

    if (abort) {
        return ProcessingResult::Aborted;
    }

/*
* Parallelize this loop where quantiles are extracted.
* The values at each time are sorted in increasing order, then
* they are walked from low to high while counting, once
* the count passes that of a quantile, the value is extracted
* into the result (_percentiles.getValues())
*/
#if 0
#pragma omp parallel for
#endif
    for (unsigned int i = 0; i < concentrations.size(); ++i) {
        std::sort(concentrations[i].begin(), concentrations[i].end(), [&] (const double v1, const double v2) { return v1 < v2; });

        PercentileRanks::const_iterator pit = _percentileRanks.cbegin();
        for (unsigned int j = 0; j < nbPatients; ++j) {
            if (pit != _percentileRanks.cend() && j >= *pit / 100.0 * nbPatients) {
		/* TODO: temporary code. figugre out whether it is correct or not */
#if 0
                (_percentiles.getValues())[i].push_back(concentrations[i][j]);
#endif
                pit++;
            }
        }
    }

    return ProcessingResult::Success;

} //MonteCarloPercentileCalculatorBase::computePredictionsAndSortPercentiles


AprioriMonteCarloPercentileCalculator::AprioriMonteCarloPercentileCalculator()
{

}


IPercentileCalculator::ProcessingResult AprioriMonteCarloPercentileCalculator::calculate(
        PercentilesPrediction &_percentiles,
        const int _nbPoints,
        const IntakeSeries &_intakes,
        const ParameterSetSeries &_parameters,
        const OmegaMatrix& _omega,
        const IResidualErrorModel &_residualErrorModel,
        const Etas& _initialEtas,
        const PercentileRanks &_percentileRanks,
        ProcessingAborter *_aborter)
{
    EigenMatrix choleskyMatrix; // Cholesky matrix after decomposition
    EigenMatrix errorMatrix; // Error matrix of both omega and sigma combined (inter and intra errors)

    /*
     * Extracting the variances from error matrix into vector
     */
    int omegaRank = omegaSize(_omega);
    errorMatrix = _omega;

    choleskyMatrix = EigenMatrix::Zero(omegaRank, omegaRank);

    /*
     * Take the lower cholesky to generate correlated normal random deviates
     */
    Eigen::LLT<EigenMatrix> llt(errorMatrix);
    choleskyMatrix = llt.matrixL().transpose();

    /*
     * Static random number generator
     */
    static boost::mt19937 rng(static_cast<unsigned>(std::time(0)));
    /*
     * The variables are normally distributed, we use boost standard normal,
     * then apply lower cholesky
     */
    boost::normal_distribution<> norm(0, 1.0);
    boost::variate_generator<boost::mt19937&, boost::normal_distribution < > > var_nor(rng, norm);

    Etas eta(omegaRank);
    Tucuxi::Core::ConcentrationPredictionPtr predictionPtr = std::make_unique<Tucuxi::Core::ConcentrationPrediction>();

    /*
     * This call to apriori is to get the times and determine a better-than-initial
     * fixed density to be used for all the simulated patients
     */
    /* TODO: remove following code. After adding code for calculating size */
#if  0
    ConcentrationCalculator::computeConcentrations(predictionPtr, isAll, _nbPoints, _intakes, _parameters, eta, _residualErrorModel);
#endif

    unsigned int nbPatients = Tucuxi::Core::IAprioriPercentileCalculator::getNumberPatients();
    /* 
    TODO: remove following code. From ezechiel, not used in the function
    And also think about remove curvelength after creating thread and factorizing code of sorting
    */
#if 0
    _percentiles.clear();
    for (int i = 0; i < predictionPtr.size(); ++i) {
        results.push_back(EigenMatrix(nbPatients));
        concs.push_back(perc_set_t());
    }
#endif

    const int curvelength = (predictionPtr->getTimes()).size();

    /*
     * Generating the random numbers
     */
    EigenMatrix rands = EigenMatrix::Zero(nbPatients, omegaRank);
    
    /* TODO: 
    The size of vector epsilons can be changed depends on the implementation
    Currently, epsilons[1][nbPatients] is initialised with normalised random number.
    */
#if 0
    std::vector<Deviations> epsilons(_residualErrorModel.nbEpsilons(), Deviations(nbPatients, var_nor())); 
#else
    std::vector<Deviations> epsilons(1, Deviations(nbPatients, var_nor()));
#endif

    for (int row = 0; row < rands.rows(); ++row) {
        //epsilons(0, (row))= var_nor();

        for (int column = 0; column < rands.cols(); ++column) {
             rands(row, column) = var_nor();
        }
    }


    std::vector<Etas> eta_samples(nbPatients);

    for(unsigned int patient = 0; patient < nbPatients; ++patient) {
            EigenVector matrixY = rands.row(patient);
        /*
         * Cholesky is applied here to get correlated random deviates
         */
        EigenVector matrixX = choleskyMatrix * matrixY;
        eta_samples[patient].assign(&matrixX(0), &matrixX(0) + omegaRank);

        for(unsigned int eta = 0; eta < _initialEtas.size(); eta++) {
            eta_samples[patient][eta] += _initialEtas[eta];
	}
    }

    return computePredictionsAndSortPercentiles
	    (_percentiles, 
	    _nbPoints, 
	    _intakes, 
	    _parameters, 
	    _residualErrorModel, 
	    _percentileRanks, 
	    eta_samples, 
	    epsilons, 
	    curvelength, 
	    _aborter);
} //AprioriMonteCarloPercentileCalculator::calculate


void IPercentileCalculator::calculateSubomega(
        const IntakeSeries &_intakes,
        const ParameterSetSeries &_parameters,
	const OmegaMatrix &_omega,
	const IResidualErrorModel &_residualErrorModel,
	const Etas &_etas,
	const SampleSeries &_samples,
	EigenMatrix &_subomega)
{
    /*
    * posterior mode (resulting eta vector from calculating aposteriori) 'eta' in args
    */
    /* TODO check Map functions correctly or not when test MC */
    const Value* etasPtr = &_etas[0]; /* get _etas pointer */

    map2EigenVectorType mean_etas(etasPtr, _etas.size());

    /*
    * evaluate inverse of hessian of loglikelihood
    */
    EigenVector eta_min = EigenVector::Zero(_omega.rows());
    EigenVector eta_max = EigenVector::Zero(_omega.rows());

    Tucuxi::Core::ConcentrationCalculator concentrationCalculator;

    Likelihood likelihood(_omega, _residualErrorModel, _samples, _intakes, _parameters, concentrationCalculator);
    likelihood.initBounds(_omega, eta_max, eta_min);

    /*
    * Get working eta
    */
    map2EigenVectorType etas(etasPtr, _etas.size());

    EigenMatrix hessian(etas.size(), etas.size());

    /*
    * Get the hessian
    */
    deriv2(likelihood, (EigenVector&)etas, (EigenMatrix&)hessian);

    /*
    * Negative inverse
    */
    // We don't take the negative hessian because we minimize the negative log likelyhood
    _subomega = hessian.inverse();
}


IPercentileCalculator::ProcessingResult AposterioriMonteCarloPercentileCalculator::calculate(
        PercentilesPrediction &_percentiles,
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
    TMP_UNUSED_PARAMETER(_percentileRanks);
    TMP_UNUSED_PARAMETER(_aborter);

    /* Return value from non-negative hessian matrix */
    EigenMatrix subomega;

    /* calculate hessian metrix and subomega */
    Tucuxi::Core::IPercentileCalculator::calculateSubomega(
        _intakes,
        _parameters,
	_omega,
	_residualErrorModel,
	_etas,
	_samples,
	subomega);

    /*
    * Get lower cholesky of omega
    */
    Eigen::LLT<EigenMatrix> _subomega_llt(subomega);
    EigenMatrix eta_lchol = _subomega_llt.matrixL().transpose();

#if 0
    /* TODO: figure out whether we need to clear or not */
    (_percentiles.getValues()).clear();

    /*
    * 3. draw initial sample from multivariate students t-dist w/ 1 degree freedom
    * A standard multivariate Student's t random vector can be written as a
    * multivariate normal vector whose covariance matrix is scaled by the reciprocal of a Gamma random variable
    * ( source: http://www.statlect.com/mcdstu1.htm )
    * generating the multivariate normal starts the same as mcarls
    */

    static std::random_device rd;
    std::mt19937 gen(rd());

    /*
     * The variables are normally distributed, we use standard normal,
     * then apply lower cholesky
     */
    // Could be 2 for instance
    int student_liberty = 1;
    std::student_t_distribution<> var_t(student_liberty);


    /*
    * Now we start making a sample. How many samples to take?
    */
    const int nbSamples = 100000; // Rename : samples
    const int nbReSamples = 10000;  // Rename : reSamples

    std::vector<error_vector_t> _eta_samples(nbSamples);
    /*
    * This is r for the ratios in step 4
    */
    dy_vec_t _r(nbSamples);
    dy_vec_t _w(nbSamples);


    /*
    * Loading random numbers as omp doesnt like it
    */
    dy_mat_t avecs = dy_mat_t::Zero(nbSamples, eta.size());
    for (int r = 0; r < avecs.rows(); ++r) {
        for (int c = 0; c < avecs.cols(); ++c) {
             avecs(r,c) = var_t(gen);
        }
    }

    exportToFile(statPath + "avecs.dat", avecs);

    bool abort = false;

    /*
    * Parallelizing this for loop with shared variables
    */
    int nbThreads = std::max(std::thread::hardware_concurrency(), (unsigned int)1);

    std::vector<std::thread> workers;
    for(int thread = 0;thread< nbThreads; thread++) {
        workers.push_back(std::thread([thread, nbSamples, &abort, aborter, nbThreads, eta, _mean_etas, avecs, eta_lchol,
                                      &_eta_samples, _cgm, student_liberty, subomega, &_r]()
        {
            int start = thread * (nbSamples / nbThreads);
            int end = (thread + 1 ) * (nbSamples / nbThreads);
            for (int j = start; j < end; ++j) {
                if (!abort) {
                    if ((aborter != nullptr) && (aborter->shouldAbort())) {
                        abort = true;
                    }

                    //dy_vec_t avec = dy_vec_t::Zero(eta.size());

                    // TOCHECK : voir si transpose est nécessaire et pourquoi
                    dy_vec_t avec = _mean_etas.transpose() + avecs.row(j) * eta_lchol;

                    error_vector_t _avec_mat;

                    for (int i = 0; i < eta.size(); ++i) {
                        // Removing the boundaries makes the step disappear
                    //    _avec_mat.push_back(qBound(_eta_min[i], avec[i], _eta_max[i]));
                        _avec_mat.push_back(avec[i]);
                    }

                    _eta_samples[j] = _avec_mat;

                    /*
                    * 4. calculate the ratios for weighting
                    * this is h*
                    */
                    // Be careful: hstart should be a logLikelihood, however we are minimizing the
                    // negative loglikelyhood, so it is a negative h start.
                    loglikelihood_t _h_star = _cgm.negativeLogLikelihood(_avec_mat);

                    /*
                    * this is g
                    * Using the multi-t-dist pdf directly from this source: http://www.statlect.com/mcdstu1.htm
                    * because the multi-t-dist doesnt exist in boost

                    * using the multi-t dist from wikipedia
                    */
                    double v = student_liberty;
                    double p = eta.size();
                    double top = tgamma((v + p)/2);
                    double part2 = std::sqrt(subomega.determinant());
                    double part3 = tgamma(v / 2) * std::pow(v * 3.14159, p/2);
                    double part35 = 1 + 1/v * (avec - _mean_etas).transpose() * subomega.inverse() * (avec - _mean_etas);
                    double part4 = std::pow(part35,(v + p)/2);
                    double _g = top / (part2 * part3 * part4);

                    /*
                    * Set the ratio
                    */
                    //_r[j] = -_h_star / _g;
                    // _r[j] =  exp(-_h_star - log(_g));
                    _r[j] =  exp(-_h_star) / _g;
                }
            }

        }));
    }
    std::for_each(workers.begin(), workers.end(), [](std::thread &t)
    {
        t.join();
    });

    if (abort) {
    //    EXLOG(QtDebugMsg, ezechiel::math::NOEZERROR, QString("Aborted stale calc: avail: %1").arg(aborter.available()));
        return ProcessingResult::Aborted;
    }

    /*
    * 5. calculate the weights
    */
    _w = _r / _r.sum();

    /*
    * 6. draw samples from discrete distribution using weights
    */
    std::discrete_distribution<> var_discrete(&_w(0), &_w(0) + _w.size());

    /*
    * this is just to get the number of points in curve
    */
    error_vector_t _err_at_w = _eta_samples[var_discrete(gen)];

    double eps = 0.0;

    std::normal_distribution<> var_nor(0,1.0);

    cxn_1d_t _forsize;
    ConcentrationCalculator::calculatePoints(_forsize,times, _nbPoints, intakes, parameters, _err_at_w, _residualErrorModel, eps);
    cxn_2d_t individualPredictions;
    for (unsigned int i = 0; i < _forsize.size(); ++i) {
        individualPredictions.push_back(cxn_1d_t(nbReSamples));
        percentileCurves.push_back(perc_set_t(percs.size()));
    }

    /*
    * this is where we do the monte carlo
    * omp doesnt like the caches
    */
    const int curvelength = _forsize.size();
    cxn_1d_t _iconcs;
    time_1d_t _itimes;
    _iconcs.reserve(curvelength);
    _itimes.reserve(curvelength);

    dy_vec_t epsilons = dy_vec_t::Zero(nbReSamples);
    std::vector<error_vector_t> real_eta_samples(nbReSamples);

    for(int patient = 0; patient < nbReSamples; ++patient) {
        real_eta_samples[patient] = _eta_samples[var_discrete(gen)];
        epsilons(patient) = var_nor(gen);
    }

    return computePredictionsAndSortPercentiles(
                percentileCurves,
                times,
                _nbPoints,
                intakes,
                parameters,
                _residualErrorModel,
                percs,
                aborter,
                nbReSamples,
                real_eta_samples,
                epsilons,
                curvelength);

#endif

    return ProcessingResult::Failure;
} // AposterioriMonteCarloPercentileCalculator::calculate


IPercentileCalculator::ProcessingResult AposterioriNormalApproximationMonteCarloPercentileCalculator::calculate(
        PercentilesPrediction &_percentiles,
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
    /* Return value from calculateSubomega */
    EigenMatrix subomega;

    /* calculate hessian metrix and subomega */
    Tucuxi::Core::IPercentileCalculator::calculateSubomega(
        _intakes,
        _parameters,
	_omega,
	_residualErrorModel,
	_etas,
	_samples,
	subomega);

    /* TODO: figure out whether we need to clear or not */
#if 0
    (_percentiles.getValues()).clear();
#endif

    /*
    * Only apply steps 1 and 2 of the Annex A of Aziz document (posteriori2.pdf)
    */
    AprioriMonteCarloPercentileCalculator aprioriMC;
    return aprioriMC.calculate
	    (_percentiles, 
	    _nbPoints,
	    _intakes,
	    _parameters,
	    subomega,
	    _residualErrorModel,
	    _etas,
	    _percentileRanks,
	    _aborter);

} //AposterioriNormalApproximationMonteCarloPercentileCalculator::calculate


}
}
