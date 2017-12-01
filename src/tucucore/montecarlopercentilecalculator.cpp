/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "montecarlopercentilecalculator.h"
#include "concentrationcalculator.h"
#include "definitions.h"

#include <thread>
#include <Eigen/Cholesky>
#include <random>

namespace Tucuxi {
namespace Core {


MonteCarloPercentileCalculatorBase::MonteCarloPercentileCalculatorBase()
{
    /*
     * Here, hardcoded number of simulated patients
     * Aziz says this is an approximate number to assure a reasonable result
     * for most cases
     */
#if 0
    setNumberPatients(10000);
#else
    setNumberPatients(8); // for test: need to fix unit test
#endif
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
	IConcentrationCalculator &_concentrationCalculator,
        ProcessingAborter *_aborter)
{
    bool abort = false;
    unsigned int nbPatients = Tucuxi::Core::MonteCarloPercentileCalculatorBase::getNumberPatients();

    std::vector< std::vector<Concentrations> > concentrations;

    /* Reset percentile */
    (_percentiles.m_values).clear();

    /* Set the size of vector "concentrations" */
    for (unsigned int cycle = 0; cycle < _intakes.size(); cycle++) {

	std::vector< std::vector<Concentration> > vec;
	for (int point = 0; point < _nbPoints; point++) {
	    vec.push_back(std::vector<Concentration>(nbPatients));
	}
	concentrations.push_back(vec);
    }

    /* Set the size of vector "percentile" */
    for(unsigned percRankIdx = 0; percRankIdx < _percentileRanks.size(); percRankIdx++) {

	std::vector< std::vector<Concentration> > vec;
	for (unsigned int cycle = 0; cycle < _intakes.size(); cycle++) {
	    vec.push_back(std::vector<Concentration>(_nbPoints));
	}
	(_percentiles.m_values).push_back(vec);
    }

    /*
     * Parallelize this for loop with some shared and some copied-to-each-thread-with-current-state (firstprivate) variables
     */
    int nbThreads = std::max(std::thread::hardware_concurrency(), (unsigned int)1);

    std::vector<std::thread> workers;
    for(int thread = 0; thread < nbThreads; thread++) {

	/* Duplicate an IntakeSeries for avoid a possible problem with multi-thread */
        IntakeSeries newIntakes;
	cloneIntakeSeries(_intakes, newIntakes);

        workers.push_back(std::thread([thread, nbPatients, &abort, _aborter, _etas, _epsilons, _parameters, newIntakes, _nbPoints, &_residualErrorModel, &concentrations, nbThreads, &_concentrationCalculator, &_percentiles]()
        {
            /* 
	     * Get concentrations for each patients, allocation will be done in
	     * computeConcentrations
	     */
            ConcentrationPredictionPtr predictionPtr;
            predictionPtr = std::make_unique<Tucuxi::Core::ConcentrationPrediction>();

            int start = thread * (nbPatients / nbThreads);
            int end = (thread + 1) * (nbPatients / nbThreads);

		for (int patient = start; patient < end; patient++) {

		    if (!abort) {
			if ((_aborter != nullptr) && (_aborter->shouldAbort())) {
			    abort = true;
			}

		    /*
		     * Call to apriori becomes population as its determined earlier in the parametersExtractor
		     */
		    ConcentrationCalculator::ComputationResult computationResult = 
				_concentrationCalculator.computeConcentrations(
					    predictionPtr,
					    false, /* fix to "false": */
					    _nbPoints,
					    newIntakes,
					    _parameters,
					    _etas[patient],
					    _residualErrorModel,
					    _epsilons[patient],
					    false);

		    /* 
		     * save the series of result of 
		     * concentrations[cycle][nbPoints][patient] for each cycle
		     */
		    if (computationResult == ConcentrationCalculator::ComputationResult::Success) {

			// save concentrations to array of [patient] for using sort() function
			for (unsigned int cycle = 0; cycle < newIntakes.size(); cycle++) {

			    // save times only one time (when patient is equal to 0)
			    if (!patient) {
				(_percentiles.m_times).push_back((predictionPtr->getTimes())[cycle]);
			    }

			    for (int point = 0; point < _nbPoints; point++) {
				concentrations[cycle][point][patient] = (predictionPtr->getValues())[cycle][point];
			    }
			}
		    }
		} /* if (!abort) */

		// for debugging
#if 0
		char filename[30];
		sprintf(filename, "result%d.dat", patient);
		predictionPtr->streamToFile(filename);
#endif

	    } /* for (patient) */
        }
        ));


    } /* for (thread) */
    std::for_each(workers.begin(), workers.end(), [](std::thread &t)
    {
        t.join();
    });

    if (abort) {
        return ProcessingResult::Aborted;
    }

    // set m_ranks with input
    for (unsigned int percRankIdx = 0; percRankIdx < _percentileRanks.size(); percRankIdx++) {
	(_percentiles.m_ranks).push_back(_percentileRanks[percRankIdx]);
    }

    // sort and set percentile
    for (unsigned int cycle = 0; cycle < _intakes.size(); cycle++) {
	for (int point = 0; point < _nbPoints; point++) {

	    /* sort concentrations in increasing order at each time (at the cycle and at the point) */
	    std::sort(concentrations[cycle][point].begin(), concentrations[cycle][point].end(), [&] (const double v1, const double v2) { return v1 < v2; });

	    /* rebuild pecentile array [percentile][cycle][point] */
	    unsigned int percRankIdx = 0;
	    for (unsigned int sortPosition = 0; sortPosition < nbPatients; sortPosition++) {

	        if ((percRankIdx < _percentileRanks.size()) && ((sortPosition + 1) >= _percentileRanks[percRankIdx] / 100.0 * nbPatients)) {
		    _percentiles.appendPercentile(percRankIdx, cycle, point, concentrations[cycle][point][sortPosition]);

	            percRankIdx++;

		    // if percentile is not completely filled, it fills with the last one
		    if ((percRankIdx < _percentileRanks.size()) && ((sortPosition +1) == nbPatients)){ 
			while(percRankIdx < _percentileRanks.size()) {
			    _percentiles.appendPercentile(percRankIdx, cycle, point, concentrations[cycle][point][sortPosition]);
			    percRankIdx++;
			}
		    }
	        }
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
	IConcentrationCalculator &_concentrationCalculator,
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
    static std::random_device randomDevice;
    std::mt19937 rnGenerator(randomDevice());
    /*
     * The variables are normally distributed, we use boost standard normal,
     * then apply lower cholesky
     */
    std::normal_distribution<> normalDistribution(0, 1.0);

    Etas eta(omegaRank);
    Tucuxi::Core::ConcentrationPredictionPtr predictionPtr = std::make_unique<Tucuxi::Core::ConcentrationPrediction>();

    unsigned int nbPatients = getNumberPatients();

    /*
     * Generating the random numbers
     */
    EigenMatrix rands = EigenMatrix::Zero(nbPatients, omegaRank);
    
    std::vector<Deviations> epsilons(nbPatients, Deviations(_residualErrorModel.nbEpsilons(), normalDistribution(rnGenerator)));

    for (int row = 0; row < rands.rows(); row++) {
        for (int column = 0; column < rands.cols(); column++) {
            rands(row, column) = normalDistribution(rnGenerator);
        }
    }


    std::vector<Etas> etaSamples(nbPatients);

    for(unsigned int patient = 0; patient < nbPatients; patient++) {
        EigenVector matrixY = rands.row(patient);
        /*
         * Cholesky is applied here to get correlated random deviates
         */
        EigenVector matrixX = choleskyMatrix * matrixY;
        etaSamples[patient].assign(&matrixX(0), &matrixX(0) + omegaRank);

        for(unsigned int eta = 0; eta < _initialEtas.size(); eta++) {
            etaSamples[patient][eta] += _initialEtas[eta];
        }
    }

    return computePredictionsAndSortPercentiles
            (_percentiles,
             _nbPoints,
             _intakes,
             _parameters,
             _residualErrorModel,
             _percentileRanks,
             etaSamples,
             epsilons,
	     _concentrationCalculator,
             _aborter);
} //AprioriMonteCarloPercentileCalculator::calculate


void MonteCarloPercentileCalculatorBase::calculateSubomega(
        const OmegaMatrix &_omega,
        const Etas &_etas,
	Likelihood &_logLikelihood,
        EigenMatrix &_subomega)
{
    /*
    * posterior mode (resulting eta vector from calculating aposteriori) 'eta' in args
    */
    const Value* etasPtr = &_etas[0]; /* get _etas pointer */

    /*
    * evaluate inverse of hessian of loglikelihood
    */
    EigenVector eta_min = EigenVector::Zero(_omega.rows());
    EigenVector eta_max = EigenVector::Zero(_omega.rows());

    _logLikelihood.initBounds(_omega, eta_max, eta_min);

    /*
    * Get working eta
    */
    map2EigenVectorType etas(etasPtr, _etas.size());

    EigenMatrix hessian(etas.size(), etas.size());

    /*
    * Get the hessian
    */
    deriv2(_logLikelihood, (EigenVector&)etas, (EigenMatrix&)hessian);

    /*
    * Negative inverse
    */
    // We don't take the negative hessian because we minimize the negative log likelyhood
    _subomega = hessian.inverse();
}


AposterioriMonteCarloPercentileCalculator::AposterioriMonteCarloPercentileCalculator()
{

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
	IConcentrationCalculator &_concentrationCalculator,
        ProcessingAborter *_aborter)
{
    /* Return value from non-negative hessian matrix and loglikelihood */
    EigenMatrix subomega;
    Likelihood logLikelihood(_omega, _residualErrorModel, _samples, _intakes, _parameters, _concentrationCalculator);

    /* 
     * 1. Calculate hessian metrix and subomega
     */
    calculateSubomega(
                _omega,
                _etas,
		logLikelihood,
                subomega);

    /*
     * 2. Get meanEtas and lower cholesky of omega
     */
    /* TODO check Map functions correctly or not when test MC */
    const Value* etasPtr = &_etas[0]; /* get _etas pointer */

    map2EigenVectorType meanEtas(etasPtr, _etas.size());

    Eigen::LLT<EigenMatrix> subomegaLLT(subomega);
    EigenMatrix etaLowerChol = subomegaLLT.matrixL().transpose();

    /*
     * 3. draw initial sample from multivariate students t-dist w/ 1 degree freedom
     * A standard multivariate Student's t random vector can be written as a
     * multivariate normal vector whose covariance matrix is scaled by the reciprocal of a Gamma random variable
     * ( source: http://www.statlect.com/mcdstu1.htm )
     * generating the multivariate normal starts the same as mcarls
     */
    static std::random_device randomDevice;
    std::mt19937 rnGenerator(randomDevice());

    /* The variables are normally distributed, we use standard normal, then apply lower cholesky */
    int studentLiberty = 1;
    std::student_t_distribution<> standardNormalDist(studentLiberty);

    /* Now we start making a sample. How many samples to take? */
    const int samples = 100000;
    const int reSamples = 10000;

    std::vector<Etas> etaSamples(samples);
    /*
    * This is ratio for the ratios in step 4
    */
    EigenVector ratio(samples);
    EigenVector weight(samples);

    /*
    * Loading random numbers as omp doesnt like it
    */
    EigenMatrix avecs = EigenMatrix::Zero(samples, _etas.size());
    for (int row = 0; row < avecs.rows(); row++) {
        for (int column = 0; column < avecs.cols(); column++) {
            avecs(row,column) = standardNormalDist(rnGenerator);
        }
    }

    bool abort = false;

    /*
    * Parallelizing this for loop with shared variables
    */
    int nbThreads = std::max(std::thread::hardware_concurrency(), (unsigned int)1);

    std::vector<std::thread> workers;
    for(int thread = 0;thread < nbThreads; thread++) {
        workers.push_back(std::thread([thread, samples, &abort, _aborter, nbThreads, _etas, meanEtas, avecs, etaLowerChol,
                                      &etaSamples, logLikelihood, studentLiberty, subomega, &ratio]()
        {
            int start = thread * (samples / nbThreads);
            int end = (thread + 1 ) * (samples / nbThreads);
            for (int sample = start; sample < end; sample++) {
                if (!abort) {
                    if ((_aborter != nullptr) && (_aborter->shouldAbort())) {
                        abort = true;
                    }

                    EigenVector avec = meanEtas.transpose() + avecs.row(sample) * etaLowerChol;
                    Etas avec_mat;

                    for (unsigned int etasIdx = 0; etasIdx < _etas.size(); etasIdx++) {
                        avec_mat.push_back(avec[etasIdx]);
                    }

                    etaSamples[sample] = avec_mat;

                    /*
                    * 4. calculate the ratios for weighting
                    * this is h*
                    */
                    // Be careful: hstart should be a logLikelihood, however we are minimizing the
                    // negative loglikelyhood, so it is a negative h start.
                    Value hstart = logLikelihood.negativeLogLikelihood(avec_mat);

                    /*
                    * this is g
                    * Using the multi-t-dist pdf directly from this source: http://www.statlect.com/mcdstu1.htm
                    * because the multi-t-dist doesnt exist in boost
                    * using the multi-t dist from wikipedia
                    */
                    double v = studentLiberty;
                    double p = _etas.size();
                    double top = tgamma((v + p)/2);
                    double part2 = std::sqrt(subomega.determinant());
                    double part3 = tgamma(v / 2) * std::pow(v * 3.14159, p/2);
                    double part35 = 1 + 1/v * (avec - meanEtas).transpose() * subomega.inverse() * (avec - meanEtas);
                    double part4 = std::pow(part35,(v + p)/2);
                    double g = top / (part2 * part3 * part4);

                    /*
                    * Set the ratio
                    */
                    ratio[sample] =  exp(-hstart) / g;
                }
            }

        }));
    }
    std::for_each(workers.begin(), workers.end(), [](std::thread &t)
    {
        t.join();
    });

    if (abort) {
        return ProcessingResult::Aborted;
    }

    /*
    * 5. calculate the weights
    */
    weight = ratio / ratio.sum();

    /*
    * 6. draw samples from discrete distribution using weights
    */
    std::normal_distribution<> normalDistribution(0, 1.0);
    std::discrete_distribution<> discreteDistribution(&weight(0), &weight(0) + weight.size());

    std::vector<Deviations> epsilons(reSamples, Deviations(_residualErrorModel.nbEpsilons(), normalDistribution(rnGenerator)));
    std::vector<Etas> RealEtaSamples(reSamples);

    /* TODO: think about whether we can use push_back or not */
    for(int patient = 0; patient < reSamples; patient++) {
	RealEtaSamples[patient] = etaSamples[discreteDistribution(rnGenerator)];
    }

    return computePredictionsAndSortPercentiles(
                _percentiles,
                _nbPoints,
                _intakes,
                _parameters,
                _residualErrorModel,
                _percentileRanks,
		RealEtaSamples,
		epsilons,
		_concentrationCalculator,
		_aborter);

} // AposterioriMonteCarloPercentileCalculator::calculate

AposterioriNormalApproximationMonteCarloPercentileCalculator::AposterioriNormalApproximationMonteCarloPercentileCalculator()
{

}

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
	IConcentrationCalculator &_concentrationCalculator,
        ProcessingAborter *_aborter)
{
    /* Return value from non-negative hessian matrix and loglikelihood */
    EigenMatrix subomega;
    Likelihood logLikelihood(_omega, _residualErrorModel, _samples, _intakes, _parameters, _concentrationCalculator);

    /* 
     * 1. Calculate hessian metrix and subomega
     */
    calculateSubomega(
                _omega,
                _etas,
		logLikelihood,
                subomega);

    /*
    * Only apply steps 1 and 2 of the Annex A of Aziz document (posteriori2.pdf)
    */
    AprioriMonteCarloPercentileCalculator aprioriMC;
    return aprioriMC.calculate(
		_percentiles,
		_nbPoints,
		_intakes,
		_parameters,
		subomega,
		_residualErrorModel,
		_etas,
		_percentileRanks,
		_concentrationCalculator,
		_aborter);
	    
} //AposterioriNormalApproximationMonteCarloPercentileCalculator::calculate


}
}
