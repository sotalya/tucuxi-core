/*
* Copyright (C) 2017 Tucuxi SA
*/

#include <thread>
#include <Eigen/Cholesky>
#include <random>
#include <time.h>
#include <mutex>

#include "definitions.h"
#include "concentrationcalculator.h"
#include "montecarlopercentilecalculator.h"
#include "percentilesprediction.h"

namespace Tucuxi {
namespace Core {

ComputingAborter::~ComputingAborter()
{

}

unsigned int MonteCarloPercentileCalculatorBase::sm_nbPatients = 0;

MonteCarloPercentileCalculatorBase::MonteCarloPercentileCalculatorBase()
{
    // Here, hardcoded number of simulated patients
    // Aziz says this is an approximate number to assure a reasonable result for most cases
#if 1
    if (sm_nbPatients != 0) {
        setNumberPatients(sm_nbPatients);
    }
    else {
        setNumberPatients(10000);
    }
#else
    setNumberPatients(8); // For test: need to fix unit test
#endif
}


#define MULTITHREADEDSORT
#define OPTIMALMULTI

#ifndef MULTITHREADEDSORT

// The following comparators are used in variants of sorting
class Compa{
    const std::vector<double>& m_values;
public:
    Compa(const std::vector<double>& _values) : m_values(_values){}
    bool operator()(int t1, int t2) const {
        return (m_values[t1] < m_values[t2]);
    }
};


class CompPred{
    int m_cycle;
    int m_point;
public:
    CompPred(int _cycle, int _point) : m_cycle(_cycle), m_point(_point){}
    bool operator()(ConcentrationPrediction* t1, ConcentrationPrediction* t2) const {
        return (t2->getValues())[m_cycle][m_point] < (t1->getValues())[m_cycle][m_point];
    }
};

void sortPercentiles(std::vector<ConcentrationPrediction*> &_predictions, int _cycle, int _point)
{
    // Sort and set percentile
            std::sort(_predictions.begin(), _predictions.end(), CompPred(_cycle,_point));
}

#endif // MULTITHREADEDSORT

IPercentileCalculator::ComputingResult MonteCarloPercentileCalculatorBase::computePredictionsAndSortPercentiles(
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
        ComputingAborter *_aborter)
{
//    auto start = std::chrono::steady_clock::now();

    bool abort = false;
    unsigned int nbPatients = Tucuxi::Core::MonteCarloPercentileCalculatorBase::getNumberPatients();
    std::vector<TimeOffsets> times;

    IntakeSeries recordedIntakes;
    selectRecordedIntakes(recordedIntakes, _intakes, _recordFrom, _recordTo);

    std::vector< std::vector< std::vector<Concentration> > > concentrations;

    // Set the size of vector "concentrations"
    for (unsigned int cycle = 0; cycle < recordedIntakes.size(); cycle++) {
        std::vector< std::vector<Concentration> > vec;
        for (int point = 0; point < recordedIntakes.at(cycle).getNbPoints(); point++) {
            vec.push_back(std::vector<Concentration>(nbPatients));
        }
        concentrations.push_back(vec);
    }

//    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::steady_clock::now( ) - start );
//    std::cout << "milliseconds for preparation: " << elapsed.count( ) << '\n';
//    start = std::chrono::steady_clock::now();

    // Parallelize this for loop with some shared and some copied-to-each-thread-with-current-state (firstprivate) variables
    int nbThreads = std::max(std::thread::hardware_concurrency(), (unsigned int)1);

    std::vector<std::thread> workers;
    for(int thread = 0; thread < nbThreads; thread++) {
        // Duplicate an IntakeSeries for avoid a possible problem with multi-thread
        IntakeSeries newIntakes;
        cloneIntakeSeries(_intakes, newIntakes);

        workers.push_back(std::thread([thread, nbPatients, &abort, _aborter,
                                      _etas, _epsilons, _parameters,
                                      newIntakes, &_residualErrorModel, &times,
                                      &concentrations, nbThreads,
                                      &_concentrationCalculator, _recordFrom,
                                      _recordTo, recordedIntakes]()
        {

            int start = thread * (nbPatients / nbThreads);
            int end = (thread + 1) * (nbPatients / nbThreads);

            for (int patient = start; patient < end; patient++) {
                if (!abort) {
                    if ((_aborter != nullptr) && (_aborter->shouldAbort())) {
                        abort = true;
                        return;
                    }


                    // Get concentrations for each patients, allocation will be done in computeConcentrations
                    // Be carefull with this pointer. If declared at the beginning of the thread, then
                    // only a single computation is stored instead of (end-start)
                    ConcentrationPredictionPtr predictionPtr;
                    predictionPtr = std::make_unique<Tucuxi::Core::ConcentrationPrediction>();

                    // Call to apriori becomes population as its determined earlier in the parametersExtractor
                    ComputationResult computationResult = _concentrationCalculator.computeConcentrations(
                                predictionPtr,
                                false, // fix to "false"
                                _recordFrom,
                                _recordTo,
                                newIntakes,
                                _parameters,
                                _etas[patient],
                                _residualErrorModel,
                                _epsilons[patient],
                                false);


                    // Save the series of result of concentrations[cycle][nbPoints][patient] for each cycle
                    if (computationResult == ComputationResult::Success) {

                        // Save concentrations to array of [patient] for using sort() function
                        for (unsigned int cycle = 0; cycle < recordedIntakes.size(); cycle++) {

                            int cyclePoints = recordedIntakes.at(cycle).getNbPoints();

                            // Save times only one time (when patient is equal to 0)
                            if (patient == 0) {
                                times.push_back((predictionPtr->getTimes())[cycle]);
                            }

                            for (int point = 0; point < cyclePoints; point++) {
                                concentrations[cycle][point][patient] = (predictionPtr->getValues())[cycle][point];
                            }
                        }
                    }

                }
#if 0
                // Debugging
                char filename[30];
                sprintf(filename, "result%d.dat", patient);
                predictionPtr->streamToFile(filename);
#endif
            }
        }));
    }

    std::for_each(workers.begin(), workers.end(), [](std::thread &t) {
        t.join();
    });

//    elapsed = std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::steady_clock::now( ) - start );
//    std::cout << "milliseconds for predictions calculation : " << elapsed.count( ) << '\n';
//    start = std::chrono::steady_clock::now();

    if (abort) {
        return ComputingResult::Aborted;
    }

    // Init our percentile prediction object
    _percentiles.init(_percentileRanks, times, recordedIntakes);

    std::vector<int> positions;
    for (unsigned int percRankIdx = 0; percRankIdx < _percentileRanks.size(); percRankIdx++) {
        positions.push_back( ((double) _percentileRanks[percRankIdx]) / 100.0
                             * ((double) nbPatients));
    }


#ifdef MULTITHREADEDSORT

#ifdef OPTIMALMULTI

    std::vector<std::thread> sortWorkers;
    std::mutex mutex;
    unsigned int currentCycle = 0;
    unsigned int currentPoint = 0;
    unsigned int nbCycles = recordedIntakes.size();
    for(int thread = 0; thread < nbThreads; thread++) {

        sortWorkers.push_back(std::thread([thread, nbPatients, &abort, _aborter,
                                      _etas, _epsilons, _parameters, &times,
                                      &concentrations, nbThreads,
                                      &_concentrationCalculator, _recordFrom,
                                      _recordTo, recordedIntakes, positions, _percentileRanks, &currentPoint,
                                          &_percentiles, &mutex, &currentCycle, nbCycles]()
        {
            bool cont = true;
            unsigned int cycle;
            unsigned int point;

            while (cont) {
                mutex.lock();
                if (currentCycle >= nbCycles) {
                    cont = false;
                    mutex.unlock();
                    break;
                }
                else {
                    point = currentPoint;
                    cycle = currentCycle;
                    if ((static_cast<int>(point)) == recordedIntakes.at(cycle).getNbPoints() - 1) {
                        currentPoint = 0;
                        currentCycle ++;
                        if (currentCycle >= nbCycles) {
                            cont = false;
                        }
                    }
                    else {
                        currentPoint ++;
                    }
                }
                mutex.unlock();
                // Sort and set percentile


                // Sort concentrations in increasing order at each time (at the cycle and at the point)
                std::sort(concentrations[cycle][point].begin(), concentrations[cycle][point].end(), [&] (const double v1, const double v2) { return v1 < v2; });

                for (unsigned int percRankIdx = 0; percRankIdx < _percentileRanks.size(); percRankIdx++) {
                    int pos = positions[percRankIdx];
                    double conc = concentrations[cycle][point][pos];
                    _percentiles.appendPercentile(percRankIdx, cycle, point, conc);

                }

                if (!cont) {
                    break;
                }
            }

        }));
    }

    std::for_each(sortWorkers.begin(), sortWorkers.end(), [](std::thread &t) {
        t.join();
    });

#else

    std::vector<std::thread> sortWorkers;
    std::mutex mutex;
    int nextCycle = 0;
    int nbCycles = recordedIntakes.size();
    for(int thread = 0; thread < nbThreads; thread++) {

        sortWorkers.push_back(std::thread([thread, nbPatients, &abort, _aborter,
                                      _etas, _epsilons, _parameters, &times,
                                      &concentrations, nbThreads,
                                      &_concentrationCalculator, _recordFrom,
                                      _recordTo, recordedIntakes, positions, _percentileRanks, &_percentiles, &mutex, &nextCycle, nbCycles]()
        {
            bool cont = true;
            unsigned int cycle;

            while (cont) {
                mutex.lock();
                cycle = nextCycle;
                nextCycle ++;
                mutex.unlock();
                if (nextCycle > nbCycles) {
                    break;
                }
                // Sort and set percentile
                    for (unsigned int point = 0; point < static_cast<unsigned int>(recordedIntakes.at(cycle).getNbPoints()); point++) {

                        // Sort concentrations in increasing order at each time (at the cycle and at the point)
                        std::sort(concentrations[cycle][point].begin(), concentrations[cycle][point].end(), [&] (const double v1, const double v2) { return v1 < v2; });

                        for (unsigned int percRankIdx = 0; percRankIdx < _percentileRanks.size(); percRankIdx++) {
                            int pos = positions[percRankIdx];
                            double conc = concentrations[cycle][point][pos];
                            _percentiles.appendPercentile(percRankIdx, cycle, point, conc);
                    }

                }
            }

        }));
    }

    std::for_each(sortWorkers.begin(), sortWorkers.end(), [](std::thread &t) {
        t.join();
    });

#endif // OPTIMALMULTI

#else
#ifdef SORT1
    std::vector<int> pointers(nbPatients);
    for(int i = 0; i < nbPatients; i++) {
        pointers[i] = i;
    }

    // Sort and set percentile
    for (unsigned int cycle = 0; cycle < recordedIntakes.size(); cycle++) {
        for (unsigned int point = 0; point < static_cast<unsigned int>(recordedIntakes.at(cycle).getNbPoints()); point++) {


            // Sort concentrations in increasing order at each time (at the cycle and at the point)
            std::sort(pointers.begin(), pointers.end(), Compa(concentrations[cycle][point]));

            for (unsigned int percRankIdx = 0; percRankIdx < _percentileRanks.size(); percRankIdx++) {
                int pos = positions[percRankIdx];
                double conc = concentrations[cycle][point][pointers[pos]];
                _percentiles.appendPercentile(percRankIdx, cycle, point, conc);
            }
        }
    }
#else

    // Sort and set percentile
    for (unsigned int cycle = 0; cycle < recordedIntakes.size(); cycle++) {
        for (unsigned int point = 0; point < static_cast<unsigned int>(recordedIntakes.at(cycle).getNbPoints()); point++) {

            // Sort concentrations in increasing order at each time (at the cycle and at the point)
            std::sort(concentrations[cycle][point].begin(), concentrations[cycle][point].end(), [&] (const double v1, const double v2) { return v1 < v2; });

            for (unsigned int percRankIdx = 0; percRankIdx < _percentileRanks.size(); percRankIdx++) {
                int pos = positions[percRankIdx];
                double conc = concentrations[cycle][point][pos];
                _percentiles.appendPercentile(percRankIdx, cycle, point, conc);
            }
        }
    }
#endif // SORT1
#endif // MULTITHREADEDSORT

//    elapsed = std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::steady_clock::now( ) - start );
//    std::cout << "milliseconds for predictions sort : " << elapsed.count( ) << '\n';
    return ComputingResult::Success;
} 


AprioriMonteCarloPercentileCalculator::AprioriMonteCarloPercentileCalculator()
{

}

IPercentileCalculator::ComputingResult AprioriMonteCarloPercentileCalculator::calculate(
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
        ComputingAborter *_aborter)
{
    EigenMatrix choleskyMatrix; // Cholesky matrix after decomposition
    EigenMatrix errorMatrix; // Error matrix of both omega and sigma combined (inter and intra errors)

    // Extracting the variances from error matrix into vector
    size_t omegaRank = omegaSize(_omega);
    errorMatrix = _omega;

    choleskyMatrix = EigenMatrix::Zero(omegaRank, omegaRank);

    // Take the lower cholesky to generate correlated normal random deviates
    Eigen::LLT<EigenMatrix> llt(errorMatrix);
    choleskyMatrix = llt.matrixL().transpose();

    // Static random number generator
    // This method does not work anymore (Linux Ubuntu 16.04, gcc 5.4.0, changing it to a time seed
    //static std::random_device randomDevice;
    //std::mt19937 rnGenerator(randomDevice());
    unsigned seed1 = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 rnGenerator(seed1);

    // The variables are normally distributed, we use boost standard normal, then apply lower cholesky
    std::normal_distribution<> normalDistribution(0, 1.0);

    Etas eta(omegaRank);
    Tucuxi::Core::ConcentrationPredictionPtr predictionPtr = std::make_unique<Tucuxi::Core::ConcentrationPrediction>();

    unsigned int nbPatients = getNumberPatients();

    
    //clock_t t1 = clock();


    // TODO : epsilons and rands could be static, in a cache to save time

    // Generating the random numbers
    EigenMatrix rands = EigenMatrix::Zero(nbPatients, omegaRank);

    std::vector<Deviations> epsilons(nbPatients);
    // We fill the epsilons
    for (unsigned int p = 0; p < nbPatients; p++) {
        epsilons[p] = Deviations(_residualErrorModel.nbEpsilons(), normalDistribution(rnGenerator));
    }

    //clock_t t2 = clock();

    for (int row = 0; row < rands.rows(); row++) {
        for (int column = 0; column < rands.cols(); column++) {
            rands(row, column) = normalDistribution(rnGenerator);
        }
    }

    //clock_t t3 = clock();

    //std::cout << "Time : " << t2 - t1 << ", " << t3 - t1 << std::endl;
    //std::cout << "Time : " << ((double)t2 - t1)/((double)CLOCKS_PER_SEC) << ", " << ((double)t3 - t1)/((double)CLOCKS_PER_SEC) << std::endl;

    std::vector<Etas> etaSamples(nbPatients);

    for(unsigned int patient = 0; patient < nbPatients; patient++) {
        EigenVector matrixY = rands.row(patient);

        // Cholesky is applied here to get correlated random deviates
        EigenVector matrixX = choleskyMatrix * matrixY;
        etaSamples[patient].assign(&matrixX(0), &matrixX(0) + omegaRank);

        for(unsigned int eta = 0; eta < _initialEtas.size(); eta++) {
            etaSamples[patient][eta] += _initialEtas[eta];
        }
    }

    return computePredictionsAndSortPercentiles(
                _percentiles,
                _recordFrom,
                _recordTo,
                _intakes,
                _parameters,
                _residualErrorModel,
                _percentileRanks,
                etaSamples,
                epsilons,
                _concentrationCalculator,
                _aborter);
}


void MonteCarloPercentileCalculatorBase::calculateSubomega(
        const OmegaMatrix &_omega,
        const Etas &_etas,
        Likelihood &_logLikelihood,
        EigenMatrix &_subomega)
{
    // Posterior mode (resulting eta vector from calculating aposteriori) 'eta' in args
    const Value* etasPtr = &_etas[0]; // get _etas pointer

    // Evaluate inverse of hessian of loglikelihood
    EigenVector eta_min = EigenVector::Zero(_omega.rows());
    EigenVector eta_max = EigenVector::Zero(_omega.rows());

    _logLikelihood.initBounds(_omega, eta_max, eta_min);

    // Get working eta
    map2EigenVectorType etas(etasPtr, _etas.size());

    EigenMatrix hessian(etas.size(), etas.size());

    // Get the hessian
    deriv2(_logLikelihood, (EigenVector&)etas, (EigenMatrix&)hessian);

    // Negative inverse
    // We don't take the negative hessian because we minimize the negative log likelyhood
    _subomega = hessian.inverse();
}


AposterioriMonteCarloPercentileCalculator::AposterioriMonteCarloPercentileCalculator()
{
}

IPercentileCalculator::ComputingResult AposterioriMonteCarloPercentileCalculator::calculate(
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
        ComputingAborter *_aborter)
{
    // If there is no sample, then there is no reason to calculate a posteriori percentiles
    if (_samples.size() == 0) {
        return ComputingResult::Failure;
    }

    // Return value from non-negative hessian matrix and loglikelihood
    EigenMatrix subomega;
    Likelihood logLikelihood(_omega, _residualErrorModel, _samples, _intakes, _parameters, _concentrationCalculator);
    
    // 1. Calculate hessian metrix and subomega
    calculateSubomega(
                _omega,
                _etas,
                logLikelihood,
                subomega);

    // 2. Get meanEtas and lower cholesky of omega
    // TODO check Map functions correctly or not when test MC
    const Value* etasPtr = &_etas[0]; // get _etas pointer

    map2EigenVectorType meanEtas(etasPtr, _etas.size());

    Eigen::LLT<EigenMatrix> subomegaLLT(subomega);
    EigenMatrix etaLowerChol = subomegaLLT.matrixL().transpose();

    // 3. Draw initial sample from multivariate students t-dist w/ 1 degree freedom
    // A standard multivariate Student's t random vector can be written as a multivariate normal
    // vector whose covariance matrix is scaled by the reciprocal of a Gamma random variable
    // (source: http://www.statlect.com/mcdstu1.htm) generating the multivariate normal starts
    // the same as mcarls
    static std::random_device randomDevice;
    std::mt19937 rnGenerator(randomDevice());

    // The variables are normally distributed, we use standard normal, then apply lower cholesky
    int studentLiberty = 1;
    std::student_t_distribution<> standardNormalDist(studentLiberty);

    // Now we start making a sample. How many samples to take?
    const int samples = 100000;
    const int reSamples = 10000;

    std::vector<Etas> etaSamples(samples);

    // This is ratio for the ratios in step 4
    EigenVector ratio(samples);
    EigenVector weight(samples);

    //clock_t t1 = clock();

    // TODO : This EigenMatrix could be static, in a cache to save time

    // Loading random numbers as omp doesnt like it
    EigenMatrix avecs = EigenMatrix::Zero(samples, _etas.size());
    for (int row = 0; row < avecs.rows(); row++) {
        for (int column = 0; column < avecs.cols(); column++) {
            avecs(row,column) = standardNormalDist(rnGenerator);
        }
    }


    //clock_t t2 = clock();

    //std::cout << "Time : " << t2 - t1 << std::endl;
    //std::cout << "Time : " << ((double)t2 - t1)/((double)CLOCKS_PER_SEC) << std::endl;


    bool abort = false;

    // Parallelizing this for loop with shared variables
    unsigned int nbThreads = std::max(std::thread::hardware_concurrency(), unsigned{1});

    // TODO : Should get rid of this, but claculation does not work as is, because of
    // race condition in the calculation.
    //nbThreads = 1;

    auto meanEtasTransposed = meanEtas.transpose();

    // This is g
    // Using the multi-t-dist pdf directly from this source: http://www.statlect.com/mcdstu1.htm
    // because the multi-t-dist doesnt exist in boost using the multi-t dist from wikipedia
    double v = studentLiberty;
    double p = static_cast<double>(_etas.size());
    double top = tgamma((v + p)/2);
    double part2 = std::sqrt(subomega.determinant());
    double part3 = tgamma(v / 2) * std::pow(v * 3.14159, p/2);

    std::vector<std::thread> workers;
    for(unsigned thread = 0;thread < nbThreads; thread++) {


        workers.push_back(std::thread([thread, &abort, _aborter, nbThreads, _etas, meanEtas, avecs, etaLowerChol,
                                      samples, &etaSamples, studentLiberty, subomega, &ratio, &meanEtasTransposed,
                                      v,p,top,part2,part3, &_intakes, &_omega, &_samples, &_residualErrorModel,
                                      &_parameters, &_concentrationCalculator]()
        {

            // Duplicate an IntakeSeries for avoid a possible problem with multi-thread
            IntakeSeries newIntakes;
            cloneIntakeSeries(_intakes, newIntakes);

            // Also duplicate the Likelihood
            Likelihood threadLogLikelihood(_omega, _residualErrorModel, _samples, newIntakes, _parameters, _concentrationCalculator);


            unsigned start = thread * (samples / nbThreads);
            unsigned end = (thread + 1 ) * (samples / nbThreads);
            for (unsigned sample = start; sample < end; sample++) {
                if (!abort) {
                    if ((_aborter != nullptr) && (_aborter->shouldAbort())) {
                        abort = true;
                        return;
                    }

                    EigenVector avec = meanEtasTransposed + avecs.row(sample) * etaLowerChol;
                    Etas avec_mat;

                    for (unsigned int etasIdx = 0; etasIdx < _etas.size(); etasIdx++) {
                        avec_mat.push_back(avec[etasIdx]);
                    }

                    etaSamples[sample] = avec_mat;
                    
                    // 4. Calculate the ratios for weighting this is h*
                    // Be careful: hstart should be a logLikelihood, however we are minimizing the
                    // negative loglikelyhood, so it is a negative h start.
                    Value hstart = threadLogLikelihood.negativeLogLikelihood(avec_mat);


                    // This is g
                    // Using the multi-t-dist pdf directly from this source: http://www.statlect.com/mcdstu1.htm
                    // because the multi-t-dist doesnt exist in boost using the multi-t dist from wikipedia
                    //double v = studentLiberty;
                    //double p = static_cast<double>(_etas.size());
                    //double top = tgamma((v + p)/2);
                    //double part2 = std::sqrt(subomega.determinant());
                    //double part3 = tgamma(v / 2) * std::pow(v * 3.14159, p/2);
                    double part35 = 1 + 1/v * (avec - meanEtas).transpose() * subomega.inverse() * (avec - meanEtas);
                    double part4 = std::pow(part35,(v + p)/2);
                    double g = top / (part2 * part3 * part4);

                    // Set the ratio
                    ratio[sample] =  exp(-hstart) / g;
                }
            }
        }));
    }

    std::for_each(workers.begin(), workers.end(), [](std::thread &t) {
        t.join();
    });

    if (abort) {
        return ComputingResult::Aborted;
    }

    // 5. Calculate the weights
    weight = ratio / ratio.sum();

    // 6. Draw samples from discrete distribution using weights
    std::normal_distribution<> normalDistribution(0, 1.0);
    std::discrete_distribution<std::size_t> discreteDistribution(&weight(0), &weight(0) + weight.size());

    // TODO : These epsilons could be stored in a cache to save time
    std::vector<Deviations> epsilons(reSamples);
    // We fill the epsilons
    for (std::size_t p = 0; p < reSamples; p++) {
        // TODO : I think we will have the same value for each epsilon of a specific sample
        epsilons[p] = Deviations(_residualErrorModel.nbEpsilons(), normalDistribution(rnGenerator));
    }

    std::vector<Etas> RealEtaSamples(reSamples);

    // TODO: think about whether we can use push_back or not
    for(std::size_t patient = 0; patient < reSamples; patient++) {
        RealEtaSamples[patient] = etaSamples[discreteDistribution(rnGenerator)];
    }

    return computePredictionsAndSortPercentiles(
                _percentiles,
                _recordFrom,
                _recordTo,
                _intakes,
                _parameters,
                _residualErrorModel,
                _percentileRanks,
                RealEtaSamples,
                epsilons,
                _concentrationCalculator,
                _aborter);
}

AposterioriNormalApproximationMonteCarloPercentileCalculator::AposterioriNormalApproximationMonteCarloPercentileCalculator()
{
}

IPercentileCalculator::ComputingResult AposterioriNormalApproximationMonteCarloPercentileCalculator::calculate(
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
        ComputingAborter *_aborter)
{
    // Return value from non-negative hessian matrix and loglikelihood
    EigenMatrix subomega;
    Likelihood logLikelihood(_omega, _residualErrorModel, _samples, _intakes, _parameters, _concentrationCalculator);

    // 1. Calculate hessian metrix and subomega
    calculateSubomega(
                _omega,
                _etas,
                logLikelihood,
                subomega);

    // Only apply steps 1 and 2 of the Annex A of Aziz document (posteriori2.pdf)
    AprioriMonteCarloPercentileCalculator aprioriMC;
    return aprioriMC.calculate(
                _percentiles,
                _recordFrom,
                _recordTo,
                _intakes,
                _parameters,
                subomega,
                _residualErrorModel,
                _etas,
                _percentileRanks,
                _concentrationCalculator,
                _aborter);
}

} // namespace Core
} // namespace Tucuxi
