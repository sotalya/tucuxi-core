/*
 * Tucuxi - Tucuxi-core library and command line tool.
 * This code allows to perform prediction of drug concentration in blood
 * and to propose dosage adaptations.
 * It has been developed by HEIG-VD, in close collaboration with CHUV.
 * Copyright (C) 2023 HEIG-VD, maintained by Yann Thoma  <yann.thoma@heig-vd.ch>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */


#include <iostream>

#include <Eigen/LU>
#include <boost/math/special_functions/erf.hpp>

#include "likelihood.h"

#include "concentrationcalculator.h"


namespace Tucuxi {
namespace Core {

Likelihood::Likelihood(
        const OmegaMatrix& _omega,
        const IResidualErrorModel& _residualErrorModel,
        const SampleSeries& _samples,
        const IntakeSeries& _intakes,
        const ParameterSetSeries& _parameters,
        IConcentrationCalculator& _concentrationCalculator)
    : // m_omega(&_omega),
      m_residualErrorModel(&_residualErrorModel), m_samples(&_samples), m_intakes(&_intakes),
      m_parameters(&_parameters), m_inverseOmega(_omega.inverse()),
      m_omegaAdd(static_cast<double>(_omega.rows()) * log(2.0 * PI) + log(_omega.determinant())),
      m_concentrationCalculator(&_concentrationCalculator)
{
    initBounds(_omega, m_omax, m_omin);
}

void Likelihood::initBounds(
        const OmegaMatrix& _omega, EigenVector& _oMax, EigenVector& _oMin, double _highX, double _lowX)
{
    _oMin = _omega.diagonal().array().sqrt() * sqrt(2) * boost::math::erf_inv(2 * _lowX - 1);
    _oMax = _omega.diagonal().array().sqrt() * sqrt(2) * boost::math::erf_inv(2 * _highX - 1);
}


Value Likelihood::operator()(const Eigen::VectorXd& _etas)
{
    ValueVector etasmd(static_cast<size_t>(_etas.size()));
    for (Eigen::Index i = 0; i < _etas.size(); ++i) {
        etasmd[static_cast<size_t>(i)] = _etas[i];
    }
    return (*this)(etasmd);
}



Value Likelihood::operator()(const ValueVector& _etas)
{
    return negativeLogLikelihood(_etas);
}

Value Likelihood::negativeLogLikelihood(const ValueVector& _etas) const
{
    Concentrations concentrations(m_samples->size());
    bool isAll = false;

    // Getting the concentration values at these _times and m_samples.
    ComputingStatus result = m_concentrationCalculator->computeConcentrationsAtTimes(
            concentrations, isAll, *m_intakes, *m_parameters, *m_samples, _etas);

    // If the calculation fails, its highly unlikely so we return the largest number we can

    if (result != ComputingStatus::Ok) {
        return std::numeric_limits<double>::max();
    }
    Value gll = 0;

    //calculate the prior which depends only on eta and omega (not the measure)
    Value logPrior = negativeLogPrior(
            Eigen::Map<const EigenVector>(&_etas[0], static_cast<Eigen::Index>(_etas.size())) /*, *m_omega*/);
    auto sit = m_samples->begin();
    auto sitEnd = m_samples->end();
    size_t sampleCounter = 0;
    while (sit != sitEnd) {
        // SampleEvent s = *sit;
        gll += calculateSampleNegativeLogLikelihood(concentrations[sampleCounter], *sit, *m_residualErrorModel)
               * sit->getWeight();
        sampleCounter++;
        sit++;
    }
    gll += logPrior;

    // If we have a really big problem, like we have a log of zero
    if (std::isnan(gll)) {
        //        EXLOG(QtWarningMsg, tucuxi::math::NOEZERROR, QObject::tr("Log likelihood is NAN"))
        gll = std::numeric_limits<double>::max();
    }

    return gll;
}

Value Likelihood::calculateSampleNegativeLogLikelihood(
        Value _expected, const SampleEvent& _observed, const IResidualErrorModel& _residualErrorModel) const
{
    return -_residualErrorModel.calculateSampleLikelihood(_expected, _observed.getValue());
}

Value Likelihood::negativeLogPrior(const EigenVector& _etas /*, const OmegaMatrix &_omega*/) const
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
