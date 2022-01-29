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
        const std::vector<IResidualErrorModel*>& _residualErrorModel,
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

    // Build the vector of association between the real samples and the time series
    // First the time index is 0
    for (size_t i = 0; i < m_samples.size(); ++i) {
        for (size_t j = 0; j < m_samples[i].size(); ++j) {
            m_sortingVector.emplace_back(SampleSorting_t{i, j, 0});
        }
    }

    // Sort the vector of associations chronologically
    std::sort(m_sortingVector.begin(), m_sortingVector.end(), [this](SampleSorting_t a, SampleSorting_t b) {
        return this->m_samples[a.analyteIndex][a.sampleIndex].getEventTime()
               < this->m_samples[b.analyteIndex][b.sampleIndex].getEventTime();
    });

    // Build the time series and avoids duplicates
    size_t added = 0;
    for (auto& sorted : m_sortingVector) {
        // Only add a time if it is the first one or not a duplicate
        if ((added == 0)
            || (m_timeSeries.back().getEventTime()
                != m_samples[sorted.analyteIndex][sorted.sampleIndex].getEventTime())) {
            m_timeSeries.emplace_back(SampleEvent(m_samples[sorted.analyteIndex][sorted.sampleIndex]));
            sorted.mergedIndex = added;
            added++;
        }
    }
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
{
    //returns the negative prior of the likelihood
    MultiCompConcentrations concentrations(m_samples.size());

    bool isAll = false;

    ComputingStatus result = ComputingStatus::Undefined;
    result = m_concentrationCalculator->computeConcentrationsAtTimes(
            concentrations, isAll, *m_intakes, *m_parameters, m_timeSeries, _etas);

    if (m_timeSeries.size() != 0 && result != ComputingStatus::Ok) {
        return std::numeric_limits<double>::max();
    }


    // If the calculation fails, its highly unlikely so we return the largest number we can

    Value gll = 0;

    //calculate the prior which depends only on eta and omega (not the measure)
    Value logPrior = negativeLogPrior(
            Eigen::Map<const EigenVector>(&_etas[0], static_cast<Eigen::Index>(_etas.size())) /*, *m_omega*/);

    for (const auto& sorted : m_sortingVector) {
        gll += calculateSampleNegativeLogLikelihood(
                concentrations[sorted.mergedIndex][sorted.analyteIndex],
                m_samples[sorted.analyteIndex][sorted.sampleIndex],
                m_residualErrorModel[sorted.analyteIndex]);
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
        Value _expected, const SampleEvent& _observed, const IResidualErrorModel* _residualErrorModel) const
{

    return -_residualErrorModel->calculateSampleLikelihood(_expected, _observed.getValue());
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
