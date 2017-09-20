/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "montecarlopercentilecalculator.h"

#include "tucucore/definitions.h"

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
    TMP_UNUSED_PARAMETER(_percentiles);
    TMP_UNUSED_PARAMETER(_nbPoints);
    TMP_UNUSED_PARAMETER(_intakes);
    TMP_UNUSED_PARAMETER(_parameters);
    TMP_UNUSED_PARAMETER(_residualErrorModel);
    TMP_UNUSED_PARAMETER(_percentileRanks);
    TMP_UNUSED_PARAMETER(_etas);
    TMP_UNUSED_PARAMETER(_epsilons);
    TMP_UNUSED_PARAMETER(_curvelength);
    TMP_UNUSED_PARAMETER(_aborter);

    #if 0

    bool abort = false;

    std::vector<cxn_1d_t> _res;
    concs.clear();
    for (int i = 0; i < curvelength; ++i) {
        _res.push_back(cxn_1d_t(numpatients));
        concs.push_back(perc_set_t());
    }


    /*
     * Parallelize this for loop with some shared and some copied-to-each-thread-with-current-state (firstprivate) variables
     */

    int nbThreads = std::max(std::thread::hardware_concurrency(), (unsigned int)1);

    std::vector<std::thread> workers;
    for(int thread = 0;thread< nbThreads; thread++) {
        workers.push_back(std::thread([thread, numpatients, &abort, aborter, etas,
                                      curvelength,epsilons, parameters, intakes, _nbPoints,
                                      sigma, residual_error_type, &_res, nbThreads]()
        {


            cxn_1d_t _iconcs;
            time_1d_t _itimes;
            _iconcs.reserve(curvelength);
            _itimes.reserve(curvelength);

            int start = thread * (numpatients / nbThreads);
            int end = (thread + 1 ) * (numpatients / nbThreads);
            for (int i = start; i < end; ++i) {
                if (!abort) {
                    if ((aborter != nullptr) && (aborter->shouldAbort())) {
                        abort = true;
                    }

                    // Generate a random epsilon
                    deviation_t eps = epsilons(i);

                    ParametersSeries _iparams = parameters;
                    IntakeSeries _iinks = intakes;
                    _iconcs.clear();
                    _itimes.clear();
                    /*
                     * Call to apriori becomes population as its determined earlier in the parametersExtractor
                     */
                    ConcentrationCalculator::calculatePoints(_iconcs,_itimes, _nbPoints, _iinks, _iparams, etas[i], sigma, residual_error_type, eps, true);

                    for (int k = 0; k < curvelength; ++k) {
                        _res[k][i] = _iconcs[k];
                    }
                }
            }

        }));
    }
    std::for_each(workers.begin(), workers.end(), [](std::thread &t)
    {
        t.join();
    });

    if (abort) {
        return EXIT_DISCARDED;
    }

    /*
    * Parallelize this loop where quantiles are extracted.
    * The values at each time are sorted in increasing order, then
    * they are walked from low to high while counting, once
    * the count passes that of a quantile, the value is extracted
    * into the result (concs)
    */
#pragma omp parallel for
    for (int i = 0; i < _res.size(); ++i) {
        std::sort(_res[i].begin(), _res[i].end(), [&] (const double v1, const double v2) { return v1 < v2; });

        perc_set_t::const_iterator pit = percs.cbegin();
        for (int j = 0; j < numpatients; ++j) {
            if (pit != percs.cend() && j >= *pit / 100.0 * numpatients) {
                concs[i].push_back(_res[i][j]);
                pit++;
            }
        }
    }

    return EXIT_SUCCESS;
#endif

    return ProcessingResult::Failure;
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
    std::vector<Deviations> epsilons(1, Deviations(nbPatients, var_nor())); 

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
    TMP_UNUSED_PARAMETER(_intakes);
    TMP_UNUSED_PARAMETER(_parameters);
    TMP_UNUSED_PARAMETER(_omega);
    TMP_UNUSED_PARAMETER(_residualErrorModel);
    TMP_UNUSED_PARAMETER(_etas);
    TMP_UNUSED_PARAMETER(_samples);
    TMP_UNUSED_PARAMETER(_percentileRanks);
    TMP_UNUSED_PARAMETER(_aborter);

    // Activate Method1
#if 0
    QString statPath = "/home/ythoma/docs/ezechiel/stats/";

    percentileCurves.clear();
    /*
    * 1. posterior mode (resulting eta vector from calculating aposteriori) 'eta' in args
    */
    dy_vec_t _mean_etas(eta.size());
    for (int i = 0; i < _mean_etas.size(); ++i) {
        _mean_etas[i] = eta[i];
    }

    /*
    * 2. evaluate inverse of hessian of loglikelihood
    */

    const int _rank = omegaSize(omega);

    dy_vec_t _eta_min = dy_vec_t::Zero(omega.rows());
    dy_vec_t _eta_max = dy_vec_t::Zero(omega.rows());

    exportToFile(statPath + "omega.dat",omega);

    ConjugateGradientMethod _cgm(omega, sigma, samples, intakes, parameters, residual_error_type);
    _cgm.initBounds(omega, _eta_max, _eta_min);
    /*
    * Get working eta
    */
    error_vector_t _eta = eta;

    dy_mat_t _hessian(_eta.size(), _eta.size());
    simple_matrix_t sm_hess(_eta.size(), _eta.size());

    /*
    * Get the hessian
    */
    deriv2(_cgm, _eta, sm_hess.data);
    for (unsigned int i = 0; i < sm_hess.nbRows(); ++i) {
        for (unsigned int j = 0; j < sm_hess.nbColumns(); ++j) {
            _hessian(i,j) = sm_hess(i,j);
        }
    }

    exportToFile(statPath + "hessian.dat",_hessian);

    /*
    * Negative inverse
    */
    // We don't take the negative hessian because we minimize the negative log likelyhood
    dy_mat_t subomega = (_hessian).inverse();

    exportToFile(statPath + "subomega.dat",subomega);

    /*
    * Get lower cholesky of omega
    */
    Eigen::LLT<dy_mat_t> _subomega_llt(subomega);
    dy_mat_t _eta_lchol = _subomega_llt.matrixL().transpose();

    exportToFile(statPath + "eta_lchol.dat", _eta_lchol);

    /*
    * 3. draw initial sample from multivariate students t-dist w/ 1 degree freedom
    * A standard multivariate Student's t random vector can be written as a
    * multivariate normal vector whose covariance matrix is scaled by the reciprocal of a Gamma random variable
    * ( source: http://www.statlect.com/mcdstu1.htm )
    * generating the multivariate normal starts the same as mcarls
    */
    static boost::mt19937 _rng(static_cast<unsigned> (std::time(0)));

    /*
    * Get a gamma dist generator (note: this is the gamma distribution, not gamma function)
    */
    // Could be 2 for instance
    int student_liberty = 1;
    boost::random::student_t_distribution<> _stud(student_liberty);
    boost::variate_generator<boost::mt19937&,boost::random::student_t_distribution<> > var_t(_rng,_stud);
    // dy_vec_t _mvt_sample(_eta.size()); // multi-variate t-dist

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
             avecs(r,c) = var_t();
        }
    }

    exportToFile(statPath + "avecs.dat", avecs);

    bool abort = false;

    /*
    * Parallelizing this for loop with shared variables
    */
#pragma omp parallel for shared(_cgm, abort)
    for (int j = 0; j < nbSamples; ++j) {
        if (!abort) {
            if ((aborter != nullptr) && (aborter->shouldAbort())) {
                abort = true;
                /*
                * Broadcast the new state of abort to all threads
                * to make them skip the rest of their looping
                */
                #pragma omp flush (abort)
            }
            dy_vec_t avec = dy_vec_t::Zero(eta.size());

            // TOCHECK : voir si transpose est nécessaire et pourquoi
            avec = _mean_etas.transpose() + avecs.row(j) * _eta_lchol;

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
            * we need the distributions in boost::math rather than boost:: or boost::random
            * im using the multi-t-dist pdf directly from this source: http://www.statlect.com/mcdstu1.htm
            * because the multi-t-dist doesnt exist in boost

            * !!! this is the gamma function, dont use gamma dist
            *    boost::math::gamma_distribution<> _math_gam(1,1); !!!

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
            _r[j] =  exp(-_h_star - log(_g));
            _r[j] =  exp(-_h_star) / _g;
        }
    }
    if (abort) {
    //    EXLOG(QtDebugMsg, ezechiel::math::NOEZERROR, QString("Aborted stale calc: avail: %1").arg(aborter.available()));
        return EXIT_DISCARDED;
    }

    exportToFile(statPath + "r.dat", _r);

    /*
    * 5. calculate the weights
    */
    _w = _r / _r.sum();

    /*
    * 6. draw samples from discrete distribution using weights
    */
    boost::random::discrete_distribution<> _ddist(&_w(0), &_w(0) + _w.size());
    boost::variate_generator<boost::mt19937&,boost::random::discrete_distribution<> > var_discrete(_rng, _ddist);

    /*
    * this is just to get the number of points in curve
    */
    error_vector_t _err_at_w = _eta_samples[var_discrete()];

    double eps = 0.0;

    boost::normal_distribution<> _norm(0,1.0);
    boost::variate_generator<boost::mt19937&,boost::normal_distribution<> > var_nor(_rng, _norm);

    cxn_1d_t _forsize;
    ConcentrationCalculator::calculatePoints(_forsize,times, _nbPoints, intakes, parameters, _err_at_w, sigma, residual_error_type, eps);
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
        real_eta_samples[patient] = _eta_samples[var_discrete()];
        epsilons(patient) = var_nor();
    }

    return computePredictionsAndSortPercentiles(
                percentileCurves,
                times,
                _nbPoints,
                intakes,
                parameters,
                sigma,
                residual_error_type,
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

#if 0
    QString statPath = "/home/ythoma/docs/ezechiel/stats/";

    percentileCurves.clear();
    /*
    * 1. posterior mode (resulting eta vector from calculating aposteriori) 'eta' in args
    */
    dy_vec_t _mean_etas(eta.size());
    for (int i = 0; i < _mean_etas.size(); ++i) {
        _mean_etas[i] = eta[i];
    }

    /*
    * 2. evaluate inverse of hessian of loglikelihood
    */

    const int _rank = omegaSize(omega);

    dy_vec_t _eta_min = dy_vec_t::Zero(omega.rows());
    dy_vec_t _eta_max = dy_vec_t::Zero(omega.rows());

    exportToFile(statPath + "omega.dat",omega);

    ConjugateGradientMethod _cgm(omega, sigma, samples, intakes, parameters, residual_error_type);
    _cgm.initBounds(omega, _eta_max, _eta_min);
    /*
    * Get working eta
    */
    error_vector_t _eta = eta;

    dy_mat_t _hessian(_eta.size(), _eta.size());
    simple_matrix_t sm_hess(_eta.size(), _eta.size());

    /*
    * Get the hessian
    */
    deriv2(_cgm, _eta, sm_hess.data);
    for (unsigned int i = 0; i < sm_hess.nbRows(); ++i) {
        for (unsigned int j = 0; j < sm_hess.nbColumns(); ++j) {
            _hessian(i,j) = sm_hess(i,j);
        }
    }

    exportToFile(statPath + "hessian.dat",_hessian);

    /*
    * Negative inverse
    */
    // We don't take the negative hessian because we minimize the negative log likelyhood
    dy_mat_t subomega = (_hessian).inverse();

    exportToFile(statPath + "subomega.dat",subomega);

    /*
    * Get lower cholesky of omega
    */
    Eigen::LLT<dy_mat_t> _subomega_llt(subomega);
    dy_mat_t _eta_lchol = _subomega_llt.matrixL().transpose();

    exportToFile(statPath + "eta_lchol.dat", _eta_lchol);

    /**
      Only apply steps 1 and 2 of the Annex A of Aziz document (posteriori2.pdf)
      */

    pop_err_t newOmega(_rank,_rank);
    for(int i=0;i< _rank; i++)
        for(int j=0;j< _rank; j++)
            newOmega(i,j) = subomega(i,j);

    MonteCarloPercentileCalculator _mc; /* NOTE AprioriMonteCarlo calculator */
    return _mc.calculate(percentileCurves,times,_nbPoints,intakes,parameters, newOmega, sigma, eta, percs, aborter);

#endif

    return ProcessingResult::Failure;
} //AposterioriNormalApproximationMonteCarloPercentileCalculator::calculate


}
}
