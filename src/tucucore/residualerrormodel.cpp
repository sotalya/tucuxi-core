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


#include "tucucore/residualerrormodel.h"

#include "tucucommon/general.h"

#include "tucucore/definitions.h"
#include "tucucore/sampleevent.h"

namespace Tucuxi {
namespace Core {



void SigmaResidualErrorModel::setSigma(Sigma _sigma)
{
    m_sigma = std::move(_sigma);
}

void SigmaResidualErrorModel::setErrorModel(ResidualErrorType _errorModel)
{
    m_errorModel = _errorModel;
    if (_errorModel == ResidualErrorType::NONE) {
        m_nbEpsilons = 0;
    }
}

bool SigmaResidualErrorModel::isEmpty() const
{
    return m_sigma.size() == 0;
}


void SigmaResidualErrorModel::applyEpsToValue(Concentration& _concentration, const Deviations& _eps) const
{

    switch (m_errorModel) {
    case ResidualErrorType::EXPONENTIAL:
    case ResidualErrorType::PROPEXP:
        _concentration *= std::exp(m_sigma[0] * _eps[0]);
        break;
    case ResidualErrorType::PROPORTIONAL:
        _concentration *= 1 + m_sigma[0] * _eps[0];
        break;
    case ResidualErrorType::ADDITIVE:
        _concentration += m_sigma[0] * _eps[0];
        break;
    case ResidualErrorType::MIXED:
        // m_sigma[0] is the additive SD and m_sigma[1] the exponential SD
        _concentration += _eps[0] * std::sqrt(std::pow(_concentration * m_sigma[1], 2) + std::pow(m_sigma[0], 2));
        break;
    default:
        // Should never happen
        break;
    }
};

void SigmaResidualErrorModel::applyEpsToArray(Concentrations& _concentrations, const Deviations& _eps) const
{

    // Loop through the main compartment concentrations and apply the residual error
    for (double& concentration : _concentrations) {
        switch (m_errorModel) {
        case ResidualErrorType::EXPONENTIAL:
        case ResidualErrorType::PROPEXP:
            concentration *= std::exp(m_sigma[0] * _eps[0]);
            break;
        case ResidualErrorType::PROPORTIONAL:
            concentration *= 1 + m_sigma[0] * _eps[0];
            break;
        case ResidualErrorType::ADDITIVE:
            concentration += m_sigma[0] * _eps[0];
            break;
        case ResidualErrorType::MIXED:
            concentration += _eps[0] * std::sqrt(std::pow(concentration * m_sigma[1], 2) + std::pow(m_sigma[0], 2));
            break;
        default:
            // Should never happen
            break;
        }
    }
};

Value SigmaResidualErrorModel::calculateSampleLikelihood(Value _expected, Value _observed) const
{
    // This first part gets the difference between the predicted mean and the measured value
    Value expectedObservedDiff = std::abs(_observed - _expected);
    // Sig is calculated with _y here
    double sig = 0.0;

    switch (m_errorModel) {
    case ResidualErrorType::ADDITIVE: {
        expectedObservedDiff = _observed - _expected;
        sig = m_sigma(0);
    } break;
    case ResidualErrorType::PROPORTIONAL: {
        expectedObservedDiff = _observed - _expected;
        sig = m_sigma(0) * _expected;
    } break;
    case ResidualErrorType::EXPONENTIAL: {
        if (_observed == 0.0) {
            expectedObservedDiff = log(1e-10) - log(_expected);
        }
        else {
            expectedObservedDiff = log(_observed) - log(_expected);
        }
        sig = m_sigma(0);
    } break;
    case ResidualErrorType::PROPEXP: {
        // The following should be used for real Exponential models
        // Unfortunately NONMEM does not work that way
        // expectedObservedDiff = log(_observed) - log(_expected);
        // sig = m_sigma(0);
        // Use proportional instead of Exponential for NONMEM compatibility
        expectedObservedDiff = _observed - _expected;
        sig = m_sigma(0) * _expected;
    } break;
    case ResidualErrorType::MIXED: {
        expectedObservedDiff = _observed - _expected;
        sig = std::sqrt(std::pow(m_sigma(1) * _expected, 2) + std::pow(m_sigma(0), 2));
    } break;
    default: {
        // Should never happen
    } break;
    }

    // If there is no resid variance, there is only one
    // possibility (assuming it can be reached based on the limits of eta)
    // The interindividual variance takes over and reaches the sample value
    if (sig == 0.0) {
        // Something wrong happened
        if (expectedObservedDiff == 0.0) {
            return 0.0;
        }
        return -std::numeric_limits<double>::max();
    }


    Value phi = 0.0;

    // The following should be used for real Exponential models
    // Unfortunately NONMEM does not work that way
    /*    if (m_errorModel == ResidualErrorType::EXPONENTIAL) {
        phi =
                (0.5 * log(2 * PI)) +
                log(_expected) +     // There is this specific log(_expected for LogNormal distribution
                log(sig) +
                0.5 * expectedObservedDiff * 1.0/(std::pow(sig,2)) * expectedObservedDiff;
    }
    else*/

    // Actually it is perfectly fine like this. It validates against NONMEM if NONMEM performs computations on
    // the log scale
    {
        // This is the calculation with a sig of one element
        phi = (0.5 * log(2 * PI)) + log(sig)
              + 0.5 * expectedObservedDiff * 1.0 / (std::pow(sig, 2)) * expectedObservedDiff;
    }

    // If we have a really big problem, like we have a log of zero
    if (std::isnan(phi)) {
        //        EXLOG(QtWarningMsg, tucuxi::math::NOEZERROR, QObject::tr("Log likelihood is NAN"))
        phi = std::numeric_limits<double>::max();
    }
    return -phi;
}

} // namespace Core
} // namespace Tucuxi
