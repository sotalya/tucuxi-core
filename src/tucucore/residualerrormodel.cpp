/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "tucucore/residualerrormodel.h"
#include "tucucore/definitions.h"
#include "tucucore/sample.h"
#include "tucucommon/general.h"

namespace Tucuxi {
namespace Core {


bool SigmaResidualErrorModel::isEmpty() const
{
    return sigma.size() == 0;
}


void SigmaResidualErrorModel::applyEpsToArray(Concentrations &_concentrations, const Deviations &_eps) const {

    // Loop through the main compartment concentrations and apply the residual error
    for (auto it = _concentrations.begin(); it != _concentrations.end(); ++it) {
        switch (errorModel) {
        case ResidualErrorType::EXPONENTIAL:
            *it *= std::pow(sigma[0], 2) * std::exp(_eps[0]);
        case ResidualErrorType::PROPORTIONAL:
            *it *= 1 + sigma[0] * _eps[0];
            break;
        case ResidualErrorType::ADDITIVE:
            *it += sigma[0] * _eps[0];
            break;
        case ResidualErrorType::MIXED:
            *it += _eps[0] * std::sqrt(std::pow(*it * sigma[1], 2)  + std::pow(sigma[0], 2));
            break;
        default:
            // Should never happen
            break;
        }
    }
    return;
};

Value SigmaResidualErrorModel::calculateSampleLikelihood(const Value _expected, const Value& _observed) const {

    static const float PI = 3.14159;
    // This first part gets the difference between the predicted mean and the measured value
    Value expectedObservedDiff = std::abs(_observed - _expected);
    // Sig is calculated with _y here
    double sig = 0.0;

    switch(errorModel) {
    case ResidualErrorType::ADDITIVE: {
        expectedObservedDiff = _observed - _expected;
        sig = sigma(0);
    } break;
    case ResidualErrorType::PROPORTIONAL: {
        expectedObservedDiff = _observed - _expected;
        sig = sigma(0) * _expected;
    } break;
    case ResidualErrorType::EXPONENTIAL: {
        expectedObservedDiff = log(_observed) - log(_expected);
        sig = sigma(0);
    } break;
    case ResidualErrorType::MIXED: {
        expectedObservedDiff = _observed - _expected;
        sig = std::sqrt(std::pow(sigma(1) * _expected, 2) + std::pow(sigma(0),2));
    } break;
    default: {
        // Should never happen
    } break;
    }

    // If there is no resid variance, there is only one
    // possibility (assuming it can be reached based on the limits of eta)
    // The interindividual variance takes over and reaches the sample value
    if(sig == 0.0) {
        // Something wrong happened
        if (expectedObservedDiff == 0)
        {
            return 0;
        } else {
            return -std::numeric_limits<double>::max();
        }
    }


    // This is the calculation with a sig of one element
    Value phi =
            (0.5 * log(2 * PI)) +
            log(sig) +
            0.5 * expectedObservedDiff * 1/(std::pow(sig,2)) * expectedObservedDiff;


    // If we have a really big problem, like we have a log of zero
    if (std::isnan(phi)) {
        //        EXLOG(QtWarningMsg, ezechiel::math::NOEZERROR, QObject::tr("Log likelihood is NAN"))
        phi = std::numeric_limits<double>::max();
    }
    return - phi;
}

}
}