/*
* Copyright (C) 2017 Tucuxi SA
*/


#include "likelihood.h"
#include "concentrationcalculator.h"
#include "residualerrormodel.h"

#include <boost/math/special_functions/erf.hpp>
#include <Eigen/LU>
#include <iostream>


namespace Tucuxi {
namespace Core {

Likelihood::Likelihood(const OmegaMatrix& _omega,
                       const IResidualErrorModel& _residualErrorModel,
                       const SampleSeries& _samples,
                       const IntakeSeries& _intakes,
                       const ParameterSetSeries& _parameters,
                       IConcentrationCalculator &_concentrationCalculator)
    : m_omega(&_omega),
      m_residualErrorModel(&_residualErrorModel),
      m_samples(&_samples),
      m_intakes(&_intakes),
      m_parameters(&_parameters),
      m_concentrationCalculator(&_concentrationCalculator)
{
    initBounds(_omega, m_omax, m_omin);
}

void Likelihood::initBounds(const OmegaMatrix& _omega, EigenVector& _oMax, EigenVector& _oMin, double _highX, double _lowX)
{
    _oMin = _omega.diagonal().array().sqrt() * sqrt(2) * boost::math::erf_inv(2 * _lowX - 1);
    _oMax = _omega.diagonal().array().sqrt() * sqrt(2) * boost::math::erf_inv(2 * _highX - 1);
}


Value Likelihood::operator()(const Eigen::VectorXd& _etas)
{
    ValueVector etasmd(_etas.size());
    for (int i = 0; i < _etas.size(); ++i) {
        etasmd[i] = _etas[i];
    }
    return (*this)(etasmd);
}



Value Likelihood::operator()(const ValueVector& _etas)
{
    return negativeLogLikelihood(_etas);
}

Value Likelihood::negativeLogLikelihood(const ValueVector& _etas) const
{
    ValueVector _cxns(m_samples->size());
    bool isAll = false;

    // Getting the concentration values at these _times and m_samples.
    ComputationResult result = m_concentrationCalculator->computeConcentrationsAtTimes(
        _cxns,
        isAll,
        *m_intakes,
        *m_parameters,
        *m_samples,
        _etas);

    // If the calculation fails, its highly unlikely so we return the largest number we can

    if (result != ComputationResult::Success) {
        return std::numeric_limits<double>::max();
    }
    Value gll = 0;

    //calculate the prior which depends only on eta and omega (not the measure)
    Value _logprior = negativeLogPrior(Eigen::Map<const EigenVector>(&_etas[0], _etas.size()), *m_omega);
    SampleSeries::const_iterator sit = m_samples->begin();
    SampleSeries::const_iterator sit_end = m_samples->end();
    int _count = 0;
    while( sit != sit_end ) {
        // SampleEvent s = *sit;
        gll += calculateSampleNegativeLogLikelihood(_cxns[_count], *sit, *m_residualErrorModel);
        _count++;
        sit++;
    }
    gll += _logprior;

    // If we have a really big problem, like we have a log of zero
    if (std::isnan(gll)) {
        //        EXLOG(QtWarningMsg, ezechiel::math::NOEZERROR, QObject::tr("Log likelihood is NAN"))
        gll = std::numeric_limits<double>::max();
    }

    return gll;
}

Value Likelihood::calculateSampleNegativeLogLikelihood(const Value _expected,
                                                       const SampleEvent& _observed,
                                                       const IResidualErrorModel &_residualErrorModel) const
{
    return - _residualErrorModel.calculateSampleLikelihood(_expected, _observed.getValue());
}

Value Likelihood::negativeLogPrior(const EigenVector& _etas, const OmegaMatrix &_omega) const
{
    //here we calculate the log of all the parts and sum them, neglecting the negative because we minimize
    return 0.5 *
           (_etas.transpose() * _omega.inverse() * _etas +
            _omega.rows() * log(2 * PI) +
            log(_omega.determinant()));
}

}
}
