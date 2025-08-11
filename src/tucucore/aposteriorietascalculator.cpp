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


#include "aposteriorietascalculator.h"

#include "concentrationcalculator.h"
#include "likelihood.h"
#include "minimize.h"

namespace Tucuxi {
namespace Core {

APosterioriEtasCalculator::APosterioriEtasCalculator() = default;


ComputingStatus APosterioriEtasCalculator::computeAposterioriEtas(
        const IntakeSeries& _intakes,
        const ParameterSetSeries& _parameters,
        const OmegaMatrix& _omega,
        const IResidualErrorModel& _residualErrorModel,
        const SampleSeries& _samples,
        Etas& _aPosterioriEtas,
        Value& _negativeLogLikelihood)
{
    // Verify we have a var-covar matrix
    if (isOmegaEmpty(_omega)) {
        //throw something
        return ComputingStatus::AposterioriEtasCalculationEmptyOmega;
    }

    // Verify is square matrix
    if (!isOmegaSquare(_omega)) {
        //throw something
        return ComputingStatus::AposterioriEtasCalculationNoSquareOmega;
    }

    // Check that there is at least one measure
    if (_samples.empty()) {

        _aPosterioriEtas.assign(_aPosterioriEtas.size(), 0.0);

        return ComputingStatus::Ok;
    }

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
    ValueVector initialEtas(static_cast<size_t>(omegaSize(_omega)));

    // Execute the minimizer
    _aPosterioriEtas = frprmn.minimize(initialEtas);

    _negativeLogLikelihood = funcd.negativeLogLikelihood(_aPosterioriEtas);


    // Prints out the eta values
    //    QTextStream(stdout) << "Eta: " << endl;
    //    for (int i = 0; i < _eta.size(); ++i) {
    //        QTextStream(stdout) << (double)_eta[i] << " " << endl;
    //    }

    return ComputingStatus::Ok;
}


} // namespace Core
} // namespace Tucuxi
