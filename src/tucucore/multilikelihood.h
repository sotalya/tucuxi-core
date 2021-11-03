/*
* Copyright (C) 2017 Tucuxi SA
*/


#ifndef TUCUXI_CORE_MULTILIKELIHOOD_H
#define TUCUXI_CORE_MULTILIKELIHOOD_H

#include <boost/math/special_functions/sign.hpp>
#include <Eigen/Dense>

#include "deriv.h"
#include "sampleevent.h"
#include "dosage.h"
#include "parameter.h"
#include "multiconcentrationcalculator.h"

namespace Tucuxi {
namespace Core {

class IResidualErrorModel;
class IMultiConcentrationCalculator;

/// \brief The MultiLikelihood class
/// This is the class that calculates the log-likelihood for aposteriori of multi-analytes equations
/// An instance is passed to the optimizer in minimize.h to calculate mode of posterior distribution.
///
/// As there can multiple analytes, the constructor takes a vector of residual error models, one per
/// analyte, and a vector of sample series, also one per analyte.
class MultiLikelihood {
public:

    MultiLikelihood(const OmegaMatrix& _omega,
               const std::vector<IResidualErrorModel>& _residualErrorModel,
               const std::vector<SampleSeries>& _samples,
               const IntakeSeries& _intakes,
               const ParameterSetSeries& _parameters,
               MultiConcentrationCalculator& _concentrationCalculator);

    /// \brief operator ()
    /// This method calculates the negative log of the posterior and prior.
    /// The concentrations at given etas and sample (measure) times are calculated first using aprioriAtTimes.
    /// Then, the log of the the prior is calculated (only once).
    /// Then, the log of the likelihood at each sample is calculated.
    /// All the negative log values are added together (do a big AND operation).
    /// This method only calls negativeLogLikelihood.
    /// \param _etas
    /// \return The vector of etas being optimized
    Value operator()(const ValueVector& _etas);

    /// \brief negativeLogLikelihood
    /// This method calculates the negative log of the posterior added to the prior.
    /// The concentrations at given etas and sample (measure) times are calculated first using aprioriAtTimes.
    /// Then, the log of the the prior is calculated (only once).
    /// Then, the log of the likelihood at each sample is calculated.
    /// All the negative log values are added together (do a big AND operation)
    ///
    /// As we can have multiple analytes, the concentrations at sample times have to be calculated for each
    /// sample, independently of their analyte. It means it can be done by first compute the times at which
    /// we need a concentration, and then run the computation.
    /// Then, the calculation of the log of the likelihood for each sample has to take care of the analyte
    /// of each sample. We can simply iterate over the analytes in an outter loop, and over the samples of
    /// this analyte in the inner loop.
    /// Be careful as we could have, for a 2-analyte example, cases where at a certain time only one
    /// analyte has been sampled, and at certain times there could be a sample per analyte. As the sample series
    /// are analyte-agnostic, it means each SampleSeries of _samples refers to a specific analyte. The analyte
    /// index shall correspond to the analyte index in the IntakeCalculator.
    ///
    /// \param _etas The vector of etas being optimized
    /// \return The negative log likelihood of the samples based on the _etas
    Value negativeLogLikelihood(const ValueVector& _etas) const;

    ///
    /// \brief operator ()
    /// This method calculates the negative log of the posterior and the prior.
    /// The concentrations at given etas and sample (measure) times are calculated first using aprioriAtTimes.
    /// Then, the log of the the prior is calculated (only once).
    /// Then, the log of the likelihood at each sample is calculated.
    /// All the negative log values are added together (do a big AND operation)
    /// \param _etas The vector of etas being optimized
    /// \return
    ///
    Value operator()(const Eigen::VectorXd& _etas);

    /// This is used by dbrents method to calculate derivatives
    /// \brief df
    /// \param _x
    /// \param _deriv
    ///
    void df(ValueVector &_x, ValueVector &_deriv)
    {
        deriv1([&](const ValueVector & _in) -> Value {return (*this)(_in);}, _x, _deriv, static_cast<Value>(_x.size()));
        for (unsigned int i = 0; i < _x.size(); i++) {
            // bounds the value:
            _deriv[i] = std::max(m_omin[i], std::min(_deriv[i], m_omax[i]));
        }
    }

    /// \brief calculateSampleNegativeLogLikelihood
    /// This method calculates the negative log of the likelihood for one sample.
    /// The initial sigma is given which may depend on the concentration _expected.
    /// The true sigma is calculated with the concentration prediction _expected,
    /// and then the resulting sigma is only one element. We fix the residual errors
    /// to be just additive, proportional/exponential(treated same), or a mix of 1 additive
    /// with 1 proportional/exponential. In case of the mix, it is approximated into a single
    /// additive value - so we always end up with just 1 value in our matrix.
    /// This method is called from operator()
    /// \param _expected Expected concentration value
    /// \param _observed Observed concentration values
    /// \param _residualErrorModel Residual error model to be used for calculation
    /// \return the negative log-likelihood of a concentration at the sample time
    Value calculateSampleNegativeLogLikelihood(Value _expected,
                                               const SampleEvent& _observed,
                                               const IResidualErrorModel &_residualErrorModel) const;

    /// Sets the bounds on etas to extreme values of normal distribution
    /// using the equation for the inverse of the cdf for normal distribution
    /// (here by default it is set to 99.9 percentile and 0.1 percentile)
    /// \param _omega
    /// \param _oMax
    /// \param _oMin
    /// \param _highX
    /// \param _lowX
    void initBounds(const OmegaMatrix& _omega,
                    EigenVector& _oMax,
                    EigenVector& _oMin,
                    double _highX = 0.999,
                    double _lowX = 0.001);

    /// \brief Calculates the negative log of the prior from Bayes theorem
    /// \param _etas vector of eta values being optimized
    /// \param _omega covariance matrix of inter-individual error
    /// \return The calculated negative log of the prior
    Value negativeLogPrior(const EigenVector& _etas /*, const OmegaMatrix &_omega*/) const;

private:

    /// vector of minimum values for eta
    EigenVector m_omin;

    /// vector of maximum values for eta
    EigenVector m_omax;

    /// inter-individual var-covar matrix
    // const OmegaMatrix* m_omega;

    /// intra-individual error model
    const std::vector<IResidualErrorModel>* m_residualErrorModel;

    /// multi-index of samples for entire curve
    const std::vector<SampleSeries>* m_samples;

    /// multi-index of intakes for entire curve
    const IntakeSeries* m_intakes;

    /// multi-index of parameters for entire curve
    const ParameterSetSeries* m_parameters;

    /// inter-individual var-covar matrix, inversed
    const OmegaMatrix m_inverseOmega;

    /// Value to add for the negative log prior calculation
    const double m_omegaAdd{0};

    // Concentration calculator used for all calculations
    MultiConcentrationCalculator* m_concentrationCalculator;
};


} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_MULTILIKELIHOOD_H

