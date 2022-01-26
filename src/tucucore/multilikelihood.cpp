/*
* Copyright (C) 2017 Tucuxi SA
*/


#include <iostream>

#include <Eigen/LU>
#include <boost/math/special_functions/erf.hpp>

#include "multilikelihood.h"

#include "multiconcentrationcalculator.h"
#include "residualerrormodel.h"


namespace Tucuxi {
namespace Core {

//Likelihood -> Combination between residual error, and variabilty
//Multilikelihood -> Likelihood adapted for multianalytes, multiconcentration...

MultiLikelihood::MultiLikelihood(
        const OmegaMatrix& _omega,
        const std::vector<SigmaResidualErrorModel>& _residualErrorModel,
        const std::vector<SampleSeries>& _samples,
        const IntakeSeries& _intakes,
        const ParameterSetSeries& _parameters,
        MultiConcentrationCalculator& _multiconcentrationCalculator)
    : // m_omega(&_omega),
      m_residualErrorModel(_residualErrorModel), m_samples(_samples), m_intakes(&_intakes), m_parameters(&_parameters),
      m_inverseOmega(_omega.inverse()),
      m_omegaAdd(static_cast<double>(_omega.rows()) * log(2 * PI) + log(_omega.determinant())),
      m_concentrationCalculator(&_multiconcentrationCalculator) //i have to fix that
{
    initBounds(_omega, m_omax, m_omin);
}

void MultiLikelihood::initBounds(
        const OmegaMatrix& _omega, EigenVector& _oMax, EigenVector& _oMin, double _highX, double _lowX)
{
    _oMin = _omega.diagonal().array().sqrt() * sqrt(2) * boost::math::erf_inv(2 * _lowX - 1);
    _oMax = _omega.diagonal().array().sqrt() * sqrt(2) * boost::math::erf_inv(2 * _highX - 1);
}


Value MultiLikelihood::operator()(const Eigen::VectorXd& _etas)
{
    ValueVector etasmd(static_cast<size_t>(_etas.size()));
    for (Eigen::Index i = 0; i < _etas.size(); ++i) {
        etasmd[static_cast<size_t>(i)] = _etas[i];
    }
    return (*this)(etasmd);
}



Value MultiLikelihood::operator()(const ValueVector& _etas)
{
    return negativeLogLikelihood(_etas);
}

Value MultiLikelihood::negativeLogLikelihood(const Etas& _etas) const
{ //returns the negative prior of the likelihood
    ValueVector concentrations(m_samples.size());
    MultiCompConcentrations _concentrations(m_samples.size());
    std::vector<MultiCompConcentrations> _concentrations2(0);

    bool isAll = false;

    // Getting the concentration values at these _times and m_samples.


    //here i need to add a loop that works for all samples, iterating on each index of the vector


    Tucuxi::Core::SampleSeries sampless;


    for(unsigned int i = 0; i < m_samples.size(); ++i){
        for(unsigned int j = 0; j < m_samples[i].size(); ++j){
               if (m_samples[i].size() != 0) sampless.push_back(m_samples[i][j]);
            }
    }

        ComputingStatus result = ComputingStatus::Undefined;
            result = m_concentrationCalculator->computeConcentrationsAtTimes(
                    _concentrations, isAll, *m_intakes, *m_parameters, sampless, _etas);

        if (sampless.size() != 0 && result != ComputingStatus::Ok) {
            return std::numeric_limits<double>::max();
        }
        _concentrations2.push_back(_concentrations);


    // If the calculation fails, its highly unlikely so we return the largest number we can

    Value gll = 0;

    //calculate the prior which depends only on eta and omega (not the measure)
    Value logPrior = negativeLogPrior(
            Eigen::Map<const EigenVector>(&_etas[0], static_cast<Eigen::Index>(_etas.size())) /*, *m_omega*/);

    size_t sampleCounter = 0;

    for (unsigned int i = 0; i < m_samples.size(); ++i) {
        SampleSeries::const_iterator sit = m_samples[i].begin();
        SampleSeries::const_iterator sitEnd = m_samples[i].end();

        while (sit != sitEnd) {
            if (m_samples[i].size() > 0) {
                // SampleEvent s = *sit;
                gll += calculateSampleNegativeLogLikelihood(
                        _concentrations2[0][sampleCounter][i], *sit, m_residualErrorModel[i]);
                sampleCounter++;
                sit++;
            }
        }
    }

    gll += logPrior;

    // If we have a really big problem, like we have a log of zero
    if (std::isnan(gll)) {
        //        EXLOG(QtWarningMsg, ezechiel::math::NOEZERROR, QObject::tr("Log likelihood is NAN"))
        gll = std::numeric_limits<double>::max();
    }

    return gll;
}

Value MultiLikelihood::calculateSampleNegativeLogLikelihood(
        Value _expected, const SampleEvent& _observed, const SigmaResidualErrorModel _residualErrorModel) const
{

    return -_residualErrorModel.calculateSampleLikelihood(_expected, _observed.getValue());
}

Value MultiLikelihood::negativeLogPrior(const EigenVector& _etas /*, const OmegaMatrix &_omega*/) const
{
    //here we calculate the log of all the parts and sum them, neglecting the negative because we minimize

    // I think we could get rid of m_omegaAdd for the computations we are doing, but to be checked
    return 0.5 * (_etas.transpose() * m_inverseOmega * _etas + m_omegaAdd);
    /*
    //here we calculate the log of all the parts and sum them, neglecting the negative because we minimize
    return 0.5 *
           (_etas.transpose() * _omega.inverse() * _etas +
            static_cast<double>(_omega.rows()) * log(2 * PI) +
            log(_omega.determinant()));
            */
}

} // namespace Core
} // namespace Tucuxi
