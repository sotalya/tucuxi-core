/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "aposteriorietascalculator.h"
#include "likelihood.h"
#include "minimize.h"
#include "concentrationcalculator.h"

namespace Tucuxi {
namespace Core {

APosterioriEtasCalculator::APosterioriEtasCalculator()
{

}


APosterioriEtasCalculator::ComputationResult APosterioriEtasCalculator::computeAposterioriEtas(
        const IntakeSeries &_intakes,
        const ParameterSetSeries &_parameters,
        const OmegaMatrix &_omega,
        const IResidualErrorModel &_residualErrorModel,
        const SampleSeries &_samples,
        Etas &_aPosterioriEtas)
{


// Verify we have a var-covar matrix
    if (isOmegaEmpty(_omega)) {
        //throw something
        return ComputationResult::Failure;
    }

// Verify is square matrix
    if ( !isOmegaSquare(_omega)) {
        //throw something
        return ComputationResult::Failure;
    }

    const size_t omegaSize = omegaSize(_omega);

// Prints out the parameter values
    //    parameters_series_t::const_iterator pit, pit_end;
    //    pit = parameters.begin();
    //    pit_end = parameters.end();
    //    while (pit != pit_end) {
    //        auto psit = pit->values.begin();
    //        auto psit_end = pit->values.end();
    //        while (psit != psit_end) {
    //            QTextStream(stdout) << QString(&psit->name[0]) << psit->value << endl;
    //            psit++;
    //        }
    //        pit++;
    //    }

    // TODO : Use a factory for the calculator
    ConcentrationCalculator calculator;
  // This is the object holding the state of minimization
    Likelihood funcd(_omega, _residualErrorModel, _samples, _intakes, _parameters, calculator);

    // This is the object responsible for minimization
    Frprmn<Likelihood> frprmn(funcd);

    // Initial etas (0)
    ValueVector initialEtas(omegaSize);

// Execute the minimizer
    _aPosterioriEtas = frprmn.minimize(initialEtas);


// Prints out the eta values
    //    QTextStream(stdout) << "Eta: " << endl;
    //    for (int i = 0; i < _eta.size(); ++i) {
    //        QTextStream(stdout) << (double)_eta[i] << " " << endl;
    //    }

    return ComputationResult::Success;
}


}
}
