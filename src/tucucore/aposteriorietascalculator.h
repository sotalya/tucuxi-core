/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef APOSTERIORIETASCALCULATOR_H
#define APOSTERIORIETASCALCULATOR_H

#include "dosage.h"
#include "parameter.h"
#include "definitions.h"
#include "residualerrormodel.h"
#include "sample.h"

namespace Tucuxi {
namespace Core {


class APosterioriEtasCalculator
{
public:

    enum class ComputationResult { Success, Failure, Aborted };

    APosterioriEtasCalculator();


    ComputationResult computeAposterioriEtas(
            const IntakeSeries &_intakes,
            const ParameterSetSeries &_parameters,
            const OmegaMatrix &_omega,
            const ResidualErrorModel &_residualErrorModel,
            const SampleSeries &_samples,
            Etas &_aPosterioriEtas);
};

}
}

#endif // APOSTERIORIETASCALCULATOR_H
